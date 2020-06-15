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
