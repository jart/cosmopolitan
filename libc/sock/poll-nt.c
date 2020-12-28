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
#include "libc/calls/internal.h"
#include "libc/nt/struct/pollfd.h"
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/sock/yoink.inc"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/errfuns.h"

textwindows int poll$nt(struct pollfd *fds, uint64_t nfds, int32_t timeout_ms) {
  int got;
  size_t i;
  struct pollfd$nt ntfds[64];
  if (nfds > 64) return einval();
  for (i = 0; i < nfds; ++i) {
    if (!__isfdkind(fds[i].fd, kFdSocket)) return ebadf();
    ntfds[i].handle = g_fds.p[fds[i].fd].handle;
    ntfds[i].events = fds[i].events & (POLLPRI | POLLIN | POLLOUT);
  }
  if ((got = WSAPoll(ntfds, nfds, timeout_ms)) != -1) {
    for (i = 0; i < nfds; ++i) {
      fds[i].revents = ntfds[i].revents;
    }
    return got;
  } else {
    return __winsockerr();
  }
}
