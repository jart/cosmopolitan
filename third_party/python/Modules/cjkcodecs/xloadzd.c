/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/intrin/atomic.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/x/x.h"
#include "third_party/zlib/zlib.h"

/**
 * Loads δzd encoded data once atomically.
 *
 * @param a points to your static pointer holder
 * @param p is read-only data compressed using raw deflate
 * @param n is byte length of deflated data
 * @param m is byte length of inflated data
 * @param c is number of items in array
 * @param z is byte length of items
 * @param s is crc32 checksum
 * @return pointer to decoded data
 * @threadsafe
 */
void *xloadzd(_Atomic(void *) *a, const void *p, size_t n, size_t m, size_t c,
              size_t z, uint32_t s) {
  size_t i;
  char *q, *b;
  void *r, *g;
  int64_t x, y;
  if ((r = atomic_load_explicit(a, memory_order_acquire))) return r;
  unassert(z == 2 || z == 4);
  if (!(b = q = malloc(m))) return 0;
  if (__inflate(q, m, p, n)) {
    free(q);
    return 0;
  }
  if (!(r = malloc(c * z))) {
    free(q);
    return 0;
  }
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
  g = 0;
  if (!atomic_compare_exchange_strong_explicit(a, &g, r, memory_order_relaxed,
                                               memory_order_relaxed)) {
    r = g;
  }
  return r;
}
