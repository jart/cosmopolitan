#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_NTEXCEPTIONPOINTERS_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_NTEXCEPTIONPOINTERS_H_
#include "libc/nt/struct/context.h"
#include "libc/nt/struct/ntexceptionrecord.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtExceptionPointers {
  struct NtExceptionRecord *ExceptionRecord;
  struct NtContext *ContextRecord;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_NTEXCEPTIONPOINTERS_H_ */
