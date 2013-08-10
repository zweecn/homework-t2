/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

const int __buf_size = 1024;
const int __header_size = 256;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int make_header(char* buf, int max_size, int res_code, const char* ct_type, int ct_size)
{
    char tmp[256];
    memset(buf, 0, max_size);
    char *p = buf;

    memset(tmp, 0, sizeof(tmp));
    sprintf(tmp, "HTTP/1.1 %d OK\r\n", res_code);
    memcpy(p, tmp, strlen(tmp));
    p += strlen(tmp);

    memset(tmp, 0, sizeof(tmp));
    sprintf(tmp, "Server: http-server/0.1.0\r\n");
    memcpy(p, tmp, strlen(tmp));
    p += strlen(tmp);

    memset(tmp, 0, sizeof(tmp));
    sprintf(tmp, "Content-Type: %s\r\n", ct_type);
    memcpy(p, tmp, strlen(tmp));
    p += strlen(tmp);

    memset(tmp, 0, sizeof(tmp));
    sprintf(tmp, "Content-Length: %d\r\n\r\n", ct_size);
    memcpy(p, tmp, strlen(tmp));
    p += strlen(tmp);

    return (p - buf);
}

int read_content(char* buf, int max_size, const char* file)
{
    memset(buf, 0, max_size);
    FILE* fp = fopen(file, "rb");
    if (fp == NULL)
    {
        error("File open failed.");
    }
    int cnt = fread(buf, sizeof(char), max_size, fp);
    fclose(fp);
    return cnt;
}

int make_body(char* dest, int max_size, const char* hd_buf, int hd_size, const char* ct_buf, int ct_size)
{
    if (hd_size + ct_size > max_size)
    {
        error("Out of range");
    }
    memset(dest, 0, max_size);
    memcpy(dest, hd_buf, hd_size);
    memcpy(dest + hd_size, ct_buf, ct_size);
    return hd_size + ct_size;
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[__buf_size];

    char hd_buf[__header_size];
    char ct_buf[__buf_size];
    char res_buf[__buf_size]; 

    struct sockaddr_in serv_addr, cli_addr;
    int n;
    if (argc < 2) 
    {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        error("ERROR opening socket");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
    {
        error("ERROR on binding");
    }
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    while (1)
    {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) 
            error("ERROR on accept");
        bzero(buffer, __buf_size);
        n = read(newsockfd,buffer, __buf_size - 1);
        if (n < 0) error("ERROR reading from socket");
        printf("Here is the message: \n%s\n",buffer);

        int ct_size = read_content(ct_buf, __buf_size, "index.html");
        int hd_size = make_header(hd_buf, __buf_size, 200, "text/heml", ct_size);
        int res_size = make_body(res_buf, __buf_size, hd_buf, hd_size, ct_buf, ct_size); 

        //n = write(newsockfd,"I got your message",18);
        n = write(newsockfd, res_buf, res_size);
        if (n < 0) error("ERROR writing to socket");
    }
    close(newsockfd);
    close(sockfd);
    return 0; 
}

