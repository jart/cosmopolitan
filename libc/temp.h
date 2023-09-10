#ifndef COSMOPOLITAN_LIBC_TEMP_H_
#define COSMOPOLITAN_LIBC_TEMP_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

char *mktemp(char *) returnsnonnull paramsnonnull();
char *mkdtemp(char *) paramsnonnull() __wur;
int mkstemp(char *) paramsnonnull() __wur;
int mkstemps(char *, int) paramsnonnull() __wur;

#if defined(_GNU_SOURCE) || defined(_COSMO_SOURCE)
int mkostemp(char *, unsigned) paramsnonnull() __wur;
int mkostemps(char *, int, unsigned) paramsnonnull() __wur;
#endif

#ifdef _COSMO_SOURCE
int openatemp(int, char *, int, int, int) paramsnonnull() __wur;
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_TEMP_H_ */
