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
#include "libc/runtime/fenv.h"

/**
 * Disables floating point exception trapping, e.g.
 *
 *     feenableexcept(FE_INVALID | FE_DIVBYZERO |
 *                    FE_OVERFLOW | FE_UNDERFLOW);
 *
 * When trapping is enabled, something should handle SIGFPE. Calling
 * ShowCrashReports() at startup will install a generic handler with
 * backtraces and the symbol of the `si->si_code` which UNIX defines
 *
 * - `FPE_INTOVF`: integer overflow
 * - `FPE_INTDIV`: integer divide by zero
 * - `FPE_FLTDIV`: floating point divide by zero
 * - `FPE_FLTOVF`: floating point overflow
 * - `FPE_FLTUND`: floating point underflow
 * - `FPE_FLTRES`: floating point inexact
 * - `FPE_FLTINV`: invalid floating point operation
 * - `FPE_FLTSUB`: subscript out of range
 *
 * It's important to not use the `-ffast-math` or `-Ofast` flags when
 * compiling code that needs to be debugged. Using `-fsignaling-nans`
 * will also help, since GCC doesn't enable that by default.
 *
 * @param excepts may bitwise-or the following:
 *     - `FE_INVALID`
 *     - `FE_DIVBYZERO`
 *     - `FE_OVERFLOW`
 *     - `FE_UNDERFLOW`
 *     - `FE_INEXACT`
 *     - `FE_ALL_EXCEPT` (all of the above)
 * @see fetestexcept() if you don't want to deal with signals
 * @see feenableexcept() to turn it on in the first place
 */
int fedisableexcept(int excepts) {

  // limit to what we know
  excepts &= FE_ALL_EXCEPT;

#ifdef __x86_64__

#ifndef NOX87
  // configure 8087 fpu control word
  // setting the bits enables suppression
  unsigned short x87cw;
  asm("fstcw\t%0" : "=m"(x87cw));
  x87cw |= excepts;
  asm("fldcw\t%0" : /* no inputs */ : "m"(x87cw));
#endif

  // configure modern sse control word
  // setting the bits enables suppression
  unsigned mxcsr;
  asm("stmxcsr\t%0" : "=m"(mxcsr));
  mxcsr |= excepts << 7;
  asm("ldmxcsr\t%0" : /* no inputs */ : "m"(mxcsr));

  return 0;

#elif defined(__aarch64__)

  unsigned fpcr;
  unsigned fpcr2;
  fpcr = __builtin_aarch64_get_fpcr();
  fpcr2 = fpcr & ~(excepts << 8);
  if (fpcr != fpcr2)
    __builtin_aarch64_set_fpcr(fpcr2);
  return (fpcr >> 8) & FE_ALL_EXCEPT;

#else
  return -1;
#endif
}
