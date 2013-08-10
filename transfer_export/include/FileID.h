#ifndef __FILE_ID
#define __FILE_ID

#include <stdint.h>

class FileID
{
private:
    unsigned char sha1sum[20];
public:
    FileID();
    FileID(const char *sum);
    ~FileID(void);
    int Dump2Net(char *ptr, int *len) const;
    char * GenFileName(void) const;
    uint32_t Hash2Uint32() const;
    bool operator == (const FileID &id) const;
};

#endif

