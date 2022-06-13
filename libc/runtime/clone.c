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
#include "libc/calls/strace.internal.h"
#include "libc/calls/struct/ucontext-netbsd.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/spinlock.h"
#include "libc/limits.h"
#include "libc/nexgen32e/threaded.h"
#include "libc/nt/runtime.h"
#include "libc/nt/thread.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/clone.h"
#include "libc/sysv/consts/futex.h"
#include "libc/sysv/consts/nr.h"
#include "libc/sysv/consts/nrlinux.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/freebsd.internal.h"
#include "libc/thread/xnu.internal.h"

STATIC_YOINK("gettid");  // for kprintf()

#define __NR_thr_new                      455
#define __NR_clone_linux                  56
#define __NR__lwp_create                  309
#define __NR_getcontext_netbsd            307
#define __NR_bsdthread_create             0x02000168
#define __NR_thread_fast_set_cthread_self 0x03000003
#define PTHREAD_START_CUSTOM_XNU          0x01000000
#define LWP_DETACHED                      0x00000040
#define LWP_SUSPENDED                     0x00000080

__msabi extern typeof(TlsSetValue) *const __imp_TlsSetValue;
__msabi extern typeof(ExitThread) *const __imp_ExitThread;

struct CloneArgs {
  union {
    int tid;
    uint32_t utid;
    int64_t tid64;
  };
  union {
    int lock;
    void *pstack;
  };
  int *ctid;
  int *ztid;
  char *tls;
  int (*func)(void *);
  void *arg;
};

////////////////////////////////////////////////////////////////////////////////
// THREADING RUNTIME

static char tibdefault[64];
extern int __threadcalls_end[];
extern int __threadcalls_start[];

static privileged dontinline void FixupThreadCalls(void) {
  /*
   * _NOPL("__threadcalls", func)
   *
   * we have this
   *
   *     0f 1f 05 b1 19 00 00  nopl func(%rip)
   *
   * we're going to turn it into this
   *
   *     67 67 e8 b1 19 00 00  addr32 addr32 call func
   */
  __morph_begin();
  for (int *p = __threadcalls_start; p < __threadcalls_end; ++p) {
    _base[*p + 0] = 0x67;
    _base[*p + 1] = 0x67;
    _base[*p + 2] = 0xe8;
  }
  __morph_end();
}

////////////////////////////////////////////////////////////////////////////////
// THE NEW TECHNOLOGY

int WinThreadLaunch(void *arg, int (*func)(void *), intptr_t rsp);

// we can't log this function because:
//   1. windows owns the backtrace pointer right now
//   2. ftrace unwinds rbp to determine depth
// we can't use address sanitizer because:
//   1. __asan_handle_no_return wipes stack
//   2. windows owns the stack memory right now
// we need win32 raw imports because:
//   1. generated thunks are function logged
noasan noinstrument static textwindows wontreturn void WinThreadEntry(
    int rdi, int rsi, int rdx, struct CloneArgs *wt) {
  int rc;
  if (wt->tls) {
    asm("mov\t%1,%%gs:%0"
        : "=m"(*((long *)0x1480 + __tls_index))
        : "r"(wt->tls));
  }
  *wt->ctid = wt->tid;
  rc = WinThreadLaunch(wt->arg, wt->func, (intptr_t)wt & -16);
  // we can now clear ctid directly since we're no longer using our own
  // stack memory, which can now be safely free'd by the parent thread.
  *wt->ztid = 0;
  // since we didn't indirect this function through NT2SYSV() it's not
  // safe to simply return, and as such, we just call ExitThread().
  __imp_ExitThread(rc);
  unreachable;
}

static textwindows int CloneWindows(int (*func)(void *), char *stk,
                                    size_t stksz, int flags, void *arg,
                                    void *tls, size_t tlssz, int *ctid) {
  int64_t h;
  struct CloneArgs *wt;
  wt = (struct CloneArgs *)(((intptr_t)(stk + stksz) -
                             sizeof(struct CloneArgs)) &
                            -alignof(struct CloneArgs));
  wt->ctid = flags & CLONE_CHILD_SETTID ? ctid : &wt->tid;
  wt->ztid = flags & CLONE_CHILD_CLEARTID ? ctid : &wt->tid;
  wt->func = func;
  wt->arg = arg;
  wt->tls = flags & CLONE_SETTLS ? tls : 0;
  if ((h = CreateThread(0, 4096, (void *)WinThreadEntry, wt, 0, &wt->utid))) {
    CloseHandle(h);
    return wt->tid;
  } else {
    return -1;
  }
}

////////////////////////////////////////////////////////////////////////////////
// XNU'S NOT UNIX

void XnuThreadThunk(void *pthread, int machport, void *(*func)(void *),
                    void *arg, intptr_t *stack, unsigned xnuflags);
asm("XnuThreadThunk:\n\t"
    "xor\t%ebp,%ebp\n\t"
    "mov\t%r8,%rsp\n\t"
    "and\t$-16,%rsp\n\t"
    "push\t%rax\n\t"
    "jmp\tXnuThreadMain\n\t"
    ".size\tXnuThreadThunk,.-XnuThreadThunk");
__attribute__((__used__, __no_reorder__))

static wontreturn void
XnuThreadMain(void *pthread, int tid, int (*func)(void *arg), void *arg,
              struct CloneArgs *wt, unsigned xnuflags) {
  int ax;
  wt->tid = tid;
  _spunlock(&wt->lock);
  if (wt->tls) {
    // XNU uses the same 0x30 offset as the WIN32 TIB x64. They told the
    // Go team at Google that they Apply stands by our ability to use it
    // https://github.com/golang/go/issues/23617#issuecomment-376662373
    asm volatile("syscall"
                 : "=a"(ax)
                 : "0"(__NR_thread_fast_set_cthread_self), "D"(wt->tls - 0x30)
                 : "rcx", "r11", "memory", "cc");
  }
  if (wt->ctid) {
    *wt->ctid = tid;
  }
  func(arg);
  // we no longer use the stack after this point
  // %rax = int bsdthread_terminate(%rdi = void *stackaddr,
  //                                %rsi = size_t freesize,
  //                                %rdx = uint32_t port,
  //                                %r10 = uint32_t sem);
  asm volatile("movl\t$0,%0\n\t"         // *wt->ztid = 0
               "xor\t%%r10d,%%r10d\n\t"  // sem = 0
               "syscall\n\t"             // __bsdthread_terminate()
               "ud2"
               : "=m"(*wt->ztid)
               : "a"(0x2000000 | 361), "D"(0), "S"(0), "d"(0)
               : "rcx", "r10", "r11", "memory");
  unreachable;
}

static int CloneXnu(int (*fn)(void *), char *stk, size_t stksz, int flags,
                    void *arg, void *tls, size_t tlssz, int *ctid) {
  int rc;
  bool failed;
  static bool once;
  static int broken;
  struct CloneArgs *wt;
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
  wt = (struct CloneArgs *)(((intptr_t)(stk + stksz) -
                             sizeof(struct CloneArgs)) &
                            -alignof(struct CloneArgs));
  wt->ctid = flags & CLONE_CHILD_SETTID ? ctid : &wt->tid;
  wt->ztid = flags & CLONE_CHILD_CLEARTID ? ctid : &wt->tid;
  wt->tls = flags & CLONE_SETTLS ? tls : 0;
  wt->lock = 1;
  if ((rc = bsdthread_create(fn, arg, wt, 0, PTHREAD_START_CUSTOM_XNU)) != -1) {
    _spinlock(&wt->lock);
    rc = wt->tid;
    _spunlock(&wt->lock);
  }
  return rc;
}

////////////////////////////////////////////////////////////////////////////////
// FREE BESIYATA DISHMAYA

static wontreturn void FreebsdThreadMain(void *p) {
  struct CloneArgs *wt = p;
  *wt->ctid = wt->tid;
  wt->func(wt->arg);
  // we no longer use the stack after this point
  // void thr_exit(%rdi = long *state);
  asm volatile("movl\t$0,%0\n\t"  // *wt->ztid = 0
               "syscall"          // thr_exit()
               : "=m"(*wt->ztid)
               : "a"(431), "D"(0)
               : "rcx", "r11", "memory");
  unreachable;
}

static int CloneFreebsd(int (*func)(void *), char *stk, size_t stksz, int flags,
                        void *arg, void *tls, size_t tlssz, int *ctid) {
  int ax;
  bool failed;
  int64_t tid;
  struct CloneArgs *wt;
  wt = (struct CloneArgs *)(((intptr_t)(stk + stksz) -
                             sizeof(struct CloneArgs)) &
                            -alignof(struct CloneArgs));
  wt->ctid = flags & CLONE_CHILD_SETTID ? ctid : &wt->tid;
  wt->ztid = flags & CLONE_CHILD_CLEARTID ? ctid : &wt->tid;
  wt->tls = tls;
  wt->func = func;
  wt->arg = arg;
  struct thr_param params = {
      .start_func = FreebsdThreadMain,
      .arg = wt,
      .stack_base = stk,
      .stack_size = (((intptr_t)wt - (intptr_t)stk) & -16) - 8,
      .tls_base = flags & CLONE_SETTLS ? tls : 0,
      .tls_size = flags & CLONE_SETTLS ? tlssz : 0,
      .child_tid = &wt->tid64,
      .parent_tid = &tid,
  };
  asm volatile(CFLAG_ASM("syscall")
               : CFLAG_CONSTRAINT(failed), "=a"(ax)
               : "1"(__NR_thr_new), "D"(&params), "S"(sizeof(params))
               : "rcx", "rdx", "r8", "r9", "r10", "r11", "memory");
  if (failed) {
    errno = ax;
    tid = -1;
  }
  return tid;
}

////////////////////////////////////////////////////////////////////////////////
// OPEN BESIYATA DISHMAYA

struct __tfork {
  void *tf_tcb;
  int32_t *tf_tid;
  void *tf_stack;
};

int __tfork(struct __tfork *params, size_t psize, struct CloneArgs *wt);
asm("__tfork:\n\t"
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
    "and\t$-16,%rsp\n\t"
    "push\t%rax\n\t"
    "jmp\tOpenbsdThreadMain\n\t"
    ".size\t__tfork,.-__tfork\n\t");
__attribute__((__used__, __no_reorder__))

static wontreturn void
OpenbsdThreadMain(struct CloneArgs *wt) {
  wt->func(wt->arg);
  // we no longer use the stack after this point. however openbsd
  // validates the rsp register too so a race condition can still
  // happen if the parent tries to free the stack. we'll solve it
  // by simply changing rsp back to the old value before exiting!
  // although ideally there should be a better solution.
  //
  // void __threxit(%rdi = int32_t *notdead);
  asm volatile("mov\t%2,%%rsp\n\t"
               "movl\t$0,(%%rdi)\n\t"  // *wt->ztid = 0
               "syscall\n\t"           // futex()
               "mov\t$302,%%eax\n\t"   // __threxit()
               "syscall"
               : "=m"(*wt->ztid)
               : "a"(83), "m"(wt->pstack), "D"(wt->ztid), "S"(FUTEX_WAKE),
                 "d"(INT_MAX)
               : "rcx", "r11", "memory");
  unreachable;
}

static int CloneOpenbsd(int (*func)(void *), char *stk, size_t stksz, int flags,
                        void *arg, void *tls, size_t tlssz, int *ctid) {
  int tid;
  struct CloneArgs *wt;
  struct __tfork params;
  wt = (struct CloneArgs *)(((intptr_t)(stk + stksz) -
                             sizeof(struct CloneArgs)) &
                            -alignof(struct CloneArgs));
  wt->ctid = flags & CLONE_CHILD_SETTID ? ctid : &wt->tid;
  wt->ztid = flags & CLONE_CHILD_CLEARTID ? ctid : &wt->tid;
  wt->pstack = __builtin_frame_address(0);
  wt->func = func;
  wt->arg = arg;
  params.tf_stack = wt;
  params.tf_tcb = flags & CLONE_SETTLS ? tls : 0;
  params.tf_tid = flags & CLONE_CHILD_SETTID ? ctid : 0;
  if ((tid = __tfork(&params, sizeof(params), wt)) < 0) {
    errno = -tid;
    tid = -1;
  }
  return tid;
}

////////////////////////////////////////////////////////////////////////////////
// NET BESIYATA DISHMAYA

static wontreturn void NetbsdThreadMain(void *arg, int (*func)(void *arg),
                                        int *tid, int *ctid, int *ztid) {
  int ax, dx;
  *ctid = *tid;
  func(arg);
  // we no longer use the stack after this point
  // %eax = int __lwp_exit(void);
  asm volatile("movl\t$0,%2\n\t"  // *wt->ztid = 0
               "syscall\n\t"      // __lwp_exit()
               "ud2"
               : "=a"(ax), "=d"(dx), "=m"(*ztid)
               : "0"(310)
               : "rcx", "r11", "memory");
  unreachable;
}

static int CloneNetbsd(int (*func)(void *), char *stk, size_t stksz, int flags,
                       void *arg, void *tls, size_t tlssz, int *ctid) {
  // NetBSD has its own clone() and it works, but it's technically a
  // second-class API, intended to help Linux folks migrate to this!
  // We put it on the thread's stack, to avoid locking this function
  // so its stack doesn't scope. The ucontext struct needs 784 bytes
  bool failed;
  int ax, *tid;
  intptr_t dx, sp;
  static bool once;
  static int broken;
  struct ucontext_netbsd ctx;
  static struct ucontext_netbsd netbsd_clone_template;

  // memoize arbitrary valid processor state structure
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
  sp = sp & -alignof(int);
  tid = (int *)sp;
  sp = sp & -16;
  sp -= 8;
  // pass parameters in process state
  memcpy(&ctx, &netbsd_clone_template, sizeof(ctx));
  ctx.uc_link = 0;
  ctx.uc_mcontext.rbp = 0;
  ctx.uc_mcontext.rsp = sp;
  ctx.uc_mcontext.rip = (intptr_t)NetbsdThreadMain;
  ctx.uc_mcontext.rdi = (intptr_t)arg;
  ctx.uc_mcontext.rsi = (intptr_t)func;
  ctx.uc_mcontext.rdx = (intptr_t)tid;
  ctx.uc_mcontext.rcx = (intptr_t)(flags & CLONE_CHILD_SETTID ? ctid : tid);
  ctx.uc_mcontext.r8 = (intptr_t)(flags & CLONE_CHILD_CLEARTID ? ctid : tid);
  ctx.uc_flags |= _UC_STACK;
  ctx.uc_stack.ss_sp = stk;
  ctx.uc_stack.ss_size = stksz;
  ctx.uc_stack.ss_flags = 0;
  if (flags & CLONE_SETTLS) {
    ctx.uc_flags |= _UC_TLSBASE;
    ctx.uc_mcontext._mc_tlsbase = (intptr_t)tls;
  }

  // perform the system call
  asm volatile(CFLAG_ASM("syscall")
               : CFLAG_CONSTRAINT(failed), "=a"(ax), "=d"(dx)
               : "1"(__NR__lwp_create), "D"(&ctx), "S"(LWP_DETACHED), "2"(tid)
               : "rcx", "r11", "memory");
  if (!failed) {
    return *tid;
  } else {
    errno = ax;
    return -1;
  }
}

////////////////////////////////////////////////////////////////////////////////
// GNU/SYSTEMD

int sys_clone_linux(int flags, char *stk, int *ptid, int *ctid, void *tls,
                    int (*func)(void *), void *arg);

////////////////////////////////////////////////////////////////////////////////
// COSMOPOLITAN

/**
 * Creates thread.
 *
 * Threads are created in a detached manner. They currently can't be
 * synchronized using wait() or posix signals. Threads created by this
 * function should be synchronized using shared memory operations.
 *
 * Any memory that's required by this system call wrapper is allocated
 * to the top of your stack. This shouldn't be more than 128 bytes.
 *
 * Your function is called from within the stack you specify. A return
 * address is pushed onto your stack, that causes returning to jump to
 * _Exit1() which terminates the thread. Even though the callback says
 * it supports a return code, that'll only work on Linux and Windows.
 *
 * The `tls` parameter is for thread-local storage. If you specify this
 * then clone() will implicitly rewire libc (e.g. errno) to use TLS:
 *
 *     static char tib[64];
 *     __initialize_tls(tib);
 *     __install_tls(tib);
 *
 * If you want a main process TLS size that's larger call it manually.
 * Once you've done the above and/or started creating your own threads
 * you'll be able to access your `tls` thread information block, using
 *
 *     char *p = __get_tls();
 *     printf("errno is %d\n", *(int *)(p + 0x3c));
 *
 * This function follows the same ABI convention as the Linux userspace
 * libraries, with a few small changes. The varargs has been removed to
 * help prevent broken code, and the stack size and tls size parameters
 * are introduced for compatibility with FreeBSD.
 *
 * To keep this system call lightweight, only the thread creation use
 * case is polyfilled across platforms. For example, if you want fork
 * that works on OpenBSD for example, don't do it with clone(SIGCHLD)
 * and please just call fork(). Even if you do that on Linux, it will
 * effectively work around libc features like atfork(), so that means
 * other calls like getpid() may return incorrect values.
 *
 * @param func is your callback function, which this wrapper requires
 *     not be null, otherwise EINVAL is raised
 * @param stk points to the bottom of a caller allocated stack, which
 *     must be allocated via mmap() using the MAP_STACK flag, or else
 *     you won't get optimal performance and it won't work on OpenBSD
 * @param stksz is the size of that stack in bytes, we recommend that
 *     that this be set to GetStackSize() or else memory safety tools
 *     like kprintf() can't do as good and quick of a job; this value
 *     must be 16-aligned plus it must be at least 4192 bytes in size
 *     and it's advised to have the bottom-most page, be a guard page
 * @param flags should have:
 *     - `CLONE_THREAD|CLONE_VM|CLONE_FS|CLONE_FILES|CLONE_SIGHAND`
 *     and you may optionally bitwise or any of the following:
 *     - `CLONE_CHILD_SETTID` is needed too if you use `ctid` which
 *       is part of the memory the child owns and it'll be set right
 *       before the callback function is invoked
 *     - `CLONE_CHILD_CLEARTID` causes `*ctid = 0` upon termination
 *       which can be used to implement join so that the parent may
 *       safely free the stack memory that the child is using
 *     - `CLONE_PARENT_SETTID` is needed too if you use `ptid` and this
 *       is guaranteed to happen before clone() returns
 *     - `CLONE_SETTLS` is needed too if you set `tls`. You may get this
 *       value from the thread by calling __get_tls(). There are a few
 *       layout expectations imposed by your C library. Those are all
 *       documented by __initialize_tls() which initializes the parts of
 *       the first 64 bytes of tls memory that libc cares about. Also
 *       note that if you decide to use tls once then you must use it
 *       for everything, since this flag also flips a runtime state that
 *       enables it for the main thread and functions such as
 *       __errno_location() will begin assuming they can safely access
 *       the tls segment register.
 * @param arg will be passed to your callback
 * @param tls may be used to set the thread local storage segment;
 *     this parameter is ignored if `CLONE_SETTLS` is not set
 * @param tlssz is the size of tls in bytes which must be at least 64
 * @param ctid lets the child receive its thread id without having to
 *     call gettid() and is ignored if `CLONE_CHILD_SETTID` isn't set
 * @return tid of child on success, or -1 w/ errno
 * @threadsafe
 */
int clone(int (*func)(void *), void *stk, size_t stksz, int flags, void *arg,
          int *ptid, void *tls, size_t tlssz, int *ctid) {
  int rc, maintid;
  struct CloneArgs *wt;

  // transition program to threaded state
  if (!__threaded && (flags & CLONE_THREAD)) {
    FixupThreadCalls();
  }
  if ((flags & CLONE_SETTLS) && !__tls_enabled) {
    if (~flags & CLONE_THREAD) {
      STRACE("clone() tls w/o thread");
      return einval();
    }
    if (__threaded) {
      STRACE("clone() tls/non-tls mixed order");
      return einval();
    }
    maintid = gettid();
    __initialize_tls(tibdefault);
    *(int *)((char *)tibdefault + 0x38) = maintid;
    *(int *)((char *)tibdefault + 0x3c) = __errno;
    __install_tls(tibdefault);
    __threaded = maintid;
  } else if (flags & CLONE_THREAD) {
    __threaded = gettid();
  }

  if (!func) {
    rc = einval();
  } else if (!IsTiny() &&
             (((flags & CLONE_VM) && (stksz < PAGESIZE || (stksz & 15))) ||
              ((flags & CLONE_SETTLS) && (tlssz < 64 || (tlssz & 7))))) {
    rc = einval();
  } else if (IsAsan() &&
             ((stksz > PAGESIZE &&
               !__asan_is_valid((char *)stk + PAGESIZE, stksz - PAGESIZE)) ||
              ((flags & CLONE_SETTLS) && !__asan_is_valid(tls, tlssz)) ||
              ((flags & CLONE_SETTLS) && !__asan_is_valid(tls, sizeof(long))) ||
              ((flags & CLONE_PARENT_SETTID) &&
               !__asan_is_valid(ptid, sizeof(*ptid))) ||
              ((flags & CLONE_CHILD_SETTID) &&
               !__asan_is_valid(ctid, sizeof(*ctid))))) {
    rc = efault();
  } else if (IsLinux()) {
    rc =
        sys_clone_linux(flags, (char *)stk + stksz, ptid, ctid, tls, func, arg);
  } else if (!IsTiny() &&
             (flags & ~(CLONE_SETTLS | CLONE_PARENT_SETTID |
                        CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID)) !=
                 (CLONE_THREAD | CLONE_VM | CLONE_FS | CLONE_FILES |
                  CLONE_SIGHAND)) {
    STRACE("clone flag unsupported on this platform");
    rc = einval();
  } else if (IsXnu()) {
    rc = CloneXnu(func, stk, stksz, flags, arg, tls, tlssz, ctid);
  } else if (IsFreebsd()) {
    rc = CloneFreebsd(func, stk, stksz, flags, arg, tls, tlssz, ctid);
  } else if (IsNetbsd()) {
    rc = CloneNetbsd(func, stk, stksz, flags, arg, tls, tlssz, ctid);
  } else if (IsOpenbsd()) {
    rc = CloneOpenbsd(func, stk, stksz, flags, arg, tls, tlssz, ctid);
  } else if (IsWindows()) {
    rc = CloneWindows(func, stk, stksz, flags, arg, tls, tlssz, ctid);
  } else {
    rc = enosys();
  }

  if (rc != -1 && (flags & CLONE_PARENT_SETTID)) {
    *ptid = rc;
  }

  STRACE("clone(%p, %p, %'zu, %#x, %p, %p, %p, %'zu, %p) → %d% m", func, stk,
         stksz, flags, arg, ptid, tls, tlssz, ctid, rc);

  return rc;
}
