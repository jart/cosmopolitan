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
#include "libc/calls/ucontext.h"
#include "libc/nt/enum/exceptionhandleractions.h"
#include "libc/nt/enum/signal.h"
#include "libc/nt/struct/ntexceptionpointers.h"
#include "libc/str/str.h"

textwindows unsigned __wincrash(struct NtExceptionPointers *ep) {
  int sig;
  struct Goodies {
    ucontext_t ctx;
    struct siginfo si;
  } g;
  switch (ep->ExceptionRecord->ExceptionCode) {
    case kNtSignalBreakpoint:
      sig = SIGTRAP;
      break;
    case kNtSignalIllegalInstruction:
    case kNtSignalPrivInstruction:
      sig = SIGILL;
      break;
    case kNtSignalGuardPage:
    case kNtSignalAccessViolation:
    case kNtSignalInPageError:
      sig = SIGSEGV;
      break;
    case kNtSignalInvalidHandle:
    case kNtSignalInvalidParameter:
    case kNtSignalAssertionFailure:
      sig = SIGABRT;
      break;
    case kNtSignalFltDenormalOperand:
    case kNtSignalFltDivideByZero:
    case kNtSignalFltInexactResult:
    case kNtSignalFltInvalidOperation:
    case kNtSignalFltOverflow:
    case kNtSignalFltStackCheck:
    case kNtSignalFltUnderflow:
    case kNtSignalIntegerDivideByZero:
    case kNtSignalFloatMultipleFaults:
    case kNtSignalFloatMultipleTraps:
      sig = SIGFPE;
      break;
    case kNtSignalDllNotFound:
    case kNtSignalOrdinalNotFound:
    case kNtSignalEntrypointNotFound:
    case kNtSignalDllInitFailed:
      sig = SIGSYS;
      break;
    default:
      return kNtExceptionContinueSearch;
  }
  memset(&g, 0, sizeof(g));
  ntcontext2linux(&g.ctx, ep->ContextRecord);
  return __sigenter(sig, &g.si, &g.ctx) ? kNtExceptionContinueExecution
                                        : kNtExceptionContinueSearch;
}
