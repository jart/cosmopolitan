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
char *ltpcpy(char *, long) paramsnonnull() libcesque nocallback;
int llog10(unsigned long) libcesque pureconst;
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
long wcstol(const wchar_t *, wchar_t **, int);
long strtol(const char *, char **, int) paramsnonnull((1)) libcesque;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § conversion » time                                         ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int64_t DosDateTimeToUnix(unsigned, unsigned);
struct timespec FileTimeToTimeSpec(struct NtFileTime);
struct NtFileTime TimeSpecToFileTime(struct timespec);
struct NtFileTime TimeToFileTime(int64_t) nothrow pureconst;
int64_t filetimetotime(struct NtFileTime) nothrow pureconst;
void FileTimeToTimeVal(struct timeval *, struct NtFileTime) nothrow;
struct NtFileTime TimeValToFileTime(const struct timeval *) nosideeffect;
long convertmicros(const struct timeval *, long) paramsnonnull() nosideeffect;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § conversion » manipulation                                 ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

char *dirname(char *);
char *basename(const char *) nosideeffect;
char *basename_n(const char *, size_t) nosideeffect;
bool isabspath(const char *) paramsnonnull() nosideeffect;

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
double RoundDecimalPlaces(double, double, double (*)(double));

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
