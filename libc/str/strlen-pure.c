/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/assert.h"
#include "libc/bits/bits.h"
#include "libc/str/str.h"

static inline noasan size_t strlen_pure_x64(const char *s, size_t i) {
  uint64_t w;
  for (;; i += 8) {
    w = READ64LE(s + i);
    if ((w = ~w & (w - 0x0101010101010101) & 0x8080808080808080)) {
      return i + ((unsigned)__builtin_ctzll(w) >> 3);
    }
  }
}

/**
 * Returns length of NUL-terminated string.
 */
size_t strlen_pure(const char *s) {
  size_t i;
  for (i = 0; (uintptr_t)(s + i) & 7; ++i) {
    if (!s[i]) return i;
  }
  i = strlen_pure_x64(s, i);
  assert(!i || s[0]);
  assert(!s[i]);
  return i;
}
