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
#include "libc/assert.h"
#include "libc/calls/internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/timeval.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/sock/select.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/sock/struct/pollfd.internal.h"
#include "libc/stdckdint.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/errfuns.h"
#ifdef __x86_64__

int sys_select_nt(int nfds, fd_set *readfds, fd_set *writefds,
                  fd_set *exceptfds, struct timeval *timeout,
                  const sigset_t *sigmask) {
  int i, pfds, events, fdcount;

  // convert bitsets to pollfd
  struct pollfd fds[64];
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
  uint32_t millis;
  if (!timeout) {
    millis = -1;
  } else {
    int64_t ms = timeval_tomillis(*timeout);
    if (ms < 0 || ms > UINT32_MAX) {
      millis = -1u;
    } else {
      millis = ms;
    }
  }

  // call our nt poll implementation
  fdcount = sys_poll_nt(fds, pfds, &millis, sigmask);
  unassert(fdcount < 64);
  if (fdcount < 0) return -1;

  // convert pollfd back to bitsets
  if (readfds) FD_ZERO(readfds);
  if (writefds) FD_ZERO(writefds);
  if (exceptfds) FD_ZERO(exceptfds);
  int bits = 0;
  for (i = 0; i < pfds; ++i) {
    if (fds[i].revents & POLLIN) {
      if (readfds) {
        FD_SET(fds[i].fd, readfds);
        ++bits;
      }
    }
    if (fds[i].revents & POLLOUT) {
      if (writefds) {
        FD_SET(fds[i].fd, writefds);
        ++bits;
      }
    }
    if (fds[i].revents & (POLLERR | POLLNVAL)) {
      if (exceptfds) {
        FD_SET(fds[i].fd, exceptfds);
        ++bits;
      }
    }
  }

  // store remaining time back in caller's timeval
  if (timeout) {
    *timeout = timeval_frommillis(millis);
  }

  return bits;
}

#endif /* __x86_64__ */
