#include <iostream>

#include "w_udpclient.h"
#include "w_dnsclient.h"

int main(int argc, char** argv)
{
    W_DnsClient client;
    if (argc == 1)
    {
        client.Lookup();
    }
    else
    {
        for (int i = 1; i < argc; i++)
        {
            client.Lookup(argv[i]);
        }
    }
    
    // just with root and use port 53
    // client.GetHostByNameLookup();
    return 0;
}
