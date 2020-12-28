/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
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
