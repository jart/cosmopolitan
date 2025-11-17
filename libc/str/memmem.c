/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/dce.h"
#include "libc/intrin/likely.h"
#include "libc/mem/alloca.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/runtime/runtime.h"
#include "libc/serialize.h"
#include "libc/str/internal.h"
#include "libc/str/str.h"
#include "third_party/aarch64/arm_neon.internal.h"
#include "third_party/intel/immintrin.internal.h"

#if defined(__x86_64__) && !defined(__chibicc__)
#pragma GCC push_options
#pragma GCC target("avx2")
static bool memmem_avx2(const unsigned char **h, const unsigned char *e,
                        const unsigned char *n, size_t l, size_t *j) {
  const unsigned char *p = *h;
  __m256i nv = _mm256_set1_epi8(n[0]);
  __m256i sv = _mm256_set1_epi8(n[1]);
  do {
    unsigned m = _mm256_movemask_epi8(_mm256_min_epu8(
        _mm256_cmpeq_epi8(_mm256_loadu_si256((const __m256i *)p), nv),
        _mm256_cmpeq_epi8(_mm256_loadu_si256((const __m256i *)(p + 1)), sv)));
    if (m) {
      p += __builtin_ctz(m);
      if (l == 2) {
        *h = p;
        return true;
      }
      p += 2;
      *j += 2;
      break;
    }
    p += 32;
  } while (e - p >= 33);
  *h = p;
  return false;
}
#pragma GCC pop_options
dontinline relegated static bool memmem_sse2(const unsigned char **h,
                                             const unsigned char *e,
                                             const unsigned char *n, size_t l,
                                             size_t *j) {
  const unsigned char *p = *h;
  __m128i nv = _mm_set1_epi8(n[0]);
  __m128i sv = _mm_set1_epi8(n[1]);
  do {
    unsigned m = _mm_movemask_epi8(
        _mm_min_epu8(_mm_cmpeq_epi8(_mm_loadu_si128((__m128i *)p), nv),
                     _mm_cmpeq_epi8(_mm_loadu_si128((__m128i *)(p + 1)), sv)));
    if (m) {
      p += __builtin_ctz(m);
      if (l == 2) {
        *h = p;
        return true;
      }
      p += 2;
      *j += 2;
      break;
    }
    p += 16;
  } while (e - p >= 17);
  *h = p;
  return false;
}
#endif  // __x86_64__

/**
 * Searches for fixed-length substring in memory region.
 *
 * @param haystack is the region of memory to be searched
 * @param haystacklen is its character count
 * @param needle contains the memory for which we're searching
 * @param needlelen is its character count
 * @return pointer to first result or NULL if not found
 * @asyncsignalsafe
 */
void *memmem(const void *haystack, size_t haystacklen,  //
             const void *needle, size_t needlelen) {

  // rename arguments
  const unsigned char *h = (const unsigned char *)haystack;
  const unsigned char *n = (const unsigned char *)needle;
  size_t k = haystacklen;
  size_t l = needlelen;

  // handle special cases
  if (!l)
    return (void *)h;
  if (k < l)
    return 0;
  if (l == 1)
    return memchr(h, *n, k);

  // use 2x slower algorithm if needle might cause stack overflow
  size_t need = sizeof(unsigned) * l;
#ifdef MODE_DBG
  if (need > 23)
#else
  if (need > 4000)
#endif
    return __memmem2way(h, h + k, n, l);

  // compute longest prefix suffix array
  size_t i = 1;
  size_t len = 0;
  unsigned *lps = (unsigned *)alloca(need);
  lps[0] = 0;
  while (i < l) {
    if (n[i] == n[len]) {
      lps[i++] = ++len;
    } else if (len) {
      len = lps[len - 1];
    } else {
      lps[i++] = 0;
    }
  }

  // generic search
  size_t j = 0;
  const unsigned char *e = h + k;
  for (;;) {

    // simd accelerate search of first two bytes in needle
#if defined(__x86_64__) && !defined(__chibicc__)
    if (!j && e - h >= 33) {
      if (X86_HAVE(AVX2) && !IsModeDbg()) {
        if (memmem_avx2(&h, e, n, l, &j))
          return (char *)h;
      } else {
        if (memmem_sse2(&h, e, n, l, &j))
          return (char *)h;
      }
    }
#elif defined(__aarch64__) && defined(__ARM_NEON)
    if (!j && e - h >= 17) {
      uint8x16_t nv = vdupq_n_u8(n[0]);
      uint8x16_t sv = vdupq_n_u8(n[1]);
      do {
        uint64_t m = vget_lane_u64(
            vreinterpret_u64_u8(vshrn_n_u16(
                vreinterpretq_u16_u8(vminq_u8(vceqq_u8(vld1q_u8(h), nv),
                                              vceqq_u8(vld1q_u8(h + 1), sv))),
                4)),
            0);
        if (m) {
          h += __builtin_ctzll(m) >> 2;
          h += 1;
          j += 1;
          break;
        }
        h += 16;
      } while (e - h >= 17);
    }
#endif

    // kmp algorithm
    if (h >= e)
      break;
    if (*h == n[j]) {
      ++h;
      ++j;
      if (j == l)
        return (void *)(h - j);
    } else if (j) {
      j = lps[j - 1];
    } else {
      ++h;
    }
  }
  return 0;
}
