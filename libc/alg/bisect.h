#ifndef COSMOPOLITAN_LIBC_ALG_BISECT_H_
#define COSMOPOLITAN_LIBC_ALG_BISECT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

forceinline void *bisect(const void *k, const void *data, size_t n, size_t size,
                         int cmp(const void *a, const void *b, void *arg),
                         void *arg) {
  int dir;
  const char *p, *pos;
  p = data;
  while (n > 0) {
    pos = p + size * (n / 2);
    dir = cmp(k, pos, arg);
    if (dir < 0) {
      n /= 2;
    } else if (dir > 0) {
      p = pos + size;
      n -= n / 2 + 1;
    } else {
      return (void *)pos;
    }
  }
  return NULL;
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ALG_BISECT_H_ */
