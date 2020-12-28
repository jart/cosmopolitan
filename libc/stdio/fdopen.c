/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=8 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/mem/mem.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

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
  if ((f = calloc(1, sizeof(FILE)))) {
    f->fd = fd;
    f->reader = __freadbuf;
    f->writer = __fwritebuf;
    f->bufmode = ischardev(fd) ? _IOLBF : _IOFBF;
    f->iomode = fopenflags(mode);
    f->size = BUFSIZ;
    if ((f->buf = valloc(f->size))) {
      if ((f->iomode & O_ACCMODE) != O_RDONLY) {
        _fflushregister(f);
      }
      return f;
    }
    free(f);
  }
  return NULL;
}
