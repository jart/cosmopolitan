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
#include "libc/runtime/fenv.h"

/**
 * Rounds to nearest integer.
 */
double nearbyint(double x) {
#ifdef __aarch64__
  asm("frinti\t%d0,%d1" : "=w"(x) : "w"(x));
#elif defined(__s390x__) && (defined(__HTM__) || __ARCH__ >= 9)
  asm("fidbra\t%0,0,%1,4" : "=f"(x) : "f"(x));
#else
#ifdef FE_INEXACT
  /* #pragma STDC FENV_ACCESS ON */
  int e;
  e = fetestexcept(FE_INEXACT);
#endif
  x = rint(x);
#ifdef FE_INEXACT
  if (!e) feclearexcept(FE_INEXACT);
#endif
#endif /* __aarch64__ */
  return x;
}

#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024
__weak_reference(nearbyint, nearbyintl);
#endif
