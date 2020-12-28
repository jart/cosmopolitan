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
 * @returns new offset or -1 on error
 */
long fseek(FILE *f, long offset, int whence) {
  int skew;
  int64_t newpos;
  if (f->fd != -1) {
    if (whence == SEEK_CUR && f->beg < f->end) {
      offset -= f->end - f->beg;
    }
    if (f->beg && !f->end) {
      f->writer(f);
    }
    if ((newpos = lseek(f->fd, offset, whence)) != -1) {
      f->state = 0;
      f->beg = 0;
      f->end = 0;
      return newpos;
    } else {
      f->state = errno == ESPIPE ? EBADF : errno;
      return -1;
    }
  } else {
    f->beg = offset % f->size;
    return -1;
  }
}
