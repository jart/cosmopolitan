/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/sig.internal.h"
#include "libc/intrin/pushpop.internal.h"
#include "libc/nt/enum/signal.h"
#include "libc/nt/enum/status.h"
#include "libc/nt/struct/ntexceptionpointers.h"

// this is all a mandatory dependency of winmain
// so, we trade away maintanibility for tininess
// see libc/sysv/consts.sh for canonical magnums

#define SIGILL_  pushpop(4)
#define SIGTRAP_ pushpop(5)
#define SIGABRT_ pushpop(6)
#define SIGFPE_  pushpop(8)
#define SIGSEGV_ pushpop(11)
#define SIGSYS_  pushpop(31)

#define TRAP_BRKPT_  pushpop(1)
#define ILL_ILLOPC_  pushpop(1)
#define ILL_PRVOPC_  pushpop(5)
#define SEGV_MAPERR_ pushpop(1)
#define SEGV_ACCERR_ pushpop(2)
#define SI_USER_     pushpop(0)
#define FPE_FLTDIV_  pushpop(3)
#define FPE_FLTOVF_  pushpop(4)
#define FPE_INTOVF_  pushpop(2)
#define FPE_FLTUND_  pushpop(5)
#define FPE_FLTRES_  pushpop(6)
#define FPE_FLTINV_  pushpop(7)
#define SI_KERNEL_   0x80

textwindows int __sig_crash_sig(struct NtExceptionPointers *ep, int *code) {
  switch (ep->ExceptionRecord->ExceptionCode) {
    case kNtSignalBreakpoint:
      *code = TRAP_BRKPT_;
      return SIGTRAP_;
    case kNtSignalIllegalInstruction:
      *code = ILL_ILLOPC_;
      return SIGILL_;
    case kNtSignalPrivInstruction:
      *code = ILL_PRVOPC_;
      return SIGILL_;
    case kNtSignalInPageError:
    case kNtStatusStackOverflow:
      *code = SEGV_MAPERR_;
      return SIGSEGV_;
    case kNtSignalGuardPage:
    case kNtSignalAccessViolation:
      *code = SEGV_ACCERR_;
      return SIGSEGV_;
    case kNtSignalInvalidHandle:
    case kNtSignalInvalidParameter:
    case kNtSignalAssertionFailure:
      *code = SI_USER_;
      return SIGABRT_;
    case kNtStatusIntegerOverflow:
      *code = FPE_INTOVF_;
      return SIGFPE_;
    case kNtSignalFltDivideByZero:
      *code = FPE_FLTDIV_;
      return SIGFPE_;
    case kNtSignalFltOverflow:
      *code = FPE_FLTOVF_;
      return SIGFPE_;
    case kNtSignalFltUnderflow:
      *code = FPE_FLTUND_;
      return SIGFPE_;
    case kNtSignalFltInexactResult:
      *code = FPE_FLTRES_;
      return SIGFPE_;
    case kNtSignalFltDenormalOperand:
    case kNtSignalFltInvalidOperation:
    case kNtSignalFltStackCheck:
    case kNtSignalIntegerDivideByZero:
    case kNtSignalFloatMultipleFaults:
    case kNtSignalFloatMultipleTraps:
      *code = FPE_FLTINV_;
      return SIGFPE_;
    case kNtSignalDllNotFound:
    case kNtSignalOrdinalNotFound:
    case kNtSignalEntrypointNotFound:
    case kNtSignalDllInitFailed:
      *code = SI_KERNEL_;
      return SIGSYS_;
    default:
      *code = ep->ExceptionRecord->ExceptionCode;
      return SIGSEGV_;
  }
}
