#ifndef COSMOPOLITAN_LIBC_CALLS_STRACE_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRACE_INTERNAL_H_

#ifndef SYSDEBUG
#define SYSDEBUG 0
#endif

#define _NTTRACE    0 /* not configurable w/ flag yet */
#define _POLLTRACE  0 /* not configurable w/ flag yet */
#define _DATATRACE  1 /* not configurable w/ flag yet */
#define _LOCKTRACE  0 /* not configurable w/ flag yet */
#define _STDIOTRACE 0 /* not configurable w/ flag yet */
#define _KERNTRACE  0 /* not configurable w/ flag yet */
#define _TIMETRACE  0 /* not configurable w/ flag yet */

#define STRACE_PROLOGUE "%rSYS %6P %6H %'18T "

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define STRACE(FMT, ...)                       \
  ((void)(SYSDEBUG && strace_enabled(0) > 0 && \
          (__stracef(STRACE_PROLOGUE FMT "\n", ##__VA_ARGS__), 0)))

#define DATATRACE(FMT, ...)                                  \
  ((void)(SYSDEBUG && _DATATRACE && strace_enabled(0) > 0 && \
          (__stracef(STRACE_PROLOGUE FMT "\n", ##__VA_ARGS__), 0)))

#define POLLTRACE(FMT, ...)                                  \
  ((void)(SYSDEBUG && _POLLTRACE && strace_enabled(0) > 0 && \
          (__stracef(STRACE_PROLOGUE FMT "\n", ##__VA_ARGS__), 0)))

#define KERNTRACE(FMT, ...)                                  \
  ((void)(SYSDEBUG && _KERNTRACE && strace_enabled(0) > 0 && \
          (__stracef(STRACE_PROLOGUE FMT "\n", ##__VA_ARGS__), 0)))

#define STDIOTRACE(FMT, ...)                                  \
  ((void)(SYSDEBUG && _STDIOTRACE && strace_enabled(0) > 0 && \
          (__stracef(STRACE_PROLOGUE FMT "\n", ##__VA_ARGS__), 0)))

#define NTTRACE(FMT, ...)                                                   \
  ((void)(SYSDEBUG && _NTTRACE && strace_enabled(0) > 0 &&                  \
          (__stracef(STRACE_PROLOGUE "\e[2m" FMT "\e[0m\n", ##__VA_ARGS__), \
           0)))

#define LOCKTRACE(FMT, ...)                                  \
  ((void)(SYSDEBUG && _LOCKTRACE && strace_enabled(0) > 0 && \
          (__stracef(STRACE_PROLOGUE FMT "\n", ##__VA_ARGS__), 0)))

#define TIMETRACE(FMT, ...)                                  \
  ((void)(SYSDEBUG && _TIMETRACE && strace_enabled(0) > 0 && \
          (__stracef(STRACE_PROLOGUE FMT "\n", ##__VA_ARGS__), 0)))

int strace_enabled(int);
void __stracef(const char *, ...);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRACE_INTERNAL_H_ */
