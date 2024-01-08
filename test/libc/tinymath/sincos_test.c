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
#include "libc/math.h"
#include "libc/mem/gc.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/xasprintf.h"

TEST(sincos, test) {
  double sine, cosine;
  sincos(.1, &sine, &cosine);
  EXPECT_STREQ("0.0998334166468282", gc(xasprintf("%.15g", sine)));
  EXPECT_STREQ("0.995004165278026", gc(xasprintf("%.15g", cosine)));
}

TEST(sincosf, test) {
  float sine, cosine;
  sincosf(.1, &sine, &cosine);
  EXPECT_STREQ("0.0998334", gc(xasprintf("%.6g", sine)));
  EXPECT_STREQ("0.995004", gc(xasprintf("%.6g", cosine)));
}

TEST(sincosl, test) {
  long double sine, cosine;
  sincosl(.1, &sine, &cosine);
#ifndef __aarch64__
  // TODO(jart): get quad floats working with printf
  EXPECT_STREQ("0.0998334166468282", gc(xasprintf("%.15Lg", sine)));
  EXPECT_STREQ("0.995004165278026", gc(xasprintf("%.15Lg", cosine)));
#endif
}

#define NUM .123

BENCH(sincos, bench) {
  double _sin(double) asm("sin");
  float _sinf(float) asm("sinf");
  long double _sinl(long double) asm("sinl");
  double _cos(double) asm("cos");
  float _cosf(float) asm("cosf");
  long double _cosl(long double) asm("cosl");
  double _sincos(double, double*, double*) asm("sincos");
  float _sincosf(float, float*, float*) asm("sincosf");
  long double _sincosl(long double, long double*, long double*) asm("sincosl");
  float sinef, cosinef;
  double sine, cosine;
  long double sinel, cosinel;
  EZBENCH2("sin+cos", donothing, (_sin(NUM), _cos(NUM)));
  EZBENCH2("sincos", donothing, _sincos(NUM, &sine, &cosine));
  EZBENCH2("sinf+cosf", donothing, (_sinf(NUM), _cosf(NUM)));
  EZBENCH2("sincosf", donothing, _sincosf(NUM, &sinef, &cosinef));
  EZBENCH2("sinl+cosl", donothing, (_sinl(NUM), _cosl(NUM)));
  EZBENCH2("sincosl", donothing, _sincosl(NUM, &sinel, &cosinel));
}
