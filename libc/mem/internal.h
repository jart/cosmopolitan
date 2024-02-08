#ifndef COSMOPOLITAN_LIBC_MEM_INTERNAL_H_
#define COSMOPOLITAN_LIBC_MEM_INTERNAL_H_
COSMOPOLITAN_C_START_

struct CritbitNode {
  void *child[2];
  uint32_t byte;
  unsigned otherbits;
};

int __putenv(char *, bool) libcesque;
bool __grow(void *, size_t *, size_t, size_t) paramsnonnull((1, 2)) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_MEM_INTERNAL_H_ */
