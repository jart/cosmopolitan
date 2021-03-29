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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/sock/internal.h"
#include "libc/sysv/consts/sock.h"

int sys_socket(int family, int type, int protocol) {
  static bool once, demodernize;
  int sock, olderr;
  if (!once && (type & (SOCK_CLOEXEC | SOCK_NONBLOCK))) {
    if (IsXnu()) {
      demodernize = true;
      once = true;
    } else {
      olderr = errno;
      if ((sock = __sys_socket(family, type, protocol)) != -1) {
        once = true;
        return sock;
      } else {
        errno = olderr;
        demodernize = true;
        once = true;
      }
    }
  }
  if (!demodernize) {
    return __sys_socket(family, type, protocol);
  } else {
    return __fixupnewsockfd(
        __sys_socket(family, type & ~(SOCK_CLOEXEC | SOCK_NONBLOCK), protocol),
        type);
  }
}
