#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <asm/io.h>

#include "w_httpserver.h"


const char* wwwroot = "wwwroot";
const char* nofound = "wwwroot/404.html";

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

W_HttpServer::W_HttpServer()
{

}

int W_HttpServer::get_file(char* file, const char* request)
{
    sscanf(request, "%*s %s %*s", file);
    return 0;
}

int W_HttpServer::make_header(char* buf, int max_size, int res_code, const char* ct_type, int ct_size)
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

int W_HttpServer::make_filename(char* filename, const char* file)
{
    sprintf(filename, "%s%s", wwwroot, file);
    return 0;
}

//Bug
int W_HttpServer::get_code(const char* filename)
{
    printf("filename=%s\n", filename);
    if (access("index.html", 0))
        return 200;
    return 404;
}

int W_HttpServer::read_content(char* buf, int max_size, const char* filename)
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


int W_HttpServer::read_404(char* buf, int max_size)
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

int W_HttpServer::make_body(char* dest, int max_size, 
        const char* hd_buf, int hd_size, const char* ct_buf, int ct_size)
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

