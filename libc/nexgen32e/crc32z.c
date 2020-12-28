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
#include "libc/bits/safemacros.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/nexgen32e/x86feature.h"

static uint32_t kCrc32Tab[256];

/**
 * Computes Phil Katz CRC-32 used by zip/zlib/gzip/etc.
 *
 *     x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x+1
 *     0b100000100110000010001110110110111
 *     bitreverse32(0x104c11db7)
 *
 * @param h is initial value
 */
uint32_t crc32_z(uint32_t h, const void *data, size_t size) {
  const unsigned char *p, *pe;
  static bool once;
  size_t skip;
  if (!once) {
    crc32init(kCrc32Tab, 0xedb88320);
    once = true;
  }
  if (data) {
    h ^= 0xffffffff;
    if (size >= 64 && X86_HAVE(PCLMUL)) {
      h = crc32$pclmul(h, data, size); /* 51x faster */
      skip = rounddown(size, 16);
    } else {
      skip = 0;
    }
    p = (const unsigned char *)data + skip;
    pe = (const unsigned char *)data + size;
    while (p < pe) {
      h = h >> 8 ^ kCrc32Tab[(h & 0xff) ^ *p++];
    }
    return h ^ 0xffffffff;
  } else {
    return 0;
  }
}
