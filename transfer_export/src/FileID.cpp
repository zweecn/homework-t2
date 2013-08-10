#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

#include "FileID.h"

#ifdef TRANSFER_DEBUG
#include <assert.h>
#else
#define assert() do{}while(0)
#endif

FileID::FileID()
{
    memset(sha1sum, 0, 20);
}

FileID::FileID(const char *sum)
{
    assert(sum);

    memcpy(sha1sum, sum, 20);
}

FileID::~FileID(void)
{
}

int FileID::Dump2Net(char *ptr, int *len) const
{
    if (NULL == ptr)
        return -1;
    if (NULL == len)
        return -2;

    memcpy(ptr, sha1sum, 20);
    memset(ptr+20, 0, 20);
    *len = 40;
    return 0;
}

/* 动态分配内存，由调用者释放 */
char * FileID::GenFileName(void) const
{
    char *fname = new char[41];

    for (int i=0; i<20; i++)
        sprintf(&fname[2*i], "%02x", sha1sum[i]);

    fname[40] = '\0';

    return fname;
}

uint32_t FileID::Hash2Uint32() const
{
    uint32_t result = 0;

    result = *(uint32_t*)(&sha1sum[16]);

    return result;
}

bool FileID::operator ==(const FileID &id) const
{
    if (memcmp(sha1sum, id.sha1sum, 20) == 0)
        return true;
    return false;
}

