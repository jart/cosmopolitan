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
#include "libc/intrin/likely.h"
#include "libc/str/kmp.h"
#include "libc/str/str.h"
#include "third_party/aarch64/arm_neon.internal.h"
#include "third_party/intel/emmintrin.internal.h"

/**
 * Searches for fixed-length substring in memory region.
 *
 * This function offers assurances against pathological cases, using KMP
 * if no progress is being made on the O(nm) vectorized fast path. It is
 * important to note that, if `needle` is untrusted, that it not be long
 * enough to overflow the stack. That's because KMP needs to allocate an
 * array of longs the same length as `needle` and it needs to do it with
 * stack memory because this function is safe to call in signal handlers
 *
 * @param haystack is the region of memory to be searched
 * @param haystacklen is its character count
 * @param needle contains the memory for which we're searching
 * @param needlelen is its character count
 * @return pointer to first result or NULL if not found
 * @asyncsignalsafe
 */
__vex void *memmem(const void *haystack, size_t haystacklen, const void *needle,
                   size_t needlelen) {
#if defined(__x86_64__) && !defined(__chibicc__)
  char c;
  __m128i n;
  const __m128i *v;
  unsigned i, k, m;
  long progress = 0;
  const char *p, *q, *e;
  long scare = -(needlelen * 10);
  if (!needlelen)
    return (void *)haystack;
  if (UNLIKELY(needlelen > haystacklen))
    return 0;
  q = needle;
  c = *q;
  n = _mm_set1_epi8(c);
  p = haystack;
  e = p + haystacklen;
  k = (uintptr_t)p & 15;
  v = (const __m128i *)((uintptr_t)p & -16);
  m = _mm_movemask_epi8(_mm_cmpeq_epi8(_mm_load_si128(v), n));
  m >>= k;
  m <<= k;
  for (;;) {
    while (!m) {
      ++v;
      progress += 16;
      if ((const char *)v >= e)
        return 0;
      m = _mm_movemask_epi8(_mm_cmpeq_epi8(_mm_load_si128(v), n));
    }
    do {
      k = __builtin_ctzl(m);
      p = (const char *)v + k;
      if (UNLIKELY(p + needlelen > e))
        return 0;
      for (i = 1;; ++i) {
        if (--progress <= scare)
          goto OfferPathologicalAssurances;
        if (i == needlelen)
          return (/*unconst*/ char *)p;
        if (p[i] != q[i])
          break;
      }
      m &= ~(1 << k);
    } while (m);
  }
OfferPathologicalAssurances:
#elif defined(__aarch64__) && defined(__ARM_NEON)
  char c;
  uint8x16_t n;
  const uint8x16_t *v;
  size_t i, k;
  uint64_t m;
  long progress = 0;
  const char *p, *q, *e;
  long scare = -(needlelen * 10);
  if (!needlelen)
    return (void *)haystack;
  if (UNLIKELY(needlelen > haystacklen))
    return 0;
  q = needle;
  c = *q;
  n = vdupq_n_u8(c);
  p = haystack;
  e = p + haystacklen;
  k = (uintptr_t)p & 15;
  v = (const uint8x16_t *)((uintptr_t)p & -16);
  uint8x16_t cmp = vceqq_u8(vld1q_u8((const uint8_t *)v), n);
  uint8x8_t mask = vshrn_n_u16(vreinterpretq_u16_u8(cmp), 4);
  vst1_u8((uint8_t *)&m, mask);
  m >>= k * 4;
  m <<= k * 4;
  for (;;) {
    while (!m) {
      ++v;
      progress += 16;
      if ((const char *)v >= e)
        return 0;
      cmp = vceqq_u8(vld1q_u8((const uint8_t *)v), n);
      mask = vshrn_n_u16(vreinterpretq_u16_u8(cmp), 4);
      vst1_u8((uint8_t *)&m, mask);
    }
    do {
      k = __builtin_ctzll(m) >> 2;
      p = (const char *)v + k;
      if (UNLIKELY(p + needlelen > e))
        return 0;
      for (i = 1;; ++i) {
        if (--progress <= scare)
          goto OfferPathologicalAssurances;
        if (i == needlelen)
          return (/*unconst*/ char *)p;
        if (p[i] != q[i])
          break;
      }
      m &= ~(0xFULL << (k * 4));
    } while (m);
  }
OfferPathologicalAssurances:
#endif
  return __memmem_kmp(haystack, haystacklen, needle, needlelen);
}
