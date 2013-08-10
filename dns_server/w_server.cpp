#include <iostream>

#include "w_udpserver.h"
#include "w_dnsserver.h"

int main()
{
    W_DnsServer server;
    server.Run();
    //server.RunTest();


    return 0;
}
