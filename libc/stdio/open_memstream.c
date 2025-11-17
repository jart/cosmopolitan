/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2025 Justine Alexandra Roberts Tunney                              │
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
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/o.h"

/**
 * Opens dynamic buffer memory stream.
 *
 * This function is similar to fmemopen() except that the buffer is
 * always allocated and will grow as more data is appended.
 *
 * If this function succeeds, then you need to call free() on the
 * pointer returned to `bufp` after you call fclose().
 *
 * You need to call fflush() or fclose() to get the most recent values
 * written back to your `bufp` and `sizep` pointers. The size will
 * contain the smaller of the current buffer length and the number of
 * bytes for open_memstream() between the beginning of the buffer and
 * the current file position indicator.
 *
 * A nul terminator at `(*bufp)[*sizep]` is guaranteed upon fclose().
 *
 * Calling fseek() on the returned stream with a position that resolves
 * past the end of buffer will extend the buffer with zero bytes.
 *
 * In terms of portability, POSIX says the stream shall be opened in
 * write mode. Musl Libc interprets that as write *only*. However both
 * Cosmo and Glibc open your stream in read/write mode.
 *
 * @return new stream or null w/ errno
 * @raise EINVAL if `bufp` or `sizep` are null
 * @raise ENOMEM if memory couldn't be allocated
 */
FILE *open_memstream(char **bufp, size_t *sizep) {
  FILE *f;

  // check args
  if (!bufp || !sizep) {
    errno = EINVAL;
    return 0;
  }

  // allocate object
  if (!(f = __stdio_alloc()))
    return 0;
  f->size = 16;
  if (!(f->buf = calloc(1, f->size))) {
    __stdio_unref(f);
    return 0;
  }
  f->freebuf = 1;  // fclose() will ignore this in our case
  f->oflags = O_RDWR;
  f->memstream_bufp = bufp;
  f->memstream_sizep = sizep;

  // return result
  *bufp = f->buf;
  *sizep = f->end;
  return f;
}
