#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_UCONTEXT_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_UCONTEXT_INTERNAL_H_
#include "libc/calls/ucontext.h"
#include "libc/nt/struct/context.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#ifdef __x86_64__
#define SP rsp
#elif defined(__aarch64__)
#define SP sp
#else
#error "unsupported architecture"
#endif

void _ntcontext2linux(struct ucontext *, const struct NtContext *) _Hide;
void _ntlinux2context(struct NtContext *, const ucontext_t *) _Hide;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_UCONTEXT_INTERNAL_H_ */
