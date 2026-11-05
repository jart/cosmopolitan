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
#include "libc/sock/sock.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "net/http/ip.h"

/**
 * Returns true if tail is a dotted-quad IPv4 address.
 *
 * This rejects the empty octets, trailing dots, and leading zeros that
 * inet_pton(AF_INET) currently tolerates, so that `::ffff:1..3.4` and
 * `::ffff:1.2.3.04` aren't accepted.
 */
static bool IsStrictIpv4(const char *s, size_t n) {
  int fields, digits;
  unsigned val;
  size_t i, start;
  for (fields = digits = val = i = start = 0; i < n; ++i) {
    if ('0' <= s[i] && s[i] <= '9') {
      val = val * 10 + (s[i] - '0');
      if (++digits > 3 || val > 255) {
        return false;
      }
    } else if (s[i] == '.') {
      if (!digits || (digits > 1 && s[start] == '0')) {
        return false;
      }
      digits = val = 0;
      ++fields;
      start = i + 1;
    } else {
      return false;
    }
  }
  if (!digits || (digits > 1 && s[start] == '0')) {
    return false;
  }
  return ++fields == 4;
}

/**
 * Parse IPv6 host address.
 *
 * This supports the full RFC4291 textual representation, including
 * `::` compression and embedded IPv4 addresses like `::ffff:1.2.3.4`.
 * Brackets and zone identifiers are not accepted; strip them first.
 *
 * @param n if -1 implies strlen
 * @param out receives 16 bytes of network-order address on success
 * @return 0 on success or -1 on failure
 * @see ParseIp(), ParseCidr6()
 */
int ParseIp6(const char *s, size_t n, uint8_t out[16]) {
  char buf[64];
  const char *colon;
  if (n == -1)
    n = s ? strlen(s) : 0;
  if (!n || n >= sizeof(buf) || memchr(s, '\0', n)) {
    return -1;
  }
  memcpy(buf, s, n);
  buf[n] = 0;
  if (strchr(buf, '.')) {
    colon = strrchr(buf, ':');
    if (!colon || !IsStrictIpv4(colon + 1, buf + n - (colon + 1))) {
      return -1;
    }
  }
  return inet_pton(AF_INET6, buf, out) == 1 ? 0 : -1;
}
