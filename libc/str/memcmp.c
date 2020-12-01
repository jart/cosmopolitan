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
#include "libc/intrin/pcmpeqb.h"
#include "libc/intrin/pmovmskb.h"
#include "libc/nexgen32e/bsf.h"
#include "libc/str/str.h"

/**
 * Compares memory.
 *
 * @return unsigned char subtraction at stop index
 * @asyncsignalsafe
 */
int memcmp(const void *a, const void *b, size_t n) {
  int c;
  size_t i;
  unsigned m;
  uint8_t *p1, *p2;
  uint8_t v1[16], v2[16];
  if (n) {
    for (p1 = a, p2 = b, i = 0; i + 16 <= n; i += 16) {
      memcpy(v1, p1 + i, 16);
      memcpy(v2, p2 + i, 16);
      pcmpeqb(v1, v1, v2);
      if ((m = pmovmskb(v1) - 0xffff)) {
        i += bsf(m);
        return p1[i] - p2[i];
      }
    }
    for (; i < n; ++i) {
      if ((c = p1[i] - p2[i])) {
        return c;
      }
    }
  }
  return 0;
}
