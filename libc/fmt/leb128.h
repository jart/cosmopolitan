#ifndef COSMOPOLITAN_LIBC_FMT_LEB128_H_
#define COSMOPOLITAN_LIBC_FMT_LEB128_H_
#ifdef _COSMO_SOURCE
COSMOPOLITAN_C_START_

#define sleb64   __sleb64
#define zleb64   __zleb64
#define uleb64   __uleb64
#define unzleb64 __unzleb64
#define unuleb64 __unuleb64

char *sleb64(char *, int64_t) libcesque;
char *zleb64(char[hasatleast 10], int64_t) libcesque;
char *uleb64(char[hasatleast 10], uint64_t) libcesque;
int unzleb64(const char *, size_t, int64_t *) libcesque;
int unuleb64(const char *, size_t, uint64_t *) libcesque;

COSMOPOLITAN_C_END_
#endif /* _COSMO_SOURCE */
#endif /* COSMOPOLITAN_LIBC_FMT_LEB128_H_ */
