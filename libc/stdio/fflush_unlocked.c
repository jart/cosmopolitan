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
#include "libc/cxxabi.h"
#include "libc/intrin/pushpop.h"
#include "libc/mem/arraylist.internal.h"
#include "libc/stdio/fflush.internal.h"
#include "libc/stdio/internal.h"

/**
 * Blocks until data from stream buffer is written out.
 *
 * @param f is the stream handle, or 0 for all streams
 * @return is 0 on success or -1 on error
 */
int fflush_unlocked(FILE *f) {
  int rc = 0;
  size_t i;
  if (!f) {
    __fflush_lock();
    for (i = __fflush.handles.i; i; --i) {
      if ((f = __fflush.handles.p[i - 1])) {
        if (fflush(f) == -1) {
          rc = -1;
        }
      }
    }
    __fflush_unlock();
  } else if (f->fd != -1) {
    if (__fflush_impl(f) == -1) {
      rc = -1;
    }
  } else if (f->beg && f->beg < f->size) {
    f->buf[f->beg] = 0;
  }
  return rc;
}

textstartup int __fflush_register(FILE *f) {
  int rc;
  size_t i;
  struct StdioFlush *sf;
  __fflush_lock();
  sf = &__fflush;
  if (!sf->handles.p) {
    sf->handles.p = sf->handles_initmem;
    pushmov(&sf->handles.n, ARRAYLEN(sf->handles_initmem));
    __cxa_atexit((void *)fflush_unlocked, 0, 0);
  }
  for (i = sf->handles.i; i; --i) {
    if (!sf->handles.p[i - 1]) {
      sf->handles.p[i - 1] = f;
      __fflush_unlock();
      return 0;
    }
  }
  rc = append(&sf->handles, &f);
  __fflush_unlock();
  return rc;
}

void __fflush_unregister(FILE *f) {
  size_t i;
  struct StdioFlush *sf;
  __fflush_lock();
  sf = &__fflush;
  sf = pushpop(sf);
  for (i = sf->handles.i; i; --i) {
    if (sf->handles.p[i - 1] == f) {
      pushmov(&sf->handles.p[i - 1], 0);
      break;
    }
  }
  __fflush_unlock();
}
