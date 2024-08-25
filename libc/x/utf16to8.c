/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/bsr.h"
#include "libc/mem/mem.h"
#include "libc/serialize.h"
#include "libc/str/str.h"
#include "libc/str/thompike.h"
#include "libc/str/utf16.h"
#include "libc/x/x.h"
#include "third_party/intel/emmintrin.internal.h"

/**
 * Transcodes UTF-16 to UTF-8.
 *
 * @param p is input value
 * @param n if -1 implies strlen
 * @param z if non-NULL receives output length
 */
char *utf16to8(const char16_t *p, size_t n, size_t *z) {
  unsigned w;
  char *r, *q;
  wint_t x, y;
  const char16_t *e;
  if (z)
    *z = 0;
  if (n == -1)
    n = p ? strlen16(p) : 0;
  if ((q = r = malloc(n * 4 + 8 + 1))) {
    for (e = p + n; p < e;) {
#if defined(__x86_64__)
      if (p + 8 < e) {
        do {
          __m128i v1 = _mm_loadu_si128((__m128i *)p);
          __m128i v2 = _mm_cmpgt_epi16(v1, _mm_setzero_si128());
          __m128i v3 = _mm_cmpgt_epi16(v1, _mm_set1_epi16(127));
          v2 = _mm_andnot_si128(v3, v2);
          if (_mm_movemask_epi8(v2) != 0xFFFF)
            break;
          _mm_storel_epi64((__m128i *)q, _mm_packs_epi16(v1, v1));
          p += 8;
          q += 8;
        } while (p + 8 < e);
      }
#endif
      x = *p++ & 0xffff;
      if (!IsUcs2(x)) {
        if (p < e) {
          y = *p++ & 0xffff;
          x = MergeUtf16(x, y);
        } else {
          x = 0xFFFD;
        }
      }
      if (x < 0200) {
        *q++ = x;
      } else {
        w = tpenc(x);
        WRITE64LE(q, w);
        q += bsr(w) >> 3;
        q += 1;
      }
    }
    if (z)
      *z = q - r;
    *q++ = '\0';
    if ((q = realloc(r, (q - r) * 1)))
      r = q;
  }
  return r;
}
