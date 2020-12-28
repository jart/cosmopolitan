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
