#include <iostream>
#include <errno.h>
#include <fstream>
// Added by sherardliu 2013-08-06 10:00
#include <string.h>

#include "schedule_mng.h"
#include "w_shm_conf.h"

int32_t CScheduleManager::init(const char *mem_file, const char *proc_file)
{
    printf("chunk_num=%d, proc_num=%d, chunk_size=%d\n", _chunk_num, _proc_num, _chunk_size);
    key_t key = ftok(proc_file, 0);
    if (-1 == key)
        return key;

    //int shmid = shmget(key, sizeof(process_schedule_item)*_proc_num, IPC_CREAT);
    //int shmid = shmget(key, sizeof(process_schedule_item) * _proc_num, IPC_CREAT | 0600 | IPC_EXCL);
    if (!isProExist())
    {
        int shmid = shmget(IPC_PRIVATE, sizeof(process_schedule_item) * _proc_num, IPC_CREAT | 0666 | IPC_EXCL);
        _proc_shmid = shmid;
        cout << "proc_shmid:" << _proc_shmid  << " errno=" << errno<< endl;
        _proc_schedule_table = (p_process_schedule_item)shmat(shmid, NULL, 0);
        writeProShmid();
    }
    key = ftok(mem_file, 0);
    if (-1 == key)
        return key;
    //shmid = shmget(key, sizeof(mem_schedule_item)*_chunk_num, IPC_CREAT);
    if (!isMemExist())
    {
        int shmid = shmget(IPC_PRIVATE , sizeof(mem_schedule_item)*_chunk_num, IPC_CREAT| 0600);
        _mem_shmid = shmid;
        struct shmid_ds ds;
        shmctl(shmid, IPC_STAT, &ds);
        _mem_schedule_table = (p_mem_schedule_item)shmat(shmid, NULL, 0);
    
        if (ds.shm_nattch == 0)
        {
            for (int32_t i=0; i<_chunk_num; i++)
            {
                _mem_schedule_table[i].shm_seq = i;
                _mem_schedule_table[i].state = FREE;
                _mem_schedule_table[i].data_state = DATA_WAITING;
                _mem_schedule_table[i].size = _chunk_size;
            }
        }
        writeMemShmid();
    }
    _proc_lock = sem_open(proc_file, O_CREAT | O_RDWR, 0644, 1);
    _mem_lock = sem_open(mem_file, O_CREAT | O_RDWR, 0644, 1);
    if (NULL == _proc_lock || NULL == _mem_lock)
    {
        return -1;
    }

    return 0;
}

bool CScheduleManager::writeMemShmid()
{
    using std::ofstream;
    ofstream fout;
    fout.open(__mem_file);
    if(fout.is_open())
    {
        fout << _mem_shmid;
    }
    else
    {
        return false;
    }
    fout.close();
    return true;
}

bool CScheduleManager::writeProShmid()
{
    using std::ofstream;
    ofstream fout;
    fout.open(__proc_file);
    if(fout.is_open())
    {
        fout << _proc_shmid;
    }
    else
    {
        return false;
    }
    fout.close();
    return true;
}
bool CScheduleManager::isMemExist()
{
    using std::ifstream;
    ifstream fin;
    fin.open(__mem_file);
    if (fin.is_open())
    {
        fin >> _mem_shmid ;
    }
    else
    {
        return false;
    }
    fin.close();
    _mem_schedule_table = (p_mem_schedule_item)shmat(_mem_shmid, NULL, 0);
    if (_mem_schedule_table == NULL)
    {
        cerr << "_mem_schdule_table shmat failed.";
        return false;
    }
    return true;
}

bool CScheduleManager::isProExist()
{
    using std::ifstream;
    ifstream fin;
    fin.open(__proc_file);
    if (fin.is_open())
    {
        fin >> _proc_shmid ;
    }
    else
    {
        return false;
    }
    fin.close();
    _proc_schedule_table = (p_process_schedule_item)shmat(_proc_shmid, NULL, 0);
    if (_proc_schedule_table == NULL)
    {
        cerr << "_mem_schdule_table shmat failed.";
        return false;
    }
    return true;
}
int32_t CScheduleManager::register_proc(pid_t pid, const char *pipe, unsigned int &seq)
{
    struct shmid_ds ds;
    int ret = shmctl(_proc_shmid, IPC_STAT, &ds);
    cout <<"ret=" << ret << " errno=" << errno
        <<" proc_shmid:"<< _proc_shmid <<" shm_nattch:" << ds.shm_nattch << endl;
    _real_proc_num = ds.shm_nattch - 1;
    cout << "_real_proc_num=" << _real_proc_num << endl;
    if (_real_proc_num > _proc_num)
        return -1;

    sem_wait(_proc_lock);
    _proc_schedule_table[_real_proc_num].pid = pid;
    _proc_schedule_table[_real_proc_num].state = FREE;
    _proc_schedule_table[_real_proc_num].seq = _real_proc_num;
    strcpy(_proc_schedule_table[_real_proc_num].pipe, pipe);
    sem_post(_proc_lock);
    seq = _real_proc_num++;
    return 0;
}

int32_t CScheduleManager::get_free_proc(unsigned int &seq, pid_t &pid, char* pipe)
{
    sem_wait(_proc_lock);
    int i = 0;
    for (; i<_proc_num; i++)
    {
        if (FREE == _proc_schedule_table[i].state)
        {
            seq = i;
            pid = _proc_schedule_table[i].pid;
            strcpy(pipe, _proc_schedule_table[i].pipe);
            break;
        }
    }
    sem_post(_proc_lock);
    if ( i == _proc_num )
        return -1;
    return 0;
}

int32_t CScheduleManager::get_free_proc_all(vector<process_schedule_item> &free_proc_list)
{
    sem_wait(_proc_lock);
    int i = 0;
    for (; i<_proc_num; i++)
    {
        if (FREE == _proc_schedule_table[i].state)
            free_proc_list.push_back(_proc_schedule_table[i]);
    }
    sem_post(_proc_lock);

    return 0;
}

int32_t CScheduleManager::get_free_mem(int32_t &shm_seq)
{
    sem_wait(_mem_lock);
    int i = 0;
    for (; i<_chunk_num; i++)
    {
        if (FREE == _mem_schedule_table[i].state && DATA_WAITING == _mem_schedule_table[i].data_state)
        {
            shm_seq = i;
            break;
        }
    }
    sem_post(_mem_lock);
    if ( i == _chunk_num)
        return -1;
    return 0;
}

int32_t CScheduleManager::get_free_mem_all(vector<int32_t> &free_mem_list)
{
    sem_wait(_mem_lock);
    int i = 0;
    for (; i<_chunk_num; i++)
    {
        if (FREE == _mem_schedule_table[i].state && DATA_WAITING == _mem_schedule_table[i].data_state)
            free_mem_list.push_back(i);
    }
    sem_post(_mem_lock);
    return 0;
}


int32_t CScheduleManager::get_data_ready_mem(int32_t &shm_seq)
{
    sem_wait(_mem_lock);
    int  i = 0;
    for (; i<_chunk_num; i++)
    {
        if (FREE == _mem_schedule_table[i].state &&  DATA_READY == _mem_schedule_table[i].data_state)
        {
            shm_seq = i;
            break;
        }
    }
    sem_post(_mem_lock);

    if (i == _chunk_num)
        return -1;
    return 0;
}

int32_t CScheduleManager::set_proc(unsigned int seq, STATE state)
{
    if (seq >= _proc_num)
        return -1;
    sem_wait(_proc_lock);
    _proc_schedule_table[seq].state = state;
    sem_post(_proc_lock);
    return 0;
}

int32_t CScheduleManager::set_mem_run_state(unsigned int seq, STATE state)
{
    if (seq >= _chunk_num)
        return -1;

    sem_wait(_mem_lock);
    _mem_schedule_table[seq].state = state;
    sem_post(_mem_lock);
    return 0;
}

int32_t CScheduleManager::set_mem_data_state(unsigned int seq, DATA_STATE data_state)
{
    if (seq >= _chunk_num)
        return -1;
    sem_wait(_mem_lock);
    _mem_schedule_table[seq].data_state = data_state;
    sem_post(_mem_lock);
    return 0;
}

int32_t CScheduleManager::dump_proc_table(vector<process_schedule_item> &proc_table)
{
    sem_wait(_proc_lock);
    int i = 0;
    for (; i<_real_proc_num; i++)
        proc_table.push_back(_proc_schedule_table[i]);
    sem_post(_proc_lock);
    i = 0;
    vector<process_schedule_item>::iterator it = proc_table.begin();
    for (; it != proc_table.end(); it++)
        printf("seq:%d\tpid:%d\tstate:%d\tpipe:%s\n", it->seq, it->pid, it->state, it->pipe);
    return 0;
}


int32_t CScheduleManager::dump_mem_table(vector<mem_schedule_item> &mem_table)
{
    sem_wait(_mem_lock);
    int i = 0;
    for (; i<_chunk_num; i++)
        mem_table.push_back(_mem_schedule_table[i]);
    sem_post(_mem_lock);

    i = 0;
    vector<mem_schedule_item>::iterator it = mem_table.begin();
    for (; it != mem_table.end(); it++)
        printf("shm_seq:%d\tstate:%d\tdata_state:%d\tsize:%d\n", it->shm_seq, it->state, it->data_state, it->size);
    return 0;
}
