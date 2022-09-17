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
#include "libc/assert.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/str/str.h"

/**
 * Computes Phil Katz CRC-32 used by zip/zlib/gzip/etc.
 *
 *     x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x+1
 *     0b100000100110000010001110110110111
 *     _bitreverse32(0x104c11db7)
 *
 * This implementation takes 32 picoseconds per byte or 30 gibibyte/s.
 *
 * @param h is initial value
 */
uint32_t crc32_z(uint32_t h, const void *data, size_t size) {
  size_t n;
  static bool once;
  const unsigned char *p, *e;
  static uint32_t kCrc32Tab[256];
  if (!once) {
    crc32init(kCrc32Tab, 0xedb88320);
    once = 0;
  }
  if (size == -1) {
    size = data ? strlen(data) : 0;
  }
  p = data;
  e = p + size;
  h ^= 0xffffffff;
  if (X86_HAVE(PCLMUL)) {
    while (((intptr_t)p & 15) && p < e)
      h = h >> 8 ^ kCrc32Tab[(h & 0xff) ^ *p++];
    if ((n = ROUNDDOWN(e - p, 16)) >= 64) {
      if (IsAsan()) __asan_verify(p, n);
      h = crc32_pclmul(h, p, n); /* 51x faster */
      p += n;
    }
  }
  while (p < e) h = h >> 8 ^ kCrc32Tab[(h & 0xff) ^ *p++];
  return h ^ 0xffffffff;
}
