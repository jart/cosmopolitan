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
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/errno.h"
#include "libc/sysv/errfuns.h"

/**
 * Copies data between file descriptors the slow way.
 *
 * @return -1 on error/interrupt, 0 on eof, or [1..uptobytes] on success
 * @see copy_file_range() for file ↔ file
 * @see sendfile() for seekable → socket
 * @see splice() for fd ↔ pipe
 */
ssize_t copyfd(int infd, int64_t *inopt_out_inoffset, int to_fd,
               int64_t *inopt_out_outoffset, size_t uptobytes, uint32_t flags) {
  size_t i;
  int64_t offset;
  ssize_t got, wrote;
  static unsigned char buf[1024 * 64];
  /* unsigned char buf[1024 * 3]; */
  uptobytes = min(sizeof(buf), uptobytes);
  if (inopt_out_inoffset) {
    got = pread(infd, buf, uptobytes, *inopt_out_inoffset);
  } else {
    got = read(infd, buf, uptobytes);
  }
  if (got == -1) return -1;
  offset = inopt_out_outoffset ? *inopt_out_outoffset : -1;
  for (i = 0; i < got; i += wrote) {
  tryagain:
    if (inopt_out_outoffset) {
      wrote = pwrite(to_fd, buf, got - i, offset + i);
    } else {
      wrote = write(to_fd, buf, got - i);
    }
    if (wrote != -1) continue;
    if (errno == EINTR) {
      if (inopt_out_inoffset != NULL) {
        return -1;
      }
      goto tryagain;
    }
    if (errno == EWOULDBLOCK) {
      assert(inopt_out_inoffset != NULL); /* or caller is nuts */
    }
    return -1;
  }
  if (inopt_out_inoffset) *inopt_out_inoffset += got;
  if (inopt_out_outoffset) *inopt_out_outoffset += got;
  return got;
}
