#include <iostream>
#include <fstream>
#include <ctime>

// Added by sherardliu 2013-08-06 10:18
#include <string.h>

#include "w_shm_ctl.h"
#include "w_msg.h"
#include "w_shm_conf.h"

using std::cerr;
using std::cout;
using std::endl;

//const char* shmid_conf = "conf/shmid.conf";
//const char* mem_block_conf = "conf/mem_block.onf";

W_ShmCtl::W_ShmCtl()
{
    shmid = shmkey = 0;
    pid = getpid();
//    LoadConfig();
}

bool W_ShmCtl::CreateShm()
{
    cout << W_Msg::CurDate() << "Pid:" << pid << endl; 

    //shmid = shmget(IPC_PRIVATE, __mem_size, IPC_CREAT|0600 ) ;
    shmid = shmget(shmkey, __mem_size, IPC_CREAT|0600 ) ;
    if ( shmid < 0 )
    {
        cerr << W_Msg::CurDate() << "shmget ipc_id error" 
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false ;
    }
    
    if (!WriteShmid())
    {
        cerr << W_Msg::CurDate() << "shmkey write failed. shmkey=" << shmkey 
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false;
    }   

    return true;
}

bool W_ShmCtl::AttachMem()
{
    if (shmid == -1)
    {
        cerr << W_Msg::CurDate() << "No shmget mem. Attach shmid=-1" 
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false;
    }
    int flag = shmctl( shmid, IPC_STAT, &shmStat) ;
    if ( flag == -1 )
    {
        cerr << W_Msg::CurDate() << "Attach shmctl shm error" 
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false ;
    }
    cout << W_Msg::CurDate() << "Attach shmid=" << shmid << endl;
    cout << W_Msg::CurDate() << "shm_segsz=" << shmStat.shm_segsz << " bytes" <<  endl;
    cout << W_Msg::CurDate() << "shm_cpid=" << shmStat.shm_cpid << endl;
    cout << W_Msg::CurDate() << "shm_lpid=" << shmStat.shm_lpid << endl;

    shmaddr = (char *)shmat( shmid, NULL, 0 ) ;
    if ( (long)shmaddr == -1 )
    {
        cerr << W_Msg::CurDate() << "shmat addr error" 
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false;
    }
    cout << W_Msg::CurDate() << "Attach shmaddr:" << (long)shmaddr << endl;

    return true;
}

bool W_ShmCtl::Destory()
{
    if (shmid == -1)
    {
        cerr << W_Msg::CurDate() << "No shmget mem. Destory shmid=-1" 
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false;
    }
    int flag = shmdt( shmaddr ) ;
    if (flag == -1 )
    {
        cerr << W_Msg::CurDate() << "Destory shmdt error" 
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false;
    }
    flag = shmctl(shmid, IPC_RMID, NULL) ;
    if ( flag == -1 )
    {
        cerr << W_Msg::CurDate() << "shmctl IPC_RMID error" 
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false ;
    }
    return true;
}

bool W_ShmCtl::CreateShmkey()
{
    if (!ReadShmid())
    {
        shmkey = time(NULL); 
        cout << W_Msg::CurDate() << "shmkey does not exist. Create it. shmkey=" << shmkey 
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
    }
    
    cout << W_Msg::CurDate() << "shmkey create succeed. shmkey=" << shmkey 
        << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
    return true;
}

bool W_ShmCtl::WriteShmid()
{
    std::ofstream fout;
    fout.open(__shmid_conf);
    if (fout.is_open())
    {
        fout << "shmid " << shmid << endl 
            << "shmkey " << shmkey << endl;
    }
    else
    {
        cerr << W_Msg::CurDate() << __shmid_conf << " open failed" << endl;
        return false;
    }
    fout.close();
    return true;
}

bool W_ShmCtl::ReadShmid()
{
    shmid = shmkey = 0;
    std::ifstream fin;
    fin.open(__shmid_conf);
    if (fin.is_open())
    {
//        fin >> shmid;
        std::string name;
        int var;
        while (fin >> name >> var)
        {
            if (name == "shmid")
                shmid = var;
            if (name == "shmkey")
                shmkey = var;
        }
    }
    else
    {
        cerr << W_Msg::CurDate() << __shmid_conf << " open failed" << endl;
        return false;
    }
    fin.close();

    if (shmid != 0 && shmkey !=0)
        return true;
    return false;
}

bool W_ShmCtl::Write(const char* buf, int len, int index)
{
    if (shmid == -1)
    {
        cerr << W_Msg::CurDate() << "No shmget mem. Write shmid=-1" 
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false;
    }
    if (index > __shm_block_size)
    {
        cerr << W_Msg::CurDate() << "Write index out of range" 
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false;
    }
    if (len > __mem_size / __shm_block_size)
    {
        cerr << W_Msg::CurDate() << "Write shm len out of range" 
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false;
    }

    char *addr = shmaddr + index * GetPerBlockSize();
    cout << W_Msg::CurDate() << "Write addr is: " << (long) addr << " Begin memcpy..."
        << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
    memcpy(addr, buf, len);
    return true;
}

bool W_ShmCtl::Read(char* buf, int len, int index)
{
    if (shmid == -1)
    {
        cerr << W_Msg::CurDate() << "No shmget mem. Read shmid=-1" 
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false;
    }
    if (index > __shm_block_size)
    {
        cerr << W_Msg::CurDate() << "Read index out of range" 
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false;
    }
    if (len != GetPerBlockSize())
    {
        cerr << W_Msg::CurDate() << "Read dest buf len out of range" 
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false;
    }

    char *addr = shmaddr + index * GetPerBlockSize();
    cout << W_Msg::CurDate() << "Read addr is: " << (long) addr << " Begin memcpy..." 
        << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
    memcpy(buf, addr, len);
    return true;
}

bool W_ShmCtl::HasShm()
{
    
    cout << W_Msg::CurDate() << "HasShm begin...shmkey=" << shmkey 
        << " shmid=" << shmid << endl;
    return (shmkey != 0 && shmid != 0);

    return true;
}

//bool W_ShmCtl::LoadConfig()
//{
//    std::ifstream fin;
//    fin.open(mem_block_conf);
//    int cnt = 0;
//    if (fin.is_open())
//    {
//        std::string name;
//        int val = 0;
//        while (fin >> name >> val)
//        {
//            if (name == "__mem_size") 
//            {
//                __mem_size = val;
//                cnt ++;
//            } 
//            else if (name == "__shm_block_size")
//            {
//                __shm_block_size = val;
//                cnt ++;
//            }
//        }
//    }
//    else
//    {
//        cerr << W_Msg::CurDate() << mem_block_conf << " open failed" 
//            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
//        return false;
//    }
//    fin.close();
//    if (cnt != 2)
//    {
//        cerr << W_Msg::CurDate() << mem_block_conf 
//            << " config error. Format: __mem_size 1000 \\n __shm_block_size 10"
//            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
//        return false;
//    }
//    return true;
//}

int W_ShmCtl::GetPerBlockSize()
{
    return __mem_size / __shm_block_size;
}

int W_ShmCtl::GetBlockSize()
{
    return __shm_block_size;
}

