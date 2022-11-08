#ifndef COSMOPOLITAN_LIBC_TIME_TIME_H_
#define COSMOPOLITAN_LIBC_TIME_TIME_H_

#define TIME_UTC       1
#define CLOCKS_PER_SEC 1000000L

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

_Hide extern const char kWeekdayNameShort[7][4];
_Hide extern const char kWeekdayName[7][10];
_Hide extern const char kMonthNameShort[12][4];
_Hide extern const char kMonthName[12][10];
_Hide extern const unsigned short kMonthYearDay[2][12];

extern char *tzname[2];
extern long timezone;
extern int daylight;

char *ctime(const int64_t *);
char *ctime_r(const int64_t *, char[hasatleast 26]);
double difftime(int64_t, int64_t) dontthrow pureconst;
extern long double (*nowl)(void);
int64_t clock(void);
int64_t posix2time(int64_t) pureconst;
int64_t time(int64_t *);
int64_t time2posix(int64_t) pureconst;
long double ConvertTicksToNanos(uint64_t);
long double dsleep(long double);
long double dtime(int);
unsigned alarm(unsigned);
unsigned sleep(unsigned);
void RefreshTime(void);
void tzset(void);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_TIME_TIME_H_ */
