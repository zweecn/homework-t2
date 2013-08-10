#ifndef __CSCUEDULER_MAMAGER_H_
#define __CSCUEDULER_MAMAGER_H_
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string>
#include <semaphore.h>
#include <fcntl.h>
#include <vector>
#include <cstdlib>

using namespace std;


enum STATE
{
    FREE = 0x00,
    BUSY = 0x01
};

enum DATA_STATE
{
    DATA_READY = 0x05,
    DATA_WAITING = 0x06
};

typedef struct _process_schedule_item{
    int32_t seq;
    pid_t pid;
    STATE state;
    char pipe[1024];
} process_schedule_item, *p_process_schedule_item;


typedef struct _mem_schedule_item{
    int32_t shm_seq ;
    STATE state;
    DATA_STATE data_state;
    unsigned int size;
} mem_schedule_item, *p_mem_schedule_item;


class CScheduleManager
{
public:
//    CScheduleManager(){}
    CScheduleManager(unsigned int proc_num, unsigned int chunk_num, unsigned int chunk_size):
        _proc_num(proc_num), _real_proc_num(0), _chunk_num(chunk_num), _chunk_size(chunk_size) {}
    ~CScheduleManager()
    {
        shmdt((void *)_mem_schedule_table);
        shmdt((void *)_proc_schedule_table);

        struct shmid_ds proc_ds;
        struct shmid_ds mem_ds;
        shmctl(_proc_shmid, IPC_STAT, &proc_ds);
        shmctl(_mem_shmid, IPC_STAT, &mem_ds);

        printf("proc nattch=%d\n", proc_ds.shm_nattch);
        printf("mem nattch=%d\n", mem_ds.shm_nattch);
        if (0 == proc_ds.shm_nattch)
        {
            int result = shmctl(_proc_shmid, IPC_RMID, NULL);
            if (0 == result)
                printf("success in removing proc_shm\n");
        }
        if (0 == mem_ds.shm_nattch)
        {
            int result = shmctl(_mem_shmid, IPC_RMID, NULL);
            if (0 == result)
                printf("success in removing mem_shm\n");
        }
    }


    int32_t init(const char *mem_file, const char *proc_file);
    int32_t register_proc(pid_t pid, const char *pipe, unsigned int &seq);

    int32_t get_free_proc(unsigned int &seq, pid_t &pid, char* pipe);
    int32_t get_free_proc_all(vector<process_schedule_item> &free_proc_list);
    int32_t get_free_mem(int32_t &shm_seq);
    int32_t get_free_mem_all(vector<int32_t> &free_mem_list);
    int32_t get_data_ready_mem(int32_t &shm_seq);

    int32_t set_proc(unsigned int seq, STATE state);
    int32_t set_mem_run_state(unsigned int seq, STATE state);
    int32_t set_mem_data_state(unsigned int seq, DATA_STATE data_state);

    int32_t dump_proc_table(vector<process_schedule_item> &proc_table);
    int32_t dump_mem_table(vector<mem_schedule_item> &mem_table);

    bool isMemExist();
    bool isProExist();

    bool writeMemShmid();
    bool writeProShmid();

private:
    sem_t *_proc_lock;
    sem_t *_mem_lock;
    unsigned int _proc_num;
    unsigned int _real_proc_num;
    unsigned int _chunk_num;
    unsigned int _chunk_size;
    int _proc_shmid;
    int _mem_shmid;
    p_mem_schedule_item _mem_schedule_table;
    p_process_schedule_item _proc_schedule_table;
};

#endif
