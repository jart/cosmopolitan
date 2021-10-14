#ifndef COSMOPOLITAN_LIBC_LOG_LOG_H_
#define COSMOPOLITAN_LIBC_LOG_LOG_H_
#include "libc/bits/likely.h"
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/winsize.h"
#include "libc/nexgen32e/stackframe.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § liblog                                                    ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define kLogFatal   0
#define kLogError   1
#define kLogWarn    2
#define kLogInfo    3
#define kLogVerbose 4
#define kLogDebug   5
#define kLogNoise   6

/**
 * Log level for compile-time DCE.
 */
#ifndef LOGGABLELEVEL
#ifndef TINY
#define LOGGABLELEVEL kLogNoise
/* #elif IsTiny() */
/* #define LOGGABLELEVEL kLogInfo */
#else
#define LOGGABLELEVEL kLogVerbose
#endif
#endif

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern FILE *__log_file;

void perror(const char *) relegated;   /* print the last system error */
void __die(void) relegated wontreturn; /* print backtrace and abort() */
void meminfo(int);                     /* shows malloc statistics &c. */
void memsummary(int);                  /* light version of same thing */
uint16_t getttycols(uint16_t);
int getttysize(int, struct winsize *) paramsnonnull();
bool IsTerminalInarticulate(void) nosideeffect;
const char *commandvenv(const char *, const char *);
const char *GetAddr2linePath(void);
const char *GetGdbPath(void);
const char *GetCallerName(const struct StackFrame *);

void ShowCrashReports(void);
void callexitontermination(struct sigset *);
bool32 IsDebuggerPresent(bool);
bool IsRunningUnderMake(void);
const char *GetSiCodeName(int, int);
void AppendResourceReport(char **, struct rusage *, const char *);
char *__get_symbol_by_addr(int64_t);
void PrintGarbage(void);
void PrintGarbageNumeric(FILE *);

#define showcrashreports() ShowCrashReports()

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § liblog » logging                                          ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/
#ifndef __STRICT_ANSI__

extern unsigned __log_level; /* log level for runtime check */

#define LOGGABLE(LEVEL)                                          \
  ((!__builtin_constant_p(LEVEL) || (LEVEL) <= LOGGABLELEVEL) && \
   (LEVEL) <= __log_level)

// log a message with the specified log level (not checking if LOGGABLE)
#define LOGF(LEVEL, FMT, ...)                                   \
  do {                                                          \
    ++g_ftrace;                                                 \
    flogf(LEVEL, __FILE__, __LINE__, NULL, FMT, ##__VA_ARGS__); \
    --g_ftrace;                                                 \
  } while (0)

// die with an error message without backtrace and debugger invocation
#define DIEF(FMT, ...)                                              \
  do {                                                              \
    ++g_ftrace;                                                     \
    flogf(kLogError, __FILE__, __LINE__, NULL, FMT, ##__VA_ARGS__); \
    exit(1);                                                        \
    unreachable;                                                    \
  } while (0)

#define FATALF(FMT, ...)                                              \
  do {                                                                \
    ++g_ftrace;                                                       \
    ffatalf(kLogFatal, __FILE__, __LINE__, NULL, FMT, ##__VA_ARGS__); \
    unreachable;                                                      \
  } while (0)

#define ERRORF(FMT, ...)                                              \
  do {                                                                \
    if (LOGGABLE(kLogError)) {                                        \
      ++g_ftrace;                                                     \
      flogf(kLogError, __FILE__, __LINE__, NULL, FMT, ##__VA_ARGS__); \
      --g_ftrace;                                                     \
    }                                                                 \
  } while (0)

#define WARNF(FMT, ...)                                              \
  do {                                                               \
    if (LOGGABLE(kLogWarn)) {                                        \
      ++g_ftrace;                                                    \
      flogf(kLogWarn, __FILE__, __LINE__, NULL, FMT, ##__VA_ARGS__); \
      --g_ftrace;                                                    \
    }                                                                \
  } while (0)

#define INFOF(FMT, ...)                                              \
  do {                                                               \
    if (LOGGABLE(kLogInfo)) {                                        \
      ++g_ftrace;                                                    \
      flogf(kLogInfo, __FILE__, __LINE__, NULL, FMT, ##__VA_ARGS__); \
      --g_ftrace;                                                    \
    }                                                                \
  } while (0)

#define VERBOSEF(FMT, ...)                                                  \
  do {                                                                      \
    if (LOGGABLE(kLogVerbose)) {                                            \
      ++g_ftrace;                                                           \
      fverbosef(kLogVerbose, __FILE__, __LINE__, NULL, FMT, ##__VA_ARGS__); \
      --g_ftrace;                                                           \
    }                                                                       \
  } while (0)

#define DEBUGF(FMT, ...)                                                \
  do {                                                                  \
    if (UNLIKELY(LOGGABLE(kLogDebug))) {                                \
      ++g_ftrace;                                                       \
      fdebugf(kLogDebug, __FILE__, __LINE__, NULL, FMT, ##__VA_ARGS__); \
      --g_ftrace;                                                       \
    }                                                                   \
  } while (0)

#define NOISEF(FMT, ...)                                                \
  do {                                                                  \
    if (UNLIKELY(LOGGABLE(kLogNoise))) {                                \
      ++g_ftrace;                                                       \
      fnoisef(kLogNoise, __FILE__, __LINE__, NULL, FMT, ##__VA_ARGS__); \
      --g_ftrace;                                                       \
    }                                                                   \
  } while (0)

#define FLOGF(F, FMT, ...)                                        \
  do {                                                            \
    if (LOGGABLE(kLogInfo)) {                                     \
      ++g_ftrace;                                                 \
      flogf(kLogInfo, __FILE__, __LINE__, F, FMT, ##__VA_ARGS__); \
      --g_ftrace;                                                 \
    }                                                             \
  } while (0)

#define FWARNF(F, FMT, ...)                                       \
  do {                                                            \
    if (LOGGABLE(kLogWarn)) {                                     \
      ++g_ftrace;                                                 \
      flogf(kLogWarn, __FILE__, __LINE__, F, FMT, ##__VA_ARGS__); \
      --g_ftrace;                                                 \
    }                                                             \
  } while (0)

#define FFATALF(F, FMT, ...)                                       \
  do {                                                             \
    ++g_ftrace;                                                    \
    ffatalf(kLogFatal, __FILE__, __LINE__, F, FMT, ##__VA_ARGS__); \
    unreachable;                                                   \
  } while (0)

#define FDEBUGF(F, FMT, ...)                                         \
  do {                                                               \
    if (UNLIKELY(LOGGABLE(kLogDebug))) {                             \
      ++g_ftrace;                                                    \
      fdebugf(kLogDebug, __FILE__, __LINE__, F, FMT, ##__VA_ARGS__); \
      --g_ftrace;                                                    \
    }                                                                \
  } while (0)

#define FNOISEF(F, FMT, ...)                                         \
  do {                                                               \
    if (UNLIKELY(LOGGABLE(kLogNoise))) {                             \
      ++g_ftrace;                                                    \
      fnoisef(kLogNoise, __FILE__, __LINE__, F, FMT, ##__VA_ARGS__); \
      --g_ftrace;                                                    \
    }                                                                \
  } while (0)

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § liblog » on error resume next                             ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define LOGIFNEG1(FORM)                                           \
  ({                                                              \
    autotype(FORM) Ax = (FORM);                                   \
    if (UNLIKELY(Ax == (typeof(Ax))(-1)) && LOGGABLE(kLogWarn)) { \
      ++g_ftrace;                                                 \
      __logerrno(__FILE__, __LINE__, #FORM);                      \
      --g_ftrace;                                                 \
    }                                                             \
    Ax;                                                           \
  })

#define LOGIFNULL(FORM)                      \
  ({                                         \
    autotype(FORM) Ax = (FORM);              \
    if (Ax == NULL && LOGGABLE(kLogWarn)) {  \
      ++g_ftrace;                            \
      __logerrno(__FILE__, __LINE__, #FORM); \
      --g_ftrace;                            \
    }                                        \
    Ax;                                      \
  })

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § liblog » implementation details                           ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

void __logerrno(const char *, int, const char *) relegated;

#define ARGS  unsigned, const char *, int, FILE *, const char *
#define ATTR  paramsnonnull((5)) printfesque(5)
#define ATTRV paramsnonnull((5, 6))
void flogf(ARGS, ...) ATTR libcesque;
void vflogf(ARGS, va_list) ATTRV libcesque;
void fverbosef(ARGS, ...) asm("flogf") ATTR relegated libcesque;
void vfverbosef(ARGS, va_list) asm("vflogf") ATTRV relegated libcesque;
void fdebugf(ARGS, ...) asm("flogf") ATTR relegated libcesque;
void vfdebugf(ARGS, va_list) asm("vflogf") ATTRV relegated libcesque;
void fnoisef(ARGS, ...) asm("flogf") ATTR relegated libcesque;
void vfnoisef(ARGS, va_list) asm("vflogf") ATTRV relegated libcesque;
void ffatalf(ARGS, ...) asm("flogf") ATTR relegated wontreturn libcesque;
void vffatalf(ARGS, va_list) asm("vflogf") ATTRV relegated wontreturn libcesque;
#undef ARGS
#undef ATTR
#undef ATTRV

#endif /* __STRICT_ANSI__ */
COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_LOG_LOG_H_ */
