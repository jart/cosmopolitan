#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGINFO_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGINFO_INTERNAL_H_
#include "libc/calls/struct/sigaction-xnu.internal.h"
#include "libc/calls/struct/siginfo-xnu.internal.h"
#include "libc/calls/struct/siginfo.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int sys_sigqueueinfo(int, const siginfo_t *) hidden;
void __sigenter_xnu(void *, int, int, struct siginfo_xnu *,
                    struct __darwin_ucontext *) hidden;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGINFO_INTERNAL_H_ */
