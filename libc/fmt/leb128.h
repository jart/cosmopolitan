#ifndef COSMOPOLITAN_LIBC_FMT_LEB128_H_
#define COSMOPOLITAN_LIBC_FMT_LEB128_H_
COSMOPOLITAN_C_START_

char *sleb64(char *, int64_t) libcesque;
char *zleb64(char[hasatleast 9], int64_t) libcesque;
char *uleb64(char[hasatleast 9], uint64_t) libcesque;
int unzleb64(const char *, size_t, int64_t *) libcesque;
int unuleb64(const char *, size_t, uint64_t *) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_FMT_LEB128_H_ */
