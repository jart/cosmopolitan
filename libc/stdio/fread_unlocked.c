/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=8 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/struct/iovec.h"
#include "libc/errno.h"
#include "libc/macros.internal.h"
#include "libc/stdckdint.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"

/**
 * Reads data from stream.
 *
 * @param stride specifies the size of individual items
 * @param count is the number of strides to fetch
 * @return count on success, [0,count) on eof, or 0 on error or count==0
 */
size_t fread_unlocked(void *buf, size_t stride, size_t count, FILE *f) {
  char *p;
  ssize_t rc;
  size_t n, m;
  struct iovec iov[2];
  if (!stride) {
    return 0;
  }
  if ((f->iomode & O_ACCMODE) == O_WRONLY) {
    f->state = errno = EBADF;
    return 0;
  }
  if (f->beg > f->end) {
    f->state = errno = EINVAL;
    return 0;
  }
  if (ckd_mul(&n, stride, count)) {
    f->state = errno = EOVERFLOW;
    return 0;
  }
  p = buf;
  m = f->end - f->beg;
  if (MIN(n, m)) {
    memcpy(p, f->buf + f->beg, MIN(n, m));
  }
  if (n < m) {
    f->beg += n;
    return count;
  }
  if (n == m) {
    f->beg = f->end = 0;
    return count;
  }
  if (f->fd == -1) {
    f->beg = 0;
    f->end = 0;
    f->state = -1;
    return m / stride;
  }
  iov[0].iov_base = p + m;
  iov[0].iov_len = n - m;
  if (f->bufmode != _IONBF && n < f->size) {
    iov[1].iov_base = f->buf;
    if (f->size > PUSHBACK) {
      iov[1].iov_len = f->size - PUSHBACK;
    } else {
      iov[1].iov_len = f->size;
    }
  } else {
    iov[1].iov_base = NULL;
    iov[1].iov_len = 0;
  }
  if ((rc = readv(f->fd, iov, 2)) == -1) {
    f->state = errno;
    return 0;
  }
  n = rc;
  f->beg = 0;
  f->end = 0;
  if (n > iov[0].iov_len) {
    f->end += n - iov[0].iov_len;
    return count;
  } else {
    n = (m + n) / stride;
    if (n < count) f->state = -1;
    return n;
  }
}
