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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"

/**
 * Waits for something to happen on multiple file descriptors at once.
 *
 * @param fds[𝑖].fd should have been created with SOCK_NONBLOCK passed
 *     to socket() or accept4()
 * @param fds[𝑖].events flags can have POLL{IN,OUT,PRI}
 * @param timeout_ms if 0 means don't wait and -1 means wait forever
 * @return number of items fds whose revents field has been set to
 *     nonzero to describe its events, or -1 w/ errno
 * @return fds[𝑖].revents flags can have:
 *     (fds[𝑖].events & POLL{IN,OUT,PRI,HUP,ERR,NVAL})
 * @asyncsignalsafe
 * @see ppoll()
 */
int poll(struct pollfd *fds, uint64_t nfds, int32_t timeout_ms) {
  if (!IsWindows()) {
    return poll$sysv(fds, nfds, timeout_ms);
  } else {
    return poll$nt(fds, nfds, timeout_ms);
  }
}
