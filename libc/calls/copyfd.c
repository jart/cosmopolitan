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
#include "libc/assert.h"
#include "libc/bits/bits.h"
#include "libc/bits/safemacros.h"
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
