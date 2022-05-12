#ifndef COSMOPOLITAN_LIBC_TIME_STRUCT_TM_H_
#define COSMOPOLITAN_LIBC_TIME_STRUCT_TM_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

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

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_TIME_STRUCT_TM_H_ */
