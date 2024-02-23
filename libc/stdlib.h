#ifndef COSMOPOLITAN_LIBC_STDLIB_H_
#define COSMOPOLITAN_LIBC_STDLIB_H_
COSMOPOLITAN_C_START_

char *fcvt(double, int, int *, int *) libcesque;
char *ecvt(double, int, int *, int *) libcesque;
char *gcvt(double, int, char *) libcesque;

#if defined(_XOPEN_SOURCE) || defined(_GNU_SOURCE) || defined(_BSD_SOURCE) || \
    defined(_COSMO_SOURCE)
void setkey(const char *) libcesque;
#endif

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_STDLIB_H_ */
