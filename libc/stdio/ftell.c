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
#include "libc/runtime/runtime.h"
#include "libc/stdio/internal.h"

static long ftell_unlocked(FILE *f) {
  long pos;

  // handle memory streams
  if (f->fd == -1)
    return f->beg;

  // get position from operating system
  if ((pos = lseek(f->fd, 0, SEEK_CUR)) == -1)
    return -1;

  // adjust for unread/unwritten content
  pos += (long)f->beg - (long)f->end;

  return pos;
}

/**
 * Returns current position of stream.
 *
 * @param stream is a non-null stream handle
 * @returns current byte offset from beginning, or -1 w/ errno
 * @raise EBADF if underlying file descriptor isn't open
 * @raise ESPIPE if underlying file descriptor is a pipe
 */
long ftell(FILE *f) {
  long rc;
  if (__isthreaded >= 2)
    flockfile(f);
  rc = ftell_unlocked(f);
  if (__isthreaded >= 2)
    funlockfile(f);
  return rc;
}

__strong_reference(ftell, ftello);
