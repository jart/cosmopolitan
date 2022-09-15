#ifndef COSMOPOLITAN_LIBC_CALLS_SCHED_SYSV_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_SCHED_SYSV_INTERNAL_H_
#include "libc/calls/struct/sched_param.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define MAXCPUS_NETBSD  256
#define MAXCPUS_OPENBSD 64
#define P_ALL_LWPS      0 /* for effect on all threads in pid */

int sys_sched_get_priority_max(int);
int sys_sched_get_priority_min(int);
int sys_sched_getparam(int, struct sched_param *);
int sys_sched_getscheduler(int);
int sys_sched_setaffinity(int, uint64_t, const void *) hidden;
int sys_sched_setparam(int, const struct sched_param *);
int sys_sched_setscheduler(int, int, const struct sched_param *);
int sys_sched_yield(void) hidden;
int64_t sys_sched_getaffinity(int, uint64_t, void *) hidden;

int sys_sched_getscheduler_netbsd(int, struct sched_param *);
int sys_sched_setparam_netbsd(int, int, int, const struct sched_param *)  //
    asm("sys_sched_setparam");
int sys_sched_getparam_netbsd(int, int, int *, struct sched_param *)  //
    asm("sys_sched_getparam");
int sys_sched_setaffinity_netbsd(int, int, size_t, const void *)  //
    asm("sys_sched_setaffinity");
int sys_sched_getaffinity_netbsd(int, int, size_t, void *)  //
    asm("sys_sched_setaffinity");

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_SCHED_SYSV_INTERNAL_H_ */
