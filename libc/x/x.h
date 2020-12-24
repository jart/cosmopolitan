#ifndef COSMOPOLITAN_LIBC_X_H_
#define COSMOPOLITAN_LIBC_X_H_
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/fmt/pflink.h"
#include "libc/stdio/stdio.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § eXtended apis                                             ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│─╝
  Standard Library veneers for folks not building embedded RTOS */

#define _XPNN   paramsnonnull()
#define _XRET   nothrow nocallback nodiscard returnsnonnull
#define _XMAL   returnspointerwithnoaliases _XRET
#define _XMALPG returnsaligned((PAGESIZE)) _XMAL

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § eXtended apis » system calls                              ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int xsigaction(int, void *, uint64_t, uint64_t, struct sigaction *);
int xwrite(int, const void *, uint64_t);

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § eXtended apis » memory                                    ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

void xdie(void) wontreturn;
char *xdtoa(long double) _XMAL;
char *xasprintf(const char *, ...) printfesque(1) paramsnonnull((1)) _XMAL;
char *xvasprintf(const char *, va_list) _XPNN _XMAL;
char *xgetline(struct FILE *) _XPNN mallocesque;
void *xmalloc(size_t) attributeallocsize((1)) _XMAL;
void *xrealloc(void *, size_t) attributeallocsize((2)) _XRET;
void *xcalloc(size_t, size_t) attributeallocsize((1, 2)) _XMAL;
void *xvalloc(size_t) attributeallocsize((1)) _XMALPG;
void *xmemalign(size_t, size_t) attributeallocalign((1))
    attributeallocsize((2)) _XMAL;
void *xmemalignzero(size_t, size_t) attributeallocalign((1))
    attributeallocsize((2)) _XMAL;
char *xstrdup(const char *) _XPNN _XMAL;
char *xstrndup(const char *, size_t) _XPNN _XMAL;
char *xstrcat(const char *, ...) paramsnonnull((1)) nullterminated() _XMAL;
char *xstrmul(const char *, size_t) paramsnonnull((1)) _XMAL;
char *xdirname(const char *) paramsnonnull() _XMAL;
char *xjoinpaths(const char *, const char *) paramsnonnull() _XMAL;
char *xinet_ntop(int, const void *) _XPNN _XMAL;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § eXtended apis » time                                      ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

char *xiso8601i(int) mallocesque;
char *xiso8601tv(struct timeval *) mallocesque;
char *xiso8601ts(struct timespec *) mallocesque;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § eXtended apis » input / output                            ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

char *xslurp(const char *, size_t *) paramsnonnull((1)) _XMALPG nodiscard;
int xbarf(const char *, const void *, size_t);

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § eXtended apis » safety                                    ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define xstrcat(...) (xstrcat)(__VA_ARGS__, NULL)

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § eXtended apis » generic typing                            ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#if __STDC_VERSION__ + 0 >= 201112

#define xiso8601(TS) \
  _Generic(*(TS), struct timeval : xiso8601tv, default : xiso8601ts)(TS)

#endif /* C11 */

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § eXtended apis » link-time optimizations                   ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define xasprintf(FMT, ...) (xasprintf)(PFLINK(FMT), ##__VA_ARGS__)
#define xvasprintf(FMT, VA) (xvasprintf)(PFLINK(FMT), VA)
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_X_H_ */
