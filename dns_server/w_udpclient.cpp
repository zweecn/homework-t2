#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

#include "w_udpclient.h"
#include "w_msg.h"
#include "w_dns_conf.h"

//const char* __server_ip = "127.0.0.1";
//const int __server_port = 5555;

const int __buf_size = 1024;

using std::cin;
using std::cout;
using std::endl;

W_UdpClient::W_UdpClient()
{

}

void W_UdpClient::UdpcRequest(int sockfd,const struct sockaddr_in *addr,int len) 
{
    char buf[__buf_size];
    int n;

    while(1) 
    {
        // 从键盘读入,写到服务端
        //fgets(buf,__buf_size,stdin);
        memset(buf, 0, __buf_size);
        cin >> buf;
        sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)addr, len);
        memset(buf, 0, __buf_size);
        
        // 从网络上读,写到屏幕上
        n = recvfrom(sockfd, buf, __buf_size, 0, NULL, NULL);
        buf[n - 1] = 0;
        //fprintf(stdout,"I received %s from server.\n",buf);
        DEBUG_MSG << "Recv:" << buf << MSG_POS;
    }
}

void W_UdpClient::Run() 
{
    DEBUG_MSG << "Input data and press enter." << MSG_POS ;

    int sockfd;
    struct sockaddr_in addr;
    
    sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0) 
    {
        ERROR_MSG << "Socket Error:" << strerror(errno) << MSG_POS;
        exit(1);
    }
    // 填充服务端的资料
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(W_DnsConf::GetPort());
    if(inet_aton(W_DnsConf::GetIp(),&addr.sin_addr) < 0) 
    {
        ERROR_MSG << "IP error:" << strerror(errno) << MSG_POS;
        exit(1);
    }
    UdpcRequest(sockfd, &addr, sizeof(struct sockaddr_in));
    close(sockfd);
    exit(0);
}

