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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/bpf.h"
#include "libc/calls/struct/filter.h"
#include "libc/calls/struct/seccomp.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/likely.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/msghdr.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/audit.h"
#include "libc/sysv/consts/nr.h"
#include "libc/sysv/consts/nrlinux.h"
#include "libc/sysv/consts/pr.h"
#include "libc/sysv/consts/ptrace.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"
#include "net/http/ip.h"

#define ORIG_RAX 120
#define RAX      80
#define RDI      112
#define RSI      104
#define RDX      96
#define R8       72
#define R9       64
#define __WALL   0x40000000

#define OFF(f) offsetof(struct seccomp_data, f)

#if 0
#define DEBUG(...) kprintf(__VA_ARGS__)
#else
#define DEBUG(...) donothing
#endif

#define ORDIE(x)                                              \
  do {                                                        \
    if (UNLIKELY((x) == -1)) {                                \
      DEBUG("%s:%d: %s failed %m\n", __FILE__, __LINE__, #x); \
      notpossible;                                            \
    }                                                         \
  } while (0)

static const struct sock_filter kInetBpf[] = {
    // cargo culted architecture assertion
    BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(arch)),
    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, AUDIT_ARCH_X86_64, 1, 0),
    BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_KILL_PROCESS),
    // block system calls from the future
    BPF_STMT(BPF_LD + BPF_W + BPF_ABS, OFF(nr)),
    BPF_JUMP(BPF_JMP | BPF_JGE | BPF_K, __NR_linux_memfd_secret, 0, 1),
    BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ERRNO | 38),  // ENOSYS
    // only allow local and internet sockets
    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_socket, 0, 5),
    BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(args[0])),
    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x001, 2, 0),  // AF_UNIX
    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x002, 1, 0),  // AF_INET
    BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ERRNO | 1),  // EPERM
    BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
    // support for these not implemented yet
    BPF_JUMP(BPF_JMP + BPF_JEQ + BPF_K, 0x133, 0, 1),  // sendmmsg
    BPF_STMT(BPF_RET + BPF_K, SECCOMP_RET_ERRNO | 1),  // EPERM
    // trace syscalls with struct sockaddr
    BPF_JUMP(BPF_JMP + BPF_JEQ + BPF_K, 0x02e, 3, 0),  // sendmsg
    BPF_JUMP(BPF_JMP + BPF_JEQ + BPF_K, 0x02c, 2, 0),  // sendto
    BPF_JUMP(BPF_JMP + BPF_JEQ + BPF_K, 0x031, 1, 0),  // bind
    BPF_JUMP(BPF_JMP + BPF_JEQ + BPF_K, 0x02a, 0, 1),  // connect
    BPF_STMT(BPF_RET + BPF_K, SECCOMP_RET_TRACE),
    // default course of action
    BPF_STMT(BPF_RET + BPF_K, SECCOMP_RET_ALLOW),
};

static int PeekData(int pid, long addr, void *buf, size_t size) {
  long i, j, w;
  for (i = 0; i < size; i += sizeof(long)) {
    if (sys_ptrace(PTRACE_PEEKTEXT, pid, addr + i, &w) != -1) {
      for (j = 0; i + j < size && j < sizeof(long); ++j) {
        ((char *)buf)[i + j] = w;
        w >>= 8;
      }
    } else {
      return -1;
    }
  }
  return 0;
}

static void LogProcessEvent(int main, int pid, int ws) {
  DEBUG("trace: %s%06d%s 0x%06x",       //
        pid == main ? "\e[31;1m" : "",  //
        pid,                            //
        pid == main ? "\e[0m" : "",     //
        ws);
  if (WIFEXITED(ws)) {
    DEBUG(" exit %d", WEXITSTATUS(ws));
  }
  if (WIFSIGNALED(ws)) {
    DEBUG(" sig %d", WTERMSIG(ws));
  }
  if (WIFSTOPPED(ws)) {
    DEBUG(" stop %s %s", strsignal(WSTOPSIG(ws)),
          DescribePtraceEvent((ws & 0xff0000) >> 16));
  }
  if (WIFCONTINUED(ws)) {
    DEBUG(" cont");
  }
  if (WCOREDUMP(ws)) {
    DEBUG(" core");
  }
  DEBUG("\n");
}

static int Raise(int sig) {
  sigset_t mask;
  sigaction(sig, &(struct sigaction){0}, 0);
  sigfillset(&mask);
  sigprocmask(SIG_SETMASK, &mask, 0);
  kill(getpid(), sig);
  sigdelset(&mask, sig);
  sigprocmask(SIG_SETMASK, &mask, 0);
  _Exit(128 + sig);
}

static bool IsSockaddrAllowed(struct sockaddr_storage *addr) {
  uint32_t ip;
  if (addr->ss_family == AF_UNIX) {
    return true;
  }
  if (addr->ss_family == AF_INET) {
    ip = ntohl(((struct sockaddr_in *)addr)->sin_addr.s_addr);
    if (IsPrivateIp(ip) || IsLoopbackIp(ip)) {
      return true;
    } else {
      kprintf("warning: attempted to communicate with public ip "
              "%hhd.%hhd.%hhd.%hhd\n",
              ip >> 24, ip >> 16, ip >> 8, ip);
      return false;
    }
  }
  DEBUG("bad family %d\n", addr->ss_family);
  return false;
}

static void OnSockaddrSyscall(int pid, int r1, int r2) {
  long si, dx;
  uint32_t addrlen;
  struct sockaddr_storage addr = {0};
  ORDIE(sys_ptrace(PTRACE_PEEKUSER, pid, r1, &si));
  ORDIE(sys_ptrace(PTRACE_PEEKUSER, pid, r2, &dx));
  addrlen = dx;
  if (!si) {
    // if address isn't supplied, it's probably safe. for example,
    // send() is implemented in cosmo using sendto() with 0/0 addr
    return;
  }
  if (PeekData(pid, si, &addr, MIN(addrlen, sizeof(addr))) == -1) {
    DEBUG("failed to peek addr\n");  // probably an efault
    goto Deny;
  }
  if (IsSockaddrAllowed(&addr)) {
    return;
  } else {
    goto Deny;
  }
Deny:
  ORDIE(sys_ptrace(PTRACE_POKEUSER, pid, ORIG_RAX, -1));
}

static void OnSendmsg(int pid) {
  long si;
  struct msghdr msg = {0};
  struct sockaddr_storage addr = {0};
  ORDIE(sys_ptrace(PTRACE_PEEKUSER, pid, RSI, &si));
  if (PeekData(pid, si, &msg, sizeof(msg)) == -1) {
    DEBUG("failed to peek msg\n");  // probably an efault
    goto Deny;
  }
  if (!msg.msg_name) {
    // if address isn't supplied, it's probably fine.
    return;
  }
  if (PeekData(pid, (long)msg.msg_name, &addr,
               MIN(msg.msg_namelen, sizeof(addr))) == -1) {
    DEBUG("failed to peek msg name\n");  // probably an efault
    goto Deny;
  }
  if (IsSockaddrAllowed(&addr)) {
    return;
  } else {
    goto Deny;
  }
Deny:
  ORDIE(sys_ptrace(PTRACE_POKEUSER, pid, ORIG_RAX, -1));
}

static void HandleSeccompTrace(int pid) {
  long ax;
  ORDIE(sys_ptrace(PTRACE_PEEKUSER, pid, ORIG_RAX, &ax));
  switch (ax) {
    case 0x031:  // bind
    case 0x02a:  // connect
      OnSockaddrSyscall(pid, RSI, RDX);
      break;
    case 0x02c:  // sendto
      OnSockaddrSyscall(pid, R8, R9);
      break;
    case 0x02e:  // sendmsg
      OnSendmsg(pid);
      break;
    default:
      break;
  }
}

static int WaitForTrace(int main) {
  int ws, pid;
  for (;;) {
    // waits for state change on any child process or thread
    // eintr isn't possible since we're blocking all signals
    ORDIE(pid = waitpid(-1, &ws, __WALL));
    LogProcessEvent(main, pid, ws);
    if (WIFEXITED(ws)) {
      if (pid == main) {
        _Exit(WEXITSTATUS(ws));
      }
    } else if (WIFSIGNALED(ws)) {
      if (pid == main) {
        Raise(WTERMSIG(ws));
      }
    } else if (WIFSTOPPED(ws)) {
      if ((ws >> 8) == (SIGTRAP | (PTRACE_EVENT_SECCOMP << 8))) {
        return pid;
      } else if ((ws >> 8) == (SIGTRAP | (PTRACE_EVENT_EXEC << 8))) {
        ORDIE(ptrace(PTRACE_CONT, pid, 0, 0));
      } else if ((ws >> 8) == (SIGTRAP | (PTRACE_EVENT_FORK << 8)) ||
                 (ws >> 8) == (SIGTRAP | (PTRACE_EVENT_VFORK << 8)) ||
                 (ws >> 8) == (SIGTRAP | (PTRACE_EVENT_CLONE << 8))) {
        ORDIE(ptrace(PTRACE_CONT, pid, 0, 0));
      } else {
        ORDIE(ptrace(PTRACE_CONT, pid, 0, WSTOPSIG(ws)));
      }
    }
  }
}

/**
 * Disables internet access.
 *
 * Warning: This function uses ptrace to react to seccomp filter events.
 * This approach is effective, but it's not bulletproof, since a highly
 * motivated attacker could theoretically use threads to modify sockaddr
 * in the short time between it being monitored and the actual syscall.
 */
int nointernet(void) {
  int ws, act, main;
  sigset_t set, old;
  char path[PATH_MAX];
  struct sock_fprog prog = {.filter = kInetBpf, .len = ARRAYLEN(kInetBpf)};

  // seccomp bpf and ptrace are pretty much just linux for now.
  if (!IsLinux() || !__is_linux_2_6_23()) {
    return enosys();
  }

  // prevent crash handlers from intercepting sigsegv
  ORDIE(sigfillset(&set));
  ORDIE(sigprocmask(SIG_SETMASK, &set, &old));

  // create traced child that'll replace this program
  if ((main = fork()) == -1) {
    ORDIE(sigprocmask(SIG_SETMASK, &old, 0));
    return -1;
  }
  if (!main) {
    if (sys_ptrace(PTRACE_TRACEME, 0, 0, 0) == -1) {
      // there can be only one
      // throw sigsegv on eperm
      // we're already being traced
      asm("hlt");
    }
    prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);
    ORDIE(prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &prog));
    ORDIE(kill(getpid(), SIGSTOP));
    ORDIE(sigprocmask(SIG_SETMASK, &old, 0));
    // return to caller from child
    return 0;
  }

  // wait for child to stop itself
  ORDIE(waitpid(main, &ws, 0));
  if (WIFSIGNALED(ws)) {
    // child couldn't enable ptrace or seccomp
    sigprocmask(SIG_SETMASK, &old, 0);
    return eperm();
  }
  _npassert(WIFSTOPPED(ws));

  // parent process becomes monitor of subprocess tree. all signals
  // continue to be blocked since we assume they'll also be sent to
  // children, which will die, and then the monitor dies afterwards
  ORDIE(sys_ptrace(PTRACE_SETOPTIONS, main, 0,
                   PTRACE_O_TRACESECCOMP | PTRACE_O_TRACEFORK |
                       PTRACE_O_TRACEVFORK | PTRACE_O_TRACECLONE |
                       PTRACE_O_TRACEEXEC));
  for (act = main;;) {
    ORDIE(sys_ptrace(PTRACE_CONT, act, 0, 0));
    act = WaitForTrace(main);
    HandleSeccompTrace(act);
  }
}
