/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/runtime/runtime.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/o.h"

static inline int64_t ftell_unlocked(FILE *f) {
  int64_t pos;
  if (f->fd != -1) {
    if (__fflush_impl(f) == -1) return -1;
    if ((pos = lseek(f->fd, 0, SEEK_CUR)) != -1) {
      if (f->beg < f->end) pos -= f->end - f->beg;
      return pos;
    } else {
      f->state = errno == ESPIPE ? EBADF : errno;
      return -1;
    }
  } else {
    return f->beg;
  }
}

/**
 * Returns current position of stream.
 *
 * @param stream is a non-null stream handle
 * @returns current byte offset from beginning, or -1 w/ errno
 */
int64_t ftell(FILE *f) {
  int64_t rc;
  flockfile(f);
  rc = ftell_unlocked(f);
  funlockfile(f);
  return rc;
}

__strong_reference(ftell, ftello);
