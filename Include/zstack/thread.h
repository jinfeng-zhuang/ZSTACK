#ifndef _ZSTACK_THREAD_H_
#define _ZSTACK_THREAD_H_

#include "types.h"

typedef void (*thread_entry)(void *arg);

BOOL thread_create(thread_entry entry, void *arg);

BOOL mutex_init(U32 *handle);
void mutex_lock(U32 handle);
void mutex_unlock(U32 handle);

BOOL sema_init(U32 *handle);
void sema_post(U32 handle);
void sema_wait(U32 handle);

#endif