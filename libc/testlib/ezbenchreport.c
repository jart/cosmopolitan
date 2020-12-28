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
#include "libc/math.h"
#include "libc/stdio/stdio.h"
#include "libc/testlib/testlib.h"
#include "libc/time/time.h"

STATIC_YOINK("ntoa");
STATIC_YOINK("stoa");
STATIC_YOINK("strnwidth");

void __testlib_ezbenchreport(const char *form, uint64_t c1, uint64_t c2) {
  uint64_t ns1, ns2;
  ns1 = rintl(converttickstonanos(c1));
  ns2 = rintl(converttickstonanos(c2));
  (fprintf)(stderr,
            VEIL("r", "%-30s l: %,10lu𝑐 %,10lu𝑛𝑠   m: %,10lu𝑐 %,10lu𝑛𝑠\n"),
            form, c1, ns1, c2, ns2);
}
