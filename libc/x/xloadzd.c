/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/assert.h"
#include "libc/fmt/leb128.h"
#include "libc/intrin/lockcmpxchg.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/x/x.h"
#include "third_party/zlib/zlib.h"

/**
 * Loads δzd encoded data once atomically.
 *
 * @param o points to your static init guard
 * @param t points to your static pointer holder
 * @param p is read-only data compressed using raw deflate
 * @param n is byte length of deflated data
 * @param m is byte length of inflated data
 * @param c is number of items in array
 * @param z is byte length of items
 * @param s is crc32 checksum
 * @return pointer to decoded data
 * @threadsafe
 */
void *xloadzd(bool *o, void **t, const void *p, size_t n, size_t m, size_t c,
              size_t z, uint32_t s) {
  void *r;
  size_t i;
  z_stream zs;
  char *q, *b;
  int64_t x, y;
  assert(z == 2 || z == 4);
  b = q = malloc(m);
  __inflate(q, m, p, n);
  r = memalign(z, c * z);
  for (x = i = 0; i < c; ++i) {
    b += unzleb64(b, 10, &y);
    x += y;
    if (z == 2) {
      ((uint16_t *)r)[i] = x;
    } else {
      ((uint32_t *)r)[i] = x;
    }
  }
  free(q);
  assert(crc32_z(0, r, c * z) == s);
  if (_lockcmpxchg(t, 0, r)) {
    __cxa_atexit(free, r, 0);
  } else {
    free(q);
  }
  *o = true;
  return *t;
}
