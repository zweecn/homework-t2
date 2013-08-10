#ifndef __W_DNS_SERVER_H_
#define __W_DNS_SERVER_H_

#include <map>
#include <string>

#include "w_dns_conf.h"

using std::map;
using std::string;

class W_DnsServer
{
public:
    explicit W_DnsServer();
    void Run();

private:
    void SetNonBlocking(int sock);
    bool InitIpMap(); 
    bool InitPtrMap();
    bool WriteFile(const char* buf, int size, const char* file);
    int GetIp(const char* domain);
    u16 MakeDomain(char * buf, const char* ipname);


    int GetRes(int req_size, char* req_buf, char* res_buf, int buf_size);
    int MakeARecord(char* res_buf_p, const char* query_dom);
    int MakePTRRecord(char* res_buf_p, const char* query_dom);

    // This is backup
    int GetA(int req_size, char* req_buf, char* res_buf, int buf_size); 

    char ip[4];
    map<string, string> ipMap;
    map<string, string> ptrMap;
};


#endif 


