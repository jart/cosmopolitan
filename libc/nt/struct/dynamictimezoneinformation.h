#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_DYNAMICTIMEZONEINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_DYNAMICTIMEZONEINFORMATION_H_
#include "libc/nt/struct/systemtime.h"

struct NtDynamicTimeZoneInformation {
  int32_t Bias;
  char16_t StandardName[32];
  struct NtSystemTime StandardDate;
  int32_t StandardBias;
  char16_t DaylightName[32];
  struct NtSystemTime DaylightDate;
  int32_t DaylightBias;
  char16_t TimeZoneKeyName[128];
  bool32 DynamicDaylightTimeDisabled;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_DYNAMICTIMEZONEINFORMATION_H_ */
