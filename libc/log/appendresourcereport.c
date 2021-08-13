/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/log/log.h"
#include "libc/math.h"
#include "libc/runtime/clktck.h"
#include "libc/stdio/append.internal.h"

/**
 * Generates process resource usage report.
 */
void AppendResourceReport(char **b, struct rusage *ru, const char *nl) {
  long utime, stime;
  long double ticks;
  if (ru->ru_maxrss) {
    (appendf)(b, "ballooned to %,ldkb in size%s", ru->ru_maxrss, nl);
  }
  if ((utime = ru->ru_utime.tv_sec * 1000000 + ru->ru_utime.tv_usec) |
      (stime = ru->ru_stime.tv_sec * 1000000 + ru->ru_stime.tv_usec)) {
    ticks = ceill((long double)(utime + stime) / (1000000.L / CLK_TCK));
    (appendf)(b, "needed %,ldµs cpu (%d%% kernel)%s", utime + stime,
              (int)((long double)stime / (utime + stime) * 100), nl);
    if (ru->ru_idrss) {
      (appendf)(b, "needed %,ldkb memory on average%s",
                lroundl(ru->ru_idrss / ticks), nl);
    }
    if (ru->ru_isrss) {
      (appendf)(b, "needed %,ldkb stack on average%s",
                lroundl(ru->ru_isrss / ticks), nl);
    }
    if (ru->ru_ixrss) {
      (appendf)(b, "mapped %,ldkb shared on average%s",
                lroundl(ru->ru_ixrss / ticks), nl);
    }
  }
  if (ru->ru_minflt || ru->ru_majflt) {
    (appendf)(b, "caused %,ld page faults (%d%% memcpy)%s",
              ru->ru_minflt + ru->ru_majflt,
              (int)((long double)ru->ru_minflt /
                    (ru->ru_minflt + ru->ru_majflt) * 100),
              nl);
  }
  if (ru->ru_nvcsw + ru->ru_nivcsw > 1) {
    (appendf)(
        b, "%,ld context switches (%d%% consensual)%s",
        ru->ru_nvcsw + ru->ru_nivcsw,
        (int)((long double)ru->ru_nvcsw / (ru->ru_nvcsw + ru->ru_nivcsw) * 100),
        nl);
  }
  if (ru->ru_msgrcv || ru->ru_msgsnd) {
    (appendf)(b, "received %,ld message%s and sent %,ld%s", ru->ru_msgrcv,
              ru->ru_msgrcv == 1 ? "" : "s", ru->ru_msgsnd, nl);
  }
  if (ru->ru_inblock || ru->ru_oublock) {
    (appendf)(b, "performed %,ld read%s and %,ld write i/o operations%s",
              ru->ru_inblock, ru->ru_inblock == 1 ? "" : "s", ru->ru_oublock,
              nl);
  }
  if (ru->ru_nsignals) {
    (appendf)(b, "received %,ld signals%s", ru->ru_nsignals, nl);
  }
  if (ru->ru_nswap) {
    (appendf)(b, "got swapped %,ld times%s", ru->ru_nswap, nl);
  }
}
