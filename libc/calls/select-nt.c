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
#include "libc/calls/internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/timeval.h"
#include "libc/limits.h"
#include "libc/macros.h"
#include "libc/sock/select.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/sock/struct/pollfd.internal.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/errfuns.h"
#ifdef __x86_64__

// <sync libc/sysv/consts.sh>
#define POLLERR_    0x0001  // implied in events
#define POLLHUP_    0x0002  // implied in events
#define POLLNVAL_   0x0004  // implied in events
#define POLLIN_     0x0300
#define POLLRDNORM_ 0x0100
#define POLLRDBAND_ 0x0200
#define POLLOUT_    0x0010
#define POLLWRNORM_ 0x0010
#define POLLWRBAND_ 0x0020  // MSDN undocumented
#define POLLPRI_    0x0400  // MSDN unsupported
// </sync libc/sysv/consts.sh>

int sys_select_nt(int nfds, fd_set *readfds, fd_set *writefds,
                  fd_set *exceptfds, const struct timespec *timeout,
                  const sigset_t *sigmask) {
  int pfds = 0;

  // convert bitsets to pollfd
  struct pollfd fds[128];
  for (int fd = 0; fd < nfds; ++fd) {
    int events = 0;
    if (readfds && FD_ISSET(fd, readfds))
      events |= POLLIN_;
    if (writefds && FD_ISSET(fd, writefds))
      events |= POLLOUT_;
    if (exceptfds && FD_ISSET(fd, exceptfds))
      events |= POLLPRI_;
    if (events) {
      if (pfds == ARRAYLEN(fds))
        return e2big();
      fds[pfds].fd = fd;
      fds[pfds].events = events;
      fds[pfds].revents = 0;
      ++pfds;
    }
  }

  // call our nt poll implementation
  int fdcount = sys_poll_nt(fds, pfds, timeout, sigmask);
  if (fdcount == -1)
    return -1;

  // convert pollfd back to bitsets
  if (readfds)
    FD_ZERO(readfds);
  if (writefds)
    FD_ZERO(writefds);
  if (exceptfds)
    FD_ZERO(exceptfds);
  int bits = 0;
  for (int i = 0; i < pfds; ++i) {
    if (fds[i].revents & (POLLIN_ | POLLHUP_ | POLLERR_ | POLLNVAL_)) {
      if (readfds) {
        FD_SET(fds[i].fd, readfds);
        ++bits;
      }
    }
    if (fds[i].revents & POLLOUT_) {
      if (writefds) {
        FD_SET(fds[i].fd, writefds);
        ++bits;
      }
    }
    if (fds[i].revents & POLLPRI_) {
      if (exceptfds) {
        FD_SET(fds[i].fd, exceptfds);
        ++bits;
      }
    }
  }

  return bits;
}

#endif /* __x86_64__ */
