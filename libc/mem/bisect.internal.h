#ifndef COSMOPOLITAN_LIBC_ALG_BISECT_H_
#define COSMOPOLITAN_LIBC_ALG_BISECT_H_
COSMOPOLITAN_C_START_

forceinline void *bisect(const void *k, const void *data, size_t n, size_t size,
                         int cmp(const void *a, const void *b, void *arg),
                         void *arg) {
  int c;
  const char *p;
  ssize_t m, l, r;
  if (n) {
    l = 0;
    r = n - 1;
    p = data;
    while (l <= r) {
      m = (l & r) + ((l ^ r) >> 1);
      c = cmp(k, p + m * size, arg);
      if (c > 0) {
        l = m + 1;
      } else if (c < 0) {
        r = m - 1;
      } else {
        return (char *)p + m * size;
      }
    }
  }
  return NULL;
}

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_ALG_BISECT_H_ */
