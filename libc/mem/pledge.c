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
#include "libc/calls/struct/filter.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/kprintf.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/nrlinux.h"
#include "libc/sysv/consts/pr.h"
#include "libc/sysv/errfuns.h"
#include "tool/net/sandbox.h"

static const uint16_t kPledgeLinuxDefault[] = {
    __NR_linux_exit,        //
    __NR_linux_exit_group,  //
};

static const uint16_t kPledgeLinuxStdio[] = {
    __NR_linux_clock_getres,   //
    __NR_linux_clock_gettime,  //
    __NR_linux_close,          //
    __NR_linux_dup,            //
    __NR_linux_dup2,           //
    __NR_linux_dup3,           //
    __NR_linux_fchdir,         //
    /* __NR_linux_fcntl,          // */
    __NR_linux_fstat,         //
    __NR_linux_fsync,         //
    __NR_linux_ftruncate,     //
    __NR_linux_getdents,      //
    __NR_linux_getegid,       //
    __NR_linux_getrandom,     //
    __NR_linux_geteuid,       //
    __NR_linux_getgid,        //
    __NR_linux_getgroups,     //
    __NR_linux_getitimer,     //
    __NR_linux_getpgid,       //
    __NR_linux_getpgrp,       //
    __NR_linux_getpid,        //
    __NR_linux_getppid,       //
    __NR_linux_getresgid,     //
    __NR_linux_getresuid,     //
    __NR_linux_getrlimit,     //
    __NR_linux_getsid,        //
    __NR_linux_gettimeofday,  //
    __NR_linux_getuid,        //
    __NR_linux_lseek,         //
    __NR_linux_madvise,       //
    __NR_linux_brk,           //
    __NR_linux_mmap,          //
    __NR_linux_mprotect,      //
    __NR_linux_munmap,        //
    __NR_linux_nanosleep,     //
    __NR_linux_pipe,          //
    __NR_linux_pipe2,         //
    __NR_linux_poll,          //
    __NR_linux_pread,         //
    __NR_linux_preadv,        //
    __NR_linux_pwrite,        //
    __NR_linux_pwritev,       //
    __NR_linux_read,          //
    __NR_linux_readv,         //
    __NR_linux_recvfrom,      //
    __NR_linux_recvmsg,       //
    __NR_linux_select,        //
    __NR_linux_sendmsg,       //
    __NR_linux_sendto,        //
    __NR_linux_setitimer,     //
    __NR_linux_shutdown,      //
    __NR_linux_sigaction,     //
    __NR_linux_sigprocmask,   //
    __NR_linux_sigreturn,     //
    __NR_linux_socketpair,    //
    __NR_linux_umask,         //
    __NR_linux_wait4,         //
    __NR_linux_write,         //
    __NR_linux_writev,        //
};

static const uint16_t kPledgeLinuxRpath[] = {
    __NR_linux_chdir,       //
    __NR_linux_getcwd,      //
    __NR_linux_openat,      //
    __NR_linux_fstatat,     //
    __NR_linux_faccessat,   //
    __NR_linux_readlinkat,  //
    __NR_linux_lstat,       //
    __NR_linux_chmod,       //
    __NR_linux_fchmod,      //
    __NR_linux_fchmodat,    //
    __NR_linux_chown,       //
    __NR_linux_fchown,      //
    __NR_linux_fchownat,    //
    __NR_linux_fstat,       //
};

static const uint16_t kPledgeLinuxWpath[] = {
    __NR_linux_getcwd,      //
    __NR_linux_openat,      //
    __NR_linux_fstatat,     //
    __NR_linux_faccessat,   //
    __NR_linux_readlinkat,  //
    __NR_linux_lstat,       //
    __NR_linux_chmod,       //
    __NR_linux_fchmod,      //
    __NR_linux_fchmodat,    //
    __NR_linux_chown,       //
    __NR_linux_fchown,      //
    __NR_linux_fchownat,    //
    __NR_linux_fstat,       //
};

static const uint16_t kPledgeLinuxCpath[] = {
    __NR_linux_rename,     //
    __NR_linux_renameat,   //
    __NR_linux_link,       //
    __NR_linux_linkat,     //
    __NR_linux_symlink,    //
    __NR_linux_symlinkat,  //
    __NR_linux_unlink,     //
    __NR_linux_unlinkat,   //
    __NR_linux_mkdir,      //
    __NR_linux_mkdirat,    //
    __NR_linux_rmdir,      //
};

static const uint16_t kPledgeLinuxDpath[] = {
    __NR_linux_mknod,  //
};

static const uint16_t kPledgeLinuxTmppath[] = {
    __NR_linux_lstat,   //
    __NR_linux_chmod,   //
    __NR_linux_chown,   //
    __NR_linux_unlink,  //
    __NR_linux_fstat,   //
};

static const uint16_t kPledgeLinuxInet[] = {
    __NR_linux_socket,       //
    __NR_linux_listen,       //
    __NR_linux_bind,         //
    __NR_linux_connect,      //
    __NR_linux_accept4,      //
    __NR_linux_accept,       //
    __NR_linux_getpeername,  //
    __NR_linux_getsockname,  //
    __NR_linux_setsockopt,   //
    __NR_linux_getsockopt,   //
};

static const uint16_t kPledgeLinuxFattr[] = {
    __NR_linux_utimes,     //
    __NR_linux_utimensat,  //
    __NR_linux_chmod,      //
    __NR_linux_fchmod,     //
    __NR_linux_fchmodat,   //
    __NR_linux_chown,      //
    __NR_linux_fchownat,   //
    __NR_linux_lchown,     //
    __NR_linux_fchown,     //
    __NR_linux_utimes,     //
};

static const uint16_t kPledgeLinuxUnix[] = {
    __NR_linux_socket,       //
    __NR_linux_listen,       //
    __NR_linux_bind,         //
    __NR_linux_connect,      //
    __NR_linux_accept4,      //
    __NR_linux_accept,       //
    __NR_linux_getpeername,  //
    __NR_linux_getsockname,  //
    __NR_linux_setsockopt,   //
    __NR_linux_getsockopt,   //
};

static const uint16_t kPledgeLinuxDns[] = {
    __NR_linux_sendto,    //
    __NR_linux_recvfrom,  //
    __NR_linux_socket,    //
    __NR_linux_connect,   //
};

static const uint16_t kPledgeLinuxProc[] = {
    __NR_linux_fork,         //
    __NR_linux_vfork,        //
    __NR_linux_kill,         //
    __NR_linux_getpriority,  //
    __NR_linux_setpriority,  //
    __NR_linux_setrlimit,    //
    __NR_linux_setpgid,      //
    __NR_linux_setsid,       //
};

static const uint16_t kPledgeLinuxExec[] = {
    __NR_linux_execve,  //
};

static const uint16_t kPledgeLinuxId[] = {
    __NR_linux_setuid,       //
    __NR_linux_setreuid,     //
    __NR_linux_setresuid,    //
    __NR_linux_setgid,       //
    __NR_linux_setregid,     //
    __NR_linux_setresgid,    //
    __NR_linux_setgroups,    //
    __NR_linux_setrlimit,    //
    __NR_linux_getpriority,  //
    __NR_linux_setpriority,  //
};

#define PLEDGELEN(pledge) pledge, ARRAYLEN(pledge)

static const struct Pledges {
  const char *name;
  const uint16_t *syscalls;
  const size_t len;
} kPledgeLinux[] = {
    {"default", PLEDGELEN(kPledgeLinuxDefault)},  //
    {"stdio", PLEDGELEN(kPledgeLinuxStdio)},      //
    {"rpath", PLEDGELEN(kPledgeLinuxRpath)},      //
    {"wpath", PLEDGELEN(kPledgeLinuxWpath)},      //
    {"cpath", PLEDGELEN(kPledgeLinuxCpath)},      //
    {"dpath", PLEDGELEN(kPledgeLinuxDpath)},      //
    {"tmppath", PLEDGELEN(kPledgeLinuxTmppath)},  //
    {"inet", PLEDGELEN(kPledgeLinuxInet)},        //
    {"fattr", PLEDGELEN(kPledgeLinuxFattr)},      //
    {"unix", PLEDGELEN(kPledgeLinuxUnix)},        //
    {"dns", PLEDGELEN(kPledgeLinuxDns)},          //
    {"proc", PLEDGELEN(kPledgeLinuxProc)},        //
    {"exec", PLEDGELEN(kPledgeLinuxExec)},        //
    {"id", PLEDGELEN(kPledgeLinuxId)},            //
    {0},                                          //
};

static const struct sock_filter kFilterStart[] = {
    _SECCOMP_MACHINE(AUDIT_ARCH_X86_64),  //
    _SECCOMP_LOAD_SYSCALL_NR(),           //
};

static const struct sock_filter kFilterEnd[] = {
    _SECCOMP_LOG_AND_RETURN_ERRNO(1),  // EPERM
};

struct Filter {
  size_t n;
  struct sock_filter *p;
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

static bool AppendPledge(struct Filter *f, const uint16_t *p, size_t len) {
  int i;
  for (i = 0; i < len; ++i) {
    struct sock_filter fragment[] = {_SECCOMP_ALLOW_SYSCALL(p[i])};
    if (!AppendFilter(f, fragment, ARRAYLEN(fragment))) {
      return false;
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
  struct Filter f = {0};
  const uint16_t *pledge;
  char *s, *tok, *state, *start;
  if (execpromises) return einval();
  if ((start = s = strdup(promises)) &&
      AppendFilter(&f, kFilterStart, ARRAYLEN(kFilterStart)) &&
      AppendPledge(&f, kPledgeLinuxDefault, ARRAYLEN(kPledgeLinuxDefault))) {
    for (ok = true; (tok = strtok_r(start, " \t\r\n", &state)); start = 0) {
      if (!(pledge = FindPledge(kPledgeLinux, tok, &plen))) {
        ok = false;
        rc = einval();
        break;
      }
      if (!AppendPledge(&f, pledge, plen)) {
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
 * Restricts system operations.
 *
 * Only available on OpenBSD and Linux.
 *
 * By default exit and exit_group are always allowed. This is useful
 * for processes that perform pure computation and interface with the
 * parent via shared memory.
 *
 * `promises` is a string that may include any of the following groups
 * delimited by spaces.
 *
 * - "stdio" allows clock_getres, clock_gettime, close, dup, dup2, dup3,
 *   fchdir, fstat, fsync, ftruncate, getdents, getegid, getrandom,
 *   geteuid, getgid, getgroups, getitimer, getpgid, getpgrp, getpid,
 *   getppid, getresgid, getresuid, getrlimit, getsid, gettimeofday,
 *   getuid, lseek, madvise, brk, mmap, mprotect, munmap, nanosleep,
 *   pipe, pipe2, poll, pread, preadv, pwrite, pwritev, read, readv,
 *   recvfrom, recvmsg, select, sendmsg, sendto, setitimer, shutdown,
 *   sigaction, sigprocmask, sigreturn, socketpair, umask, wait4, write,
 *   writev.
 *
 * - "rpath" allows chdir, getcwd, openat, fstatat, faccessat,
 *   readlinkat, lstat, chmod, fchmod, fchmodat, chown, fchown,
 *   fchownat, fstat.
 *
 * - "wpath" allows getcwd, openat, fstatat, faccessat, readlinkat,
 *   lstat, chmod, fchmod, fchmodat, chown, fchown, fchownat, fstat.
 *
 * - "cpath" allows rename, renameat, link, linkat, symlink, symlinkat,
 *   unlink, unlinkat, mkdir, mkdirat, rmdir.
 *
 * - "dpath" allows mknod
 *
 * - "tmppath" allows lstat, chmod, chown, unlink, fstat.
 *
 * - "inet" allows socket, listen, bind, connect, accept4, accept,
 *   getpeername, getsockname, setsockopt, getsockopt.
 *
 * - "fattr" allows utimes, utimensat, chmod, fchmod, fchmodat, chown,
 *   fchownat, lchown, fchown, utimes.
 *
 * - "unix" allows socket, listen, bind, connect, accept4, accept,
 *   getpeername, getsockname, setsockopt, getsockopt.
 *
 * - "dns" allows sendto, recvfrom, socket, connect.
 *
 * - "proc" allows fork, vfork, kill, getpriority, setpriority,
 *   setrlimit, setpgid, setsid.
 *
 * - "exec" allows execve.
 *
 * - "id" allows setuid, setreuid, setresuid, setgid, setregid,
 *   setresgid, setgroups, setrlimit, getpriority, setpriority.
 *
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
