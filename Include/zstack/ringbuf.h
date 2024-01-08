//
// if buffer is NULL, just update wp, rp
//

#ifndef ZSTACK_RINGBUF_H
#define ZSTACK_RINGBUF_H

struct ringbuf {
	unsigned char *buf;
	unsigned int size;
	unsigned int wr;
	unsigned int rd;
};

extern int ringbuf_init(struct ringbuf *r, unsigned char* buf, unsigned int len);

// if put data overflow, ignore the total put behaviour
extern unsigned int ringbuf_put(struct ringbuf* r, unsigned char* buf, unsigned int len);
extern unsigned int ringbuf_get(struct ringbuf* r, unsigned char* buf, unsigned int len);

extern float ringbuf_datalen_percent(struct ringbuf *r);
extern unsigned int ringbuf_data_len(struct ringbuf* r);
extern int ringbuf_reset(struct ringbuf *r);
extern unsigned int ringbuf_empty_len(struct ringbuf *r);

#endif
