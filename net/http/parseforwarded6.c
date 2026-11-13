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
#include "net/http/http.h"
#include "net/http/ip.h"

/**
 * Parses IPv6 address from X-Forwarded-For.
 *
 * Only the last element is examined. It may be a bare IPv6 literal
 * like `2001:db8::1`, or an address with a port in brackets like
 * `[2001:db8::1]:8080`. The port defaults to zero if absent.
 *
 * This doesn't handle IPv4 elements; use ParseForwarded() for those.
 *
 * @param s is input data
 * @param n if -1 implies strlen
 * @param ip receives last/right ip on success if not NULL
 * @param port receives port on success if not NULL
 * @return 0 on success or -1 on failure
 * @see ParseForwarded(), ParseIp6()
 */
int ParseForwarded6(const char *s, size_t n, uint8_t ip[16], uint16_t *port) {
  int c, t;
  size_t i;
  uint8_t addr[16];
  const char *r, *p;
  if (n == -1)
    n = s ? strlen(s) : 0;
  if (!n) {
    return -1;
  }
  if ((r = memrchr(s, ',', n))) {
    i = r - s + 1;
    if (i < n && s[i] == ' ') {
      ++i;
    }
    s += i;
    n -= i;
  }
  t = 0;
  if (n && s[0] == '[') {
    if (!(r = memchr(s + 1, ']', n - 1))) {
      return -1;
    }
    if (ParseIp6(s + 1, r - s - 1, addr)) {
      return -1;
    }
    p = r + 1;
    n -= p - s;
    if (n) {
      if (*p++ != ':') {
        return -1;
      }
      --n;
      while (n--) {
        c = *p++ & 255;
        if (!isdigit(c)) {
          return -1;
        }
        t *= 10;
        t += c - '0';
        if (t > 65535) {
          return -1;
        }
      }
    }
  } else {
    if (ParseIp6(s, n, addr)) {
      return -1;
    }
  }
  if (ip) {
    memcpy(ip, addr, 16);
  }
  if (port) {
    *port = t;
  }
  return 0;
}
