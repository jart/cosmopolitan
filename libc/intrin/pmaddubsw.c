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
#include "libc/intrin/pmaddubsw.h"
#include "libc/limits.h"
#include "libc/macros.h"
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
