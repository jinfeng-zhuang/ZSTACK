#ifndef _ZSTACK_TCP_H_
#define _ZSTACK_TCP_H_

#include "zstack/types.h"

BOOL tcp_bind(U16 port, U32 *handle);

BOOL tcp_connect(U8 *host, U16 port, U32 *handle);

BOOL tcp_send(U32 handle, U8 *buffer, U32 length);

BOOL tcp_recv(U32 handle, U8 *buffer, U32 length, U32 *recvd);

BOOL tcp_close(U32 handle);

#endif