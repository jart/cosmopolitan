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
#include "libc/nexgen32e/crc32.h"
#include "libc/nexgen32e/x86feature.h"

/**
 * Computes 32-bit Castagnoli Cyclic Redundancy Check.
 *
 *     x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x+1
 *     0b00011110110111000110111101000001
 *
 * @param init is the initial hash value
 * @param data points to the data
 * @param size is the byte size of data
 * @return eax is the new hash value
 * @note Used by ISCSI, TensorFlow, etc.
 */
uint32_t crc32c(uint32_t init, const void *data, size_t size) {
  uint64_t h;
  static bool once;
  const unsigned char *p, *pe;
  static uint32_t kCrc32cTab[256];
  if (!once) {
    crc32init(kCrc32cTab, 0x82f63b78);
    once = 1;
  }
  p = data;
  pe = p + size;
  h = init ^ 0xffffffff;
  if (X86_HAVE(SSE4_2)) {
    while (p < pe && ((intptr_t)p & 7)) {
      h = h >> 8 ^ kCrc32cTab[(h & 0xff) ^ *p++];
    }
    for (; p + 8 <= pe; p += 8) {
      asm("crc32q\t%1,%0" : "+r"(h) : "rm"(*(const uint64_t *)p));
    }
    while (p < pe) {
      asm("crc32b\t%1,%0" : "+r"(h) : "rm"(*p++));
    }
  } else {
    while (p < pe) {
      h = h >> 8 ^ kCrc32cTab[(h & 0xff) ^ *p++];
    }
  }
  return h ^ 0xffffffff;
}
