#ifndef __W_SHM_CTL_H_
#define __W_SHM_CTL_H_

#include <cstdio>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

/* When write:
 * 1. CreateShmkey
 * 2. CreateShm
 * 3. AttachMem
 * 4. Write
 * 
 * When Read:
 * 1. ReadShmid
 * 2. AttachMem
 * 3. Read
 */

class W_ShmCtl
{
public:
    explicit W_ShmCtl();
   
    bool Write(const char* buf, int len, int index);
    bool Read(char* buf, int len, int index);

    bool CreateShm();
    bool AttachMem();
    bool Destory();

    bool CreateShmkey();
    bool WriteShmid();
    bool ReadShmid();

    bool HasShm();

//    bool LoadConfig();

    int GetPerBlockSize();
    int GetBlockSize();

private:
//    int mem_size; 
//    int block_size;

    int shmkey;
    int pid ;
    int shmid;
    char *shmaddr ;
    struct shmid_ds shmStat ;
};


#endif
