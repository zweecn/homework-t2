#ifndef __W_SEM_TABLE_H_
#define __W_SEM_TABLE_H_

#include <sys/types.h> 
#include <sys/ipc.h>
#include <sys/sem.h>

union semun{    
    int val;    
    struct semid_ds *buf;    
    ushort *array;    
};    


class W_SemTable
{
public:
    explicit W_SemTable(const char* _sem_table_conf, int _blockSize); 
    explicit W_SemTable(); 

    bool Init(const char* _sem_table_conf, int _blockSize);

    bool WriteInit();
    bool ReadInit();

    bool ReadSemid();
    bool WriteSemid();
    bool CreateSemkey();

    bool CreateSem();
    bool Destory(int semNum);
    bool Destory();

    bool P(int semNum);
    bool V(int semNum);

    bool RunTest();

    bool ResetSem();

private:

    const char* sem_table_conf;
    int blockSize;

    int pid;
    int semid;
    int semkey;
};

#endif 


