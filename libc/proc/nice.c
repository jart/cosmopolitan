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
#include "libc/fmt/conv.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/sysv/consts/prio.h"

static int clamp(int p) {
  return MAX(-NZERO, MIN(NZERO - 1, p));
}

/**
 * Changes process priority.
 *
 * @param delta is added to current priority w/ clamping
 * @return new priority, or -1 w/ errno
 * @see Linux claims ioprio_set() is tuned automatically by this
 */
int nice(int delta) {
  int p;
  if (ABS(delta) >= NZERO * 2) {
    p = delta;
  } else {
    delta = clamp(delta);
    if ((p = getpriority(PRIO_PROCESS, 0)) == -1) return -1;
    p += delta;
  }
  p = clamp(p);
  if (setpriority(PRIO_PROCESS, 0, p) == -1) return -1;
  return p;
}
