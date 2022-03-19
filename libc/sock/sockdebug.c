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
#include "libc/calls/struct/sockaddr6.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/macros.internal.h"
#include "libc/sock/sock.h"
#include "libc/sock/sockdebug.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/sock.h"

const char *__describe_socket_family(int family) {
  static char buf[12];
  if (family == AF_UNIX) return "AF_UNIX";
  if (family == AF_INET) return "AF_INET";
  if (family == AF_INET6) return "AF_INET6";
  FormatInt32(buf, family);
  return buf;
}

const char *__describe_socket_type(int type) {
  int x;
  char *p;
  static char buf[12 + 1 + 12 + 1 + 13 + 1];
  p = buf;
  x = type & ~(SOCK_CLOEXEC | SOCK_NONBLOCK);
  if (x == SOCK_STREAM) {
    p = stpcpy(p, "SOCK_STREAM");
  } else if (x == SOCK_DGRAM) {
    p = stpcpy(p, "SOCK_DGRAM");
  } else if (x == SOCK_RAW) {
    p = stpcpy(p, "SOCK_RAW");
  } else {
    p = FormatInt32(p, x);
  }
  if (type & SOCK_CLOEXEC) p = stpcpy(p, "|SOCK_CLOEXEC");
  if (type & SOCK_NONBLOCK) p = stpcpy(p, "|SOCK_NONBLOCK");
  return buf;
}

const char *__describe_socket_protocol(int family) {
  static char buf[12];
  if (family == IPPROTO_IP) return "IPPROTO_IP";
  if (family == IPPROTO_ICMP) return "IPPROTO_ICMP";
  if (family == IPPROTO_TCP) return "IPPROTO_TCP";
  if (family == IPPROTO_UDP) return "IPPROTO_UDP";
  if (family == IPPROTO_RAW) return "IPPROTO_RAW";
  if (family == IPPROTO_IPV6) return "IPPROTO_IPv6";
  FormatInt32(buf, family);
  return buf;
}

const char *__describe_sockaddr(const struct sockaddr *sa, size_t sasize) {
  int e;
  size_t n;
  uint16_t port;
  char *p, ip[72];
  static char buf[128];
  e = errno;
  stpcpy(buf, "NULL");
  if (sa && sasize >= sizeof(sa->sa_family)) {
    stpcpy(buf, __describe_socket_family(sa->sa_family));
    if (sa->sa_family == AF_INET && sasize >= sizeof(struct sockaddr_in)) {
      const struct sockaddr_in *in;
      in = (const struct sockaddr_in *)sa;
      if (inet_ntop(AF_INET, &in->sin_addr, ip, sizeof(ip))) {
        p = buf;
        p = stpcpy(p, ip);
        *p++ = ':';
        p = FormatUint32(p, in->sin_port);
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
      n = MIN(n, sizeof(buf) - 1);
      memcpy(buf, unix->sun_path, n);
      buf[n] = 0;
    }
  }
  errno = e;
  return buf;
}
