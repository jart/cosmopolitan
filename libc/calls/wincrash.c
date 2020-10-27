/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/internal.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/ucontext.h"
#include "libc/nt/enum/exceptionhandleractions.h"
#include "libc/nt/signals.h"
#include "libc/nt/struct/ntexceptionpointers.h"
#include "libc/nt/struct/ntexceptionrecord.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sig.h"

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
