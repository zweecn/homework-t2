#include <vector>

// Added by sherardliu 2013-08-06 10:18
#include <string.h>

#include "w_route_ctl.h"
#include "w_msg.h"
#include "w_shm_conf.h"

using std::cout;
using std::cerr;
using std::endl;

W_RouteCtl::W_RouteCtl()
{
    Init(); 
}

bool W_RouteCtl::Init()
{
    bool ret = true;

    pidSemTable.Init(__sem_pid_table_conf, __process_size);
    pidShmTable.Init(__shm_pid_table_conf, __process_size);
    if (!pidShmTable.ShmReadInit())
    {
        cerr << W_Msg::CurDate() << "W_RouteCtl pidShmTable.ShmReadInit failed"
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false;
    }
    if (!pidSemTable.ReadInit())
    {
        cerr << W_Msg::CurDate() << "W_RouteCtl pidSemTable.ReadInit failed"
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false;
    }
//    ret &= pidShmTable.ShmWriteInit();
//    ret &= pidSemTable.WriteInit();

    memSemTable.Init(__sem_mem_table_conf, __shm_block_size);
    memShmTable.Init(__shm_mem_table_conf, __shm_block_size);
    if (!memShmTable.ShmReadInit())
    {
        cerr << W_Msg::CurDate() << "W_RouteCtl memShmTable.ShmReadInit failed"
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
    }
    if (!memSemTable.ReadInit())
    {
        cerr << W_Msg::CurDate() << "W_RouteCtl memSemTable.ReadInit failed"
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false;
    }

//    ret &= memShmTable.ShmWriteInit();
//    ret &= memSemTable.WriteInit();
//   
//    if (!ret)
//    {
//        cerr << W_Msg::CurDate() << "W_RouteCtl init failed"
//            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
//    }
//    else
//    {
//        cerr << W_Msg::CurDate() << "W_RouteCtl init succeed."
//            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
//    }
    return ret;
}

bool W_RouteCtl::InitMemTable()
{
    W_ShmTableItem item;
    for (int i = 0; i < __shm_block_size; i++)
    {
        item.shmIndex = i;
        item.isMemFree = true;
        item.isDataReady = false;

        if (!memSemTable.P(i))
        {
            cerr << W_Msg::CurDate() << "lock failed."
                << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
            return false;
        }

        int ret = memShmTable.Write(&item, i);
        if (!memSemTable.V(i))
        {
            cerr << W_Msg::CurDate() << "unlock failed."
                << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
            return false;
        }
        if (!ret)
        {
            cerr << W_Msg::CurDate() << "InitMemTable failed."
                << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
            return false;
        }
    }
    return true;
}

bool W_RouteCtl::InitPidTable()
{
    W_ShmPidTableItem item;
    for (int i = 0; i < __process_size; i++)
    {
        item.pid = -1;
        item.isPidFree = false;
        memset(item.fifoPath, 0, sizeof(item.fifoPath));

        if (!pidSemTable.P(i))
        {
            cerr << W_Msg::CurDate() << "lock failed."
                << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
            return false;
        }
        int ret = pidShmTable.Write(&item, i);
        if (!pidSemTable.V(i))
        {
            cerr << W_Msg::CurDate() << "unlock failed."
                << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
            return false;
        }
        if (!ret)
        {
            cerr << W_Msg::CurDate() << "InitMemTable failed."
                << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
            return false;
        }
    }

    return true;
}


W_ShmPidTableItem W_RouteCtl::GetFreePid()
{
    W_ShmPidTableItem item;
    for (int i = 0; i < __process_size; i++)
    {
        pidSemTable.P(i); 
        pidShmTable.Read(&item, i);
        pidSemTable.V(i); 

        if (item.isPidFree)
        {
            return item;
        }
    }

    item.pid = -1;
    return item;
}

W_ShmTableItem W_RouteCtl::GetFreeMem()
{
    W_ShmTableItem item;
    for (int i = 0; i < __shm_block_size; i++)
    {
        memSemTable.P(i); 
        memShmTable.Read(&item, i);
        memSemTable.V(i); 

        if (item.isMemFree)
        {
            return item;
        }
    }

    item.shmIndex = -1;
    return item;
}

W_ShmTableItem W_RouteCtl::GetDataReadyMem()
{
    W_ShmTableItem item;
    for (int i = 0; i < __shm_block_size; i++)
    {
        memSemTable.P(i); 
        memShmTable.Read(&item, i);
        memSemTable.V(i); 

        if (item.isDataReady)
        {
            return item;
        }
    }

    item.shmIndex = -1;
    return item;
}

bool W_RouteCtl::SetPidItem(const W_ShmPidTableItem & item, int index)
{
    pidSemTable.P(index); 
    bool ret = pidShmTable.Write(&item, index);
    pidSemTable.V(index); 

    if (!ret)
    {
        cerr << W_Msg::CurDate() << "W_RouteCtl SetPidItem failed"
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
    }
    return ret; 
}

bool W_RouteCtl::SetMemItem(const W_ShmTableItem & item, int index)
{
    memSemTable.P(index); 
    bool ret = memShmTable.Write(&item, index);
    memSemTable.V(index); 
    
    if (!ret)
    {
        cerr << W_Msg::CurDate() << "W_RouteCtl SetMemItem failed"
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
    }
    return ret;
}

bool W_RouteCtl::AddPidItem(int pid)
{
    W_ShmPidTableItem item;
    item.pid = pid;
    item.isPidFree = true;
    
    memset(item.fifoPath, 0, sizeof(item.fifoPath));
    sprintf(item.fifoPath, "conf/fifo_%d", pid);

    for (int i = 0; i < __process_size; i++)
    {
        W_ShmPidTableItem tmp;
        pidSemTable.P(i); 
        int ret = pidShmTable.Read(&tmp, i);
        pidSemTable.V(i); 

        if (!ret)
        {
            cerr << W_Msg::CurDate() << "AddPidItem read failed."
                << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
            return false;
        }
        if (tmp.pid == -1)
        {
            ret = SetPidItem(item, i);
            if (!ret)
            {
                cerr << W_Msg::CurDate() << "AddPidItem SetPidItem failed."
                    << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
                return false;
            }   
            return true;
        }
    }

    return false;
}

bool W_RouteCtl::ResetSem()
{
    return pidSemTable.ResetSem() && memSemTable.ResetSem();
}

bool W_RouteCtl::Print()
{
    std::vector<W_ShmPidTableItem> itemlist;
    for (int i = 0; i < __process_size; i++)
    {
        W_ShmPidTableItem tmp;
        pidSemTable.P(i); 
        int ret = pidShmTable.Read(&tmp, i);
        pidSemTable.V(i); 

        if (!ret)
        {
            cerr << W_Msg::CurDate() << "AddPidItem read failed."
                << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
            return false;
        }
        
        itemlist.push_back(tmp);
    }
    cout << "Pid\tFree\tfifo" << endl;
    for (int i = 0; i < __process_size; i++)
    {
        cout << itemlist[i].pid << "\t" << itemlist[i].isPidFree << "\t" << itemlist[i].fifoPath << endl; 
    }
    return true;
}
