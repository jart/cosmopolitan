#ifndef COSMOPOLITAN_LIBC_CALLS_STRACE_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRACE_INTERNAL_H_
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/stat.h"

#define _NT_RLIMIT_PWSS_MB 1000 /* nocommit */
#define _KERNTRACE         0    /* not configurable w/ flag yet */
#define _POLLTRACE         0    /* not configurable w/ flag yet */
#define _NTTRACE           0    /* not configurable w/ flag yet */

#define STRACE_PROLOGUE "%rSYS %5P %'18T "

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#ifdef SYSDEBUG
#define STRACE(FMT, ...)                                  \
  do {                                                    \
    if (__strace > 0) {                                   \
      __stracef(STRACE_PROLOGUE FMT "%n", ##__VA_ARGS__); \
    }                                                     \
  } while (0)
#else
#define STRACE(FMT, ...) (void)0
#endif

#if defined(SYSDEBUG) && _POLLTRACE
#define POLLTRACE(FMT, ...) STRACE(FMT, ##__VA_ARGS__)
#else
#define POLLTRACE(FMT, ...) (void)0
#endif

#if defined(SYSDEBUG) && _KERNTRACE
#define KERNTRACE(FMT, ...) STRACE(FMT, ##__VA_ARGS__)
#else
#define KERNTRACE(FMT, ...) (void)0
#endif

#if defined(SYSDEBUG) && _NTTRACE
#define NTTRACE(FMT, ...) STRACE(FMT, ##__VA_ARGS__)
#else
#define NTTRACE(FMT, ...) (void)0
#endif

extern int __strace;

void __stracef(const char *, ...);
const char *__strace_stat(int, const struct stat *);
const char *__strace_sigaction(char *, size_t, int, const struct sigaction *);
const char *__strace_sigset(char[41], size_t, int, const sigset_t *);
const char *__strace_rlimit_name(int);
const char *__strace_rlimit(char[41], size_t, int, const struct rlimit *);
const char *__strace_timespec(char[45], size_t, int, const struct timespec *);
const char *__strace_dirfd(char[12], int);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRACE_INTERNAL_H_ */
