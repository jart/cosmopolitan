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
#include "libc/intrin/punpcklwd.h"

/**
 * Interleaves low words.
 *
 *          0  1  2  3  4  5  6  7
 *       B  AA BB CC DD ee ff gg hh
 *       C  II JJ KK LL mm nn oo pp
 *          ├┘ ├┘ ├┘ ├┘
 *          │  │  │  └────────┐
 *          │  │  └─────┐     │
 *          │  └──┐     │     │
 *          ├───┐ ├───┐ ├───┐ ├───┐
 *     → A  AA II BB JJ CC KK DD LL
 *
 * @param 𝑎 [w/o] receives reduced 𝑏 and 𝑐 interleaved
 * @param 𝑏 [r/o] supplies eight words
 * @param 𝑐 [r/o] supplies eight words
 * @mayalias
 */
void(punpcklwd)(uint16_t a[8], const uint16_t b[8], const uint16_t c[8]) {
  a[7] = c[3];
  a[6] = b[3];
  a[5] = c[2];
  a[4] = b[2];
  a[3] = c[1];
  a[2] = b[1];
  a[1] = c[0];
  a[0] = b[0];
}
