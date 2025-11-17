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
#include "libc/mem/alloca.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/runtime/stack.h"
#include "libc/str/str.h"
#include "third_party/aarch64/arm_neon.internal.h"
#include "third_party/intel/immintrin.internal.h"

#if defined(__x86_64__) && !defined(__chibicc__)
#pragma GCC push_options
#pragma GCC target("avx2")
static bool strstr16_avx2(const char16_t **h, const char16_t *n, size_t l,
                          size_t *j) {
  __m256i nv = _mm256_set1_epi16(n[0]);
  __m256i sv = _mm256_set1_epi16(n[1]);
  __m256i zv = _mm256_setzero_si256();
  do {
    unsigned m = _mm256_movemask_epi8(_mm256_or_si256(
        _mm256_cmpeq_epi16(_mm256_loadu_si256((const __m256i *)*h), zv),
        _mm256_min_epu16(
            _mm256_cmpeq_epi16(_mm256_loadu_si256((const __m256i *)*h), nv),
            _mm256_cmpeq_epi16(_mm256_loadu_si256((const __m256i *)(*h + 1)),
                               sv))));
    if (m) {
      *h += __builtin_ctz(m) >> 1;
      if (!**h) {
        *h = 0;
        return true;
      }
      if (l == 2)
        return true;
      *h += 2;
      *j += 2;
      break;
    }
    *h += 32 / sizeof(char16_t);
  } while (((intptr_t)*h & 4095) <= 4096 - 34);
  return false;
}
#pragma GCC pop_options
dontinline relegated static bool strstr16_sse2(const char16_t **h,
                                               const char16_t *n, size_t l,
                                               size_t *j) {
  __m128i nv = _mm_set1_epi16(n[0]);
  __m128i sv = _mm_set1_epi16(n[1]);
  __m128i zv = _mm_setzero_si128();
  do {
    unsigned m = _mm_movemask_epi8(_mm_or_si128(
        _mm_cmpeq_epi16(_mm_loadu_si128((__m128i *)*h), zv),
        _mm_and_si128(
            _mm_cmpeq_epi16(_mm_loadu_si128((__m128i *)*h), nv),
            _mm_cmpeq_epi16(_mm_loadu_si128((__m128i *)(*h + 1)), sv))));
    if (m) {
      *h += __builtin_ctz(m) >> 1;
      if (!**h) {
        *h = 0;
        return true;
      }
      if (l == 2)
        return true;
      *h += 2;
      *j += 2;
      break;
    }
    *h += 16 / sizeof(char16_t);
  } while (((intptr_t)*h & 4095) <= 4096 - 18);
  return false;
}
#endif  // __x86_64__

/**
 * Searches for substring.
 *
 * This implementation requires 4 bytes of stack space for every
 * character in needle.
 *
 * @param haystack is the search area, as a NUL-terminated string
 * @param needle is the desired substring, also NUL-terminated
 * @return pointer to first substring within haystack, or NULL
 * @asyncsignalsafe
 */
char16_t *strstr16(const char16_t *haystack, const char16_t *needle) {

  // handle special cases
  if (!*needle)
    return (char16_t *)haystack;
  if (!needle[1])
    return strchr16(haystack, *needle);

  // rename arguments
  const char16_t *h = (const char16_t *)haystack;
  const char16_t *n = (const char16_t *)needle;
  size_t l = strlen16(needle);

  // compute longest prefix suffix array
  size_t i = 1;
  size_t len = 0;
#pragma GCC push_options
#pragma GCC diagnostic ignored "-Walloca-larger-than="
#pragma GCC diagnostic ignored "-Wanalyzer-out-of-bounds"
  unsigned need = sizeof(unsigned) * l;
  unsigned *lps = (unsigned *)alloca(need);
  CheckLargeStackAllocation(lps, need);
#pragma GCC pop_options
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
  for (;;) {

    // simd accelerate search of first two chars in needle
#if defined(__x86_64__) && !defined(__chibicc__)
    if (!j && ((intptr_t)h & 4095) <= 4096 - 34) {
      if (X86_HAVE(AVX2) && !IsModeDbg()) {
        if (strstr16_avx2(&h, n, l, &j))
          return (char16_t *)h;
      } else {
        if (strstr16_sse2(&h, n, l, &j))
          return (char16_t *)h;
      }
    }
#elif defined(__aarch64__) && defined(__ARM_NEON)
    if (!j && ((intptr_t)h & 4095) <= 4096 - 18) {
      uint16x8_t nv = vdupq_n_u16(n[0]);
      uint16x8_t sv = vdupq_n_u16(n[1]);
      uint16x8_t zv = vdupq_n_u16(0);
      do {
        uint64_t m = vget_lane_u64(
            vreinterpret_u64_u8(vshrn_n_u16(
                vorrq_u16(
                    vceqq_u16(vld1q_u16((const uint16_t *)h), zv),
                    vminq_u16(
                        vceqq_u16(vld1q_u16((const uint16_t *)h), nv),
                        vceqq_u16(vld1q_u16((const uint16_t *)(h + 1)), sv))),
                4)),
            0);
        if (m) {
          h += __builtin_ctzll(m) >> 3;
          if (!*h)
            return 0;
          if (l == 2)
            return (char16_t *)h;
          h += 2;
          j += 2;
          break;
        }
        h += 8;
      } while (((intptr_t)h & 4095) <= 4096 - 18);
    }
#endif

    // kmp algorithm
    if (!*h)
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
