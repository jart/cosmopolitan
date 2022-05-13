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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/strace.internal.h"
#include "libc/calls/struct/ucontext-netbsd.internal.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/spinlock.h"
#include "libc/intrin/threaded.internal.h"
#include "libc/intrin/tls.h"
#include "libc/intrin/winthread.internal.h"
#include "libc/nt/runtime.h"
#include "libc/nt/thread.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/clone.h"
#include "libc/sysv/consts/nr.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/freebsd.internal.h"
#include "libc/thread/xnu.internal.h"

STATIC_YOINK("gettid");  // for kprintf()

#define __NR_thr_new                      455
#define __NR_clone_linux                  56
#define __NR__lwp_create                  309
#define __NR_getcontext_netbsd            307
#define __NR__lwp_setprivate              317
#define __NR_bsdthread_create             0x02000168
#define __NR_thread_fast_set_cthread_self 0x03000003
#define PTHREAD_START_CUSTOM_XNU          0x01000000
#define LWP_DETACHED                      0x00000040
#define LWP_SUSPENDED                     0x00000080

uint32_t WinThreadThunk(void *warg);
asm(".section\t.text.windows,\"ax\",@progbits\n\t"
    ".local\tWinThreadThunk\n"
    "WinThreadThunk:\n\t"
    "xor\t%ebp,%ebp\n\t"
    "mov\t%rcx,%rdi\n\t"
    "mov\t%rcx,%rsp\n\t"
    "jmp\tWinThreadMain\n\t"
    ".size\tWinThreadThunk,.-WinThreadThunk\n\t"
    ".previous");
__attribute__((__used__, __no_reorder__))

static textwindows wontreturn void
WinThreadMain(struct WinThread *wt) {
  int rc;
  if (wt->flags & CLONE_CHILD_SETTID) {
    *wt->ctid = wt->tid;
  }
  // TlsSetValue(__winthread, wt);
  rc = wt->func(wt->arg);
  if (wt->flags & CLONE_CHILD_CLEARTID) {
    *wt->ctid = 0;
  }
  _Exit1(rc);
}

static textwindows int CloneWindows(int (*func)(void *), char *stk,
                                    size_t stksz, int flags, void *arg,
                                    int *ptid, void *tls, size_t tlssz,
                                    int *ctid) {
  int64_t h;
  struct WinThread *wt;
  wt = (struct WinThread *)(((intptr_t)(stk + stksz) -
                             sizeof(struct WinThread)) &
                            -alignof(struct WinThread));
  wt->flags = flags;
  wt->ctid = ctid;
  wt->func = func;
  wt->arg = arg;
  if ((h = CreateThread(0, 0, WinThreadThunk, wt, 0, &wt->tid))) {
    CloseHandle(h);
    if (flags & CLONE_PARENT_SETTID) {
      *ptid = wt->tid;
    }
    return wt->tid;
  } else {
    __releasefd(wt->tid);
    return -1;
  }
}

void XnuThreadThunk(void *pthread, int machport, void *(*func)(void *),
                    void *arg, intptr_t *stack, unsigned flags);
asm(".local\tXnuThreadThunk\n"
    "XnuThreadThunk:\n\t"
    "xor\t%ebp,%ebp\n\t"
    "mov\t%r8,%rsp\n\t"
    "jmp\tXnuThreadMain\n\t"
    ".size\tXnuThreadThunk,.-XnuThreadThunk");
__attribute__((__used__, __no_reorder__))

static wontreturn void
XnuThreadMain(void *pthread, int tid, int (*func)(void *arg), void *arg,
              intptr_t *sp, unsigned flags) {
  int rc;
  sp[1] = tid;
  _spunlock(sp);
  if (sp[4] & CLONE_SETTLS) {
    // XNU uses the same 0x30 offset as the WIN32 TIB x64. They told the
    // Go team at Google that they Apply stands by our ability to use it
    // https://github.com/golang/go/issues/23617#issuecomment-376662373
    asm volatile("syscall"
                 : "=a"(rc)
                 : "0"(__NR_thread_fast_set_cthread_self), "D"(sp[3] - 0x30)
                 : "rcx", "r11", "memory", "cc");
  }
  if (sp[4] & CLONE_CHILD_SETTID) {
    *(int *)sp[2] = tid;
  }
  rc = func(arg);
  if (sp[4] & CLONE_CHILD_CLEARTID) {
    *(int *)sp[2] = 0;
  }
  _Exit1(rc);
}

static int CloneXnu(int (*fn)(void *), char *stk, size_t stksz, int flags,
                    void *arg, int *ptid, void *tls, size_t tlssz, int *ctid) {
  int rc;
  bool failed;
  intptr_t *sp;
  static bool once;
  static int broken;
  if (!once) {
    if (bsdthread_register(XnuThreadThunk, 0, 0, 0, 0, 0, 0) == -1) {
      broken = errno;
    }
    once = true;
  }
  if (broken) {
    errno = broken;
    return -1;
  }
  sp = (intptr_t *)(stk + stksz);
  *--sp = 0;               // 5 padding
  *--sp = flags;           // 4 clone() flags
  *--sp = (intptr_t)tls;   // 3 thread local storage
  *--sp = (intptr_t)ctid;  // 2 child tid api
  *--sp = 0;               // 1 receives tid
  *--sp = 0;               // 0 lock
  _seizelock(sp);          // TODO: How can we get the tid without locking?
  if ((rc = bsdthread_create(fn, arg, sp, 0, PTHREAD_START_CUSTOM_XNU)) != -1) {
    _spinlock(sp);
    if (flags & CLONE_PARENT_SETTID) {
      *ptid = sp[1];
    }
    rc = sp[1];
  }
  return rc;
}

void FreebsdThreadThunk(void *sp) wontreturn;
asm(".local\tFreebsdThreadThunk\n"
    "FreebsdThreadThunk:\n\t"
    "xor\t%ebp,%ebp\n\t"
    "mov\t%rdi,%rsp\n\t"
    "jmp\tFreebsdThreadMain\n\t"
    ".size\tFreebsdThreadThunk,.-FreebsdThreadThunk");
__attribute__((__used__, __no_reorder__))

static wontreturn void
FreebsdThreadMain(intptr_t *sp) {
  int rc;
  if (sp[3] & CLONE_CHILD_SETTID) {
    *(int *)sp[2] = sp[4];
  }
  rc = ((int (*)(intptr_t))sp[0])(sp[1]);
  if (sp[3] & CLONE_CHILD_CLEARTID) {
    *(int *)sp[2] = 0;
  }
  _Exit1(rc);
}

static int CloneFreebsd(int (*func)(void *), char *stk, size_t stksz, int flags,
                        void *arg, int *ptid, void *tls, size_t tlssz,
                        int *ctid) {
  int ax;
  bool failed;
  int64_t tid;
  intptr_t *sp;
  sp = (intptr_t *)(stk + stksz);
  *--sp = 0;               // 5 [padding]
  *--sp = 0;               // 4 [child_tid]
  *--sp = flags;           // 3
  *--sp = (intptr_t)ctid;  // 2
  *--sp = (intptr_t)arg;   // 1
  *--sp = (intptr_t)func;  // 0
  struct thr_param params = {
      .start_func = FreebsdThreadThunk,
      .arg = sp,
      .stack_base = stk,
      .stack_size = stksz,
      .tls_base = flags & CLONE_SETTLS ? tls : 0,
      .tls_size = flags & CLONE_SETTLS ? tlssz : 0,
      .child_tid = sp + 4,
      .parent_tid = &tid,
  };
  asm volatile(CFLAG_ASM("syscall")
               : CFLAG_CONSTRAINT(failed), "=a"(ax)
               : "1"(__NR_thr_new), "D"(&params), "S"(sizeof(params))
               : "rcx", "rdx", "r8", "r9", "r10", "r11", "memory");
  if (!failed) {
    if (flags & CLONE_PARENT_SETTID) {
      *ptid = tid;
    }
    return tid;
  } else {
    errno = ax;
    return -1;
  }
}

struct __tfork {
  void *tf_tcb;
  int32_t *tf_tid;
  void *tf_stack;
};

int __tfork(struct __tfork *params, size_t psize, intptr_t *stack);
asm(".section\t.privileged,\"ax\",@progbits\n\t"
    ".local\t__tfork\n"
    "__tfork:\n\t"
    "push\t$8\n\t"
    "pop\t%rax\n\t"
    "mov\t%rdx,%r8\n\t"
    "syscall\n\t"
    "jc\t1f\n\t"
    "test\t%eax,%eax\n\t"
    "jz\t2f\n\t"
    "ret\n1:\t"
    "neg\t%eax\n\t"
    "ret\n2:\t"
    "xor\t%ebp,%ebp\n\t"
    "mov\t%r8,%rsp\n\t"
    "mov\t%r8,%rdi\n\t"
    "jmp\tOpenbsdThreadMain\n\t"
    ".size\t__tfork,.-__tfork\n\t"
    ".previous");
__attribute__((__used__, __no_reorder__))

static privileged wontreturn void
OpenbsdThreadMain(intptr_t *sp) {
  int rc;
  rc = ((int (*)(intptr_t))sp[0])(sp[1]);
  if (sp[3] & CLONE_CHILD_CLEARTID) {
    *(int *)sp[2] = 0;
  }
  _Exit1(rc);
}

static int CloneOpenbsd(int (*func)(void *), char *stk, size_t stksz, int flags,
                        void *arg, int *ptid, void *tls, size_t tlssz,
                        int *ctid) {
  int tid;
  intptr_t *sp;
  struct __tfork params;
  sp = (intptr_t *)(stk + stksz);
  *--sp = flags;           // 3
  *--sp = (intptr_t)ctid;  // 2
  *--sp = (intptr_t)arg;   // 1
  *--sp = (intptr_t)func;  // 0
  params.tf_stack = sp;
  params.tf_tcb = flags & CLONE_SETTLS ? tls : 0;
  params.tf_tid = flags & CLONE_CHILD_SETTID ? ctid : 0;
  if ((tid = __tfork(&params, sizeof(params), sp)) > 0) {
    if (flags & CLONE_PARENT_SETTID) {
      *ptid = tid;
    }
  } else {
    errno = -tid;
    tid = -1;
  }
  return tid;
}

static wontreturn void NetbsdThreadMain(void *arg, int (*func)(void *arg),
                                        int *tid, int *ctid, int flags) {
  int rc;
  if (flags & CLONE_CHILD_SETTID) {
    *ctid = *tid;
  }
  rc = func(arg);
  if (flags & CLONE_CHILD_CLEARTID) {
    *ctid = 0;
  }
  _Exit1(rc);
}

static int CloneNetbsd(int (*func)(void *), char *stk, size_t stksz, int flags,
                       void *arg, int *ptid, void *tls, size_t tlssz,
                       int *ctid) {
  // NetBSD has its own clone() and it works, but it's technically a
  // second-class API, intended to help Linux folks migrate to this!
  // We put it on the thread's stack, to avoid locking this function
  // so its stack doesn't scope. The ucontext struct needs 784 bytes
  bool failed;
  int ax, *tid;
  intptr_t dx, sp;
  static bool once;
  static int broken;
  struct ucontext_netbsd *ctx;
  static struct ucontext_netbsd netbsd_clone_template;
  if (!once) {
    asm volatile(CFLAG_ASM("syscall")
                 : CFLAG_CONSTRAINT(failed), "=a"(ax)
                 : "1"(__NR_getcontext_netbsd), "D"(&netbsd_clone_template)
                 : "rcx", "rdx", "r11", "memory");
    if (failed) {
      broken = ax;
    }
    once = true;
  }
  if (broken) {
    errno = broken;
    return -1;
  }
  sp = (intptr_t)(stk + stksz);
  sp -= sizeof(int);
  tid = (int *)sp;
  sp -= sizeof(*ctx);
  sp = sp & -alignof(*ctx);
  ctx = (struct ucontext_netbsd *)sp;
  memcpy(ctx, &netbsd_clone_template, sizeof(*ctx));
  ctx->uc_link = 0;
  ctx->uc_mcontext.rbp = 0;
  ctx->uc_mcontext.rsp = sp;
  ctx->uc_mcontext.rip = (intptr_t)NetbsdThreadMain;
  ctx->uc_mcontext.rdi = (intptr_t)arg;
  ctx->uc_mcontext.rsi = (intptr_t)func;
  ctx->uc_mcontext.rdx = (intptr_t)tid;
  ctx->uc_mcontext.rcx = (intptr_t)ctid;
  ctx->uc_mcontext.r8 = flags;
  ctx->uc_flags |= _UC_STACK;
  ctx->uc_stack.ss_sp = stk;
  ctx->uc_stack.ss_size = stksz;
  ctx->uc_stack.ss_flags = 0;
  if (flags & CLONE_SETTLS) {
    ctx->uc_flags |= _UC_TLSBASE;
    ctx->uc_mcontext._mc_tlsbase = (intptr_t)tls;
  }
  asm volatile(CFLAG_ASM("syscall")
               : CFLAG_CONSTRAINT(failed), "=a"(ax), "=d"(dx)
               : "1"(__NR__lwp_create), "D"(ctx), "S"(LWP_DETACHED), "2"(tid)
               : "rcx", "r11", "memory");
  if (!failed) {
    if (flags & CLONE_PARENT_SETTID) {
      *ptid = *tid;
    }
    return *tid;
  } else {
    errno = ax;
    return -1;
  }
}

static int CloneLinux(int (*func)(void *), char *stk, size_t stksz, int flags,
                      void *arg, int *ptid, void *tls, size_t tlssz,
                      int *ctid) {
  int ax;
  bool failed;
  intptr_t *stack;
  register int *r8 asm("r8") = tls;
  register int (*r9)(void *) asm("r9") = func;
  register int *r10 asm("r10") = ctid;
  stack = (intptr_t *)(stk + stksz);
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
 * Threads are created in a detached manner. They currently can't be
 * synchronized using wait() and posix signals. Threads created by this
 * function should be synchronized using shared memory operations.
 *
 * Any memory that's required by this system call wrapper is allocated
 * to the top of your stack. This is normally about 64 bytes, although
 * on NetBSD it's currently 800.
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
 *     good and quick of a job; this value must be 4096-aligned, plus
 *     it must be at minimum 4096 bytes in size
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
 * @threadsafe
 */
int clone(int (*func)(void *), void *stk, size_t stksz, int flags, void *arg,
          int *ptid, void *tls, size_t tlssz, int *ctid) {
  int rc;

  // let kprintf() switch from pids to tids
  __threaded = true;

  // verify memory is kosher
  if (IsAsan() &&
      ((stksz > PAGESIZE &&
        !__asan_is_valid((char *)stk + PAGESIZE, stksz - PAGESIZE)) ||
       ((flags & CLONE_SETTLS) && !__asan_is_valid(tls, tlssz)) ||
       ((flags & CLONE_SETTLS) && !__asan_is_valid(tls, sizeof(long))) ||
       ((flags & CLONE_PARENT_SETTID) &&
        !__asan_is_valid(ptid, sizeof(*ptid))) ||
       ((flags & CLONE_CHILD_SETTID) &&
        !__asan_is_valid(ctid, sizeof(*ctid))))) {
    rc = efault();
  }

  // delegate to bona fide clone()
  else if (IsLinux()) {
    rc = CloneLinux(func, stk, stksz, flags, arg, ptid, tls, tlssz, ctid);
  }

  // polyfill fork() and vfork() use cases on platforms without clone()
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
  else if (!IsTiny() &&
           ((stksz < PAGESIZE || (stksz & (PAGESIZE - 1))) ||
            (flags &
             ~(CLONE_SETTLS | CLONE_PARENT_SETTID | CLONE_CHILD_SETTID)) !=
                (CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND))) {
    rc = einval();
  } else if (IsXnu()) {
    rc = CloneXnu(func, stk, stksz, flags, arg, ptid, tls, tlssz, ctid);
  } else if (IsFreebsd()) {
    rc = CloneFreebsd(func, stk, stksz, flags, arg, ptid, tls, tlssz, ctid);
  } else if (IsNetbsd()) {
    rc = CloneNetbsd(func, stk, stksz, flags, arg, ptid, tls, tlssz, ctid);
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
