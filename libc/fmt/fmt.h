#ifndef COSMOPOLITAN_LIBC_FMT_FMT_H_
#define COSMOPOLITAN_LIBC_FMT_FMT_H_
#include "libc/fmt/pflink.h"
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

int snprintf(char *, size_t, const char *, ...) printfesque(3)
    paramsnonnull((3)) nothrow nocallback;
int vsnprintf(char *, size_t, const char *, va_list)
    paramsnonnull((3)) nothrow nocallback;
int sprintf(char *, const char *, ...) printfesque(2)
    paramsnonnull((2)) nothrow nocallback frownedupon(snprintf);
int vsprintf(char *, const char *, va_list)
    paramsnonnull((2)) nothrow nocallback frownedupon(vsnprintf);
int sscanf(const char *, const char *, ...) scanfesque(2);
int vsscanf(const char *, const char *, va_list);
int vcscanf(int (*)(void *), int (*)(int, void *), void *, const char *,
            va_list);
char *strerror(int) returnsnonnull nothrow nocallback;
int strerror_r(int, char *, size_t) nothrow nocallback;
int palandprintf(void *, void *, const char *, va_list) hidden;
char *itoa(int, char *, int) compatfn;
char *fcvt(double, int, int *, int *);
char *ecvt(double, int, int *, int *);

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § string formatting » optimizations                         ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define sprintf(BUF, FMT, ...)        (sprintf)(BUF, PFLINK(FMT), ##__VA_ARGS__)
#define vsprintf(BUF, FMT, VA)        (vsprintf)(BUF, PFLINK(FMT), VA)
#define snprintf(B, Z, F, ...)        (snprintf)(B, Z, PFLINK(F), ##__VA_ARGS__)
#define vsnprintf(BUF, SIZE, FMT, VA) (vsnprintf)(BUF, SIZE, PFLINK(FMT), VA)
#define sscanf(STR, FMT, ...)         (sscanf)(STR, SFLINK(FMT), ##__VA_ARGS__)
#define vsscanf(STR, FMT, VA)         (vsscanf)(STR, SFLINK(FMT), VA)
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_FMT_FMT_H_ */
