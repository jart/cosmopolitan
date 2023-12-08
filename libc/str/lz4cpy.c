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
#include "libc/serialize.h"
#include "libc/intrin/pushpop.internal.h"
#include "libc/intrin/repmovsb.h"
#include "libc/nexgen32e/kompressor.h"
#include "libc/str/str.h"

/**
 * Decompresses LZ4 block.
 *
 * This is a 103 byte implementation of the LZ4 algorithm. Please note
 * LZ4 files are comprised of multiple frames, which may be decoded
 * together using the wrapper function lz4decode().
 *
 * @see rldecode() for a 16-byte decompressor
 */
textstartup void *lz4cpy(void *dest, const void *blockdata, size_t blocksize) {
  unsigned char *op, *match;
  const unsigned char *ip, *ipe;
  unsigned token, length, fifteen, offset, matchlen;
  for (op = dest, ip = blockdata, ipe = ip + blocksize;;) {
    token = *ip++;
    length = token >> 4;
    fifteen = pushpop(15);
    if (length == fifteen) {
      do {
        length += *ip;
      } while (*ip++ == 255);
    }
    repmovsb((void **)&op, (const void **)&ip, length);
    if (ip >= ipe) break;
    offset = READ16LE(ip);
    matchlen = token & fifteen;
    ip += 2;
    if (matchlen == fifteen) {
      do {
        matchlen += *ip;
      } while (*ip++ == 255);
    }
    match = op - offset;
    repmovsb((void **)&op, (const void **)&match, (matchlen += 4));
  }
  return op;
}
