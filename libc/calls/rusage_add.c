/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/rusage.h"
#include "libc/macros.internal.h"

/**
 * Accumulates resource statistics in `y` to `x`.
 */
void rusage_add(struct rusage *x, const struct rusage *y) {
  x->ru_utime = timeval_add(x->ru_utime, y->ru_utime);
  x->ru_stime = timeval_add(x->ru_stime, y->ru_stime);
  x->ru_maxrss = MAX(x->ru_maxrss, y->ru_maxrss);
  x->ru_ixrss += y->ru_ixrss;
  x->ru_idrss += y->ru_idrss;
  x->ru_isrss += y->ru_isrss;
  x->ru_minflt += y->ru_minflt;
  x->ru_majflt += y->ru_majflt;
  x->ru_nswap += y->ru_nswap;
  x->ru_inblock += y->ru_inblock;
  x->ru_oublock += y->ru_oublock;
  x->ru_msgsnd += y->ru_msgsnd;
  x->ru_msgrcv += y->ru_msgrcv;
  x->ru_nsignals += y->ru_nsignals;
  x->ru_nvcsw += y->ru_nvcsw;
  x->ru_nivcsw += y->ru_nivcsw;
}
