#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_RUSAGE_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_RUSAGE_INTERNAL_H_
#include "libc/calls/struct/rusage.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int __sys_getrusage(int, struct rusage *) _Hide;
int __sys_wait4(int, int *, int, struct rusage *) _Hide;
int sys_getrusage(int, struct rusage *) _Hide;
int sys_wait4(int, int *, int, struct rusage *) _Hide;
void __rusage2linux(struct rusage *) _Hide;
int sys_getrusage_nt(int, struct rusage *) _Hide;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_RUSAGE_INTERNAL_H_ */
