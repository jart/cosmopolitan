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
#include "libc/sysv/consts/sig.h"
#include "libc/calls/sig.internal.h"
#include "libc/intrin/pushpop.h"
#include "libc/macros.h"
#include "libc/nt/enum/signal.h"
#include "libc/nt/enum/status.h"
#include "libc/nt/struct/ntexceptionpointers.h"
#include "libc/sysv/consts/sicode.h"

// this is all a mandatory dependency of winmain
// so, we trade away maintanibility for tininess
// see libc/sysv/consts.sh for canonical magnums

#define ILL_ILLOPC_ 1
#define ILL_PRVOPC_ 5
#define SI_USER_    0
#define FPE_FLTDIV_ 3
#define FPE_FLTOVF_ 4
#define FPE_INTOVF_ 2
#define FPE_FLTUND_ 5
#define FPE_FLTRES_ 6
#define FPE_FLTINV_ 7
#define SI_KERNEL_  128

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
    ROW(kNtStatusBreakpoint, TRAP_BRKPT, SIGTRAP),             //
    ROW(kNtStatusIllegalInstruction, ILL_ILLOPC_, SIGILL),     //
    ROW(kNtStatusPrivilegedInstruction, ILL_PRVOPC_, SIGILL),  //
    ROW(kNtStatusInPageError, BUS_ADRERR, SIGBUS),             //
    ROW(kNtStatusStackOverflow, SEGV_MAPERR, SIGSEGV),         //
    ROW(kNtStatusGuardPageViolation, SEGV_ACCERR, SIGSEGV),    //
    ROW(kNtStatusAccessViolation, SEGV_ACCERR, SIGSEGV),       //
    ROW(kNtStatusInvalidHandle, SI_USER_, SIGABRT),            //
    ROW(kNtStatusInvalidParameter, SI_USER_, SIGABRT),         //
    ROW(kNtStatusIntegerOverflow, FPE_INTOVF_, SIGFPE),        //
    ROW(kNtStatusFloatDivideByZero, FPE_FLTDIV_, SIGFPE),      //
    ROW(kNtStatusFloatOverflow, FPE_FLTOVF_, SIGFPE),          //
    ROW(kNtStatusFloatUnderflow, FPE_FLTUND_, SIGFPE),         //
    ROW(kNtStatusFloatInexactResult, FPE_FLTRES_, SIGFPE),     //
    ROW(kNtStatusFloatDenormalOperand, FPE_FLTINV_, SIGFPE),   //
    ROW(kNtStatusFloatInvalidOperation, FPE_FLTINV_, SIGFPE),  //
    ROW(kNtStatusFloatStackCheck, FPE_FLTINV_, SIGFPE),        //
    ROW(kNtStatusIntegerDivideBYZero, FPE_FLTINV_, SIGFPE),    //
    // ROW(kNtSignalAssertionFailure, SI_USER_, SIGABRT),
    // ROW(kNtSignalFloatMultipleTraps, FPE_FLTINV_, SIGFPE),
    // ROW(kNtSignalFloatMultipleFaults, FPE_FLTINV_, SIGFPE),
    // ROW(kNtSignalDllNotFound, SI_KERNEL_, SIGSYS),
    // ROW(kNtSignalOrdinalNotFound, SI_KERNEL_, SIGSYS),
    // ROW(kNtSignalEntrypointNotFound, SI_KERNEL_, SIGSYS),
    // ROW(kNtSignalDllInitFailed, SI_KERNEL_, SIGSYS),
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
  return SIGSEGV;
}
