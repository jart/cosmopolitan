#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_SYSTEMTIME_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_SYSTEMTIME_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtSystemTime {
  uint16_t wYear;
  uint16_t wMonth;
  uint16_t wDayOfWeek;
  uint16_t wDay;
  uint16_t wHour;
  uint16_t wMinute;
  uint16_t wSecond;
  uint16_t wMilliseconds;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_SYSTEMTIME_H_ */
