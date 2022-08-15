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
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/timeval.h"
#include "libc/macros.internal.h"
#include "libc/sock/select.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/sock/struct/pollfd.internal.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/errfuns.h"

int sys_select_nt(int nfds, fd_set *readfds, fd_set *writefds,
                  fd_set *exceptfds, struct timeval *timeout,
                  const sigset_t *sigmask) {
  uint64_t millis;
  int i, pfds, events, fdcount;
  struct pollfd fds[64];

  // convert bitsets to pollfd
  for (pfds = i = 0; i < nfds; ++i) {
    events = 0;
    if (readfds && FD_ISSET(i, readfds)) events |= POLLIN;
    if (writefds && FD_ISSET(i, writefds)) events |= POLLOUT;
    if (exceptfds && FD_ISSET(i, exceptfds)) events |= POLLERR;
    if (events) {
      if (pfds < ARRAYLEN(fds)) {
        fds[pfds].fd = i;
        fds[pfds].events = events;
        fds[pfds].revents = 0;
        pfds += 1;
      } else {
        return enomem();
      }
    }
  }

  // convert the wait time to a word
  if (!timeout || __builtin_add_overflow(timeout->tv_sec,
                                         timeout->tv_usec / 1000, &millis)) {
    millis = -1;
  }

  // call our nt poll implementation
  fdcount = sys_poll_nt(fds, pfds, &millis, sigmask);
  if (fdcount == -1) return -1;

  // convert pollfd back to bitsets
  if (readfds) FD_ZERO(readfds);
  if (writefds) FD_ZERO(writefds);
  if (exceptfds) FD_ZERO(exceptfds);
  for (i = 0; i < fdcount; ++i) {
    if (fds[i].revents & POLLIN) FD_SET(fds[i].fd, readfds);
    if (fds[i].revents & POLLOUT) FD_SET(fds[i].fd, writefds);
    if (fds[i].revents & (POLLERR | POLLNVAL)) FD_SET(fds[i].fd, exceptfds);
  }

  // store remaining time back in caller's timeval
  if (timeout) {
    timeout->tv_sec = millis / 1000;
    timeout->tv_usec = millis % 1000 * 1000;
  }

  return fdcount;
}
