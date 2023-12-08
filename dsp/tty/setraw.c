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
#include "libc/calls/termios.h"
#include "libc/sysv/consts/termios.h"

/**
 * Enables direct teletypewriter communication.
 *
 * @see ttyconfig(), ttyrestore()
 */
int ttysetraw(struct termios *conf, int64_t flags) {
  conf->c_iflag &= ~(INPCK | ISTRIP | PARMRK | INLCR | IGNCR | ICRNL | IXON);
  conf->c_lflag &= ~(IEXTEN | ICANON);
  conf->c_cflag &= ~(CSIZE | PARENB);
  conf->c_cflag |= CS8;
  conf->c_iflag |= IUTF8;
  if (!(flags & kTtySigs)) {
    conf->c_iflag &= ~(IGNBRK | BRKINT);
    conf->c_lflag &= ~(ISIG);
  }
  if (flags & kTtyEcho) {
    conf->c_lflag |= ECHO | ECHONL;
  } else {
    conf->c_lflag &= ~(ECHO | ECHONL);
  }
  return 0;
}
