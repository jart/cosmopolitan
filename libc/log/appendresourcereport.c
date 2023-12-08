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
#include "libc/fmt/itoa.h"
#include "libc/serialize.h"
#include "libc/log/log.h"
#include "libc/math.h"
#include "libc/runtime/clktck.h"
#include "libc/stdio/append.h"

struct State {
  char **b;
  const char *nl;
  char ibuf[27];
};

static void AppendNl(struct State *s) {
  appends(s->b, s->nl);
}

static void AppendInt(struct State *s, int64_t x) {
  char *e = FormatInt64Thousands(s->ibuf, x);
  appendd(s->b, s->ibuf, e - s->ibuf);
}

static void AppendMetric(struct State *s, const char *s1, int64_t x,
                         const char *s2) {
  appends(s->b, s1);
  AppendInt(s, x);
  appends(s->b, s2);
  AppendNl(s);
}

static void AppendUnit(struct State *s, int64_t x, const char *t) {
  AppendInt(s, x);
  appendw(s->b, ' ');
  appends(s->b, t);
  if (x == 1) {
    appendw(s->b, 's');
  }
}

/**
 * Generates process resource usage report.
 */
void AppendResourceReport(char **b, struct rusage *ru, const char *nl) {
  double ticks;
  struct State s;
  long utime, stime;
  struct State *st = &s;
  s.b = b;
  s.nl = nl;
  asm("" : "+r"(st));
  if (ru->ru_maxrss) {
    AppendMetric(st, "ballooned to ", ru->ru_maxrss, "kb in size");
  }
  if ((utime = ru->ru_utime.tv_sec * 1000000 + ru->ru_utime.tv_usec) |
      (stime = ru->ru_stime.tv_sec * 1000000 + ru->ru_stime.tv_usec)) {
    appends(b, "needed ");
    AppendInt(st, utime + stime);
    appends(b, "us cpu (");
    AppendInt(st, (double)stime / (utime + stime) * 100);
    appends(b, "% kernel)");
    AppendNl(st);
    ticks = ceill((double)(utime + stime) / (1000000.L / CLK_TCK));
    if (ru->ru_idrss) {
      AppendMetric(st, "needed ", lroundl(ru->ru_idrss / ticks),
                   "kb private on average");
    }
    if (ru->ru_ixrss) {
      AppendMetric(st, "needed ", lroundl(ru->ru_ixrss / ticks),
                   "kb shared on average");
    }
    if (ru->ru_isrss) {
      AppendMetric(st, "needed ", lroundl(ru->ru_isrss / ticks),
                   "kb stack on average");
    }
  }
  if (ru->ru_minflt || ru->ru_majflt) {
    appends(b, "caused ");
    AppendInt(st, ru->ru_minflt + ru->ru_majflt);
    appends(b, " page faults (");
    AppendInt(st,
              (double)ru->ru_minflt / (ru->ru_minflt + ru->ru_majflt) * 100);
    appends(b, "% memcpy)");
    AppendNl(st);
  }
  if (ru->ru_nvcsw + ru->ru_nivcsw > 1) {
    AppendInt(st, ru->ru_nvcsw + ru->ru_nivcsw);
    appends(b, " context switch");
    if ((ru->ru_nvcsw + ru->ru_nivcsw) > 1) {
      appendw(b, READ16LE("es"));
    }
    appendw(b, READ16LE(" ("));
    AppendInt(st, (double)ru->ru_nvcsw / (ru->ru_nvcsw + ru->ru_nivcsw) * 100);
    appends(b, "% consensual)");
    AppendNl(st);
  }
  if (ru->ru_msgrcv || ru->ru_msgsnd) {
    appends(b, "received ");
    AppendUnit(st, ru->ru_msgrcv, "message");
    appends(b, " and sent ");
    AppendInt(st, ru->ru_msgsnd);
    AppendNl(st);
  }
  if (ru->ru_inblock || ru->ru_oublock) {
    appends(b, "performed ");
    AppendUnit(st, ru->ru_inblock, "read");
    appends(b, " and ");
    AppendInt(st, ru->ru_oublock);
    appends(b, " write i/o operations");
    AppendNl(st);
  }
  if (ru->ru_nsignals) {
    appends(b, "delivered ");
    AppendUnit(st, ru->ru_nsignals, "signal");
    if ((ru->ru_nsignals) > 1) {
      appendw(b, READ16LE("s"));
    }
    AppendNl(st);
  }
  if (ru->ru_nswap) {
    appends(b, "got swapped ");
    AppendUnit(st, ru->ru_nswap, "time");
    AppendNl(st);
  }
}
