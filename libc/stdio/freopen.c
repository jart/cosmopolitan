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
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/o.h"

/**
 * Overwrites existing stream.
 *
 * This function can be used in two ways. The first is sort of a
 * mutating assignment. The second behavior, if pathname is NULL, is
 * just changing the mode of an already open file descriptor.
 *
 * @param pathname is the file to open or NULL
 * @param mode is the mode string flags, see fopenflags()
 * @param stream is the existing allocated stream memory, which is
 *     flushed and closed if already open
 * @return stream object if successful, or NULL w/ errno
 */
FILE *freopen(const char *pathname, const char *mode, FILE *stream) {
  int fd;
  FILE *res;
  unsigned flags;
  flags = fopenflags(mode);
  flockfile(stream);
  fflush_unlocked(stream);
  if (pathname) {
    /* open new stream, overwriting existing alloc */
    if ((fd = open(pathname, flags, 0666)) != -1) {
      dup3(fd, stream->fd, flags & O_CLOEXEC);
      close(fd);
      stream->iomode = flags;
      stream->beg = 0;
      stream->end = 0;
      res = stream;
    } else {
      res = NULL;
    }
  } else {
    fcntl(stream->fd, F_SETFD, !!(flags & O_CLOEXEC));
    fcntl(stream->fd, F_SETFL, flags & ~O_CLOEXEC);
    res = stream;
  }
  funlockfile(stream);
  return res;
}
