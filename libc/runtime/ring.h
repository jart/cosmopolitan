#ifndef COSMOPOLITAN_LIBC_RUNTIME_RING_H_
#define COSMOPOLITAN_LIBC_RUNTIME_RING_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct RingBuffer {
  void *p;
  char *_addr;
  size_t _size;
};

void *ringalloc(struct RingBuffer *, size_t);
int ringfree(struct RingBuffer *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_RING_H_ */
