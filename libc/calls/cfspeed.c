/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/termios.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/errfuns.h"

/**
 * Returns input baud rate.
 * @asyncsignalsafe
 */
uint32_t cfgetispeed(const struct termios *t) {
  if (CBAUD) {
    return t->c_cflag & CBAUD;
  } else {
    return t->c_ispeed;
  }
}

/**
 * Returns output baud rate.
 * @asyncsignalsafe
 */
uint32_t cfgetospeed(const struct termios *t) {
  if (CBAUD) {
    return t->c_cflag & CBAUD;
  } else {
    return t->c_ospeed;
  }
}

/**
 * Sets input baud rate.
 *
 * @return 0 on success, or -1 w/ errno
 * @asyncsignalsafe
 */
int cfsetispeed(struct termios *t, unsigned speed) {
  if (speed) {
    if (CBAUD) {
      if (!(speed & ~CBAUD)) {
        t->c_cflag &= ~CBAUD;
        t->c_cflag |= speed;
      } else {
        return einval();
      }
    } else {
      t->c_ispeed = speed;
    }
  }
  return 0;
}

/**
 * Sets output baud rate.
 *
 * @return 0 on success, or -1 w/ errno
 * @asyncsignalsafe
 */
int cfsetospeed(struct termios *t, unsigned speed) {
  if (CBAUD) {
    if (!(speed & ~CBAUD)) {
      t->c_cflag &= ~CBAUD;
      t->c_cflag |= speed;
      return 0;
    } else {
      return einval();
    }
  } else {
    t->c_ospeed = speed;
    return 0;
  }
}
