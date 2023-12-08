/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/intrin/pmaddubsw.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/str/str.h"

/**
 * Multiplies bytes and adds adjacent results w/ short saturation.
 *
 *     𝑤ᵢ ← CLAMP[ 𝑏₂ᵢ𝑐₂ᵢ + 𝑏₍₂ᵢ₊₁₎𝑐₍₂ᵢ₊₁₎ ]
 *
 * @param 𝑤 [w/o] receives shorts
 * @param 𝑏 [r/o] is your byte data
 * @param 𝑐 [r/o] are your int8 coefficients
 * @note SSSE3 w/ Prescott c. 2004, Bulldozer c. 2011
 * @note greatest simd op, like, ever
 * @mayalias
 */
void(pmaddubsw)(int16_t w[8], const uint8_t b[16], const int8_t c[16]) {
  unsigned i;
  for (i = 0; i < 8; ++i) {
    w[i] = MIN(SHRT_MAX, MAX(SHRT_MIN, (c[i * 2 + 0] * b[i * 2 + 0] +
                                        c[i * 2 + 1] * b[i * 2 + 1])));
  }
}
