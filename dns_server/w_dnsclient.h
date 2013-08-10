#ifndef __W_DNS_CLIENT_H_ 
#define __W_DNS_CLIENT_H_ 

#include "w_dns_conf.h"

class W_DnsClient
{
public:
    explicit W_DnsClient();
    
    bool Lookup(const char *argv);
    bool Lookup();
    bool GetHostByNameLookup();
};


#endif 


