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
#include "libc/rand/rand.h"
#include "libc/rand/xorshift.h"
#include "libc/str/str.h"

/**
 * Fills memory with random bytes, e.g.
 *
 *   char buf[1024];
 *   rngset(buf, sizeof(buf), rand64, -1);
 *
 * @param seed can be rand64() and is always called at least once
 * @param reseed is bytes between seed() calls and -1 disables it
 * @return original buf
 */
void *rngset(void *buf, size_t size, uint64_t seed(void), size_t reseed) {
  unsigned char *p;
  uint64_t i, x, state;
  p = buf;
  state = seed();
  for (i = 0; size - i >= sizeof(x); i += sizeof(x)) {
    x = MarsagliaXorshift64(&state);
    memcpy(p + i, &x, sizeof(x));
    if (i >= reseed) {
      state = seed();
      p += i;
      size -= i;
      i = 0;
    }
  }
  if (i < size) {
    x = MarsagliaXorshift64(&state);
    for (; i < size; ++i, x >>= 8) {
      p[i] = x & 0xff;
    }
  }
  return buf;
}
