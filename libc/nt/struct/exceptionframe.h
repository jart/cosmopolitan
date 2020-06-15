#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_EXCEPTIONFRAME_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_EXCEPTIONFRAME_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtExceptionFrame {
  struct NtExceptionFrame *Prev;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_EXCEPTIONFRAME_H_ */
