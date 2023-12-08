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
#include "libc/str/tab.internal.h"
#include "libc/testlib/testlib.h"

/**
 * Tests that raw memory is equal to numeric representation, e.g.
 *
 *   testlib_hexequals("00010203", "\0\1\2\3", -1ul);
 *
 * @see unhexstr()
 */
bool testlib_hexequals(const char *want, const void *got, size_t n) {
  size_t i;
  const unsigned char *p = (const unsigned char *)got;
  if (!got) return false;
  for (i = 0; i < n; ++i) {
    if (!want[i * 2]) break;
    if (i == n) break;
    if (p[i] != (kHexToInt[want[i * 2 + 0] & 255] * 16 +
                 kHexToInt[want[i * 2 + 1] & 255])) {
      return false;
    }
  }
  return true;
}
