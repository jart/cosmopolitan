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
#include "libc/calls/calls.h"
#include "libc/calls/strace.internal.h"
#include "libc/calls/struct/bpf.h"
#include "libc/calls/struct/filter.h"
#include "libc/calls/struct/seccomp.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/kprintf.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/audit.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/nrlinux.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/pr.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/errfuns.h"

#define READONLY 0x8000
#define ADDRLESS 0x8000
#define INET     0x8000
#define UNIX     0x4000
#define LOCK     0x8000
#define TTY      0x8000

#define OFF(f)         offsetof(struct seccomp_data, f)
#define PLEDGE(pledge) pledge, ARRAYLEN(pledge)

struct Filter {
  size_t n;
  struct sock_filter *p;
};

static const uint16_t kPledgeLinuxDefault[] = {
    __NR_linux_exit,        //
    __NR_linux_exit_group,  //
};

static const uint16_t kPledgeLinuxStdio[] = {
    __NR_linux_clock_gettime,      //
    __NR_linux_clock_getres,       //
    __NR_linux_close,              //
    __NR_linux_write,              //
    __NR_linux_writev,             //
    __NR_linux_pwrite,             //
    __NR_linux_pwritev,            //
    __NR_linux_pwritev2,           //
    __NR_linux_read,               //
    __NR_linux_readv,              //
    __NR_linux_pread,              //
    __NR_linux_preadv,             //
    __NR_linux_preadv2,            //
    __NR_linux_dup,                //
    __NR_linux_dup2,               //
    __NR_linux_dup3,               //
    __NR_linux_fchdir,             //
    __NR_linux_fcntl,              //
    __NR_linux_fstat,              //
    __NR_linux_fsync,              //
    __NR_linux_fdatasync,          //
    __NR_linux_ftruncate,          //
    __NR_linux_getdents,           //
    __NR_linux_getegid,            //
    __NR_linux_getrandom,          //
    __NR_linux_getgroups,          //
    __NR_linux_getpgid,            //
    __NR_linux_getpgrp,            //
    __NR_linux_getpid,             //
    __NR_linux_gettid,             //
    __NR_linux_getuid,             //
    __NR_linux_getgid,             //
    __NR_linux_getsid,             //
    __NR_linux_getppid,            //
    __NR_linux_geteuid,            //
    __NR_linux_getrlimit,          //
    __NR_linux_getresgid,          //
    __NR_linux_getresuid,          //
    __NR_linux_getitimer,          //
    __NR_linux_setitimer,          //
    __NR_linux_gettimeofday,       //
    __NR_linux_copy_file_range,    //
    __NR_linux_splice,             //
    __NR_linux_lseek,              //
    __NR_linux_tee,                //
    __NR_linux_brk,                //
    __NR_linux_mmap,               //
    __NR_linux_msync,              //
    __NR_linux_munmap,             //
    __NR_linux_madvise,            //
    __NR_linux_fadvise,            //
    __NR_linux_mprotect,           //
    __NR_linux_arch_prctl,         //
    __NR_linux_set_tid_address,    //
    __NR_linux_nanosleep,          //
    __NR_linux_pipe,               //
    __NR_linux_pipe2,              //
    __NR_linux_poll,               //
    __NR_linux_select,             //
    __NR_linux_recvmsg,            //
    __NR_linux_recvfrom,           //
    __NR_linux_sendto | ADDRLESS,  //
    __NR_linux_ioctl,              //
    __NR_linux_shutdown,           //
    __NR_linux_sigaction,          //
    __NR_linux_sigaltstack,        //
    __NR_linux_sigprocmask,        //
    __NR_linux_sigsuspend,         //
    __NR_linux_sigreturn,          //
    __NR_linux_socketpair,         //
    __NR_linux_umask,              //
    __NR_linux_wait4,              //
    __NR_linux_uname,              //
};

static const uint16_t kPledgeLinuxFlock[] = {
    __NR_linux_flock,         //
    __NR_linux_fcntl | LOCK,  //
};

static const uint16_t kPledgeLinuxRpath[] = {
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
    __NR_linux_readlink,           //
    __NR_linux_readlinkat,         //
    __NR_linux_chmod,              //
    __NR_linux_fchmod,             //
    __NR_linux_fchmodat,           //
};

static const uint16_t kPledgeLinuxWpath[] = {
    __NR_linux_getcwd,      //
    __NR_linux_open,        //
    __NR_linux_openat,      //
    __NR_linux_stat,        //
    __NR_linux_fstat,       //
    __NR_linux_lstat,       //
    __NR_linux_fstatat,     //
    __NR_linux_access,      //
    __NR_linux_faccessat,   //
    __NR_linux_readlinkat,  //
    __NR_linux_fchmod,      //
    __NR_linux_fchmodat,    //
};

static const uint16_t kPledgeLinuxCpath[] = {
    __NR_linux_rename,     //
    __NR_linux_renameat,   //
    __NR_linux_renameat2,  //
    __NR_linux_link,       //
    __NR_linux_linkat,     //
    __NR_linux_symlink,    //
    __NR_linux_symlinkat,  //
    __NR_linux_rmdir,      //
    __NR_linux_unlink,     //
    __NR_linux_unlinkat,   //
    __NR_linux_mkdir,      //
    __NR_linux_mkdirat,    //
};

static const uint16_t kPledgeLinuxFattr[] = {
    __NR_linux_chmod,      //
    __NR_linux_fchmod,     //
    __NR_linux_fchmodat,   //
    __NR_linux_utime,      //
    __NR_linux_utimes,     //
    __NR_linux_futimesat,  //
    __NR_linux_utimensat,  //
};

static const uint16_t kPledgeLinuxInet[] = {
    __NR_linux_socket | INET,  //
    __NR_linux_listen,         //
    __NR_linux_bind,           //
    __NR_linux_sendto,         //
    __NR_linux_connect,        //
    __NR_linux_accept,         //
    __NR_linux_accept4,        //
    __NR_linux_getsockopt,     //
    __NR_linux_setsockopt,     //
    __NR_linux_getpeername,    //
    __NR_linux_getsockname,    //
};

static const uint16_t kPledgeLinuxUnix[] = {
    __NR_linux_socket | UNIX,  //
    __NR_linux_listen,         //
    __NR_linux_bind,           //
    __NR_linux_connect,        //
    __NR_linux_sendto,         //
    __NR_linux_accept,         //
    __NR_linux_accept4,        //
    __NR_linux_getsockopt,     //
    __NR_linux_setsockopt,     //
    __NR_linux_getpeername,    //
    __NR_linux_getsockname,    //
};

static const uint16_t kPledgeLinuxDns[] = {
    __NR_linux_socket | INET,  //
    __NR_linux_sendto,         //
    __NR_linux_connect,        //
    __NR_linux_recvfrom,       //
};

static const uint16_t kPledgeLinuxTty[] = {
    __NR_linux_ioctl | TTY,  //
};

static const uint16_t kPledgeLinuxProc[] = {
    __NR_linux_fork,         //
    __NR_linux_vfork,        //
    __NR_linux_clone,        //
    __NR_linux_kill,         //
    __NR_linux_setsid,       //
    __NR_linux_setpgid,      //
    __NR_linux_prlimit,      //
    __NR_linux_setrlimit,    //
    __NR_linux_getpriority,  //
    __NR_linux_setpriority,  //
};

static const uint16_t kPledgeLinuxThread[] = {
    __NR_linux_clone,  //
    __NR_linux_futex,  //
};

static const uint16_t kPledgeLinuxId[] = {
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
};

static const uint16_t kPledgeLinuxExec[] = {
    __NR_linux_execve,    //
    __NR_linux_execveat,  //
};

static const struct Pledges {
  const char *name;
  const uint16_t *syscalls;
  const size_t len;
} kPledgeLinux[] = {
    {"default", PLEDGE(kPledgeLinuxDefault)},  //
    {"stdio", PLEDGE(kPledgeLinuxStdio)},      //
    {"rpath", PLEDGE(kPledgeLinuxRpath)},      //
    {"wpath", PLEDGE(kPledgeLinuxWpath)},      //
    {"cpath", PLEDGE(kPledgeLinuxCpath)},      //
    {"flock", PLEDGE(kPledgeLinuxFlock)},      //
    {"fattr", PLEDGE(kPledgeLinuxFattr)},      //
    {"inet", PLEDGE(kPledgeLinuxInet)},        //
    {"unix", PLEDGE(kPledgeLinuxUnix)},        //
    {"dns", PLEDGE(kPledgeLinuxDns)},          //
    {"tty", PLEDGE(kPledgeLinuxTty)},          //
    {"proc", PLEDGE(kPledgeLinuxProc)},        //
    {"thread", PLEDGE(kPledgeLinuxThread)},    //
    {"exec", PLEDGE(kPledgeLinuxExec)},        //
    {"id", PLEDGE(kPledgeLinuxId)},            //
    {0},                                       //
};

static const struct sock_filter kFilterStart[] = {
    // make sure this isn't an i386 binary or something
    BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(arch)),
    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, AUDIT_ARCH_X86_64, 1, 0),
    BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_KILL_PROCESS),
    // each filter assumes ordinal is already loaded into accumulator
    BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
};

static const struct sock_filter kFilterEnd[] = {
    // if syscall isn't whitelisted then have it return -EPERM (-1)
    BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ERRNO | (1 & SECCOMP_RET_DATA)),
};

static bool AppendFilter(struct Filter *f, struct sock_filter *p, size_t n) {
  size_t m;
  struct sock_filter *q;
  m = f->n + n;
  if (!(q = realloc(f->p, m * sizeof(*f->p)))) return false;
  memcpy(q + f->n, p, n * sizeof(*q));
  f->p = q;
  f->n = m;
  return true;
}

// SYSCALL is only allowed in the .privileged section
static bool AppendOriginVerification(struct Filter *f) {
  intptr_t x = (intptr_t)__privileged_start;
  intptr_t y = (intptr_t)__privileged_end;
  struct sock_filter fragment[] = {
      /*L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(instruction_pointer)),
      /*L2*/ BPF_JUMP(BPF_JMP | BPF_JGE | BPF_K, x, 0, 4 - 3),
      /*L3*/ BPF_JUMP(BPF_JMP | BPF_JGE | BPF_K, y, 0, 5 - 4),
      /*L4*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_KILL),
      /*L5*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L6*/ /* next filter */
  };
  return AppendFilter(f, PLEDGE(fragment));
}

// Authorize specific system call w/o considering its arguments.
static bool AllowSyscall(struct Filter *f, uint16_t w) {
  struct sock_filter fragment[] = {
      /*L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, w, 0, 2 - 1),
      /*L1*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L2*/ /* next filter */
  };
  return AppendFilter(f, PLEDGE(fragment));
}

// The second argument of ioctl() must be one of:
//
//   - FIONREAD (0x541b)
//   - FIONBIO  (0x5421)
//   - FIOCLEX  (0x5451)
//   - FIONCLEX (0x5450)
//
static bool AllowIoctl(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /*L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_ioctl, 0, 8 - 1),
      /*L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[1])),
      /*L2*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x541b, 6 - 3, 0),
      /*L3*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x5421, 6 - 4, 0),
      /*L4*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x5451, 6 - 5, 0),
      /*L5*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x5450, 0, 7 - 6),
      /*L6*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L7*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L8*/ /* next filter */
  };
  return AppendFilter(f, PLEDGE(fragment));
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
//   - TIOCSBRK   (0x5427)
//
static bool AllowIoctlTty(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /* L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_ioctl, 0, 13 - 1),
      /* L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[1])),
      /* L2*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x5401, 11 - 3, 0),
      /* L3*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x5402, 11 - 4, 0),
      /* L4*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x5403, 11 - 5, 0),
      /* L5*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x5404, 11 - 6, 0),
      /* L6*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x5413, 11 - 7, 0),
      /* L7*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x5410, 11 - 8, 0),
      /* L8*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x540f, 11 - 9, 0),
      /* L9*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x5414, 11 - 10, 0),
      /*L10*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x5427, 0, 12 - 11),
      /*L11*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L12*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L13*/ /* next filter */
  };
  return AppendFilter(f, PLEDGE(fragment));
}

// The level argument of setsockopt() must be one of:
//
//   - SOL_SOCKET (1)
//   - SOL_TCP    (6)
//
// The optname argument of setsockopt() must be one of:
//
//   - SO_TYPE      ( 3)
//   - SO_REUSEPORT (15)
//   - SO_REUSEADDR ( 2)
//   - SO_KEEPALIVE ( 9)
//   - SO_RCVTIMEO  (20)
//   - SO_SNDTIMEO  (21)
//
static bool AllowSetsockopt(struct Filter *f) {
  static const int nr = __NR_linux_setsockopt;
  static const struct sock_filter fragment[] = {
      /* L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, nr, 0, 13 - 1),
      /* L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[1])),
      /* L2*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 1, 11 - 3, 0),
      /* L3*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 6, 11 - 4, 0),
      /* L4*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[2])),
      /* L5*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 3, 11 - 6, 0),
      /* L6*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 15, 11 - 7, 0),
      /* L7*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 2, 11 - 8, 0),
      /* L8*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 9, 11 - 9, 0),
      /* L9*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 20, 11 - 10, 0),
      /*L10*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 21, 0, 12 - 11),
      /*L11*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L12*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L13*/ /* next filter */
  };
  return AppendFilter(f, PLEDGE(fragment));
}

// The level argument of getsockopt() must be one of:
//
//   - SOL_SOCKET (1)
//   - SOL_TCP    (6)
//
// The optname argument of getsockopt() must be one of:
//
//   - SO_TYPE      ( 3)
//   - SO_REUSEPORT (15)
//   - SO_REUSEADDR ( 2)
//   - SO_KEEPALIVE ( 9)
//   - SO_RCVTIMEO  (20)
//   - SO_SNDTIMEO  (21)
//
static bool AllowGetsockopt(struct Filter *f) {
  static const int nr = __NR_linux_getsockopt;
  static const struct sock_filter fragment[] = {
      /* L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, nr, 0, 13 - 1),
      /* L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[1])),
      /* L2*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 1, 11 - 3, 0),
      /* L3*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 6, 11 - 4, 0),
      /* L4*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[2])),
      /* L5*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 3, 11 - 6, 0),
      /* L6*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 15, 11 - 7, 0),
      /* L7*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 2, 11 - 8, 0),
      /* L8*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 9, 11 - 9, 0),
      /* L9*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 20, 11 - 10, 0),
      /*L10*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 21, 0, 12 - 11),
      /*L11*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L12*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L13*/ /* next filter */
  };
  return AppendFilter(f, PLEDGE(fragment));
}

// The flags parameter must not have:
//
//   - MAP_LOCKED   (0x02000)
//   - MAP_POPULATE (0x08000)
//   - MAP_NONBLOCK (0x10000)
//   - MAP_HUGETLB  (0x40000)
//
static bool AllowMmap(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /*L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_mmap, 0, 9 - 1),
      /*L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[2])),  // prot
      /*L2*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, 0x80),         // lazy
      /*L3*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 0, 8 - 4),
      /*L4*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[3])),  // flags
      /*L5*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, 0x5a000),
      /*L6*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 0, 8 - 7),
      /*L7*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L8*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L9*/ /* next filter */
  };
  return AppendFilter(f, PLEDGE(fragment));
}

// The prot parameter of mmap() must not have:
//
//   - PROT_EXEC (4)
//
// The flags parameter must not have:
//
//   - MAP_LOCKED   (0x02000)
//   - MAP_POPULATE (0x08000)
//   - MAP_NONBLOCK (0x10000)
//   - MAP_HUGETLB  (0x40000)
//
static bool AllowMmapNoexec(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /*L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_mmap, 0, 9 - 1),
      /*L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[2])),  // prot
      /*L2*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, PROT_EXEC),
      /*L3*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 0, 8 - 4),
      /*L4*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[3])),  // flags
      /*L5*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, 0x5a000),
      /*L6*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 0, 8 - 7),
      /*L7*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L8*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L9*/ /* next filter */
  };
  return AppendFilter(f, PLEDGE(fragment));
}

// The prot parameter of mprotect() must not have:
//
//   - PROT_EXEC (4)
//
static bool AllowMprotectNoexec(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /*L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_mprotect, 0, 6 - 1),
      /*L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[2])),  // prot
      /*L2*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, PROT_EXEC),
      /*L3*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 0, 5 - 4),
      /*L4*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L5*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L6*/ /* next filter */
  };
  return AppendFilter(f, PLEDGE(fragment));
}

// The flags parameter of open() must not have:
//
//   - O_WRONLY (1)
//   - O_RDWR   (2)
//
static bool AllowOpenReadonly(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /*L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_open, 0, 6 - 1),
      /*L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[1])),
      /*L2*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, O_ACCMODE),
      /*L3*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, O_RDONLY, 0, 5 - 4),
      /*L4*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L5*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L6*/ /* next filter */
  };
  return AppendFilter(f, PLEDGE(fragment));
}

// The flags parameter of open() must not have:
//
//   - O_WRONLY (1)
//   - O_RDWR   (2)
//
static bool AllowOpenatReadonly(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /*L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_openat, 0, 6 - 1),
      /*L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[2])),
      /*L2*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, O_ACCMODE),
      /*L3*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, O_RDONLY, 0, 5 - 4),
      /*L4*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L5*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L6*/ /* next filter */
  };
  return AppendFilter(f, PLEDGE(fragment));
}

// If the flags parameter of open() has one of:
//
//   - O_CREAT   (000000100)
//   - O_TMPFILE (020200000)
//
// Then the mode parameter must not have:
//
//   - S_ISVTX (01000 sticky)
//   - S_ISGID (02000 setgid)
//   - S_ISUID (04000 setuid)
//
static bool AllowOpen(struct Filter *f) {
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
      /* L9*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 0, 11 - 10),
      /*L10*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L11*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L12*/ /* next filter */
  };
  return AppendFilter(f, PLEDGE(fragment));
}

// If the flags parameter of openat() has one of:
//
//   - O_CREAT   (000000100)
//   - O_TMPFILE (020200000)
//
// Then the mode parameter must not have:
//
//   - S_ISVTX (01000 sticky)
//   - S_ISGID (02000 setgid)
//   - S_ISUID (04000 setuid)
//
static bool AllowOpenat(struct Filter *f) {
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
      /* L9*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 0, 11 - 10),
      /*L10*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L11*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L12*/ /* next filter */
  };
  return AppendFilter(f, PLEDGE(fragment));
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
static bool AllowFcntl(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /*L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_fcntl, 0, 6 - 1),
      /*L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[1])),
      /*L2*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 1030, 4 - 3, 0),
      /*L3*/ BPF_JUMP(BPF_JMP | BPF_JGE | BPF_K, 5, 5 - 4, 0),
      /*L4*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L5*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L6*/ /* next filter */
  };
  return AppendFilter(f, PLEDGE(fragment));
}

// The second argument of fcntl() must be one of:
//
//   - F_GETLK (5)
//   - F_SETLK (6)
//   - F_SETLKW (7)
//
static bool AllowFcntlLock(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /*L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_fcntl, 0, 6 - 1),
      /*L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[1])),
      /*L2*/ BPF_JUMP(BPF_JMP | BPF_JGE | BPF_K, 5, 0, 5 - 3),
      /*L3*/ BPF_JUMP(BPF_JMP | BPF_JGE | BPF_K, 8, 5 - 4, 0),
      /*L4*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L5*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L6*/ /* next filter */
  };
  return AppendFilter(f, PLEDGE(fragment));
}

// The addr parameter of sendto() must be
//
//   - NULL
//
static bool AllowSendtoRestricted(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /*L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_sendto, 0, 5 - 1),
      /*L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[4])),
      /*L2*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 0, 4 - 3),
      /*L3*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L4*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L5*/ /* next filter */
  };
  return AppendFilter(f, PLEDGE(fragment));
}

// The family parameter of socket() must be one of:
//
//   - AF_INET (2)
//   - AF_INET6 (10)
//
static bool AllowSocketInet(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /*L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_socket, 0, 6 - 1),
      /*L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[0])),
      /*L2*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 2, 4 - 3, 0),
      /*L3*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 10, 0, 5 - 4),
      /*L4*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L5*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L6*/ /* next filter */
  };
  return AppendFilter(f, PLEDGE(fragment));
}

// The family parameter of socket() must be one of:
//
//   - AF_UNIX (1)
//   - AF_LOCAL (1)
//
static bool AllowSocketUnix(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /*L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_socket, 0, 5 - 1),
      /*L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[0])),
      /*L2*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 1, 0, 4 - 3),
      /*L3*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L4*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L5*/ /* next filter */
  };
  return AppendFilter(f, PLEDGE(fragment));
}

// The mode parameter of chmod() can't have the following:
//
//   - S_ISVTX (01000 sticky)
//   - S_ISGID (02000 setgid)
//   - S_ISUID (04000 setuid)
//
static bool AllowChmod(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /*L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_chmod, 0, 6 - 1),
      /*L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[1])),
      /*L2*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, 07000),
      /*L3*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 0, 5 - 4),
      /*L4*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L5*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L6*/ /* next filter */
  };
  return AppendFilter(f, PLEDGE(fragment));
}

// The mode parameter of fchmod() can't have the following:
//
//   - S_ISVTX (01000 sticky)
//   - S_ISGID (02000 setgid)
//   - S_ISUID (04000 setuid)
//
static bool AllowFchmod(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /*L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_fchmod, 0, 6 - 1),
      /*L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[1])),
      /*L2*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, 07000),
      /*L3*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 0, 5 - 4),
      /*L4*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L5*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L6*/ /* next filter */
  };
  return AppendFilter(f, PLEDGE(fragment));
}

// The mode parameter of fchmodat() can't have the following:
//
//   - S_ISVTX (01000 sticky)
//   - S_ISGID (02000 setgid)
//   - S_ISUID (04000 setuid)
//
static bool AllowFchmodat(struct Filter *f) {
  static const struct sock_filter fragment[] = {
      /*L0*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_fchmodat, 0, 6 - 1),
      /*L1*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[2])),
      /*L2*/ BPF_STMT(BPF_ALU | BPF_AND | BPF_K, 07000),
      /*L3*/ BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 0, 5 - 4),
      /*L4*/ BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      /*L5*/ BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
      /*L6*/ /* next filter */
  };
  return AppendFilter(f, PLEDGE(fragment));
}

static bool AppendPledge(struct Filter *f, const uint16_t *p, size_t len,
                         bool needmapexec, bool needmorphing) {
  int i;
  for (i = 0; i < len; ++i) {
    switch (p[i]) {
      case __NR_linux_mmap:
        if (needmapexec) {
          if (!AllowMmap(f)) return false;
        } else {
          if (!AllowMmapNoexec(f)) return false;
        }
        break;
      case __NR_linux_mprotect:
        if (needmorphing) {
          if (!AllowSyscall(f, __NR_linux_mprotect)) return false;
        } else {
          if (!AllowMprotectNoexec(f)) return false;
        }
        break;
      case __NR_linux_chmod:
        if (!AllowChmod(f)) return false;
        break;
      case __NR_linux_fchmod:
        if (!AllowFchmod(f)) return false;
        break;
      case __NR_linux_fchmodat:
        if (!AllowFchmodat(f)) return false;
        break;
      case __NR_linux_open:
        if (!AllowOpen(f)) return false;
        break;
      case __NR_linux_openat:
        if (!AllowOpenat(f)) return false;
        break;
      case __NR_linux_open | READONLY:
        if (!AllowOpenReadonly(f)) return false;
        break;
      case __NR_linux_openat | READONLY:
        if (!AllowOpenatReadonly(f)) return false;
        break;
      case __NR_linux_setsockopt:
        if (!AllowSetsockopt(f)) return false;
        break;
      case __NR_linux_getsockopt:
        if (!AllowGetsockopt(f)) return false;
        break;
      case __NR_linux_fcntl:
        if (!AllowFcntl(f)) return false;
        break;
      case __NR_linux_fcntl | LOCK:
        if (!AllowFcntlLock(f)) return false;
        break;
      case __NR_linux_ioctl:
        if (!AllowIoctl(f)) return false;
        break;
      case __NR_linux_ioctl | TTY:
        if (!AllowIoctlTty(f)) return false;
        break;
      case __NR_linux_socket | INET:
        if (!AllowSocketInet(f)) return false;
        break;
      case __NR_linux_socket | UNIX:
        if (!AllowSocketUnix(f)) return false;
        break;
      case __NR_linux_sendto | ADDRLESS:
        if (!AllowSendtoRestricted(f)) return false;
        break;
      default:
        if (!AllowSyscall(f, p[i])) return false;
        break;
    }
  }
  return true;
}

static const uint16_t *FindPledge(const struct Pledges *p, const char *name,
                                  size_t *len) {
  int i;
  for (i = 0; p[i].name; ++i) {
    if (!strcasecmp(name, p[i].name)) {
      *len = p[i].len;
      return p[i].syscalls;
    }
  }
  return 0;
}

static int sys_pledge_linux(const char *promises, const char *execpromises) {
  bool ok;
  int rc = -1;
  size_t plen;
  bool needmapexec;
  bool needmorphing;
  struct Filter f = {0};
  const uint16_t *pledge;
  char *s, *tok, *state, *start;
  if (execpromises) return einval();
  needmapexec = strstr(promises, "exec");
  needmorphing = strstr(promises, "thread");
  if ((start = s = strdup(promises)) &&
      AppendFilter(&f, kFilterStart, ARRAYLEN(kFilterStart)) &&
      (needmapexec || AppendOriginVerification(&f)) &&
      AppendPledge(&f, kPledgeLinuxDefault, ARRAYLEN(kPledgeLinuxDefault),
                   needmapexec, needmorphing)) {
    for (ok = true; (tok = strtok_r(start, " \t\r\n", &state)); start = 0) {
      if (!(pledge = FindPledge(kPledgeLinux, tok, &plen))) {
        ok = false;
        rc = einval();
        break;
      }
      if (!AppendPledge(&f, pledge, plen, needmapexec, needmorphing)) {
        ok = false;
        break;
      }
    }
    if (ok && AppendFilter(&f, kFilterEnd, ARRAYLEN(kFilterEnd)) &&
        (rc = prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0)) != -1) {
      struct sock_fprog sandbox = {.len = f.n, .filter = f.p};
      rc = prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &sandbox);
    }
  }
  free(f.p);
  free(s);
  return rc;
}

/**
 * Restricts system operations, e.g.
 *
 *     pledge("stdio tty", 0);
 *
 * Pledging causes most system calls to become unavailable. On Linux the
 * disabled calls will return EPERM whereas OpenBSD kills the process.
 *
 * Using pledge is irreversible. On Linux it causes PR_SET_NO_NEW_PRIVS
 * to be set on your process.
 *
 * By default exit and exit_group are always allowed. This is useful
 * for processes that perform pure computation and interface with the
 * parent via shared memory.
 *
 * Once pledge is in effect, the chmod functions (if allowed) will not
 * permit the sticky/setuid/setgid bits to change. Linux will EPERM here
 * and OpenBSD should ignore those three bits rather than crashing.
 *
 * User and group IDs also can't be changed once pledge is in effect.
 * OpenBSD should ignore the chown functions without crashing. Linux
 * will just EPERM.
 *
 * Memory functions won't permit creating executable code after pledge.
 * Restrictions on origin of SYSCALL instructions will become enforced
 * on Linux (cf. msyscall) after pledge too, which means the process
 * gets killed if SYSCALL is used outside the .privileged section. One
 * exception is if the "exec" group is specified, in which case these
 * restrictions need to be loosened.
 *
 * `promises` is a string that may include any of the following groups
 * delimited by spaces.
 *
 * - "stdio" allows clock_getres, clock_gettime, close, dup, dup2, dup3,
 *   fchdir, fstat, fsync, fdatasync, ftruncate, getdents, getegid,
 *   getrandom, geteuid, getgid, getgroups, getitimer, getpgid, getpgrp,
 *   getpid, getppid, getresgid, getresuid, getrlimit, getsid,
 *   gettimeofday, getuid, lseek, madvise, brk, arch_prctl, uname,
 *   set_tid_address, mmap (PROT_EXEC and weird flags aren't allowed),
 *   mprotect (PROT_EXEC isn't allowed), msync, munmap, nanosleep, pipe,
 *   pipe2, read, readv, pread, recv, recvmsg, poll, recvfrom, preadv,
 *   write, writev, pwrite, pwritev, select, send, sendto (only if addr
 *   is null), setitimer, shutdown, sigaction, sigaltstack, sigprocmask,
 *   sigreturn, sigsuspend, umask socketpair, wait4, ioctl(FIONREAD),
 *   ioctl(FIONBIO), ioctl(FIOCLEX), ioctl(FIONCLEX), fcntl(F_GETFD),
 *   fcntl(F_SETFD), fcntl(F_GETFL), fcntl(F_SETFL).
 *
 * - "rpath" (read-only path ops) allows chdir, getcwd, open(O_RDONLY),
 *   openat(O_RDONLY), stat, fstat, lstat, fstatat, access, faccessat,
 *   readlink, readlinkat, chmod, fchmod, fchmodat.
 *
 * - "wpath" (write path ops) allows getcwd, open, openat, stat, fstat,
 *   lstat, fstatat, access, faccessat, readlink, readlinkat, chmod,
 *   fchmod, fchmodat.
 *
 * - "cpath" (create path ops) allows rename, renameat, renameat2, link,
 *   linkat, symlink, symlinkat, unlink, rmdir, unlinkat, mkdir,
 *   mkdirat.
 *
 * - "flock" allows flock, fcntl(F_GETLK), fcntl(F_SETLK),
 *   fcntl(F_SETLKW).
 *
 * - "tty" allows ioctl(TIOCGWINSZ), ioctl(TCGETS), ioctl(TCSETS),
 *   ioctl(TCSETSW), ioctl(TCSETSF).
 *
 * - "fattr" allows chmod, fchmod, fchmodat, utime, utimes, futimens,
 *   utimensat.
 *
 * - "inet" allows socket(AF_INET), listen, bind, connect, accept,
 *   accept4, getpeername, getsockname, setsockopt, getsockopt, sendto.
 *
 * - "unix" allows socket(AF_UNIX), listen, bind, connect, accept,
 *   accept4, getpeername, getsockname, setsockopt, getsockopt.
 *
 * - "dns" allows socket(AF_INET), sendto, recvfrom, connect.
 *
 * - "proc" allows fork, vfork, kill, getpriority, setpriority, prlimit,
 *   setrlimit, setpgid, setsid.
 *
 * - "thread" allows clone, futex, and permits PROT_EXEC in mprotect.
 *
 * - "id" allows setuid, setreuid, setresuid, setgid, setregid,
 *   setresgid, setgroups, prlimit, setrlimit, getpriority, setpriority.
 *
 * - "exec" allows execve, execveat. If this is used then APE binaries
 *   should be assimilated in order to work on OpenBSD. On Linux, mmap()
 *   will be loosened up to allow creating PROT_EXEC memory (for APE
 *   loader) and system call origin verification won't be activated.
 *
 * @return 0 on success, or -1 w/ errno
 * @raise ENOSYS if host os isn't Linux or OpenBSD
 */
int pledge(const char *promises, const char *execpromises) {
  int rc;
  if (IsLinux()) {
    rc = sys_pledge_linux(promises, execpromises);
  } else {
    rc = sys_pledge(promises, execpromises);
  }
  STRACE("pledge(%#s, %#s) → %d% m", promises, execpromises, rc);
  return rc;
}
