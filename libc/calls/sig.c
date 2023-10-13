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
#include "libc/assert.h"
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/struct/ucontext.internal.h"
#include "libc/calls/ucontext.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/bsf.h"
#include "libc/intrin/bsr.h"
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
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/ss.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#ifdef __x86_64__

/**
 * @fileoverview Cosmopolitan Signals for Windows.
 */

struct SignalFrame {
  unsigned rva;
  unsigned flags;
  siginfo_t si;
  ucontext_t ctx;
};

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

textwindows void __sig_delete(int sig) {
  struct Dll *e;
  __sig.pending &= ~(1ull << (sig - 1));
  _pthread_lock();
  for (e = dll_last(_pthread_list); e; e = dll_prev(_pthread_list, e)) {
    POSIXTHREAD_CONTAINER(e)->tib->tib_sigpending &= ~(1ull << (sig - 1));
  }
  _pthread_unlock();
}

static textwindows bool __sig_should_use_altstack(unsigned flags,
                                                  struct CosmoTib *tib) {
  if (!(flags & SA_ONSTACK)) {
    return false;  // signal handler didn't enable it
  }
  if (!tib->tib_sigstack_size) {
    return false;  // sigaltstack() wasn't installed on this thread
  }
  if (tib->tib_sigstack_flags & SS_DISABLE) {
    return false;  // sigaltstack() on this thread was disabled by user
  }
  char *bp = __builtin_frame_address(0);
  if (tib->tib_sigstack_addr <= bp &&
      bp <= tib->tib_sigstack_addr + tib->tib_sigstack_size) {
    return false;  // we're already on the alternate stack
  }
  return true;
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
    STRACE("enqueing %G on %d", sig, _pthread_tid(pt));
    pt->tib->tib_sigpending |= 1ull << (sig - 1);
    return false;
  }
  if (*rva == (intptr_t)SIG_DFL) {
    STRACE("terminating on %G due to no handler", sig);
    __sig_terminate(sig);
  }
  return true;
}

static textwindows sigaction_f __sig_handler(unsigned rva) {
  return (sigaction_f)(__executable_start + rva);
}

textwindows int __sig_raise(int sig, int sic) {
  unsigned rva, flags;
  struct PosixThread *pt = _pthread_self();
  ucontext_t ctx = {.uc_sigmask = pt->tib->tib_sigmask};
  if (!__sig_start(pt, sig, &rva, &flags)) return 0;
  if (flags & SA_RESETHAND) {
    STRACE("resetting %G handler", sig);
    __sighandrvas[sig] = (int32_t)(intptr_t)SIG_DFL;
  }
  siginfo_t si = {.si_signo = sig, .si_code = sic};
  struct NtContext nc;
  nc.ContextFlags = kNtContextFull;
  GetThreadContext(GetCurrentThread(), &nc);
  _ntcontext2linux(&ctx, &nc);
  pt->tib->tib_sigmask |= __sighandmask[sig];
  if (!(flags & SA_NODEFER)) {
    pt->tib->tib_sigmask |= 1ull << (sig - 1);
  }
  NTTRACE("entering raise(%G) signal handler %t with mask %s → %s", sig,
          __sig_handler(rva), DescribeSigset(0, &ctx.uc_sigmask),
          DescribeSigset(0, (sigset_t *)&pt->tib->tib_sigmask));
  __sig_handler(rva)(sig, &si, &ctx);
  NTTRACE("leaving raise(%G) signal handler %t with mask %s → %s", sig,
          __sig_handler(rva),
          DescribeSigset(0, (sigset_t *)&pt->tib->tib_sigmask),
          DescribeSigset(0, &ctx.uc_sigmask));
  atomic_store_explicit(&pt->tib->tib_sigmask, ctx.uc_sigmask,
                        memory_order_release);
  return (flags & SA_RESTART) ? 2 : 1;
}

// cancels blocking operations being performed by signaled thread
textwindows void __sig_cancel(struct PosixThread *pt, int sig, unsigned flags) {
  bool should_restart;
  atomic_int *blocker;
  // cancelation points need to set pt_blocker before entering a wait op
  blocker = atomic_load_explicit(&pt->pt_blocker, memory_order_acquire);
  // cancelation points should set it back to this after blocking
  // however, code that longjmps might mess it up a tolerable bit
  if (blocker == PT_BLOCKER_CPU) {
    STRACE("%G delivered to %d asynchronously", sig, _pthread_tid(pt));
    return;
  }
  // most cancelation points can be safely restarted w/o raising eintr
  should_restart = (flags & SA_RESTART) && (pt->pt_flags & PT_RESTARTABLE);
  // we can cancel another thread's overlapped i/o op after the freeze
  if (blocker == PT_BLOCKER_IO) {
    if (should_restart) {
      STRACE("%G restarting %d's i/o op", sig, _pthread_tid(pt));
    } else {
      STRACE("%G interupting %d's i/o op", sig, _pthread_tid(pt));
      CancelIoEx(pt->pt_iohandle, pt->pt_ioverlap);
    }
    return;
  }
  // threads can create semaphores on an as-needed basis
  if (blocker == PT_BLOCKER_SEM) {
    if (should_restart) {
      STRACE("%G restarting %d's semaphore", sig, _pthread_tid(pt));
    } else {
      STRACE("%G releasing %d's semaphore", sig, _pthread_tid(pt));
      pt->pt_flags &= ~PT_RESTARTABLE;
      ReleaseSemaphore(pt->pt_semaphore, 1, 0);
    }
    return;
  }
  // all other blocking ops that aren't overlap should use futexes
  // we force restartable futexes to churn by waking w/o releasing
  STRACE("%G waking %d's futex", sig, _pthread_tid(pt));
  if (!should_restart) {
    atomic_store_explicit(blocker, 1, memory_order_release);
  }
  WakeByAddressSingle(blocker);
}

static textwindows wontreturn void __sig_tramp(struct SignalFrame *sf) {
  ++__sig.count;
  int sig = sf->si.si_signo;
  sigset_t blocksigs = __sighandmask[sig];
  if (!(sf->flags & SA_NODEFER)) blocksigs |= 1ull << (sig - 1);
  sf->ctx.uc_sigmask = atomic_fetch_or_explicit(
      &__get_tls()->tib_sigmask, blocksigs, memory_order_acq_rel);
  __sig_handler(sf->rva)(sig, &sf->si, &sf->ctx);
  atomic_store_explicit(&__get_tls()->tib_sigmask, sf->ctx.uc_sigmask,
                        memory_order_release);
  __sig_restore(&sf->ctx);
}

static int __sig_killer(struct PosixThread *pt, int sig, int sic) {

  // prepare for signal
  unsigned rva, flags;
  if (!__sig_start(pt, sig, &rva, &flags)) {
    return 0;
  }

  // take control of thread
  // suspending the thread happens asynchronously
  // however getting the context blocks until it's frozen
  static pthread_spinlock_t killer_lock;
  pthread_spin_lock(&killer_lock);
  uintptr_t th = _pthread_syshand(pt);
  if (SuspendThread(th) == -1u) {
    STRACE("SuspendThread failed w/ %d", GetLastError());
    pthread_spin_unlock(&killer_lock);
    return ESRCH;
  }
  struct NtContext nc;
  nc.ContextFlags = kNtContextFull;
  if (!GetThreadContext(th, &nc)) {
    STRACE("GetThreadContext failed w/ %d", GetLastError());
    ResumeThread(th);
    pthread_spin_unlock(&killer_lock);
    return ESRCH;
  }
  pthread_spin_unlock(&killer_lock);

  // we can't preempt threads that are running in win32 code
  if ((pt->tib->tib_sigmask & (1ull << (sig - 1))) ||
      !((uintptr_t)__executable_start <= nc.Rip &&
        nc.Rip < (uintptr_t)__privileged_start)) {
    STRACE("enqueing %G on %d", sig, _pthread_tid(pt));
    pt->tib->tib_sigpending |= 1ull << (sig - 1);
    ResumeThread(th);
    __sig_cancel(pt, sig, flags);
    return 0;
  }

  // we're committed to delivering this signal now
  if (flags & SA_RESETHAND) {
    STRACE("resetting %G handler", sig);
    __sighandrvas[sig] = (int32_t)(intptr_t)SIG_DFL;
  }

  // inject trampoline function call into thread
  uintptr_t sp;
  if (__sig_should_use_altstack(flags, pt->tib)) {
    sp = (uintptr_t)pt->tib->tib_sigstack_addr + pt->tib->tib_sigstack_size;
  } else {
    sp = nc.Rsp;
  }
  sp -= sizeof(struct SignalFrame);
  sp &= -16;
  struct SignalFrame *sf = (struct SignalFrame *)sp;
  _ntcontext2linux(&sf->ctx, &nc);
  bzero(&sf->si, sizeof(sf->si));
  sf->rva = rva;
  sf->flags = flags;
  sf->si.si_code = sic;
  sf->si.si_signo = sig;
  *(uintptr_t *)(sp -= sizeof(uintptr_t)) = nc.Rip;
  nc.Rip = (intptr_t)__sig_tramp;
  nc.Rdi = (intptr_t)sf;
  nc.Rsp = sp;
  if (!SetThreadContext(th, &nc)) {
    STRACE("SetThreadContext failed w/ %d", GetLastError());
    return ESRCH;
  }
  ResumeThread(th);
  __sig_cancel(pt, sig, flags);
  return 0;
}

textwindows int __sig_kill(struct PosixThread *pt, int sig, int sic) {
  int rc;
  BLOCK_SIGNALS;
  rc = __sig_killer(pt, sig, sic);
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
  BLOCK_SIGNALS;
  _pthread_lock();
  for (e = dll_first(_pthread_list); e; e = dll_next(_pthread_list, e)) {
    pt = POSIXTHREAD_CONTAINER(e);
    if (pt != _pthread_self() &&
        atomic_load_explicit(&pt->pt_status, memory_order_acquire) <
            kPosixThreadTerminated &&
        !(pt->tib->tib_sigmask & (1ull << (sig - 1)))) {
      mark = pt;
      break;
    }
  }
  _pthread_unlock();
  if (mark) {
    STRACE("generating %G by killing %d", sig, _pthread_tid(mark));
    __sig_killer(mark, sig, sic);
  } else {
    STRACE("all threads block %G so adding to pending signals of process", sig);
    __sig.pending |= 1ull << (sig - 1);
  }
  ALLOW_SIGNALS;
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
    case kNtSignalInPageError:
    case kNtStatusStackOverflow:
      *code = SEGV_MAPERR;
      return SIGSEGV;
    case kNtSignalGuardPage:
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

static void __sig_unmaskable(struct NtExceptionPointers *ep, int code, int sig,
                             struct CosmoTib *tib) {

  // increment the signal count for getrusage()
  ++__sig.count;

  // log vital crash information reliably for --strace before doing much
  // we don't print this without the flag since raw numbers scare people
  // this needs at least one page of stack memory in order to get logged
  // otherwise it'll print a warning message about the lack of stack mem
  STRACE("win32 vectored exception 0x%08Xu raising %G "
         "cosmoaddr2line %s %lx %s",
         ep->ExceptionRecord->ExceptionCode, sig, program_invocation_name,
         ep->ContextRecord->Rip,
         DescribeBacktrace((struct StackFrame *)ep->ContextRecord->Rbp));

  // if the user didn't install a signal handler for this unmaskable
  // exception, then print a friendly helpful hint message to stderr
  unsigned rva = __sighandrvas[sig];
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

  // if this signal handler is configured to auto-reset to the default
  // then that reset needs to happen before the user handler is called
  unsigned flags = __sighandflags[sig];
  if (flags & SA_RESETHAND) {
    STRACE("resetting %G handler", sig);
    __sighandrvas[sig] = (int32_t)(intptr_t)SIG_DFL;
  }

  // determine the true memory address at which fault occurred
  // if this is a stack overflow then reapply guard protection
  void *si_addr;
  if (ep->ExceptionRecord->ExceptionCode == kNtSignalGuardPage) {
    si_addr = (void *)ep->ExceptionRecord->ExceptionInformation[1];
  } else {
    si_addr = ep->ExceptionRecord->ExceptionAddress;
  }

  // call the user signal handler
  // with a temporarily replaced signal mask
  // and a modifiable view of the faulting code's cpu state
  // note ucontext_t is a hefty data structures on top of NtContext
  ucontext_t ctx = {0};
  siginfo_t si = {.si_signo = sig, .si_code = code, .si_addr = si_addr};
  _ntcontext2linux(&ctx, ep->ContextRecord);
  ctx.uc_sigmask = tib->tib_sigmask;
  tib->tib_sigmask |= __sighandmask[sig];
  if (!(flags & SA_NODEFER)) {
    tib->tib_sigmask |= 1ull << (sig - 1);
  }
  __sig_handler(rva)(sig, &si, &ctx);
  atomic_store_explicit(&tib->tib_sigmask, ctx.uc_sigmask,
                        memory_order_release);
  _ntlinux2context(ep->ContextRecord, &ctx);
}

void __stack_call(struct NtExceptionPointers *, int, int, struct CosmoTib *,
                  void (*)(struct NtExceptionPointers *, int, int,
                           struct CosmoTib *),
                  void *);

//                         abashed the devil stood
//                      and felt how awful goodness is
__msabi dontinstrument unsigned __sig_crash(struct NtExceptionPointers *ep) {

  // translate win32 to unix si_signo and si_code
  int code, sig = __sig_crash_sig(ep, &code);

  // advance the instruction pointer to skip over debugger breakpoints
  // this behavior is consistent with how unix kernels are implemented
  if (sig == SIGTRAP) {
    ep->ContextRecord->Rip++;
    if (__sig_ignored(sig)) {
      return kNtExceptionContinueExecution;
    }
  }

  // win32 stack overflow detection executes INSIDE the guard page
  // thus switch to the alternate signal stack as soon as possible
  struct CosmoTib *tib = __get_tls();
  unsigned flags = __sighandflags[sig];
  if (__sig_should_use_altstack(flags, tib)) {
    __stack_call(ep, code, sig, tib, __sig_unmaskable,
                 tib->tib_sigstack_addr + tib->tib_sigstack_size);
  } else {
    __sig_unmaskable(ep, code, sig, tib);
  }

  // resume running user program
  // hopefully the user fixed the cpu state
  // otherwise the crash will keep happening
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

static textwindows int __sig_checker(atomic_ulong *sigs, struct CosmoTib *tib) {
  int sig, handler_was_called = 0;
  sigset_t bit, pending, masked, deliverable;
  pending = atomic_load_explicit(sigs, memory_order_acquire);
  masked = atomic_load_explicit(&tib->tib_sigmask, memory_order_acquire);
  if ((deliverable = pending & ~masked)) {
    do {
      sig = _bsf(deliverable) + 1;
      bit = 1ull << (sig - 1);
      if (atomic_fetch_and_explicit(sigs, ~bit, memory_order_acq_rel) & bit) {
        STRACE("found pending %G we can raise now", sig);
        handler_was_called |= __sig_raise(sig, SI_KERNEL);
      }
    } while ((deliverable &= ~bit));
  }
  return handler_was_called;
}

// returns 0 if no signal handlers were called, otherwise a bitmask
// consisting of `1` which means a signal handler was invoked which
// didn't have the SA_RESTART flag, and `2`, which means SA_RESTART
// handlers were called (or `3` if both were the case).
textwindows int __sig_check(void) {
  int handler_was_called = false;
  struct CosmoTib *tib = __get_tls();
  handler_was_called |= __sig_checker(&tib->tib_sigpending, tib);
  handler_was_called |= __sig_checker(&__sig.pending, tib);
  return handler_was_called;
}

textstartup void __sig_init(void) {
  if (!IsWindows()) return;
  AddVectoredExceptionHandler(true, (void *)__sig_crash);
  SetConsoleCtrlHandler((void *)__sig_console, true);
}

const void *const __sig_ctor[] initarray = {__sig_init};

#endif /* __x86_64__ */
