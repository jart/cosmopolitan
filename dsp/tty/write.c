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
#include "dsp/tty/tty.h"
#include "libc/calls/calls.h"
#include "libc/calls/termios.h"
#include "libc/errno.h"
#include "libc/sock/sock.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/termios.h"

/**
 * Sends data to teletypewriter the pain-free way.
 *
 * This function blocks forever until the full amount is transmitted,
 * regardless of whether or not the character device is in non-blocking
 * mode, regardless of whether or not it's interrupted by a signal. It
 * can still be escaped by longjmp'ing out of a signal handler.
 *
 * @return 0 on success, or -1 w/ errno
 */
int ttywrite(int fd, const void *data, size_t size) {
  char *p;
  ssize_t rc;
  size_t wrote, n;
  p = data;
  n = size;
  do {
  TryAgain:
    if ((rc = write(fd, p, n)) != -1) {
      wrote = rc;
      p += wrote;
      n -= wrote;
    } else if (errno == EINTR) {
      goto TryAgain;
    } else if (errno == EAGAIN) {
      poll((struct pollfd[]){{fd, POLLOUT}}, 1, -1);
    } else {
      return -1;
    }
  } while (n);
  return 0;
}
