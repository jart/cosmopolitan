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
#include "libc/calls/sigbits.h"
#include "libc/calls/struct/filter.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/user_regs_struct.h"
#include "libc/calls/ucontext.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/pr.h"
#include "libc/sysv/consts/ptrace.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "tool/net/sandbox.h"

#define __WALL 0x40000000

static const struct sock_filter kSandboxFilter[] = {
    _SECCOMP_MACHINE(AUDIT_ARCH_X86_64),  //
    _SECCOMP_LOAD_SYSCALL_NR(),           //
    _SECCOMP_ALLOW_SYSCALL(0x000),        // read
    _SECCOMP_ALLOW_SYSCALL(0x001),        // write
    _SECCOMP_ALLOW_SYSCALL(0x013),        // readv
    _SECCOMP_ALLOW_SYSCALL(0x014),        // writev
    _SECCOMP_ALLOW_SYSCALL(0x005),        // fstat
    _SECCOMP_ALLOW_SYSCALL(0x007),        // poll
    _SECCOMP_ALLOW_SYSCALL(0x008),        // lseek
    _SECCOMP_ALLOW_SYSCALL(0x009),        // mmap
    _SECCOMP_ALLOW_SYSCALL(0x00b),        // munmap
    _SECCOMP_ALLOW_SYSCALL(0x04f),        // getcwd
    _SECCOMP_ALLOW_SYSCALL(0x003),        // close
    _SECCOMP_ALLOW_SYSCALL(0x010),        // ioctl todo
    _SECCOMP_ALLOW_SYSCALL(0x016),        // pipe
    _SECCOMP_ALLOW_SYSCALL(0x125),        // pipe2
    _SECCOMP_ALLOW_SYSCALL(0x035),        // socketpair
    _SECCOMP_ALLOW_SYSCALL(0x020),        // dup
    _SECCOMP_ALLOW_SYSCALL(0x021),        // dup2
    _SECCOMP_ALLOW_SYSCALL(0x124),        // dup3
    _SECCOMP_ALLOW_SYSCALL(0x039),        // fork
    _SECCOMP_ALLOW_SYSCALL(0x03a),        // vfork
    _SECCOMP_ALLOW_SYSCALL(0x011),        // pread
    _SECCOMP_ALLOW_SYSCALL(0x012),        // pwrite
    _SECCOMP_ALLOW_SYSCALL(0x127),        // preadv
    _SECCOMP_ALLOW_SYSCALL(0x128),        // pwritev
    _SECCOMP_ALLOW_SYSCALL(0x0d9),        // getdents
    _SECCOMP_ALLOW_SYSCALL(0x027),        // getpid
    _SECCOMP_ALLOW_SYSCALL(0x066),        // getuid
    _SECCOMP_ALLOW_SYSCALL(0x068),        // getgid
    _SECCOMP_ALLOW_SYSCALL(0x06e),        // getppid
    _SECCOMP_ALLOW_SYSCALL(0x06f),        // getpgrp
    _SECCOMP_ALLOW_SYSCALL(0x07c),        // getsid
    _SECCOMP_ALLOW_SYSCALL(0x06b),        // geteuid
    _SECCOMP_ALLOW_SYSCALL(0x06c),        // getegid
    _SECCOMP_ALLOW_SYSCALL(0x061),        // getrlimit
    _SECCOMP_ALLOW_SYSCALL(0x028),        // sendfile
    _SECCOMP_ALLOW_SYSCALL(0x02d),        // recvfrom
    _SECCOMP_ALLOW_SYSCALL(0x033),        // getsockname
    _SECCOMP_ALLOW_SYSCALL(0x034),        // getpeername
    _SECCOMP_ALLOW_SYSCALL(0x00f),        // rt_sigreturn
    _SECCOMP_ALLOW_SYSCALL(0x082),        // rt_sigsuspend
    _SECCOMP_ALLOW_SYSCALL(0x0e4),        // clock_gettime
    _SECCOMP_ALLOW_SYSCALL(0x060),        // gettimeofday
    _SECCOMP_ALLOW_SYSCALL(0x03f),        // uname
    _SECCOMP_ALLOW_SYSCALL(0x03c),        // exit
    _SECCOMP_ALLOW_SYSCALL(0x0e7),        // exit_group
    _SECCOMP_TRACE_SYSCALL(0x03e, 0),     // kill
    _SECCOMP_TRACE_SYSCALL(0x101, 0),     // openat
    _SECCOMP_TRACE_SYSCALL(0x106, 0),     // newfstatat
    _SECCOMP_TRACE_SYSCALL(0x029, 0),     // socket
    _SECCOMP_TRACE_SYSCALL(0x031, 0),     // bind
    _SECCOMP_TRACE_SYSCALL(0x02a, 0),     // connect
    _SECCOMP_TRACE_SYSCALL(0x02c, 0),     // sendto
    _SECCOMP_TRACE_SYSCALL(0x036, 0),     // setsockopt
    _SECCOMP_TRACE_SYSCALL(0x048, 0),     // fcntl
    _SECCOMP_TRACE_SYSCALL(0x03b, 0),     // execve
    _SECCOMP_TRACE_SYSCALL(0x102, 0),     // mkdirat
    _SECCOMP_TRACE_SYSCALL(0x104, 0),     // chownat
    _SECCOMP_TRACE_SYSCALL(0x107, 0),     // unlinkat
    _SECCOMP_TRACE_SYSCALL(0x108, 0),     // renameat
    _SECCOMP_TRACE_SYSCALL(0x109, 0),     // linkat
    _SECCOMP_TRACE_SYSCALL(0x10a, 0),     // symlinkat
    _SECCOMP_TRACE_SYSCALL(0x10b, 0),     // readlinkat
    _SECCOMP_TRACE_SYSCALL(0x10c, 0),     // fchmodat
    _SECCOMP_TRACE_SYSCALL(0x10d, 0),     // faccessat
    _SECCOMP_TRACE_SYSCALL(0x0eb, 0),     // utimes
    _SECCOMP_TRACE_SYSCALL(0x105, 0),     // futimesat
    _SECCOMP_TRACE_SYSCALL(0x118, 0),     // utimensat
    _SECCOMP_LOG_AND_RETURN_ERRNO(1),     // EPERM
};

static const struct sock_fprog kSandbox = {
    .len = ARRAYLEN(kSandboxFilter),
    .filter = kSandboxFilter,
};

void OnSys(int sig, siginfo_t *si, ucontext_t *ctx) {
  kprintf("Got SIGSYS%n");
}

int main(int argc, char *argv[]) {
  sigset_t mask, origmask;
  struct user_regs_struct regs;
  int child, evpid, signal, wstatus;

  if (!IsLinux()) {
    kprintf("error: %s is only supported on linux right now%n", argv[0]);
    return 1;
  }
  if (argc < 2) {
    kprintf("Usage: %s PROGRAM [ARGS...]%n", argv[0]);
    return 1;
  }
  /* ShowCrashReports(); */

  sigaction(SIGINT, &(struct sigaction){.sa_handler = SIG_IGN}, 0);
  sigaction(SIGQUIT, &(struct sigaction){.sa_handler = SIG_IGN}, 0);
  sigaction(SIGSYS,
            &((struct sigaction){
                .sa_sigaction = OnSys,
                .sa_flags = SA_SIGINFO,
            }),
            0);

  sigemptyset(&mask);
  sigaddset(&mask, SIGCHLD);
  sigprocmask(SIG_BLOCK, &mask, &origmask);

  CHECK_NE(-1, (child = fork()));
  if (!child) {
    sigaction(SIGINT, &(struct sigaction){.sa_handler = SIG_DFL}, 0);
    sigaction(SIGQUIT, &(struct sigaction){.sa_handler = SIG_DFL}, 0);
    kprintf("CHILD ptrace(PTRACE_TRACEME)%n");
    if (ptrace(PTRACE_TRACEME) == -1) {
      kprintf("CHILD ptrace(PTRACE_TRACEME) failed %m%n");
      _Exit(124);
    }
    kprintf("CHILD prctl(PR_SET_NO_NEW_PRIVS)%n");
    if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0) == -1) {
      kprintf("CHILD prctl(PR_SET_NO_NEW_PRIVS) failed %m%n");
      _Exit(125);
    }
    kprintf("CHILD prctl(PR_SET_SECCOMP)%n");
    if (prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &kSandbox) == -1) {
      kprintf("CHILD prctl(PR_SET_SECCOMP) failed %m%n");
      _Exit(126);
    }
    kprintf("CHILD sigsuspend()%n");
    if (sigsuspend(0) == -1) {
      kprintf("CHILD sigsuspend() failed %m%n");
    }
    sigaction(SIGSYS, &(struct sigaction){.sa_handler = SIG_DFL}, 0);
    sigprocmask(SIG_SETMASK, &origmask, 0);
    execv(argv[1], argv + 1);
    kprintf("CHILD execve(%#s) failed %m%n", argv[1]);
    _Exit(127);
  }

  // wait for ptrace(PTRACE_TRACEME) to be called
  kprintf("PARENT waitpid(child, &wstatus)%n");
  CHECK_EQ(child, waitpid(child, &wstatus, 0));

  // configure linux process tracing
  kprintf("PARENT ptrace(PTRACE_SETOPTIONS)%n");
  CHECK_NE(-1, ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_TRACESECCOMP));

  // continue child process
  kprintf("PARENT ptrace(PTRACE_CONT)%n");
  CHECK_NE(-1, ptrace(PTRACE_CONT, child, 0, 0));

  kprintf("PARENT kill(child, SIGSYS)%n");
  kill(child, SIGSYS);

  for (;;) {
    kprintf("PARENT waitpid()%n");
    CHECK_NE(-1, (evpid = waitpid(-1, &wstatus, __WALL)));
    if (WIFSTOPPED(wstatus)) {
      signal = (wstatus >> 8) & 0xffff;
      if (signal == SIGTRAP | PTRACE_EVENT_SECCOMP) {
        // CHECK_NE(-1, ptrace(PTRACE_GETEVENTMSG, evpid, 0, &msg));
        CHECK_NE(-1, ptrace(PTRACE_GETREGS, evpid, 0, regs));
        regs.rax = -EPERM;
        CHECK_NE(-1, ptrace(PTRACE_GETREGS, evpid, 0, regs));
        ptrace(PTRACE_CONT, evpid, 0, 0);
      } else {
        ptrace(PTRACE_CONT, evpid, 0, signal & 127);
      }
    } else if (WIFEXITED(wstatus)) {
      exit(WEXITSTATUS(wstatus));
    } else {
      exit(128 + WTERMSIG(wstatus));
    }
  }

  return 0;
}
