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
#include "libc/dce.h"
#include "libc/rand/rand.h"
#include "libc/str/str.h"
#include "libc/calls/internal.h"
#include "libc/calls/calls.h"
#include "libc/sysv/errfuns.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/nr.h"
#include "libc/sysv/consts/o.h"

/**
 * Reads random bytes from system pseudo random number api.
 * @return 0 on success or -1 w/ errno
 */
int devrand(void *buf, size_t size) {
  int fd;
  size_t got;
  ssize_t rc;
  unsigned char *p, *pe;
  fd = -1;
  if (IsWindows()) return enosys();
  if ((fd = openat$sysv(AT_FDCWD, "/dev/urandom", O_RDONLY, 0)) == -1) {
    return -1;
  }
  p = buf;
  pe = p + size;
  while (p < pe) {
    if ((rc = read$sysv(fd, p, pe - p)) == -1) break;
    if (!(got = (size_t)rc)) break;
    p += got;
  }
  close$sysv(fd);
  return p == pe ? 0 : -1;
}
