/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/fmt/leb128.h"

/**
 * Encodes unsigned integer to array.
 *
 * This function is the reverse operation of unuleb64().
 *
 * The maximum number of bytes that'll be serialized to `p` is 9. The
 * ninth byte is special. Normally with LEB encoding, the highest bit of
 * each byte indicates if we should consume additional bytes. That means
 * each byte has 7 bits of content, and 8*7=56 which means by the time
 * we hit the ninth byte, there's exactly 8 bits of content remaining in
 * the 64-bit integer we're decoding. So we don't use LEB on the ninth
 * byte and treat all eight bits of it as content.
 *
 * @param p is output array
 * @param x is number
 * @return p + i
 */
char *uleb64(char p[hasatleast 9], uint64_t x) {
  int c;
#pragma GCC unroll 1000
  for (int i = 0; i < 8; ++i) {
    c = x & 127;
    if (!(x >>= 7)) {
      *p++ = c;
      return p;
    } else {
      *p++ = c | 128;
    }
  }
  *p++ = x;
  return p;
}
