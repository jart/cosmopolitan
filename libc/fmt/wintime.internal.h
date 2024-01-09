#ifndef COSMOPOLITAN_LIBC_FMT_WINTIME_H_
#define COSMOPOLITAN_LIBC_FMT_WINTIME_H_
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/nt/struct/filetime.h"

#define MODERNITYSECONDS 11644473600ull
#define HECTONANOSECONDS 10000000ull

COSMOPOLITAN_C_START_

libcesque int64_t DosDateTimeToUnix(uint32_t, uint32_t) pureconst;
libcesque int64_t TimeSpecToWindowsTime(struct timespec) pureconst;
libcesque int64_t TimeValToWindowsTime(struct timeval) pureconst;
libcesque struct timespec WindowsDurationToTimeSpec(int64_t) pureconst;
libcesque struct timespec WindowsTimeToTimeSpec(int64_t) pureconst;
libcesque struct timeval WindowsDurationToTimeVal(int64_t) pureconst;
libcesque struct timeval WindowsTimeToTimeVal(int64_t) pureconst;

#define MakeFileTime(x)                                        \
  ({                                                           \
    int64_t __x = x;                                           \
    (struct NtFileTime){(uint32_t)__x, (uint32_t)(__x >> 32)}; \
  })

#define ReadFileTime(t)                     \
  ({                                        \
    struct NtFileTime __t = t;              \
    uint64_t x = __t.dwHighDateTime;        \
    (int64_t)(x << 32 | __t.dwLowDateTime); \
  })

#define FileTimeToTimeSpec(x) WindowsTimeToTimeSpec(ReadFileTime(x))
#define FileTimeToTimeVal(x)  WindowsTimeToTimeVal(ReadFileTime(x))
#define TimeSpecToFileTime(x) MakeFileTime(TimeSpecToWindowsTime(x))
#define TimeValToFileTime(x)  MakeFileTime(TimeValToWindowsTime(x))

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_FMT_WINTIME_H_ */
