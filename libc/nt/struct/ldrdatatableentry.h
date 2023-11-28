#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_LDRDATATABLEENTRY_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_LDRDATATABLEENTRY_H_
#include "libc/nt/struct/linkedlist.h"
#include "libc/nt/struct/unicodestring.h"

struct NtLdrDataTableEntry {
  struct NtLinkedList InLoadOrderLinks; /* msdn:reserved */
  struct NtLinkedList InMemoryOrderLinks;
  struct NtLinkedList InInitOrderLinks; /* msdn:reserved */
  void *DllBase;
  void *EntryPoint;
  union {
    uint32_t SizeOfImage;
    unsigned char SizeOfImagePadding[__SIZEOF_POINTER__];
  };
  struct NtUnicodeString FullDllName;
  struct NtUnicodeString BaseDllName;
  uint32_t Flags;
  uint16_t Load_Count;
  uint16_t TlsIndex;
  union {
    struct NtLinkedList HashLinks;
    struct {
      void *SectionPointer;
      uint32_t CheckSum;
    };
  };
  union {
    void *LoadedImports;
    uint32_t TimeDateStamp;
  };
  void *EntryPointActivationContext;
  void *PatchInformation;
  struct NtLinkedList ForwarderLinks;
  struct NtLinkedList ServiceTagLinks;
  struct NtLinkedList StaticLinks;
  void *ContextInformation;
  uintptr_t OriginalBase;
  int64_t LoadTime;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_LDRDATATABLEENTRY_H_ */
