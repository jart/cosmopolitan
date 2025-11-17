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
#include "libc/dce.h"
#include "libc/fmt/conv.h"
#include "libc/str/str.h"
#include "libc/str/utf16.h"
#include "third_party/aarch64/arm_neon.internal.h"
#include "third_party/intel/emmintrin.internal.h"

#if !IsModeDbg()
#if defined(__x86_64__)

static axdx_t tprecode16to8_sse2(char *dst, size_t dstsize, const char16_t *src,
                                 axdx_t r) {
  __m128i v1, v2, v3, vz;
  vz = _mm_setzero_si128();
  while (r.ax + 8 < dstsize) {
    v1 = _mm_loadu_si128((__m128i *)(src + r.dx));
    v2 = _mm_cmpgt_epi16(v1, vz);
    v3 = _mm_cmpgt_epi16(v1, _mm_set1_epi16(0x7F));
    v2 = _mm_andnot_si128(v3, v2);
    if (_mm_movemask_epi8(v2) != 0xFFFF)
      break;
    v1 = _mm_packs_epi16(v1, v1);
    _mm_storel_epi64((__m128i *)(dst + r.ax), v1);
    r.ax += 8;
    r.dx += 8;
  }
  return r;
}

#elif defined(__aarch64__)

static axdx_t tprecode16to8_neon(char *dst, size_t dstsize, const char16_t *src,
                                 axdx_t r) {
  uint16x8_t v1, v2, v3;
  while (r.ax + 8 < dstsize) {
    v1 = vld1q_u16((const uint16_t *)(src + r.dx));
    v2 = vcgtq_u16(v1, vdupq_n_u16(0));
    v3 = vcgtq_u16(v1, vdupq_n_u16(0x7F));
    v2 = vbicq_u16(v2, v3);
    if (vaddvq_u16(v2) != 8 * 0xFFFF)
      break;
    vst1_u8((uint8_t *)(dst + r.ax), vqmovn_u16(v1));
    r.ax += 8;
    r.dx += 8;
  }
  return r;
}

#endif
#endif

/**
 * Transcodes UTF-16 to UTF-8.
 *
 * This is a low-level function intended for the core runtime. Use
 * utf16to8() for a much better API that uses malloc().
 *
 * @param dst is output buffer
 * @param dstsize is bytes in dst
 * @param src is NUL-terminated UTF-16 input string
 * @return ax bytes written excluding nul
 * @return dx index of character after nul word in src
 */
axdx_t tprecode16to8(char *dst, size_t dstsize, const char16_t *src) {
  axdx_t r;
  uint64_t w;
  wint_t x, y;
  r.ax = 0;
  r.dx = 0;
  for (;;) {
#if !IsModeDbg()
#if defined(__x86_64__)
    if (!((uintptr_t)(src + r.dx) & 15))
      r = tprecode16to8_sse2(dst, dstsize, src, r);
#elif defined(__aarch64__)
    if (!((uintptr_t)(src + r.dx) & 15))
      r = tprecode16to8_neon(dst, dstsize, src, r);
#endif
#endif
    if (!(x = src[r.dx++]))
      break;
    if (IsUtf16Cont(x))
      continue;
    if (!IsUcs2(x)) {
      if (!(y = src[r.dx++]))
        break;
      x = MergeUtf16(x, y);
    }
    w = tpenc(x);
    while (w && r.ax + 1 < dstsize) {
      dst[r.ax++] = w & 0xFF;
      w >>= 8;
    }
  }
  if (r.ax < dstsize) {
    dst[r.ax] = 0;
  }
  return r;
}
