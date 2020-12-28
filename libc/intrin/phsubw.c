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
#include "libc/intrin/phsubw.h"
#include "libc/str/str.h"

/**
 * Subtracts adjacent 16-bit integers.
 *
 * @param 𝑎 [w/o] receives reduced 𝑏 and 𝑐 concatenated
 * @param 𝑏 [r/o] supplies four pairs of shorts
 * @param 𝑐 [r/o] supplies four pairs of shorts
 * @note goes fast w/ ssse3 (intel c. 2004, amd c. 2011)
 * @mayalias
 */
void(phsubw)(int16_t a[8], const int16_t b[8], const int16_t c[8]) {
  int16_t t[8];
  t[0] = b[0] - b[1];
  t[1] = b[2] - b[3];
  t[2] = b[4] - b[5];
  t[3] = b[6] - b[7];
  t[4] = c[0] - c[1];
  t[5] = c[2] - c[3];
  t[6] = c[4] - c[5];
  t[7] = c[6] - c[7];
  memcpy(a, t, sizeof(t));
}
