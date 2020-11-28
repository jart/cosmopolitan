#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_CRITICALSECTION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_CRITICALSECTION_H_
#include "libc/nt/struct/criticalsectiondebug.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtCriticalSection {
  struct NtCriticalSectionDebug *DebugInfo;
  int32_t LockCount;
  int32_t RecursionCount;
  void *OwningThread;
  void *LockSemaphore;
  uintptr_t SpinCount;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_CRITICALSECTION_H_ */
