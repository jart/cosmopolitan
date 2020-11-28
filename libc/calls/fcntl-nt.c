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
#include "libc/nt/files.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

textwindows int fcntl$nt(int fd, int cmd, unsigned arg) {
  uint32_t flags;
  if (!__isfdkind(fd, kFdFile)) return ebadf();
  switch (cmd) {
    case F_GETFL:
      return g_fds.p[fd].flags;
    case F_SETFL:
      return (g_fds.p[fd].flags = arg);
    case F_GETFD:
      if (g_fds.p[fd].flags & O_CLOEXEC) {
        return FD_CLOEXEC;
      } else {
        return 0;
      }
    case F_SETFD:
      if (arg & O_CLOEXEC) {
        g_fds.p[fd].flags |= O_CLOEXEC;
        return FD_CLOEXEC;
      } else {
        g_fds.p[fd].flags &= ~O_CLOEXEC;
        return 0;
      }
    default:
      return 0; /* TODO(jart): Implement me. */
  }
}
