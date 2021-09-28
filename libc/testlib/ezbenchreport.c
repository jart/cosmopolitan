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
#include "libc/fmt/fmt.h"
#include "libc/math.h"
#include "libc/runtime/gc.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/testlib/testlib.h"
#include "libc/time/time.h"
#include "libc/x/x.h"

STATIC_YOINK("strnwidth");

void __testlib_ezbenchreport(const char *form, uint64_t c1, uint64_t c2) {
  uint64_t ns1, ns2;
  ns1 = rintl(ConvertTicksToNanos(c1));
  ns2 = rintl(ConvertTicksToNanos(c2));
  (fprintf)(stderr,
            VEIL("r", " *     %-19s l: %,9lu𝑐 %,9lu𝑛𝑠   m: %,9lu𝑐 %,9lu𝑛𝑠\n"),
            form, c1, ns1, c2, ns2);
}

void __testlib_ezbenchreport_n(const char *form, char z, size_t n, uint64_t c) {
  char msg[128];
  uint64_t bps;
  long double cn, lat;
  (snprintf)(msg, sizeof(msg), "%s %c=%d", form, z, n);
  cn = ConvertTicksToNanos(c);
  if (!n) {
    (fprintf)(stderr, "\n");
    (fprintf)(stderr, " *     %-28s", msg);
    if (cn < 1) {
      (fprintf)(stderr, VEIL("r", " %,9lu %-12s"), (int64_t)(cn * 1024),
                "picoseconds");
    } else if (cn > 1024) {
      (fprintf)(stderr, VEIL("r", " %,9lu %-12s"), (int64_t)(cn / 1024),
                "microseconds");
    } else {
      (fprintf)(stderr, VEIL("r", " %,9lu %-12s"), (int64_t)cn, "nanoseconds");
    }
  } else {
    (fprintf)(stderr, " *     %-28s", msg);
    bps = n / cn * 1e9;
    lat = cn / n;
    if (lat < 1e-3) {
      (fprintf)(stderr, VEIL("r", " %,9lu %-12s"), (int64_t)(lat * 1024 * 1024),
                "fs/byte");
    } else if (lat < 1) {
      (fprintf)(stderr, VEIL("r", " %,9lu %-12s"), (int64_t)(lat * 1024),
                "ps/byte");
    } else if (lat > 1024) {
      (fprintf)(stderr, VEIL("r", " %,9lu %-12s"), (int64_t)(lat / 1024),
                "µs/byte");
    } else {
      (fprintf)(stderr, VEIL("r", " %,9lu %-12s"), (int64_t)lat, "ns/byte");
    }
    if (bps < 10 * 1000) {
      (fprintf)(stderr, VEIL("r", " %,9lu b/s"), bps);
    } else if (bps < 10 * 1000 * 1024) {
      (fprintf)(stderr, VEIL("r", " %,9lu kb/s"), bps / 1024);
    } else if (bps < 10ul * 1000 * 1024 * 1024) {
      (fprintf)(stderr, VEIL("r", " %,9lu mb/s"), bps / (1024 * 1024));
    } else if (bps < 10ul * 1000 * 1024 * 1024 * 1024) {
      (fprintf)(stderr, VEIL("r", " %,9lu GB/s"), bps / (1024 * 1024 * 1024));
    } else {
      (fprintf)(stderr, VEIL("r", " %,9lu TB/s"),
                bps / (1024ul * 1024 * 1024 * 1024));
    }
  }
  (fprintf)(stderr, "\n", form);
}
