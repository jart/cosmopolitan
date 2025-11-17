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
#include "libc/errno.h"
#include "libc/intrin/weaken.h"
#include "libc/mem/mem.h"
#include "libc/stdckdint.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/thread.h"

/**
 * Opens fixed length buffer stream.
 *
 * @param buf becomes owned by this function, and is allocated if NULL
 * @return new stream or NULL w/ errno
 * @raise ENOMEM if `buf` is NULL and we failed to allocate it
 * @raise ENOMEM if `buf` is NULL and malloc() wasn't linked
 * @raise EINVAL if `buf` is NULL when `+` isn't in `mode`
 * @see open_memstream()
 */
FILE *fmemopen(void *buf, size_t max_size, const char *mode) {
  FILE *f;
  char *p;
  int oflags;
  size_t size;

  if ((oflags = fopenflags(mode)) == -1)
    return NULL;

  // Because this feature is only useful when the stream is opened for
  // updating (because there is no way to get a pointer to the buffer)
  // the fmemopen() call may fail if the mode argument does not include
  // a '+'. -Quoth POSIX.1-2024
  if (!buf && (oflags & O_ACCMODE) != O_RDWR) {
    errno = EINVAL;
    return NULL;
  }

  if (!(f = __stdio_alloc()))
    return NULL;

  // If a null pointer is specified as the buf argument, fmemopen()
  // shall allocate max_size bytes of memory as if by a call to
  // malloc(). This buffer shall be automatically freed when the stream
  // is closed. Because this feature is only useful when the stream is
  // opened for updating (because there is no way to get a pointer to
  // the buffer) the fmemopen() call may fail if the mode argument does
  // not include a '+'. -Quoth POSIX.1-2024
  size = max_size;
  if (!buf) {
    size_t need;
    if (ckd_add(&need, size, 1)) {
      errno = ENOMEM;
      __stdio_unref(f);
      return NULL;
    }
    if (!(buf = calloc(1, need))) {
      __stdio_unref(f);
      return NULL;
    }
    f->freebuf = 1;
  }
  f->buf = buf;
  if (!(oflags & O_TRUNC))
    f->end = size;
  f->size = size;
  f->oflags = oflags;

  // If buf is a null pointer, the initial position shall always be set
  // to the beginning of the buffer. -Quoth POSIX.1-2024
  //
  // The stream shall maintain a current position in the buffer. This
  // position shall be initially set to either the beginning of the
  // buffer (for r and w modes) or to the first null byte in the buffer
  // (for a modes). If no null byte is found in append mode, the initial
  // position shall be set to one byte after the end of the buffer.
  //                                 -Quoth POSIX.1-2024
  if (!f->freebuf) {
    if (oflags & O_APPEND) {
      if ((p = memchr(buf, '\0', size))) {
        f->end = f->beg = p - (char *)buf;
      } else {
        f->beg = f->end;
      }
    }
  }

  // If the mode argument begins with 'w' and max_size is not zero, the
  // buffer contents shall be truncated by writing a null byte at the
  // beginning. If the mode argument includes 'b', the results are
  // implementation-defined. -Quoth POSIX.1-2024
  if (*mode == 'w' && max_size > 0)
    ((char *)buf)[0] = 0;

  return f;
}
