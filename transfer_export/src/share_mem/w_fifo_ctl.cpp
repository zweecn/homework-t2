#include <iostream>
#include <errno.h>   
#include <unistd.h>   
#include <fcntl.h> 
#include <cstdio>
#include <stdlib.h>

//Added by sherardliu 2013-08-06-10:15
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "w_msg.h"
#include "w_fifo_ctl.h"

using std::cout;
using std::cerr;
using std::endl;

W_FifoCtl::W_FifoCtl()
{

}

int W_FifoCtl::FifoWrite(const char* fifoName, int data, bool isNonBlock /* = false */)
{
    int res = mkfifo(fifoName, 0600);
    if(res == -1 && errno != EEXIST)
    {
        cerr << W_Msg::CurDate() << "Fifo " << fifoName << " create failed." << endl;
        return -1;
    }
    if (isNonBlock)
    {
        fd_w = open(fifoName ,O_WRONLY | O_NONBLOCK);
    }
    else
    {
        fd_w = open(fifoName ,O_WRONLY);
    }
    if (fd_w == -1)
    {
        cerr << W_Msg::CurDate() << "Fifo " << fifoName << " open failed." 
            << " file:" << __FILE__ << " line:" << __LINE__ << endl;
        return -1;
    }

    char buf[INT_LEN];
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%d", data);
    int len = write(fd_w, buf, sizeof(buf));
    if (len == -1)
    {
        cerr << W_Msg::CurDate() << "Fifo " << fifoName << " write failed." << endl;
        return -1;
    }
    close(fd_w);
    cout << W_Msg::CurDate() << "FifoWrite data:" << buf << endl;

    return sizeof(buf);
}

int W_FifoCtl::FifoRead(const char* fifoName, bool isNonBlock /* = false */)
{

    int res = mkfifo(fifoName, 0600);
    if(res == -1 && errno != EEXIST)
    {
        cerr << W_Msg::CurDate() << "Fifo " << fifoName << " create failed." << endl;
        return false;
    }
    if (isNonBlock)
    {
        fd_r = open(fifoName ,O_RDONLY | O_NONBLOCK);
    }
    else
    {
        fd_r = open(fifoName ,O_RDONLY);
    }
    if (fd_r == -1)
    {
        cerr << W_Msg::CurDate() << "Fifo " << fifoName << " open failed."
            << " file:" << __FILE__ << " line:" << __LINE__ << endl;
        return -1;
    }
    char buf[INT_LEN];
    int len = read(fd_r, buf, sizeof(buf));
    if (len == -1)
    {
        cerr << W_Msg::CurDate() << "Fifo " << fifoName << " write failed." << endl;
        return -1;
    }
    close(fd_r);
    int data = atoi(buf);
    cout << W_Msg::CurDate() << "FifoRead data:" << buf << " to int:" << data << endl;

    return data;
}

