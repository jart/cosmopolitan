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

/**
 * Transcodes UTF-8 to UTF-16.
 *
 * @param p is input value
 * @param n if -1 implies strlen
 * @param z if non-NULL receives output length
 */
char16_t *utf8to16(const char *p, size_t n, size_t *z) {
  size_t i;
  wint_t x, a, b;
  char16_t *r, *q;
  unsigned m, j, w;
  if (z) *z = 0;
  if (n == -1) n = p ? strlen(p) : 0;
  if ((q = r = malloc((n + 16) * sizeof(char16_t) * 2 + sizeof(char16_t)))) {
    for (i = 0; i < n;) {
#if defined(__SSE2__) && defined(__GNUC__) && !defined(__STRICT_ANSI__) && \
    !defined(__llvm__) && !defined(__chibicc__)
      if (i + 16 < n) {
        typedef char xmm_t __attribute__((__vector_size__(16), __aligned__(1)));
        xmm_t vi, vz = {0};
        do {
          vi = *(const xmm_t *)(p + i);
          *(xmm_t *)(q + 0) = __builtin_ia32_punpcklbw128(vi, vz);
          *(xmm_t *)(q + 8) = __builtin_ia32_punpckhbw128(vi, vz);
          if (!(m = __builtin_ia32_pmovmskb128(vi > vz) ^ 0xffff)) {
            i += 16;
            q += 16;
          } else {
            m = __builtin_ctzl(m);
            i += m;
            q += m;
            break;
          }
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
            if (!ThomPikeCont(b)) break;
            a = ThomPikeMerge(a, b);
            if (++j == m) {
              x = a;
              i += j;
              break;
            }
          }
        }
      }
      w = EncodeUtf16(x);
      *q++ = w;
      if ((w >>= 16)) *q++ = w;
    }
    if (z) *z = q - r;
    *q++ = '\0';
    if ((q = realloc(r, (q - r) * sizeof(char16_t)))) r = q;
  }
  return r;
}
