#ifndef COSMOPOLITAN_LIBC_LOG_LOG_H_
#define COSMOPOLITAN_LIBC_LOG_LOG_H_
#include "libc/dce.h"
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § liblog                                                    ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define kLogFatal 0u
#define kLogError 1u
#define kLogWarn  2u
#define kLogInfo  3u
#define kLogDebug 4u

/**
 * Log level for compile-time DCE.
 */
#ifndef LOGGABLELEVEL
#ifndef NDEBUG
#define LOGGABLELEVEL kLogDebug
#elif IsTiny()
#define LOGGABLELEVEL kLogFatal
#else
#define LOGGABLELEVEL kLogInfo
#endif
#endif

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct sigset;
struct winsize;
struct StackFrame;
typedef struct FILE FILE;

extern FILE *g_logfile;

void backtrace(FILE *) relegated;    /* shows fn backtrace and args */
void perror(const char *) relegated; /* print the last system error */
void die(void) relegated noreturn;   /* print backtrace and abort() */
void meminfo(FILE *);                /* shows malloc statistics &c. */
void memsummary(FILE *);             /* light version of same thing */
uint16_t getttycols(uint16_t);
int getttysize(int, struct winsize *) paramsnonnull();
bool cancolor(void) nothrow nocallback;
bool isterminalinarticulate(void) nosideeffect;
char *commandvenv(const char *, const char *) nodiscard;

void showmappings(FILE *);
void showcrashreports(void);
void callexitontermination(struct sigset *);
bool32 IsDebuggerPresent(bool);
bool isrunningundermake(void);
void showbacktrace(FILE *, const struct StackFrame *);

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § liblog » logging                                          ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

extern unsigned g_loglevel; /* log level for runtime check */

#define LOGGABLE(LEVEL) \
  ((!isconstant(LEVEL) || (LEVEL) <= LOGGABLELEVEL) && (LEVEL) <= g_loglevel)

#define LOGF(FMT, ...)                                               \
  do {                                                               \
    if (LOGGABLE(kLogInfo)) {                                        \
      flogf(kLogInfo, __FILE__, __LINE__, NULL, FMT, ##__VA_ARGS__); \
    }                                                                \
  } while (0)

#define VFLOG(FMT, VA)                                     \
  do {                                                     \
    if (LOGGABLE(kLogInfo)) {                              \
      vflogf(kLogInfo, __FILE__, __LINE__, NULL, FMT, VA); \
    }                                                      \
  } while (0)

#define FLOGF(F, FMT, ...)                                        \
  do {                                                            \
    if (LOGGABLE(kLogInfo)) {                                     \
      flogf(kLogInfo, __FILE__, __LINE__, F, FMT, ##__VA_ARGS__); \
    }                                                             \
  } while (0)

#define VFLOGF(F, FMT, VA)                              \
  do {                                                  \
    if (LOGGABLE(kLogInfo)) {                           \
      vflogf(kLogInfo, __FILE__, __LINE__, F, FMT, VA); \
    }                                                   \
  } while (0)

#define WARNF(FMT, ...)                                              \
  do {                                                               \
    if (LOGGABLE(kLogWarn)) {                                        \
      flogf(kLogWarn, __FILE__, __LINE__, NULL, FMT, ##__VA_ARGS__); \
    }                                                                \
  } while (0)

#define VWARNF(FMT, VA)                                    \
  do {                                                     \
    if (LOGGABLE(kLogWarn)) {                              \
      vflogf(kLogWarn, __FILE__, __LINE__, NULL, FMT, VA); \
    }                                                      \
  } while (0)

#define FWARNF(F, FMT, ...)                                       \
  do {                                                            \
    if (LOGGABLE(kLogWarn)) {                                     \
      flogf(kLogWarn, __FILE__, __LINE__, F, FMT, ##__VA_ARGS__); \
    }                                                             \
  } while (0)

#define VFWARNF(F, FMT, VA)                             \
  do {                                                  \
    if (LOGGABLE(kLogWarn)) {                           \
      vflogf(kLogWarn, __FILE__, __LINE__, F, FMT, VA); \
    }                                                   \
  } while (0)

#define FATALF(FMT, ...)                                              \
  do {                                                                \
    ffatalf(kLogFatal, __FILE__, __LINE__, NULL, FMT, ##__VA_ARGS__); \
    unreachable;                                                      \
  } while (0)

#define VFATALF(FMT, VA)                                    \
  do {                                                      \
    vffatalf(kLogFatal, __FILE__, __LINE__, NULL, FMT, VA); \
    unreachable;                                            \
  } while (0)

#define FFATALF(F, FMT, ...)                                       \
  do {                                                             \
    ffatalf(kLogFatal, __FILE__, __LINE__, F, FMT, ##__VA_ARGS__); \
    unreachable;                                                   \
  } while (0)

#define VFFATALF(F, FMT, VA)                             \
  do {                                                   \
    vffatalf(kLogFatal, __FILE__, __LINE__, F, FMT, VA); \
    unreachable;                                         \
  } while (0)

#define DEBUGF(FMT, ...)                                                \
  do {                                                                  \
    if (LOGGABLE(kLogDebug)) {                                          \
      fdebugf(kLogDebug, __FILE__, __LINE__, NULL, FMT, ##__VA_ARGS__); \
    }                                                                   \
  } while (0)

#define VDEBUGF(FMT, VA)                                      \
  do {                                                        \
    if (LOGGABLE(kLogDebug)) {                                \
      vfdebugf(kLogDebug, __FILE__, __LINE__, NULL, FMT, VA); \
    }                                                         \
  } while (0)

#define FDEBUGF(F, FMT, ...)                                         \
  do {                                                               \
    if (LOGGABLE(kLogDebug)) {                                       \
      fdebugf(kLogDebug, __FILE__, __LINE__, F, FMT, ##__VA_ARGS__); \
    }                                                                \
  } while (0)

#define VFDEBUGF(F, FMT, VA)                               \
  do {                                                     \
    if (LOGGABLE(kLogDebug)) {                             \
      vfdebugf(kLogDebug, __FILE__, __LINE__, F, FMT, VA); \
    }                                                      \
  } while (0)

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § liblog » on error resume next                             ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define LOGIFNEG1(FORM)                                 \
  ({                                                    \
    autotype(FORM) Ax = (FORM);                         \
    if (Ax == (typeof(Ax))(-1) && LOGGABLE(kLogWarn)) { \
      __logerrno(__FILE__, __LINE__, #FORM);            \
    }                                                   \
    Ax;                                                 \
  })

#define LOGIFNULL(FORM)                      \
  ({                                         \
    autotype(FORM) Ax = (FORM);              \
    if (Ax == NULL && LOGGABLE(kLogWarn)) {  \
      __logerrno(__FILE__, __LINE__, #FORM); \
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
void fdebugf(ARGS, ...) asm("flogf") ATTR relegated libcesque;
void vfdebugf(ARGS, va_list) asm("vflogf") ATTRV relegated libcesque;
void ffatalf(ARGS, ...) asm("flogf") ATTR relegated noreturn libcesque;
void vffatalf(ARGS, va_list) asm("vflogf") ATTRV relegated noreturn libcesque;
#undef ARGS
#undef ATTR
#undef ATTRV

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_LOG_LOG_H_ */
