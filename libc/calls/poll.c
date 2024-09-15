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
#include "libc/calls/struct/timespec.h"
#include "libc/sock/struct/pollfd.h"

/**
 * Checks status on multiple file descriptors at once.
 *
 * Servers that need to handle an unbounded number of client connections
 * should just create a separate thread for each client. poll() isn't a
 * scalable i/o solution on any platform.
 *
 * One of the use cases for poll() is to quickly check if a number of
 * file descriptors are valid. The canonical way to do this is to set
 * events to 0 which prevents blocking and causes only the invalid,
 * hangup, and error statuses to be checked.
 *
 * On XNU, the POLLHUP and POLLERR statuses aren't checked unless either
 * POLLIN, POLLOUT, or POLLPRI are specified in the events field. Cosmo
 * will however polyfill the checking of POLLNVAL on XNU with the events
 * doesn't specify any of the above i/o events.
 *
 * When XNU and BSD OSes report POLLHUP, they will always set POLLIN too
 * when POLLIN is requested, even in cases when there isn't unread data.
 *
 * Your poll() function will check the status of all file descriptors
 * before returning. This function won't block unless none of the fds
 * had had any reportable status.
 *
 * The impact shutdown() will have on poll() is a dice roll across OSes.
 *
 * @param fds[𝑖].fd should be a socket, input pipe, or conosle input
 *     and if it's a negative number then the entry is ignored, plus
 *     revents will be set to zero
 * @param fds[𝑖].events flags can have POLLIN, POLLOUT, POLLPRI,
 *     POLLRDNORM, POLLWRNORM, POLLRDBAND, POLLWRBAND as well as
 *     POLLERR, POLLHUP, and POLLNVAL although the latter are
 *     always implied (assuming fd≥0) so they're ignored here
 * @param timeout_ms if 0 means don't wait and negative waits forever
 * @return number of `fds` whose revents field has been set to a nonzero
 *     number, 0 if the timeout elapsed without events, or -1 w/ errno
 * @return fds[𝑖].revents is always zero initializaed and then will
 *     be populated with POLL{IN,OUT,PRI,HUP,ERR,NVAL} if something
 *     was determined about the file descriptor
 * @raise ECANCELED if thread was cancelled in masked mode
 * @raise EINVAL if `nfds` exceeded `RLIMIT_NOFILE`
 * @raise ENOMEM on failure to allocate memory
 * @raise EINTR if signal was delivered
 * @cancelationpoint
 * @asyncsignalsafe
 * @norestart
 */
int poll(struct pollfd *fds, size_t nfds, int timeout_ms) {
  struct timespec ts;
  struct timespec *tsp;
  if (timeout_ms >= 0) {
    ts.tv_sec = timeout_ms / 1000;
    ts.tv_nsec = timeout_ms % 1000 * 1000000;
    tsp = &ts;
  } else {
    tsp = 0;
  }
  return ppoll(fds, nfds, tsp, 0);
}
