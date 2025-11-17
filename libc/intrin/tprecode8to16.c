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
#include <arm_neon.h>
#include <stdint.h>
#include <string.h>
#include "libc/dce.h"
#include "libc/str/str.h"
#include "libc/str/thompike.h"
#include "libc/str/utf16.h"
#include "third_party/aarch64/arm_neon.internal.h"
#include "third_party/intel/emmintrin.internal.h"

#if !IsModeDbg()
#if defined(__x86_64__)

static inline axdx_t tprecode8to16_sse2(char16_t *dst, size_t dstsize,
                                        const char *src, axdx_t r) {
  __m128i v1, v2, vz;
  vz = _mm_setzero_si128();
  while (r.ax + 16 < dstsize) {
    v1 = _mm_loadu_si128((__m128i *)(src + r.dx));
    v2 = _mm_cmpgt_epi8(v1, vz);
    if (_mm_movemask_epi8(v2) != 0xFFFF)
      break;
    __m128i lo = _mm_unpacklo_epi8(v1, vz);
    __m128i hi = _mm_unpackhi_epi8(v1, vz);
    _mm_storeu_si128((__m128i *)(dst + r.ax), lo);
    _mm_storeu_si128((__m128i *)(dst + r.ax + 8), hi);
    r.ax += 16;
    r.dx += 16;
  }
  return r;
}

#elif defined(__aarch64__)

static inline axdx_t tprecode8to16_neon(char16_t *dst, size_t dstsize,
                                        const char *src, axdx_t r) {
  uint8x16_t v1;
  while (r.ax + 16 < dstsize) {
    v1 = vld1q_u8((const uint8_t *)(src + r.dx));
    uint8x16_t cmp = vcgtq_u8(v1, vdupq_n_u8(0));
    if (vaddvq_u8(cmp) != 16 * 0xFF)
      break;
    uint16x8_t lo = vmovl_u8(vget_low_u8(v1));
    uint16x8_t hi = vmovl_u8(vget_high_u8(v1));
    vst1q_u16((uint16_t *)(dst + r.ax), lo);
    vst1q_u16((uint16_t *)(dst + r.ax + 8), hi);
    r.ax += 16;
    r.dx += 16;
  }
  return r;
}

#endif
#endif

/**
 * Transcodes UTF-8 to UTF-16.
 *
 * This is a low-level function intended for the core runtime. Use
 * utf8to16() for a much better API that uses malloc().
 *
 * @param dst is output buffer
 * @param dstsize is shorts in dst
 * @param src is NUL-terminated UTF-8 input string
 * @return ax shorts written excluding nul
 * @return dx index of character after nul word in src
 * @asyncsignalsafe
 */
axdx_t tprecode8to16(char16_t *dst, size_t dstsize, const char *src) {
  axdx_t r;
  unsigned w;
  int x, a, b, i, n;
  r.ax = 0;
  r.dx = 0;
  for (;;) {
#if !IsModeDbg()
#if defined(__x86_64__)
    if (!((uintptr_t)(src + r.dx) & 15))
      r = tprecode8to16_sse2(dst, dstsize, src, r);
#elif defined(__aarch64__)
    if (!((uintptr_t)(src + r.dx) & 15))
      r = tprecode8to16_neon(dst, dstsize, src, r);
#endif
#endif
    x = src[r.dx++] & 0377;
    if (x >= 0300) {
      a = ThomPikeByte(x);
      n = ThomPikeLen(x) - 1;
      for (i = 0;;) {
        if (!(b = src[r.dx + i] & 0377))
          break;
        if (!ThomPikeCont(b))
          break;
        a = ThomPikeMerge(a, b);
        if (++i == n) {
          r.dx += i;
          x = a;
          break;
        }
      }
    }
    if (!x)
      break;
    w = EncodeUtf16(x);
    while (w && r.ax + 1 < dstsize) {
      dst[r.ax++] = w;
      w >>= 16;
    }
  }
  if (r.ax < dstsize) {
    dst[r.ax] = 0;
  }
  return r;
}
