#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_RTLUSERPROCESSINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_RTLUSERPROCESSINFORMATION_H_
#include "libc/nt/struct/clientid.h"
#include "libc/nt/struct/sectionimageinformation.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtRtlUserProcessInformation {
  uint32_t SizeOfThis;
  int64_t ProcessHandle;
  int64_t ThreadHandle;
  struct NtClientId ClientId;
  struct NtSectionImageInformation ImageInformation;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_RTLUSERPROCESSINFORMATION_H_ */
