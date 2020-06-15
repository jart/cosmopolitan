/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/internal.h"
#include "libc/nt/struct/pollfd.h"
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/errfuns.h"

textwindows int poll$nt(struct pollfd *fds, uint64_t nfds, int32_t timeout_ms) {
  int got;
  size_t i;
  struct pollfd$nt ntfds[64];
  if (nfds > 64) return einval();
  for (i = 0; i < nfds; ++i) {
    if (!isfdkind(fds[i].fd, kFdSocket)) return ebadf();
    ntfds[i].handle = g_fds.p[fds[i].fd].handle;
    ntfds[i].events = fds[i].events & (POLLPRI | POLLIN | POLLOUT);
  }
  if ((got = WSAPoll(ntfds, nfds, timeout_ms)) != -1) {
    for (i = 0; i < nfds; ++i) {
      fds[i].revents = ntfds[i].revents;
    }
    return got;
  } else {
    return winsockerr();
  }
}
