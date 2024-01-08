#ifndef SERIAL_H
#define SERIAL_H

#include "zstack/types.h"

void serial_port_list(void);

// TODO: return result
U32 serial_open(U8 id);

// TODO: fd->index
U32 serial_read(U32 fd, U8 *buffer, U32 length, U32 timeout);
U32 serial_write(U32 fd, U8 *buffer, U32 length);

U32 serial_close(U8 index);

#endif