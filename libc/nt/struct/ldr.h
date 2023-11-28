#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_LDR_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_LDR_H_
#include "libc/nt/struct/linkedlist.h"

struct NtLdr {
  uint32_t SizeOfThis;                       /* msdn:reserved */
  uint32_t IsInitialized;                    /* msdn:reserved */
  void *SsHandle;                            /* msdn:reserved */
  struct NtLinkedList InLoadOrderModuleList; /* msdn:reserved */
  struct NtLinkedList /*∩NtLdrDataTableEntry*/ InMemoryOrderModuleList;
  struct NtLinkedList InInitOrderModuleList; /* msdn:reserved */
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_LDR_H_ */
