/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/sock/select.h"
#include "libc/calls/struct/timeval.h"

/**
 * Checks status on multiple file descriptors at once.
 *
 * @param readfds may be used to be notified when you can call read() on
 *     a file descriptor without it blocking; this includes when data is
 *     is available to be read as well as eof and error conditions
 * @param writefds may be used to be notified when write() may be called
 *     on a file descriptor without it blocking
 * @param exceptfds may be used to be notified of exceptional conditions
 *     such as out-of-band data on a socket; it is equivalent to POLLPRI
 *     in the revents of poll()
 * @param timeout may be null which means to block indefinitely; cosmo's
 *     implementation of select() never modifies this parameter
 * @raise E2BIG if we exceeded the 64 socket limit on Windows
 * @raise ECANCELED if thread was cancelled in masked mode
 * @raise EINTR if signal was delivered
 * @cancelationpoint
 * @asyncsignalsafe
 * @norestart
 */
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
           struct timeval *timeout) {
  struct timespec ts;
  struct timespec *tsp;
  if (timeout) {
    ts = timeval_totimespec(*timeout);
    tsp = &ts;
  } else {
    tsp = 0;
  }
  return pselect(nfds, readfds, writefds, exceptfds, tsp, 0);
}
