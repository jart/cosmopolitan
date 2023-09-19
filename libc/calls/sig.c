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
#include "libc/sysv/consts/sig.h"
#include "ape/sections.internal.h"
#include "libc/atomic.h"
#include "libc/calls/blockcancel.internal.h"
#include "libc/calls/blocksigs.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/ucontext.internal.h"
#include "libc/calls/ucontext.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/describebacktrace.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/popcnt.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/context.h"
#include "libc/nt/enum/exceptionhandleractions.h"
#include "libc/nt/enum/signal.h"
#include "libc/nt/enum/status.h"
#include "libc/nt/errors.h"
#include "libc/nt/runtime.h"
#include "libc/nt/signals.h"
#include "libc/nt/struct/context.h"
#include "libc/nt/struct/ntexceptionpointers.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/ss.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/tls.h"

#ifdef __x86_64__

struct SignalFrame {
  struct PosixThread *pt;
  struct NtContext *nc;
  unsigned rva;
  unsigned flags;
  siginfo_t si;
};

struct ContextFrame {
  struct SignalFrame sf;
  struct NtContext nc;
};

void __stack_call(int, siginfo_t *, void *, long,
                  void (*)(int, siginfo_t *, void *), void *);

static textwindows bool __sig_ignored_by_default(int sig) {
  return sig == SIGURG ||   //
         sig == SIGCONT ||  //
         sig == SIGCHLD ||  //
         sig == SIGWINCH;
}

textwindows bool __sig_ignored(int sig) {
  return __sighandrvas[sig] == (intptr_t)SIG_IGN ||
         (__sighandrvas[sig] == (intptr_t)SIG_DFL &&
          __sig_ignored_by_default(sig));
}

static textwindows bool __sig_should_use_altstack(unsigned flags,
                                                  struct CosmoTib *tib) {
  return (flags & SA_ONSTACK) &&    //
         tib->tib_sigstack_size &&  //
         !(tib->tib_sigstack_flags & (SS_DISABLE | SS_ONSTACK));
}

static textwindows wontreturn void __sig_terminate(int sig) {
  TerminateThisProcess(sig);
}

static textwindows bool __sig_start(struct PosixThread *pt, int sig,
                                    unsigned *rva, unsigned *flags) {
  *rva = __sighandrvas[sig];
  *flags = __sighandflags[sig];
  if (*rva == (intptr_t)SIG_IGN ||
      (*rva == (intptr_t)SIG_DFL && __sig_ignored_by_default(sig))) {
    STRACE("ignoring %G", sig);
    return false;
  }
  if (pt->tib->tib_sigmask & (1ull << (sig - 1))) {
    STRACE("tid %d masked %G delivering to tib_sigpending", _pthread_tid(pt),
           sig);
    pt->tib->tib_sigpending |= 1ull << (sig - 1);
    return false;
  }
  if (*rva == (intptr_t)SIG_DFL) {
    STRACE("terminating on %G due to no handler", sig);
    __sig_terminate(sig);
  }
  if (*flags & SA_RESETHAND) {
    STRACE("resetting %G handler", sig);
    __sighandrvas[sig] = (int32_t)(intptr_t)SIG_DFL;
  }
  return true;
}

static textwindows void __sig_call(int sig, siginfo_t *si, ucontext_t *ctx,
                                   unsigned rva, unsigned flags,
                                   struct CosmoTib *tib) {
  sigaction_f handler;
  handler = (sigaction_f)(__executable_start + rva);
  ++__sig.count;
  if (__sig_should_use_altstack(flags, tib)) {
    tib->tib_sigstack_flags |= SS_ONSTACK;
    __stack_call(sig, si, ctx, 0, handler,
                 tib->tib_sigstack_addr + tib->tib_sigstack_size);
    tib->tib_sigstack_flags &= ~SS_ONSTACK;
  } else {
    handler(sig, si, ctx);
  }
}

textwindows int __sig_raise(int sig, int sic) {
  unsigned rva, flags;
  struct CosmoTib *tib = __get_tls();
  struct PosixThread *pt = (struct PosixThread *)tib->tib_pthread;
  ucontext_t ctx = {.uc_sigmask.__bits[0] = tib->tib_sigmask};
  if (!__sig_start(pt, sig, &rva, &flags)) return 0;
  siginfo_t si = {.si_signo = sig, .si_code = sic};
  struct NtContext nc;
  nc.ContextFlags = kNtContextAll;
  GetThreadContext(GetCurrentThread(), &nc);
  _ntcontext2linux(&ctx, &nc);
  STRACE("raising %G", sig);
  if (!(flags & SA_NODEFER)) {
    tib->tib_sigmask |= 1ull << (sig - 1);
  }
  __sig_call(sig, &si, &ctx, rva, flags, tib);
  tib->tib_sigmask = ctx.uc_sigmask.__bits[0];
  return (flags & SA_RESTART) ? 2 : 1;
}

textwindows void __sig_cancel(struct PosixThread *pt, unsigned flags) {
  atomic_int *futex;
  if (_weaken(WakeByAddressSingle) &&
      (futex = atomic_load_explicit(&pt->pt_futex, memory_order_acquire))) {
    _weaken(WakeByAddressSingle)(futex);
  } else if (!(flags & SA_RESTART) && pt->iohandle > 0) {
    pt->abort_errno = EINTR;
    if (!CancelIoEx(pt->iohandle, pt->ioverlap)) {
      int err = GetLastError();
      if (err != kNtErrorNotFound) {
        STRACE("CancelIoEx() failed w/ %d", err);
      }
    }
  } else if (pt->pt_flags & PT_INSEMAPHORE) {
    pt->abort_errno = EINTR;
    if (!ReleaseSemaphore(pt->semaphore, 1, 0)) {
      STRACE("ReleaseSemaphore() failed w/ %d", GetLastError());
    }
  }
}

static textwindows wontreturn void __sig_panic(const char *msg) {
#ifndef TINY
  char s[128], *p = s;
  p = stpcpy(p, "sig panic: ");
  p = stpcpy(p, msg);
  p = stpcpy(p, " failed w/ ");
  p = FormatInt32(p, GetLastError());
  *p++ = '\n';
  WriteFile(GetStdHandle(kNtStdErrorHandle), s, p - s, 0, 0);
#endif
  TerminateThisProcess(SIGVTALRM);
}

static textwindows wontreturn void __sig_tramp(struct SignalFrame *sf) {
  ucontext_t ctx = {0};
  sigaction_f handler = (sigaction_f)(__executable_start + sf->rva);
  STRACE("__sig_tramp(%G, %t)", sf->si.si_signo, handler);
  _ntcontext2linux(&ctx, sf->nc);
  ctx.uc_sigmask.__bits[0] = sf->pt->tib->tib_sigmask;
  if (!(sf->flags & SA_NODEFER)) {
    sf->pt->tib->tib_sigmask |= 1ull << (sf->si.si_signo - 1);
  }
  ++__sig.count;
  handler(sf->si.si_signo, &sf->si, &ctx);
  sf->pt->tib->tib_sigmask = ctx.uc_sigmask.__bits[0];
  _ntlinux2context(sf->nc, &ctx);
  SetThreadContext(GetCurrentThread(), sf->nc);
  __sig_panic("SetThreadContext(GetCurrentThread)");
}

static textwindows int __sig_killer(struct PosixThread *pt, int sig, int sic) {
  uintptr_t th;
  unsigned rva, flags;
  if (!__sig_start(pt, sig, &rva, &flags)) return 0;
  th = _pthread_syshand(pt);
  uint32_t old_suspend_count;
  old_suspend_count = SuspendThread(th);
  if (old_suspend_count == -1u) {
    STRACE("SuspendThread failed w/ %d", GetLastError());
    return ESRCH;
  }
  if (old_suspend_count) {
    STRACE("kill contention of %u on tid %d", old_suspend_count,
           _pthread_tid(pt));
    pt->tib->tib_sigpending |= 1ull << (sig - 1);
    return 0;
  }
  struct NtContext nc;
  nc.ContextFlags = kNtContextAll;
  if (!GetThreadContext(th, &nc)) {
    STRACE("GetThreadContext failed w/ %d", GetLastError());
    return ESRCH;
  }
  uintptr_t sp;
  if (__sig_should_use_altstack(flags, pt->tib)) {
    pt->tib->tib_sigstack_flags |= SS_ONSTACK;
    sp = (uintptr_t)pt->tib->tib_sigstack_addr + pt->tib->tib_sigstack_size;
    pt->tib->tib_sigstack_flags &= ~SS_ONSTACK;
  } else {
    sp = (nc.Rsp - 128 - sizeof(struct ContextFrame)) & -16;
  }
  struct ContextFrame *cf = (struct ContextFrame *)sp;
  bzero(&cf->sf.si, sizeof(cf->sf.si));
  memcpy(&cf->nc, &nc, sizeof(nc));
  cf->sf.pt = pt;
  cf->sf.rva = rva;
  cf->sf.nc = &cf->nc;
  cf->sf.flags = flags;
  cf->sf.si.si_code = sic;
  cf->sf.si.si_signo = sig;
  *(uintptr_t *)(sp -= sizeof(uintptr_t)) = nc.Rip;
  nc.Rip = (intptr_t)__sig_tramp;
  nc.Rdi = (intptr_t)&cf->sf;
  nc.Rsp = sp;
  if (!SetThreadContext(th, &nc)) {
    STRACE("SetThreadContext failed w/ %d", GetLastError());
    return ESRCH;
  }
  ResumeThread(th);         // doesn't actually resume if doing blocking i/o
  __sig_cancel(pt, flags);  // we can wake it up immediately by canceling it
  return 0;
}

textwindows int __sig_kill(struct PosixThread *pt, int sig, int sic) {
  int rc;
  BLOCK_SIGNALS;
  BLOCK_CANCELLATIONS;
  rc = __sig_killer(pt, sig, sic);
  ALLOW_CANCELLATIONS;
  ALLOW_SIGNALS;
  return rc;
}

textwindows void __sig_generate(int sig, int sic) {
  struct Dll *e;
  struct PosixThread *pt, *mark = 0;
  if (__sig_ignored(sig)) {
    STRACE("ignoring %G", sig);
    return;
  }
  if (__sighandrvas[sig] == (intptr_t)SIG_DFL) {
    STRACE("terminating on %G due to no handler", sig);
    __sig_terminate(sig);
  }
  pthread_spin_lock(&_pthread_lock);
  for (e = dll_first(_pthread_list); e; e = dll_next(_pthread_list, e)) {
    pt = POSIXTHREAD_CONTAINER(e);
    if (atomic_load_explicit(&pt->status, memory_order_acquire) <
            kPosixThreadTerminated &&
        !(pt->tib->tib_sigmask & (1ull << (sig - 1)))) {
      mark = pt;
      break;
    }
  }
  pthread_spin_unlock(&_pthread_lock);
  if (mark) {
    __sig_kill(mark, sig, sic);
  } else {
    STRACE("all threads block %G so adding to pending signals of process", sig);
    __sig.pending |= sig;
  }
}

static int __sig_crash_sig(struct NtExceptionPointers *ep, int *code) {
  switch (ep->ExceptionRecord->ExceptionCode) {
    case kNtSignalBreakpoint:
      *code = TRAP_BRKPT;
      return SIGTRAP;
    case kNtSignalIllegalInstruction:
      *code = ILL_ILLOPC;
      return SIGILL;
    case kNtSignalPrivInstruction:
      *code = ILL_PRVOPC;
      return SIGILL;
    case kNtSignalGuardPage:
    case kNtSignalInPageError:
    case kNtStatusStackOverflow:
      *code = SEGV_MAPERR;
      return SIGSEGV;
    case kNtSignalAccessViolation:
      *code = SEGV_ACCERR;
      return SIGSEGV;
    case kNtSignalInvalidHandle:
    case kNtSignalInvalidParameter:
    case kNtSignalAssertionFailure:
      *code = SI_USER;
      return SIGABRT;
    case kNtStatusIntegerOverflow:
      *code = FPE_INTOVF;
      return SIGFPE;
    case kNtSignalFltDivideByZero:
      *code = FPE_FLTDIV;
      return SIGFPE;
    case kNtSignalFltOverflow:
      *code = FPE_FLTOVF;
      return SIGFPE;
    case kNtSignalFltUnderflow:
      *code = FPE_FLTUND;
      return SIGFPE;
    case kNtSignalFltInexactResult:
      *code = FPE_FLTRES;
      return SIGFPE;
    case kNtSignalFltDenormalOperand:
    case kNtSignalFltInvalidOperation:
    case kNtSignalFltStackCheck:
    case kNtSignalIntegerDivideByZero:
    case kNtSignalFloatMultipleFaults:
    case kNtSignalFloatMultipleTraps:
      *code = FPE_FLTINV;
      return SIGFPE;
    case kNtSignalDllNotFound:
    case kNtSignalOrdinalNotFound:
    case kNtSignalEntrypointNotFound:
    case kNtSignalDllInitFailed:
      *code = SI_KERNEL;
      return SIGSYS;
    default:
      *code = ep->ExceptionRecord->ExceptionCode;
      return SIGSEGV;
  }
}

// abashed the devil stood, and felt how awful goodness is
__msabi unsigned __sig_crash(struct NtExceptionPointers *ep) {
  int code, sig = __sig_crash_sig(ep, &code);
  STRACE("win32 vectored exception 0x%08Xu raising %G "
         "cosmoaddr2line %s %lx %s",
         ep->ExceptionRecord->ExceptionCode, sig, program_invocation_name,
         ep->ContextRecord->Rip,
         DescribeBacktrace((struct StackFrame *)ep->ContextRecord->Rbp));
  if (sig == SIGTRAP) {
    ep->ContextRecord->Rip++;
    if (__sig_ignored(sig)) {
      return kNtExceptionContinueExecution;
    }
  }
  struct PosixThread *pt = _pthread_self();
  siginfo_t si = {.si_signo = sig,
                  .si_code = code,
                  .si_addr = ep->ExceptionRecord->ExceptionAddress};
  unsigned rva = __sighandrvas[sig];
  unsigned flags = __sighandflags[sig];
  if (rva == (intptr_t)SIG_DFL || rva == (intptr_t)SIG_IGN) {
#ifndef TINY
    intptr_t hStderr;
    char sigbuf[21], s[128], *p;
    hStderr = GetStdHandle(kNtStdErrorHandle);
    p = stpcpy(s, "Terminating on uncaught ");
    p = stpcpy(p, strsignal_r(sig, sigbuf));
    p = stpcpy(p, ". Pass --strace and/or ShowCrashReports() for details.\n");
    WriteFile(hStderr, s, p - s, 0, 0);
#endif
    __sig_terminate(sig);
  }
  if (flags & SA_RESETHAND) {
    STRACE("resetting %G handler", sig);
    __sighandrvas[sig] = (int32_t)(intptr_t)SIG_DFL;
  }
  ucontext_t ctx = {0};
  _ntcontext2linux(&ctx, ep->ContextRecord);
  ctx.uc_sigmask.__bits[0] = pt->tib->tib_sigmask;
  __sig_call(sig, &si, &ctx, rva, flags, pt->tib);
  pt->tib->tib_sigmask = ctx.uc_sigmask.__bits[0];
  _ntlinux2context(ep->ContextRecord, &ctx);
  return kNtExceptionContinueExecution;
}

static textwindows int __sig_console_sig(uint32_t dwCtrlType) {
  switch (dwCtrlType) {
    case kNtCtrlCEvent:
      return SIGINT;
    case kNtCtrlBreakEvent:
      return SIGQUIT;
    case kNtCtrlCloseEvent:
    case kNtCtrlLogoffEvent:    // only received by services
    case kNtCtrlShutdownEvent:  // only received by services
      return SIGHUP;
    default:
      return SIGSTKFLT;
  }
}

__msabi textwindows dontinstrument bool32 __sig_console(uint32_t dwCtrlType) {
  struct CosmoTib tls;
  __bootstrap_tls(&tls, __builtin_frame_address(0));
  __sig_generate(__sig_console_sig(dwCtrlType), SI_KERNEL);
  return true;
}

static textwindows int __sig_checkem(atomic_ulong *sigs, struct CosmoTib *tib,
                                     const char *thing, int id) {
  bool handler_was_called = false;
  uint64_t pending, masked, deliverable;
  pending = atomic_load_explicit(sigs, memory_order_acquire);
  masked = atomic_load_explicit(&tib->tib_sigmask, memory_order_acquire);
  deliverable = pending & ~masked;
  POLLTRACE("%s %d blocks %d sigs w/ %d pending and %d deliverable", thing, id,
            popcnt(masked), popcnt(pending), popcnt(deliverable));
  if (deliverable) {
    for (int sig = 1; sig <= 64; ++sig) {
      if ((deliverable & (1ull << (sig - 1))) &&
          atomic_fetch_and(sigs, ~(1ull << (sig - 1))) & (1ull << (sig - 1))) {
        handler_was_called |= __sig_raise(sig, SI_KERNEL);
      }
    }
  }
  return handler_was_called;
}

// returns 0 if no signal handlers were called, otherwise a bitmask
// consisting of `1` which means a signal handler was invoked which
// didn't have the SA_RESTART flag, and `2`, which means SA_RESTART
// handlers were called (or `3` if both were the case).
textwindows int __sig_check(void) {
  bool handler_was_called = false;
  struct CosmoTib *tib = __get_tls();
  handler_was_called |=
      __sig_checkem(&tib->tib_sigpending, tib, "tid", tib->tib_tid);
  handler_was_called |= __sig_checkem(&__sig.pending, tib, "pid", getpid());
  POLLTRACE("__sig_check() → %hhhd", handler_was_called);
  return handler_was_called;
}

textstartup void __sig_init(void) {
  if (!IsWindows()) return;
  AddVectoredExceptionHandler(true, (void *)__sig_crash);
  SetConsoleCtrlHandler((void *)__sig_console, true);
}

const void *const __sig_ctor[] initarray = {__sig_init};

#endif /* __x86_64__ */
