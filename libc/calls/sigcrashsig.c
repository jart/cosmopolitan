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
#include "libc/intrin/pushpop.h"
#include "libc/macros.h"
#include "libc/nt/enum/signal.h"
#include "libc/nt/enum/status.h"
#include "libc/nt/struct/ntexceptionpointers.h"

// this is all a mandatory dependency of winmain
// so, we trade away maintanibility for tininess
// see libc/sysv/consts.sh for canonical magnums

#define SIGILL_  4
#define SIGTRAP_ 5
#define SIGABRT_ 6
#define SIGFPE_  8
#define SIGSEGV_ 11
#define SIGSYS_  31

#define TRAP_BRKPT_  1
#define ILL_ILLOPC_  1
#define ILL_PRVOPC_  5
#define SEGV_MAPERR_ 1
#define SEGV_ACCERR_ 2
#define SI_USER_     0
#define FPE_FLTDIV_  3
#define FPE_FLTOVF_  4
#define FPE_INTOVF_  2
#define FPE_FLTUND_  5
#define FPE_FLTRES_  6
#define FPE_FLTINV_  7
#define SI_KERNEL_   128

#define LO(x) (x & 255)
#define HI(x) ((x >> 24) / !(x & 0x00ffff00u))
#define ROW(x, sic, sig)                                                   \
  {                                                                        \
    {                                                                      \
      {                                                                    \
        LO(x), HI(x), sic / !(sic & 0xffffff00), sig / !(sig & 0xffffff00) \
      }                                                                    \
    }                                                                      \
  }

struct CrashSig {
  union {
    struct {
      unsigned char lo;
      unsigned char hi;
      unsigned char sic;
      unsigned char sig;
    };
    unsigned word;
  };
};

static const struct CrashSig kNtCrashSigs[] = {
    ROW(kNtSignalBreakpoint, TRAP_BRKPT_, SIGTRAP_),          //
    ROW(kNtSignalIllegalInstruction, ILL_ILLOPC_, SIGILL_),   //
    ROW(kNtSignalPrivInstruction, ILL_PRVOPC_, SIGILL_),      //
    ROW(kNtSignalInPageError, SEGV_MAPERR_, SIGSEGV_),        //
    ROW(kNtStatusStackOverflow, SEGV_MAPERR_, SIGSEGV_),      //
    ROW(kNtSignalGuardPage, SEGV_ACCERR_, SIGSEGV_),          //
    ROW(kNtSignalAccessViolation, SEGV_ACCERR_, SIGSEGV_),    //
    ROW(kNtSignalInvalidHandle, SI_USER_, SIGABRT_),          //
    ROW(kNtSignalInvalidParameter, SI_USER_, SIGABRT_),       //
    ROW(kNtStatusIntegerOverflow, FPE_INTOVF_, SIGFPE_),      //
    ROW(kNtSignalFltDivideByZero, FPE_FLTDIV_, SIGFPE_),      //
    ROW(kNtSignalFltOverflow, FPE_FLTOVF_, SIGFPE_),          //
    ROW(kNtSignalFltUnderflow, FPE_FLTUND_, SIGFPE_),         //
    ROW(kNtSignalFltInexactResult, FPE_FLTRES_, SIGFPE_),     //
    ROW(kNtSignalFltDenormalOperand, FPE_FLTINV_, SIGFPE_),   //
    ROW(kNtSignalFltInvalidOperation, FPE_FLTINV_, SIGFPE_),  //
    ROW(kNtSignalFltStackCheck, FPE_FLTINV_, SIGFPE_),        //
    ROW(kNtSignalIntegerDivideByZero, FPE_FLTINV_, SIGFPE_),  //
    // ROW(kNtSignalAssertionFailure, SI_USER_, SIGABRT_),
    // ROW(kNtSignalFloatMultipleTraps, FPE_FLTINV_, SIGFPE_),
    // ROW(kNtSignalFloatMultipleFaults, FPE_FLTINV_, SIGFPE_),
    // ROW(kNtSignalDllNotFound, SI_KERNEL_, SIGSYS_),
    // ROW(kNtSignalOrdinalNotFound, SI_KERNEL_, SIGSYS_),
    // ROW(kNtSignalEntrypointNotFound, SI_KERNEL_, SIGSYS_),
    // ROW(kNtSignalDllInitFailed, SI_KERNEL_, SIGSYS_),
};

textwindows dontinstrument int __sig_crash_sig(unsigned exception, int *code) {
  for (int i = 0; i < ARRAYLEN(kNtCrashSigs); ++i) {
    struct CrashSig cs;
    cs.word = kNtCrashSigs[i].word;
    unsigned lo = cs.lo;
    unsigned hi = cs.hi;
    unsigned ec = lo | hi << 24;
    if (ec == exception) {
      *code = cs.sic;
      return cs.sig;
    }
  }
  *code = exception;
  return SIGSEGV_;
}
