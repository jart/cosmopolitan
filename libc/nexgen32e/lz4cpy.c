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
#include "libc/bits/bits.h"
#include "libc/bits/pushpop.h"
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
  unsigned char *op, *ip, *ipe, *match;
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
    repmovsb(&op, &ip, length);
    if (ip >= ipe) break;
    offset = read16le(ip);
    matchlen = token & fifteen;
    ip += 2;
    if (matchlen == fifteen) {
      do {
        matchlen += *ip;
      } while (*ip++ == 255);
    }
    match = op - offset;
    repmovsb(&op, &match, (matchlen += 4));
  }
  return op;
}
