/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2026 Justine Alexandra Roberts Tunney                              │
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
#include "libc/ctype.h"
#include "libc/str/str.h"
#include "net/http/ip.h"

/**
 * Parse IPv6 network address.
 *
 * For example `2001:db8::/32` returns the 16 address bytes and a cidr
 * of 32. If the prefix length is omitted then 128 is used. Like
 * ParseCidr(), host bits that extend past the prefix length are kept.
 *
 * @param n if -1 implies strlen
 * @return addr receives network-order address on success
 * @return cidr is number of bits in network, or -1 on failure
 */
struct Cidr6 ParseCidr6(const char *s, size_t n) {
  size_t i, a;
  const char *p;
  struct Cidr6 c;
  c.cidr = -1;
  if (n == -1)
    n = s ? strlen(s) : 0;
  if ((p = memchr(s, '/', n))) {
    a = p - s;
    if (!ParseIp6(s, a, c.addr)) {
      c.cidr = 0;
      for (i = a + 1; i < n; ++i) {
        if (isdigit(s[i])) {
          c.cidr *= 10;
          c.cidr += s[i] - '0';
          if (c.cidr > 128) {
            c.cidr = -1;
            break;
          }
        } else {
          c.cidr = -1;
          break;
        }
      }
      if (c.cidr <= 0) {
        c.cidr = -1;
      }
    }
  } else if (!ParseIp6(s, n, c.addr)) {
    c.cidr = 128;
  }
  return c;
}
