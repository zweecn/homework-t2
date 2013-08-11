#include <iostream>

#include "w_httpserver.h"
#include "w_epoll.h"

int main()
{
//    W_HttpServer hs;
//    hs.run();
    W_Epoll we;
    we.run();
    return 0;
}
