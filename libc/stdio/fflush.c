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
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/o.h"

struct StdioFlushHandles {
  size_t i, n;
  FILE **p;
};

struct StdioFlush {
  struct StdioFlushHandles handles;
  FILE *handles_initmem[8];
};

static struct StdioFlush g_fflush;

/**
 * Blocks until data from stream buffer is written out.
 *
 * @param f is the stream handle
 * @return number of bytes written or -1 on error
 */
int fflush(FILE *f) {
  size_t i;
  int res, wrote;
  res = 0;
  if (!f) {
    for (i = g_fflush.handles.i; i; --i) {
      if ((f = g_fflush.handles.p[i - 1])) {
        if ((wrote = fflush(f)) != -1) {
          res += wrote;
        } else {
          res = -1;
          break;
        }
      }
    }
  } else if (f->fd != -1) {
    while (!f->state && f->beg && !f->end) {
      if ((wrote = __fwritebuf(f)) != -1) {
        res += wrote;
      }
    }
  } else if (f->beg && f->beg < f->size) {
    f->buf[f->beg] = 0;
  }
  return res;
}

textstartup int _fflushregister(FILE *f) {
  size_t i;
  struct StdioFlush *sf;
  sf = &g_fflush;
  sf = sf;
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

void _fflushunregister(FILE *f) {
  size_t i;
  struct StdioFlush *sf;
  sf = &g_fflush;
  sf = pushpop(sf);
  for (i = sf->handles.i; i; --i) {
    if (sf->handles.p[i - 1] == f) {
      pushmov(&sf->handles.p[i - 1], NULL);
      return;
    }
  }
}
