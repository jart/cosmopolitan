#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_NTEXCEPTIONPOINTERS_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_NTEXCEPTIONPOINTERS_H_
#include "libc/nt/struct/context.h"
#include "libc/nt/struct/ntexceptionrecord.h"

struct NtExceptionPointers {
  struct NtExceptionRecord *ExceptionRecord;
  struct NtContext *ContextRecord;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_NTEXCEPTIONPOINTERS_H_ */
