#ifndef COSMOPOLITAN_LIBC_UNISTD_H_
#define COSMOPOLITAN_LIBC_UNISTD_H_
COSMOPOLITAN_C_START_

#define _CS_PATH 0

size_t confstr(int, char *, size_t) libcesque;

#if defined(_XOPEN_SOURCE) || defined(_GNU_SOURCE) || defined(_BSD_SOURCE) || \
    defined(_COSMO_SOURCE)
void encrypt(char *, int) libcesque;
char *crypt(const char *, const char *) libcesque;
#endif

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_UNISTD_H_ */
