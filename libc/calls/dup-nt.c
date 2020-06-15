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
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/mem/mem.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

/**
 * Implements dup(), dup2(), and dup3() for Windows NT.
 */
textwindows int dup$nt(int oldfd, int newfd, int flags) {
  if (!isfdkind(oldfd, kFdFile)) return ebadf();
  if (newfd == -1) {
    if ((newfd = createfd()) == -1) return -1;
  } else if (isfdindex(newfd)) {
    close(newfd);
  } else if (isfdlegal(newfd)) {
    do {
      if (growfds() == -1) return -1;
    } while (newfd >= g_fds.n);
  } else {
    return ebadf();
  }
  if (DuplicateHandle(GetCurrentProcess(), g_fds.p[oldfd].handle,
                      GetCurrentProcess(), &g_fds.p[newfd].handle, 0,
                      (flags & O_CLOEXEC), kNtDuplicateSameAccess)) {
    g_fds.p[newfd].kind = g_fds.p[oldfd].kind;
    g_fds.p[newfd].flags = flags;
    return newfd;
  } else {
    return winerr();
  }
}
