#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/epoll.h>
#include<fcntl.h>

#define MAX_LISTEN 5
#define PORT 8888
#define IP "127.0.0.1"
#define MAX_EVENTS 100

int setnonblocking(int sock)
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

int create_and_bind()
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
    addr_serv.sin_port = htons(PORT);
    addr_serv.sin_addr.s_addr = inet_addr(IP);

    if (bind(sock_fd,(struct sockaddr *)&addr_serv,sizeof(struct sockaddr_in)) < 0) 
    {
        perror("bind error");
        exit(1);
    }

    if (listen(sock_fd, MAX_LISTEN) < 0) 
    {
        perror("listen failed");
        exit(1);
    }
    return sock_fd;
}

int main()
{
    int sock_fd = create_and_bind();
    int recv_num, send_num;
    char recv_buf[100], send_buf[100];	

    struct sockaddr_in addr_client;
    socklen_t client_size = sizeof(struct sockaddr_in);

    //初始化epoll描述符
    int epfd = epoll_create(MAX_EVENTS);
    if (epfd <= 0) 
    {
        perror("create epoll failed!");
        exit(1);
    }
    int i, nready;
    struct epoll_event ev, events[MAX_EVENTS];

    //注册sock_fd描述符监听事件
    ev.data.fd = sock_fd;
    ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(epfd,EPOLL_CTL_ADD,sock_fd,&ev);

    while (1) 
    {
        nready = epoll_wait(epfd, events, MAX_EVENTS, 500);

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
                sprintf(send_buf, "server proc got %d bytes\n", recv_num);
                send_num = send(events[i].data.fd, send_buf, strlen(send_buf), 0);
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

