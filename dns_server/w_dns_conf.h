#ifndef __DNS_CONF_H_
#define __DNS_CONF_H_
#include <cstdio>
#include <stdint.h>

//extern const char* __server_ip;
//extern const int __server_port; 

typedef uint16_t u16;
typedef uint32_t u32;

typedef struct
{   
    u16 id : 16;
    u16 flags : 16;
    u16 nques : 16;
    u16 nanswer: 16;
    u16 nauth : 16;
    u16 naddi : 16;
}dns_header;

typedef struct
{
    u16 type : 16;
    u16 class_type : 16;
}dns_query;

class W_DnsConf
{
public:
    explicit W_DnsConf();
    static bool Init();

    static const char* GetIp();
    static int GetPort();

private:
    static char ip[256];
    static int port;
    static bool isInit;
};

#endif 

