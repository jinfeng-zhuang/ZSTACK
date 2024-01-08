#ifndef ZSTACK_TIME
#define ZSTACK_TIME

#include "zstack/types.h"

struct time {
    U32 year;
    U32 month;
    U32 day;
    U32 hour;
    U32 min;
    U32 sec;
};

extern void msleep(U64 ms);

extern U64 timestamp_us(void);
extern U64 timestamp_ms(void);
extern U64 timestamp_sec(void);

extern void udelay(U64 us);
extern void mdelay(U64 ms);

extern void get_time(struct time *dt);



#endif
