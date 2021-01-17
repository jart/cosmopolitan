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
#include "libc/errno.h"
#include "libc/macros.h"

/**
 * Copies data between file descriptors the slow way.
 *
 * @return -1 on error/interrupt, 0 on eof, or [1..size] on success
 * @see copy_file_range() for file ↔ file
 * @see sendfile() for seekable → socket
 * @see splice() for fd ↔ pipe
 */
ssize_t copyfd(int infd, int64_t *inoutopt_inoffset, int outfd,
               int64_t *inoutopt_outoffset, size_t size, uint32_t flags) {
  ssize_t rc;
  char buf[2048];
  size_t i, j, n;
  for (i = 0; i < size; i += j) {
    n = MIN(size - i, sizeof(buf));
    if (inoutopt_inoffset) {
      rc = pread(infd, buf, n, *inoutopt_inoffset);
    } else {
      rc = read(infd, buf, n);
    }
    if (!rc) return i;
    if (rc == -1) {
      if (i) return i;
      return -1;
    }
    n = rc;
    for (j = 0; j < n; j += rc) {
    TryAgain:
      if (inoutopt_outoffset) {
        rc = pwrite(outfd, buf + j, n - j, *inoutopt_outoffset);
      } else {
        rc = write(outfd, buf + j, n - j);
      }
      if (rc == -1) {
        if (errno == EINTR) goto TryAgain;
        if (errno == EWOULDBLOCK) goto TryAgain; /* suboptimal */
        return -1;
      }
      if (inoutopt_inoffset) *inoutopt_inoffset += rc;
      if (inoutopt_outoffset) *inoutopt_outoffset += rc;
    }
  }
  return i;
}
