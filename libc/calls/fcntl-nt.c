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
#include "libc/sysv/errfuns.h"

textwindows int fcntl$nt(int fd, int cmd, unsigned arg) {
  uint32_t flags;
  if (!isfdkind(fd, kFdFile)) return ebadf();
  switch (cmd) {
    case F_GETFD:
      if (!GetHandleInformation(g_fds.p[fd].handle, &flags)) return -1;
      arg = (flags & FD_CLOEXEC) ^ FD_CLOEXEC;
      return arg;
    case F_SETFD:
      arg ^= FD_CLOEXEC;
      if (!SetHandleInformation(g_fds.p[fd].handle, FD_CLOEXEC, arg)) return -1;
      return 0;
    default:
      return 0; /* TODO(jart): Implement me. */
  }
}
