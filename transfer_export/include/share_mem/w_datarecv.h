#ifndef __W_DATA_RECV_H_
#define __W_DATA_RECV_H_

#include "w_shm_ctl.h"
#include "w_fifo_ctl.h"
#include "w_route_ctl.h"
#include "schedule_mng.h"
#include "w_shm_conf.h"

class W_DataRecv
{
public:
    explicit W_DataRecv();
   
    bool ShmReadInit();

    bool WriteData(const char* buf, int len); //[1] Invoke it to save data to shm
    bool ReadData(char* buf, int len);

    bool WriteTest();
    bool ReadTest();

    bool WriteFileTest();
    bool ReadFileTest();

    const char* GetFifoName();

	int id;
private:
    W_ShmCtl w_shmctl;
    W_FifoCtl w_fifoctl;
    //W_RouteCtl w_routectl;

    int fd_w;
    int fd_r;
    
    unsigned int seq;
    char fifoname[1024];

    W_SemTable w_semtable;
    
    int index;

};


#endif
