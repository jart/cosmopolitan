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
#include "libc/fmt/itoa.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/errfuns.h"

/**
 * Formats internet address to string.
 *
 * @param af can be AF_INET or AF_INET6
 * @param src is the binary-encoded address, e.g. &addr->sin_addr
 * @param dst is the output string buffer
 * @param size needs to be 16+ for IPv4 and 46+ for IPv6
 * @return dst on success or NULL w/ errno
 */
const char *inet_ntop(int af, const void *src, char *dst, uint32_t size) {
  char *p;
  int i, t, a, b, c, d;
  const unsigned char *ip;
  p = dst;
  if (!size) return dst;
  if ((ip = src)) {
    if (af == AF_INET) {
      if (size >= 16) {
        p = FormatUint32(p, ip[0]);
        *p++ = '.';
        p = FormatUint32(p, ip[1]);
        *p++ = '.';
        p = FormatUint32(p, ip[2]);
        *p++ = '.';
        p = FormatUint32(p, ip[3]);
        *p = '\0';
        return dst;
      } else {
        enospc();
      }
    } else if (af == AF_INET6) {
      if (size >= 46) {
        t = 0;
        i = 0;
        for (i = 0; i < 16; i += 2) {
          switch (t) {
            case 0:
              if (!ip[i] && !ip[i + 1]) {
                t = 1;
                *p++ = ':';
                *p++ = ':';
                continue;
              } else if (i) {
                *p++ = ':';
              }
              break;
            case 1:
              if (!ip[i] && !ip[i + 1]) {
                continue;
              } else {
                t = 2;
              }
              break;
            case 2:
              *p++ = ':';
              break;
            default:
              __builtin_unreachable();
          }
          a = (ip[i + 0] & 0xF0) >> 4;
          b = (ip[i + 0] & 0x0F) >> 0;
          c = (ip[i + 1] & 0xF0) >> 4;
          d = (ip[i + 1] & 0x0F) >> 0;
          if (a) *p++ = "0123456789abcdef"[a];
          if (a || b) *p++ = "0123456789abcdef"[b];
          if (a || b || c) *p++ = "0123456789abcdef"[c];
          *p++ = "0123456789abcdef"[d];
        }
        *p = '\0';
        return dst;
      } else {
        enospc();
      }
    } else {
      eafnosupport();
    }
  } else {
    einval();
  }
  return 0;
}
