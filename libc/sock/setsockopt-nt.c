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
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/timeval.h"
#include "libc/nt/struct/linger.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/sock/struct/linger.h"
#include "libc/sock/syscall_fd.internal.h"
#include "libc/stdio/sysparam.h"
#include "libc/sysv/consts/so.h"
#include "libc/sysv/consts/sol.h"
#include "libc/sysv/errfuns.h"
#ifdef __x86_64__

__msabi extern typeof(__sys_setsockopt_nt) *const __imp_setsockopt;

textwindows int sys_setsockopt_nt(struct Fd *fd, int level, int optname,
                                  const void *optval, uint32_t optlen) {

  // socket read/write timeouts
  if (level == SOL_SOCKET &&
      (optname == SO_RCVTIMEO || optname == SO_SNDTIMEO)) {
    if (!(optval && optlen == sizeof(struct timeval))) return einval();
    const struct timeval *tv = optval;
    int64_t ms = timeval_tomillis(*tv);
    if (ms >= 0xffffffffu) ms = 0;  // wait forever (default)
    if (optname == SO_RCVTIMEO) fd->rcvtimeo = ms;
    if (optname == SO_SNDTIMEO) fd->sndtimeo = ms;
    return 0;  // we want to handle this on our own
  }

  // how to make close() a blocking i/o call
  union {
    uint32_t millis;
    struct linger_nt linger;
  } u;
  if (level == SOL_SOCKET &&             //
      optname == SO_LINGER && optval &&  //
      optlen == sizeof(struct linger)) {
    const struct linger *linger = optval;
    u.linger.l_onoff = linger->l_onoff;
    u.linger.l_linger = MIN(0xFFFF, MAX(0, linger->l_linger));
    optval = &u.linger;
    optlen = sizeof(u.linger);
  }

  if (__imp_setsockopt(fd->handle, level, optname, optval, optlen) != -1) {
    return 0;
  } else {
    return __winsockerr();
  }
}

#endif /* __x86_64__ */
