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
#include "libc/calls/termios.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/pty.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/errfuns.h"

/**
 * Opens new pseudo teletypewriter.
 *
 * @params flags is usually O_RDWR|O_NOCTTY
 * @return file descriptor, or -1 w/ errno
 */
int posix_openpt(int flags) {
  int fd, ilduce;
  struct IoctlPtmGet ptm;
  if ((flags & O_ACCMODE) != O_RDWR) return einval();
  if (SupportsFreebsd() &&
      ((fd = posix_openpt$sysv(flags)) != -1 || errno != ENOSYS)) {
    return fd;
  } else if ((fd = open("/dev/ptmx", flags)) != -1 || errno != ENOENT) {
    return fd;
  } else if (SupportsOpenbsd() &&
             ((fd = open("/dev/ptm", flags)) != -1 || errno != ENOENT)) {
    if (ioctl(fd, PTMGET, &ptm) != -1) {
      close(ptm.workerfd);
      ilduce = ptm.theduxfd;
    } else {
      ilduce = -1;
    }
    close(fd);
    return ilduce;
  } else {
    return enosys();
  }
}
