#ifndef COSMOPOLITAN_LIBC_RUNTIME_BUFFER_H_
#define COSMOPOLITAN_LIBC_RUNTIME_BUFFER_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct GuardedBuffer {
  void *p;
};

void *balloc(struct GuardedBuffer *, unsigned, size_t);
void bfree(struct GuardedBuffer *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_BUFFER_H_ */
