/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/safemacros.internal.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/nexgen32e/uart.internal.h"
#include "libc/runtime/pc.internal.h"
#include "libc/sock/internal.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/sysv/consts/poll.h"
#ifdef __x86_64__

int sys_poll_metal(struct pollfd *fds, size_t nfds, unsigned timeout_ms) {
  int rc;
  size_t i;
  bool blocking;
  uint64_t start, timeout;
  if (!timeout_ms) {
    start = 0;
    timeout = 0;
    blocking = false;
  } else {
    start = rdtsc();
    timeout = timeout_ms;
    timeout *= 3; /* approx. cycles to nanoseconds */
    timeout *= 1000000;
    blocking = true;
  }
  for (rc = 0;;) {
    for (i = 0; i < nfds; ++i) {
      fds[i].revents = 0;
      if (fds[i].fd >= 0) {
        if (__isfdopen(fds[i].fd)) {
          switch (g_fds.p[fds[i].fd].kind) {
            case kFdSerial:
              if ((fds[i].events & POLLIN) &&
                  (inb(g_fds.p[fds[i].fd].handle + UART_LSR) & UART_TTYDA)) {
                fds[i].revents |= POLLIN;
              }
              if ((fds[i].events & POLLOUT) &&
                  (inb(g_fds.p[fds[i].fd].handle + UART_LSR) & UART_TTYTXR)) {
                fds[i].revents |= POLLOUT;
              }
              break;
            case kFdFile:
              if (fds[i].events & (POLLIN | POLLOUT)) {
                fds[i].revents |= fds[i].events & (POLLIN | POLLOUT);
              }
              break;
            default:
              fds[i].revents = POLLNVAL;
              break;
          }
        } else {
          fds[i].revents = POLLNVAL;
        }
      }
      if (fds[i].revents) ++rc;
    }
    if (rc || !blocking || unsignedsubtract(rdtsc(), start) >= timeout) {
      break;
    } else {
      __builtin_ia32_pause();
    }
  }
  return rc;
}

#endif
