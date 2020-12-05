#ifndef COSMOPOLITAN_LIBC_LOG_BSD_H_
#define COSMOPOLITAN_LIBC_LOG_BSD_H_
#include "libc/fmt/pflink.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § logging » berkeley logger                                 ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

void err(int, const char *, ...) printfesque(2) wontreturn;
void errx(int, const char *, ...) printfesque(2) wontreturn;
void verr(int, const char *, va_list) paramsnonnull((3)) wontreturn;
void verrx(int, const char *, va_list) paramsnonnull((3)) wontreturn;
void vwarn(const char *, va_list) paramsnonnull((2));
void vwarnx(const char *, va_list) paramsnonnull((2));
void warn(const char *, ...) printfesque(1);
void warnx(const char *, ...) printfesque(1);

#define err(EVAL, FMT, ...)  (err)(EVAL, PFLINK(FMT), ##__VA_ARGS__)
#define errx(EVAL, FMT, ...) (errx)(EVAL, PFLINK(FMT), ##__VA_ARGS__)
#define verr(EVAL, FMT, VA)  (verr)(EVAL, PFLINK(FMT), VA)
#define verrx(EVAL, FMT, VA) (verrx)(EVAL, PFLINK(FMT), VA)
#define vwarn(FMT, VA)       (vwarn)(PFLINK(FMT), VA)
#define vwarnx(FMT, VA)      (vwarnx)(PFLINK(FMT), VA)
#define warn(FMT, ...)       (warn)(PFLINK(FMT), ##__VA_ARGS__)
#define warnx(FMT, ...)      (warn)(PFLINK(FMT), ##__VA_ARGS__)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_LOG_BSD_H_ */
