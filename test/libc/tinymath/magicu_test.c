/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/tinymath/magicu.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/tinymath/magicu.h"

#define T    uint32_t
#define TBIT (sizeof(T) * CHAR_BIT - 1)
#define TMIN (((T) ~(T)0) > 1 ? (T)0 : (T)((uintmax_t)1 << TBIT))
#define TMAX (((T) ~(T)0) > 1 ? (T) ~(T)0 : (T)(((uintmax_t)1 << TBIT) - 1))
T V[] = {5,           4,        77,       4,         7,        0,
         1,           2,        3,        4,         -1,       -2,
         -3,          -4,       TMIN,     TMIN + 1,  TMIN + 2, TMIN + 3,
         TMIN + 5,    TMIN + 7, TMAX,     TMAX - 1,  TMAX - 2, TMAX - 77,
         TMAX - 3,    TMAX - 5, TMAX - 7, TMAX - 50, TMIN / 2, TMAX / 2,
         TMAX / 2 - 3};

TEST(magicu, test) {
  int i, j;
  for (i = 0; i < ARRAYLEN(V); ++i) {
    if (!V[i]) continue;
    struct magicu d = __magicu_get(V[i]);
    for (j = 0; j < ARRAYLEN(V); ++j) {
      EXPECT_EQ(V[j] / V[i], __magicu_div(V[j], d));
    }
  }
}

BENCH(magicu, bench) {
  struct magicu d = __magicu_get(UINT32_MAX);
  EZBENCH2("__magicu_get", donothing, __magicu_get(__veil("r", UINT32_MAX)));
  EZBENCH2("__magicu_div", donothing,
           __expropriate(__magicu_div(__veil("r", 77u), d)));
  EZBENCH2("/", donothing,
           __expropriate(__veil("r", 77u) / __veil("r", UINT32_MAX)));
}
