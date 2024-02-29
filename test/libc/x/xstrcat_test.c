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
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "libc/x/xasprintf.h"

TEST(xstrcat, test) {
  EXPECT_STREQ("hi", gc(xstrcat("hi")));
  EXPECT_STREQ("hithere", gc(xstrcat("hi", "there")));
  EXPECT_STREQ("einszweidrei", gc(xstrcat("eins", "zwei", "drei")));
}

TEST(xstrcat, pointerAbuse) {
  EXPECT_STREQ("hi there", gc(xstrcat("hi", ' ', "there")));
  EXPECT_STREQ("hi there\n", gc(xstrcat("hi", ' ', "there", '\n')));
}

#if defined(__GNUC__) && __GNUC__ >= 12
#pragma GCC diagnostic ignored "-Wuse-after-free"
#endif

int hard_static(void) {
  char *b, *p;
  p = b = malloc(16);
  p = stpcpy(p, "eins");
  p = stpcpy(p, "zwei");
  p = stpcpy(p, "drei");
  free(b);
  return (intptr_t)b;
}

int hard_dynamic(void) {
  char *b, *p;
  p = b = malloc(16);
  p = stpcpy(p, __veil("r", "eins"));
  p = stpcpy(p, __veil("r", "zwei"));
  p = stpcpy(p, __veil("r", "drei"));
  free(b);
  return (intptr_t)b;
}

BENCH(xstrcat, bench) {
  EZBENCH2("hard_static", donothing, __expropriate(hard_static()));
  EZBENCH2("hard_dynamic", donothing, __expropriate(hard_dynamic()));
  EZBENCH2("xstrcat", donothing, free(xstrcat("eins", "zwei", "drei")));
  EZBENCH2("xasprintf", donothing,
           free(xasprintf("%s%s%s", "eins", "zwei", "drei")));
  EZBENCH2("xstrcat2", donothing,
           free(xstrcat("einseinseins", "zweizweizwei", "dreidreidrei")));
  EZBENCH2("xasprintf2", donothing,
           free(xasprintf("%s%s%s", "einseinseins", "zweizweizwei",
                          "dreidreidrei")));
}
