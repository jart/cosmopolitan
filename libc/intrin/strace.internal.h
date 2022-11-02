#ifndef COSMOPOLITAN_LIBC_CALLS_STRACE_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRACE_INTERNAL_H_
#include "libc/intrin/likely.h"
#include "libc/runtime/runtime.h"

#define _KERNTRACE  0 /* not configurable w/ flag yet */
#define _POLLTRACE  0 /* not configurable w/ flag yet */
#define _DATATRACE  1 /* not configurable w/ flag yet */
#define _STDIOTRACE 0 /* not configurable w/ flag yet */
#define _LOCKTRACE  0 /* not configurable w/ flag yet */
#define _NTTRACE    0 /* not configurable w/ flag yet */

#define STRACE_PROLOGUE "%rSYS %6P %'18T "

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#ifdef SYSDEBUG
#define STRACE(FMT, ...)                                   \
  do {                                                     \
    if (UNLIKELY(__strace > 0) && strace_enabled(0) > 0) { \
      __stracef(STRACE_PROLOGUE FMT "\n", ##__VA_ARGS__);  \
    }                                                      \
  } while (0)
#else
#define STRACE(FMT, ...) (void)0
#endif

#if defined(SYSDEBUG) && _DATATRACE
#define DATATRACE(FMT, ...) STRACE(FMT, ##__VA_ARGS__)
#else
#define DATATRACE(FMT, ...) (void)0
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

#if defined(SYSDEBUG) && _STDIOTRACE
#define STDIOTRACE(FMT, ...) STRACE(FMT, ##__VA_ARGS__)
#else
#define STDIOTRACE(FMT, ...) (void)0
#endif

#if defined(SYSDEBUG) && _NTTRACE
#define NTTRACE(FMT, ...) STRACE(FMT, ##__VA_ARGS__)
#else
#define NTTRACE(FMT, ...) (void)0
#endif

#if defined(SYSDEBUG) && _LOCKTRACE
#define LOCKTRACE(FMT, ...) STRACE(FMT, ##__VA_ARGS__)
#else
#define LOCKTRACE(FMT, ...) (void)0
#endif

void __stracef(const char *, ...);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRACE_INTERNAL_H_ */
