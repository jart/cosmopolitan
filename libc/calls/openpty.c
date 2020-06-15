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
#include "libc/calls/calls.h"
#include "libc/calls/termios.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/pty.h"
#include "libc/sysv/errfuns.h"

/**
 * Opens new pseudo teletypewriter.
 *
 * @param ilduce receives controlling tty rw fd on success
 * @param aworker receives subordinate tty rw fd on success
 * @param termp may be passed to tune a century of legacy behaviors
 * @param winp may be passed to set terminal display dimensions
 * @params flags is usually O_RDWR|O_NOCTTY
 * @return file descriptor, or -1 w/ errno
 */
int openpty(int *ilduce, int *aworker, char *name, const struct termios *termp,
            const struct winsize *winp) {
  return enosys();
  /* TODO(jart) */
  /* int fd, flags; */
  /* flags = O_RDWR | O_NOCTTY; */
  /* if ((fd = posix_openpt(flags)) != -1) { */
  /*   if (ioctl(m, TIOCSPTLCK, &n) || ioctl(m, TIOCGPTN, &n)) { */
  /*   } else { */
  /*     close(fd); */
  /*   } */
  /* } else { */
  /*   return -1; */
  /* } */
}
