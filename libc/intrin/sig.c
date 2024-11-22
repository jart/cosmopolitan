/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/calls.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/struct/ucontext.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/calls/ucontext.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/bsf.h"
#include "libc/intrin/describebacktrace.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/maps.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/weaken.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/context.h"
#include "libc/nt/enum/exceptionhandleractions.h"
#include "libc/nt/enum/processcreationflags.h"
#include "libc/nt/enum/signal.h"
#include "libc/nt/enum/status.h"
#include "libc/nt/events.h"
#include "libc/nt/runtime.h"
#include "libc/nt/signals.h"
#include "libc/nt/struct/ntexceptionpointers.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/ss.h"
#include "libc/thread/posixthread.internal.h"
#ifdef __x86_64__

/**
 * @fileoverview Cosmopolitan Signals for Windows.
 */

#define STKSZ 65536

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
  atomic_fetch_and_explicit(__sig.process, ~(1ull << (sig - 1)),
                            memory_order_relaxed);
  _pthread_lock();
  for (e = dll_last(_pthread_list); e; e = dll_prev(_pthread_list, e))
    atomic_fetch_and_explicit(&POSIXTHREAD_CONTAINER(e)->tib->tib_sigpending,
                              ~(1ull << (sig - 1)), memory_order_relaxed);
  _pthread_unlock();
}

static textwindows int __sig_getter(atomic_ulong *sigs, sigset_t masked) {
  int sig;
  sigset_t bit, pending, deliverable;
  for (;;) {
    pending = atomic_load_explicit(sigs, memory_order_acquire);
    if ((deliverable = pending & ~masked)) {
      sig = bsfl(deliverable) + 1;
      bit = 1ull << (sig - 1);
      if (atomic_fetch_and_explicit(sigs, ~bit, memory_order_acq_rel) & bit)
        return sig;
    } else {
      return 0;
    }
  }
}

textwindows int __sig_get(sigset_t masked) {
  int sig;
  if (!(sig = __sig_getter(&__get_tls()->tib_sigpending, masked)))
    sig = __sig_getter(__sig.process, masked);
  return sig;
}

static textwindows bool __sig_should_use_altstack(unsigned flags,
                                                  struct CosmoTib *tib) {
  if (!(flags & SA_ONSTACK))
    return false;  // signal handler didn't enable it
  if (!tib->tib_sigstack_size)
    return false;  // sigaltstack() wasn't installed on this thread
  if (tib->tib_sigstack_flags & SS_DISABLE)
    return false;  // sigaltstack() on this thread was disabled by user
  char *bp = __builtin_frame_address(0);
  if (tib->tib_sigstack_addr <= bp &&
      bp <= tib->tib_sigstack_addr + tib->tib_sigstack_size)
    return false;  // we're already on the alternate stack
  return true;
}

static textwindows wontreturn void __sig_terminate(int sig) {
  TerminateThisProcess(sig);
}

textwindows static bool __sig_wake(struct PosixThread *pt, int sig) {
  atomic_int *blocker;
  blocker = atomic_load_explicit(&pt->pt_blocker, memory_order_acquire);
  if (!blocker)
    return false;
  // threads can create semaphores on an as-needed basis
  if (blocker == PT_BLOCKER_EVENT) {
    STRACE("%G set %d's event object", sig, _pthread_tid(pt));
    SetEvent(pt->pt_event);
    return !!atomic_load_explicit(&pt->pt_blocker, memory_order_acquire);
  }
  // all other blocking ops that aren't overlap should use futexes
  // we force restartable futexes to churn by waking w/o releasing
  STRACE("%G waking %d's futex", sig, _pthread_tid(pt));
  WakeByAddressSingle(blocker);
  return !!atomic_load_explicit(&pt->pt_blocker, memory_order_acquire);
}

textwindows static bool __sig_start(struct PosixThread *pt, int sig,
                                    unsigned *rva, unsigned *flags) {
  *rva = __sighandrvas[sig];
  *flags = __sighandflags[sig];
  if (*rva == (intptr_t)SIG_IGN ||
      (*rva == (intptr_t)SIG_DFL && __sig_ignored_by_default(sig))) {
    STRACE("ignoring %G", sig);
    return false;
  }
  if (atomic_load_explicit(&pt->tib->tib_sigmask, memory_order_acquire) &
      (1ull << (sig - 1))) {
    STRACE("enqueing %G on %d", sig, _pthread_tid(pt));
    atomic_fetch_or_explicit(&pt->tib->tib_sigpending, 1ull << (sig - 1),
                             memory_order_relaxed);
    __sig_wake(pt, sig);
    return false;
  }
  if (*rva == (intptr_t)SIG_DFL) {
    STRACE("terminating on %G due to no handler", sig);
    __sig_terminate(sig);
  }
  return true;
}

textwindows static sigaction_f __sig_handler(unsigned rva) {
  atomic_fetch_add_explicit(&__sig.count, 1, memory_order_relaxed);
  return (sigaction_f)(__executable_start + rva);
}

textwindows int __sig_raise(volatile int sig, int sic) {

  // bitset of kinds of handlers called
  volatile int handler_was_called = 0;

  // loop over pending signals
  ucontext_t ctx;
  getcontext(&ctx);
  if (!sig) {
    if ((sig = __sig_get(ctx.uc_sigmask))) {
      sic = SI_KERNEL;
    } else {
      return handler_was_called;
    }
  }

  // process signal(s)
  unsigned rva, flags;
  struct PosixThread *pt = _pthread_self();
  if (__sig_start(pt, sig, &rva, &flags)) {

    if (flags & SA_RESETHAND) {
      STRACE("resetting %G handler", sig);
      __sighandrvas[sig] = (int32_t)(intptr_t)SIG_DFL;
    }

    // update the signal mask in preparation for signal handller
    sigset_t blocksigs = __sighandmask[sig];
    if (!(flags & SA_NODEFER))
      blocksigs |= 1ull << (sig - 1);
    ctx.uc_sigmask = atomic_fetch_or_explicit(&pt->tib->tib_sigmask, blocksigs,
                                              memory_order_acquire);

    // call the user's signal handler
    char ssbuf[128];
    siginfo_t si = {.si_signo = sig, .si_code = sic};
    STRACE("__sig_raise(%G, %t) mask %s", sig, __sig_handler(rva),
           _DescribeSigset(ssbuf, 0, (sigset_t *)&pt->tib->tib_sigmask));
    __sig_handler(rva)(sig, &si, &ctx);

    // record this handler
    if (flags & SA_RESTART) {
      handler_was_called |= SIG_HANDLED_SA_RESTART;
    } else {
      handler_was_called |= SIG_HANDLED_NO_RESTART;
    }
  }

  // restore sigmask
  // loop back to top
  // jump where handler says
  sig = 0;
  return setcontext(&ctx);
}

textwindows int __sig_relay(int sig, int sic, sigset_t waitmask) {
  sigset_t m;
  int handler_was_called;
  m = atomic_exchange_explicit(&__get_tls()->tib_sigmask, waitmask,
                               memory_order_acquire);
  handler_was_called = __sig_raise(sig, SI_KERNEL);
  atomic_store_explicit(&__get_tls()->tib_sigmask, m, memory_order_release);
  return handler_was_called;
}

// the user's signal handler callback is wrapped with this trampoline
static textwindows wontreturn void __sig_tramp(struct SignalFrame *sf) {
  int sig = sf->si.si_signo;
  struct CosmoTib *tib = __get_tls();
  struct PosixThread *pt = (struct PosixThread *)tib->tib_pthread;
  atomic_store_explicit(&pt->pt_intoff, 0, memory_order_release);
  for (;;) {

    // update the signal mask in preparation for signal handler
    sigset_t blocksigs = __sighandmask[sig];
    if (!(sf->flags & SA_NODEFER))
      blocksigs |= 1ull << (sig - 1);
    sf->ctx.uc_sigmask = atomic_fetch_or_explicit(&tib->tib_sigmask, blocksigs,
                                                  memory_order_acquire);

    // call the user's signal handler
    char ssbuf[2][128];
    STRACE("__sig_tramp(%G, %t) mask %s → %s", sig, __sig_handler(sf->rva),
           _DescribeSigset(ssbuf[0], 0, &sf->ctx.uc_sigmask),
           _DescribeSigset(ssbuf[1], 0, (sigset_t *)&tib->tib_sigmask));
    __sig_handler(sf->rva)(sig, &sf->si, &sf->ctx);

    // restore the signal mask that was used by the interrupted code
    // this may have been modified by the signal handler in the callback
    atomic_store_explicit(&tib->tib_sigmask, sf->ctx.uc_sigmask,
                          memory_order_release);

    // jump back into original code if there aren't any pending signals
    do {
      if (!(sig = __sig_get(sf->ctx.uc_sigmask)))
        __sig_restore(&sf->ctx);
    } while (!__sig_start(pt, sig, &sf->rva, &sf->flags));

    // tail recurse into another signal handler
    sf->si.si_signo = sig;
    sf->si.si_code = SI_KERNEL;
    if (sf->flags & SA_RESETHAND) {
      STRACE("resetting %G handler", sig);
      __sighandrvas[sig] = (int32_t)(intptr_t)SIG_DFL;
    }
  }
}

// sends signal to another specific thread which is ref'd
static textwindows int __sig_killer(struct PosixThread *pt, int sig, int sic) {
  unsigned rva = __sighandrvas[sig];
  unsigned flags = __sighandflags[sig];

  // do nothing if signal is ignored
  if (rva == (intptr_t)SIG_IGN ||
      (rva == (intptr_t)SIG_DFL && __sig_ignored_by_default(sig))) {
    STRACE("ignoring %G", sig);
    return 0;
  }

  // we can't preempt threads that masked sigs or are blocked on i/o
  while ((atomic_load_explicit(&pt->tib->tib_sigmask, memory_order_acquire) &
          (1ull << (sig - 1)))) {
    if (atomic_fetch_or_explicit(&pt->tib->tib_sigpending, 1ull << (sig - 1),
                                 memory_order_acq_rel) &
        (1ull << (sig - 1)))
      // we believe signal was already enqueued
      return 0;
    if (__sig_wake(pt, sig))
      // we believe i/o routine will handle signal
      return 0;
    if (atomic_load_explicit(&pt->tib->tib_sigmask, memory_order_acquire) &
        (1ull << (sig - 1)))
      // we believe ALLOW_SIGNALS will handle signal
      return 0;
    if (!(atomic_fetch_and_explicit(&pt->tib->tib_sigpending,
                                    ~(1ull << (sig - 1)),
                                    memory_order_acq_rel) &
          (1ull << (sig - 1))))
      // we believe another thread sniped our signal
      return 0;
    break;
  }

  // avoid race conditions and deadlocks with thread suspend process
  if (atomic_exchange_explicit(&pt->pt_intoff, 1, memory_order_acquire)) {
    // we believe another thread is asynchronously waking the mark
    if (atomic_fetch_or_explicit(&pt->tib->tib_sigpending, 1ull << (sig - 1),
                                 memory_order_acq_rel) &
        (1ull << (sig - 1)))
      // we believe our signal is already being delivered
      return 0;
    if (atomic_load_explicit(&pt->pt_intoff, memory_order_acquire) ||
        atomic_exchange_explicit(&pt->pt_intoff, 1, memory_order_acquire))
      // we believe __sig_tramp will deliver our signal
      return 0;
    if (!(atomic_fetch_and_explicit(&pt->tib->tib_sigpending,
                                    ~(1ull << (sig - 1)),
                                    memory_order_acq_rel) &
          (1ull << (sig - 1))))
      // we believe another thread sniped our signal
      return 0;
  }

  // if there's no handler then killing a thread kills the process
  if (rva == (intptr_t)SIG_DFL) {
    STRACE("terminating on %G due to no handler", sig);
    __sig_terminate(sig);
  }

  // take control of thread
  // suspending the thread happens asynchronously
  // however getting the context blocks until it's frozen
  uintptr_t th = _pthread_syshand(pt);
  if (SuspendThread(th) == -1u) {
    STRACE("SuspendThread failed w/ %d", GetLastError());
    atomic_store_explicit(&pt->pt_intoff, 0, memory_order_release);
    return ESRCH;
  }
  struct NtContext nc;
  nc.ContextFlags = kNtContextFull;
  if (!GetThreadContext(th, &nc)) {
    STRACE("GetThreadContext failed w/ %d", GetLastError());
    ResumeThread(th);
    atomic_store_explicit(&pt->pt_intoff, 0, memory_order_release);
    return ESRCH;
  }

  // we can't preempt threads that masked sig or are blocked
  // we can't preempt threads that are running in win32 code
  // so we shall unblock the thread and let it signal itself
  if (!((uintptr_t)__executable_start <= nc.Rip &&
        nc.Rip < (uintptr_t)__privileged_start)) {
    atomic_fetch_or_explicit(&pt->tib->tib_sigpending, 1ull << (sig - 1),
                             memory_order_relaxed);
    ResumeThread(th);
    atomic_store_explicit(&pt->pt_intoff, 0, memory_order_release);
    __sig_wake(pt, sig);
    return 0;
  }

  // preferring to live dangerously
  // the thread will be signaled asynchronously
  if (flags & SA_RESETHAND) {
    STRACE("resetting %G handler", sig);
    __sighandrvas[sig] = (int32_t)(intptr_t)SIG_DFL;
  }

  // inject call to trampoline function into thread
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
    atomic_store_explicit(&pt->pt_intoff, 0, memory_order_release);
    return ESRCH;
  }
  ResumeThread(th);
  __sig_wake(pt, sig);
  return 0;
}

// sends signal to another specific thread
textwindows int __sig_kill(struct PosixThread *pt, int sig, int sic) {
  int rc;
  BLOCK_SIGNALS;
  rc = __sig_killer(pt, sig, sic);
  ALLOW_SIGNALS;
  return rc;
}

// sends signal to any other thread
// this should only be called by non-posix threads
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
  if (atomic_load_explicit(__sig.process, memory_order_acquire) &
      (1ull << (sig - 1))) {
    return;
  }
  _pthread_lock();
  for (e = dll_first(_pthread_list); e; e = dll_next(_pthread_list, e)) {
    pt = POSIXTHREAD_CONTAINER(e);
    // we don't want to signal ourself
    if (pt == _pthread_self())
      continue;
    // we don't want to signal a thread that isn't running
    if (atomic_load_explicit(&pt->pt_status, memory_order_acquire) >=
        kPosixThreadTerminated) {
      continue;
    }
    // choose this thread if it isn't masking sig
    if (!(atomic_load_explicit(&pt->tib->tib_sigmask, memory_order_acquire) &
          (1ull << (sig - 1)))) {
      _pthread_ref(pt);
      mark = pt;
      break;
    }
    // if a thread is blocking then we check to see if it's planning
    // to unblock our sig once the wait operation is completed; when
    // that's the case we can cancel the thread's i/o to deliver sig
    if (atomic_load_explicit(&pt->pt_blocker, memory_order_acquire) &&
        !(pt->pt_blkmask & (1ull << (sig - 1)))) {
      _pthread_ref(pt);
      mark = pt;
      break;
    }
  }
  _pthread_unlock();
  if (mark) {
    // no lock needed since current thread is nameless and formless
    __sig_killer(mark, sig, sic);
    _pthread_unref(mark);
  } else {
    atomic_fetch_or_explicit(__sig.process, 1ull << (sig - 1),
                             memory_order_relaxed);
  }
}

static textwindows char *__sig_stpcpy(char *d, const char *s) {
  size_t i;
  for (i = 0;; ++i)
    if (!(d[i] = s[i]))
      return d + i;
}

static textwindows wontreturn void __sig_death(int sig, const char *thing) {
#ifndef TINY
  intptr_t hStderr;
  char sigbuf[21], s[128], *p;
  hStderr = GetStdHandle(kNtStdErrorHandle);
  p = __sig_stpcpy(s, "Terminating on ");
  p = __sig_stpcpy(p, thing);
  p = __sig_stpcpy(p, strsignal_r(sig, sigbuf));
  p = __sig_stpcpy(p,
                   ". Pass --strace and/or ShowCrashReports() for details.\n");
  WriteFile(hStderr, s, p - s, 0, 0);
#endif
  __sig_terminate(sig);
}

static textwindows void __sig_unmaskable(struct NtExceptionPointers *ep,
                                         int code, int sig,
                                         struct CosmoTib *tib) {

  // log vital crash information reliably for --strace before doing much
  // we don't print this without the flag since raw numbers scare people
  // this needs at least one page of stack memory in order to get logged
  // otherwise it'll print a warning message about the lack of stack mem
  STRACE("win32 vectored exception 0x%08Xu raising %G "
         "cosmoaddr2line %s %lx %s",
         ep->ExceptionRecord->ExceptionCode, sig,
         _weaken(FindDebugBinary) ? _weaken(FindDebugBinary)()
                                  : program_invocation_name,
         ep->ContextRecord->Rip,
         DescribeBacktrace((struct StackFrame *)ep->ContextRecord->Rbp));

  // if the user didn't install a signal handler for this unmaskable
  // exception, then print a friendly helpful hint message to stderr
  unsigned rva = __sighandrvas[sig];
  if (rva == (intptr_t)SIG_DFL || rva == (intptr_t)SIG_IGN)
    __sig_death(sig, "uncaught ");

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
  // and a modifiable view of the faulting code's cpu state
  // temporarily replace signal mask while calling crash handler
  // abort process if sig is already blocked to avoid crash loop
  // note ucontext_t is a hefty data structures on top of NtContext
  ucontext_t ctx = {0};
  siginfo_t si = {.si_signo = sig, .si_code = code, .si_addr = si_addr};
  _ntcontext2linux(&ctx, ep->ContextRecord);
  sigset_t blocksigs = __sighandmask[sig];
  if (!(flags & SA_NODEFER))
    blocksigs |= 1ull << (sig - 1);
  ctx.uc_sigmask = atomic_fetch_or_explicit(&tib->tib_sigmask, blocksigs,
                                            memory_order_acquire);
  if (ctx.uc_sigmask & (1ull << (sig - 1))) {
    __sig_death(sig, "masked ");
    __sig_terminate(sig);
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
  int code, sig = __sig_crash_sig(ep->ExceptionRecord->ExceptionCode, &code);

  // advance the instruction pointer to skip over debugger breakpoints
  // this behavior is consistent with how unix kernels are implemented
  if (sig == SIGTRAP) {
    ep->ContextRecord->Rip++;
    if (__sig_ignored(sig))
      return kNtExceptionContinueExecution;
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
  // win32 launches a thread to deliver ctrl-c and ctrl-break when typed
  // it only happens when kNtEnableProcessedInput is in play on console.
  // otherwise we need to wait until read-nt.c discovers that keystroke.
  struct CosmoTib tls;
  __bootstrap_tls(&tls, __builtin_frame_address(0));
  __sig_generate(__sig_console_sig(dwCtrlType), SI_KERNEL);
  return true;
}

// returns 0 if no signal handlers were called, otherwise a bitmask
// consisting of `1` which means a signal handler was invoked which
// didn't have the SA_RESTART flag, and `2`, which means SA_RESTART
// handlers were called (or `3` if both were the case).
textwindows int __sig_check(void) {
  int sig, res = 0;
  while ((sig = __sig_get(atomic_load_explicit(&__get_tls()->tib_sigmask,
                                               memory_order_acquire))))
    res |= __sig_raise(sig, SI_KERNEL);
  return res;
}

// background thread for delivering inter-process signals asynchronously
// this checks for undelivered process-wide signals, once per scheduling
// quantum, which on windows should be every ~15ms or so, unless somehow
// the process was tuned to have more fine-grained event timing. we want
// signals to happen faster when possible; that happens when cancelation
// points, e.g. read need to wait on i/o; they too check for new signals
textwindows dontinstrument static uint32_t __sig_worker(void *arg) {
  struct CosmoTib tls;
  __bootstrap_tls(&tls, __builtin_frame_address(0));
  char *sp = __builtin_frame_address(0);
  __maps_track((char *)(((uintptr_t)sp + __pagesize - 1) & -__pagesize) - STKSZ,
               STKSZ);
  for (;;) {

    // dequeue all pending signals and fire them off. if there's no
    // thread that can handle them then __sig_generate will requeue
    // those signals back to __sig.process; hence the need for xchg
    unsigned long sigs =
        atomic_exchange_explicit(__sig.process, 0, memory_order_acq_rel);
    while (sigs) {
      int sig = bsfl(sigs) + 1;
      sigs &= ~(1ull << (sig - 1));
      __sig_generate(sig, SI_KERNEL);
    }

    // unblock stalled asynchronous signals in threads
    _pthread_lock();
    for (struct Dll *e = dll_first(_pthread_list); e;
         e = dll_next(_pthread_list, e)) {
      struct PosixThread *pt = POSIXTHREAD_CONTAINER(e);
      if (atomic_load_explicit(&pt->pt_status, memory_order_acquire) >=
          kPosixThreadTerminated) {
        break;
      }
      sigset_t pending =
          atomic_load_explicit(&pt->tib->tib_sigpending, memory_order_acquire);
      sigset_t mask =
          atomic_load_explicit(&pt->tib->tib_sigmask, memory_order_acquire);
      if (pending & ~mask) {
        _pthread_ref(pt);
        _pthread_unlock();
        while (!atomic_compare_exchange_weak_explicit(
            &pt->tib->tib_sigpending, &pending, pending & ~mask,
            memory_order_acq_rel, memory_order_relaxed)) {
        }
        while ((pending = pending & ~mask)) {
          int sig = bsfl(pending) + 1;
          pending &= ~(1ull << (sig - 1));
          __sig_killer(pt, sig, SI_KERNEL);
        }
        _pthread_lock();
        _pthread_unref(pt);
      }
    }
    _pthread_unlock();

    // wait until next scheduler quantum
    Sleep(POLL_INTERVAL_MS);
  }
  return 0;
}

__attribute__((__constructor__(10))) textstartup void __sig_init(void) {
  if (!IsWindows())
    return;
  AddVectoredExceptionHandler(true, (void *)__sig_crash);
  SetConsoleCtrlHandler((void *)__sig_console, true);
  CreateThread(0, STKSZ, __sig_worker, 0, kNtStackSizeParamIsAReservation, 0);
}

#endif /* __x86_64__ */
