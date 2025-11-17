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
#include "libc/calls/struct/iovec.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/stdckdint.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"

static ssize_t writevall(int fd, struct iovec *iov, int iovlen) {
  int olde;
  ssize_t rc;
  size_t wrote, toto;
  toto = 0;
  olde = errno;
  do {
    if ((rc = writev(fd, iov, iovlen)) == -1) {
      if (toto && errno != ECANCELED) {
        errno = olde;
        return toto;
      }
      return -1;
    }
    wrote = rc;
    toto += wrote;
    for (;;) {
      if (!iov->iov_len) {
        --iovlen;
        ++iov;
      } else if (wrote >= iov->iov_len) {
        wrote -= iov->iov_len;
        --iovlen;
        ++iov;
      } else {
        iov->iov_base += wrote;
        iov->iov_len -= wrote;
        break;
      }
    }
  } while (wrote && iovlen);
  return toto;
}

/**
 * Writes data to stream.
 *
 * @param stride specifies the size of individual items
 * @param count is the number of strides to write
 * @return count on success, [0,count) on EOF or error or count==0
 * @cancelationpoint
 */
size_t fwrite_unlocked(const void *data, size_t stride, size_t count, FILE *f) {
  ldiv_t d;
  ssize_t rc;
  size_t n, m;
  const char *p;
  struct iovec iov[2];

  // check state
  if ((f->oflags & O_ACCMODE) == O_RDONLY) {
    f->state = errno = EBADF;
    return 0;
  }

  // flush buffered input (repositions file descriptor)
  if (f->beg < f->end && f->fd != -1) {
    if (IsModeDbg())
      // the application shall ensure that output is not directly
      // followed by input without an intervening call to fflush() or to
      // a file positioning function (fseek(), fsetpos(), or rewind()),
      // and input is not directly followed by output without an
      // intervening call to a file positioning function, unless the
      // input operation encounters end-of-file. -Quoth POSIX.1-2024
      unassert(!"posix requires fflush between stdio reads and writes");
    if (fflush_unlocked(f))
      return 0;
  }

  // compute size
  if (ckd_mul(&n, stride, count)) {
  OnOverflow:
    f->state = errno = EOVERFLOW;
    return 0;
  }

  // simulate append mode for fmemopen() streams
  if ((f->oflags & O_APPEND) && f->fd == -1)
    f->beg = f->end;

  // exit early if there's no work
  if (!n)
    return 0;

  // extend open_memstream() buffer
  if (f->memstream_bufp) {
    size_t need;
    if (ckd_add(&need, f->beg, n))
      goto OnOverflow;
    if (ckd_add(&need, need, 1))
      goto OnOverflow;
    if (need > f->size) {
      char *buf2;
      size_t size2;
      size2 = f->size;
      do {
        if (ckd_add(&size2, size2, size2 >> 1))
          goto OnOverflow;
      } while (need > size2);
      if (!(buf2 = realloc(f->buf, size2))) {
        f->state = errno = ENOMEM;
        return 0;
      }
      memset(buf2 + f->size, 0, size2 - f->size);
      *f->memstream_bufp = f->buf = buf2;
      f->size = size2;
    }
  }

  // handle i/o buffering
  m = f->size - f->beg;
  if (n <= m && f->bufmode != _IONBF) {
    // this isn't an unbuffered stream, and
    // there's enough room in the buffer for the request
    memcpy(f->buf + f->beg, data, n);
    f->beg += n;
    // add nul terminator if fmemopen() stream becomes longer
    if (f->beg > f->end && f->fd == -1) {
      f->end = f->beg;
      if (!f->freebuf)
        if (f->beg < f->size)
          f->buf[f->beg] = 0;
    }
    if (f->fd != -1 && f->bufmode == _IOLBF &&
        (p = memrchr(f->buf, '\n', f->beg))) {
      // write out as many lines as possible
      n = p + 1 - f->buf;
      if ((rc = write(f->fd, f->buf, n)) == -1) {
        f->beg -= n;
        f->state = errno;
        return 0;
      }
      // copy backwards last line fragment or uncompleted data
      n = rc;
      if (f->beg - n)
        memmove(f->buf, f->buf + n, f->beg - n);
      f->beg -= n;
    }
    return count;
  }

  // what's happening is either
  // (1) a fully buffered stream, or
  // (2) no room in buffer to hold full request
  if (f->fd == -1) {
    // this is an in-memory stream
    // store as much of request as we can hold
    n = MIN(n, m);
    d = ldiv(n, stride);
    n -= d.rem;
    if (n) {
      memcpy(f->buf + f->beg, data, n);
      f->beg += n;
      // add nul terminator if fmemopen() stream becomes longer
      if (f->beg > f->end && f->fd == -1) {
        f->end = f->beg;
        if (!f->freebuf)
          if (f->beg < f->size)
            f->buf[f->beg] = 0;
      }
    }
    errno = ENOSPC;
    return d.quot;
  }

  // perform a fragmented write
  // (1) we avoid needless copies in fully buffered mode
  // (2) we avoid need for malloc() when it's out of room
  iov[0].iov_base = f->buf;
  iov[0].iov_len = f->beg;
  iov[1].iov_base = (void *)data;
  iov[1].iov_len = n;
  n += f->beg;
  if ((rc = writevall(f->fd, iov, 2)) == -1) {
    f->state = errno;
    return 0;
  }
  f->beg = 0;
  return count;
}
