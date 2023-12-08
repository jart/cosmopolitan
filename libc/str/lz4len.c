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
#include "libc/intrin/pushpop.internal.h"
#include "libc/nexgen32e/kompressor.h"
#include "libc/str/str.h"

/**
 * Returns the uncompressed content size for a compressed LZ4 block, without
 * actually decompressing it.
 *
 * @see lz4cpy()
 */
size_t lz4len(const void *blockdata, size_t blocksize) {
  const unsigned char *ip, *ipe;
  unsigned token, length, fifteen, matchlen;
  size_t unpacklen = 0;
  for (ip = blockdata, ipe = ip + blocksize;;) {
    token = *ip++;
    length = token >> 4;
    fifteen = pushpop(15);
    if (length == fifteen) {
      do {
        length += *ip;
      } while (*ip++ == 255);
    }
    ip += length;
    unpacklen += length;
    if (ip >= ipe) break;
    matchlen = token & fifteen;
    ip += 2;
    if (matchlen == fifteen) {
      do {
        matchlen += *ip;
      } while (*ip++ == 255);
    }
    unpacklen += matchlen + 4;
  }
  return unpacklen;
}
