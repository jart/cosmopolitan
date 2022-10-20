#ifndef COSMOPOLITAN_LIBC_TIME_STRUCT_TM_H_
#define COSMOPOLITAN_LIBC_TIME_STRUCT_TM_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
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

char *asctime(const struct tm *);
char *asctime_r(const struct tm *, char[hasatleast 26]);
char *iso8601(char[hasatleast 20], struct tm *);
char *iso8601us(char[hasatleast 27], struct tm *, long);
char *strptime(const char *, const char *, struct tm *);
int64_t mktime(struct tm *);
int64_t timegm(struct tm *);
int64_t timelocal(struct tm *);
int64_t timeoff(struct tm *, long);
size_t strftime(char *, size_t, const char *, const struct tm *)
    strftimeesque(3);
size_t wcsftime(wchar_t *, size_t, const wchar_t *, const struct tm *);
struct tm *gmtime(const int64_t *);
struct tm *gmtime_r(const int64_t *, struct tm *);
struct tm *localtime(const int64_t *);
struct tm *localtime_r(const int64_t *, struct tm *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_TIME_STRUCT_TM_H_ */
