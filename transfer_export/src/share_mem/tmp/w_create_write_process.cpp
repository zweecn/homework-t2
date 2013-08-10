#include <iostream>

#include "w_shm_conf.h"
#include "w_shm_ctl.h"
#include "w_msg.h"
#include "w_datarecv.h"
#include "w_fifo_ctl.h"
#include "w_shm_table.h"
#include "w_sem_table.h"
#include "w_route_ctl.h"


int main()
{
//    W_DataRecv dr;
//    dr.WriteTest();

    W_DataRecv dr;
    dr.WriteFileTest();

    //char buf[10] = "hello";
    //dr.WriteData(buf, 10);

//    W_FifoCtl fc;
//    for (int i = 1; i < 10; i++)
//        fc.FifoWrite("conf/fifo", i);
    
//    W_ShmTable<W_ShmTableItem> st(__shm_mem_table_conf, __shm_block_size);
//    W_ShmTableItem item;
//    st.WriteTest(item);

//    W_SemTable st(__sem_mem_table_conf, __shm_block_size);
//    st.RunTest();

//    W_RouteCtl rc;
//    rc.Init();
//    
//    rc.InitPidTable();
//    rc.InitMemTable();
//
//    cout << rc.GetFreePid().pid << endl;
//    cout << rc.GetFreeMem().shmIndex << endl;
    return 0 ;
}
