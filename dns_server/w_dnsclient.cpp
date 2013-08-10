#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h> 
#include <string.h>
#include <error.h>
#include <errno.h>
#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>

#include "w_dnsclient.h"
#include "w_msg.h"

const int __buf_size = 1024;

using std::cout;
using std::cerr;
using std::endl;

W_DnsClient::W_DnsClient()
{
}

bool W_DnsClient::Lookup(const char *argv)
{
    int	connfd, len = 0;
    struct  sockaddr_in servaddr;
    char    buf[__buf_size];
    char    *p;
    int 	i=0;
    struct timeval tv;

    tv.tv_sec = 5;
    tv.tv_usec = 0;
    dns_header  *dnshdr = (dns_header *)buf;
    dns_query   *dnsqer;

    if ((connfd = socket(AF_INET, SOCK_DGRAM, 0 )) < 0)
    {
        ERROR_MSG <<  "socket error!" << MSG_POS;
        return false;
    } 

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(W_DnsConf::GetPort());
    if(inet_pton(AF_INET,W_DnsConf::GetIp(),&servaddr.sin_addr) < 0)
    {
        ERROR_MSG << "inet_pton error." << MSG_POS;
        return false;
    }

    memset(buf, 0, __buf_size); 
    dnshdr->id = (u16)1;
    dnshdr->flags = htons(0x0100);
    dnshdr->nques = htons(1);
    dnshdr->nanswer = htons(0);
    dnshdr->nauth = htons(0);
    dnshdr->naddi = htons(0);
    strcpy(buf + sizeof(dns_header) + 1, argv);
    p = buf + sizeof(dns_header) + 1; 
    while (p < (buf + sizeof(dns_header) + 1 + strlen(argv)))
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
    dnsqer = (dns_query *)(buf + sizeof(dns_header) + 2 + strlen(argv));
    dnsqer->class_type = htons(1);
    dnsqer->type = htons(1);
    setsockopt(connfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    if(sendto(connfd, buf, sizeof(dns_header) + sizeof(dns_query) + strlen(argv) + 2, 0, 
                (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        ERROR_MSG << "sendto error." << MSG_POS;
        return false;
    }

    i = sizeof(struct sockaddr_in);
    if((len = recvfrom(connfd, buf, __buf_size, 0, (struct sockaddr *)&servaddr, (socklen_t *)&i)) < 0)
    {
        if(errno == EWOULDBLOCK){
            ERROR_MSG << "timeout" << MSG_POS;
            return false;
        }else{	
            ERROR_MSG << "recvfrom error" << MSG_POS;
            return false;
        }
    }

    if (dnshdr->nanswer == 0)
    {
        ERROR_MSG << "ack error" << MSG_POS;
        return false;
    }

    p = buf + len -4;
    printf("%s ==> %u.%u.%u.%u\n", argv, (unsigned char)*p, (unsigned char)*(p + 1), 
            (unsigned char)*(p + 2), (unsigned char)*(p + 3));
    close(connfd);
    return true;
}

bool W_DnsClient::Lookup()
{
    return Lookup("www.qq.com");
}

bool W_DnsClient::GetHostByNameLookup()
{
    struct hostent *server = gethostbyname("www.qq.com");
    if (server == NULL) 
    {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    DEBUG_MSG << "Result addr:" << server->h_addr << MSG_POS;
    return true;
}

