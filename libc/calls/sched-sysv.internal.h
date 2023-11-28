#ifndef COSMOPOLITAN_LIBC_CALLS_SCHED_SYSV_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_SCHED_SYSV_INTERNAL_H_
#include "libc/calls/struct/sched_param.h"

#define MAXCPUS_NETBSD  256
#define MAXCPUS_FREEBSD 256
#define MAXCPUS_OPENBSD 64
#define P_ALL_LWPS      0 /* for effect on all threads in pid */

#define CPU_LEVEL_WHICH 3
#define CPU_WHICH_TID   1
#define CPU_WHICH_PID   2

COSMOPOLITAN_C_START_

int sys_sched_get_priority_max(int);
int sys_sched_get_priority_min(int);
int sys_sched_getparam(int, struct sched_param *);
int sys_sched_getscheduler(int);
int sys_sched_setaffinity(int, uint64_t, const void *);
int sys_sched_setparam(int, const struct sched_param *);
int sys_sched_setscheduler(int, int, const struct sched_param *);
int sys_sched_yield(void);
int64_t sys_sched_getaffinity(int, uint64_t, void *);

int sys_sched_getscheduler_netbsd(int, struct sched_param *);
int sys_sched_setparam_netbsd(int, int, int, const struct sched_param *)  //
    asm("sys_sched_setparam");
int sys_sched_getparam_netbsd(int, int, int *, struct sched_param *)  //
    asm("sys_sched_getparam");
int sys_sched_setaffinity_netbsd(int, int, size_t, const void *)  //
    asm("sys_sched_setaffinity");
int sys_sched_getaffinity_netbsd(int, int, size_t, void *)  //
    asm("sys_sched_setaffinity");

int sys_sched_setaffinity_freebsd(
    int level, int which, int id, size_t setsize,
    const void *mask) asm("sys_sched_setaffinity");
int sys_sched_getaffinity_freebsd(int level, int which, int id, size_t setsize,
                                  void *mask) asm("sys_sched_getaffinity");

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_SCHED_SYSV_INTERNAL_H_ */
