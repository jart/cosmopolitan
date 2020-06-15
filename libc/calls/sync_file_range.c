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
#include "libc/errno.h"

/**
 * Flushes subset of file to disk.
 *
 * @param offset is page rounded
 * @param bytes is page rounded; 0 means until EOF
 * @param flags can have SYNC_FILE_RANGE_{WAIT_BEFORE,WRITE,WAIT_AFTER}
 * @note Linux documentation says this call is "dangerous"; for highest
 *     assurance of data recovery after crash, consider fsync() on both
 *     file and directory
 * @see fsync(), fdatasync(), PAGESIZE
 */
int sync_file_range(int fd, int64_t offset, int64_t bytes, unsigned flags) {
  int rc, olderr;
  olderr = errno;
  if ((rc = sync_file_range$sysv(fd, offset, bytes, flags)) != -1 ||
      errno != ENOSYS) {
    return rc;
  } else {
    errno = olderr;
    return fdatasync(fd);
  }
}
