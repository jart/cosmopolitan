#ifndef COSMOPOLITAN_LIBC_BSDSTDLIB_H_
#define COSMOPOLITAN_LIBC_BSDSTDLIB_H_
COSMOPOLITAN_C_START_

uint32_t arc4random(void);
void arc4random_buf(void *, size_t);
uint32_t arc4random_uniform(uint32_t);

void *reallocarray(void *, size_t, size_t);
long long strtonum(const char *, long long, long long, const char **);
int heapsort(void *, size_t, size_t, int (*)(const void *, const void *));
int mergesort(void *, size_t, size_t, int (*)(const void *, const void *));

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_BSDSTDLIB_H_ */
