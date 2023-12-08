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
#include "libc/fmt/conv.h"
#include "libc/intrin/packsswb.h"
#include "libc/intrin/pandn.h"
#include "libc/intrin/pcmpgtw.h"
#include "libc/intrin/pmovmskb.h"
#include "libc/str/str.h"
#include "libc/str/utf16.h"

static const int16_t kDel16[8] = {127, 127, 127, 127, 127, 127, 127, 127};

/* 10x speedup for ascii */
static axdx_t tprecode16to8_sse2(char *dst, size_t dstsize, const char16_t *src,
                                 axdx_t r) {
  int16_t v1[8], v2[8], v3[8], vz[8];
  memset(vz, 0, 16);
  while (r.ax + 8 < dstsize) {
    memcpy(v1, src + r.dx, 16);
    pcmpgtw(v2, v1, vz);
    pcmpgtw(v3, v1, kDel16);
    pandn((void *)v2, (void *)v3, (void *)v2);
    if (pmovmskb((void *)v2) != 0xFFFF) break;
    packsswb((void *)v1, v1, v1);
    memcpy(dst + r.ax, v1, 8);
    r.ax += 8;
    r.dx += 8;
  }
  return r;
}

/**
 * Transcodes UTF-16 to UTF-8.
 *
 * This is a low-level function intended for the core runtime. Use
 * utf16to8() for a much better API that uses malloc().
 *
 * @param dst is output buffer
 * @param dstsize is bytes in dst
 * @param src is NUL-terminated UTF-16 input string
 * @return ax bytes written excluding nul
 * @return dx index of character after nul word in src
 */
axdx_t tprecode16to8(char *dst, size_t dstsize, const char16_t *src) {
  axdx_t r;
  uint64_t w;
  wint_t x, y;
  r.ax = 0;
  r.dx = 0;
  for (;;) {
#if defined(__x86_64__) && !IsModeDbg() && !IsTiny()
    if (!((uintptr_t)(src + r.dx) & 15)) {
      r = tprecode16to8_sse2(dst, dstsize, src, r);
    }
#endif
    if (!(x = src[r.dx++])) break;
    if (IsUtf16Cont(x)) continue;
    if (!IsUcs2(x)) {
      if (!(y = src[r.dx++])) break;
      x = MergeUtf16(x, y);
    }
    w = tpenc(x);
    while (w && r.ax + 1 < dstsize) {
      dst[r.ax++] = w & 0xFF;
      w >>= 8;
    }
  }
  if (r.ax < dstsize) {
    dst[r.ax] = 0;
  }
  return r;
}
