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
#include "libc/errno.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/consts/o.h"

/**
 * Writes data to file.
 *
 * @param size can be -1 to strlen(data)
 * @return if failed, -1 w/ errno
 * @note this is uninterruptible
 */
int xbarf(const char *path, const void *data, size_t size) {
  char *p;
  ssize_t rc;
  int fd, res;
  size_t i, wrote;
  res = 0;
  p = data;
  if (size == -1) size = strlen(p);
  if ((fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644)) != -1) {
    if (ftruncate(fd, size) != -1) {
      if (size > 2 * 1024 * 1024) {
        fadvise(fd, 0, size, MADV_SEQUENTIAL);
      }
      for (i = 0; i < size; i += wrote) {
      TryAgain:
        if ((rc = pwrite(fd, p + i, size - i, i)) != -1) {
          wrote = rc;
        } else if (errno == EINTR) {
          goto TryAgain;
        } else {
          res = -1;
          break;
        }
      }
    } else {
      res = -1;
    }
    close(fd);
  } else {
    res = -1;
  }
  return res;
}
