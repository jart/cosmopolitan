/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/ucontext.internal.h"
#include "libc/calls/ucontext.h"
#include "libc/intrin/describebacktrace.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/nt/enum/exceptionhandleractions.h"
#include "libc/nt/enum/signal.h"
#include "libc/nt/enum/status.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/ntexceptionpointers.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/thread/tls.h"
#include "libc/thread/tls2.internal.h"

#ifdef __x86_64__

// win32 calls this; we're running inside the thread that crashed
__msabi unsigned __wincrash(struct NtExceptionPointers *ep) {
  int sig, code;
  struct CosmoTib *tib;
  static bool noreentry;
  noreentry = true;

  if ((tib = __tls_enabled ? __get_tls() : 0)) {
    if (~tib->tib_flags & TIB_FLAG_WINCRASHING) {
      tib->tib_flags |= TIB_FLAG_WINCRASHING;
    } else {
      ExitProcess(SIGSEGV);
    }
  } else {
    if (!noreentry) {
      noreentry = true;
    } else {
      ExitProcess(SIGSEGV);
    }
  }

  switch (ep->ExceptionRecord->ExceptionCode) {
    case kNtSignalBreakpoint:
      code = TRAP_BRKPT;
      sig = SIGTRAP;
      // Windows seems to be the only operating system that traps INT3 at
      // addressof(INT3) rather than addressof(INT3)+1. So we must adjust
      // RIP to prevent the same INT3 from being trapped forevermore.
      ep->ContextRecord->Rip++;
      break;
    case kNtSignalIllegalInstruction:
      code = ILL_ILLOPC;
      sig = SIGILL;
      break;
    case kNtSignalPrivInstruction:
      code = ILL_PRVOPC;
      sig = SIGILL;
      break;
    case kNtSignalGuardPage:
    case kNtSignalInPageError:
    case kNtStatusStackOverflow:
      code = SEGV_MAPERR;
      sig = SIGSEGV;
      break;
    case kNtSignalAccessViolation:
      code = SEGV_ACCERR;
      sig = SIGSEGV;
      break;
    case kNtSignalInvalidHandle:
    case kNtSignalInvalidParameter:
    case kNtSignalAssertionFailure:
      code = SI_USER;
      sig = SIGABRT;
      break;
    case kNtStatusIntegerOverflow:
      code = FPE_INTOVF;
      sig = SIGFPE;
      break;
    case kNtSignalFltDivideByZero:
      code = FPE_FLTDIV;
      sig = SIGFPE;
      break;
    case kNtSignalFltOverflow:
      code = FPE_FLTOVF;
      sig = SIGFPE;
      break;
    case kNtSignalFltUnderflow:
      code = FPE_FLTUND;
      sig = SIGFPE;
      break;
    case kNtSignalFltInexactResult:
      code = FPE_FLTRES;
      sig = SIGFPE;
      break;
    case kNtSignalFltDenormalOperand:
    case kNtSignalFltInvalidOperation:
    case kNtSignalFltStackCheck:
    case kNtSignalIntegerDivideByZero:
    case kNtSignalFloatMultipleFaults:
    case kNtSignalFloatMultipleTraps:
      code = FPE_FLTINV;
      sig = SIGFPE;
      break;
    case kNtSignalDllNotFound:
    case kNtSignalOrdinalNotFound:
    case kNtSignalEntrypointNotFound:
    case kNtSignalDllInitFailed:
      code = SI_KERNEL;
      sig = SIGSYS;
      break;
    default:
      code = ep->ExceptionRecord->ExceptionCode;
      sig = SIGSEGV;
      break;
  }

  STRACE("wincrash %G rip %x bt %s", sig, ep->ContextRecord->Rip,
         DescribeBacktrace((struct StackFrame *)ep->ContextRecord->Rbp));

  if (__sighandflags[sig] & SA_SIGINFO) {
    struct Delivery pkg = {kSigOpUnmaskable, sig, code, ep->ContextRecord};
    __sig_tramp(&pkg);
  } else {
    __sig_handle(kSigOpUnmaskable, sig, code, 0);
  }

  noreentry = false;
  if (tib) {
    tib->tib_flags &= ~TIB_FLAG_WINCRASHING;
  }

  return kNtExceptionContinueExecution;
}

#endif /* __x86_64__ */
