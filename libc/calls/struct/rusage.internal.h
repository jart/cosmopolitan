#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_RUSAGE_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_RUSAGE_INTERNAL_H_
#include "libc/calls/struct/rusage.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int __sys_getrusage(int, struct rusage *) hidden;
int __sys_wait4(int, int *, int, struct rusage *) hidden;
int sys_getrusage(int, struct rusage *) hidden;
int sys_wait4(int, int *, int, struct rusage *) hidden;
void __rusage2linux(struct rusage *) hidden;
int sys_getrusage_nt(int, struct rusage *) hidden;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_RUSAGE_INTERNAL_H_ */
