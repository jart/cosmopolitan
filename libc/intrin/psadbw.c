/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/intrin/psadbw.h"
#include "libc/macros.h"

/**
 * Computes sum of absolute differences.
 *
 * @param 𝑎 [w/o] receives sum at first index and rest is zero'd
 * @param 𝑏 [r/o] is your first unsigned byte array
 * @param 𝑐 [r/o] is your second unsigned byte array
 * @mayalias
 */
void(psadbw)(uint64_t a[2], const uint8_t b[16], const uint8_t c[16]) {
  unsigned i, x, y;
  for (x = i = 0; i < 8; ++i) x += ABS(b[i] - c[i]);
  for (y = 0; i < 16; ++i) y += ABS(b[i] - c[i]);
  a[0] = x;
  a[1] = y;
}
