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
#include "libc/assert.h"
#include "libc/intrin/pcmpeqb.h"
#include "libc/intrin/pmovmskb.h"
#include "libc/nexgen32e/bsf.h"
#include "libc/str/str.h"

/**
 * Returns length of NUL-terminated string.
 *
 * @param s is non-null NUL-terminated string pointer
 * @return number of bytes (excluding NUL)
 * @asyncsignalsafe
 */
size_t strlen(const char *s) {
  const char *p;
  unsigned k, m;
  uint8_t v1[16], vz[16];
  k = (uintptr_t)s & 15;
  p = (const char *)((uintptr_t)s & -16);
  memset(vz, 0, 16);
  memcpy(v1, p, 16);
  pcmpeqb(v1, v1, vz);
  m = pmovmskb(v1) >> k << k;
  while (!m) {
    p += 16;
    memcpy(v1, p, 16);
    pcmpeqb(v1, v1, vz);
    m = pmovmskb(v1);
  }
  return p + bsf(m) - s;
}
