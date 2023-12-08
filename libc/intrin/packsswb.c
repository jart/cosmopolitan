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
#include "libc/intrin/packsswb.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/str/str.h"

/**
 * Casts shorts to signed chars w/ saturation.
 *
 *   𝑎 ← {CLAMP[𝑏ᵢ]|𝑖∈[0,4)} ║ {CLAMP[𝑐ᵢ]|𝑖∈[4,8)}
 *
 * @see packuswb()
 * @mayalias
 */
void(packsswb)(int8_t a[16], const int16_t b[8], const int16_t c[8]) {
  unsigned i;
  int8_t r[16];
  for (i = 0; i < 8; ++i) r[i + 0] = MIN(INT8_MAX, MAX(INT8_MIN, b[i]));
  for (i = 0; i < 8; ++i) r[i + 8] = MIN(INT8_MAX, MAX(INT8_MIN, c[i]));
  __builtin_memcpy(a, r, 16);
}
