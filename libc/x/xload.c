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
#include "libc/intrin/atomic.h"
#include "libc/mem/mem.h"
#include "libc/runtime/internal.h"
#include "libc/x/x.h"
#include "third_party/zlib/zlib.h"

/**
 * Inflates data once atomically, e.g.
 *
 *     void *GetData(void) {
 *       static _Atomic(void *) ptr;
 *       static const unsigned char rodata[] = {...};
 *       return xload(&ptr, rodata, 112, 1024);
 *     }
 *
 * The above is an example of how this helper may be used to have lazy
 * loading of big infrequently accessed image data.
 *
 * @param a points to your static pointer holder
 * @param p is read-only data compressed using raw deflate
 * @param n is byte length of deflated data
 * @param m is byte length of inflated data
 * @return pointer to inflated data
 */
void *xload(_Atomic(void *) *a, const void *p, size_t n, size_t m) {
  void *r, *z;
  if ((r = atomic_load_explicit(a, memory_order_acquire))) return r;
  if (!(r = malloc(m))) return 0;
  if (__inflate(r, m, p, n)) return 0;
  z = 0;
  if (!atomic_compare_exchange_strong_explicit(a, &z, r, memory_order_release,
                                               memory_order_relaxed)) {
    r = z;
  }
  return r;
}
