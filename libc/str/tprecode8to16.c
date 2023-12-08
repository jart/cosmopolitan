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
#include "libc/intrin/pcmpgtb.h"
#include "libc/intrin/pmovmskb.h"
#include "libc/intrin/punpckhbw.h"
#include "libc/intrin/punpcklbw.h"
#include "libc/str/str.h"
#include "libc/str/thompike.h"
#include "libc/str/utf16.h"

// 34x speedup for ascii
static inline axdx_t tprecode8to16_sse2(char16_t *dst, size_t dstsize,
                                        const char *src, axdx_t r) {
  uint8_t v1[16], v2[16], vz[16];
  memset(vz, 0, 16);
  while (r.ax + 16 < dstsize) {
    memcpy(v1, src + r.dx, 16);
    pcmpgtb((int8_t *)v2, (int8_t *)v1, (int8_t *)vz);
    if (pmovmskb(v2) != 0xFFFF) break;
    punpcklbw(v2, v1, vz);
    punpckhbw(v1, v1, vz);
    memcpy(dst + r.ax + 0, v2, 16);
    memcpy(dst + r.ax + 8, v1, 16);
    r.ax += 16;
    r.dx += 16;
  }
  return r;
}

/**
 * Transcodes UTF-8 to UTF-16.
 *
 * This is a low-level function intended for the core runtime. Use
 * utf8to16() for a much better API that uses malloc().
 *
 * @param dst is output buffer
 * @param dstsize is shorts in dst
 * @param src is NUL-terminated UTF-8 input string
 * @return ax shorts written excluding nul
 * @return dx index of character after nul word in src
 * @asyncsignalsafe
 */
axdx_t tprecode8to16(char16_t *dst, size_t dstsize, const char *src) {
  axdx_t r;
  unsigned w;
  int x, a, b, i, n;
  r.ax = 0;
  r.dx = 0;
  for (;;) {
#if defined(__x86_64__) && !IsModeDbg()
    if (!((uintptr_t)(src + r.dx) & 15)) {
      r = tprecode8to16_sse2(dst, dstsize, src, r);
    }
#endif
    x = src[r.dx++] & 0377;
    if (x >= 0300) {
      a = ThomPikeByte(x);
      n = ThomPikeLen(x) - 1;
      for (i = 0;;) {
        if (!(b = src[r.dx + i] & 0377)) break;
        if (!ThomPikeCont(b)) break;
        a = ThomPikeMerge(a, b);
        if (++i == n) {
          r.dx += i;
          x = a;
          break;
        }
      }
    }
    if (!x) break;
    w = EncodeUtf16(x);
    while (w && r.ax + 1 < dstsize) {
      dst[r.ax++] = w;
      w >>= 16;
    }
  }
  if (r.ax < dstsize) {
    dst[r.ax] = 0;
  }
  return r;
}
