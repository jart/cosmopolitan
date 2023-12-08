/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/macros.internal.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/o.h"

/**
 * Repositions open file stream.
 *
 * This function flushes the buffer (unless it's currently in the EOF
 * state) and then calls lseek() on the underlying file. If the stream
 * is in the EOF state, this function can be used to restore it without
 * needing to reopen the file.
 *
 * @param f is a non-null stream handle
 * @param offset is the byte delta
 * @param whence can be SEET_SET, SEEK_CUR, or SEEK_END
 * @returns 0 on success or -1 on error
 */
int fseek_unlocked(FILE *f, int64_t offset, int whence) {
  int res;
  int64_t pos;
  if (f->fd != -1) {
    if (__fflush_impl(f) == -1) return -1;
    if (whence == SEEK_CUR && f->beg < f->end) {
      offset -= f->end - f->beg;
    }
    if (lseek(f->fd, offset, whence) != -1) {
      f->beg = 0;
      f->end = 0;
      f->state = 0;
      res = 0;
    } else {
      f->state = errno == ESPIPE ? EBADF : errno;
      res = -1;
    }
  } else {
    switch (whence) {
      case SEEK_SET:
        pos = offset;
        break;
      case SEEK_CUR:
        pos = f->beg + offset;
        break;
      case SEEK_END:
        pos = f->end + offset;
        break;
      default:
        pos = -1;
        break;
    }
    f->end = MAX(f->beg, f->end);
    if (0 <= pos && pos <= f->end) {
      f->beg = pos;
      f->state = 0;
      res = 0;
    } else {
      f->state = errno = EINVAL;
      res = -1;
    }
  }
  return res;
}
