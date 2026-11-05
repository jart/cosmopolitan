#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_ACTIVATIONCONTEXTSTACK_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_ACTIVATIONCONTEXTSTACK_H_

#include "libc/nt/struct/linkedlist.h"

struct NtActivationContextStack {
  struct NtActivationContextStackFrame* ActiveFrame;
  struct NtLinkedList FrameListCache;
  uint32_t Flags;
  uint32_t NextCookieSequenceNumber;
  uint32_t StackId;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_ACTIVATIONCONTEXTSTACK_H_ */
