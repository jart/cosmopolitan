#ifndef COSMOPOLITAN_LIBC_STDIO_ICONV_H_
#define COSMOPOLITAN_LIBC_STDIO_ICONV_H_
COSMOPOLITAN_C_START_

typedef void *iconv_t;

iconv_t iconv_open(const char *, const char *);
size_t iconv(iconv_t, char **, size_t *, char **, size_t *);
int iconv_close(iconv_t);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_STDIO_ICONV_H_ */
