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
#include "libc/bits/bits.h"
#include "libc/calls/internal.h"
#include "libc/macros.internal.h"
#include "libc/nt/struct/pollfd.h"
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/sock/yoink.inc"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/errfuns.h"

textwindows int sys_poll_nt(struct pollfd *fds, uint64_t nfds, uint64_t ms) {
  int i, got, waitfor;
  struct sys_pollfd_nt ntfds[64];
  if (nfds >= ARRAYLEN(ntfds)) return einval();
  for (i = 0; i < nfds; ++i) {
    if (fds[i].fd >= 0) {
      if (!__isfdkind(fds[i].fd, kFdSocket)) return enotsock();
      ntfds[i].handle = g_fds.p[fds[i].fd].handle;
      ntfds[i].events = fds[i].events & (POLLPRI | POLLIN | POLLOUT);
    } else {
      ntfds[i].handle = -1;
      ntfds[i].events = POLLIN;
    }
  }
  for (;;) {
    if (cmpxchg(&__interrupted, true, false)) return eintr();
    waitfor = MIN(1000, ms); /* for ctrl+c */
    if ((got = WSAPoll(ntfds, nfds, waitfor)) != -1) {
      if (!got && (ms -= waitfor) > 0) continue;
      for (i = 0; i < nfds; ++i) {
        fds[i].revents = ntfds[i].handle < 0 ? 0 : ntfds[i].revents;
      }
      return got;
    } else {
      return __winsockerr();
    }
  }
}
