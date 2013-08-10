#ifndef __INTERLINE_H
#define __INTERLINE_H

#include <stdint.h>

/* port of storage server */
#define STORAGE_SERVER_PORT 4099
#define TRANSFER_PORT		10231

/* ipv4 address of storage server */
//#define STORAGE_SERVER_IP "192.168.16.131"
#define STORAGE_SERVER_IP "127.0.0.1"
#define TRANSFER_IP	STORAGE_SERVER_IP

/* max accept backlog */
#define MAX_ACCEPT_CONNECTION 5
#define TS_MAX_ACCEPT_CONNECTION 5

/* */
#define FILE_ID_LEN 40

/* max package length */
#define PACKAGE_SIZE 8192

/* max server pthreads */
#define SS_N_THREADS    5
#define TS_N_THREADS	5

#define MSG_NEW_FILE    'N'
#define MSG_FILE_DATA   'D'
#define MSG_FILE_END    'E'
#define MSG_HEADER_LEN  51

#if 1
#ifdef WORDS_BIGENDIAN
inline uint64_t htobe64(uint64_t host)
{
    return host;
}

inline uint64_t be64toh(uint64_t net)
{
    return net;
}
#else
inline uint64_t htobe64(uint64_t host)
{
    return htonl((uint32_t)(host>>32)) | (((uint64_t)htonl(host&0xffffffffUL))<<32);
}

inline uint64_t be64toh(uint64_t net)
{
    return ntohl((uint32_t)(net>>32)) | (((uint64_t)ntohl(net&0xffffffffUL))<<32);
}
#endif
#endif

#endif
