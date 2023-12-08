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
#include "libc/assert.h"
#include "libc/dce.h"
#include "libc/str/str.h"
#ifndef __aarch64__

static size_t strnlen_x64(const char *s, size_t n, size_t i) {
  uint64_t w;
  for (; i + 8 < n; i += 8) {
    w = *(uint64_t *)(s + i);
    if ((w = ~w & (w - 0x0101010101010101) & 0x8080808080808080)) {
      i += (unsigned)__builtin_ctzll(w) >> 3;
      break;
    }
  }
  return i;
}

/**
 * Returns length of NUL-terminated string w/ limit.
 *
 * @param s is string
 * @param n is max length
 * @return byte length
 * @asyncsignalsafe
 */
size_t strnlen(const char *s, size_t n) {
  size_t i;
  for (i = 0; (uintptr_t)(s + i) & 7; ++i) {
    if (i == n || !s[i]) return i;
  }
  i = strnlen_x64(s, n, i);
  for (;; ++i) {
    if (i == n || !s[i]) break;
  }
  unassert(i == n || (i < n && !s[i]));
  return i;
}

#endif /* __aarch64__ */
