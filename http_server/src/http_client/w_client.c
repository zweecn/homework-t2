#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

const char* __ip = "127.0.0.1";
const int __port = 8888;
const int __buf_size = 1024;

int make_header(char* buf, int max_size, const char* file)
{
    char tmp[256];
    memset(buf, 0, max_size);
    char *p = buf;

    memset(tmp, 0, sizeof(tmp));
    sprintf(tmp, "GET %s HTTP/1.1\r\n", file);
    memcpy(p, tmp, strlen(tmp));
    p += strlen(tmp);

    memset(tmp, 0, sizeof(tmp));
    sprintf(tmp, "Host: localhost\r\n");
    memcpy(p, tmp, strlen(tmp));
    p += strlen(tmp);

    memset(tmp, 0, sizeof(tmp));
    sprintf(tmp, "Connection: Keep-Alive\r\n\r\n");
    memcpy(p, tmp, strlen(tmp));
    p += strlen(tmp);

    return (p - buf);
}


void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[__buf_size];

    char * file = "/index.html";
    if (argc == 1)
    {
        file = argv[1];
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        error("ERROR opening socket");
    }
    server = gethostbyname(__ip);
    if (server == NULL) 
    {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
            (char *)&serv_addr.sin_addr.s_addr,
            server->h_length);
    serv_addr.sin_port = htons(__port);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
    {
        error("ERROR connecting");
    }
    //printf("Please enter the message: ");
    bzero(buffer, __buf_size);
    //fgets(buffer, __buf_size - 1,stdin);

    int hd_size = make_header(buffer, __buf_size, file);
    n = write(sockfd, buffer, hd_size);
    if (n < 0) 
    {
        error("ERROR writing to socket");
    }
    bzero(buffer, __buf_size);
    n = read(sockfd,buffer, __buf_size - 1);
    if (n < 0) 
    {
        error("ERROR reading from socket");
    }
    printf("%s\n",buffer);
    close(sockfd);
    return 0;
}

