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
#include "libc/fmt/itoa.h"
#include "libc/log/log.h"
#include "libc/math.h"
#include "libc/runtime/clktck.h"
#include "libc/stdio/append.internal.h"

static void AppendInt(char **b, int64_t x) {
  char buf[27], *e;
  e = FormatInt64Thousands(buf, x);
  appendd(b, buf, e - buf);
}

static void AppendMetric(char **b, const char *s1, int64_t x, const char *s2,
                         const char *nl) {
  appends(b, s1);
  AppendInt(b, x);
  appends(b, s2);
  appends(b, nl);
}

static void AppendUnit(char **b, int64_t x, const char *s) {
  AppendInt(b, x);
  appendw(b, ' ');
  appends(b, s);
  if (x == 1) {
    appendw(b, 's');
  }
}

/**
 * Generates process resource usage report.
 */
void AppendResourceReport(char **b, struct rusage *ru, const char *nl) {
  char ibuf[27];
  long utime, stime;
  long double ticks;
  if (ru->ru_maxrss) {
    AppendMetric(b, "ballooned to ", ru->ru_maxrss, "kb in size", nl);
  }
  if ((utime = ru->ru_utime.tv_sec * 1000000 + ru->ru_utime.tv_usec) |
      (stime = ru->ru_stime.tv_sec * 1000000 + ru->ru_stime.tv_usec)) {
    appends(b, "needed ");
    AppendInt(b, utime + stime);
    appends(b, "us cpu (");
    AppendInt(b, (long double)stime / (utime + stime) * 100);
    appends(b, "% kernel)");
    appends(b, nl);
    ticks = ceill((long double)(utime + stime) / (1000000.L / CLK_TCK));
    if (ru->ru_idrss) {
      AppendMetric(b, "needed ", lroundl(ru->ru_idrss / ticks),
                   " memory on average", nl);
    }
    if (ru->ru_isrss) {
      AppendMetric(b, "needed ", lroundl(ru->ru_isrss / ticks),
                   " stack on average", nl);
    }
    if (ru->ru_ixrss) {
      AppendMetric(b, "needed ", lroundl(ru->ru_ixrss / ticks),
                   " shared on average", nl);
    }
  }
  if (ru->ru_minflt || ru->ru_majflt) {
    appends(b, "caused ");
    AppendInt(b, ru->ru_minflt + ru->ru_majflt);
    appends(b, " page faults (");
    AppendInt(
        b, (long double)ru->ru_minflt / (ru->ru_minflt + ru->ru_majflt) * 100);
    appends(b, "% memcpy)");
    appends(b, nl);
  }
  if (ru->ru_nvcsw + ru->ru_nivcsw > 1) {
    AppendInt(b, ru->ru_nvcsw + ru->ru_nivcsw);
    appends(b, " context switch (");
    AppendInt(b,
              (long double)ru->ru_nvcsw / (ru->ru_nvcsw + ru->ru_nivcsw) * 100);
    appends(b, "% consensual)");
    appends(b, nl);
  }
  if (ru->ru_msgrcv || ru->ru_msgsnd) {
    appends(b, "received ");
    AppendUnit(b, ru->ru_msgrcv, "message");
    appends(b, " and sent ");
    AppendInt(b, ru->ru_msgsnd);
    appends(b, nl);
  }
  if (ru->ru_inblock || ru->ru_oublock) {
    appends(b, "performed ");
    AppendUnit(b, ru->ru_inblock, "read");
    appends(b, " and ");
    AppendInt(b, ru->ru_oublock);
    appends(b, " write i/o operations");
    appends(b, nl);
  }
  if (ru->ru_nsignals) {
    appends(b, "received ");
    AppendUnit(b, ru->ru_nsignals, "signal");
    appends(b, nl);
  }
  if (ru->ru_nswap) {
    appends(b, "got swapped ");
    AppendUnit(b, ru->ru_nswap, "time");
    appends(b, nl);
  }
}
