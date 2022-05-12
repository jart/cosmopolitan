#ifndef COSMOPOLITAN_LIBC_TIME_STRUCT_TM_H_
#define COSMOPOLITAN_LIBC_TIME_STRUCT_TM_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/**
 * Time component structure.
 *
 * This structure is used by gmtime() and localtime(). It's not a kernel
 * interface. We use a different ABI than most C libraries here, because
 * we want to support timestamps dating back to the big bang, as well as
 * timestamps through much of the stelliferous era.
 */
struct tm {
  int64_t tm_year; /* minus 1900 */
  int64_t tm_mon;  /* 0-indexed */
  int64_t tm_mday; /* 1-indexed */
  int64_t tm_hour;
  int64_t tm_min;
  int64_t tm_sec;
  int64_t tm_wday;
  int64_t tm_yday;
  int64_t tm_isdst;
  int64_t tm_gmtoff;
  const char *tm_zone;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_TIME_STRUCT_TM_H_ */
