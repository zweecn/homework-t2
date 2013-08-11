#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/epoll.h>
#include<fcntl.h>
#include <unistd.h>

#include "w_epoll.h"
#include "w_httpserver.h"

//const char* __ip = "127.0.0.1";
const int __port = 8888;
const int __max_listen = 5;
const int __max_events = 100;
const int __filename_size = 256;
const int __buf_size = 1024*10;

int W_Epoll::make_http_res(char* res_buf, const char* req_buf)
{
    W_HttpServer hs;
    char file[__filename_size], filename[__filename_size];
    char ct_buf[__buf_size], hd_buf[__buf_size];
    bzero(file, __filename_size);
    bzero(filename, __filename_size);
    bzero(ct_buf, __buf_size);
    bzero(hd_buf, __buf_size);

    hs.get_file(file, req_buf);
    hs.make_filename(filename, file);

    int ct_size = hs.read_content(ct_buf, __buf_size, filename);
    int hd_size = 0, res_size = 0;
    if (ct_size > 0)
    {
        //hd_size = hs.make_header(hd_buf, __buf_size, 200, "text/html", ct_size);
        hd_size = hs.make_header(hd_buf, __buf_size, 200, "application/octet-stream", ct_size);
        res_size = hs.make_body(res_buf, __buf_size, hd_buf, hd_size, ct_buf, ct_size); 
    }
    else
    {
        hd_size = hs.make_header(hd_buf, __buf_size, 404, "text/html", ct_size);
        res_size = hs.make_body(res_buf, __buf_size, hd_buf, hd_size, NULL, 0); 
    }
    return res_size;
}

int W_Epoll::setnonblocking(int sock)
{
    int opts;
    opts = fcntl(sock,F_GETFL);
    if (opts < 0) 
    {
        return 0;
    }
    opts = opts | O_NONBLOCK;
    if (fcntl(sock,F_SETFL,opts) < 0) 
    {
        return 0;
    }
    return 1;
}

int W_Epoll::create_and_bind()
{
    int sock_fd = socket(AF_INET,SOCK_STREAM,0);
    if (sock_fd < 0) 
    {
        perror("create socket failed");
        exit(1);
    }

    struct sockaddr_in addr_serv;
    memset(&addr_serv, 0, sizeof(addr_serv));
    addr_serv.sin_family = AF_INET;
    addr_serv.sin_port = htons(__port);
    //addr_serv.sin_addr.s_addr = inet_addr(__ip);
    addr_serv.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock_fd,(struct sockaddr *)&addr_serv,sizeof(struct sockaddr_in)) < 0) 
    {
        perror("bind error");
        exit(1);
    }

    if (listen(sock_fd, __max_listen) < 0) 
    {
        perror("listen failed");
        exit(1);
    }
    return sock_fd;
}

int W_Epoll::run()
{
    fprintf(stdout, "HTTP server runing. Listening port:%d\n", __port);
    int sock_fd = create_and_bind();
    int recv_num, send_num;
    char recv_buf[__buf_size], send_buf[__buf_size];	

    struct sockaddr_in addr_client;
    socklen_t client_size = sizeof(struct sockaddr_in);

    //初始化epoll描述符
    int epfd = epoll_create(__max_events);
    if (epfd <= 0) 
    {
        perror("create epoll failed!");
        exit(1);
    }
    int i, nready;
    struct epoll_event ev, events[__max_events];

    //注册sock_fd描述符监听事件
    ev.data.fd = sock_fd;
    ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(epfd,EPOLL_CTL_ADD,sock_fd,&ev);

    while (1) 
    {
        nready = epoll_wait(epfd, events, __max_events, 500);

        for (i = 0;i < nready;i ++) 
        {
            //如果是sock_fd描述符可读，则创建新连接，并添加监听事件
            if (events[i].data.fd == sock_fd) 
            {
                int conn_fd = accept(sock_fd, (struct sockaddr *)&addr_client, &client_size);
                if (conn_fd < 0) 
                {
                    perror("accept failed");
                    exit(1);
                }
                setnonblocking(conn_fd);
                ev.data.fd = conn_fd;
                ev.events = EPOLLIN | EPOLLET;
                epoll_ctl(epfd,EPOLL_CTL_ADD,conn_fd,&ev);
            }
            else if(events[i].events & EPOLLIN && events[i].data.fd >= 0) 
            {
                //如果描述符可读，则读取数据后修改描述符状态去监听可写状态
                bzero(recv_buf, sizeof(recv_buf));
                recv_num = recv(events[i].data.fd, recv_buf, sizeof(recv_buf), 0);
                if (recv_num <= 0) 
                {
                    close(events[i].data.fd);
                    events[i].data.fd = -1;
                } 

                ev.data.fd = events[i].data.fd;
                ev.events = EPOLLOUT | EPOLLET;
                epoll_ctl(epfd,EPOLL_CTL_MOD,events[i].data.fd,&ev);
            }
            else if(events[i].events & EPOLLOUT) 
            {
                bzero(send_buf, sizeof(send_buf));
                //sprintf(send_buf, "server proc got %d bytes\n", recv_num);
                int http_res_size = make_http_res(send_buf, recv_buf);
                send_num = send(events[i].data.fd, send_buf, http_res_size, 0);
                if (send_num <= 0) 
                {
                    close(events[i].data.fd);
                    events[i].data.fd = -1;
                }

                ev.data.fd = events[i].data.fd;
                ev.events = EPOLLIN | EPOLLET;
                epoll_ctl(epfd,EPOLL_CTL_MOD,events[i].data.fd,&ev);
            }
        } // End for
    } // End while

    close(sock_fd);
    return 0;
}

