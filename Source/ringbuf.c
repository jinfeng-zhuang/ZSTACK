#include <stdio.h>
#include <string.h>

#include "zstack/ringbuf.h"

//#define RINGBUF_DEBUG

#ifdef RINGBUF_DEBUG
static void ringbuf_dump(struct ringbuf* r)
{
  unsigned int mask = r->size - 1;
  printf("ringbuf: size %d rd %d wr %d\n", r->size, r->rd & mask, r->wr & mask);
}
#endif

static unsigned char is_power_of_two(unsigned int value)
{
    return 1; // TODO
}

unsigned int ringbuf_data_len(struct ringbuf *r) {
  return r->wr - r->rd;
}

unsigned int ringbuf_empty_len(struct ringbuf *r) {
  return r->size - (r->wr - r->rd);
}

float ringbuf_datalen_percent(struct ringbuf *r) {
  return ((float)ringbuf_data_len(r) / r->size) * 100;
}

static unsigned int _ringbuf_get_empty_length(struct ringbuf *r) {
  return (r->size - ringbuf_data_len(r));
}

static unsigned int _ringbuf_is_enough(struct ringbuf *r,
                                       unsigned int data_to_put) {
  return (data_to_put <= _ringbuf_get_empty_length(r));
}

// TODO: make sure the length = 2^n
int ringbuf_init(struct ringbuf *r, unsigned char* buf, unsigned int len)
{
  // TODO: to wrap easy, e.g. mask = 0x1FFFFF
  if (0 ==is_power_of_two(len)) {
    return -1;
  }

  r->size = len;
  r->rd = 0;
  r->wr = 0;
  r->buf = buf;
  
  return 0;
}

int ringbuf_reset(struct ringbuf* r)
{
  r->rd = 0;
  r->wr = 0;

  return 0;
}

unsigned int ringbuf_put(struct ringbuf* r, unsigned char* buf, unsigned int len)
{
  unsigned int mask = r->size - 1;
  unsigned int empty = r->size - (r->wr - r->rd);
  unsigned int wr, rd;

  if (len > empty) {
    return 0;
  }

  wr = r->wr & mask;
  rd = r->rd & mask;
  
  if (wr + len < r->size) {
    memcpy(&r->buf[wr], buf, len);
  }
  else {
    memcpy(&r->buf[wr], buf, r->size - wr);
    memcpy(r->buf, &buf[r->size - wr], len - (r->size - wr));
  }

  r->wr += len;

#ifdef RINGBUF_DEBUG
  ringbuf_dump(r);
#endif

  return len;
}

unsigned int ringbuf_get(struct ringbuf* r, unsigned char* buf, unsigned int len)
{
  unsigned int mask = r->size - 1;
  unsigned int remain = r->wr - r->rd;
  unsigned int reallen;
  unsigned int wr, rd;
  
  wr = r->wr & mask;
  rd = r->rd & mask;
  reallen = len > remain ? remain : len;

  if (rd + reallen < r->size) {
    memcpy(buf, &r->buf[rd], reallen);
  }
  else {
    memcpy(buf, &r->buf[rd], r->size - rd);
    memcpy(&buf[r->size - rd], r->buf, reallen - (r->size - rd));
  }

  r->rd += reallen;

#ifdef RINGBUF_DEBUG
  ringbuf_dump(r);
#endif

  return reallen;
}

