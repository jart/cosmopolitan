#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_LDR_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_LDR_H_
#include "libc/nt/struct/linkedlist.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtLdr {
  uint32_t SizeOfThis;                       /* msdn:reserved */
  uint32_t IsInitialized;                    /* msdn:reserved */
  void *SsHandle;                            /* msdn:reserved */
  struct NtLinkedList InLoadOrderModuleList; /* msdn:reserved */
  struct NtLinkedList /*∩NtLdrDataTableEntry*/ InMemoryOrderModuleList;
  struct NtLinkedList InInitOrderModuleList; /* msdn:reserved */
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_LDR_H_ */
