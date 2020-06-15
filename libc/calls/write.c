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
#include "libc/assert.h"
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/dce.h"
#include "libc/sock/internal.h"
#include "libc/sysv/errfuns.h"
#include "libc/zipos/zipos.h"

/**
 * Writes data to file descriptor.
 *
 * @param fd is something open()'d earlier
 * @param buf is copied from, cf. copy_file_range(), sendfile(), etc.
 * @param size in range [1..0x7ffff000] is reasonable
 * @return [1..size] bytes on success, or -1 w/ errno; noting zero is
 *     impossible unless size was passed as zero to do an error check
 * @see read(), pwrite(), writev(), SIGPIPE
 * @asyncsignalsafe
 */
ssize_t write(int fd, const void *buf, size_t size) {
  ssize_t rc;
  size_t wrote;
  if (fd == -1) return einval();
  if (isfdkind(fd, kFdZip)) {
    rc = weaken(__zipos_write)(
        (struct ZiposHandle *)(intptr_t)g_fds.p[fd].handle,
        (struct iovec[]){{buf, size}}, 1, -1);
  } else if (!IsWindows()) {
    rc = write$sysv(fd, buf, size);
  } else if (isfdkind(fd, kFdSocket)) {
    rc = weaken(sendto$nt)(&g_fds.p[fd], (struct iovec[]){{buf, size}}, 1, 0,
                           NULL, 0);
  } else if (isfdkind(fd, kFdFile) || isfdkind(fd, kFdConsole)) {
    rc = write$nt(&g_fds.p[fd], (struct iovec[]){{buf, size}}, 1, -1);
  } else {
    return ebadf();
  }
  if (rc != -1) {
    wrote = (size_t)rc;
    if (wrote == 0) {
      assert(size == 0);
    } else {
      assert(wrote <= size);
    }
  }
  if (!IsTrustworthy() && rc != -1) {
    if (!rc && size) abort();
    if ((size_t)rc > size) abort();
  }
  return rc;
}
