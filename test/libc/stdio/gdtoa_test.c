/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2025 Justine Alexandra Roberts Tunney                              │
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
#include "third_party/gdtoa/gdtoa.h"
#include "libc/assert.h"
#include "libc/math.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/benchmark.h"
#include "libc/testlib/testlib.h"
#include "third_party/gdtoa/gdtoa.h"

void RunDtoa(int mode, double x) {
  char *rve;
  int decpt, sign;
  char *str = dtoa(x, mode, 10, &decpt, &sign, &rve);
  unassert(str);
  freedtoa(str);
}

void DtoaTough(void) {
  RunDtoa(0, 0.1);
}

void DtoaSoftball(void) {
  RunDtoa(0, 2);
}

void DtoaTorture(void) {
  RunDtoa(0, 2.2250738585072011e-308);
}

void DtoaAgony(void) {
  RunDtoa(3, DBL_MAX);
}

TEST(dtoa, bench) {
  BENCHMARK(1000, 1, DtoaSoftball());
  BENCHMARK(1000, 1, DtoaTough());
  BENCHMARK(1000, 1, DtoaTorture());
  BENCHMARK(1000, 1, DtoaAgony());
}

////////////////////////////////////////////////////////////////////////////////

static const FPI kFpiDbl = {
    .nbits = DBL_MANT_DIG,
    .emin = 3 - DBL_MAX_EXP - DBL_MANT_DIG,
    .emax = DBL_MAX_EXP - DBL_MANT_DIG,
    .rounding = FPI_Round_near,
    .sudden_underflow = 0,
};

void RunGdtoa(int mode, double x) {
  union {
    double x;
    unsigned i[2];
  } u = {x};
  char *rve;
  int decpt, kind = STRTOG_Normal;
  char *str = gdtoa(&kFpiDbl, 16, u.i, &kind, mode, 10, &decpt, &rve);
  unassert(str);
  freedtoa(str);
}

void GdtoaTough(void) {
  RunGdtoa(0, 0.1);
}

void GdtoaSoftball(void) {
  RunGdtoa(0, 2);
}

void GdtoaTorture(void) {
  RunGdtoa(0, 2.2250738585072011e-308);
}

void GdtoaAgony(void) {
  RunGdtoa(3, DBL_MAX);
}

TEST(gdtoa, bench) {
  BENCHMARK(1000, 1, GdtoaSoftball());
  BENCHMARK(1000, 1, GdtoaTough());
  BENCHMARK(1000, 1, GdtoaTorture());
  BENCHMARK(1000, 1, GdtoaAgony());
}
