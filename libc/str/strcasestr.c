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
#include "libc/str/str.h"
#include "libc/ctype.h"
#include "libc/mem/alloca.h"
#include "libc/runtime/stack.h"
#include "libc/str/tab.h"
#include "third_party/aarch64/arm_neon.internal.h"
#include "third_party/intel/immintrin.internal.h"

static int ToUpper(int c) {
  return 'a' <= c && c <= 'z' ? c - ('a' - 'A') : c;
}

static void computeLPS(const char *pattern, long M, long *lps) {
  long len = 0;
  lps[0] = 0;
  long i = 1;
  while (i < M) {
    if (kToLower[pattern[i] & 255] == kToLower[pattern[len] & 255]) {
      len++;
      lps[i] = len;
      i++;
    } else {
      if (len != 0) {
        len = lps[len - 1];
      } else {
        lps[i] = 0;
        i++;
      }
    }
  }
}

static char *kmp(const char *s, size_t n, const char *ss, size_t m) {
  if (!m)
    return (char *)s;
  if (n < m)
    return NULL;
#pragma GCC push_options
#pragma GCC diagnostic ignored "-Walloca-larger-than="
#pragma GCC diagnostic ignored "-Wanalyzer-out-of-bounds"
  long need = sizeof(long) * m;
  long *lps = (long *)alloca(need);
  CheckLargeStackAllocation(lps, need);
#pragma GCC pop_options
  computeLPS(ss, m, lps);
  long i = 0;
  long j = 0;
  while (i < n) {
    if (kToLower[ss[j] & 255] == kToLower[s[i] & 255]) {
      i++;
      j++;
    }
    if (j == m) {
      return (char *)(s + i - j);
    } else if (i < n && kToLower[ss[j] & 255] != kToLower[s[i] & 255]) {
      if (j != 0) {
        j = lps[j - 1];
      } else {
        i++;
      }
    }
  }
  return NULL;
}

/**
 * Searches for substring case-insensitively.
 *
 * @param haystack is the search area, as a NUL-terminated string
 * @param needle is the desired substring, also NUL-terminated
 * @return pointer to first substring within haystack, or NULL
 * @asyncsignalsafe
 * @see strstr()
 */
char *strcasestr(const char *haystack, const char *needle) {
  if (haystack == needle || !*needle)
    return (char *)haystack;
#if defined(__x86_64__) && !defined(__chibicc__)
  size_t i;
  unsigned k, m;
  const __m128i *p;
  long progress = 0;
  __m128i v, nl, nu, z = _mm_setzero_si128();
  const char *hay = haystack;
  char first_lower = kToLower[*needle & 255];
  char first_upper = ToUpper(*needle);
  nl = _mm_set1_epi8(first_lower);
  nu = _mm_set1_epi8(first_upper);
  for (;;) {
    k = (uintptr_t)hay & 15;
    p = (const __m128i *)((uintptr_t)hay & -16);
    v = _mm_load_si128(p);
    m = _mm_movemask_epi8(_mm_or_si128(
        _mm_or_si128(_mm_cmpeq_epi8(v, z),    // Check for null terminator
                     _mm_cmpeq_epi8(v, nl)),  // Check lowercase
        _mm_cmpeq_epi8(v, nu)));              // Check uppercase
    m >>= k;
    m <<= k;
    while (!m) {
      progress += 16;
      v = _mm_load_si128(++p);
      m = _mm_movemask_epi8(_mm_or_si128(
          _mm_or_si128(_mm_cmpeq_epi8(v, z), _mm_cmpeq_epi8(v, nl)),
          _mm_cmpeq_epi8(v, nu)));
    }
    int offset = __builtin_ctzl(m);
    progress += offset;
    hay = (const char *)p + offset;
    for (i = 0;; ++i) {
      if (--progress <= -512)
        goto OfferPathologicalAssurances;
      if (!needle[i])
        return (char *)hay;
      if (!hay[i])
        break;
      if (kToLower[needle[i] & 255] != kToLower[hay[i] & 255])
        break;
    }
    if (!*hay++)
      break;
  }
  return 0;
#elif defined(__aarch64__) && defined(__ARM_NEON)
  size_t i;
  const char *hay = haystack;
  uint8_t first_lower = kToLower[*needle & 255];
  uint8_t first_upper = ToUpper(*needle);
  uint8x16_t nl = vdupq_n_u8(first_lower);
  uint8x16_t nu = vdupq_n_u8(first_upper);
  uint8x16_t z = vdupq_n_u8(0);
  long progress = 0;
  for (;;) {
    int k = (uintptr_t)hay & 15;
    hay = (const char *)((uintptr_t)hay & -16);
    uint8x16_t v = vld1q_u8((const uint8_t *)hay);
    uint8x16_t cmp_lower = vceqq_u8(v, nl);
    uint8x16_t cmp_upper = vceqq_u8(v, nu);
    uint8x16_t cmp_null = vceqq_u8(v, z);
    uint8x16_t cmp = vorrq_u8(vorrq_u8(cmp_lower, cmp_upper), cmp_null);
    uint8x8_t mask = vshrn_n_u16(vreinterpretq_u16_u8(cmp), 4);
    uint64_t m;
    vst1_u8((uint8_t *)&m, mask);
    m >>= k * 4;
    m <<= k * 4;
    while (!m) {
      hay += 16;
      progress += 16;
      v = vld1q_u8((const uint8_t *)hay);
      cmp_lower = vceqq_u8(v, nl);
      cmp_upper = vceqq_u8(v, nu);
      cmp_null = vceqq_u8(v, z);
      cmp = vorrq_u8(vorrq_u8(cmp_lower, cmp_upper), cmp_null);
      mask = vshrn_n_u16(vreinterpretq_u16_u8(cmp), 4);
      vst1_u8((uint8_t *)&m, mask);
    }
    int offset = __builtin_ctzll(m) >> 2;
    progress += offset;
    hay += offset;
    for (i = 0;; ++i) {
      if (--progress <= -512)
        goto OfferPathologicalAssurances;
      if (!needle[i])
        return (char *)hay;
      if (!hay[i])
        break;
      if (kToLower[needle[i] & 255] != kToLower[hay[i] & 255])
        break;
    }
    if (!*hay++)
      break;
  }
  return 0;
#endif
OfferPathologicalAssurances:
  return kmp(haystack, strlen(haystack), needle, strlen(needle));
}
