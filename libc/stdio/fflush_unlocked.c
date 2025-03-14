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
#include "libc/errno.h"
#include "libc/intrin/weaken.h"
#include "libc/mem/mem.h"
#include "libc/stdio/internal.h"
#include "libc/sysv/consts/o.h"

/**
 * Blocks until data from stream buffer is written out.
 *
 * @param f is the stream handle, which must not be null
 * @return is 0 on success or EOF on error
 */
int fflush_unlocked(FILE *f) {
  size_t i;
  if (f->getln) {
    if (_weaken(free))
      _weaken(free)(f->getln);
    f->getln = 0;
  }
  if (f->fd != -1) {
    if (f->beg && !f->end && (f->oflags & O_ACCMODE) != O_RDONLY) {
      ssize_t rc;
      for (i = 0; i < f->beg; i += rc) {
        if ((rc = write(f->fd, f->buf + i, f->beg - i)) == -1) {
          f->state = errno;
          return EOF;
        }
      }
      f->beg = 0;
    }
    if (f->beg < f->end && (f->oflags & O_ACCMODE) != O_WRONLY) {
      if (lseek(f->fd, -(int)(f->end - f->beg), SEEK_CUR) == -1) {
        f->state = errno;
        return EOF;
      }
      f->end = f->beg;
    }
  }
  if (f->buf && f->beg && f->beg < f->size)
    f->buf[f->beg] = 0;
  return 0;
}
