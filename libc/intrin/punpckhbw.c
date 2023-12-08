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
#include "libc/intrin/punpckhbw.h"

/**
 * Interleaves high bytes.
 *
 * @param 𝑎 [w/o] receives reduced 𝑏 and 𝑐 interleaved
 * @param 𝑏 [r/o] supplies eight words
 * @param 𝑐 [r/o] supplies eight words
 * @mayalias
 */
void(punpckhbw)(uint8_t a[16], const uint8_t b[16], const uint8_t c[16]) {
  a[0x0] = b[0x8];
  a[0x1] = c[0x8];
  a[0x2] = b[0x9];
  a[0x3] = c[0x9];
  a[0x4] = b[0xa];
  a[0x5] = c[0xa];
  a[0x6] = b[0xb];
  a[0x7] = c[0xb];
  a[0x8] = b[0xc];
  a[0x9] = c[0xc];
  a[0xa] = b[0xd];
  a[0xb] = c[0xd];
  a[0xc] = b[0xe];
  a[0xd] = c[0xe];
  a[0xe] = b[0xf];
  a[0xf] = c[0xf];
}
