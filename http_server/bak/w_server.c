#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <asm/io.h>

const int __buf_size = 1024;
const int __header_size = 256;

const char* wwwroot = "wwwroot";
const char* nofound = "wwwroot/404.html";

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int get_file(char* file, const char* request)
{
    sscanf(request, "%*s %s %*s", file);
    return 0;
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

int make_filename(char* filename, const char* file)
{
    sprintf(filename, "%s%s", wwwroot, file);
    return 0;
}

int get_code(const char* filename)
{
    printf("filename=%s\n", filename);
    if (access("index.html", 0))
        return 200;
    return 404;
}

int read_content(char* buf, int max_size, const char* filename)
{
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        printf("File not found\n");
        return -1;
    }
    int cnt = fread(buf, sizeof(char), max_size, fp);
    fclose(fp);
    return cnt;
}


int read_404(char* buf, int max_size)
{
    FILE* fp = fopen(nofound, "rb"); 
    if (fp == NULL)
    {
        printf("File not found\n");
        return -1;
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
    if (argc < 2) 
    {
        fprintf(stderr,"usage:\n %s port\n", argv[0]);
        exit(1);
    }

    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[__buf_size];

    char hd_buf[__header_size];
    char ct_buf[__buf_size];
    char ct_buf_404[__buf_size];
    char res_buf[__buf_size]; 

    char file[__buf_size];
    char filename[__buf_size];

    int ct_size_404 = read_404(ct_buf_404, __buf_size);

    struct sockaddr_in serv_addr, cli_addr;
    int n;
    
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
        printf("%s\n",buffer);
        bzero(file, __buf_size);
        get_file(file, buffer);
        bzero(filename, __buf_size);
        make_filename(filename, file);

        int ct_size = read_content(ct_buf, __buf_size, filename);
        if (ct_size > 0)
        {
            int hd_size = make_header(hd_buf, __buf_size, 200, "text/html", ct_size);
            int res_size = make_body(res_buf, __buf_size, hd_buf, hd_size, ct_buf, ct_size); 
            n = write(newsockfd, res_buf, res_size);
        }
        else
        {
            int hd_size = make_header(hd_buf, __buf_size, 404, "text/html", ct_size);
            int res_size = make_body(res_buf, __buf_size, hd_buf, hd_size, ct_buf_404, ct_size_404); 
            n = write(newsockfd, res_buf, res_size);
        }
        if (n < 0) error("ERROR writing to socket");
    }
    close(newsockfd);
    close(sockfd);
    return 0; 
}

