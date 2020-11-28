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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/dce.h"
#include "libc/macros.h"
#include "libc/sysv/errfuns.h"

/**
 * Writes to file at offset, thus avoiding superfluous lseek().
 *
 * @param fd is something open()'d earlier, noting pipes might not work
 * @param buf is copied from, cf. copy_file_range(), sendfile(), etc.
 * @param size in range [1..0x7ffff000] is reasonable
 * @param offset is bytes from start of file at which write begins
 * @return [1..size] bytes on success, or -1 w/ errno; noting zero is
 *     impossible unless size was passed as zero to do an error check
 * @see pread(), write()
 */
ssize_t pwrite(int fd, const void *buf, size_t size, int64_t offset) {
  ssize_t rc;
  size_t wrote;
  if (fd == -1 || offset < 0) return einval();
  size = MIN(size, 0x7ffff000);
  if (!IsWindows()) {
    rc = pwrite$sysv(fd, buf, size, offset);
  } else if (__isfdkind(fd, kFdFile)) {
    rc = write$nt(&g_fds.p[fd], (struct iovec[]){{buf, size}}, 1, offset);
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
  return rc;
}
