#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SYSINFO_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SYSINFO_INTERNAL_H_
#include "libc/calls/struct/sysinfo.h"
COSMOPOLITAN_C_START_

int sys_sysinfo(struct sysinfo *);
int sys_sysinfo_nt(struct sysinfo *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SYSINFO_INTERNAL_H_ */
