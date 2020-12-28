/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/alg/arraylist2.internal.h"
#include "libc/assert.h"
#include "libc/log/log.h"
#include "tool/build/lib/breakpoint.h"

void PopBreakpoint(struct Breakpoints *bps) {
  if (bps->i) {
    --bps->i;
  }
}

ssize_t PushBreakpoint(struct Breakpoints *bps, struct Breakpoint *b) {
  int i;
  for (i = 0; i < bps->i; ++i) {
    if (bps->p[i].disable) {
      memcpy(&bps->p[i], b, sizeof(*b));
      return i;
    }
  }
  return APPEND(&bps->p, &bps->i, &bps->n, b);
}

ssize_t IsAtBreakpoint(struct Breakpoints *bps, int64_t addr) {
  size_t i;
  for (i = bps->i; i--;) {
    if (bps->p[i].disable) continue;
    if (bps->p[i].addr == addr) {
      if (bps->p[i].oneshot) {
        bps->p[i].disable = true;
        if (i == bps->i - 1) {
          --bps->i;
        }
      }
      return i;
    }
  }
  return -1;
}
