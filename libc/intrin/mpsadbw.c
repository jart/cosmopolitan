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
#include "libc/intrin/mpsadbw.h"
#include "libc/macros.h"
#include "libc/str/str.h"

/**
 * Computes multiple sum of absolute differences.
 *
 * This appears to be intended for video encoding motion estimation. It
 * can be combined with phminposuw. That allows us to search for an int
 * overlapping inside 𝑏 that's nearest to an aligned int in 𝑎.
 *
 * @note goes fast w/ sse4 cf. core c. 2006 cf. bulldozer c. 2011
 * @mayalias
 */
void(mpsadbw)(uint16_t c[8], const uint8_t b[16], const uint8_t a[16],
              uint8_t control) {
  unsigned i, j;
  uint16_t r[8];
  for (i = 0; i < 8; ++i) {
    r[i] = 0;
    for (j = 0; j < 4; ++j) {
      r[i] += ABS(b[(control & 4) + i + j] - a[(control & 3) * 4 + j]);
    }
  }
  memcpy(c, r, 16);
}
