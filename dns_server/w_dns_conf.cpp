#include <fstream>
#include <iostream>

#include "w_dns_conf.h"
#include "w_msg.h"

//const char* __server_ip = "127.0.0.1";
//const int __server_port = 55555;

const char* ip_port = "conf/ip_port";

char W_DnsConf::ip[256] = {0};
int W_DnsConf::port = 0;

bool W_DnsConf::isInit = false;

W_DnsConf::W_DnsConf()
{
}

bool W_DnsConf::Init()
{
    std::ifstream fin;
    fin.open(ip_port);
    if (!fin.is_open())
    {
        ERROR_MSG << "Can not open file: " << ip_port << MSG_POS;
        return false;
    }
    std::string name;
    while (fin >> name)
    {
        if (name == "ip")
        {
            fin >> ip;
        }
        else if (name == "port")
        {
            fin >> port;
        }
    }
    fin.close();
    return true;
}

const char* W_DnsConf::GetIp()
{
    if (!isInit)
    {
        isInit = Init(); 
    }
    return ip;
}

int W_DnsConf::GetPort()
{    
    if (!isInit)
    {
        isInit = Init(); 
    }
 
    return port;
}
