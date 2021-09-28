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
#include "libc/runtime/fenv.h"
#include "libc/runtime/gc.internal.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "third_party/gdtoa/gdtoa.h"

int oldround;

void SetUp(void) {
  oldround = fegetround();
}

void TearDown(void) {
  fesetround(oldround);
}

TEST(strtod, testNearest) {
  if (IsWindows()) return;
  fesetround(FE_TONEAREST);
  EXPECT_STREQ("-1.79769313486231e+308",
               gc(xasprintf("%.15g", strtod("-1.79769313486231e+308", NULL))));
}

TEST(strtod, testDownward) {
  if (IsWindows()) return;
  fesetround(FE_DOWNWARD);
  EXPECT_STREQ("-1.79769313486232e+308",
               gc(xasprintf("%.15g", strtod("-1.79769313486231e+308", NULL))));
}

TEST(strtod, testUpward) {
  if (IsWindows()) return;
  fesetround(FE_UPWARD);
  EXPECT_STREQ("-1.7976931348623e+308",
               gc(xasprintf("%.15g", strtod("-1.79769313486231e+308", NULL))));
}

TEST(strtod, testTowardzero) {
  if (IsWindows()) return;
  char *p;
  for (int i = 0; i < 9999; ++i) {
    fesetround(FE_TOWARDZERO);
    EXPECT_STREQ(
        "-1.7976931348623e+308",
        (p = xasprintf("%.15g", strtod("-1.79769313486231e+308", NULL))));
    fesetround(FE_TONEAREST);
    free(p);
  }
}
