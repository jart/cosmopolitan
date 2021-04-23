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
#include "libc/macros.internal.h"
#include "libc/sock/internal.h"
#include "libc/sysv/consts/so.h"
#include "libc/sysv/errfuns.h"

struct linger_nt {   /* Linux+XNU+BSD ABI */
  uint16_t l_onoff;  /* on/off */
  uint16_t l_linger; /* seconds */
};

textwindows int sys_setsockopt_nt(struct Fd *fd, int level, int optname,
                                  const void *optval, uint32_t optlen) {
  struct timeval *tv;
  struct linger *linger;
  union {
    uint32_t millis;
    struct linger_nt linger;
  } nt;

  if (optname == SO_LINGER && optval && optlen == sizeof(struct linger)) {
    linger = optval;
    nt.linger.l_onoff = linger->l_onoff;
    nt.linger.l_linger = MIN(0xFFFF, MAX(0, linger->l_linger));
    optval = &nt.linger;
    optlen = sizeof(nt.linger);
  }

  if ((optname == SO_RCVTIMEO || optname == SO_SNDTIMEO) && optval &&
      optlen == sizeof(struct timeval)) {
    tv = optval;
    nt.millis = MIN(0xFFFFFFFF, MAX(0, tv->tv_sec * 1000 + tv->tv_usec / 1000));
    optval = &nt.millis;
    optlen = sizeof(nt.millis);
  }

  if (__sys_setsockopt_nt(fd->handle, level, optname, optval, optlen) != -1) {
    return 0;
  } else {
    return __winsockerr();
  }
}
