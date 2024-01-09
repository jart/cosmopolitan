#ifndef COSMOPOLITAN_LIBC_TIME_STRUCT_TM_H_
#define COSMOPOLITAN_LIBC_TIME_STRUCT_TM_H_
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

char *asctime(const struct tm *) libcesque;
char *asctime_r(const struct tm *, char[hasatleast 26]) libcesque;
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

#ifdef _COSMO_SOURCE
#define iso8601   __iso8601
#define iso8601us __iso8601us
char *iso8601(char[hasatleast 20], struct tm *) libcesque;
char *iso8601us(char[hasatleast 27], struct tm *, long) libcesque;
#endif /* _COSMO_SOURCE */

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_TIME_STRUCT_TM_H_ */
