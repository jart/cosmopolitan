#ifndef COSMOPOLITAN_LIBC_LOG_LOG_H_
#define COSMOPOLITAN_LIBC_LOG_LOG_H_
#include "libc/stdio/stdio.h"
#ifdef _COSMO_SOURCE

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

#ifdef TINY
#define _LOG_TINY 1
#else
#define _LOG_TINY 0
#endif

COSMOPOLITAN_C_START_

extern FILE *__log_file;

int __watch(void *, size_t);
void __die(void) relegated wontreturn; /* print backtrace and abort() */
void _meminfo(int);                    /* shows malloc statistics &c. */
void _memsummary(int);                 /* light version of same thing */
const char *commandvenv(const char *, const char *);
const char *GetAddr2linePath(void);
const char *GetGdbPath(void);
bool32 IsDebuggerPresent(bool32);
bool32 IsRunningUnderMake(void);
char *GetSymbolByAddr(int64_t);
void PrintGarbage(void);
void PrintGarbageNumeric(FILE *);
void PrintWindowsMemory(const char *, size_t);

#ifndef __STRICT_ANSI__

#define _LOG_UNLIKELY(x) __builtin_expect(!!(x), 0)

extern unsigned __log_level; /* log level for runtime check */

#define LOGGABLE(LEVEL)                                          \
  ((!__builtin_constant_p(LEVEL) || (LEVEL) <= LOGGABLELEVEL) && \
   (LEVEL) <= __log_level)

// log a message with the specified log level (not checking if LOGGABLE)
#define LOGF(LEVEL, FMT, ...)                                   \
  do {                                                          \
    if (!_LOG_TINY) _log_untrace();                             \
    flogf(LEVEL, __FILE__, __LINE__, NULL, FMT, ##__VA_ARGS__); \
    if (!_LOG_TINY) _log_retrace();                             \
  } while (0)

// report an error without backtrace and debugger invocation
#define FATALF(FMT, ...)                                            \
  do {                                                              \
    if (!_LOG_TINY) _log_untrace();                                 \
    flogf(kLogError, __FILE__, __LINE__, NULL, FMT, ##__VA_ARGS__); \
    _log_exit(1);                                                   \
  } while (0)

#define DIEF(FMT, ...)                                                \
  do {                                                                \
    if (!_LOG_TINY) _log_untrace();                                   \
    ffatalf(kLogFatal, __FILE__, __LINE__, NULL, FMT, ##__VA_ARGS__); \
    __builtin_unreachable();                                          \
  } while (0)

#define ERRORF(FMT, ...)                   \
  do {                                     \
    if (LOGGABLE(kLogError)) {             \
      LOGF(kLogError, FMT, ##__VA_ARGS__); \
    }                                      \
  } while (0)

#define WARNF(FMT, ...)                   \
  do {                                    \
    if (LOGGABLE(kLogWarn)) {             \
      LOGF(kLogWarn, FMT, ##__VA_ARGS__); \
    }                                     \
  } while (0)

#define INFOF(FMT, ...)                   \
  do {                                    \
    if (LOGGABLE(kLogInfo)) {             \
      LOGF(kLogInfo, FMT, ##__VA_ARGS__); \
    }                                     \
  } while (0)

#define VERBOSEF(FMT, ...)                                                  \
  do {                                                                      \
    if (LOGGABLE(kLogVerbose)) {                                            \
      if (!_LOG_TINY) _log_untrace();                                       \
      fverbosef(kLogVerbose, __FILE__, __LINE__, NULL, FMT, ##__VA_ARGS__); \
      if (!_LOG_TINY) _log_retrace();                                       \
    }                                                                       \
  } while (0)

#define DEBUGF(FMT, ...)                                                \
  do {                                                                  \
    if (_LOG_UNLIKELY(LOGGABLE(kLogDebug))) {                           \
      if (!_LOG_TINY) _log_untrace();                                   \
      fdebugf(kLogDebug, __FILE__, __LINE__, NULL, FMT, ##__VA_ARGS__); \
      if (!_LOG_TINY) _log_retrace();                                   \
    }                                                                   \
  } while (0)

#define NOISEF(FMT, ...)                                                \
  do {                                                                  \
    if (_LOG_UNLIKELY(LOGGABLE(kLogNoise))) {                           \
      if (!_LOG_TINY) _log_untrace();                                   \
      fnoisef(kLogNoise, __FILE__, __LINE__, NULL, FMT, ##__VA_ARGS__); \
      if (!_LOG_TINY) _log_retrace();                                   \
    }                                                                   \
  } while (0)

#define FLOGF(F, FMT, ...)                                        \
  do {                                                            \
    if (LOGGABLE(kLogInfo)) {                                     \
      if (!_LOG_TINY) _log_untrace();                             \
      flogf(kLogInfo, __FILE__, __LINE__, F, FMT, ##__VA_ARGS__); \
      if (!_LOG_TINY) _log_retrace();                             \
    }                                                             \
  } while (0)

#define FWARNF(F, FMT, ...)                                       \
  do {                                                            \
    if (LOGGABLE(kLogWarn)) {                                     \
      if (!_LOG_TINY) _log_untrace();                             \
      flogf(kLogWarn, __FILE__, __LINE__, F, FMT, ##__VA_ARGS__); \
      if (!_LOG_TINY) _log_retrace();                             \
    }                                                             \
  } while (0)

#define FFATALF(F, FMT, ...)                                     \
  do {                                                           \
    if (!_LOG_TINY) _log_untrace();                              \
    flogf(kLogError, __FILE__, __LINE__, F, FMT, ##__VA_ARGS__); \
    _log_exit(1);                                                \
  } while (0)

#define FDEBUGF(F, FMT, ...)                                         \
  do {                                                               \
    if (_LOG_UNLIKELY(LOGGABLE(kLogDebug))) {                        \
      if (!_LOG_TINY) _log_untrace();                                \
      fdebugf(kLogDebug, __FILE__, __LINE__, F, FMT, ##__VA_ARGS__); \
      if (!_LOG_TINY) _log_retrace();                                \
    }                                                                \
  } while (0)

#define FNOISEF(F, FMT, ...)                                         \
  do {                                                               \
    if (_LOG_UNLIKELY(LOGGABLE(kLogNoise))) {                        \
      if (!_LOG_TINY) _log_untrace();                                \
      fnoisef(kLogNoise, __FILE__, __LINE__, F, FMT, ##__VA_ARGS__); \
      if (!_LOG_TINY) _log_retrace();                                \
    }                                                                \
  } while (0)

#define LOGIFNEG1(FORM)                                                \
  ({                                                                   \
    int e = _log_get_errno();                                          \
    autotype(FORM) Ax = (FORM);                                        \
    if (_LOG_UNLIKELY(Ax == (typeof(Ax))(-1)) && LOGGABLE(kLogWarn)) { \
      if (!_LOG_TINY) _log_untrace();                                  \
      _log_errno(__FILE__, __LINE__, #FORM);                           \
      if (!_LOG_TINY) _log_retrace();                                  \
      _log_set_errno(e);                                               \
    }                                                                  \
    Ax;                                                                \
  })

#define LOGIFNULL(FORM)                      \
  ({                                         \
    int e = _log_get_errno();                \
    autotype(FORM) Ax = (FORM);              \
    if (Ax == NULL && LOGGABLE(kLogWarn)) {  \
      if (!_LOG_TINY) _log_untrace();        \
      _log_errno(__FILE__, __LINE__, #FORM); \
      if (!_LOG_TINY) _log_retrace();        \
      _log_set_errno(e);                     \
    }                                        \
    Ax;                                      \
  })

void _log_errno(const char *, int, const char *) relegated;
int _log_get_errno(void);
void _log_set_errno(int);
void _log_untrace(void);
void _log_retrace(void);
void _log_exit(int) wontreturn;

#define ARGS  unsigned, const char *, int, FILE *, const char *
#define ATTR  paramsnonnull((5)) printfesque(5)
#define ATTRV paramsnonnull((5))
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
#endif /* _COSMO_SOURCE */
#endif /* COSMOPOLITAN_LIBC_LOG_LOG_H_ */
