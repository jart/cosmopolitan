#ifndef COSMOPOLITAN_LIBC_NT_TYPEDEF_EXCEPTIONHANDLER_H_
#define COSMOPOLITAN_LIBC_NT_TYPEDEF_EXCEPTIONHANDLER_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct NtExceptionFrame;
struct NtExceptionRecord;
struct NtContext;

typedef unsigned (*NtExceptionHandler)(struct NtExceptionRecord *,
                                       struct NtExceptionFrame *,
                                       struct NtContext *, void *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_TYPEDEF_EXCEPTIONHANDLER_H_ */
