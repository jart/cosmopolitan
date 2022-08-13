#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_UCONTEXT_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_UCONTEXT_INTERNAL_H_
#include "libc/calls/ucontext.h"
#include "libc/nt/struct/context.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void _ntcontext2linux(struct ucontext *, const struct NtContext *) hidden;
void _ntlinux2context(struct NtContext *, const ucontext_t *) hidden;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_UCONTEXT_INTERNAL_H_ */
