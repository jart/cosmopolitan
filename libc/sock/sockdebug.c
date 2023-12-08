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
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/macros.internal.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/sock/struct/sockaddr6.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/sock.h"

const char *(DescribeSockaddr)(char buf[128], const struct sockaddr *sa,
                               size_t sasize) {
  int e;
  size_t n;
  char *p, ip[72];
  e = errno;
  stpcpy(buf, "NULL");
  if (sa && sasize >= sizeof(sa->sa_family)) {
    stpcpy(buf, DescribeSocketFamily(sa->sa_family));
    if (sa->sa_family == AF_INET && sasize >= sizeof(struct sockaddr_in)) {
      const struct sockaddr_in *in;
      in = (const struct sockaddr_in *)sa;
      if (inet_ntop(AF_INET, &in->sin_addr, ip, sizeof(ip))) {
        p = buf;
        p = stpcpy(p, ip);
        *p++ = ':';
        p = FormatUint32(p, ntohs(in->sin_port));
      }
    } else if (sa->sa_family == AF_INET6 &&
               sasize >= sizeof(struct sockaddr_in6)) {
      const struct sockaddr_in6 *in6;
      in6 = (const struct sockaddr_in6 *)sa;
      if (inet_ntop(AF_INET6, &in6->sin6_addr, ip, sizeof(ip))) {
        p = buf;
        *p++ = '[';
        p = stpcpy(p, ip);
        *p++ = ']';
        *p++ = ':';
        p = FormatUint32(p, in6->sin6_port);
      }
    } else if (sa->sa_family == AF_UNIX &&
               sasize >= sizeof(struct sockaddr_un)) {
      const struct sockaddr_un *unix;
      unix = (const struct sockaddr_un *)sa;
      n = strnlen(unix->sun_path, sizeof(unix->sun_path));
      n = MIN(n, 128 - 1);
      if (n) memcpy(buf, unix->sun_path, n);
      buf[n] = 0;
    }
  }
  errno = e;
  return buf;
}
