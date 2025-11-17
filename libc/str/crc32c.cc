/*-*-mode:c++;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8-*-│
│ vi: set et ft=c++ ts=2 sts=2 sw=2 fenc=utf-8                             :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2025 Justine Alexandra Roberts Tunney                              │
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
#include "libc/runtime/runtime.h"
#include "libc/serialize.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/hwcap.h"
#include "third_party/aarch64/arm_acle.internal.h"
#include "third_party/aarch64/arm_neon.internal.h"
#include "third_party/intel/immintrin.internal.h"

#define POLYNOMIAL 0x82f63b78u

constexpr uint32_t shift(int bits) {
  uint32_t crc = 1;
  for (int i = 0; i < bits; i++) {
    if (crc & 1) {
      crc = (crc >> 1) ^ POLYNOMIAL;
    } else {
      crc = crc >> 1;
    }
  }
  return crc;
}

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

  static struct {
    bool once;
    bool have;
    bool pmul;
    uint32_t tab[256];
  } crc32;
  if (!crc32.once) {
#if defined(__aarch64__)
    long hwcap = getauxval(AT_HWCAP);
    crc32.have = !!(hwcap & HWCAP_CRC32);
    crc32.pmul = !!(hwcap & HWCAP_PMULL);
#elif defined(__x86_64__) && !defined(__chibicc__)
    crc32.have = X86_HAVE(SSE4_2);
    crc32.pmul = X86_HAVE(PCLMUL);
#endif
    if (!crc32.have)
      crc32init(crc32.tab, POLYNOMIAL);
    crc32.once = 1;
  }

  size_t n = size;
  uint64_t h = ~init;
  const unsigned char *p = (const unsigned char *)data;

  if (!crc32.have)
    goto fallback;

#if defined(__aarch64__)
  if (crc32.pmul) {
    while (n >= 256 * 4 + 8) {
      uint32_t a = h;
      uint32_t b = 0;
      uint32_t c = 0;
      uint32_t d = 0;
      for (int i = 0; i < 32; ++i) {
        a = __crc32cd(a, READ64LE(p + i * 8 + 256 * 0));
        b = __crc32cd(b, READ64LE(p + i * 8 + 256 * 1));
        c = __crc32cd(c, READ64LE(p + i * 8 + 256 * 2));
        d = __crc32cd(d, READ64LE(p + i * 8 + 256 * 3));
      }
      h = __crc32cd(d, READ64LE(p + 256 * 4));
      h ^= __crc32cd(0, vmull_p64(c, shift(256 * 1 * 8)));
      h ^= __crc32cd(0, vmull_p64(b, shift(256 * 2 * 8)));
      h ^= __crc32cd(0, vmull_p64(a, shift(256 * 3 * 8)));
      p += 256 * 4 + 8;
      n -= 256 * 4 + 8;
    }
  }
  while (n >= 8) {
    h = __crc32cd(h, READ64LE(p));
    p += 8;
    n -= 8;
  }
  if (n & 4) {
    h = __crc32cw(h, READ32LE(p));
    p += 4;
  }
  if (n & 2) {
    h = __crc32ch(h, READ16LE(p));
    p += 2;
  }
  if (n & 1)
    h = __crc32cb(h, *p);
  return ~h;

#elif defined(__x86_64__) && !defined(__chibicc__)
  if (crc32.pmul) {
    while (n >= 256 * 4 + 8) {
      uint32_t a = h;
      uint32_t b = 0;
      uint32_t c = 0;
      uint32_t d = 0;
      for (int i = 0; i < 32; ++i) {
        a = _mm_crc32_u64(a, READ64LE(p + i * 8 + 256 * 0));
        b = _mm_crc32_u64(b, READ64LE(p + i * 8 + 256 * 1));
        c = _mm_crc32_u64(c, READ64LE(p + i * 8 + 256 * 2));
        d = _mm_crc32_u64(d, READ64LE(p + i * 8 + 256 * 3));
      }
      h = _mm_crc32_u64(d, READ64LE(p + 256 * 4));
      h ^= _mm_crc32_u64(
          0, _mm_extract_epi64(_mm_clmulepi64_si128(
                                   _mm_set_epi64x(0, c),
                                   _mm_set_epi64x(0, shift(256 * 1 * 8)), 0x00),
                               0));
      h ^= _mm_crc32_u64(
          0, _mm_extract_epi64(_mm_clmulepi64_si128(
                                   _mm_set_epi64x(0, b),
                                   _mm_set_epi64x(0, shift(256 * 2 * 8)), 0x00),
                               0));
      h ^= _mm_crc32_u64(
          0, _mm_extract_epi64(_mm_clmulepi64_si128(
                                   _mm_set_epi64x(0, a),
                                   _mm_set_epi64x(0, shift(256 * 3 * 8)), 0x00),
                               0));
      p += 256 * 4 + 8;
      n -= 256 * 4 + 8;
    }
  }
  while (n >= 8) {
    h = _mm_crc32_u64(h, READ64LE(p));
    p += 8;
    n -= 8;
  }
  if (n & 4) {
    h = _mm_crc32_u32(h, READ32LE(p));
    p += 4;
  }
  if (n & 2) {
    h = _mm_crc32_u16(h, READ16LE(p));
    p += 2;
  }
  if (n & 1)
    h = _mm_crc32_u8(h, *p);
  return ~h;

#endif

fallback:
  for (size_t i = 0; i < n; ++i)
    h = h >> 8 ^ crc32.tab[(h & 0xff) ^ p[i]];
  return ~h;
}
