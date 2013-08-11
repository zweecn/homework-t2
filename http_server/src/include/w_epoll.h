#ifndef __W_EPOLL_H_
#define __W_EPOLL_H_

#ifdef __cplusplus
extern "C" {
#endif 

class W_Epoll
{
public:
    explicit W_Epoll(){}
    int run();

private:
    int make_http_res(char* res_buf, const char* req_buf);
    int setnonblocking(int sock);
    int create_and_bind();
};


#endif 

#ifdef __cplusplus
}
#endif 
