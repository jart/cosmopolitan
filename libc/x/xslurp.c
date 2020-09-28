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
#include "libc/calls/struct/stat.h"
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/consts/o.h"

/**
 * Reads entire file into memory.
 *
 * @return NUL-terminated malloc'd contents, or NULL w/ errno
 * @note this is uninterruptible
 */
char *xslurp(const char *path, size_t *opt_out_size) {
  int fd;
  ssize_t rc;
  size_t i, got;
  char *res, *p;
  struct stat st;
  res = NULL;
  if ((fd = open(path, O_RDONLY)) != -1) {
    if (fstat(fd, &st) != -1 && (res = valloc(st.st_size))) {
      if (st.st_size > 2 * 1024 * 1024) {
        fadvise(fd, 0, st.st_size, MADV_SEQUENTIAL);
      }
      for (i = 0; i < st.st_size; i += got) {
      TryAgain:
        if ((rc = pread(fd, res + i, st.st_size - i, i)) != -1) {
          if (!(got = rc)) {
            if (fstat(fd, &st) == -1) {
              abort();
            }
          }
        } else if (errno == EINTR) {
          goto TryAgain;
        } else {
          free(res);
          res = NULL;
          break;
        }
      }
      if (res) {
        if (opt_out_size) {
          *opt_out_size = st.st_size;
        }
        res[i] = '\0';
      }
    }
    close(fd);
  }
  return res;
}
