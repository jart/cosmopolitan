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
#include "libc/alg/arraylist.internal.h"
#include "libc/bits/bits.h"
#include "libc/bits/pushpop.h"
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/fflush.internal.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/o.h"

/**
 * Blocks until data from stream buffer is written out.
 *
 * @param f is the stream handle
 * @return is 0 on success or -1 on error
 */
int fflush(FILE *f) {
  size_t i;
  if (!f) {
    for (i = __fflush.handles.i; i; --i) {
      if ((f = __fflush.handles.p[i - 1])) {
        if (fflush(f) == -1) return -1;
      }
    }
  } else if (f->fd != -1) {
    if (__fflush_impl(f) == -1) return -1;
  } else if (f->beg && f->beg < f->size) {
    f->buf[f->beg] = 0;
  }
  return 0;
}

textstartup int __fflush_register(FILE *f) {
  size_t i;
  struct StdioFlush *sf;
  sf = &__fflush;
  if (!sf->handles.p) {
    sf->handles.p = &sf->handles_initmem[0];
    pushmov(&sf->handles.n, ARRAYLEN(sf->handles_initmem));
    __cxa_atexit(fflush, NULL, NULL);
  }
  for (i = sf->handles.i; i; --i) {
    if (!sf->handles.p[i - 1]) {
      sf->handles.p[i - 1] = f;
      return 0;
    }
  }
  return append(&sf->handles, &f);
}

void __fflush_unregister(FILE *f) {
  size_t i;
  struct StdioFlush *sf;
  sf = &__fflush;
  sf = pushpop(sf);
  for (i = sf->handles.i; i; --i) {
    if (sf->handles.p[i - 1] == f) {
      pushmov(&sf->handles.p[i - 1], NULL);
      return;
    }
  }
}
