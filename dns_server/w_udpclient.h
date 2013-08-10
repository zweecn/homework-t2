#ifndef __W_CLIENT_H_
#define __W_CLIENT_H_


class W_UdpClient
{
public:
    explicit W_UdpClient();
    void UdpcRequest(int sockfd,const struct sockaddr_in *addr,int len);
    void Run();
};

#endif 


