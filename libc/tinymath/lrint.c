/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/math.h"

/**
 * Rounds to integer in current rounding mode.
 *
 * The floating-point exception `FE_INEXACT` is raised if the result is
 * different from the input.
 */
long lrint(double x) {
  long i;
#ifdef __x86_64__
  asm("cvtsd2si\t%1,%0" : "=r"(i) : "x"(x));
#elif defined(__aarch64__)
  asm("frintx\t%d1,%d1\n\t"
      "fcvtzs\t%x0,%d1"
      : "=r"(i), "+w"(x));
#elif defined(__powerpc64__) && defined(_ARCH_PWR5X)
  asm("fctid\t%0,%1" : "=d"(i) : "d"(x));
#else
  i = rint(x);
#endif /* __x86_64__ */
  return i;
}

#if __SIZEOF_LONG__ == __SIZEOF_LONG_LONG__
__weak_reference(lrint, llrint);
#endif

#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024
__weak_reference(lrint, lrintl);
#if __SIZEOF_LONG__ == __SIZEOF_LONG_LONG__
__weak_reference(lrint, llrintl);
#endif
#endif
