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
#include "libc/stdckdint.h"
#include "libc/str/str.h"
#include "net/http/ip.h"

/**
 * Parse IPv4 host address.
 *
 * @param n if -1 implies strlen
 * @return -1 on failure, otherwise 32-bit host-order unsigned integer
 * @see ParseCidr()
 */
int64_t ParseIp(const char *s, size_t n) {
  int c, j;
  size_t i;
  unsigned b, x;
  bool dotted = false;
  if (n == -1) n = s ? strlen(s) : 0;
  if (!n) return -1;
  for (b = x = j = i = 0; i < n; ++i) {
    c = s[i] & 255;
    if (isdigit(c)) {
      if (ckd_mul(&b, b, 10) ||       //
          ckd_add(&b, b, c - '0') ||  //
          (b > 255 && dotted)) {
        return -1;
      }
    } else if (c == '.') {
      if (b > 255) return -1;
      dotted = true;
      x <<= 8;
      x |= b;
      b = 0;
      ++j;
    } else {
      return -1;
    }
  }
  x <<= 8;
  x |= b;
  return x;
}
