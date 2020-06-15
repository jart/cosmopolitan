
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
#include "dsp/tty/tty.h"
#include "libc/alg/alg.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/str/str.h"

static int histcmp(const uint8_t *i1p, const uint8_t *i2p,
                   uint32_t histogram[hasatleast 256]) {
  if (histogram[*i1p] > histogram[*i2p]) {
    return -1;
  } else if (histogram[*i1p] < histogram[*i2p]) {
    return 1;
  } else {
    return 0;
  }
}

void ttyhisto(uint32_t histogram[hasatleast 256],
              uint8_t dominant[hasatleast 256], const uint8_t *xtcolors,
              const uint8_t *eqmask, size_t size) {
  size_t i;
  uint64_t q;
  memset(histogram, 0, sizeof(uint32_t) * 256);
  for (i = 0; i < size / 8; ++i) {
    memcpy(&q, &xtcolors[i * 8], 8);
    if (q == (xtcolors[i * 8] & 0xff) * 0x0101010101010101ul) {
      histogram[xtcolors[i * 8]]++;
    }
  }
  imapxlatab(dominant);
  qsort_r(dominant, 256, 1, (void *)histcmp, histogram);
}
