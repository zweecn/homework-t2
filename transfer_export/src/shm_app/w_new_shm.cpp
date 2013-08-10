#include <iostream>

#include "w_shm_conf.h"
#include "w_shm_ctl.h"
#include "w_msg.h"
#include "w_datarecv.h"
#include "w_fifo_ctl.h"
#include "w_shm_table.h"
#include "w_sem_table.h"
#include "w_route_ctl.h"

bool NewShm()
{
    W_ShmCtl w_shmctl;
    cout << W_Msg::CurDate() << "Make sure you are the first to new shm. " 
           << "file:" << __FILE__ << " line:"<< __LINE__  << endl;

    // 1. CreateShmKey
    bool flag = w_shmctl.CreateShmkey();
    if (!flag)
    {
        cerr << W_Msg::CurDate() << "Create shmkey failed. file:" 
            << __FILE__ << " line:"<< __LINE__  << endl;
        return false;
    }
    // 2. CreateShm
    flag = w_shmctl.CreateShm();
    if (!flag)
    {
        cerr << W_Msg::CurDate() << "Create shm failed. file:" 
            << __FILE__ << " line:"<< __LINE__ << endl;
        return false;
    }
    
    return true;
}

bool NewSem()
{
    bool ret = true;

    W_ShmTable<W_ShmPidTableItem> pidShmTable; 
    W_SemTable pidSemTable;

    W_ShmTable<W_ShmTableItem> memShmTable; 
    W_SemTable memSemTable;

    pidSemTable.Init(__sem_pid_table_conf, __process_size);
    pidShmTable.Init(__shm_pid_table_conf, __process_size);

    if (!pidShmTable.ShmWriteInit())
    {
        cerr << W_Msg::CurDate() << "NewSem init failed"
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false;
    }
    if (!pidSemTable.WriteInit())
    {
        cerr << W_Msg::CurDate() << "NewSem init failed"
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false;
    }

    memSemTable.Init(__sem_mem_table_conf, __shm_block_size);
    memShmTable.Init(__shm_mem_table_conf, __shm_block_size);
    
    if (!memShmTable.ShmWriteInit())
    {
        cerr << W_Msg::CurDate() << "NewSem init failed"
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false;
    }
    if (!memSemTable.WriteInit())
    {
        cerr << W_Msg::CurDate() << "NewSem init failed"
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false;
    }
    
    return true;
}

bool NewSem2()
{
    bool ret = true;

    W_SemTable semtable(__sem_table_conf, __shm_block_size);

    if (!semtable.WriteInit())
    {
        cerr << W_Msg::CurDate() << "NewSem init failed"
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false;
    }
    
    return true;
}
int main()
{
    if (NewShm() && NewSem2())
    {
        cerr << W_Msg::CurDate() << "w_new_mem main succeed."
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return 0;
    }
    cerr << W_Msg::CurDate() << "w_new_mem main failed."
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
    return 1;
}

