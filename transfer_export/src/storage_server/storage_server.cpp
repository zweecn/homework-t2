#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#ifdef TRANSFER_DEBUG
#include <assert.h>
#else
#define assert() do{}while(0)
#endif

#include "transfer.h"
#include "PendingFile.h"

static pthread_mutex_t mlock = PTHREAD_MUTEX_INITIALIZER;
static int listen_fd = -1;

int start_server(int *listenfd)
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
	servaddr.sin_port = htons(STORAGE_SERVER_PORT);

	err = bind(*listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	if (err < 0) {
		fprintf(stderr, "Bind listenfd to socket failed: %s\n",
				strerror(errno));
		return -1;
	}

	err = listen(*listenfd, MAX_ACCEPT_CONNECTION);
	if (err < 0) {
		fprintf(stderr, "Listen socket failed: %s\n", strerror(errno));
		return -1;
	}

	printf("Server started! You can run the transfer now.\n");
	return 0;
}


static int SS_GetFileHeader(int conn_fd)
{
    char sha1sum[20];
    char buf[PACKAGE_SIZE];
    int64_t file_size = -1;
    int fname_len = 0;
//    char *fname = NULL;
	int neglect = PACKAGE_SIZE-1;

    read(conn_fd, sha1sum, 20);
	neglect -= 20;
    read(conn_fd, buf, 20); //丢弃
	neglect -= 20;
    read(conn_fd, &file_size, 8);
	neglect -= 8;
    file_size = be64toh(file_size);

    read(conn_fd, &fname_len, 2);
	neglect -= 2;
    fname_len = ntohs(fname_len);
//    fname = new char[fname_len];
//    read(conn_fd, fname, fname_len);
//    delete fname;   //暂时不需要文件名
//    fname = NULL;

	read(conn_fd, buf, neglect);
    /* 设置文件大小 */
    FileID id(sha1sum);

    SS_SetPendingFileSize(id, file_size);
    return 0;
}

static int SS_CloseFile(int conn_fd)
{
    char sha1sum[20];
    char buf[PACKAGE_SIZE]; //20字节的预留空间，以及10个0字节

    read(conn_fd, sha1sum, 20);
    read(conn_fd, buf, PACKAGE_SIZE-1-30); //丢弃
    /* 设置文件大小 */
    FileID id(sha1sum);

    /* 找到文件，等待条件，关闭文件描述符 */
    SS_ClosePendingFile(id);
    return 0;
}

static int SS_GetFileData(int conn_fd)
{
    char sha1sum[20];
    char buf[20];
    uint64_t offset = 0;
    int data_len = 0, neglect=PACKAGE_SIZE-1;
    char buffer[PACKAGE_SIZE];

    read(conn_fd, sha1sum, 20);
	neglect -= 20;
    read(conn_fd, buf, 20); //丢弃
	neglect -= 20;
    /* 设置文件大小 */
    FileID id(sha1sum);

    read(conn_fd, &offset, 8);
	neglect -= 8;
    offset = be64toh(offset);

    read(conn_fd, &data_len, 2);
	neglect -= 2;
    data_len = ntohs(data_len);

    assert(data_len < PACKAGE_SIZE);
    read(conn_fd, buffer, data_len);
	neglect -= data_len;
    
    /* 找到文件，在相应位置写入数据，必要时打开 */
    SS_WriteData2PendingFile(id, buffer, offset, data_len);
	fprintf(stderr, "SS_WriteData2PendingFile(offset=%llu, data_len=%d)\n", (unsigned long long)offset, data_len);
    if (neglect)
		read(conn_fd, buffer, neglect); //neglect
    return 0;
}

void * thread_main(void *arg)
{
	int conn_fd=0;
    char msg;
    size_t ret = 0;

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
            ret = read(conn_fd, &msg, 1);

            if (0 == ret) {
                fprintf(stderr, "Reach end of connect socket!\n");
                close(conn_fd);
                break;
            }
            if (0 > ret) {
                fprintf(stderr, "Read conn_fd failed: %s\n", strerror(errno));
                break;
            }
            
ready:
            switch (msg) {
            case MSG_NEW_FILE:
                SS_GetFileHeader(conn_fd);
                break;
            case MSG_FILE_END:
                SS_CloseFile(conn_fd);
                break;
            case MSG_FILE_DATA:
                SS_GetFileData(conn_fd);
                break;
            default:
				while (msg != MSG_NEW_FILE || msg != MSG_FILE_DATA || msg != MSG_FILE_END)
					read(conn_fd, &msg, 1);
				goto ready;
                fprintf(stderr, "Unknown message type from client: 0x%x\n", msg);
                break;
            }
        }
	}
}

void sig_int(int signal)
{
    fprintf(stderr, "Server exit!\n");
    DeinitHashTable();
    exit(1);
}

int main()
{
	int err = 0;
    struct sigaction act;
    pthread_t tid[SS_N_THREADS];

	err = start_server(&listen_fd);
	if (err < 0) {
		fprintf(stderr, "Start server failed: %s\n", strerror(errno));
		exit(1);
	}

    InitHashTable();

	for (int i=0; i<SS_N_THREADS; i++)
		pthread_create(&tid[i], NULL, &thread_main, (void*)NULL);

    act.sa_handler = sig_int;
    act.sa_flags = 0;
	sigaction(SIGINT, &act, NULL);

	pause();
}
