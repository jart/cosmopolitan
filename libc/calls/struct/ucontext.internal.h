#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_UCONTEXT_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_UCONTEXT_INTERNAL_H_
#include "libc/calls/ucontext.h"
#include "libc/nt/struct/context.h"
COSMOPOLITAN_C_START_

#ifdef __x86_64__
#define PC   rip
#define SP   rsp
#define BP   rbp
#define ARG0 rdi
#define ARG1 rsi
#define ARG2 rdx
#define ARG3 rcx
#define ARG4 r8
#define ARG5 r9
#elif defined(__aarch64__)
#define PC   pc
#define SP   sp
#define BP   regs[29]
#define ARG0 regs[0]
#define ARG1 regs[1]
#define ARG2 regs[2]
#define ARG3 regs[3]
#define ARG4 regs[4]
#define ARG5 regs[5]
#else
#error "unsupported architecture"
#endif

void _ntcontext2linux(struct ucontext *, const struct NtContext *);
void _ntlinux2context(struct NtContext *, const ucontext_t *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_UCONTEXT_INTERNAL_H_ */
