#ifndef __TRANSFER_CLIENT_H
#define __TRANSFER_CLIENT_H

/**
 * @brief Send data to storage server
 * @param[in] buffer  data to be sent
 * @param[in] len	data length
 * @notice  thread-unsafe
 */
extern int SendData(const char *buffer, int len);

#endif
