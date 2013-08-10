#ifndef __W_FIFO_CTL_H_
#define __W_FIFO_CTL_H_


class W_FifoCtl
{
public:
    explicit W_FifoCtl();
   
    int FifoWrite(const char* fifoName, int data, bool isNonBlock = false);
    int FifoRead(const char* fifoName, bool isNonBlock = false); 

private:
    enum
    {
        INT_LEN = 32
    };
    int fd_w;
    int fd_r;
};


#endif
