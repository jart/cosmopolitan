#ifndef COSMOPOLITAN_LIBC_TIME_TIME_H_
#define COSMOPOLITAN_LIBC_TIME_TIME_H_

#define TIME_UTC 1

COSMOPOLITAN_C_START_

extern char *tzname[2];
extern long timezone;
extern int daylight;

libcesque char *ctime(const int64_t *);
libcesque char *ctime_r(const int64_t *, char[hasatleast 26]);
libcesque double difftime(int64_t, int64_t) pureconst;
libcesque int64_t posix2time(int64_t) pureconst;
libcesque int64_t time2posix(int64_t) pureconst;
libcesque int stime(const int64_t *);
libcesque void tzset(void);

#ifdef _COSMO_SOURCE
extern const char kWeekdayNameShort[7][4];
extern const char kWeekdayName[7][10];
extern const char kMonthNameShort[12][4];
extern const char kMonthName[12][10];
extern const unsigned short kMonthYearDay[2][12];
#endif /* _COSMO_SOURCE */

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_TIME_TIME_H_ */
