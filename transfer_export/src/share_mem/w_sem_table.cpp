#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdio>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <string.h>

#include "w_msg.h"
#include "w_sem_table.h"

using std::cerr;
using std::cout;
using std::endl;


W_SemTable::W_SemTable(const char* _sem_table_conf, int _blockSize) 
{
    Init(_sem_table_conf, _blockSize);
}

W_SemTable::W_SemTable() 
{
}

bool W_SemTable::Init(const char* _sem_table_conf, int _blockSize) 
{
    sem_table_conf = _sem_table_conf;
    blockSize = _blockSize;
    
    semid = semkey = 0;
    pid = getpid();
}

bool W_SemTable::ReadSemid()
{
    semid = semkey = 0;
    std::ifstream fin;
    fin.open(sem_table_conf);
    if (fin.is_open())
    {
        std::string name;
        int var;
        while (fin >> name >> var)
        {
            if (name == "semid")
                semid = var;
            if (name == "semkey")
                semkey = var;
        }
    }
    else
    {
        cerr << W_Msg::CurDate() << sem_table_conf << " open failed"
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false;
    }
    fin.close();

    if (semid != 0 && semkey !=0)
        return true;
    return false;
}

bool W_SemTable::WriteSemid()
{
    std::ofstream fout;
    fout.open(sem_table_conf);
    if (fout.is_open())
    {
        fout << "semid " << semid << endl 
            << "semkey " << semkey << endl;
    }
    else
    {
        cerr << W_Msg::CurDate() << sem_table_conf << " open failed"
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false;
    }
    fout.close();
    return true;
}

bool W_SemTable::CreateSemkey()
{
    if (!ReadSemid())
    {
        semkey = time(NULL); 
        cout << W_Msg::CurDate() << "semkey does not exist. Create it. semkey=" << semkey 
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
    }
    cout << W_Msg::CurDate() << "semkey create succeed. semkey=" << semkey 
        << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
    return true;
}

bool W_SemTable::CreateSem()
{
    cout << W_Msg::CurDate() << "Pid:" << pid << " blockSize=" << blockSize << endl; 

    semid=semget(semkey, blockSize ,0600|IPC_CREAT); 
    //semid=semget(IPC_PRIVATE, blockSize ,0600|IPC_CREAT); 
    if ( semid < 0 )
    {
        cerr << W_Msg::CurDate() << "semget ipc_id error．errno=" << errno << " " <<strerror(errno)
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false ;
    }
    
    if (!WriteSemid())
    {
        cerr << W_Msg::CurDate() << "semkey write failed. semkey=" << semkey 
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false;
    }   
    
    return ResetSem(); 
}

bool W_SemTable::ResetSem()
{
    semun sem_u;
    sem_u.val = 1;
    for (int i = 0; i < blockSize; i++)
    {
        int ret = semctl(semid, i, SETVAL, sem_u);    
        if (ret == -1)
        {
            cerr << W_Msg::CurDate() << "semctl SETVAL failed."
                << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
            return false;

        }
    }
    cout << W_Msg::CurDate() << "ResetSem succeed."
        << " file:" << __FILE__ << " line:" << __LINE__ << endl; 

    return true;
}

bool W_SemTable::P(int semNum)
{
    struct sembuf sem_p;
    sem_p.sem_num = semNum; 
    //sem_p.sem_num = 0; 
    sem_p.sem_op = -1;
    sem_p.sem_flg = SEM_UNDO;
    errno = 0;
    int ret = semop(semid, &sem_p, 1);
    if (ret == -1)
    {
        cerr << W_Msg::CurDate() << "P(" << semNum << ") failed. errno=" << errno
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false;
    }
    cout << W_Msg::CurDate() << "P(" << semNum << ") succeed. sem_mum=" << semNum
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
    return true;
} 

bool W_SemTable::V(int semNum)
{
    struct sembuf sem_p;
    sem_p.sem_num = semNum; 
    //sem_p.sem_num = 0; 
    sem_p.sem_op = +1;
    sem_p.sem_flg = SEM_UNDO;
    errno = 0;
    int ret = semop(semid, &sem_p, 1);
    if (ret == -1)
    {
        cerr << W_Msg::CurDate() << "V(" << semNum << ") failed. errno=" <<errno
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false;
    }
    cout << W_Msg::CurDate() << "V(" << semNum << ") succeed. sem_mum=" << semNum
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
    return true;
} 

bool W_SemTable::Destory(int semNum)
{
    int ret = semctl(semid, semNum, IPC_RMID, 0);
    if (ret == -1)
    {
        cerr << W_Msg::CurDate() << "Destory failed" 
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return false;
    }
    return true;
}

bool W_SemTable::Destory()
{
    for (int i = 0; i < blockSize; i++)
    {
        Destory(i);
    }
    return true;
}

bool W_SemTable::RunTest()
{
    CreateSemkey();
    WriteSemid();
    CreateSem();

    P(1);
    V(1);
}

bool W_SemTable::WriteInit()
{ 
    cout << W_Msg::CurDate() << "Make sure it is the first time to Sem WriteInit." 
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 

    if (CreateSemkey()  && WriteSemid() && CreateSem())
    {
        cout << W_Msg::CurDate() << "Sem Write init succeed." 
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return true;
    }
    cout << W_Msg::CurDate() << "Sem Write init failed." 
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 

    return false;
}

bool W_SemTable::ReadInit()
{   
    if (ReadSemid())
    {
        cout << W_Msg::CurDate() << "Sem Read init succeed." 
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 
        return true;
    }
    cout << W_Msg::CurDate() << "Sem Read init failed." 
            << " file:" << __FILE__ << " line:" << __LINE__ << endl; 

    return false;
}

