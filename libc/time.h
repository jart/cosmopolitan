#ifndef COSMOPOLITAN_LIBC_TIME_H_
#define COSMOPOLITAN_LIBC_TIME_H_

#define TIME_UTC 1

COSMOPOLITAN_C_START_

struct tm {
  int32_t tm_sec;
  int32_t tm_min;
  int32_t tm_hour;
  int32_t tm_mday; /* 1-indexed */
  int32_t tm_mon;  /* 0-indexed */
  int32_t tm_year; /* minus 1900 */
  int32_t tm_wday;
  int32_t tm_yday;
  int32_t tm_isdst;
  int64_t tm_gmtoff;
  const char *tm_zone;
};

struct timezone {
  int32_t tz_minuteswest;
  int32_t tz_dsttime;
};

extern char *tzname[2];
extern long timezone;
extern int daylight;

void tzset(void) libcesque;
char *asctime(const struct tm *) libcesque;
char *asctime_r(const struct tm *, char *) libcesque;
char *strptime(const char *, const char *, struct tm *) libcesque;
int64_t mktime(struct tm *) libcesque;
int64_t timegm(struct tm *) libcesque;
int64_t timelocal(struct tm *) libcesque;
int64_t timeoff(struct tm *, long) libcesque;
size_t strftime(char *, size_t, const char *, const struct tm *) libcesque
    strftimeesque(3) libcesque;
size_t wcsftime(wchar_t *, size_t, const wchar_t *,
                const struct tm *) libcesque;
struct tm *gmtime(const int64_t *) libcesque;
struct tm *gmtime_r(const int64_t *, struct tm *) libcesque;
struct tm *localtime(const int64_t *) libcesque;
struct tm *localtime_r(const int64_t *, struct tm *) libcesque;

char *ctime(const int64_t *) libcesque;
char *ctime_r(const int64_t *, char *) libcesque;
double difftime(int64_t, int64_t)
pureconst libcesque;
int stime(const int64_t *) libcesque;
void tzset(void) libcesque;

#ifdef _COSMO_SOURCE
extern const char kWeekdayNameShort[7][4];
extern const char kWeekdayName[7][10];
extern const char kMonthNameShort[12][4];
extern const char kMonthName[12][10];
extern const unsigned short kMonthYearDay[2][12];
#define iso8601 __iso8601
char *iso8601(char[hasatleast 20], struct tm *) libcesque;
#define iso8601us __iso8601us
char *iso8601us(char[hasatleast 27], struct tm *, long) libcesque;
#endif /* _COSMO_SOURCE */

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_TIME_H_ */
