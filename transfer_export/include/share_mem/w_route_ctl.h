#ifndef __W_ROUTE_H_
#define __W_ROUTE_H_

#include "w_shm_table.h"
#include "w_sem_table.h"

class W_RouteCtl
{
public:
    explicit W_RouteCtl();

    bool Init();

    bool InitMemTable();
    bool InitPidTable();

    W_ShmPidTableItem GetFreePid();
    W_ShmTableItem GetFreeMem();
    W_ShmTableItem GetDataReadyMem();

    bool SetPidItem(const W_ShmPidTableItem & item, int index);
    bool SetMemItem(const W_ShmTableItem & item, int index); 

    
    bool AddPidItem(int pid);

    bool ResetSem();

    bool Print();
private:

    W_ShmTable<W_ShmPidTableItem> pidShmTable; 
    W_SemTable pidSemTable;

    W_ShmTable<W_ShmTableItem> memShmTable; 
    W_SemTable memSemTable;

};


#endif 


