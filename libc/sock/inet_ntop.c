/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/sysv/consts/af.h"
#include "libc/sysv/errfuns.h"

/**
 * Formats internet address to string.
 *
 * @param af can be AF_INET
 * @param src is the binary-encoded address, e.g. &addr->sin_addr
 * @param dst is the output string buffer
 * @param size is bytes in dst, which needs 16+ for IPv4
 * @return dst on success or NULL w/ errno
 */
const char *inet_ntop(int af, const void *src, char *dst, uint32_t size) {
  char *p;
  unsigned char *ip4;
  if (src) {
    if (af == AF_INET) {
      if (size >= 16) {
        p = dst;
        ip4 = src;
        p += uint64toarray_radix10(ip4[0], p);
        *p++ = '.';
        p += uint64toarray_radix10(ip4[1], p);
        *p++ = '.';
        p += uint64toarray_radix10(ip4[2], p);
        *p++ = '.';
        p += uint64toarray_radix10(ip4[3], p);
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
  if (size) dst[0] = '\0';
  return NULL;
}
