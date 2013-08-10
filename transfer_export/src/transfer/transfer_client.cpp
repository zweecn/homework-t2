#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include "transfer.h"

static int Connect2Server(const char *ipv4_addr,
				     const uint16_t port,
					 int *sockfd)
{
	struct sockaddr_in servaddr;

	if ( (*sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Socket error: %s\n", strerror(errno));
		return -1;
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	if (inet_pton(AF_INET, ipv4_addr, &servaddr.sin_addr) <= 0) {
		fprintf(stderr, "Connect error: %s\n", strerror(errno));
		return -1;
	}

	if (connect(*sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
		fprintf(stderr, "Connect to server failed: %s\n", strerror(errno));
		return -1;
	}

	return 0;
}

#if 0
////////////////////////////////////////////////////////////////////////////////
// 为每次发送建立一个新的TCP连接
// 可重入
int SendData(const char *buffer, int len)
{
    int socket_fd = -1;

	if (!buffer) {
		fprintf(stderr, "Invalid pointer in %s:%d\n", __FILE__, __LINE__);
		return -1;
	}

	if (0 == len) {
		fprintf(stdout, "Warning: attempt to send zero byte!\n");
		return 0;
	}

    err = Connect2Server(STORAGE_SERVER_IP, STORAGE_SERVER_PORT, &sockfd);
    if (err) {
        fprintf(stderr, "Connect to storage server failed: %s\n",
            strerror(errno));
        return -1;
    }

    write(socket_fd, buffer, len);
    close(socket_fd);
}
#endif


////////////////////////////////////////////////////////////////////////////////
// 不可重入，线程安全？
static int socket_fd = -1;
static void sig_alarm(int signal)
{
    //超时关闭TCP连接
    close(socket_fd);
    socket_fd = -1;
}

int SendData(const char *buffer, int len)
{
    struct sigaction act;
	int err = 0;

    if (socket_fd < 0) {
        err = Connect2Server(STORAGE_SERVER_IP, STORAGE_SERVER_PORT, &socket_fd);
        if (err) {
            fprintf(stderr, "Connect to storage server failed: %s\n",
                strerror(errno));
            return -1;
        }
    }

    write(socket_fd, buffer, len);
    alarm(60); //1分钟超时自动关闭连接
    act.sa_handler = sig_alarm;
    act.sa_flags = 0;
    sigaction(SIGALRM, &act, NULL);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// 线程安全版本
