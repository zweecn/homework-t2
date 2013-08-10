#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "transfer.h"
#include "FileID.h"

#ifdef TRANSFER_DEBUG
#include <assert.h>
#else
#define assert() do{}while(0)
#endif


void Usage(const char *progname)
{
    fprintf(stderr, "%s file_name\n", progname);
    return ;
}

int CheckFile(const char *file_path)
{
    int err=0;
    
    err = access(file_path, R_OK);
    if (err) {
        fprintf(stderr, "File access error: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

int Connect2Server(const char *ipv4_addr,
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

/* Generate a near-unique file identity */
int GenerateFileID(const char *file_path, FileID **p_id)
{
    char *command = NULL;
    char buf[40], sha1sum[20];
    FILE *fp = NULL;

    assert(file_path);

    command = new char[strlen("sha1sum ") + strlen(file_path)+1];
    if (NULL == command) {
        fprintf(stderr, "Out of memory!\n");
        exit(1);
    }

    /* Use tool sha1sum */
    sprintf(command, "sha1sum %s", file_path);
    fp = popen(command, "r");
    fread(buf, 1, 40, fp);
    pclose(fp);

    /* convert to binary */
    for (int i=0; i<20; i++) {
        sha1sum[i] = buf[2*i]>'9' ? buf[2*i]-'a'+10 : buf[2*i]-'0';
        sha1sum[i] = (sha1sum[i]<<4) | (buf[2*i+1]>'9' ? buf[2*i+1]-'a'+10 : buf[2*i+1]-'0');
    }

    if (command)
        delete [] command;

    *p_id = new FileID(sha1sum);
    return 0;
}

static int SendFileHeader(int sockfd, const char *file_path, FileID **p_id)
{
    char buf[512];
    char *ptr = NULL;
    int len=0, err=0;
    struct stat sb;

    assert(file_path);
    assert(p_id);
    assert(!*p_id);
    assert(512 >= 1+sizeof(FileID)+2+strlen(file_path)+8);

    err = stat(file_path, &sb);
    if (err) {
        fprintf(stderr, "Stat file \"%s\" failed: %s\n",
            file_path, strerror(errno));
        return -1;
    }

    ptr = buf;
    *ptr++ = MSG_NEW_FILE;
    err = GenerateFileID(file_path, p_id);
    if (err) {
        return -1;
    }
    //memcpy((void*)ptr, (void*)id, sizeof(FileID));
    (**p_id).Dump2Net(ptr, &len);
    ptr += len;
    *(uint64_t*)ptr = htobe64((uint64_t)sb.st_size);
    ptr += 8;

    len = strlen(file_path);
    *(uint16_t*)ptr = htons((uint16_t)len);
    ptr += 2;
    strncpy(ptr, file_path, len);
    ptr += len;    

    write(sockfd, buf, ptr-buf);

    return 0;
}

static int SendFileData(int sockfd, const char *file_path, const FileID *id)
{
    char buf[PACKAGE_SIZE];
    char *ptr = NULL;
    int len=0, err=0, nread=0;
    FILE *fp = NULL;
    off_t offset = 0;

    assert(file_path);
    assert(id);

    fp = fopen(file_path, "r");
    if (NULL == fp) {
        fprintf(stderr, "Open file \"%s\" to read failed: %s\n",
            file_path, strerror(errno));
        return -1;
    }

    buf[0] = MSG_FILE_DATA;
    err = (*id).Dump2Net(&buf[1], &len);
    if (err) {
        fprintf(stderr, "Dump failed!\n");
        return -1;
    }
    ptr = buf + 1 + len;

    len = PACKAGE_SIZE - (ptr-buf) - 10;

    do {
        *(uint64_t*)ptr = htobe64(offset);
        nread = fread(ptr+10, 1, len, fp);
        offset += nread;
        *(uint16_t*)(ptr+8) = htons(nread);
        write(sockfd, buf, (ptr-buf)+10+nread);
    } while (!feof(fp));

    fclose(fp);

    return 0;
}

static int SendFileEnd(int sockfd, const char *file_path, const FileID *id)
{
    char buf[512];
    char *ptr = NULL;
    int len = 0;

    assert(512 >= 1+sizeof(*id));

    ptr = buf;
    *ptr++ = MSG_FILE_END;
    (*id).Dump2Net(ptr, &len);
    ptr += len;

    // Ìí¼ÓÁã×Ö¶Î
    memset(ptr, 0, 10);
    ptr += 10;

    write(sockfd, buf, ptr-buf);

    return 0;
}

int SendFile(const char *file_path)
{
    int sockfd;
    FileID *file_id = NULL;
    int err = 0;

//    err = Connect2Server(STORAGE_SERVER_IP, STORAGE_SERVER_PORT, &sockfd);
    err = Connect2Server(TRANSFER_IP, TRANSFER_PORT, &sockfd);
    if (err) {
		fprintf(stderr, "Connect to %s:%d failed!\n",
				TRANSFER_IP, TRANSFER_PORT);
//				STORAGE_SERVER_IP, STORAGE_SERVER_PORT);
		return -1;
    }

    /* The idea process will be sending block and checksum at the same time.
       Now, checksum is calculated at first. */

    err = SendFileHeader(sockfd, file_path, &file_id);
    if (err) {
        fprintf(stderr, "Send file header failed!\n");
        exit(1);
    }

    err = SendFileData(sockfd, file_path, file_id);
    if (err) {
        fprintf(stderr, "Send file data failed!\n");
        exit(1);
    }

    err = SendFileEnd(sockfd, file_path, file_id);
    if (err) {
        fprintf(stderr, "Send file ender failed!\n");
        exit(1);
    }

    close(sockfd);

    return 0;
}

int main(int argc, char *argv[])
{
	int err=0;

    if (2 != argc) {
        Usage(argv[0]);
        exit(1);
    }

    /* check the accessibility of file */
    err = CheckFile(argv[1]);
    if (err) {
        exit(1);
    }

    /* send file */
    err = SendFile(argv[1]);
    if (err) {
        exit(1);
    }

	return 0;
}
