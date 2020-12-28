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
