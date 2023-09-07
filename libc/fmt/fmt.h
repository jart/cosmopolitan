#ifndef COSMOPOLITAN_LIBC_FMT_FMT_H_
#define COSMOPOLITAN_LIBC_FMT_FMT_H_

#if __SIZEOF_POINTER__ == 8
#define POINTER_XDIGITS 12 /* math.log(2**48-1,16) */
#else
#define POINTER_XDIGITS 8
#endif

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

char *fcvt(double, int, int *, int *);
char *ecvt(double, int, int *, int *);
char *gcvt(double, int, char *);

#ifdef _COSMO_SOURCE
int __vcscanf(int (*)(void *), int (*)(int, void *), void *, const char *,
              va_list);
int __fmt(void *, void *, const char *, va_list);
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_FMT_FMT_H_ */
