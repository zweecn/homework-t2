#ifndef __PENDING_FILE
#define __PENDING_FILE

#include <stdint.h>
#include <pthread.h>

#include "FileID.h"
#include "list.h"

struct PendingFile {
    int opened_fd;
    int64_t file_size;
    int64_t dl_size;
    FileID file_id;
    pthread_mutex_t flock;
    pthread_cond_t fsize_cond;
    struct hlist_node list;

    PendingFile(const FileID &id);
    ~PendingFile();
};

extern int InitHashTable(void);
extern int DeinitHashTable(void);
extern int SS_SetPendingFileSize(const FileID &id, int64_t fsize);
extern int SS_WriteData2PendingFile(const FileID &id, const char *buffer,
                             uint64_t offset, int len);
extern int SS_ClosePendingFile(const FileID &id);

#endif

