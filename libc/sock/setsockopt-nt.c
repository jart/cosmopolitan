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
#include "libc/calls/struct/timeval.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/nt/struct/linger.h"
#include "libc/sock/internal.h"
#include "libc/sock/struct/linger.h"
#include "libc/sock/syscall_fd.internal.h"
#include "libc/sysv/consts/so.h"
#include "libc/sysv/consts/sol.h"
#include "libc/sysv/errfuns.h"

textwindows int sys_setsockopt_nt(struct Fd *fd, int level, int optname,
                                  const void *optval, uint32_t optlen) {
  int64_t ms, micros;
  struct timeval *tv;
  struct linger *linger;
  struct SockFd *sockfd;
  union {
    uint32_t millis;
    struct linger_nt linger;
  } u;

  if (level == SOL_SOCKET) {
    if (optname == SO_LINGER && optval && optlen == sizeof(struct linger)) {
      linger = optval;
      u.linger.l_onoff = linger->l_onoff;
      u.linger.l_linger = MIN(0xFFFF, MAX(0, linger->l_linger));
      optval = &u.linger;
      optlen = sizeof(u.linger);
    } else if ((optname == SO_RCVTIMEO || optname == SO_SNDTIMEO) && optval &&
               optlen == sizeof(struct timeval)) {
      tv = optval;
      if (__builtin_mul_overflow(tv->tv_sec, 1000, &ms) ||
          __builtin_add_overflow(tv->tv_usec, 999, &micros) ||
          __builtin_add_overflow(ms, micros / 1000, &ms) ||
          (ms < 0 || ms > 0xffffffff)) {
        u.millis = 0xffffffff;
      } else {
        u.millis = ms;
      }
      optval = &u.millis;
      optlen = sizeof(u.millis);
      sockfd = (struct SockFd *)fd->extra;
      if (optname == SO_RCVTIMEO) {
        sockfd->rcvtimeo = u.millis;
      }
      if (optname == SO_SNDTIMEO) {
        sockfd->sndtimeo = u.millis;
      }
      return 0;
    }
  }

  if (__sys_setsockopt_nt(fd->handle, level, optname, optval, optlen) != -1) {
    return 0;
  } else {
    return __winsockerr();
  }
}
