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
#include "libc/calls/internal.h"
#include "libc/calls/struct/termios.h"
#include "libc/calls/struct/winsize.h"
#include "libc/nt/console.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

textwindows int ioctl$tiocswinsz$nt(int fd, const struct winsize *ws) {
  uint32_t mode;
  struct NtCoord coord;
  if (!ws) return efault();
  if (!isfdkind(fd, kFdFile)) return ebadf();
  if (!GetConsoleMode(g_fds.p[fd].handle, &mode)) return enotty();
  coord.X = ws->ws_col;
  coord.Y = ws->ws_row;
  if (!SetConsoleScreenBufferSize(g_fds.p[fd].handle, coord)) return winerr();
  return 0;
}
