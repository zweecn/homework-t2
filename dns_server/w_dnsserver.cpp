#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h> 
#include <errno.h> 
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <iostream>
#include <fstream>

#include "w_dnsserver.h"
#include "w_msg.h"

const bool __write_to_file = false;

const char* domain_file = "conf/a_record";
const char* ptr_file = "conf/ptr_record";

const int __max_event = 10000;
const int __buf_size = 1024;


W_DnsServer::W_DnsServer()
{
    if(!InitIpMap())
    {
        ERROR_MSG << "Init ip map failed. exit(1)" << MSG_POS;
        exit(1);
    }
    if(!InitPtrMap())
    {
        ERROR_MSG << "Init ptr map failed. exit(1)" << MSG_POS;
        exit(1);
    }
}

void W_DnsServer::SetNonBlocking(int sock)
{
    int opts;
    opts = fcntl(sock,F_GETFL);

    if(opts < 0)
    {
        ERROR_MSG << "fcntl(sock,GETFL)" << MSG_POS;
        exit(1);
    }

    opts = opts|O_NONBLOCK;
    if(fcntl(sock,F_SETFL,opts) < 0)
    {
        ERROR_MSG << "fcntl(sock,SETFL,opts)" << MSG_POS;
        exit(1);
    }    

}

bool W_DnsServer::InitIpMap()
{
    std::ifstream fin;
    fin.open(domain_file);
    if (!fin.is_open()) 
    {
        ERROR_MSG << "Can not open file: " << domain_file << MSG_POS; 
        return false;
    }
    string dom, ipt;
    while (fin >> dom >> ipt)
    {
        ipMap[dom] = ipt;
        DEBUG_MSG << "IPMAP: " << dom << "==>" << ipMap[dom] << MSG_POS;
    }
    fin.close();
    return true;
}

bool W_DnsServer::InitPtrMap()
{
    std::ifstream fin;
    fin.open(ptr_file);
    if (!fin.is_open()) 
    {
        ERROR_MSG << "Can not open file: " << domain_file << MSG_POS; 
        return false;
    }
    string ip, ptr_name;
    while (fin >> ip >> ptr_name)
    {
        ptrMap[ip] = ptr_name;
        char tmp[__buf_size] = {0};
        sprintf(tmp, "%s.in-addr.arpa", ip.c_str());
        ptrMap[string(tmp)] = ptr_name;
        DEBUG_MSG << "PTRMAP: " << ip << "==>" << ptrMap[ip] << MSG_POS;
    }
    fin.close();
    return true;
}

//const char* W_DnsServer::GetIp(const char* domain)
int W_DnsServer::GetIp(const char* domain)
{
    memset(ip, 0, sizeof(ip));
    string ipRes = ipMap[string(domain)];
    
    DEBUG_MSG << "Get " << domain << " ip=" << ipRes << MSG_POS;
    char c = 0;
    int j = 0;
    for (int i = 0; i < ipRes.size(); i++)
    {
        if (ipRes[i] == '.')
        {
            ip[j++] = c;
            c = 0;
        }
        else
        {
            c = c * 10 + ipRes[i] - '0';
        }
    }
    ip[j] = c;
    
    int ip_t = (ip[0] << 24) + (ip[1] << 16) + (ip[2] << 8) + ip[3];

    return ip_t;
}

u16 W_DnsServer::MakeDomain(char * buf, const char* ipname)
{
    string ptr = ptrMap[ipname];
    const char* tmp = ptr.c_str();
    memcpy(buf+1, tmp, strlen(tmp));
    int i = 0;
    char* p = buf + 1; 
    while (p < (buf + 1 + strlen(tmp)))
    {
        if ( *p == '.')
        {
            *(p - i - 1) = i;
            i = 0;
        } 
        else
        {
            i++;
        }

        p++;
    }

    *(p - i - 1) = i;

    return strlen(buf) + 1; 
}

bool W_DnsServer::WriteFile(const char* buf, int size, const char* file)
{
    if (__write_to_file)
    {
        std::ofstream fout;
        fout.open(file, std::ios::binary);
        if (!fout.is_open())
        {
            DEBUG_MSG << "Can't open file:" << file << MSG_POS;
            return false;
        }
        fout.write(buf, size);
        fout.close();
    }
    return true;
}


void W_DnsServer::Run()
{
    DEBUG_MSG << "Pid=" << getpid() << MSG_POS;

    int listenfd, sockfd,epfd, nfds;
    int ret = 0;
    ssize_t n;
    ssize_t res_size;
    socklen_t clilen;

    char req_buf[__buf_size];
    char res_buf[__buf_size];

    //声明epoll_event结构体的变量,ev用于注册事件,数组用于回传要处理的事件
    struct epoll_event ev,events[20];

    //生成用epoll专用文件描述符
    epfd = epoll_create(256);
    if (epfd < 0)
    {
        ERROR_MSG << "epoll_create(256) failed. epfd=" << epfd << MSG_POS;
        exit(1);
    }
    DEBUG_MSG << "epoll_create(256) epfd=" << epfd << MSG_POS;

    struct sockaddr_in clientaddr;	
    struct sockaddr_in serveraddr;	
    listenfd = socket(AF_INET, SOCK_DGRAM, 0);
    DEBUG_MSG << "listenfd=" << listenfd << MSG_POS;

    socklen_t addrlen = sizeof(clientaddr);

    //把socket设置为非阻塞方式	
    SetNonBlocking(listenfd);	
    //设置与要处理的事件相关的文件描述符	
    ev.data.fd=listenfd;
    //设置要处理的事件类型	
    ev.events=EPOLLIN|EPOLLET;	
    //注册epoll事件

    ret = epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev);
    if (ret < 0)
    {
        ERROR_MSG << "epoll_ctl failed. ret=" << ret << MSG_POS;
        exit(1);
    }
    memset(&serveraddr, 0, sizeof(serveraddr));
    
    serveraddr.sin_family = AF_INET;
    //const char *local_addr = __server_ip;
    //inet_aton(local_addr,&(serveraddr.sin_addr));
    //serveraddr.sin_addr.s_addr = inet_addr(local_addr);
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    serveraddr.sin_port = htons(W_DnsConf::GetPort());

    ret = bind(listenfd,(struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (ret < 0)
    {
        ERROR_MSG << "bind failed. ret=" << ret << MSG_POS;
        exit(1);
    }
    DEBUG_MSG << "DNS server PORT=" << W_DnsConf::GetPort() << MSG_POS;
    memset(req_buf, 0, __buf_size);

    for ( ; ; ) 
    {
        //等待epoll事件的发生
        nfds = epoll_wait(epfd, events, __max_event, 0);
        //处理所发生的所有事件      

        for(int i = 0; i < nfds; ++i)	
        {
            if(events[i].events & EPOLLIN)				
            {
                if ( (sockfd = events[i].data.fd) < 0)
                    continue;

                memset(req_buf, 0, __buf_size);
                if ((n = recvfrom(sockfd, req_buf, __buf_size,0,(struct sockaddr*)&clientaddr,&addrlen)) < 0)
                {
                    if (errno == ECONNRESET)
                    {
                        close(sockfd);
                        events[i].data.fd = -1;
                    } 
                    else 
                    {
                        ERROR_MSG << "readreq_buf error" << MSG_POS;
                    }
                } 
                else if (n == 0)
                {
                    ERROR_MSG << "connfd=0" << MSG_POS;
                    close(sockfd);
                    events[i].data.fd = -1;
                }

                DEBUG_MSG << "recv n=" << n << MSG_POS;

                char* p = (char *)&clientaddr.sin_addr;
                char clientIp[256] = "\0";
                sprintf(clientIp, "%d.%d.%d.%d", *p, *(p+1), *(p+2), *(p+3));
                DEBUG_MSG << "recv from: " << clientIp << MSG_POS;

                //请求报文写入文件看看
                WriteFile(req_buf, n, "req_buf");

                //构造DNS返回报文
                //res_size = GetA(n, req_buf, res_buf, __buf_size);
                res_size = GetRes(n, req_buf, res_buf, __buf_size);

                WriteFile(res_buf, res_size, "res_buf");

                //设置用于写操作的文件描述符				
                ev.data.fd = sockfd;
                //设置用于注测的写操作事件				
                ev.events = EPOLLOUT | EPOLLET;				
                //修改sockfd上要处理的事件为EPOLLOUT				
                ret = epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);				
                if (ret < 0)
                {
                    ERROR_MSG << "epoll_ctl failed. ret=" << ret << MSG_POS;
                }
            }
            else if(events[i].events & EPOLLOUT)				
            {    
                if(events[i].data.fd == -1)
                    continue;
               
                sockfd = events[i].data.fd;

                // Answer to client
                DEBUG_MSG << "sendto client..." << MSG_POS;				
                sendto(sockfd, res_buf, res_size, 0, (struct sockaddr*)&clientaddr, addrlen);

                //write(sockfd, req_buf, n);
                //设置用于读操作的文件描述符				
                ev.data.fd = sockfd;				
                //设置用于注测的读操作事件			
                ev.events = EPOLLIN|EPOLLET;		
                //修改sockfd上要处理的事件为EPOLIN
                epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);				
            }
        }
    }
}

int W_DnsServer::GetRes(int req_size, char* req_buf, char* res_buf, int buf_size)
{
    //Get response buf
    memset(res_buf, 0, __buf_size);
    char *res_buf_p = res_buf + req_size; 

    DEBUG_MSG << "[1] Put request data to response ..." << MSG_POS;
    // [1] Put request data to response
    memcpy(res_buf, req_buf, req_size);

    DEBUG_MSG << "[2] Let it be response msg ..." << MSG_POS;
    // [2] Let it be response msg
    dns_header *hd_p = (dns_header *) res_buf;
    hd_p->flags |= htons(0x8000); 
    hd_p->nanswer = htons(1); 


    DEBUG_MSG << "[3] Get query domain ..." << MSG_POS;
    // [3] Get query domain
    char *domptr = req_buf + sizeof(dns_header);
    char query_dom[__buf_size] = "\0";
    int dom_len = *domptr;
    int j = 0;
    domptr++;
    while (dom_len)
    {
        query_dom[j++] = *(domptr++);
        dom_len--;
        if (dom_len == 0 && *domptr == 0) 
        {
            break;
        }
        else if (dom_len == 0)
        {
            dom_len = *domptr;
            domptr++;
            query_dom[j++] = '.';
        }
    }

    // [4] Get the query type
    u16* query_type_p = (u16 *)&req_buf[req_size - 4];
    int query_type = ntohs(*query_type_p);
    DEBUG_MSG << "[4] query_type=" << query_type << MSG_POS;
    int len = 0;
    switch (query_type)
    {
        case 1:
            len = MakeARecord(res_buf_p, query_dom);
            break;
        case 12:
            len = MakePTRRecord(res_buf_p, query_dom);
            break;
        default:
            break;
    }

    return req_size + len;
}


int W_DnsServer::MakeARecord(char* res_buf_p, const char* query_dom)
{
    DEBUG_MSG << "[5] Append response info (Record A)...domain=" << query_dom << MSG_POS;
    // [5] Append response info (Record A)
    u16* p16 = (u16*)res_buf_p;
    *p16 = htons(0xc00c);   // domain ptr : 2 bytes
    *(p16+1) = htons(1);    // type : 2 bytes
    *(p16+2) = htons(1);    // class_type : 2 bytes
    u32* p32 = (u32 *)(res_buf_p + 6); 
    *p32 = htons(0);     // ttl : 4 bytes
    p16 = (u16 *)(res_buf_p + 10);
    *p16 = htons(0x0004);   // len : 2 bytes
    p32 = (u32 *)(res_buf_p + 12);
    *p32 = htonl(GetIp(query_dom));     //IP : 4 bytes
    DEBUG_MSG << "[6] Make response packet finished." << MSG_POS;

    return 16;
}

int W_DnsServer::MakePTRRecord(char* res_buf_p, const char* query_dom)
{
    DEBUG_MSG << "[5] Append response info (Record PTR)...domain=" << query_dom << MSG_POS;
    // [5] Append response info (Record A)
    u16* p16 = (u16*)res_buf_p;
    *p16 = htons(0xc00c);   // domain ptr : 2 bytes
    *(p16+1) = htons(12);    // type : 2 bytes
    *(p16+2) = htons(1);    // class_type : 2 bytes
    u32* p32 = (u32 *)(res_buf_p + 6); 
    *p32 = htons(1111);     // ttl : 4 bytes
    p16 = (u16 *)(res_buf_p + 10);

    char tmp[256] = {0};
    u16 len = MakeDomain(tmp, query_dom);
    *p16 = htons(len);   // len : 2 bytes
    memcpy(res_buf_p + 12, tmp, len); // len bytes

    return 12 + len;
}

int W_DnsServer::GetA(int req_size, char* req_buf, char* res_buf, int buf_size)
{
    int n = req_size;
    //Get response buf
    memset(res_buf, 0, __buf_size);
    char *res_buf_p = res_buf + n ;

    DEBUG_MSG << "[1] Put request data to response ..." << MSG_POS;
    // [1] Put request data to response
    memcpy(res_buf, req_buf, n);

    DEBUG_MSG << "[2] Let it be response msg ..." << MSG_POS;
    // [2] Let it be response msg
    dns_header *hd_p = (dns_header *) res_buf;
    hd_p->flags |= htons(0x8000); 
    hd_p->nanswer = htons(1); 


    DEBUG_MSG << "[3] Get query domain ..." << MSG_POS;
    // [3] Get query domain
    char *domptr = req_buf + sizeof(dns_header);
    char query_dom[__buf_size] = "\0";
    int dom_len = *domptr;
    int j = 0;
    domptr++;
    while (dom_len)
    {
        query_dom[j++] = *(domptr++);
        dom_len--;
        if (dom_len == 0 && *domptr == 0) 
        {
            break;
        }
        else if (dom_len == 0)
        {
            dom_len = *domptr;
            domptr++;
            query_dom[j++] = '.';
        }
    }

    DEBUG_MSG << "[4] Append response info (Record A)...domain=" << query_dom << MSG_POS;
    // [4] Append response info (Record A)
    u16* p16 = (u16*)res_buf_p;
    *p16 = htons(0xc00c);   // domain ptr : 2 bytes
    *(p16+1) = htons(1);    // type : 2 bytes
    *(p16+2) = htons(1);    // class_type : 2 bytes
    u32* p32 = (u32 *)(res_buf_p + 6); 
    *p32 = htons(1111);     // ttl : 4 bytes
    p16 = (u16 *)(res_buf_p + 10);
    *p16 = htons(0x0004);   // len : 2 bytes
    p32 = (u32 *)(res_buf_p + 12);
    *p32 = htonl(GetIp(query_dom));     //IP : 4 bytes
    DEBUG_MSG << "[5] Make response packet finished." << MSG_POS;

    return n + 16;
}


