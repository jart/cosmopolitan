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
#include "libc/str/str.h"
#include "libc/dce.h"
#include "libc/mem/alloca.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/runtime/stack.h"
#include "libc/str/internal.h"
#include "libc/str/tab.h"
#include "third_party/aarch64/arm_neon.internal.h"
#include "third_party/intel/immintrin.internal.h"

#if defined(__x86_64__) && !defined(__chibicc__)
#pragma GCC push_options
#pragma GCC target("avx2")
static bool strcasestr_avx2(const unsigned char **h, const unsigned char *n,
                            size_t l, size_t *j) {
  __m256i zv = _mm256_setzero_si256();
  __m256i nvl = _mm256_set1_epi8(n[0]);
  __m256i nvu = _mm256_set1_epi8(kToUpper[n[0]]);
  __m256i svl = _mm256_set1_epi8(n[1]);
  __m256i svu = _mm256_set1_epi8(kToUpper[n[1]]);
  do {
    unsigned m = _mm256_movemask_epi8(_mm256_or_si256(
        _mm256_cmpeq_epi8(_mm256_loadu_si256((const __m256i *)*h), zv),
        _mm256_min_epu8(
            _mm256_or_si256(
                _mm256_cmpeq_epi8(_mm256_loadu_si256((const __m256i *)*h), nvl),
                _mm256_cmpeq_epi8(_mm256_loadu_si256((const __m256i *)*h),
                                  nvu)),
            _mm256_or_si256(
                _mm256_cmpeq_epi8(_mm256_loadu_si256((const __m256i *)(*h + 1)),
                                  svl),
                _mm256_cmpeq_epi8(_mm256_loadu_si256((const __m256i *)(*h + 1)),
                                  svu)))));
    if (m) {
      *h += __builtin_ctz(m);
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
    *h += 32;
  } while (((intptr_t)*h & 4095) <= 4096 - 33);
  return false;
}
#pragma GCC pop_options
dontinline relegated static bool strcasestr_sse2(const unsigned char **h,
                                                 const unsigned char *n,
                                                 size_t l, size_t *j) {
  __m128i zv = _mm_setzero_si128();
  __m128i nvl = _mm_set1_epi8(n[0]);
  __m128i nvu = _mm_set1_epi8(kToUpper[n[0]]);
  __m128i svl = _mm_set1_epi8(n[1]);
  __m128i svu = _mm_set1_epi8(kToUpper[n[1]]);
  do {
    unsigned m = _mm_movemask_epi8(_mm_or_si128(
        _mm_cmpeq_epi8(_mm_loadu_si128((__m128i *)*h), zv),
        _mm_min_epu8(
            _mm_or_si128(_mm_cmpeq_epi8(_mm_loadu_si128((__m128i *)*h), nvl),
                         _mm_cmpeq_epi8(_mm_loadu_si128((__m128i *)*h), nvu)),
            _mm_or_si128(
                _mm_cmpeq_epi8(_mm_loadu_si128((__m128i *)(*h + 1)), svl),
                _mm_cmpeq_epi8(_mm_loadu_si128((__m128i *)(*h + 1)), svu)))));
    if (m) {
      *h += __builtin_ctz(m);
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
    *h += 16;
  } while (((intptr_t)*h & 4095) <= 4096 - 17);
  return false;
}
#endif  // __x86_64__

/**
 * Searches for substring case insensitively.
 *
 * @param haystack is the search area, as a NUL-terminated string
 * @param needle is the desired substring, also NUL-terminated
 * @return pointer to first substring within haystack, or NULL
 * @asyncsignalsafe
 * @see strcasestr()
 * @see memmem()
 */
char *strcasestr(const char *haystack, const char *needle) {

  // handle special cases
  if (!*needle)
    return (char *)haystack;
  if (!needle[1])
    return strcasechr(haystack, *needle);

  // rename arguments
  const unsigned char *h = (const unsigned char *)haystack;
  size_t l = strlen(needle);

  // copy needle as lowercase
#pragma GCC push_options
#pragma GCC diagnostic ignored "-Walloca-larger-than="
#pragma GCC diagnostic ignored "-Wanalyzer-out-of-bounds"
  unsigned char *n = (unsigned char *)alloca(l + 1);
  CheckLargeStackAllocation(n, l + 1);
  for (size_t i = 0; i < l; ++i)
    n[i] = kToLower[needle[i] & 255];
  n[l] = 0;
#pragma GCC pop_options

  // compute longest prefix suffix array
  size_t i = 1;
  size_t len = 0;
#pragma GCC push_options
#pragma GCC diagnostic ignored "-Walloca-larger-than="
#pragma GCC diagnostic ignored "-Wanalyzer-out-of-bounds"
  size_t need = sizeof(unsigned) * l;
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
    if (!j && ((intptr_t)h & 4095) <= 4096 - 33) {
      if (X86_HAVE(AVX2) && !IsModeDbg()) {
        if (strcasestr_avx2(&h, n, l, &j))
          return (char *)h;
      } else {
        if (strcasestr_sse2(&h, n, l, &j))
          return (char *)h;
      }
    }
#elif defined(__aarch64__) && defined(__ARM_NEON)
    if (!j && ((intptr_t)h & 4095) <= 4096 - 17) {
      uint8x16_t zv = vdupq_n_u8(0);
      uint8x16_t nvl = vdupq_n_u8(n[0]);
      uint8x16_t nvu = vdupq_n_u8(kToUpper[n[0]]);
      uint8x16_t svl = vdupq_n_u8(n[1]);
      uint8x16_t svu = vdupq_n_u8(kToUpper[n[1]]);
      do {
        uint64_t m = vget_lane_u64(
            vreinterpret_u64_u8(vshrn_n_u16(
                vreinterpretq_u16_u8(vorrq_u8(
                    vceqq_u8(vld1q_u8(h), zv),
                    vminq_u8(vorrq_u8(vceqq_u8(vld1q_u8(h), nvl),
                                      vceqq_u8(vld1q_u8(h), nvu)),
                             vorrq_u8(vceqq_u8(vld1q_u8(h + 1), svl),
                                      vceqq_u8(vld1q_u8(h + 1), svu))))),
                4)),
            0);
        if (m) {
          h += __builtin_ctzll(m) >> 2;
          if (!*h)
            return 0;
          if (l == 2)
            return (char *)h;
          h += 2;
          j += 2;
          break;
        }
        h += 16;
      } while (((intptr_t)h & 4095) <= 4096 - 17);
    }
#endif

    // kmp algorithm
    if (!*h)
      break;
    if (kToLower[*h] == n[j]) {
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
