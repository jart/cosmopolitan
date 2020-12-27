#ifndef COSMOPOLITAN_LIBC_TIME_TIME_H_
#define COSMOPOLITAN_LIBC_TIME_TIME_H_
#include "libc/calls/struct/itimerval.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/time/struct/timezone.h"
#include "libc/time/struct/utimbuf.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const char kWeekdayNameShort[7][4];
hidden extern const char kWeekdayName[7][10];
hidden extern const char kMonthNameShort[12][4];
hidden extern const char kMonthName[12][10];
hidden extern const unsigned short kMonthYearDay[2][12];

extern char *tzname[2];
extern long CLOCKS_PER_SEC;

int64_t clock(void);
int64_t time(int64_t *);
int gettimeofday(struct timeval *, struct timezone *);
int clock_gettime(int, struct timespec *);
int clock_getres(int, struct timespec *);

int sleep(uint32_t);
int usleep(uint32_t);
int nanosleep(const struct timespec *, struct timespec *);
unsigned alarm(unsigned);
int getitimer(int, struct itimerval *);
int setitimer(int, const struct itimerval *, struct itimerval *);

void tzset(void);
struct tm *gmtime(const int64_t *);
struct tm *gmtime_r(const int64_t *, struct tm *);
struct tm *localtime(const int64_t *);
struct tm *localtime_r(const int64_t *, struct tm *);
int64_t timegm(struct tm *);
int64_t mktime(struct tm *);
int64_t timelocal(struct tm *);
int64_t timeoff(struct tm *, long);
int64_t time2posix(int64_t) pureconst;
int64_t posix2time(int64_t) pureconst;

char *strptime(const char *, const char *, struct tm *);
size_t strftime(char *, size_t, const char *, const struct tm *)
    strftimeesque(3);
char *asctime(const struct tm *);
char *ctime(const int64_t *);
char *ctime_r(const int64_t *, char[hasatleast 64]);
char *asctime_r(const struct tm *, char[hasatleast 64]);

int futimens(int, const struct timespec[2]);
int utimensat(int, const char *, const struct timespec[2], int);
int utimes(const char *, const struct timeval[2]);
int utime(const char *, const struct utimbuf *);
int futimes(int, const struct timeval[2]);
int futimesat(int, const char *, const struct timeval[2]);

long double dtime(int);
long double dsleep(long double);
extern long double (*nowl)(void);
long double converttickstonanos(uint64_t);
long double converttickstoseconds(uint64_t);

double difftime(int64_t, int64_t) nothrow pureconst;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_TIME_TIME_H_ */
