/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/bits/likely.h"
#include "libc/calls/calls.h"
#include "libc/calls/pledge.internal.h"
#include "libc/calls/struct/bpf.h"
#include "libc/calls/struct/filter.h"
#include "libc/calls/struct/seccomp.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/promises.internal.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/bsr.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/sysv/consts/audit.h"
#include "libc/sysv/consts/nrlinux.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/pr.h"
#include "libc/sysv/consts/prot.h"

/**
 * @fileoverview OpenBSD pledge() Polyfill Payload for GNU/Systemd
 *
 * This file contains only the minimum amount of Linux-specific code
 * that's necessary to get a pledge() policy installed. This file is
 * designed to not use static or tls memory or libc depnedencies, so
 * it can be transplanted into codebases and injected into programs.
 */

#define Eperm       1
#define Sigabrt     6
#define Enosys      38
#define Sigsys      31
#define Sig_Setmask 2
#define Sa_Siginfo  4
#define Sa_Restorer 0x04000000
#define Sa_Restart  0x10000000

#define SPECIAL   0xf000
#define ADDRLESS  0x2000
#define INET      0x8000
#define LOCK      0x4000
#define NOEXEC    0x8000
#define EXEC      0x4000
#define READONLY  0x8000
#define WRITEONLY 0x4000
#define CREATONLY 0x2000
#define STDIO     0x8000
#define THREAD    0x8000
#define TTY       0x8000
#define UNIX      0x4000
#define NOBITS    0x8000
#define NOSIGSYS  0x8000
#define RESTRICT  0x1000

#define PLEDGE(pledge) pledge, ARRAYLEN(pledge)
#define OFF(f)         offsetof(struct seccomp_data, f)

#define AbortPledge(reason) \
  do {                      \
    asm("hlt");             \
    unreachable;            \
  } while (0)

struct Filter {
  size_t n;
  struct sock_filter p[700];
};

static const uint16_t kPledgeDefault[] = {
    __NR_linux_exit,  // thread return / exit()
};

// the stdio contains all the benign system calls. openbsd makes the
// assumption that preexisting file descriptors are trustworthy. we
// implement checking for these as a simple linear scan rather than
// binary search, since there doesn't appear to be any measurable
// difference in the latency of sched_yield() if it's at the start of
// the bpf script or the end.
static const uint16_t kPledgeStdio[] = {
    __NR_linux_sigreturn,             //
    __NR_linux_restart_syscall,       //
    __NR_linux_exit_group,            //
    __NR_linux_sched_yield,           //
    __NR_linux_sched_getaffinity,     //
    __NR_linux_clock_getres,          //
    __NR_linux_clock_gettime,         //
    __NR_linux_clock_nanosleep,       //
    __NR_linux_close_range,           //
    __NR_linux_close,                 //
    __NR_linux_write,                 //
    __NR_linux_writev,                //
    __NR_linux_pwrite,                //
    __NR_linux_pwritev,               //
    __NR_linux_pwritev2,              //
    __NR_linux_read,                  //
    __NR_linux_readv,                 //
    __NR_linux_pread,                 //
    __NR_linux_preadv,                //
    __NR_linux_preadv2,               //
    __NR_linux_dup,                   //
    __NR_linux_dup2,                  //
    __NR_linux_dup3,                  //
    __NR_linux_fchdir,                //
    __NR_linux_fcntl | STDIO,         //
    __NR_linux_fstat,                 //
    __NR_linux_fsync,                 //
    __NR_linux_sysinfo,               //
    __NR_linux_fdatasync,             //
    __NR_linux_ftruncate,             //
    __NR_linux_getdents,              //
    __NR_linux_getrandom,             //
    __NR_linux_getgroups,             //
    __NR_linux_getpgid,               //
    __NR_linux_getpgrp,               //
    __NR_linux_getpid,                //
    __NR_linux_gettid,                //
    __NR_linux_getuid,                //
    __NR_linux_getgid,                //
    __NR_linux_getsid,                //
    __NR_linux_getppid,               //
    __NR_linux_geteuid,               //
    __NR_linux_getegid,               //
    __NR_linux_getrlimit,             //
    __NR_linux_getresgid,             //
    __NR_linux_getresuid,             //
    __NR_linux_getitimer,             //
    __NR_linux_setitimer,             //
    __NR_linux_timerfd_create,        //
    __NR_linux_timerfd_settime,       //
    __NR_linux_timerfd_gettime,       //
    __NR_linux_copy_file_range,       //
    __NR_linux_gettimeofday,          //
    __NR_linux_sendfile,              //
    __NR_linux_vmsplice,              //
    __NR_linux_splice,                //
    __NR_linux_lseek,                 //
    __NR_linux_tee,                   //
    __NR_linux_brk,                   //
    __NR_linux_msync,                 //
    __NR_linux_mmap | NOEXEC,         //
    __NR_linux_mremap,                //
    __NR_linux_munmap,                //
    __NR_linux_mincore,               //
    __NR_linux_madvise,               //
    __NR_linux_fadvise,               //
    __NR_linux_mprotect | NOEXEC,     //
    __NR_linux_arch_prctl,            //
    __NR_linux_migrate_pages,         //
    __NR_linux_sync_file_range,       //
    __NR_linux_set_tid_address,       //
    __NR_linux_nanosleep,             //
    __NR_linux_pipe,                  //
    __NR_linux_pipe2,                 //
    __NR_linux_poll,                  //
    __NR_linux_ppoll,                 //
    __NR_linux_select,                //
    __NR_linux_pselect6,              //
    __NR_linux_epoll_create,          //
    __NR_linux_epoll_create1,         //
    __NR_linux_epoll_ctl,             //
    __NR_linux_epoll_wait,            //
    __NR_linux_epoll_pwait,           //
    __NR_linux_epoll_pwait2,          //
    __NR_linux_recvfrom,              //
    __NR_linux_sendto | ADDRLESS,     //
    __NR_linux_ioctl | RESTRICT,      //
    __NR_linux_alarm,                 //
    __NR_linux_pause,                 //
    __NR_linux_shutdown,              //
    __NR_linux_eventfd,               //
    __NR_linux_eventfd2,              //
    __NR_linux_signalfd,              //
    __NR_linux_signalfd4,             //
    __NR_linux_sigaction | NOSIGSYS,  //
    __NR_linux_sigaltstack,           //
    __NR_linux_sigprocmask,           //
    __NR_linux_sigsuspend,            //
    __NR_linux_sigpending,            //
    __NR_linux_socketpair,            //
    __NR_linux_getrusage,             //
    __NR_linux_times,                 //
    __NR_linux_umask,                 //
    __NR_linux_wait4,                 //
    __NR_linux_uname,                 //
    __NR_linux_prctl | STDIO,         //
    __NR_linux_clone | THREAD,        //
    __NR_linux_futex,                 //
    __NR_linux_set_robust_list,       //
    __NR_linux_get_robust_list,       //
    __NR_linux_prlimit | STDIO,       //
};

static const uint16_t kPledgeFlock[] = {
    __NR_linux_flock,         //
    __NR_linux_fcntl | LOCK,  //
};

static const uint16_t kPledgeRpath[] = {
    __NR_linux_chdir,              //
    __NR_linux_getcwd,             //
    __NR_linux_open | READONLY,    //
    __NR_linux_openat | READONLY,  //
    __NR_linux_stat,               //
    __NR_linux_lstat,              //
    __NR_linux_fstat,              //
    __NR_linux_fstatat,            //
    __NR_linux_access,             //
    __NR_linux_faccessat,          //
    __NR_linux_faccessat2,         //
    __NR_linux_readlink,           //
    __NR_linux_readlinkat,         //
    __NR_linux_statfs,             //
    __NR_linux_fstatfs,            //
};

static const uint16_t kPledgeWpath[] = {
    __NR_linux_getcwd,              //
    __NR_linux_open | WRITEONLY,    //
    __NR_linux_openat | WRITEONLY,  //
    __NR_linux_stat,                //
    __NR_linux_fstat,               //
    __NR_linux_lstat,               //
    __NR_linux_fstatat,             //
    __NR_linux_access,              //
    __NR_linux_faccessat,           //
    __NR_linux_faccessat2,          //
    __NR_linux_readlinkat,          //
    __NR_linux_chmod | NOBITS,      //
    __NR_linux_fchmod | NOBITS,     //
    __NR_linux_fchmodat | NOBITS,   //
};

static const uint16_t kPledgeCpath[] = {
    __NR_linux_open | CREATONLY,    //
    __NR_linux_openat | CREATONLY,  //
    __NR_linux_creat | RESTRICT,    //
    __NR_linux_rename,              //
    __NR_linux_renameat,            //
    __NR_linux_renameat2,           //
    __NR_linux_link,                //
    __NR_linux_linkat,              //
    __NR_linux_symlink,             //
    __NR_linux_symlinkat,           //
    __NR_linux_rmdir,               //
    __NR_linux_unlink,              //
    __NR_linux_unlinkat,            //
    __NR_linux_mkdir,               //
    __NR_linux_mkdirat,             //
};

static const uint16_t kPledgeDpath[] = {
    __NR_linux_mknod,    //
    __NR_linux_mknodat,  //
};

static const uint16_t kPledgeFattr[] = {
    __NR_linux_chmod | NOBITS,     //
    __NR_linux_fchmod | NOBITS,    //
    __NR_linux_fchmodat | NOBITS,  //
    __NR_linux_utime,              //
    __NR_linux_utimes,             //
    __NR_linux_futimesat,          //
    __NR_linux_utimensat,          //
};

static const uint16_t kPledgeInet[] = {
    __NR_linux_socket | INET,          //
    __NR_linux_listen,                 //
    __NR_linux_bind,                   //
    __NR_linux_sendto,                 //
    __NR_linux_connect,                //
    __NR_linux_accept,                 //
    __NR_linux_accept4,                //
    __NR_linux_getsockopt | RESTRICT,  //
    __NR_linux_setsockopt | RESTRICT,  //
    __NR_linux_getpeername,            //
    __NR_linux_getsockname,            //
};

static const uint16_t kPledgeUnix[] = {
    __NR_linux_socket | UNIX,          //
    __NR_linux_listen,                 //
    __NR_linux_bind,                   //
    __NR_linux_connect,                //
    __NR_linux_sendto,                 //
    __NR_linux_accept,                 //
    __NR_linux_accept4,                //
    __NR_linux_getsockopt | RESTRICT,  //
    __NR_linux_setsockopt | RESTRICT,  //
    __NR_linux_getpeername,            //
    __NR_linux_getsockname,            //
};

static const uint16_t kPledgeDns[] = {
    __NR_linux_socket | INET,      //
    __NR_linux_bind,               //
    __NR_linux_sendto,             //
    __NR_linux_connect,            //
    __NR_linux_recvfrom,           //
    __NR_linux_fstatat,            //
    __NR_linux_openat | READONLY,  //
    __NR_linux_read,               //
    __NR_linux_close,              //
};

static const uint16_t kPledgeTty[] = {
    __NR_linux_ioctl | TTY,  //
};

static const uint16_t kPledgeRecvfd[] = {
    __NR_linux_recvmsg,   //
    __NR_linux_recvmmsg,  //
};

static const uint16_t kPledgeSendfd[] = {
    __NR_linux_sendmsg,   //
    __NR_linux_sendmmsg,  //
};

static const uint16_t kPledgeProc[] = {
    __NR_linux_fork,                    //
    __NR_linux_vfork,                   //
    __NR_linux_clone | RESTRICT,        //
    __NR_linux_kill,                    //
    __NR_linux_setsid,                  //
    __NR_linux_setpgid,                 //
    __NR_linux_prlimit,                 //
    __NR_linux_setrlimit,               //
    __NR_linux_getpriority,             //
    __NR_linux_setpriority,             //
    __NR_linux_ioprio_get,              //
    __NR_linux_ioprio_set,              //
    __NR_linux_sched_getscheduler,      //
    __NR_linux_sched_setscheduler,      //
    __NR_linux_sched_get_priority_min,  //
    __NR_linux_sched_get_priority_max,  //
    __NR_linux_sched_getaffinity,       //
    __NR_linux_sched_setaffinity,       //
    __NR_linux_sched_getparam,          //
    __NR_linux_sched_setparam,          //
    __NR_linux_tkill,                   //
    __NR_linux_tgkill,                  //
};

static const uint16_t kPledgeId[] = {
    __NR_linux_setuid,       //
    __NR_linux_setreuid,     //
    __NR_linux_setresuid,    //
    __NR_linux_setgid,       //
    __NR_linux_setregid,     //
    __NR_linux_setresgid,    //
    __NR_linux_setgroups,    //
    __NR_linux_prlimit,      //
    __NR_linux_setrlimit,    //
    __NR_linux_getpriority,  //
    __NR_linux_setpriority,  //
    __NR_linux_setfsuid,     //
    __NR_linux_setfsgid,     //
};

static const uint16_t kPledgeSettime[] = {
    __NR_linux_settimeofday,   //
    __NR_linux_clock_adjtime,  //
};

static const uint16_t kPledgeProtExec[] = {
    __NR_linux_mmap | EXEC,  //
    __NR_linux_mprotect,     //
};

static const uint16_t kPledgeExec[] = {
    __NR_linux_execve,    //
    __NR_linux_execveat,  //
};

static const uint16_t kPledgeUnveil[] = {
    __NR_linux_landlock_create_ruleset,  //
    __NR_linux_landlock_add_rule,        //
    __NR_linux_landlock_restrict_self,   //
};

// placeholder group
//
// pledge.com checks this to do auto-unveiling
static const uint16_t kPledgeVminfo[] = {
    __NR_linux_sched_yield,  //
};

// placeholder group
//
// pledge.com uses this to auto-unveil /tmp and $TMPPATH with rwc
// permissions. pledge() alone (without unveil() too) offers very
// little security here. consider using them together.
static const uint16_t kPledgeTmppath[] = {
    __NR_linux_lstat,     //
    __NR_linux_unlink,    //
    __NR_linux_unlinkat,  //
};

const struct Pledges kPledge[PROMISE_LEN_] = {
    [PROMISE_STDIO] = {"stdio", PLEDGE(kPledgeStdio)},             //
    [PROMISE_RPATH] = {"rpath", PLEDGE(kPledgeRpath)},             //
    [PROMISE_WPATH] = {"wpath", PLEDGE(kPledgeWpath)},             //
    [PROMISE_CPATH] = {"cpath", PLEDGE(kPledgeCpath)},             //
    [PROMISE_DPATH] = {"dpath", PLEDGE(kPledgeDpath)},             //
    [PROMISE_FLOCK] = {"flock", PLEDGE(kPledgeFlock)},             //
    [PROMISE_FATTR] = {"fattr", PLEDGE(kPledgeFattr)},             //
    [PROMISE_INET] = {"inet", PLEDGE(kPledgeInet)},                //
    [PROMISE_UNIX] = {"unix", PLEDGE(kPledgeUnix)},                //
    [PROMISE_DNS] = {"dns", PLEDGE(kPledgeDns)},                   //
    [PROMISE_TTY] = {"tty", PLEDGE(kPledgeTty)},                   //
    [PROMISE_RECVFD] = {"recvfd", PLEDGE(kPledgeRecvfd)},          //
    [PROMISE_SENDFD] = {"sendfd", PLEDGE(kPledgeSendfd)},          //
    [PROMISE_PROC] = {"proc", PLEDGE(kPledgeProc)},                //
    [PROMISE_EXEC] = {"exec", PLEDGE(kPledgeExec)},                //
    [PROMISE_ID] = {"id", PLEDGE(kPledgeId)},                      //
    [PROMISE_UNVEIL] = {"unveil", PLEDGE(kPledgeUnveil)},          //
    [PROMISE_SETTIME] = {"settime", PLEDGE(kPledgeSettime)},       //
    [PROMISE_PROT_EXEC] = {"prot_exec", PLEDGE(kPledgeProtExec)},  //
    [PROMISE_VMINFO] = {"vminfo", PLEDGE(kPledgeVminfo)},          //
    [PROMISE_TMPPATH] = {"tmppath", PLEDGE(kPledgeTmppath)},       //
};

static const struct sock_filter kPledgeStart[] = {
    // make sure this isn't an i386 binary or something
    BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(arch)),
    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, AUDIT_ARCH_X86_64, 1, 0),
    BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_KILL_PROCESS),
    // each filter assumes ordinal is already loaded into accumulator
    BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
    // forbid some system calls with ENOSYS (rather than EPERM)
    BPF_JUMP(BPF_JMP | BPF_JGE | BPF_K, __NR_linux_memfd_secret, 5, 0),
    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_rseq, 4, 0),
    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_memfd_create, 3, 0),
    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_openat2, 2, 0),
    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_clone3, 1, 0),
    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_statx, 0, 1),
    BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ERRNO | (Enosys & SECCOMP_RET_DATA)),
};

static const struct sock_filter kFilterIgnoreExitGroup[] = {
    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_exit_group, 0, 1),
    BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ERRNO | (Eperm & SECCOMP_RET_DATA)),
};

static privileged unsigned long StrLen(const char *s) {
  unsigned long n = 0;
  while (*s++) ++n;
  return n;
}

static privileged void *MemCpy(void *d, const void *s, unsigned long n) {
  unsigned long i = 0;
  for (; i < n; ++i) ((char *)d)[i] = ((char *)s)[i];
  return (char *)d + n;
}

static privileged char *FixCpy(char p[17], uint64_t x, uint8_t k) {
  while (k > 0) *p++ = "0123456789abcdef"[(x >> (k -= 4)) & 15];
  *p = '\0';
  return p;
}

static privileged char *HexCpy(char p[17], uint64_t x) {
  return FixCpy(p, x, ROUNDUP(x ? bsrl(x) + 1 : 1, 4));
}

static privileged int GetPid(void) {
  int ax;
  asm volatile("syscall"
               : "=a"(ax)
               : "0"(__NR_linux_getpid)
               : "rcx", "r11", "memory");
  return ax;
}

static privileged int GetTid(void) {
  int ax;
  asm volatile("syscall"
               : "=a"(ax)
               : "0"(__NR_linux_gettid)
               : "rcx", "r11", "memory");
  return ax;
}

static privileged void Log(const char *s, ...) {
  int ax;
  va_list va;
  va_start(va, s);
  do {
    asm volatile("syscall"
                 : "=a"(ax)
                 : "0"(__NR_linux_write), "D"(2), "S"(s), "d"(StrLen(s))
                 : "rcx", "r11", "memory");
  } while ((s = va_arg(va, const char *)));
  va_end(va);
}

static privileged int Prctl(int op, long a, void *b, long c, long d) {
  int rc;
  va_list va;
  asm volatile("mov\t%5,%%r10\n\t"
               "mov\t%6,%%r8\n\t"
               "syscall"
               : "=a"(rc)
               : "0"(__NR_linux_prctl), "D"(op), "S"(a), "d"(b), "g"(c), "g"(d)
               : "rcx", "r8", "r10", "r11", "memory");
  return rc;
}

static privileged int SigAction(int sig, struct sigaction *act,
                                struct sigaction *old) {
  int ax;
  act->sa_flags |= Sa_Restorer;
  act->sa_restorer = &__restore_rt;
  asm volatile("mov\t%5,%%r10\n\t"
               "syscall"
               : "=a"(ax)
               : "0"(__NR_linux_sigaction), "D"(sig), "S"(act), "d"(old), "g"(8)
               : "rcx", "r10", "r11", "memory");
  return ax;
}

static privileged int SigProcMask(int how, int64_t set, int64_t *old) {
  int ax;
  asm volatile("mov\t%5,%%r10\n\t"
               "syscall"
               : "=a"(ax)
               : "0"(__NR_linux_sigprocmask), "D"(how), "S"(&set), "d"(old),
                 "g"(8)
               : "rcx", "r10", "r11", "memory");
  return ax;
}

static privileged void KillThisProcess(void) {
  int ax;
  struct sigaction dfl = {.sa_sigaction = SIG_DFL};
  if (!SigAction(Sigabrt, &dfl, 0)) {
    SigProcMask(Sig_Setmask, -1, 0);
    asm volatile("syscall"
                 : "=a"(ax)
                 : "0"(__NR_linux_kill), "D"(GetPid()), "S"(Sigabrt)
                 : "rcx", "r11", "memory");
    SigProcMask(Sig_Setmask, 0, 0);
  }
  asm volatile("syscall"
               : "=a"(ax)
               : "0"(__NR_linux_exit_group), "D"(128 + Sigabrt)
               : "rcx", "r11", "memory");
}

static privileged void KillThisThread(void) {
  int ax;
  struct sigaction dfl = {.sa_sigaction = SIG_DFL};
  if (!SigAction(Sigabrt, &dfl, 0)) {
    SigProcMask(Sig_Setmask, -1, 0);
    asm volatile("syscall"
                 : "=a"(ax)
                 : "0"(__NR_linux_tkill), "D"(GetTid()), "S"(Sigabrt)
                 : "rcx", "r11", "memory");
    SigProcMask(Sig_Setmask, 0, 0);
  }
  asm volatile("syscall"
               : /* no outputs */
               : "a"(__NR_linux_exit), "D"(128 + Sigabrt)
               : "rcx", "r11", "memory");
}

static privileged bool HasSyscall(struct Pledges *p, uint16_t n) {
  int i;
  for (i = 0; i < p->len; ++i) {
    if ((p->syscalls[i] & 0x0fff) == n) {
      return true;
    }
  }
  return false;
}

static privileged void OnSigSys(int sig, siginfo_t *si, ucontext_t *ctx) {
  int i;
  bool found;
  char ord[17], rip[17];
  enum PledgeMode mode = si->si_errno;
  ctx->uc_mcontext.rax = -Eperm;
  FixCpy(ord, si->si_syscall, 12);
  HexCpy(rip, ctx->uc_mcontext.rip);
  for (found = i = 0; i < ARRAYLEN(kPledge); ++i) {
    if (HasSyscall(kPledge + i, si->si_syscall)) {
      Log("error: has not pledged ", kPledge[i].name,  //
          " (ord=", ord, " rip=", rip, ")\n", 0);
      found = true;
      break;
    }
  }
  if (!found) {
    Log("error: unsupported syscall (ord=", ord, " rip=", rip, ")\n", 0);
  }
  switch (mode) {
    case kPledgeModeKillProcess:
      KillThisProcess();
      // fallthrough
    case kPledgeModeKillThread:
      KillThisThread();
      unreachable;
    default:
      break;
  }
}

static privileged void MonitorSigSys(void) {
  int ax;
  struct sigaction sa = {
      .sa_sigaction = OnSigSys,
      .sa_flags = Sa_Siginfo | Sa_Restart,
  };
  // we block changing sigsys once pledge is installed
  // so we aren't terribly concerned if this will fail
  SigAction(Sigsys, &sa, 0);
}

static privileged void AppendFilter(struct Filter *f, struct sock_filter *p,
                                    size_t n) {
  if (UNLIKELY(f->n + n > ARRAYLEN(f->p))) {
    AbortPledge("need to increase array size");
  }
  MemCpy(f->p + f->n, p, n * sizeof(*f->p));
  f->n += n;
}

// The following system calls are allowed:
//
//   - write(2) to allow logging
//   - kill(getpid(), SIGABRT) to abort process
//   - tkill(gettid(), SIGABRT) to abort thread
//   - sigaction(SIGABRT) to force default signal handler
//   - sigprocmask() to force signal delivery
//
static privileged void AllowMonitor(struct Filter *f) {
  struct sock_filter fragment[] = {
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_write, 0, 4),
      BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[0])),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 2, 0, 1),
      BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_kill, 0, 6),
      BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[0])),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, GetPid(), 0, 3),
      BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[1])),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, Sigabrt, 0, 1),
      BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_tkill, 0, 6),
      BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[0])),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, GetTid(), 0, 3),
      BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[1])),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, Sigabrt, 0, 1),
      BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_sigaction, 0, 4),
      BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[0])),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, Sigabrt, 0, 1),
      BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_sigprocmask, 0, 1),
      BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
  };
  AppendFilter(f, PLEDGE(fragment));
}

// SYSCALL is only allowed in the .privileged section
// We assume program image is loaded in 32-bit spaces
static privileged void AppendOriginVerification(struct Filter *f) {
  long x = (long)__privileged_start;
  long y = (long)__privileged_end;
  struct sock_filter fragment[] = {
      /*L0*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(instruction_pointer) + 4),
      /*L1*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 0, 5 - 2),
      /*L2*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(instruction_pointer)),
      /*L3*/ BPF_JUMP(BPF_JMP | BPF_JGE | BPF_K, x, 0, 5 - 4),
      /*L4*/ BPF_JUMP(BPF_JMP | BPF_JGE | BPF_K, y, 0, 6 - 5),
      /*L5*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_KILL),
      /*L6*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L7*/ /* next filter */
  };
  AppendFilter(f, PLEDGE(fragment));
}

// The first argument of sys_clone_linux() must NOT have:
//
//   - CLONE_NEWNS    (0x00020000)
//   - CLONE_PTRACE   (0x00002000)
//   - CLONE_UNTRACED (0x00800000)
//
static privileged void AllowCloneRestrict(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /*L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_clone, 0, 6 - 1),
      /*L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[0])),
      /*L2*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, 0x00822000),
      /*L3*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 0, 1),
      /*L4*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L5*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L6*/ /* next filter */
  };
  AppendFilter(f, PLEDGE(fragment));
}

// The first argument of sys_clone_linux() must have:
//
//   - CLONE_VM       (0x00000100)
//   - CLONE_FS       (0x00000200)
//   - CLONE_FILES    (0x00000400)
//   - CLONE_THREAD   (0x00010000)
//   - CLONE_SIGHAND  (0x00000800)
//
// The first argument of sys_clone_linux() must NOT have:
//
//   - CLONE_NEWNS    (0x00020000)
//   - CLONE_PTRACE   (0x00002000)
//   - CLONE_UNTRACED (0x00800000)
//
static privileged void AllowCloneThread(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /*L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_clone, 0, 9 - 1),
      /*L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[0])),
      /*L2*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, 0x00010f00),
      /*L3*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x00010f00, 0, 8 - 4),
      /*L4*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[0])),
      /*L5*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, 0x00822000),
      /*L6*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 0, 1),
      /*L7*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L8*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L9*/ /* next filter */
  };
  AppendFilter(f, PLEDGE(fragment));
}

// The second argument of ioctl() must be one of:
//
//   - FIONREAD (0x541b)
//   - FIONBIO  (0x5421)
//   - FIOCLEX  (0x5451)
//   - FIONCLEX (0x5450)
//
static privileged void AllowIoctlStdio(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /*L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_ioctl, 0, 8 - 1),
      /*L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[1])),
      /*L2*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x541b, 3, 0),
      /*L3*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x5421, 2, 0),
      /*L4*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x5451, 1, 0),
      /*L5*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x5450, 0, 1),
      /*L6*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L7*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L8*/ /* next filter */
  };
  AppendFilter(f, PLEDGE(fragment));
}

// The second argument of ioctl() must be one of:
//
//   - TCGETS     (0x5401)
//   - TCSETS     (0x5402)
//   - TCSETSW    (0x5403)
//   - TCSETSF    (0x5404)
//   - TIOCGWINSZ (0x5413)
//   - TIOCSPGRP  (0x5410)
//   - TIOCGPGRP  (0x540f)
//   - TIOCSWINSZ (0x5414)
//   - TCFLSH     (0x540b)
//   - TCXONC     (0x540a)
//   - TCSBRK     (0x5409)
//   - TIOCSBRK   (0x5427)
//
static privileged void AllowIoctlTty(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /* L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_ioctl, 0, 16 - 1),
      /* L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[1])),
      /* L2*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x5401, 11, 0),
      /* L3*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x5402, 10, 0),
      /* L4*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x5403, 9, 0),
      /* L5*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x5404, 8, 0),
      /* L6*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x5413, 7, 0),
      /* L7*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x5410, 6, 0),
      /* L8*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x540f, 5, 0),
      /* L9*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x5414, 4, 0),
      /*L10*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x540b, 3, 0),
      /*L11*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x540a, 2, 0),
      /*L12*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x5409, 1, 0),
      /*L13*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x5427, 0, 1),
      /*L14*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L15*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L16*/ /* next filter */
  };
  AppendFilter(f, PLEDGE(fragment));
}

// The level argument of setsockopt() must be one of:
//
//   - SOL_IP     (0)
//   - SOL_SOCKET (1)
//   - SOL_TCP    (6)
//
// The optname argument of setsockopt() must be one of:
//
//   - TCP_NODELAY          (0x01)
//   - TCP_CORK             (0x03)
//   - TCP_KEEPIDLE         (0x04)
//   - TCP_KEEPINTVL        (0x05)
//   - SO_TYPE              (0x03)
//   - SO_ERROR             (0x04)
//   - SO_DONTROUTE         (0x05)
//   - SO_REUSEPORT         (0x0f)
//   - SO_REUSEADDR         (0x02)
//   - SO_KEEPALIVE         (0x09)
//   - SO_RCVTIMEO          (0x14)
//   - SO_SNDTIMEO          (0x15)
//   - IP_RECVTTL           (0x0c)
//   - IP_RECVERR           (0x0b)
//   - TCP_FASTOPEN         (0x17)
//   - TCP_FASTOPEN_CONNECT (0x1e)
//
static privileged void AllowSetsockoptRestrict(struct Filter *f) {
  static const int nr = __NR_linux_setsockopt;
  static const struct sock_filter fragment[] = {
      /* L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, nr, 0, 21 - 1),
      /* L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[1])),
      /* L2*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 2, 0),
      /* L3*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 1, 1, 0),
      /* L4*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 6, 0, 20 - 5),
      /* L5*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[2])),
      /* L6*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x0f, 13, 0),
      /* L6*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x03, 12, 0),
      /* L7*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x0c, 11, 0),
      /* L8*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x13, 10, 0),
      /* L9*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x02, 9, 0),
      /*L10*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x09, 8, 0),
      /*L11*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x14, 7, 0),
      /*L12*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x01, 6, 0),
      /*L13*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x0b, 5, 0),
      /*L14*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x04, 4, 0),
      /*L15*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x05, 3, 0),
      /*L16*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x17, 2, 0),
      /*L17*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x1e, 1, 0),
      /*L18*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x15, 0, 1),
      /*L19*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L20*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L21*/ /* next filter */
  };
  AppendFilter(f, PLEDGE(fragment));
}

// The level argument of getsockopt() must be one of:
//
//   - SOL_SOCKET (1)
//   - SOL_TCP    (6)
//
// The optname argument of getsockopt() must be one of:
//
//   - SO_TYPE      (0x03)
//   - SO_REUSEPORT (0x0f)
//   - SO_REUSEADDR (0x02)
//   - SO_KEEPALIVE (0x09)
//   - SO_RCVTIMEO  (0x14)
//   - SO_SNDTIMEO  (0x15)
//
static privileged void AllowGetsockoptRestrict(struct Filter *f) {
  static const int nr = __NR_linux_getsockopt;
  static const struct sock_filter fragment[] = {
      /* L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, nr, 0, 13 - 1),
      /* L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[1])),
      /* L2*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 1, 1, 0),
      /* L3*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 6, 0, 12 - 4),
      /* L4*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[2])),
      /* L5*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x03, 5, 0),
      /* L6*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x0f, 4, 0),
      /* L7*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x02, 3, 0),
      /* L8*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x09, 2, 0),
      /* L9*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x14, 1, 0),
      /*L10*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x15, 0, 1),
      /*L11*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L12*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L13*/ /* next filter */
  };
  AppendFilter(f, PLEDGE(fragment));
}

// The flags parameter of mmap() must not have:
//
//   - MAP_LOCKED   (0x02000)
//   - MAP_NONBLOCK (0x10000)
//   - MAP_HUGETLB  (0x40000)
//
static privileged void AllowMmapExec(struct Filter *f) {
  long y = (long)__privileged_end;
  static const struct sock_filter fragment[] = {
      /*L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_mmap, 0, 6 - 1),
      /*L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[3])),  // flags
      /*L2*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, 0x52000),
      /*L3*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 0, 5 - 4),
      /*L4*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L5*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L6*/ /* next filter */
  };
  AppendFilter(f, PLEDGE(fragment));
}

// The prot parameter of mmap() may only have:
//
//   - PROT_NONE  (0)
//   - PROT_READ  (1)
//   - PROT_WRITE (2)
//
// The flags parameter must not have:
//
//   - MAP_LOCKED   (0x02000)
//   - MAP_POPULATE (0x08000)
//   - MAP_NONBLOCK (0x10000)
//   - MAP_HUGETLB  (0x40000)
//
static privileged void AllowMmapNoexec(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /*L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_mmap, 0, 9 - 1),
      /*L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[2])),  // prot
      /*L2*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, ~(PROT_READ | PROT_WRITE)),
      /*L3*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 0, 8 - 4),
      /*L4*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[3])),  // flags
      /*L5*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, 0x5a000),
      /*L6*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 0, 1),
      /*L7*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L8*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L9*/ /* next filter */
  };
  AppendFilter(f, PLEDGE(fragment));
}

// The prot parameter of mprotect() may only have:
//
//   - PROT_NONE  (0)
//   - PROT_READ  (1)
//   - PROT_WRITE (2)
//
static privileged void AllowMprotectNoexec(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /*L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_mprotect, 0, 6 - 1),
      /*L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[2])),  // prot
      /*L2*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, ~(PROT_READ | PROT_WRITE)),
      /*L3*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 0, 1),
      /*L4*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L5*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L6*/ /* next filter */
  };
  AppendFilter(f, PLEDGE(fragment));
}

// The open() system call is permitted only when
//
//   - (flags & O_ACCMODE) == O_RDONLY
//
// The flags parameter of open() must not have:
//
//   - O_CREAT     (000000100)
//   - O_TRUNC     (000001000)
//   - __O_TMPFILE (020000000)
//
static privileged void AllowOpenReadonly(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /*L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_open, 0, 9 - 1),
      /*L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[1])),
      /*L2*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, O_ACCMODE),
      /*L3*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, O_RDONLY, 0, 8 - 4),
      /*L4*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[1])),
      /*L5*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, 020001100),
      /*L6*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 0, 1),
      /*L7*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L8*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L9*/ /* next filter */
  };
  AppendFilter(f, PLEDGE(fragment));
}

// The open() system call is permitted only when
//
//   - (flags & O_ACCMODE) == O_RDONLY
//
// The flags parameter of open() must not have:
//
//   - O_CREAT     (000000100)
//   - O_TRUNC     (000001000)
//   - __O_TMPFILE (020000000)
//
static privileged void AllowOpenatReadonly(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /*L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_openat, 0, 9 - 1),
      /*L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[2])),
      /*L2*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, O_ACCMODE),
      /*L3*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, O_RDONLY, 0, 8 - 4),
      /*L4*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[2])),
      /*L5*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, 020001100),
      /*L6*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 0, 1),
      /*L7*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L8*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L9*/ /* next filter */
  };
  AppendFilter(f, PLEDGE(fragment));
}

// The open() system call is permitted only when
//
//   - (flags & O_ACCMODE) == O_WRONLY
//   - (flags & O_ACCMODE) == O_RDWR
//
// The open() flags parameter must not contain
//
//   - O_CREAT     (000000100)
//   - __O_TMPFILE (020000000)
//
static privileged void AllowOpenWriteonly(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /* L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_open, 0, 10 - 1),
      /* L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[1])),
      /* L2*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, O_ACCMODE),
      /* L3*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, O_WRONLY, 1, 0),
      /* L4*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, O_RDWR, 0, 9 - 5),
      /* L5*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[1])),
      /* L6*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, 020000100),
      /* L7*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 0, 1),
      /* L8*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /* L9*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L10*/ /* next filter */
  };
  AppendFilter(f, PLEDGE(fragment));
}

// The open() system call is permitted only when
//
//   - (flags & O_ACCMODE) == O_WRONLY
//   - (flags & O_ACCMODE) == O_RDWR
//
// The openat() flags parameter must not contain
//
//   - O_CREAT     (000000100)
//   - __O_TMPFILE (020000000)
//
static privileged void AllowOpenatWriteonly(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /* L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_openat, 0, 10 - 1),
      /* L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[2])),
      /* L2*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, O_ACCMODE),
      /* L3*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, O_WRONLY, 1, 0),
      /* L4*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, O_RDWR, 0, 9 - 5),
      /* L5*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[2])),
      /* L6*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, 020000100),
      /* L7*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 0, 1),
      /* L8*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /* L9*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L10*/ /* next filter */
  };
  AppendFilter(f, PLEDGE(fragment));
}

// If the flags parameter of open() has one of:
//
//   - O_CREAT     (000000100)
//   - __O_TMPFILE (020000000)
//
// Then the mode parameter must not have:
//
//   - S_ISVTX (01000 sticky)
//   - S_ISGID (02000 setgid)
//   - S_ISUID (04000 setuid)
//
static privileged void AllowOpenCreatonly(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /* L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_open, 0, 12 - 1),
      /* L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[1])),
      /* L2*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, 000000100),
      /* L3*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 000000100, 7 - 4, 0),
      /* L4*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[1])),
      /* L5*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, 020200000),
      /* L6*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 020200000, 0, 10 - 7),
      /* L7*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[2])),
      /* L8*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, 07000),
      /* L9*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 0, 1),
      /*L10*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L11*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L12*/ /* next filter */
  };
  AppendFilter(f, PLEDGE(fragment));
}

// If the flags parameter of openat() has one of:
//
//   - O_CREAT     (000000100)
//   - __O_TMPFILE (020000000)
//
// Then the mode parameter must not have:
//
//   - S_ISVTX (01000 sticky)
//   - S_ISGID (02000 setgid)
//   - S_ISUID (04000 setuid)
//
static privileged void AllowOpenatCreatonly(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /* L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_openat, 0, 12 - 1),
      /* L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[2])),
      /* L2*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, 000000100),
      /* L3*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 000000100, 7 - 4, 0),
      /* L4*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[2])),
      /* L5*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, 020200000),
      /* L6*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 020200000, 0, 10 - 7),
      /* L7*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[3])),
      /* L8*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, 07000),
      /* L9*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 0, 1),
      /*L10*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L11*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L12*/ /* next filter */
  };
  AppendFilter(f, PLEDGE(fragment));
}

// Then the mode parameter must not have:
//
//   - S_ISVTX (01000 sticky)
//   - S_ISGID (02000 setgid)
//   - S_ISUID (04000 setuid)
//
static privileged void AllowCreatRestrict(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /*L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_creat, 0, 6 - 1),
      /*L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[1])),
      /*L2*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, 07000),
      /*L3*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 0, 1),
      /*L4*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L5*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L6*/ /* next filter */
  };
  AppendFilter(f, PLEDGE(fragment));
}

// The second argument of fcntl() must be one of:
//
//   - F_DUPFD (0)
//   - F_DUPFD_CLOEXEC (1030)
//   - F_GETFD (1)
//   - F_SETFD (2)
//   - F_GETFL (3)
//   - F_SETFL (4)
//
static privileged void AllowFcntlStdio(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /*L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_fcntl, 0, 6 - 1),
      /*L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[1])),
      /*L2*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 1030, 4 - 3, 0),
      /*L3*/ BPF_JUMP(BPF_JMP | BPF_JGE | BPF_K, 5, 5 - 4, 0),
      /*L4*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L5*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L6*/ /* next filter */
  };
  AppendFilter(f, PLEDGE(fragment));
}

// The second argument of fcntl() must be one of:
//
//   - F_GETLK (5)
//   - F_SETLK (6)
//   - F_SETLKW (7)
//
static privileged void AllowFcntlLock(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /*L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_fcntl, 0, 6 - 1),
      /*L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[1])),
      /*L2*/ BPF_JUMP(BPF_JMP | BPF_JGE | BPF_K, 5, 0, 5 - 3),
      /*L3*/ BPF_JUMP(BPF_JMP | BPF_JGE | BPF_K, 8, 5 - 4, 0),
      /*L4*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L5*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L6*/ /* next filter */
  };
  AppendFilter(f, PLEDGE(fragment));
}

// The addr parameter of sendto() must be
//
//   - NULL
//
static privileged void AllowSendtoAddrless(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /*L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_sendto, 0, 7 - 1),
      /*L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[4]) + 0),
      /*L2*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 0, 6 - 3),
      /*L3*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[4]) + 4),
      /*L4*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 0, 6 - 5),
      /*L5*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L6*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L7*/ /* next filter */
  };
  AppendFilter(f, PLEDGE(fragment));
}

// The sig parameter of sigaction() must NOT be
//
//   - SIGSYS (31) [always eperm]
//
static privileged void AllowSigactionNosigsys(struct Filter *f) {
  static const int nr = __NR_linux_sigaction;
  static const struct sock_filter fragment[] = {
      /*L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, nr, 0, 6 - 1),
      /*L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[0])),
      /*L2*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 31, 0, 1),
      /*L3*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ERRNO | Eperm),
      /*L4*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L5*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L6*/ /* next filter */
  };
  AppendFilter(f, PLEDGE(fragment));
}

// The family parameter of socket() must be one of:
//
//   - AF_INET  (0x02)
//   - AF_INET6 (0x0a)
//
// The type parameter of socket() will ignore:
//
//   - SOCK_CLOEXEC  (0x80000)
//   - SOCK_NONBLOCK (0x00800)
//
// The type parameter of socket() must be one of:
//
//   - SOCK_STREAM (0x01)
//   - SOCK_DGRAM  (0x02)
//
// The protocol parameter of socket() must be one of:
//
//   - 0
//   - IPPROTO_ICMP (0x01)
//   - IPPROTO_TCP  (0x06)
//   - IPPROTO_UDP  (0x11)
//
static privileged void AllowSocketInet(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /* L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_socket, 0, 15 - 1),
      /* L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[0])),
      /* L2*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x02, 1, 0),
      /* L3*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x0a, 0, 14 - 4),
      /* L4*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[1])),
      /* L5*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, ~0x80800),
      /* L6*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x01, 1, 0),
      /* L7*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x02, 0, 14 - 8),
      /* L8*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[2])),
      /* L9*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x00, 3, 0),
      /*L10*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x01, 2, 0),
      /*L11*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x06, 1, 0),
      /*L12*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x11, 0, 1),
      /*L13*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L14*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L15*/ /* next filter */
  };
  AppendFilter(f, PLEDGE(fragment));
}

// The family parameter of socket() must be one of:
//
//   - AF_UNIX (1)
//   - AF_LOCAL (1)
//
// The type parameter of socket() will ignore:
//
//   - SOCK_CLOEXEC  (0x80000)
//   - SOCK_NONBLOCK (0x00800)
//
// The type parameter of socket() must be one of:
//
//   - SOCK_STREAM (1)
//   - SOCK_DGRAM  (2)
//
// The protocol parameter of socket() must be one of:
//
//   - 0
//
static privileged void AllowSocketUnix(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /* L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_socket, 0, 11 - 1),
      /* L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[0])),
      /* L2*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 1, 0, 10 - 3),
      /* L3*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[1])),
      /* L5*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, ~0x80800),
      /* L5*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 1, 1, 0),
      /* L6*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 2, 0, 10 - 7),
      /* L7*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[2])),
      /* L8*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 0, 1),
      /* L9*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L10*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L11*/ /* next filter */
  };
  AppendFilter(f, PLEDGE(fragment));
}

// The first parameter of prctl() can be any of
//
//   - PR_SET_NAME         (15)
//   - PR_GET_NAME         (16)
//   - PR_GET_SECCOMP      (21)
//   - PR_SET_SECCOMP      (22)
//   - PR_SET_NO_NEW_PRIVS (38)
//   - PR_CAPBSET_READ     (23)
//   - PR_CAPBSET_DROP     (24)
//
static privileged void AllowPrctlStdio(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /* L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_prctl, 0, 11 - 1),
      /* L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[0])),
      /* L2*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 15, 6, 0),
      /* L3*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 16, 5, 0),
      /* L4*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 21, 4, 0),
      /* L5*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 22, 3, 0),
      /* L6*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 23, 2, 0),
      /* L7*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 24, 1, 0),
      /* L8*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 38, 0, 1),
      /* L9*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L10*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L11*/ /* next filter */
  };
  AppendFilter(f, PLEDGE(fragment));
}

// The mode parameter of chmod() can't have the following:
//
//   - S_ISVTX (01000 sticky)
//   - S_ISGID (02000 setgid)
//   - S_ISUID (04000 setuid)
//
static privileged void AllowChmodNobits(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /*L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_chmod, 0, 6 - 1),
      /*L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[1])),
      /*L2*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, 07000),
      /*L3*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 0, 1),
      /*L4*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L5*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L6*/ /* next filter */
  };
  AppendFilter(f, PLEDGE(fragment));
}

// The mode parameter of fchmod() can't have the following:
//
//   - S_ISVTX (01000 sticky)
//   - S_ISGID (02000 setgid)
//   - S_ISUID (04000 setuid)
//
static privileged void AllowFchmodNobits(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /*L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_fchmod, 0, 6 - 1),
      /*L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[1])),
      /*L2*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, 07000),
      /*L3*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 0, 1),
      /*L4*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L5*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L6*/ /* next filter */
  };
  AppendFilter(f, PLEDGE(fragment));
}

// The mode parameter of fchmodat() can't have the following:
//
//   - S_ISVTX (01000 sticky)
//   - S_ISGID (02000 setgid)
//   - S_ISUID (04000 setuid)
//
static privileged void AllowFchmodatNobits(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /*L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_fchmodat, 0, 6 - 1),
      /*L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[2])),
      /*L2*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, 07000),
      /*L3*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 0, 1),
      /*L4*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L5*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L6*/ /* next filter */
  };
  AppendFilter(f, PLEDGE(fragment));
}

// The new_limit parameter of prlimit() must be
//
//   - NULL (0)
//
static privileged void AllowPrlimitStdio(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /*L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_prlimit, 0, 7 - 1),
      /*L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[2])),
      /*L2*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 0, 6 - 3),
      /*L3*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[2]) + 4),
      /*L4*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 0, 1),
      /*L5*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L6*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L7*/ /* next filter */
  };
  AppendFilter(f, PLEDGE(fragment));
}

static privileged int CountUnspecial(const uint16_t *p, size_t len) {
  int i, count;
  for (count = i = 0; i < len; ++i) {
    if (!(p[i] & SPECIAL)) {
      ++count;
    }
  }
  return count;
}

static privileged void AppendPledge(struct Filter *f,   //
                                    const uint16_t *p,  //
                                    size_t len) {       //
  int i, j, count;

  // handle ordinals which allow syscalls regardless of args
  // we put in extra effort here to reduce num of bpf instrs
  if ((count = CountUnspecial(p, len))) {
    if (count < 256) {
      for (j = i = 0; i < len; ++i) {
        if (p[i] & SPECIAL) continue;
        // jump to ALLOW rule below if accumulator equals ordinal
        struct sock_filter fragment[] = {
            BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K,  // instruction
                     p[i],                       // operand
                     count - j - 1,              // jump if true displacement
                     j == count - 1),            // jump if false displacement
        };
        AppendFilter(f, PLEDGE(fragment));
        ++j;
      }
      struct sock_filter fragment[] = {
          BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      };
      AppendFilter(f, PLEDGE(fragment));
    } else {
      AbortPledge("list of ordinals exceeds max displacement");
    }
  }

  // handle "special" ordinals which use hand-crafted bpf
  for (i = 0; i < len; ++i) {
    if (!(p[i] & SPECIAL)) continue;
    switch (p[i]) {
      case __NR_linux_mmap | EXEC:
        AllowMmapExec(f);
        break;
      case __NR_linux_mmap | NOEXEC:
        AllowMmapNoexec(f);
        break;
      case __NR_linux_mprotect | NOEXEC:
        AllowMprotectNoexec(f);
        break;
      case __NR_linux_chmod | NOBITS:
        AllowChmodNobits(f);
        break;
      case __NR_linux_fchmod | NOBITS:
        AllowFchmodNobits(f);
        break;
      case __NR_linux_fchmodat | NOBITS:
        AllowFchmodatNobits(f);
        break;
      case __NR_linux_sigaction | NOSIGSYS:
        AllowSigactionNosigsys(f);
        break;
      case __NR_linux_prctl | STDIO:
        AllowPrctlStdio(f);
        break;
      case __NR_linux_open | CREATONLY:
        AllowOpenCreatonly(f);
        break;
      case __NR_linux_openat | CREATONLY:
        AllowOpenatCreatonly(f);
        break;
      case __NR_linux_open | READONLY:
        AllowOpenReadonly(f);
        break;
      case __NR_linux_openat | READONLY:
        AllowOpenatReadonly(f);
        break;
      case __NR_linux_open | WRITEONLY:
        AllowOpenWriteonly(f);
        break;
      case __NR_linux_openat | WRITEONLY:
        AllowOpenatWriteonly(f);
        break;
      case __NR_linux_setsockopt | RESTRICT:
        AllowSetsockoptRestrict(f);
        break;
      case __NR_linux_getsockopt | RESTRICT:
        AllowGetsockoptRestrict(f);
        break;
      case __NR_linux_creat | RESTRICT:
        AllowCreatRestrict(f);
        break;
      case __NR_linux_fcntl | STDIO:
        AllowFcntlStdio(f);
        break;
      case __NR_linux_fcntl | LOCK:
        AllowFcntlLock(f);
        break;
      case __NR_linux_ioctl | RESTRICT:
        AllowIoctlStdio(f);
        break;
      case __NR_linux_ioctl | TTY:
        AllowIoctlTty(f);
        break;
      case __NR_linux_socket | INET:
        AllowSocketInet(f);
        break;
      case __NR_linux_socket | UNIX:
        AllowSocketUnix(f);
        break;
      case __NR_linux_sendto | ADDRLESS:
        AllowSendtoAddrless(f);
        break;
      case __NR_linux_clone | RESTRICT:
        AllowCloneRestrict(f);
        break;
      case __NR_linux_clone | THREAD:
        AllowCloneThread(f);
        break;
      case __NR_linux_prlimit | STDIO:
        AllowPrlimitStdio(f);
        break;
      default:
        AbortPledge("switch forgot to define a special ordinal");
    }
  }
}

/**
 * Installs SECCOMP BPF filter on Linux thread.
 *
 * @param ipromises is inverted integer bitmask of pledge() promises
 * @param mode configures the course of action on sandbox violations
 * @param want_msyscall if set will cause syscall origin checking to be
 *     enabled, but only if `exec` hasn't been pledged
 * @return 0 on success, or negative error number on error
 * @asyncsignalsafe
 * @threadsafe
 * @vforksafe
 */
privileged int sys_pledge_linux(unsigned long ipromises,  //
                                enum PledgeMode mode,     //
                                bool want_msyscall) {     //
  int i, rc = -1;
  struct Filter f;
  struct sock_filter sf[1] = {BPF_STMT(BPF_RET | BPF_K, 0)};
  CheckLargeStackAllocation(&f, sizeof(f));
  f.n = 0;

  // set up the seccomp filter
  AppendFilter(&f, PLEDGE(kPledgeStart));
  if (ipromises == -1) {
    // if we're pledging empty string, then avoid triggering a sigsys
    // when _Exit() gets called since we need to fallback to _Exit1()
    AppendFilter(&f, PLEDGE(kFilterIgnoreExitGroup));
  }
  if (want_msyscall && !(~ipromises & (1ul << PROMISE_EXEC))) {
    AppendOriginVerification(&f);
  }
  AppendPledge(&f, PLEDGE(kPledgeDefault));
  for (i = 0; i < ARRAYLEN(kPledge); ++i) {
    if (~ipromises & (1ul << i)) {
      AppendPledge(&f, kPledge[i].syscalls, kPledge[i].len);
    }
  }

  // now determine what we'll do on sandbox violations
  if (~ipromises & (1ul << PROMISE_EXEC)) {
    // our sigsys error message handler can't be inherited across
    // execve() boundaries so if you've pledged exec then that'll
    // mean no error messages for you.
    switch (mode) {
      case kPledgeModeKillThread:
        sf[0].k = SECCOMP_RET_KILL_THREAD;
        break;
      case kPledgeModeKillProcess:
        sf[0].k = SECCOMP_RET_KILL_PROCESS;
        break;
      case kPledgeModeErrno:
        sf[0].k = SECCOMP_RET_ERRNO | Eperm;
        break;
      default:
        unreachable;
    }
    AppendFilter(&f, PLEDGE(sf));
  } else {
    // if we haven't pledged exec, then we can monitor SIGSYS
    // and print a helpful error message when things do break
    // to avoid tls / static memory, we embed mode within bpf
    MonitorSigSys();
    AllowMonitor(&f);
    sf[0].k = SECCOMP_RET_TRAP | (mode & SECCOMP_RET_DATA);
    AppendFilter(&f, PLEDGE(sf));
  }

  // register our seccomp filter with the kernel
  if ((rc = Prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0)) != -1) {
    struct sock_fprog sandbox = {.len = f.n, .filter = f.p};
    rc = Prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &sandbox, 0, 0);
  }

  return rc;
}
