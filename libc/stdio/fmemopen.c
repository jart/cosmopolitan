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
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

/**
 * Opens buffer as stream.
 *
 * This function is the heart of the streams implementation, and it's
 * truly magnificent for unit testing.
 *
 * @param buf becomes owned by this function, and is allocated if NULL
 * @return new stream or NULL w/ errno
 * @error ENOMEM, EINVAL
 */
FILE *fmemopen(void *buf, size_t size, const char *mode) {
  FILE *res;
  unsigned flags;

  if (buf && !size) {
    einval();
    return NULL;
  }

  if (size && popcount(size) != 1) {
    einval();
    return NULL;
  }

  if (!(res = calloc(1, sizeof(FILE)))) {
    return NULL;
  }

  if (!buf) {
    if (!size) size = FRAMESIZE;
    if (!(buf = valloc(size))) {
      free(res);
      return NULL;
    }
  }

  res->fd = -1;
  setbuffer(res, buf, size);
  res->bufmode = res->buf ? _IOFBF : _IONBF;
  flags = fopenflags(mode);
  res->iomode = (flags & O_ACCMODE) == O_RDWR
                    ? O_RDWR
                    : (flags & O_ACCMODE) == O_WRONLY ? O_WRONLY : O_RDONLY;

  return res;
}
