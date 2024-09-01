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
#include "libc/str/kmp.h"
#include "third_party/aarch64/arm_neon.internal.h"
#include "third_party/intel/immintrin.internal.h"

/**
 * Searches for substring.
 *
 * This function offers assurances against pathological cases, using KMP
 * if no progress is being made on the O(nm) vectorized fast path. It is
 * important to note that, if `needle` is untrusted, that it not be long
 * enough to overflow the stack. That's because KMP needs to allocate an
 * array of longs the same length as `needle` and it needs to do it with
 * stack memory since POSIX requires this function to be safe to call in
 * signal handlers.
 *
 * @param haystack is the search area, as a NUL-terminated string
 * @param needle is the desired substring, also NUL-terminated
 * @return pointer to first substring within haystack, or NULL
 * @asyncsignalsafe
 * @see strcasestr()
 * @see memmem()
 */
__vex char *strstr(const char *haystack, const char *needle) {
  if (haystack == needle || !*needle)
    return (char *)haystack;
#if defined(__x86_64__) && !defined(__chibicc__)
  size_t i;
  unsigned k, m;
  const __m128i *p;
  long progress = 0;
  __m128i v, n, z = _mm_setzero_si128();
  const char *hay = haystack;
  n = _mm_set1_epi8(*needle);
  for (;;) {
    k = (uintptr_t)hay & 15;
    p = (const __m128i *)((uintptr_t)hay & -16);
    v = _mm_load_si128(p);
    m = _mm_movemask_epi8(
        _mm_or_si128(_mm_cmpeq_epi8(v, z), _mm_cmpeq_epi8(v, n)));
    m >>= k;
    m <<= k;
    while (!m) {
      progress += 16;
      v = _mm_load_si128(++p);
      m = _mm_movemask_epi8(
          _mm_or_si128(_mm_cmpeq_epi8(v, z), _mm_cmpeq_epi8(v, n)));
    }
    int offset = __builtin_ctzl(m);
    progress += offset;
    hay = (const char *)p + offset;
    for (i = 0;; ++i) {
      if (--progress <= -512)
        goto OfferPathologicalAssurances;
      if (!needle[i])
        return (/*unconst*/ char *)hay;
      if (!hay[i])
        break;
      if (needle[i] != hay[i])
        break;
    }
    if (!*hay++)
      break;
  }
  return 0;
OfferPathologicalAssurances:
#elif defined(__aarch64__) && defined(__ARM_NEON)
  size_t i;
  const char *hay = haystack;
  uint8x16_t n = vdupq_n_u8(*needle);
  uint8x16_t z = vdupq_n_u8(0);
  long progress = 0;
  for (;;) {
    int k = (uintptr_t)hay & 15;
    hay = (const char *)((uintptr_t)hay & -16);
    uint8x16_t v = vld1q_u8((const uint8_t *)hay);
    uint8x16_t cmp = vorrq_u8(vceqq_u8(v, z), vceqq_u8(v, n));
    uint8x8_t mask = vshrn_n_u16(vreinterpretq_u16_u8(cmp), 4);
    uint64_t m;
    vst1_u8((uint8_t *)&m, mask);
    m >>= k * 4;
    m <<= k * 4;
    while (!m) {
      hay += 16;
      progress += 16;
      v = vld1q_u8((const uint8_t *)hay);
      cmp = vorrq_u8(vceqq_u8(v, z), vceqq_u8(v, n));
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
        return (/*unconst*/ char *)hay;
      if (!hay[i])
        break;
      if (needle[i] != hay[i])
        break;
    }
    if (!*hay++)
      break;
  }
  return 0;
OfferPathologicalAssurances:
#endif
  return __memmem_kmp(haystack, strlen(haystack), needle, strlen(needle));
}
