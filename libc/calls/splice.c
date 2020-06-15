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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/sysv/errfuns.h"

static ssize_t splicer(int infd, int64_t *inoffset, int outfd,
                       int64_t *outoffset, size_t uptobytes, uint32_t flags,
                       int64_t impl(int infd, int64_t *inoffset, int outfd,
                                    int64_t *outoffset, size_t uptobytes,
                                    uint32_t flags)) {
  int olderr;
  ssize_t transferred;
  if (!uptobytes || flags == -1) return einval();
  if (IsModeDbg() && uptobytes > 1) uptobytes >>= 1;
  olderr = errno;
  if ((transferred =
           impl(infd, inoffset, outfd, outoffset, uptobytes, flags)) == -1 &&
      errno == ENOSYS) {
    errno = olderr;
    transferred = copyfd(infd, inoffset, outfd, outoffset, uptobytes, flags);
  }
  return transferred;
}

/**
 * Transfers data to/from pipe.
 *
 * @param flags can have SPLICE_F_{MOVE,NONBLOCK,MORE,GIFT}
 * @return number of bytes transferred, 0 on input end, or -1 w/ errno
 * @see copy_file_range() for file ↔ file
 * @see sendfile() for seekable → socket
 */
ssize_t splice(int infd, int64_t *inopt_out_inoffset, int outfd,
               int64_t *inopt_out_outoffset, size_t uptobytes, uint32_t flags) {
  return splicer(infd, inopt_out_inoffset, outfd, inopt_out_outoffset,
                 uptobytes, flags, splice$sysv);
}

/**
 * Transfers data between files.
 *
 * @param outfd should be a writable file, but not O_APPEND
 * @param flags is reserved for future use
 * @return number of bytes actually copied, or -1 w/ errno
 * @see sendfile() for seekable → socket
 * @see splice() for fd ↔ pipe
 */
ssize_t copy_file_range(int infd, int64_t *inopt_out_inoffset, int outfd,
                        int64_t *inopt_out_outoffset, size_t uptobytes,
                        uint32_t flags) {
  return splicer(infd, inopt_out_inoffset, outfd, inopt_out_outoffset,
                 uptobytes, flags, copy_file_range$sysv);
}
