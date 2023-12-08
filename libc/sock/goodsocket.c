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
#include "libc/calls/struct/timeval.h"
#include "libc/errno.h"
#include "libc/sock/goodsocket.internal.h"
#include "libc/sock/sock.h"
#include "libc/sysv/consts/so.h"
#include "libc/sysv/consts/sol.h"
#include "libc/sysv/consts/tcp.h"

static bool Tune(int fd, int a, int b, int x) {
  if (!b) return false;
  return setsockopt(fd, a, b, &x, sizeof(x)) != -1;
}

/**
 * Returns new socket with modern goodness enabled.
 */
int GoodSocket(int family, int type, int protocol, bool isserver,
               const struct timeval *timeout) {
  int e, fd;
  if ((fd = socket(family, type, protocol)) != -1) {
    e = errno;
    if (isserver) {
      Tune(fd, SOL_TCP, TCP_FASTOPEN, 100);
      Tune(fd, SOL_SOCKET, SO_REUSEADDR, 1);
    } else {
      Tune(fd, SOL_TCP, TCP_FASTOPEN_CONNECT, 1);
    }
    errno = e;
    if (!Tune(fd, SOL_TCP, TCP_QUICKACK, 1)) {
      e = errno;
      Tune(fd, SOL_TCP, TCP_NODELAY, 1);
      errno = e;
    }
    if (timeout) {
      e = errno;
      if (timeout->tv_sec < 0) {
        Tune(fd, SOL_SOCKET, SO_KEEPALIVE, 1);
        Tune(fd, SOL_TCP, TCP_KEEPIDLE, -timeout->tv_sec);
        Tune(fd, SOL_TCP, TCP_KEEPINTVL, -timeout->tv_sec);
      } else {
        setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, timeout, sizeof(*timeout));
        setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, timeout, sizeof(*timeout));
      }
      errno = e;
    }
  }
  return fd;
}
