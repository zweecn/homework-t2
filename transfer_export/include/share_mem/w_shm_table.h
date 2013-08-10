#ifndef __W_SHM_TABLE_H_
#define __W_SHM_TABLE_H_
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdio>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "w_msg.h"

using std::cerr;
using std::cout;
using std::endl;


struct W_ShmTableItem
{
    int shmIndex;
    bool isMemFree;
    bool isDataReady; 
};

struct W_ShmPidTableItem
{
    int pid;
    bool isPidFree;
    char fifoPath[256];    
};

template <class T> class W_ShmTable
{
public:
    explicit W_ShmTable(const char* _shm_table_conf, int _blockSize);
    explicit W_ShmTable();
 
    bool Init(const char* _shm_table_conf, int _blockSize);

    bool ShmWriteInit();
    bool ShmReadInit();

    bool Write(const T* buf, int index);
    bool Read(T* buf, int index);

    bool CreateShm();
    bool AttachMem();
    bool Destory();

//    bool CreateShmkey();
    bool WriteShmid();
    bool ReadShmid();

//    bool HasShm();

    bool WriteTest(T &t);
    bool ReadTest(T &t);

private:

    int blockSize;
    const char* shm_table_conf;

//    int shmkey;
    int pid ;
    int shmid;
    char *shmaddr ;
    struct shmid_ds shmStat ;
};


template<class T> 
W_ShmTable<T>::W_ShmTable(const char* _shm_table_conf, int _blockSize)
{
    Init(_shm_table_conf, _blockSize);
}

template<class T> 
W_ShmTable<T>::W_ShmTable()
{

}

template<class T> 
bool W_ShmTable<T>::Init(const char* _shm_table_conf, int _blockSize)
{
    shm_table_conf = _shm_table_conf;
    blockSize = _blockSize;
    
    shmid = 0;
    pid = getpid();

    return true;
}

template<class T> 
bool W_ShmTable<T>::ShmWriteInit()
{
    cout << W_Msg::CurDate() << "Make sure it is the first time to ShmWriteInit. file:" 
            << __FILE__ << " line:"<< __LINE__  << endl;
    
    // 2. CreateShm
    bool flag = CreateShm();
    if (!flag)
    {
        cerr << W_Msg::CurDate() << "Create shm failed. file:" 
            << __FILE__ << " line:"<< __LINE__ << endl;
        return false;
    }
    // 3. AttachMem
    flag = AttachMem();
    if (!flag)
    {
        cerr << W_Msg::CurDate() << "AttachShm failed. file:" 
            << __FILE__ << " line:"<< __LINE__  << endl;
        return false;
    }
    cout << W_Msg::CurDate() << "Make sure it is the first time to ShmWriteInit. succeed. file:" 
            << __FILE__ << " line:"<< __LINE__  << endl;
    
    return true;
}

template<class T> 
bool W_ShmTable<T>::ShmReadInit()
{
    // 1. CreateShmKey
    bool flag = ReadShmid();
    if (!flag)
    {
        cerr << W_Msg::CurDate() << "Create shmkey failed. file:" 
            << __FILE__ << " line:"<< __LINE__  << endl;
        return false;
    }
    // 2. AttachMem
    flag = AttachMem();
    if (!flag)
    {
        cerr << W_Msg::CurDate() << "AttachShm failed. file:" 
            << __FILE__ << " line:"<< __LINE__  << endl;
        return false;
    }
    return true;
}

template <class T>  
bool W_ShmTable<T>::CreateShm()
{
    cout << W_Msg::CurDate() << "Pid:" << pid << endl; 

    shmid = shmget(IPC_PRIVATE, blockSize * sizeof(T), IPC_CREAT|0600 ) ;
    if ( shmid < 0 )
    {
        cerr << W_Msg::CurDate() << "shmget ipc_id error" 
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false ;
    }
    
    if (!WriteShmid())
    {
        cerr << W_Msg::CurDate() << "shmid write failed. shmid=" << shmid 
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false;
    }   

    return true;
}

template<class T> 
bool W_ShmTable<T>::AttachMem()
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

template<class T> 
bool W_ShmTable<T>::Destory()
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

//template<class T> 
//bool W_ShmTable<T>::CreateShmkey()
//{
//    if (!ReadShmid())
//    {
//        shmkey = time(NULL) + rand(); 
//        cout << W_Msg::CurDate() << "shmkey does not exist. Create it. shmkey=" << shmkey 
//            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
//    }
//    
//    cout << W_Msg::CurDate() << "shmkey create succeed. shmkey=" << shmkey 
//        << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
//    return true;
//}

template<class T> 
bool W_ShmTable<T>::WriteShmid()
{
    std::ofstream fout;
    fout.open(shm_table_conf);
    if (fout.is_open())
    {
        fout << "shmid " << shmid << endl ;
           // << "shmkey " << shmkey << endl;
    }
    else
    {
        cerr << W_Msg::CurDate() << shm_table_conf << " open failed" << endl;
        return false;
    }
    fout.close();
    return true;
}

template<class T> 
bool W_ShmTable<T>::ReadShmid()
{
    shmid =  0;
    std::ifstream fin;
    fin.open(shm_table_conf);
    if (fin.is_open())
    {
        std::string name;
        int var;
        while (fin >> name >> var)
        {
            if (name == "shmid")
                shmid = var;
            //if (name == "shmkey")
            //    shmkey = var;
        }
    }
    else
    {
        cerr << W_Msg::CurDate() << shm_table_conf << " open failed" << endl;
        return false;
    }
    fin.close();

    if (shmid != 0)
        return true;
    return false;
}

template<class T> 
bool W_ShmTable<T>::Write(const T* buf, int index)
{
    if (shmid == -1)
    {
        cerr << W_Msg::CurDate() << "No shmget mem. Write shmid=-1" 
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false;
    }
    if (index > blockSize)
    {
        cerr << W_Msg::CurDate() << "Error: Write index out of range blockSize=" 
            << blockSize << " index=" << index 
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false;
    }

    char *addr = shmaddr + index * sizeof(T);
    cout << W_Msg::CurDate() << "Write addr is: " << (long) addr << " Begin memcpy...index=" << index
        << " shmid=" << shmid
        << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
    memcpy(addr, buf, sizeof(T));
    return true;
}

template<class T> 
bool W_ShmTable<T>::Read(T* buf, int index)
{
    if (shmid == -1)
    {
        cerr << W_Msg::CurDate() << "No shmget mem. Read shmid=-1" 
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false;
    }
    if (index > blockSize)
    {
        cerr << W_Msg::CurDate() << "Read index out of range" 
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false;
    }
    
    char *addr = shmaddr + index * sizeof(T);
    cout << W_Msg::CurDate() << "Read addr is: " << (long) addr << " Begin memcpy..." 
        << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
    memcpy(buf, addr, sizeof(T));
    return true;
}

//template<class T> 
//bool W_ShmTable<T>::HasShm()
//{
//    
//    cout << W_Msg::CurDate() << "HasShm begin...shmkey=" << shmkey 
//        << " shmid=" << shmid << endl;
//    return (shmkey != 0 && shmid != 0);
//
//    return true;
//}

template<class T> 
bool W_ShmTable<T>::WriteTest(T &t)
{
    cout << W_Msg::CurDate() << "Write test begin..."   
        << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
    ShmWriteInit();
    W_ShmTableItem item = (W_ShmTableItem) t;
    item.shmIndex = 10;
    item.isMemFree = true;
    item.isDataReady = false;
    Write(&item, 0);
    return true;
}

template<class T> 
bool W_ShmTable<T>::ReadTest(T &t)
{
    cout << W_Msg::CurDate() << "read test begin..."   
        << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
    ShmReadInit();
    W_ShmTableItem item = (W_ShmTableItem) t;
    Read(&item, 0);
    
    cout << W_Msg::CurDate() << "item.shmIndex=" << item.shmIndex 
        << " item.isMemFree=" << item.isMemFree 
        << " item.isDataReady=" << item.isDataReady << endl;
    return true;
}

#endif
