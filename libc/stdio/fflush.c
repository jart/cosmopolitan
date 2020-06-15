/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=8 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/alg/arraylist.h"
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
 * @see fwritebuf
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
  } else if (f->fd != -1 && (f->iomode & O_WRONLY)) {
    if (!f->state) {
      while (f->beg != f->end) {
        if ((wrote = fwritebuf(f)) != -1) {
          res += wrote;
        } else {
          res = -1;
          break;
        }
      }
    } else if (f->state != -1) {
      res = fseterr(f, f->state);
    }
  }
  return res;
}

int fflushregister(FILE *f) {
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

void fflushunregister(FILE *f) {
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
