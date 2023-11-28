#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_CRITICALSECTIONDEBUG_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_CRITICALSECTIONDEBUG_H_
#include "libc/nt/struct/criticalsection.h"
#include "libc/nt/struct/linkedlist.h"

struct NtCriticalSectionDebug {
  uint16_t Type;
  uint16_t CreatorBackTraceIndex;
  struct NtCriticalSection *CriticalSection;
  struct NtLinkedList ProcessLocksList;
  uint32_t EntryCount;
  uint32_t ContentionCount;
  uint32_t Spare[2];
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_CRITICALSECTIONDEBUG_H_ */
