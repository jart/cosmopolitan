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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/iovec.h"
#include "libc/errno.h"
#include "libc/macros.h"
#include "libc/stdckdint.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

static ssize_t readvall(FILE *f, struct iovec *iov, int iovlen, size_t need) {
  ssize_t rc;
  size_t got, toto;
  for (toto = 0;;) {

    // perform i/o
    if ((rc = readv(f->fd, iov, iovlen)) == -1) {
      f->state = errno;
      if (toto)
        return toto;
      return -1;
    }
    got = rc;
    toto += got;
    if (!got) {
      f->state = EOF;
      return toto;
    }

    // roll forward iov
    // skip over empty elements
    for (;;) {
      if (!iov->iov_len) {
        --iovlen;
        ++iov;
      } else if (got >= iov->iov_len) {
        got -= iov->iov_len;
        --iovlen;
        ++iov;
      } else {
        iov->iov_base += got;
        iov->iov_len -= got;
        break;
      }
      if (!iovlen)
        return toto;
    }

    // don't trigger eof condition if we're rolling greed to fill buffer
    if (toto >= need)
      return toto;
  }
}

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
  struct iovec iov[2];
  size_t n, m, got, need;

  // check state and parameters
  if ((f->oflags & O_ACCMODE) == O_WRONLY) {
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
  if (!n)
    return 0;

  // try to fulfill request from buffer if possible
  p = buf;
  m = f->end - f->beg;
  if (n <= m) {
    memcpy(p, f->buf + f->beg, n);
    if ((f->beg += n) == f->end) {
      f->beg = 0;
      f->end = 0;
    }
    return count;
  }

  // handle end-of-file condition in fileless mode
  if (f->fd == -1) {
    m /= stride;
    m *= stride;
    if (m)
      memcpy(p, f->buf + f->beg, m);
    if ((f->beg += m) == f->end) {
      f->state = EOF;
      f->beg = 0;
      f->end = 0;
    }
    return m / stride;
  }

  // `n` is number of bytes requested by caller
  // `m` is how much of `n` came from existing buffer
  // `iov[0]` reads remainder of the caller request
  // `iov[1]` reads ahead extra content into buffer
  if (m)
    memcpy(p, f->buf + f->beg, m);
  iov[0].iov_base = p + m;
  iov[0].iov_len = need = n - m;
  if (f->bufmode != _IONBF && n < f->size) {
    iov[1].iov_base = f->buf;
    if (f->size > PUSHBACK)
      iov[1].iov_len = f->size - PUSHBACK;
    else
      iov[1].iov_len = f->size;
  } else {
    iov[1].iov_base = NULL;
    iov[1].iov_len = 0;
  }
  rc = readvall(f, iov, 2, need);
  if (rc == -1)
    return 0;
  got = rc;

  // handle partial fulfillment
  if (got < need) {
    got += m;
    f->beg = 0;
    f->end = 0;
    return got / stride;
  }

  // handle overfulfillment
  f->beg = 0;
  f->end = got - need;
  return count;
}
