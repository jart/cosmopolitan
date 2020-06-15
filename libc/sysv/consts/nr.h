#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_NR_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_NR_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long __NR_access;
hidden extern const long __NR_arch_prctl;
hidden extern const long __NR_clock_gettime;
hidden extern const long __NR_close;
hidden extern const long __NR_exit;
hidden extern const long __NR_fadvise;
hidden extern const long __NR_fork;
hidden extern const long __NR_fstat;
hidden extern const long __NR_getpid;
hidden extern const long __NR_gettid;
hidden extern const long __NR_gettimeofday;
hidden extern const long __NR_kill;
hidden extern const long __NR_lstat;
hidden extern const long __NR_madvise;
hidden extern const long __NR_mmap;
hidden extern const long __NR_mprotect;
hidden extern const long __NR_munmap;
hidden extern const long __NR_open;
hidden extern const long __NR_pread;
hidden extern const long __NR_pwrite;
hidden extern const long __NR_read;
hidden extern const long __NR_sched_yield;
hidden extern const long __NR_sendfile;
hidden extern const long __NR_sigaction;
hidden extern const long __NR_sigprocmask;
hidden extern const long __NR_stat;
hidden extern const long __NR_write;
hidden extern const long __NR_wait4;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define __NR_access        SYMBOLIC(__NR_access)
#define __NR_arch_prctl    SYMBOLIC(__NR_arch_prctl)
#define __NR_clock_gettime SYMBOLIC(__NR_clock_gettime)
#define __NR_close         SYMBOLIC(__NR_close)
#define __NR_exit          SYMBOLIC(__NR_exit)
#define __NR_fadvise       SYMBOLIC(__NR_fadvise)
#define __NR_fork          SYMBOLIC(__NR_fork)
#define __NR_fstat         SYMBOLIC(__NR_fstat)
#define __NR_getpid        SYMBOLIC(__NR_getpid)
#define __NR_gettid        SYMBOLIC(__NR_gettid)
#define __NR_gettimeofday  SYMBOLIC(__NR_gettimeofday)
#define __NR_kill          SYMBOLIC(__NR_kill)
#define __NR_lstat         SYMBOLIC(__NR_lstat)
#define __NR_madvise       SYMBOLIC(__NR_madvise)
#define __NR_mmap          SYMBOLIC(__NR_mmap)
#define __NR_mprotect      SYMBOLIC(__NR_mprotect)
#define __NR_munmap        SYMBOLIC(__NR_munmap)
#define __NR_open          SYMBOLIC(__NR_open)
#define __NR_pread         SYMBOLIC(__NR_pread)
#define __NR_pwrite        SYMBOLIC(__NR_pwrite)
#define __NR_read          SYMBOLIC(__NR_read)
#define __NR_sched_yield   SYMBOLIC(__NR_sched_yield)
#define __NR_sendfile      SYMBOLIC(__NR_sendfile)
#define __NR_sigaction     SYMBOLIC(__NR_sigaction)
#define __NR_sigprocmask   SYMBOLIC(__NR_sigprocmask)
#define __NR_stat          SYMBOLIC(__NR_stat)
#define __NR_write         SYMBOLIC(__NR_write)
#define __NR_wait4         SYMBOLIC(__NR_wait4)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_NR_H_ */
