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
#include "libc/intrin/palignr.h"
#include "libc/assert.h"
#include "libc/macros.internal.h"

/**
 * Overlaps vectors.
 *
 *     𝑖= 0 means 𝑐←𝑎
 *   0<𝑖<16 means 𝑐←𝑎║𝑏
 *     𝑖=16 means 𝑐←𝑏
 *  16<𝑖<32 means 𝑐←𝑏║0
 *     𝑖≥32 means 𝑐←0
 *
 * @param 𝑖 goes faster as constexpr
 * @note not compatible with mmx
 * @see pvalignr()
 * @mayalias
 */
void(palignr)(void *c, const void *b, const void *a, unsigned long i) {
  char t[48];
  __builtin_memcpy(t, a, 16);
  __builtin_memcpy(t + 16, b, 16);
  __builtin_memset(t + 32, 0, 16);
  __builtin_memcpy(c, t + MIN(i, 32), 16);
}
