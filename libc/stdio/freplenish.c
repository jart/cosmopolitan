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
#include "libc/calls/struct/iovec.h"
#include "libc/errno.h"
#include "libc/sock/sock.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"

/* TODO(jart): Delete or rework */

/**
 * Fills empty space in buffer with whatever's available.
 *
 * This can be used alongside functions like select() or poll() to
 * perform reads at opportune moments, thereby minimizing latency.
 */
int freplenish(FILE *f) {
  ssize_t rc;
  size_t got;
  struct iovec iov[2];
  if (f->beg == f->end) {
    f->beg = f->end = 0;
  }
  if (f->beg <= f->end) {
    if (f->beg) {
      iov[0].iov_base = f->buf + f->end;
      iov[0].iov_len = f->size - f->end;
      iov[1].iov_base = f->buf;
      iov[1].iov_len = f->beg - 1;
      rc = readv(f->fd, iov, 2);
    } else {
      rc = read(f->fd, f->buf, f->size - (f->end - f->beg) - 1);
    }
  } else {
    if (f->end + 1 == f->beg) return 0;
    rc = read(f->fd, f->buf + f->end, f->beg - f->end - 1);
  }

  if (rc != -1) {
    if (rc) {
      got = rc;
      f->end = (f->end + got) % f->size;
      return got;
    } else {
      return __fseteof(f);
    }
  } else if (errno == EINTR || errno == EAGAIN) {
    return 0;
  } else {
    return __fseterrno(f);
  }
}
