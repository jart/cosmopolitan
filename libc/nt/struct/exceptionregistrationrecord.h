#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_EXCEPTIONREGISTRATIONRECORD_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_EXCEPTIONREGISTRATIONRECORD_H_

#include "libc/nt/typedef/exceptionhandler.h"

struct NtExceptionRegistrationRecord {
  struct NtExceptionRegistrationRecord *Next;
  NtExceptionHandler Handler;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_EXCEPTIONREGISTRATIONRECORD_H_ */
