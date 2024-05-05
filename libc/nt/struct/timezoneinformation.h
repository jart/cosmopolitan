#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_TIMEZONEINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_TIMEZONEINFORMATION_H_
#include "libc/nt/struct/systemtime.h"

struct NtTimeZoneInformation {
  int Bias;                  /* in minutes e.g. +480 for -8:00 */
  char16_t StandardName[32]; /* e.g. "Pacific Standard Time" */
  struct NtSystemTime StandardDate;
  int StandardBias;
  char16_t DaylightName[32]; /* e.g. "Pacific Daylight Time" */
  struct NtSystemTime DaylightDate;
  int DaylightBias; /* e.g. -60 */
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_TIMEZONEINFORMATION_H_ */
