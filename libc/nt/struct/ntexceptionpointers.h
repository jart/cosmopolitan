#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_NTEXCEPTIONPOINTERS_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_NTEXCEPTIONPOINTERS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtContext;
struct NtExceptionRecord;

struct NtExceptionPointers {
  struct NtExceptionRecord *ExceptionRecord;
  struct NtContext *ContextRecord;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_NTEXCEPTIONPOINTERS_H_ */
