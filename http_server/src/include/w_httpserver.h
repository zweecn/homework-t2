#ifndef __W_HTTP_SERVER_H_
#define __W_HTTP_SERVER_H_ 


class W_HttpServer
{
public:
    explicit W_HttpServer();

    int get_file(char* file, const char* request);
    int make_header(char* buf, int max_size, int res_code, const char* ct_type, int ct_size);
    int make_filename(char* filename, const char* file);
    int read_content(char* buf, int max_size, const char* filename);
    int read_404(char* buf, int max_size);
    int make_body(char* dest, int max_size, 
            const char* hd_buf, int hd_size, const char* ct_buf, int ct_size);
private:
    int get_code(const char* filename);
};

#endif 


