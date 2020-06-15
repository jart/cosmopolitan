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
#include "libc/calls/internal.h"
#include "libc/calls/struct/metatermios.h"
#include "libc/calls/termios-internal.h"
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
