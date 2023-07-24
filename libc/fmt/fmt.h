#ifndef COSMOPOLITAN_LIBC_FMT_FMT_H_
#define COSMOPOLITAN_LIBC_FMT_FMT_H_
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § string formatting                                         ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#if __SIZEOF_POINTER__ == 8
#define POINTER_XDIGITS 12 /* math.log(2**48-1,16) */
#else
#define POINTER_XDIGITS 8
#endif

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int snprintf(char *, size_t, const char *, ...)
    printfesque(3) dontthrow nocallback;
int vsnprintf(char *, size_t, const char *, va_list)
dontthrow nocallback;
int sprintf(char *, const char *, ...) printfesque(2) dontthrow nocallback;
int vsprintf(char *, const char *, va_list)
dontthrow nocallback;
int sscanf(const char *, const char *, ...) scanfesque(2);
int vsscanf(const char *, const char *, va_list);
char *fcvt(double, int, int *, int *);
char *ecvt(double, int, int *, int *);
char *gcvt(double, int, char *);

#ifdef COSMO
int __vcscanf(int (*)(void *), int (*)(int, void *), void *, const char *,
              va_list);
int __fmt(void *, void *, const char *, va_list);
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_FMT_FMT_H_ */
