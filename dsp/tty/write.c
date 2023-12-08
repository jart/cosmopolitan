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
#include "dsp/tty/tty.h"
#include "libc/calls/calls.h"
#include "libc/calls/termios.h"
#include "libc/errno.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/termios.h"

/* TODO(jart): DELETE */

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
ssize_t ttywrite(int fd, const void *data, size_t size) {
  ssize_t rc;
  const char *p;
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
