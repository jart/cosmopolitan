/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/atomic.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/ucontext-netbsd.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asmflag.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/ulock.h"
#include "libc/limits.h"
#include "libc/mem/alloca.h"
#include "libc/nt/enum/processcreationflags.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/sock/internal.h"
#include "libc/sysv/consts/arch.h"
#include "libc/thread/freebsd.internal.h"
#include "libc/thread/openbsd.internal.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/xnu.internal.h"

#define kMaxThreadIds 32768
#define kMinThreadId  262144

#define AMD64_SET_FSBASE 129
#define AMD64_SET_GSBASE 131

#define __NR_thr_new                      455
#define __NR_clone_linux                  56
#define __NR__lwp_create                  309
#define __NR_getcontext_netbsd            307
#define __NR_bsdthread_create             0x02000168
#define __NR_thread_fast_set_cthread_self 0x03000003
#define PTHREAD_START_CUSTOM_XNU          0x01000000
#define LWP_DETACHED                      0x00000040
#define LWP_SUSPENDED                     0x00000080

struct CloneArgs {
  union {
    long sp;
    int64_t tid64;
  };
  atomic_int *ptid;
  atomic_int *ctid;
  char *tls;
  int (*func)(void *);
  void *arg;
};

int sys_set_tls(uintptr_t, void *);
int __stack_call(void *, int, long, long, int (*)(void *), long);

#ifdef __x86_64__

////////////////////////////////////////////////////////////////////////////////
// THE NEW TECHNOLOGY

__msabi extern typeof(ExitThread) *const __imp_ExitThread;
__msabi extern typeof(GetCurrentThreadId) *const __imp_GetCurrentThreadId;
__msabi extern typeof(WakeByAddressAll) *const __imp_WakeByAddressAll;

textwindows dontinstrument wontreturn static void  //
WinThreadEntry(int rdi,                            // rcx
               int rsi,                            // rdx
               int rdx,                            // r8
               struct CloneArgs *wt) {             // r9
  __set_tls_win32(wt->tls);
  int tid = __imp_GetCurrentThreadId();
  atomic_int *ctid = wt->ctid;
  atomic_init(ctid, tid);
  atomic_init(wt->ptid, tid);
  int rc = __stack_call(wt->arg, tid, 0, 0, wt->func, wt->sp);
  // we can now clear ctid directly since we're no longer using our own
  // stack memory, which can now be safely free'd by the parent thread.
  atomic_store_explicit(ctid, 0, memory_order_release);
  __imp_WakeByAddressAll(ctid);
  // since we didn't indirect this function through NT2SYSV() it's not
  // safe to simply return, and as such, we need ExitThread().
  __imp_ExitThread(rc);
  __builtin_unreachable();
}

textwindows static errno_t CloneWindows(int (*func)(void *), char *stk,
                                        size_t stksz, void *arg, void *tls,
                                        atomic_int *ptid, atomic_int *ctid) {
  long sp;
  int64_t h;
  intptr_t tip;
  uint32_t utid;
  struct CloneArgs *wt;
  sp = tip = (intptr_t)stk + stksz;
  sp -= sizeof(struct CloneArgs);
  sp &= -alignof(struct CloneArgs);
  wt = (struct CloneArgs *)sp;
  wt->ctid = ctid;
  wt->ptid = ptid;
  wt->func = func;
  wt->arg = arg;
  wt->tls = tls;
  wt->sp = tip & -16;
  if ((h = CreateThread(0, 65536, (void *)WinThreadEntry, wt,
                        kNtStackSizeParamIsAReservation, &utid))) {
    atomic_init(ptid, utid);
    struct CosmoTib *tib = tls;
    atomic_store_explicit(&tib->tib_syshand, h, memory_order_release);
    return 0;
  } else {
    return __dos2errno(GetLastError());
  }
}

////////////////////////////////////////////////////////////////////////////////
// XNU'S NOT UNIX

void XnuThreadThunk(void *pthread,          // rdi x0
                    int machport,           // rsi x1
                    void *(*func)(void *),  // rdx x2
                    void *arg,              // rcx x3
                    intptr_t *stack,        // r8  x4
                    unsigned xnuflags);     // r9  x5
asm("XnuThreadThunk:\n\t"
    "xor\t%ebp,%ebp\n\t"
    "mov\t%r8,%rsp\n\t"
    "push\t%rax\n\t"
    "jmp\tXnuThreadMain\n\t"
    ".size\tXnuThreadThunk,.-XnuThreadThunk");
__attribute__((__used__))

dontinstrument wontreturn static void
XnuThreadMain(void *pthread,           // rdi
              int tid,                 // rsi
              int (*func)(void *arg),  // rdx
              void *arg,               // rcx
              struct CloneArgs *wt,    // r8
              unsigned xnuflags) {     // r9
  atomic_init(wt->ctid, tid);
  atomic_init(wt->ptid, tid);

  // XNU uses the same 0x30 offset as the WIN32 TIB x64. They told the
  // Go team at Google that they Apply stands by our ability to use it
  // https://github.com/golang/go/issues/23617#issuecomment-376662373
  int ax;
  asm volatile("syscall"
               : "=a"(ax)
               : "0"(__NR_thread_fast_set_cthread_self), "D"(wt->tls - 0x30)
               : "rcx", "rdx", "r8", "r9", "r10", "r11", "memory", "cc");

  func(arg);

  // we no longer use the stack after this point
  // %rax = int bsdthread_terminate(%rdi = void *stackaddr,
  //                                %rsi = size_t freesize,
  //                                %rdx = uint32_t port,
  //                                %r10 = uint32_t sem);
  asm volatile("movl\t$0,(%%rsi)\n\t"        // *wt->ctid = 0
               "mov\t$0x101,%%edi\n\t"       // wake all
               "xor\t%%edx,%%edx\n\t"        // wake_value
               "mov\t$0x02000204,%%eax\n\t"  // ulock_wake()
               "syscall\n\t"                 //
               "xor\t%%edi,%%edi\n\t"        // freeaddr
               "xor\t%%esi,%%esi\n\t"        // freesize
               "xor\t%%edx,%%edx\n\t"        // kport
               "xor\t%%r10d,%%r10d\n\t"      // joinsem
               "mov\t$0x02000169,%%eax\n\t"  // bsdthread_terminate()
               "syscall"
               : /* no outputs */
               : "S"(wt->ctid)
               : "rax", "rcx", "r10", "r11", "memory");
  __builtin_unreachable();
}

static errno_t CloneXnu(int (*fn)(void *), char *stk, size_t stksz, void *arg,
                        void *tls, atomic_int *ptid, atomic_int *ctid) {

  // perform this weird mandatory system call once
  static bool once;
  if (!once) {
    sys_bsdthread_register(XnuThreadThunk, 0, 0, 0, 0, 0, 0);
    once = true;
  }

  // setup stack for thread
  long sp;
  struct CloneArgs *wt;
  sp = (intptr_t)stk + stksz;
  sp -= sizeof(struct CloneArgs);
  sp &= -alignof(struct CloneArgs);
  wt = (struct CloneArgs *)sp;
  sp &= -16;

  // pass parameters to new thread via xnu
  wt->ctid = ctid;
  wt->ptid = ptid;
  wt->tls = tls;
  return sys_clone_xnu(fn, arg, wt, 0, PTHREAD_START_CUSTOM_XNU);
}

////////////////////////////////////////////////////////////////////////////////
// OPEN BESIYATA DISHMAYA

// we can't use address sanitizer because:
//   1. __asan_handle_no_return wipes stack [todo?]
relegated dontinstrument wontreturn static void OpenbsdThreadMain(void *p) {
  struct CloneArgs *wt = p;
  int tid = atomic_load_explicit(wt->ctid, memory_order_relaxed);
  atomic_init(wt->ptid, tid);
  wt->func(wt->arg);
  asm volatile("mov\t%1,%%rsp\n\t"     // so syscall can validate stack exists
               "movl\t$0,(%2)\n\t"     // *wt->ctid = 0 (old stack now free'd)
               "syscall\n\t"           // futex(int*, op, val) will wake wait0
               "xor\t%%edi,%%edi\n\t"  // so kernel doesn't write to old stack
               "mov\t$302,%%eax\n\t"   // __threxit(int *notdead) doesn't wake
               "syscall"
               : /* no outputs */
               : "a"(83), "m"(__oldstack), "D"(wt->ctid),
                 "S"(2 /* FUTEX_WAKE */), "d"(INT_MAX)
               : "rcx", "r11", "memory");
  __builtin_unreachable();
}

relegated static errno_t CloneOpenbsd(int (*func)(void *), char *stk,
                                      size_t stksz, void *arg, void *tls,
                                      atomic_int *ptid, atomic_int *ctid) {
  int rc;
  intptr_t sp;
  struct __tfork *tf;
  struct CloneArgs *wt;
  sp = (intptr_t)stk + stksz;
  sp -= sizeof(struct __tfork);
  sp &= -alignof(struct __tfork);
  tf = (struct __tfork *)sp;
  sp -= sizeof(struct CloneArgs);
  sp &= -alignof(struct CloneArgs);
  wt = (struct CloneArgs *)sp;
  sp &= -16;
  sp -= 8;
  *(intptr_t *)sp = (intptr_t)CloneOpenbsd + 1;
  wt->ctid = ctid;
  wt->ptid = ptid;
  wt->arg = arg;
  wt->func = func;
  tf->tf_stack = (char *)sp;
  tf->tf_tcb = tls;
  tf->tf_tid = ctid;
  if ((rc = __tfork_thread(tf, sizeof(*tf), OpenbsdThreadMain, wt)) >= 0) {
    atomic_init(ptid, rc);
    return 0;
  } else {
    return -rc;
  }
}

////////////////////////////////////////////////////////////////////////////////
// NET BESIYATA DISHMAYA

wontreturn dontinstrument static void NetbsdThreadMain(
    void *arg,            // rdi
    int (*func)(void *),  // rsi
    atomic_int *ctid,     // rdx
    atomic_int *ptid) {   // rcx
  int ax;
  asm("syscall"
      : "=a"(ax)  // man says always succeeds
      : "0"(311)  // _lwp_self()
      : "rcx", "rdx", "r8", "r9", "r10", "r11", "memory", "cc");
  atomic_init(ctid, ax);
  atomic_init(ptid, ax);
  func(arg);
  // we no longer use the stack after this point
  // %eax = int __lwp_exit(void);
  asm volatile("movl\t$0,(%2)\n\t"  // *ztid = 0
               "syscall"            // __lwp_exit()
               : "=a"(ax)
               : "0"(310), "r"(ctid)
               : "rcx", "r11", "memory");
  __builtin_unreachable();
}

static int CloneNetbsd(int (*func)(void *), char *stk, size_t stksz, void *arg,
                       void *tls, atomic_int *ptid, atomic_int *ctid) {
  // NetBSD has its own clone() and it works, but it's technically a
  // second-class API, intended to help Linux folks migrate to this.
  int ax;
  bool failed;
  intptr_t dx, sp;
  static bool once;
  struct ucontext_netbsd *ctx;
  static struct ucontext_netbsd netbsd_clone_template;

  // memoize arbitrary valid processor state structure
  if (!once) {
    asm volatile(CFLAG_ASM("syscall")
                 : CFLAG_CONSTRAINT(failed), "=a"(ax)
                 : "1"(__NR_getcontext_netbsd), "D"(&netbsd_clone_template)
                 : "rcx", "rdx", "r8", "r9", "r10", "r11", "memory");
    once = true;
  }
  sp = (intptr_t)stk + stksz;

  // align the stack
  sp &= -16;

  // simulate call to misalign stack and ensure backtrace looks good
  sp -= 8;
  *(intptr_t *)sp = (intptr_t)CloneNetbsd + 1;

  // place the giant 784 byte ucontext structure in the red zone!
  // it only has to live long enough for the thread to come alive
  ctx = (struct ucontext_netbsd *)((sp - sizeof(struct ucontext_netbsd)) & -64);

  // pass parameters in process state
  memcpy(ctx, &netbsd_clone_template, sizeof(*ctx));
  ctx->uc_link = 0;
  ctx->uc_mcontext.rbp = 0;
  ctx->uc_mcontext.rsp = sp;
  ctx->uc_mcontext.rip = (intptr_t)NetbsdThreadMain;
  ctx->uc_mcontext.rdi = (intptr_t)arg;
  ctx->uc_mcontext.rsi = (intptr_t)func;
  ctx->uc_mcontext.rdx = (intptr_t)ctid;
  ctx->uc_mcontext.rcx = (intptr_t)ptid;
  ctx->uc_flags |= _UC_STACK;
  ctx->uc_stack.ss_sp = stk;
  ctx->uc_stack.ss_size = stksz;
  ctx->uc_stack.ss_flags = 0;
  ctx->uc_flags |= _UC_TLSBASE;
  ctx->uc_mcontext._mc_tlsbase = (intptr_t)tls;

  // perform the system call
  int tid = 0;
  asm volatile(CFLAG_ASM("syscall")
               : CFLAG_CONSTRAINT(failed), "=a"(ax), "=d"(dx)
               : "1"(__NR__lwp_create), "D"(ctx), "S"(LWP_DETACHED), "2"(&tid)
               : "rcx", "r8", "r9", "r10", "r11", "memory");
  if (!failed) {
    atomic_init(ptid, tid);
    return 0;
  } else {
    return ax;
  }
}

#endif /* __x86_64__ */

////////////////////////////////////////////////////////////////////////////////
// FREE BESIYATA DISHMAYA

wontreturn dontinstrument static void FreebsdThreadMain(void *p) {
  struct CloneArgs *wt = p;
#ifdef __aarch64__
  asm volatile("mov\tx28,%0" : /* no outputs */ : "r"(wt->tls));
#elif defined(__x86_64__)
  sys_set_tls(AMD64_SET_GSBASE, wt->tls);
#endif
  atomic_init(wt->ctid, wt->tid64);
  atomic_init(wt->ptid, wt->tid64);
  wt->func(wt->arg);
  // we no longer use the stack after this point
  // void thr_exit(%rdi = long *state);
#ifdef __x86_64__
  asm volatile("movl\t$0,%0\n\t"       // *wt->ctid = 0
               "syscall\n\t"           // _umtx_op(wt->ctid, WAKE, INT_MAX)
               "movl\t$431,%%eax\n\t"  // thr_exit(long *nonzeroes_and_wake)
               "xor\t%%edi,%%edi\n\t"  // sad we can't use this free futex op
               "syscall\n\t"           // thr_exit() fails if thread is orphaned
               "movl\t$1,%%eax\n\t"    // _exit()
               "syscall"               //
               : "=m"(*wt->ctid)
               : "a"(454), "D"(wt->ctid), "S"(UMTX_OP_WAKE), "d"(INT_MAX)
               : "rcx", "r8", "r9", "r10", "r11", "memory");
#elif defined(__aarch64__)
  register long x0 asm("x0") = (long)wt->ctid;
  register long x1 asm("x1") = UMTX_OP_WAKE;
  register long x2 asm("x2") = INT_MAX;
  register long x8 asm("x8") = 454;  // _umtx_op
  asm volatile("str\twzr,%0\n\t"     // *wt->ctid = 0
               "svc\t0\n\t"          // _umtx_op(wt->ctid, WAKE, INT_MAX)
               "mov\tx0,#0\n\t"      // arg0 = 0
               "mov\tx8,#431\n\t"    // thr_exit
               "svc\t0\n\t"          // thr_exit(long *nonzeroes_and_wake = 0)
               "mov\tx8,#1\n\t"      // _exit
               "svc\t0"              // _exit(long *nonzeroes_and_wake = 0)
               : "=m"(*wt->ctid)
               : "r"(x0), "r"(x1), "r"(x2), "r"(x8));
#else
#error "unsupported architecture"
#endif
  __builtin_unreachable();
}

static errno_t CloneFreebsd(int (*func)(void *), char *stk, size_t stksz,
                            void *arg, void *tls, atomic_int *ptid,
                            atomic_int *ctid) {
  long sp;
  int64_t tid64;
  struct CloneArgs *wt;
  sp = (intptr_t)stk + stksz;
  sp -= sizeof(struct CloneArgs);
  sp &= -alignof(struct CloneArgs);
  wt = (struct CloneArgs *)sp;
  sp &= -16;
  wt->ctid = ctid;
  wt->ptid = ptid;
  wt->tls = tls;
  wt->func = func;
  wt->arg = arg;
  struct thr_param params = {
      .start_func = FreebsdThreadMain,
      .arg = wt,
      .stack_base = stk,
      .stack_size = sp - (long)stk,
      .tls_base = tls,
      .tls_size = 64,
      .child_tid = &wt->tid64,
      .parent_tid = &tid64,
  };
#ifdef __x86_64__
  int ax;
  bool failed;
  asm volatile(CFLAG_ASM("syscall")
               : CFLAG_CONSTRAINT(failed), "=a"(ax)
               : "1"(__NR_thr_new), "D"(&params), "S"(sizeof(params))
               : "rcx", "rdx", "r8", "r9", "r10", "r11", "memory");
  if (failed)
    return ax;
#elif defined(__aarch64__)
  register long x0 asm("x0") = (long)&params;
  register long x1 asm("x1") = sizeof(params);
  register int x8 asm("x8") = 0x1c7;  // thr_new
  asm volatile("svc\t0" : "+r"(x0) : "r"(x1), "r"(x8) : "memory");
  if (x0)
    return x0;
#else
#error "unsupported architecture"
#endif
  atomic_init(ptid, tid64);
  return 0;
}

#ifdef __aarch64__

////////////////////////////////////////////////////////////////////////////////
// APPLE SILICON

dontinstrument static void *SiliconThreadMain(void *arg) {
  struct CloneArgs *wt = arg;
  atomic_int *ctid = wt->ctid;
  int tid = atomic_load_explicit(ctid, memory_order_relaxed);
  asm volatile("mov\tx28,%0" : /* no outputs */ : "r"(wt->tls));
  __stack_call(wt->arg, tid, 0, 0, wt->func, wt->sp);
  atomic_store_explicit(ctid, 0, memory_order_release);
  ulock_wake(UL_COMPARE_AND_WAIT | ULF_WAKE_ALL, ctid, 0);
  return 0;
}

static errno_t CloneSilicon(int (*fn)(void *), char *stk, size_t stksz,
                            void *arg, void *tls, atomic_int *ptid,
                            atomic_int *ctid) {

  // assign tid to new thread
  static atomic_uint tids;
  unsigned tid = atomic_fetch_add_explicit(&tids, 1, memory_order_relaxed);
  tid %= kMaxThreadIds;
  tid += kMinThreadId;
  atomic_init(ctid, tid);
  atomic_init(ptid, tid);

  // pass temp data on stack
  intptr_t sp, tip;
  struct CloneArgs *wt;
  sp = tip = (intptr_t)stk + stksz;
  sp -= sizeof(struct CloneArgs);
  sp &= -alignof(struct CloneArgs);
  wt = (struct CloneArgs *)sp;
  wt->func = fn;
  wt->arg = arg;
  wt->tls = tls;
  wt->ctid = ctid;
  wt->sp = tip & -16;

  // ask apple libc to spawn thread
  errno_t res;
  pthread_t th;
  size_t babystack = __syslib->__pthread_stack_min;
#pragma GCC push_options
#pragma GCC diagnostic ignored "-Walloca-larger-than="
  void *attr = alloca(__syslib->__sizeof_pthread_attr_t);
#pragma GCC pop_options
  __syslib->__pthread_attr_init(attr);
  __syslib->__pthread_attr_setguardsize(attr, 0);
  __syslib->__pthread_attr_setstacksize(attr, babystack);
  if (!(res = __syslib->__pthread_create(&th, attr, SiliconThreadMain, wt))) {
    atomic_init(ptid, tid);
    struct CosmoTib *tib = tls;
    atomic_store_explicit(&tib[-1].tib_syshand, th, memory_order_release);
  }
  __syslib->__pthread_attr_destroy(attr);
  return res;
}

#endif /* __aarch64__ */

////////////////////////////////////////////////////////////////////////////////
// GNU/SYSTEMD

struct LinuxCloneArgs {
  int (*func)(void *);
  void *arg;
  char *tls;
};

int sys_clone_linux(int flags,         // rdi
                    long sp,           // rsi
                    atomic_int *ptid,  // rdx
                    atomic_int *ctid,  // rcx
                    void *tls,         // r8
                    void *func,        // r9
                    void *arg);        // 8(rsp)

dontinstrument static int AmdLinuxThreadEntry(void *arg) {
  struct LinuxCloneArgs *wt = arg;
#if defined(__x86_64__)
  sys_set_tls(ARCH_SET_GS, wt->tls);
#endif
  return wt->func(wt->arg);
}

static int CloneLinux(int (*func)(void *), char *stk, size_t stksz, int flags,
                      void *arg, void *tls, atomic_int *ptid,
                      atomic_int *ctid) {
  long sp = (intptr_t)stk + stksz;

#if defined(__x86_64__)
  sp -= sizeof(struct LinuxCloneArgs);
  sp &= -alignof(struct LinuxCloneArgs);
  struct LinuxCloneArgs *wt = (struct LinuxCloneArgs *)sp;
  sp &= -16;  // align the stack
  wt->arg = arg;
  wt->tls = tls;
  wt->func = func;
  func = AmdLinuxThreadEntry;
  arg = wt;
#elif defined(__aarch64__)
  sp &= -128;  // for kernels <=4.6
#endif

  int rc;
  if ((rc = sys_clone_linux(flags, sp, ptid, ctid, tls, func, arg)) >= 0) {
    // clone() is documented as setting ptid before return
    return 0;
  } else {
    return -rc;
  }
}

////////////////////////////////////////////////////////////////////////////////
// COSMOPOLITAN

/**
 * Creates thread without malloc() being linked.
 *
 * If you use clone() you're on your own.
 */
errno_t clone(void *func, void *stk, size_t stksz, int flags, void *arg,
              void *ptid, void *tls, void *ctid) {
  errno_t err;

  atomic_fetch_add(&_pthread_count, 1);

  if (IsLinux()) {
    err = CloneLinux(func, stk, stksz, flags, arg, tls, ptid, ctid);
  } else if (IsXnu()) {
#if defined(__x86_64__)
    err = CloneXnu(func, stk, stksz, arg, tls, ptid, ctid);
#elif defined(__aarch64__)
    err = CloneSilicon(func, stk, stksz, arg, tls, ptid, ctid);
#else
#error "unsupported architecture"
#endif
  } else if (IsFreebsd()) {
    err = CloneFreebsd(func, stk, stksz, arg, tls, ptid, ctid);
#if defined(__x86_64__)
  } else if (IsWindows()) {
    err = CloneWindows(func, stk, stksz, arg, tls, ptid, ctid);
  } else if (IsNetbsd()) {
    err = CloneNetbsd(func, stk, stksz, arg, tls, ptid, ctid);
  } else if (IsOpenbsd()) {
    err = CloneOpenbsd(func, stk, stksz, arg, tls, ptid, ctid);
#endif /* __x86_64__ */
  } else {
    err = ENOSYS;
  }

  if (SupportsBsd() && err == EPROCLIM)
    err = EAGAIN;

  if (err)
    atomic_fetch_sub(&_pthread_count, 1);

  return err;
}
