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
#include "libc/calls/termios.h"
#include "libc/dce.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/errfuns.h"

#define CBAUD   0x100f
#define CBAUDEX 0x1000

/**
 * Returns input baud rate.
 * @asyncsignalsafe
 */
uint32_t cfgetispeed(const struct termios *t) {
  if (IsLinux()) {
    return t->c_cflag & CBAUD;
  } else {
    return t->_c_ispeed;
  }
}

/**
 * Returns output baud rate.
 * @asyncsignalsafe
 */
uint32_t cfgetospeed(const struct termios *t) {
  if (IsLinux()) {
    return t->c_cflag & CBAUD;
  } else {
    return t->_c_ospeed;
  }
}

/**
 * Sets output baud rate.
 *
 * @param speed can be `B0`, `B50`, `B38400`, `B4000000`, etc.
 * @return 0 on success, or -1 w/ errno
 * @raise EINVAL if `speed` isn't valid
 * @asyncsignalsafe
 */
int cfsetospeed(struct termios *t, uint32_t speed) {
  if (IsLinux()) {
    if (!(speed & ~CBAUD)) {
      t->c_cflag &= ~CBAUD;
      t->c_cflag |= speed;
      return 0;
    } else {
      return einval();
    }
  } else {
    t->_c_ospeed = speed;
    return 0;
  }
}

/**
 * Sets input baud rate.
 *
 * @param speed can be `B0`, `B50`, `B38400`, `B4000000`, etc.
 * @return 0 on success, or -1 w/ errno
 * @raise EINVAL if `speed` isn't valid
 * @asyncsignalsafe
 */
int cfsetispeed(struct termios *t, uint32_t speed) {
  if (IsLinux()) {
    if (speed) {
      return cfsetospeed(t, speed);
    } else {
      return 0;
    }
  } else {
    t->_c_ispeed = speed;
    return 0;
  }
}

/**
 * Sets input and output baud rate.
 *
 * @param speed can be `B0`, `B50`, `B38400`, `B4000000`, etc.
 * @return 0 on success, or -1 w/ errno
 * @raise EINVAL if `speed` isn't valid
 * @asyncsignalsafe
 */
int cfsetspeed(struct termios *t, uint32_t speed) {
  if (cfsetispeed(t, speed) == -1) return -1;
  if (cfsetospeed(t, speed) == -1) return -1;
  return 0;
}
