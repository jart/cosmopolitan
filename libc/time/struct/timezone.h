#ifndef COSMOPOLITAN_LIBC_TIME_STRUCT_TIMEZONE_H_
#define COSMOPOLITAN_LIBC_TIME_STRUCT_TIMEZONE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct timezone {
  int32_t tz_minuteswest;
  int32_t tz_dsttime;
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_TIME_STRUCT_TIMEZONE_H_ */
