#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_TIB_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_TIB_H_

#include "libc/nt/struct/exceptionregistrationrecord.h"

struct NtTib {
  struct NtExceptionRegistrationRecord* ExceptionList;
  void* StackBase;
  void* StackLimit;
  void* SubSystemTib;
  union {
    void* FiberData;
    uint32_t Version;
  };
  void* ArbitraryUserPointer;
  struct NtTib* Self;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_TIB_H_ */
