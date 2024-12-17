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
#include "libc/calls/struct/stat.h"
#include "libc/mem/mem.h"
#include "libc/stdio/internal.h"
#include "libc/sysv/consts/s.h"

__static_yoink("fflush");

/**
 * Allocates stream object for already-opened file descriptor.
 *
 * @param fd existing file descriptor or -1 for plain old buffer
 * @param mode is passed to fopenflags()
 * @return new stream or NULL w/ errno
 * @error ENOMEM
 */
FILE *fdopen(int fd, const char *mode) {
  FILE *f;
  struct stat st;
  if (fstat(fd, &st))
    return 0;
  if (!(f = __stdio_alloc()))
    return 0;
  f->bufmode = S_ISCHR(st.st_mode) ? _IONBF : _IOFBF;
  f->oflags = fopenflags(mode);
  f->size = BUFSIZ;
  if (!(f->buf = malloc(f->size))) {
    __stdio_unref(f);
    return 0;
  }
  f->freebuf = 1;
  f->fd = fd;
  return f;
}
