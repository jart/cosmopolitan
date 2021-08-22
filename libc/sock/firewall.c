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
#include "libc/bits/weaken.h"
#include "libc/dce.h"
#include "libc/log/log.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/sysv/consts/af.h"
#include "net/http/ip.h"

/**
 * Checks outbound address against firewall.
 *
 * The goal is to keep local software local, by raising an error if our
 * Makefile tries to talk to the Internet.
 */
void _firewall(const void *addr, uint32_t addrsize) {
  char b[64], *p;
  if (!IsTiny() && addr && addrsize >= sizeof(struct sockaddr_in) &&
      ((struct sockaddr_in *)addr)->sin_family == AF_INET &&
      IsPublicIp(ntohl(((struct sockaddr_in *)addr)->sin_addr.s_addr)) &&
      IsRunningUnderMake()) {
    p = stpcpy(b, "make shan't internet: ");
    p = stpcpy(p, inet_ntoa(((struct sockaddr_in *)addr)->sin_addr));
    *p++ = '\n';
    write(2, b, p - b);
    if (weaken(__die)) weaken(__die)();
    abort();
  }
}
