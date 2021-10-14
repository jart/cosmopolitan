#ifndef COSMOPOLITAN_LIBC_FMT_CONV_H_
#define COSMOPOLITAN_LIBC_FMT_CONV_H_
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/nt/struct/filetime.h"

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § conversion                                                ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define MODERNITYSECONDS 11644473600ull
#define HECTONANOSECONDS 10000000ull

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int abs(int) libcesque pureconst;
long labs(long) libcesque pureconst;
long long llabs(long long) libcesque pureconst;
int atoi(const char *) paramsnonnull() libcesque;
long atol(const char *) paramsnonnull() libcesque;
long long atoll(const char *) paramsnonnull() libcesque;
unsigned long strtoul(const char *, char **, int) paramsnonnull((1));
long long strtoll(const char *, char **, int) paramsnonnull((1));
unsigned long long strtoull(const char *, char **, int) paramsnonnull((1));
long long strtonum(const char *, long long, long long, const char **);
intmax_t div10(intmax_t, unsigned *) hidden;
intmax_t strtoimax(const char *, char **, int) paramsnonnull((1));
uintmax_t strtoumax(const char *, char **, int) paramsnonnull((1));
intmax_t wcstoimax(const wchar_t *, wchar_t **, int);
uintmax_t wcstoumax(const wchar_t *, wchar_t **, int);
long wcstol(const wchar_t *, wchar_t **, int);
unsigned long wcstoul(const wchar_t *, wchar_t **, int);
long strtol(const char *, char **, int) paramsnonnull((1)) libcesque;
long sizetol(const char *, long) paramsnonnull() libcesque;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § conversion » time                                         ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int64_t DosDateTimeToUnix(unsigned, unsigned) nothrow;
struct timeval WindowsTimeToTimeVal(int64_t) nothrow;
struct timespec WindowsTimeToTimeSpec(int64_t) nothrow;
int64_t TimeSpecToWindowsTime(struct timespec) nothrow;
int64_t TimeValToWindowsTime(struct timeval) nothrow;
struct timeval WindowsDurationToTimeVal(int64_t) nothrow;
struct timespec WindowsDurationToTimeSpec(int64_t) nothrow;

static inline struct NtFileTime MakeFileTime(int64_t x) {
  return (struct NtFileTime){(uint32_t)x, (uint32_t)(x >> 32)};
}

static inline int64_t ReadFileTime(struct NtFileTime t) {
  uint64_t x = t.dwHighDateTime;
  return x << 32 | t.dwLowDateTime;
}

#define FileTimeToTimeSpec(x) WindowsTimeToTimeSpec(ReadFileTime(x))
#define FileTimeToTimeVal(x)  WindowsTimeToTimeVal(ReadFileTime(x))
#define TimeSpecToFileTime(x) MakeFileTime(TimeSpecToWindowsTime(x))
#define TimeValToFileTime(x)  MakeFileTime(TimeValToWindowsTime(x))

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § conversion » manipulation                                 ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

char *dirname(char *);
char *basename(const char *) nosideeffect;
char *basename_n(const char *, size_t) nosideeffect;
bool isabspath(const char *) paramsnonnull() nosideeffect;
char *stripext(char *);
char *stripexts(char *);

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § conversion » computation                                  ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

typedef struct {
  int quot;
  int rem;
} div_t;

typedef struct {
  long int quot;
  long int rem;
} ldiv_t;

typedef struct {
  long long int quot;
  long long int rem;
} lldiv_t;

typedef struct {
  intmax_t quot;
  intmax_t rem;
} imaxdiv_t;

div_t div(int, int) pureconst;
ldiv_t ldiv(long, long) pureconst;
lldiv_t lldiv(long long, long long) pureconst;
imaxdiv_t imaxdiv(intmax_t, intmax_t) pureconst;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § conversion » optimizations                                ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#if __STDC_VERSION__ + 0 >= 199901L
#define div(num, den)   ((div_t){(num) / (den), (num) % (den)})
#define ldiv(num, den)  ((ldiv_t){(num) / (den), (num) % (den)})
#define lldiv(num, den) ((lldiv_t){(num) / (den), (num) % (den)})
#endif

#ifndef __STRICT_ANSI__
intmax_t __imaxabs(intmax_t) libcesque pureconst;
#define imaxabs(x) __imaxabs(x)
#endif /* !ANSI */

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_FMT_CONV_H_ */
