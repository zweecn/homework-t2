#include <iostream>
#include <fstream>
#include <errno.h>   
#include <unistd.h>   
#include <fcntl.h> 
#include <stdlib.h>
#include <cstdio>

#include "w_datarecv.h"
#include "w_shm_ctl.h"
#include "w_msg.h"

// Added by sherardliu 2013-08-06 10:53
#include "transfer_client.h"

using std::cout;
using std::cerr;
using std::endl;

const char* datafile = "data/datafile";
const char* fifofile = "data/fifofile";

W_DataRecv::W_DataRecv()
{
    index = 0;
	id = 0;
}

const char* W_DataRecv::GetFifoName()
{
    sprintf(fifoname, "fifo_%d", id);
    return fifoname;
}

bool W_DataRecv::ShmReadInit()
{
    // 1. GetShmKey
    bool flag = w_shmctl.ReadShmid();
    if (!flag)
    {
        cerr << W_Msg::CurDate() << "Create shmkey failed. file:" 
            << __FILE__ << " line:"<< __LINE__  << endl;
        return false;
    }
    // 2. AttachMem
    flag = w_shmctl.AttachMem();
    if (!flag)
    {
        cerr << W_Msg::CurDate() << "AttachShm failed. file:" 
            << __FILE__ << " line:"<< __LINE__  << endl;
        return false;
    }

    w_semtable.Init(__sem_table_conf, __shm_block_size);
    if (!w_semtable.ReadInit())
    {
        cerr << W_Msg::CurDate() << "AttachShm failed. file:" 
            << __FILE__ << " line:"<< __LINE__  << endl;
        return false;
    }
    if (!w_semtable.ResetSem())
    {
        cerr << W_Msg::CurDate() << "AttachShm failed. file:" 
            << __FILE__ << " line:"<< __LINE__  << endl;
        return false; 
    }

    return true;
}

bool W_DataRecv::WriteData(const char* buf, int len)
{
	static int w_cnt = 0;
    if (!w_shmctl.HasShm())
    {
        bool flag = ShmReadInit();
        if (!flag)
        {
            cerr << W_Msg::CurDate() << "WriteData.ReadInit failed." 
                << " file:" <<  __FILE__ << " line:"<< __LINE__  << endl;
            return false;
        }
        cout << W_Msg::CurDate() << "WriteData.ReadInit succeed." 
            << " file:" <<  __FILE__ << " line:"<< __LINE__  << endl;
            
    }

    // Main logic:
    // 1. Listen
    // 2. Receive data (TCP)
    // 3. Find free shm and Write data to shm
    // 4. Go step 1
   
    index++;
    index %= __shm_block_size;
    
	cout << W_Msg::CurDate() << "WriteData.FifoWrite wait..." << " cnt=" << w_cnt
		<< " index=" << index
        << " file:" <<  __FILE__ << " line:"<< __LINE__  << endl;
    w_semtable.P(index);
    bool flag = w_shmctl.Write(buf, len, index);
    w_semtable.V(index);
    if (!flag)
    {
        cerr << W_Msg::CurDate() << "WriteData.w_shmctl.write failed." 
            << " file:" <<  __FILE__ << " line:"<< __LINE__  << endl;
        return false; 
    }
    cout << W_Msg::CurDate() << "WriteData.Write succeed." << " cnt=" << w_cnt
            << " file:" <<  __FILE__ << " line:"<< __LINE__  << endl;
    
    //if (w_fifoctl.FifoWrite(fifofile, index) == -1)
    //if (w_fifoctl.FifoWrite(fifofile, index, true) == -1)
	id++;
	id %= __process_size;
    if (w_fifoctl.FifoWrite(GetFifoName(), index) == -1)
    {
        cerr << W_Msg::CurDate() << "WriteData.FifoWrite failed." 
            << " file:" <<  __FILE__ << " line:"<< __LINE__  << endl;
        return false;
    }
	cout << W_Msg::CurDate() << "WriteData.FifoWrite succeed." << " cnt=" << w_cnt++
            << " file:" <<  __FILE__ << " line:"<< __LINE__  << endl;
    return true;
}


bool W_DataRecv::ReadData(char* buf, int len)
{
    if (!w_shmctl.HasShm())
    {
        bool flag = ShmReadInit();
        if (!flag)
        {
            cerr << W_Msg::CurDate() << "ReadData.ShmReadInit failed. Pid=" << getpid()  
                << " file:" << __FILE__ << " line:" << __LINE__ << endl;
            return false;
        }
    }

    // Main logic (Muti process):
    // 1. Receive the notice to read
    // 2. Read it
    // 3. Send data to server(TCP). See ReadFileTest()
    // 4. Go 1. See ReadFileTest()
    
    //cout << W_Msg::CurDate() << "ReadData.read fifo=" << fifofile << " pid=" << getpid()
    cout << W_Msg::CurDate() << "ReadData.read fifo=" << GetFifoName() << "... pid=" << getpid()
            << " file:" << __FILE__ << " line:" << __LINE__ << endl;
    int index = w_fifoctl.FifoRead(GetFifoName());
    //int index = w_fifoctl.FifoRead(fifofile);
 	cout << W_Msg::CurDate() << "ReadData.index=" << index
            << " file:" << __FILE__ << " line:" << __LINE__ << endl;
    if (index == -1)
    {
        cerr << W_Msg::CurDate() << "ReadData.fofictl.fiforead failed. Pid=" << getpid()
            << " file:" << __FILE__ << " line:" << __LINE__ << endl;
        return false; 
    }
 
    w_semtable.P(index);
    bool flag = w_shmctl.Read(buf, len, index);
    w_semtable.V(index);

    if (!flag)
    {
        cerr << W_Msg::CurDate() << "ReadData.w_shmctl.read failed. Pid=" << getpid()
            << " file:" << __FILE__ << " line:" << __LINE__ << endl;
        return false; 
    }

    cout << W_Msg::CurDate() << "ReadData. Pid=" << getpid() << endl;
    return true;
}


bool W_DataRecv::WriteFileTest()
{
    using std::ifstream;
    ifstream fin;
    fin.open(datafile);
    if (!fin.is_open())
    {
        cerr << W_Msg::CurDate() << "File " << datafile << " open failed:" 
            << " file:" << __FILE__ << " line:" << __LINE__ << endl;
        return false;
    }
    
    //w_routectl.Init();
    //w_routectl.ResetSem();
    
    const int size = w_shmctl.GetPerBlockSize();
    char buf[size];
    while (fin.getline(buf, size))
    {
        if (!WriteData(buf, size))
        {
            fin.close();
            cerr << W_Msg::CurDate() << "Write failed:" 
                << " file:" << __FILE__ << " line:" << __LINE__ << endl;
        
            return false;
        }
        cout << W_Msg::CurDate() << "WriteFileTest.data=" << buf << endl;
    }

    fin.close();
    return true;
}

bool W_DataRecv::ReadFileTest()
{    
	static int cnt = 0;
    const int size = w_shmctl.GetPerBlockSize();
    char buf[size];
    while (true)
    {
        if (ReadData(buf, size))
		{
			cout << W_Msg::CurDate() << "Pid=" << getpid() << " cnt=" << cnt++
				<< " ReadFileTest.data=" << buf << endl;
        	// [2] Put data here and send to server
			SendData(buf, size);
		}
    }

    return true;
}

bool W_DataRecv::WriteTest()
{
    const int SIZE = 20;
    char buf[SIZE] = "Helloworld";
    // AttachMem
    cout << W_Msg::CurDate() << "WriteTest.hasShm=" << w_shmctl.HasShm() << endl;
    if (!w_shmctl.HasShm())
    {
        cout << W_Msg::CurDate() << "WriteTest begin init..." << endl;
        if (!ShmReadInit()) 
        {
            cerr << W_Msg::CurDate() << "WriteTest init failed:" 
                << " file:" << __FILE__ << " line:" << __LINE__ << endl;
            return false;
        }
    }
    //  Write
    if (w_shmctl.Write(buf, SIZE, 2))
    {
        cout << W_Msg::CurDate() << "WriteData:" << buf << endl;
    }
    return true;
}

bool W_DataRecv::ReadTest()
{
    const int SIZE = w_shmctl.GetPerBlockSize();
    char buf[SIZE];
    // 1-2 ReadShmid, AttachMem
    if (!w_shmctl.HasShm())
    {
        cout << W_Msg::CurDate() << "ReadTest begin init..." << endl;
        if (!ShmReadInit())
        {
            cerr << W_Msg::CurDate() << "ReadTest init failed:" 
                << " file:" << __FILE__ << " line:" << __LINE__ << endl;
            return false;
        }
    }
    // 3. Read
    if (w_shmctl.Read(buf, SIZE, 2))
    {
        cout << W_Msg::CurDate() << "ReadData:" << buf << endl;
    }
    return true;
}
