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
#include "libc/intrin/weaken.h"
#include "libc/mem/mem.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/thread.h"

/**
 * Opens buffer as stream.
 *
 * @param buf becomes owned by this function, and is allocated if NULL
 * @return new stream or NULL w/ errno
 * @raise ENOMEM if `buf` is NULL and we failed to allocate it
 * @raise ENOMEM if `buf` is NULL and malloc() wasn't linked
 * @raise EINVAL if `buf` is NULL when `+` isn't in `mode`
 */
FILE *fmemopen(void *buf, size_t size, const char *mode) {
  FILE *f;
  char *p;
  int iomode;
  iomode = fopenflags(mode);
  if ((size && size > 0x7ffff000) ||  //
      (!buf && (iomode & O_ACCMODE) != O_RDWR)) {
    einval();
    return NULL;
  }
  if (!(f = __stdio_alloc())) {
    return NULL;
  }
  if (buf) {
    f->nofree = true;
  } else {
    if (!size) size = BUFSIZ;
    // TODO(jart): Why do we need calloc()?
    if (!_weaken(calloc) || !(buf = _weaken(calloc)(1, size))) {
      __stdio_free(f);
      enomem();
      return NULL;
    }
  }
  f->fd = -1;
  f->buf = buf;
  if (!(iomode & O_TRUNC)) {
    f->end = size;
  }
  f->size = size;
  f->iomode = iomode;
  if (iomode & O_APPEND) {
    if ((p = memchr(buf, '\0', size))) {
      f->beg = p - (char *)buf;
    } else {
      f->beg = f->end;
    }
  }
  return f;
}
