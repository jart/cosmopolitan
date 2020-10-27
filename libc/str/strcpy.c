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
#include "libc/limits.h"
#include "libc/nexgen32e/bsf.h"
#include "libc/str/str.h"

/**
 * Copies bytes from 𝑠 to 𝑑 until a NUL is encountered.
 *
 * @param 𝑑 is destination memory
 * @param 𝑠 is a NUL-terminated string
 * @note 𝑑 and 𝑠 can't overlap
 * @return original dest
 * @see memccpy()
 * @asyncsignalsafe
 */
char *strcpy(char *d, const char *s) {
  size_t i;
  uint8_t v1[16], v2[16], vz[16];
  i = 0;
  while (((uintptr_t)(s + i) & 15)) {
    if (!(d[i] = s[i])) {
      return d;
    }
    ++i;
  }
  for (;;) {
    memset(vz, 0, 16);
    memcpy(v1, s + i, 16);
    pcmpeqb(v2, v1, vz);
    if (!pmovmskb(v2)) {
      memcpy(d + i, v1, 16);
      i += 16;
    } else {
      break;
    }
  }
  for (;;) {
    if (!(d[i] = s[i])) {
      return d;
    }
    ++i;
  }
}
