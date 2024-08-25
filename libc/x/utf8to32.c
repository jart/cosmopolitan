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
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/str/thompike.h"
#include "libc/str/utf16.h"
#include "libc/x/x.h"
#include "third_party/intel/emmintrin.internal.h"

/**
 * Transcodes UTF-8 to UTF-32.
 *
 * @param p is input value
 * @param n if -1 implies strlen
 * @param z if non-NULL receives output length
 */
wchar_t *utf8to32(const char *p, size_t n, size_t *z) {
  size_t i;
  unsigned m, j;
  wint_t x, a, b;
  wchar_t *r, *q;
  if (z)
    *z = 0;
  if (n == -1)
    n = p ? strlen(p) : 0;
  if ((q = r = malloc(n * sizeof(wchar_t) + sizeof(wchar_t)))) {
    for (i = 0; i < n;) {
#ifdef __x86_64__
      if (!((uintptr_t)(p + i) & 15) && i + 16 < n) {
        do {
          __m128i v1, v2, v3, v4;
          v1 = _mm_loadu_si128((__m128i *)(p + i));
          v2 = _mm_cmpgt_epi8(v1, _mm_setzero_si128());
          if (_mm_movemask_epi8(v2) != 0xFFFF)
            break;
          v3 = _mm_unpacklo_epi8(v1, _mm_setzero_si128());
          v1 = _mm_unpackhi_epi8(v1, _mm_setzero_si128());
          v4 = _mm_unpacklo_epi16(v3, _mm_setzero_si128());
          v3 = _mm_unpackhi_epi16(v3, _mm_setzero_si128());
          v2 = _mm_unpacklo_epi16(v1, _mm_setzero_si128());
          v1 = _mm_unpackhi_epi16(v1, _mm_setzero_si128());
          _mm_storeu_si128((__m128i *)(q + 0), v4);
          _mm_storeu_si128((__m128i *)(q + 4), v3);
          _mm_storeu_si128((__m128i *)(q + 8), v2);
          _mm_storeu_si128((__m128i *)(q + 12), v1);
          i += 16;
          q += 16;
        } while (i + 16 < n);
      }
#endif
      x = p[i++] & 0xff;
      if (x >= 0300) {
        a = ThomPikeByte(x);
        m = ThomPikeLen(x) - 1;
        if (i + m <= n) {
          for (j = 0;;) {
            b = p[i + j] & 0xff;
            if (!ThomPikeCont(b))
              break;
            a = ThomPikeMerge(a, b);
            if (++j == m) {
              x = a;
              i += j;
              break;
            }
          }
        }
      }
      *q++ = x;
    }
    if (z)
      *z = q - r;
    *q++ = '\0';
    if ((q = realloc(r, (q - r) * sizeof(wchar_t))))
      r = q;
  }
  return r;
}
