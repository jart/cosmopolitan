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
#include "libc/crypto/rijndael.h"
#include "libc/dce.h"

static noinline aes_block_t xtime(aes_block_t x) {
  return ((x ^ (x & 0x80808080)) << 1) ^ (((x & 0x80808080) >> 7) * 0x1b);
}

static aes_block_t gf256mulx4(aes_block_t x, aes_block_t c) {
  return ((((c >> 0 & 0x01010101) * 0xff) & x) ^
          (((c >> 1 & 0x01010101) * 0xff) & xtime(x)) ^
          (((c >> 2 & 0x01010101) * 0xff) & xtime(xtime(x))) ^
          (((c >> 3 & 0x01010101) * 0xff) & xtime(xtime(xtime(x)))) ^
          (((c >> 4 & 0x01010101) * 0xff) & xtime(xtime(xtime(x)))));
}

/**
 * Applies inverse of Rijndael MixColumns() transformation.
 * @see FIPS-197
 */
aes_block_t InvMixColumns(aes_block_t x) {
  uint32_t i;
  aes_block_t y = {0, 0, 0, 0};
  aes_block_t c = {0x090D0B0E, 0x090D0B0E, 0x090D0B0E, 0x090D0B0E};
  for (i = 0; i < 4; ++i) {
    y ^= gf256mulx4((x & 0xff) * 0x01010101,
                    (((c >> 000) & 0xff) << 000 | ((c >> 010) & 0xff) << 030 |
                     ((c >> 020) & 0xff) << 020 | ((c >> 030) & 0xff) << 010));
    x = x << 8 | x >> 24;
    c = c << 8 | c >> 24;
  }
  return y;
}
