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
#include "libc/calls/ioctl.h"
#include "libc/calls/termios.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/errfuns.h"

/**
 * Sets struct on teletypewriter w/ drains and flushes.
 *
 * @param fd open file descriptor that isatty()
 * @param opt can be:
 *     - TCSANOW:                                   sets console settings
 *     - TCSADRAIN:              drains output, and sets console settings
 *     - TCSAFLUSH: drops input, drains output, and sets console settings
 * @return 0 on success, -1 w/ errno
 * @asyncsignalsafe
 */
int(tcsetattr)(int fd, int opt, const struct termios *tio) {
  switch (opt) {
    case TCSANOW:
      return ioctl(fd, TCSETS, (void *)tio);
    case TCSADRAIN:
      return ioctl(fd, TCSETSW, (void *)tio);
    case TCSAFLUSH:
      return ioctl(fd, TCSETSF, (void *)tio);
    default:
      return einval();
  }
}
