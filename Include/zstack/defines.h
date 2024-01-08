#ifndef _DEFINES_H_
#define _DEFINES_H_

#include <stdio.h>

#ifndef TRUE
#define TRUE (1)
#endif

#ifndef FALSE
#define FALSE (0)
#endif

#define SECOND  (1000*1000)
#define MSEC    (1000)
#define USEC    (1)

enum RETURN_TYPES
{
	RET_OK = 0,
	RET_FAIL,
	RET_PARAM,
	RET_MEM,
	RET_IO,
	RET_TIMEOUT,
};

#define TRACE(x, ...) printf("%s %d: %s\n", __func__, __LINE__, x)

#endif