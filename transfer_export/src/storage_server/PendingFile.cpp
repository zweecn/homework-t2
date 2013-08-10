#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "FileID.h"
#include "PendingFile.h"


#define HASH_TABLE_MASK 0xFF
struct hlist_head HTable[HASH_TABLE_MASK+1];
static pthread_rwlock_t ht_lock = PTHREAD_RWLOCK_INITIALIZER;

PendingFile::PendingFile(const FileID &id)
{
    opened_fd = -1;
    file_size = -1;
    dl_size = 0;
    file_id = id;   //��Ҫ���ظ�ֵ�������
    //flock = PTHREAD_MUTEX_INITIALIZER;
    //fsize_cond = PTHREAD_COND_INITIALIZER;
    pthread_mutex_init(&flock, NULL);
    pthread_cond_init(&fsize_cond, NULL);
    INIT_HLIST_NODE(&list);
}

PendingFile::~PendingFile()
{
    pthread_mutex_destroy(&flock);
    pthread_cond_destroy(&fsize_cond);
}

int InitHashTable(void)
{
    for (int i=0; i<HASH_TABLE_MASK+1; i++) {
        INIT_HLIST_HEAD(&HTable[i]);
    }

    return 0;
}

int DeinitHashTable(void)
{
    struct hlist_head *head = NULL;
    struct hlist_node *pos=NULL, *n=NULL;
    struct PendingFile *target = NULL;
    //PendingFile *target = NULL;

    for (int i=0; i<HASH_TABLE_MASK+1; i++) {
        head = &HTable[i];
        if (!hlist_empty(head)) {
            fprintf(stderr, "PendingFile hash table has not been destroyed\n");
            hlist_for_each_entry_safe(target, pos, n, head, list) {
                hlist_del(pos);
                if (target->opened_fd >= 0)
                    close(target->opened_fd);
                delete target;
            }
        }
    }

    return 0;
}

static struct PendingFile * FindPendingFileByHash(const FileID &id)
//PendingFile * FindPendingFileByHash(const FileID &id)
{
    uint32_t hash = id.Hash2Uint32();
    uint32_t index = hash & HASH_TABLE_MASK;
    struct hlist_head *head = &HTable[index];
    struct PendingFile *target = NULL;
    //PendingFile *target = NULL;
    struct hlist_node *pos = NULL;

    pthread_rwlock_rdlock(&ht_lock);
    hlist_for_each_entry(target, pos, head, list) {
        if (target->file_id == id) {
            pthread_rwlock_unlock(&ht_lock);
            return target;
        }
    }
    pthread_rwlock_unlock(&ht_lock);

    return NULL;
}

/* �����ļ�����Hash��
 * ����ʱ��Ҫ��д�������¼�飬�����ڣ���ɾ����ǰ����
 * ���ش�����Hash���еĶ��� */
static struct PendingFile * AddPendingFile2Hash(struct PendingFile *pf)
//PendingFile * AddPendingFile2Hash(struct PendingFile *pf)
{
    uint32_t hash = pf->file_id.Hash2Uint32();
    uint32_t index = hash & HASH_TABLE_MASK;
    struct hlist_head *head = &HTable[index];
    struct PendingFile *result=NULL, *tmp=NULL;
    //PendingFile *result=NULL, *tmp=NULL;
    struct hlist_node *pos = NULL;

    pthread_rwlock_wrlock(&ht_lock);
    hlist_for_each_entry(tmp, pos, head, list) {
        if (tmp->file_id == pf->file_id) {
            result = tmp;
            break;
        }
    }
    if (result) {
        delete pf;
    } else {
        hlist_add_head(&pf->list, head);
        result = pf;
    }
    pthread_rwlock_unlock(&ht_lock);

    return result;
}

static int DelPendingFileFromHash(struct PendingFile *pf)
//int DelPendingFileFromHash(PendingFile *pf)
{
    pthread_rwlock_wrlock(&ht_lock);
    hlist_del(&pf->list);
    pthread_rwlock_unlock(&ht_lock);
    return 0;
}

/* ����id�ҵ���Ӧ���ļ�����Ҫʱ���� */
static struct PendingFile * GetPendingFile(const FileID &id)
//PendingFile * GetPendingFile(const FileID &id)
{
    struct PendingFile *pf = NULL;
    //PendingFile *pf = NULL;

    pf = FindPendingFileByHash(id);
    if (!pf) {
        pf = new PendingFile(id);
        pf = AddPendingFile2Hash(pf);
    }

    return pf;
}

/* ����id�ҵ��ļ��������ļ���С���������� */
int SS_SetPendingFileSize(const FileID &id, int64_t fsize)
{
    struct PendingFile *fp = NULL;
    //PendingFile *fp = NULL;

    fp = GetPendingFile(id);

    pthread_mutex_lock(&fp->flock);
    fp->file_size = fsize;
    pthread_cond_signal(&fp->fsize_cond);
    pthread_mutex_unlock(&fp->flock);

    return 0;
}

/* ����id�ҵ��ļ�������Ӧ��λ��д������ */
int SS_WriteData2PendingFile(const FileID &id,
                             const char *buffer,
                             uint64_t offset,
                             int len)
{
    struct PendingFile *fp = NULL;
    //PendingFile *fp = NULL;

    fp = GetPendingFile(id);
    pthread_mutex_lock(&fp->flock);
    if (0 > fp->opened_fd) {
        char *file_name = id.GenFileName();
        if (0 == access(file_name, F_OK)) {
            //�ݲ������ļ��ظ�д������
        }
        fp->opened_fd = open(file_name, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR|S_IROTH);
        delete [] file_name;
    }
    pthread_mutex_unlock(&fp->flock);

    /* ����д��ͬ��λ�ã����ü��� */
    lseek(fp->opened_fd, offset, SEEK_SET);
    write(fp->opened_fd, buffer, len);

    pthread_mutex_lock(&fp->flock);
    fp->dl_size += len;
    pthread_cond_signal(&fp->fsize_cond);
    pthread_mutex_unlock(&fp->flock);
    return 0;
}

/* ����id�ҵ��ļ����������ϵȴ����ر� */
int SS_ClosePendingFile(const FileID &id)
{
    struct PendingFile *fp = NULL;
    //PendingFile *fp = NULL;

    fp = GetPendingFile(id);

    pthread_mutex_lock(&fp->flock);
    while (fp->file_size != fp->dl_size)
        pthread_cond_wait(&fp->fsize_cond, &fp->flock);
    close(fp->opened_fd);
    fp->opened_fd = -1;
    DelPendingFileFromHash(fp);
    pthread_mutex_unlock(&fp->flock);

    delete fp;

    return 0;
}

