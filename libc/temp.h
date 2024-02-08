#ifndef COSMOPOLITAN_LIBC_TEMP_H_
#define COSMOPOLITAN_LIBC_TEMP_H_
COSMOPOLITAN_C_START_

char *mktemp(char *) libcesque returnsnonnull paramsnonnull();
char *mkdtemp(char *) libcesque paramsnonnull() __wur;
int mkstemp(char *) libcesque paramsnonnull() __wur;
int mkstemps(char *, int) libcesque paramsnonnull() __wur;

#if defined(_GNU_SOURCE) || defined(_COSMO_SOURCE)
int mkostemp(char *, unsigned) libcesque paramsnonnull() __wur;
int mkostemps(char *, int, unsigned) libcesque paramsnonnull() __wur;
#endif

#ifdef _COSMO_SOURCE
int openatemp(int, char *, int, int, int) libcesque paramsnonnull() __wur;
#endif

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_TEMP_H_ */
