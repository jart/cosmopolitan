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
#include "libc/intrin/lockcmpxchg.h"
#include "libc/log/check.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/x/x.h"
#include "third_party/zlib/zlib.h"

/**
 * Inflates data once atomically, e.g.
 *
 *     void *GetData(void) {
 *       static char once;
 *       static void *ptr;
 *       static const unsigned char rodata[] = {...};
 *       if (once) return ptr;
 *       return xload(&once, &ptr, rodata, 112, 1024);
 *     }
 *
 * The above is an example of how this helper may be used to have lazy
 * loading of big infrequently accessed image data.
 *
 * @param o points to your static init guard
 * @param t points to your static pointer holder
 * @param p is read-only data compressed using raw deflate
 * @param n is byte length of deflated data
 * @param m is byte length of inflated data
 * @return pointer to inflated data
 * @threadsafe
 */
void *xload(bool *o, void **t, const void *p, size_t n, size_t m) {
  void *q;
  z_stream zs;
  q = malloc(m);
  zs.zfree = 0;
  zs.zalloc = 0;
  zs.next_in = p;
  zs.avail_in = n;
  zs.total_in = n;
  zs.avail_out = m;
  zs.total_out = m;
  zs.next_out = (void *)q;
  inflateInit2(&zs, -MAX_WBITS);
  inflate(&zs, Z_NO_FLUSH);
  if (_lockcmpxchg(t, 0, q)) {
    __cxa_atexit(free, q, 0);
  } else {
    free(q);
  }
  *o = true;
  return *t;
}
