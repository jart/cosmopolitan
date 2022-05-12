/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/bits/asmflag.h"
#include "libc/bits/weaken.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/strace.internal.h"
#include "libc/calls/struct/ucontext-netbsd.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/spinlock.h"
#include "libc/intrin/tls.h"
#include "libc/intrin/winthread.internal.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/nt2sysv.h"
#include "libc/nexgen32e/stackframe.h"
#include "libc/nt/runtime.h"
#include "libc/nt/thread.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/clone.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/freebsd.internal.h"
#include "libc/thread/openbsd.internal.h"

// TODO(jart): work in progress

STATIC_YOINK("gettid");  // for kprintf()

#define __NR_thr_new           455
#define __NR___tfork           8
#define __NR_clone_linux       56
#define __NR__lwp_create       309
#define __NR_getcontext_netbsd 307
#define __NR__lwp_setprivate   317

extern bool __threaded;

static struct Cloner {
  _Alignas(64) char lock;
  _Alignas(64) int flags;
  int64_t tid;
  int (*func)(void *);
  void *arg;
  void *stack;
  int *ctid;
  int *ptid;
} __cloner;

static textwindows uint32_t WinThreadMain(void *notused) {
  intptr_t rdi, rdx;
  int (*func)(void *);
  void *arg, *stack;
  struct WinThread *wt;
  int exitcode, tid, flags, *ctid;
  tid = __cloner.tid;
  arg = __cloner.arg;
  func = __cloner.func;
  ctid = __cloner.ctid;
  flags = __cloner.flags;
  stack = __cloner.stack;
  _spunlock(&__cloner.lock);
  wt = calloc(1, sizeof(struct WinThread));
  wt->pid = tid;
  TlsSetValue(__winthread, wt);
  if (flags & CLONE_CHILD_SETTID) *ctid = tid;
  asm volatile("push\t%%rbp\n\t"
               "mov\t%%rsp,%%r15\n\t"
               "xor\t%%ebp,%%ebp\n\t"
               "xchg\t%%rax,%%rsp\n\t"
               "call\t*%2\n\t"
               "mov\t%%rbx,%%rbp\n\t"
               "mov\t%%r15,%%rsp\n\t"
               "pop\t%%rbp"
               : "=a"(exitcode), "=D"(rdi), "=d"(rdx)
               : "0"(stack), "1"(arg), "2"(func)
               : "rbx", "rcx", "rsi", "r8", "r9", "r10", "r11", "r15",
                 "memory");
  if (flags & CLONE_CHILD_CLEARTID) *ctid = 0;
  __releasefd(tid);
  free(wt);
  return exitcode;
}

static textwindows int CloneWindows(int (*func)(void *), void *stk,
                                    size_t stksz, int flags, void *arg,
                                    int *ptid, void *tls, size_t tlssz,
                                    int *ctid) {
  int tid;
  int64_t hand;
  uint32_t wintid;
  if ((tid = __reservefd(-1)) == -1) return -1;
  _spinlock(&__cloner.lock);
  __cloner.tid = tid;
  __cloner.arg = arg;
  __cloner.func = func;
  __cloner.ctid = ctid;
  __cloner.flags = flags;
  __cloner.stack = (char *)stk + stksz;
  if (!(hand = CreateThread(0, 0, NT2SYSV(WinThreadMain), 0, 0, &wintid))) {
    _spunlock(&__cloner.lock);
    return -1;
  }
  if (flags & CLONE_CHILD_SETTID) *ctid = tid;
  if (flags & CLONE_PARENT_SETTID) *ptid = tid;
  // XXX: this should be tracked in a separate data structure
  g_fds.p[tid].kind = kFdProcess;
  g_fds.p[tid].handle = hand;
  g_fds.p[tid].flags = O_CLOEXEC;
  g_fds.p[tid].zombie = false;
  return tid;
}

static dontinline wontreturn void BsdThreadMain(void *unused) {
  void *arg;
  int (*func)(void *);
  int tid, flags, exitcode, *ctid;
  asm("xor\t%ebp,%ebp");
  tid = __cloner.tid;
  arg = __cloner.arg;
  func = __cloner.func;
  ctid = __cloner.ctid;
  flags = __cloner.flags;
  _spunlock(&__cloner.lock);
  if (flags & CLONE_CHILD_SETTID) *ctid = tid;
  exitcode = func(arg);
  if (flags & CLONE_CHILD_CLEARTID) *ctid = 0;
  _Exit1(exitcode);
}

static privileged noasan int CloneFreebsd(int (*func)(void *), void *stk,
                                          size_t stksz, int flags, void *arg,
                                          int *ptid, void *tls, size_t tlssz,
                                          int *ctid) {
  int ax;
  bool failed;
  int64_t tid;
  struct thr_param params = {0};
  _spinlock(&__cloner.lock);
  __cloner.arg = arg;
  __cloner.func = func;
  __cloner.ctid = ctid;
  __cloner.flags = flags;
  params.start_func = BsdThreadMain;
  params.stack_base = stk;
  params.stack_size = stksz;
  params.tls_base = flags & CLONE_SETTLS ? tls : 0;
  params.tls_size = flags & CLONE_SETTLS ? tlssz : 0;
  params.child_tid = &__cloner.tid;
  params.parent_tid = &tid;
  asm volatile(CFLAG_ASM("syscall")
               : CFLAG_CONSTRAINT(failed), "=a"(ax)
               : "1"(__NR_thr_new), "D"(&params), "S"(sizeof(params))
               : "rcx", "r11", "memory", "cc");
  if (!failed) {
    if (flags & CLONE_PARENT_SETTID) *ptid = tid;
    return tid;
  } else {
    errno = ax;
    return -1;
  }
}

static privileged noasan int CloneOpenbsd(int (*func)(void *), char *stk,
                                          size_t stksz, int flags, void *arg,
                                          int *ptid, void *tls, size_t tlssz,
                                          int *ctid) {
  int ax;
  bool failed;
  struct __tfork params;
  _spinlock(&__cloner.lock);
  __cloner.arg = arg;
  __cloner.func = func;
  __cloner.ctid = ctid;
  __cloner.flags = flags;
  __cloner.tid = 0;
  asm volatile("" ::: "memory");
  params.tf_tid = (int *)&__cloner.tid;
  params.tf_tcb = flags & CLONE_SETTLS ? tls : 0;
  // we need openbsd:stackbound because openbsd kernel enforces rsp must
  // be on interval [stack, stack+size) thus the top address is an error
  // furthermore this needs to be allocated using MAP_STACK OR GROWSDOWN
  params.tf_stack = (void *)((intptr_t)((char *)stk + stksz - 1) & -16);
  asm volatile(CFLAG_ASM("syscall")
               : CFLAG_CONSTRAINT(failed), "=a"(ax)
               : "1"(__NR___tfork), "D"(&params), "S"(sizeof(params))
               : "r11", "memory", "cc");
  if (failed) {
    errno = ax;
    return -1;
  }
  if (ax) {
    if (flags & CLONE_PARENT_SETTID) *ptid = ax;
    return ax;
  }
  BsdThreadMain(0);
  unreachable;
}

static privileged noasan int CloneNetbsd(int (*func)(void *), void *stk,
                                         size_t stksz, int flags, void *arg,
                                         int *ptid, void *tls, size_t tlssz,
                                         int *ctid) {
  int ax, tid;
  bool failed;
  intptr_t *stack;
  struct ucontext_netbsd ctx;
  asm volatile(CFLAG_ASM("syscall")
               : CFLAG_CONSTRAINT(failed), "=a"(ax)
               : "1"(__NR_getcontext_netbsd), "D"(&ctx)
               : "rcx", "r11", "memory", "cc");
  if (failed) {
    errno = ax;
    return -1;
  }
  stack = (intptr_t *)((intptr_t)((char *)stk + stksz) & -16);
  *--stack = (intptr_t)_Exit1;
  ctx.uc_link = 0;
  ctx.uc_mcontext.rip = (intptr_t)func;
  ctx.uc_mcontext.rdi = (intptr_t)arg;
  ctx.uc_mcontext.rsp = (intptr_t)stack;
  ctx.uc_mcontext.rbp = 0;
  ctx.uc_flags |= _UC_STACK;
  ctx.uc_stack.ss_sp = stk;
  ctx.uc_stack.ss_size = stksz;
  ctx.uc_stack.ss_flags = 0;
  if (flags & CLONE_SETTLS) {
    ctx.uc_flags |= _UC_TLSBASE;
    ctx.uc_mcontext._mc_tlsbase = (intptr_t)tls;
  }
  asm volatile("" ::: "memory");
  asm volatile(CFLAG_ASM("syscall")
               : CFLAG_CONSTRAINT(failed), "=a"(ax)
               : "1"(__NR__lwp_create), "D"(&ctx), "S"(flags), "d"(&tid)
               : "rcx", "r11", "memory", "cc");
  if (failed) {
    errno = ax;
    return -1;
  }
  if (flags & CLONE_PARENT_SETTID) *ptid = ax;
  if (flags & CLONE_CHILD_SETTID) *ctid = ax;
  return tid;
}

static privileged int CloneLinux(int (*func)(void *), void *stk, size_t stksz,
                                 int flags, void *arg, int *ptid, void *tls,
                                 size_t tlssz, int *ctid) {
  int ax;
  bool failed;
  intptr_t *stack;
  register int *r8 asm("r8") = tls;
  register int (*r9)(void *) asm("r9") = func;
  register int *r10 asm("r10") = ctid;
  stack = (intptr_t *)((long)((char *)stk + stksz) & -16);
  *--stack = (long)arg;  // push 1
  asm volatile("syscall"
               : "=a"(ax)
               : "0"(__NR_clone_linux), "D"(flags), "S"(stack), "d"(ptid),
                 "r"(r10), "r"(r8), "r"(r9)
               : "rcx", "r11", "memory");
  if (ax > -4096u) {
    errno = -ax;
    return -1;
  }
  if (ax) return ax;
  asm volatile("xor\t%%ebp,%%ebp\n\t"
               "pop\t%%rdi\n\t"  // pop 1
               "call\t*%0\n\t"
               "xchg\t%%eax,%%edi\n\t"
               "jmp\t_Exit1"
               : /* no outputs */
               : "r"(r9)
               : "memory");
  unreachable;
}

/**
 * Creates thread.
 *
 * This function follows the same ABI convention as the Linux userspace
 * libraries, with a few small changes. The varargs has been removed to
 * help prevent broken code, and the stack size and tls size parameters
 * are introduced for compatibility with FreeBSD.
 *
 * @param func is your callback function
 * @param stk points to the bottom of a caller allocated stack, which
 *     must be null when fork() and vfork() equivalent flags are used
 *     and furthermore this must be mmap()'d using MAP_STACK in order
 *     to work on OpenBSD
 * @param stksz is the size of that stack in bytes which must be zero
 *     if the fork() or vfork() equivalent flags are used it's highly
 *     recommended that this value be GetStackSize(), or else kprintf
 *     and other runtime services providing memory safety can't do as
 *     good and quick of a job at that
 * @param flags usually has one of
 *     - `SIGCHLD` will delegate to fork()
 *     - `CLONE_VFORK|CLONE_VM|SIGCHLD` means vfork()
 *     - `CLONE_VM|CLONE_FS|CLONE_FILES|CLONE_SIGHAND` for threads
 *     as part high bytes, and the low order byte may optionally contain
 *     a signal e.g. SIGCHLD, to enable parent notification on terminate
 *     although the signal isn't supported on non-Linux and non-NetBSD
 *     at the moment; 'flags' may optionally bitwise or the following:
 *     - `CLONE_PARENT_SETTID` is needed for `ctid` should be set
 *     - `CLONE_CHILD_SETTID` is needed for `ptid` should be set
 *     - `CLONE_SETTLS` is needed to set `%fs` segment to `tls`
 * @param arg will be passed to your callback
 * @param ptid lets the parent receive the child thread id;
 *     this parameter is ignored if `CLONE_PARENT_SETTID` is not set
 * @param tls may be used to set the thread local storage segment;
 *     this parameter is ignored if `CLONE_SETTLS` is not set
 * @param tlssz is the size of tls in bytes
 * @param ctid lets the child receive its thread id;
 *     this parameter is ignored if `CLONE_CHILD_SETTID` is not set
 * @return tid on success and 0 to the child, otherwise -1 w/ errno
 * @returnstwice
 * @threadsafe
 */
privileged int clone(int (*func)(void *), void *stk, size_t stksz, int flags,
                     void *arg, int *ptid, void *tls, size_t tlssz, int *ctid) {
  int rc;
  __threaded = true;
  if (IsAsan() &&
      (!__asan_is_valid(stk, stksz) ||
       ((flags & CLONE_SETTLS) && !__asan_is_valid(tls, tlssz)) ||
       ((flags & CLONE_SETTLS) && !__asan_is_valid(tls, sizeof(long))) ||
       ((flags & CLONE_PARENT_SETTID) &&
        !__asan_is_valid(ptid, sizeof(*ptid))) ||
       ((flags & CLONE_CHILD_SETTID) &&
        !__asan_is_valid(ctid, sizeof(*ctid))))) {
    rc = efault();
  } else if (IsLinux()) {
    rc = CloneLinux(func, stk, stksz, flags, arg, ptid, tls, tlssz, ctid);
  } else if (IsNetbsd()) {
    rc = CloneNetbsd(func, stk, stksz, flags, arg, ptid, tls, tlssz, ctid);
  }

  // polyfill fork() and vfork() use case on platforms w/o clone
  else if ((SupportsWindows() || SupportsBsd()) &&
           flags == (CLONE_VFORK | CLONE_VM | SIGCHLD)) {
    if (IsTiny()) {
      rc = einval();
    } else if (!arg && !stksz) {
      return vfork();  // don't log clone()
    } else {
      rc = einval();
    }
  } else if ((SupportsWindows() || SupportsBsd()) && flags == SIGCHLD) {
    if (IsTiny()) {
      rc = eopnotsupp();
    } else if (!arg && !stksz) {
      return fork();  // don't log clone()
    } else {
      rc = einval();
    }
  }

  // we now assume we're creating a thread
  // these platforms can't do signals the way linux does
  else if ((flags &
            ~(CLONE_SETTLS | CLONE_PARENT_SETTID | CLONE_CHILD_SETTID)) !=
           (CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND)) {
    rc = einval();
  } else if (IsFreebsd()) {
    rc = CloneFreebsd(func, stk, stksz, flags, arg, ptid, tls, tlssz, ctid);
  } else if (IsOpenbsd()) {
    rc = CloneOpenbsd(func, stk, stksz, flags, arg, ptid, tls, tlssz, ctid);
  }

  // These platforms can't do segment registers like linux does
  else if (flags & CLONE_SETTLS) {
    rc = einval();
  } else if (IsWindows()) {
    rc = CloneWindows(func, stk, stksz, flags, arg, ptid, tls, tlssz, ctid);
  } else {
    rc = enosys();
  }

  STRACE("clone(%p, %p, %'zu, %#x, %p, %p, %p, %'zu, %p) → %d% m", func, stk,
         stksz, flags, arg, ptid, tls, tlssz, ctid, rc);
  return rc;
}
