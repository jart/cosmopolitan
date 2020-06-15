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
#include "libc/dce.h"
#include "libc/sock/internal.h"
#include "libc/sysv/errfuns.h"
#include "libc/zipos/zipos.h"

/**
 * Closes file descriptor.
 *
 * @return 0 on success, or -1 w/ errno
 * @asyncsignalsafe
 */
int close(int fd) {
  int rc;
  if (fd == -1) return 0;
  if (isfdkind(fd, kFdZip)) {
    rc = weaken(__zipos_close)(
        (struct ZiposHandle *)(intptr_t)g_fds.p[fd].handle);
  } else if (!IsWindows()) {
    rc = close$sysv(fd);
  } else if (isfdkind(fd, kFdSocket)) {
    rc = weaken(closesocket$nt)(fd);
  } else {
    rc = close$nt(fd);
  }
  removefd(fd);
  return rc;
}
