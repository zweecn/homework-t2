#ifndef __W_UDP_SERVER_H_
#define __W_UDP_SERVER_H_

class W_UdpServer
{
public:
    explicit W_UdpServer();
    void setnonblocking(int sock);
    void Run();
};


#endif 


