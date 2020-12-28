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
#include "libc/calls/struct/metatermios.internal.h"
#include "libc/calls/termios.internal.h"
#include "libc/dce.h"
#include "libc/sysv/consts/termios.h"

int ioctl$tcsets$nt(int, uint64_t, const struct termios *);

static int ioctl$tcsets$sysv(int fd, uint64_t request,
                             const struct termios *tio) {
  union metatermios t;
  return ioctl$sysv(fd, request, termios2host(&t, tio));
}

/**
 * Changes terminal behavior.
 *
 * @see tcsetattr(fd, TCSA{NOW,DRAIN,FLUSH}, tio) dispatches here
 * @see ioctl(fd, TCSETS{,W,F}, tio) dispatches here
 * @see ioctl(fd, TIOCGETA{,W,F}, tio) dispatches here
 */
int ioctl$tcsets(int fd, uint64_t request, const struct termios *tio) {
  if (!IsWindows()) {
    return ioctl$tcsets$sysv(fd, request, tio);
  } else {
    return ioctl$tcsets$nt(fd, request, tio);
  }
}
