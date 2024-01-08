#include <time.h>

#include <Windows.h>

#include "zstack/types.h"
#include "time.h"

#if 0
void get_time(struct time *t) {
  time_t tt;
  struct tm *lt;

  tt = time(NULL);

  lt = localtime(&tt);

  t->year = lt->tm_year + 1900;
  t->month = lt->tm_mon + 1;
  t->day = lt->tm_mday;
  t->hour = lt->tm_hour;
  t->min = lt->tm_min;
  t->sec = lt->tm_sec;
}
#endif

U64 get_ms(void)
{
  return GetTickCount64();
}

void msleep(U64 ms)
{
  Sleep((U32)ms);
}

U32 hr_timer_freq(void) {
  LARGE_INTEGER freq;

  QueryPerformanceFrequency(&freq);

  return (U32)freq.QuadPart;
}

U64 hr_timer_counter(void) {
  LARGE_INTEGER counter;

  QueryPerformanceCounter(&counter);

  return (U64)counter.QuadPart;
}

U64 timestamp_us(void)
{
    LARGE_INTEGER freq;
    LARGE_INTEGER counter;
    F64 result;
    
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    
    result = (F64)counter.QuadPart / freq.QuadPart * 1000 * 1000;

    return (U64)result;
}

void udelay(U64 us)
{
    U64 start;
    //U64 end;

    start = timestamp_us();

    while ((timestamp_us() - start) < us);

    //end = timestamp();
    //printf("end - start = %lld\n", end - start);
}

void mdelay(U64 ms)
{
    udelay(ms * 1000);
}
