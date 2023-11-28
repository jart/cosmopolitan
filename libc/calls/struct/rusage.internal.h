#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_RUSAGE_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_RUSAGE_INTERNAL_H_
#include "libc/calls/struct/rusage.h"
COSMOPOLITAN_C_START_

int __sys_getrusage(int, struct rusage *);
int __sys_wait4(int, int *, int, struct rusage *);
int sys_getrusage(int, struct rusage *);
int sys_wait4(int, int *, int, struct rusage *);
void __rusage2linux(struct rusage *);
int sys_getrusage_nt(int, struct rusage *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_RUSAGE_INTERNAL_H_ */
