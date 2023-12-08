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
#include "libc/intrin/kprintf.h"
#include "libc/math.h"
#include "libc/runtime/runtime.h"

void __testlib_ezbenchreport(const char *form, double c1, double c2) {
  __warn_if_powersave();
  kprintf(" *     %-19s l: %,9luc %,9luns   m: %,9luc %,9luns\n", form,
          lrint(c1), lrint(c1 / 3), lrint(c2), lrint(c2 / 3));
}

void __testlib_ezbenchreport_n(const char *form, char z, size_t n, double c) {
  long cn, lat;
  uint64_t bps;
  char msg[128];
  __warn_if_powersave();
  ksnprintf(msg, sizeof(msg), "%s %c=%d", form, z, n);
  cn = lrint(c / 3);
  if (!n) {
    kprintf("\n");
    kprintf(" *     %-28s", msg);
    if (cn < 1) {
      kprintf(" %'9lu %-12s", (int64_t)(cn * 1024), "picoseconds");
    } else if (cn > 1024) {
      kprintf(" %'9lu %-12s", (int64_t)(cn / 1024), "microseconds");
    } else {
      kprintf(" %'9lu %-12s", (int64_t)cn, "nanoseconds");
    }
  } else {
    kprintf(" *     %-28s", msg);
    bps = n / cn * 1e9;
    lat = cn / n;
    if (lat < 1e-3) {
      kprintf(" %'9lu %-12s", (int64_t)(lat * 1024 * 1024), "fs/byte");
    } else if (lat < 1) {
      kprintf(" %'9lu %-12s", (int64_t)(lat * 1024), "ps/byte");
    } else if (lat > 1024) {
      kprintf(" %'9lu %-12s", (int64_t)(lat / 1024), "µs/byte");
    } else {
      kprintf(" %'9lu %-12s", (int64_t)lat, "ns/byte");
    }
    if (bps < 10 * 1000) {
      kprintf(" %'9lu b/s", bps);
    } else if (bps < 10 * 1000 * 1024) {
      kprintf(" %'9lu kb/s", bps / 1024);
    } else if (bps < 10ul * 1000 * 1024 * 1024) {
      kprintf(" %'9lu mb/s", bps / (1024 * 1024));
    } else if (bps < 10ul * 1000 * 1024 * 1024 * 1024) {
      kprintf(" %'9lu GB/s", bps / (1024 * 1024 * 1024));
    } else {
      kprintf(" %'9lu TB/s", bps / (1024ul * 1024 * 1024 * 1024));
    }
  }
  kprintf("\n", form);
}
