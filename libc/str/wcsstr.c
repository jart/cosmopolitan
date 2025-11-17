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
static bool wcsstr_avx2(const wchar_t **h, const wchar_t *n, size_t l,
                        size_t *j) {
  __m256i nv = _mm256_set1_epi32(n[0]);
  __m256i sv = _mm256_set1_epi32(n[1]);
  __m256i zv = _mm256_setzero_si256();
  do {
    unsigned m = _mm256_movemask_epi8(_mm256_or_si256(
        _mm256_cmpeq_epi32(_mm256_loadu_si256((const __m256i *)*h), zv),
        _mm256_min_epu32(
            _mm256_cmpeq_epi32(_mm256_loadu_si256((const __m256i *)*h), nv),
            _mm256_cmpeq_epi32(_mm256_loadu_si256((const __m256i *)(*h + 1)),
                               sv))));
    if (m) {
      *h += __builtin_ctz(m) >> 2;
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
    *h += 32 / sizeof(wchar_t);
  } while (((intptr_t)*h & 4095) <= 4096 - 36);
  return false;
}
#pragma GCC pop_options
dontinline relegated static bool wcsstr_sse2(const wchar_t **h,
                                             const wchar_t *n, size_t l,
                                             size_t *j) {
  __m128i nv = _mm_set1_epi32(n[0]);
  __m128i sv = _mm_set1_epi32(n[1]);
  __m128i zv = _mm_setzero_si128();
  do {
    unsigned m = _mm_movemask_epi8(_mm_or_si128(
        _mm_cmpeq_epi32(_mm_loadu_si128((__m128i *)*h), zv),
        _mm_and_si128(
            _mm_cmpeq_epi32(_mm_loadu_si128((__m128i *)*h), nv),
            _mm_cmpeq_epi32(_mm_loadu_si128((__m128i *)(*h + 1)), sv))));
    if (m) {
      *h += __builtin_ctz(m) >> 2;
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
    *h += 16 / sizeof(wchar_t);
  } while (((intptr_t)*h & 4095) <= 4096 - 20);
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
 * @see memmem()
 */
wchar_t *wcsstr(const wchar_t *haystack, const wchar_t *needle) {

  // handle special cases
  if (!*needle)
    return (wchar_t *)haystack;
  if (!needle[1])
    return wcschr(haystack, *needle);

  // rename arguments
  const wchar_t *h = (const wchar_t *)haystack;
  const wchar_t *n = (const wchar_t *)needle;
  size_t l = wcslen(needle);

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
    if (!j && ((intptr_t)h & 4095) <= 4096 - 36) {
      if (X86_HAVE(AVX2) && !IsModeDbg()) {
        if (wcsstr_avx2(&h, n, l, &j))
          return (wchar_t *)h;
      } else {
        if (wcsstr_sse2(&h, n, l, &j))
          return (wchar_t *)h;
      }
    }
#elif defined(__aarch64__) && defined(__ARM_NEON)
    if (!j && ((intptr_t)h & 4095) <= 4096 - 20) {
      uint32x4_t nv = vdupq_n_u32(n[0]);
      uint32x4_t sv = vdupq_n_u32(n[1]);
      uint32x4_t zv = vdupq_n_u32(0);
      do {
        uint64_t m = vget_lane_u64(
            vreinterpret_u64_u8(vshrn_n_u16(
                vreinterpretq_u16_u32(vorrq_u32(
                    vceqq_u32(vld1q_u32((const uint32_t *)h), zv),
                    vminq_u32(
                        vceqq_u32(vld1q_u32((const uint32_t *)h), nv),
                        vceqq_u32(vld1q_u32((const uint32_t *)(h + 1)), sv)))),
                4)),
            0);
        if (m) {
          h += __builtin_ctzll(m) >> 4;
          if (!*h)
            return 0;
          if (l == 2)
            return (wchar_t *)h;
          h += 2;
          j += 2;
          break;
        }
        h += 4;
      } while (((intptr_t)h & 4095) <= 4096 - 20);
    }
#endif

    // kmp algorithm
    if (!*h)
      break;
    if (*h == n[j]) {
      ++h;
      ++j;
      if (j == l)
        return (wchar_t *)(h - j);
    } else if (j) {
      j = lps[j - 1];
    } else {
      ++h;
    }
  }
  return 0;
}
