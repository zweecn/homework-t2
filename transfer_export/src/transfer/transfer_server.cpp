#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include "transfer.h"
#include "w_datarecv.h"

static pthread_mutex_t mlock = PTHREAD_MUTEX_INITIALIZER;
static int listen_fd = -1;

static int start_server(int *listenfd)
{
	struct sockaddr_in servaddr;
	int err=0;
	int on;

	*listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (*listenfd < 0) {
		fprintf(stderr, "Socket error: %s\n", strerror(errno));
		return -1;
	}

	on = 1;
	setsockopt(*listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(TRANSFER_PORT);

	err = bind(*listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	if (err < 0) {
		fprintf(stderr, "Bind listenfd to socket failed: %s\n",
				strerror(errno));
		return -1;
	}

	err = listen(*listenfd, TS_MAX_ACCEPT_CONNECTION);
	if (err < 0) {
		fprintf(stderr, "Listen socket failed: %s\n", strerror(errno));
		return -1;
	}

	printf("transfer_server started! You can run the clients now.\n");
	return 0;
}

W_DataRecv data_rcv;

static void * thread_main(void *arg)
{
	int conn_fd=0;
    char buffer[PACKAGE_SIZE];
    uint16_t len = 0;
	//W_DataRecv data_rcv;
	bool err = false;
	size_t ret = 0;

    //extern int WriteData(const char *buf, int length);

	while (1) {
		pthread_mutex_lock(&mlock);
		conn_fd = accept(listen_fd, (struct sockaddr*)NULL, NULL);
		pthread_mutex_unlock(&mlock);
		if (conn_fd < 0) {
			fprintf(stderr, "Accept connection failed: %s\n",
					strerror(errno));
			exit(1);
		}

		while (1) {
        	ret = read(conn_fd, buffer, MSG_HEADER_LEN);
            if (0 == ret) {
                fprintf(stderr, "Reach end of connect socket!\n");
                close(conn_fd);
                break;
            }
            if (0 > ret) {
                fprintf(stderr, "Read conn_fd failed: %s\n", strerror(errno));
                break;
            }
        	/* Just check the length */
        	len = *(uint16_t*)(&buffer[MSG_HEADER_LEN-2]);
        	len = ntohs(len);
        	
        	if (len)
        	    read(conn_fd, buffer+MSG_HEADER_LEN, len);

			fprintf(stderr, "data_rcv.WriteData(len=%d+%d) wait....!\n",
					MSG_HEADER_LEN, len);
        	err = data_rcv.WriteData(buffer, MSG_HEADER_LEN+len);
			fprintf(stderr, "data_rcv.WriteData finished!\n");
			if (false == err) {
				fprintf(stderr, "WriteData to share memory failed!\n");
			}
		}
	}
}

static void sig_int(int signal)
{
    fprintf(stderr, "transfer_server exit!\n");
    exit(1);
}

int main()
{
	int err = 0;
    struct sigaction act;
    pthread_t tid[TS_N_THREADS];

	err = start_server(&listen_fd);
	if (err < 0) {
		fprintf(stderr, "Start server failed: %s\n", strerror(errno));
		exit(1);
	}

	//for (int i=0; i<TS_N_THREADS; i++)
		//pthread_create(&tid[i], NULL, &thread_main, (void*)NULL);
		pthread_create(&tid[0], NULL, &thread_main, (void*)NULL);

    act.sa_handler = sig_int;
    act.sa_flags = 0;
	sigaction(SIGINT, &act, NULL);

	pause();
}
