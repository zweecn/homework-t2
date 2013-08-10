#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>

#include "w_shm_conf.h"
#include "w_shm_ctl.h"
#include "w_msg.h"
#include "w_datarecv.h"
#include "w_fifo_ctl.h"
#include "w_shm_table.h"
#include "w_sem_table.h"
#include "w_shm_conf.h"

using std::cout;
using std::endl;

int main()
{
//    W_DataRecv dr;
//    dr.ReadTest();
   	 
    W_DataRecv dr;
	int pid = fork();
//    for (int i = 0; i < __process_size - 2; i++) 
//    {
//        if (pid > 0) 
//        {
//            pid = fork();
//        }
//    }
	if (pid == 0)
	{
		dr.id = 0;
	}
	else
	{
		dr.id = 1;
	}
    dr.ReadFileTest();

//    dr.RegisterProc();

//    char str[1024];
//    memset(str, 0, sizeof(str));
//    sprintf(str, "conf/fifo_%d", getpid());
//    cout << "str=" << str << endl;
//    unsigned int seq = -1;
//    int ret = dr.GetCScheduleMamager()->register_proc(getpid(), str, seq);
//
//    cout << "main ret=" << ret << endl;
//    
//    std::cout << "main seq=" << seq << endl;
//    dr.SetSeq(seq);

//    W_FifoCtl fc;
//    while (true)
//    {
//        fc.FifoRead("conf/fifo");
//    }

//    W_ShmTable<W_ShmTableItem> st(__shm_mem_table_conf, __shm_block_size);
//    W_ShmTableItem item;
//    st.ReadTest(item);

    
//    W_SemTable st(__sem_mem_table_conf, __shm_block_size);
    return 0 ;
}
