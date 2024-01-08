/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/calls.h"
#include "libc/calls/struct/sched_param.h"
#include "libc/dce.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/vendor.internal.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/stack.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/clone.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sched.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"
#include "libc/x/x.h"

#define DUB(i) (union Dub){i}.x

#define DUBBLE(a, b, c, d, e)             \
  {                                       \
    { 0x##e, 0x##d, 0x##c, 0x##b, 0x##a } \
  }

char buf[128];

union Dub {
  uint64_t i;
  double x;
};

void SetUpOnce(void) {
  ASSERT_SYS(0, 0, pledge("stdio", 0));
}

void *Worker(void *p) {
  int i;
  char str[64];
  for (i = 0; i < 256; ++i) {
    bzero(str, sizeof(str));
    snprintf(str, sizeof(str), "%.15g", atan2(-1., -.5));
    ASSERT_STREQ("-2.0344439357957", str);
  }
  return 0;
}

TEST(dtoa, locks) {
  int i, n = 32;
  pthread_t *t = gc(malloc(sizeof(pthread_t) * n));
  for (i = 0; i < n; ++i) {
    ASSERT_EQ(0, pthread_create(t + i, 0, Worker, 0));
  }
  for (i = 0; i < n; ++i) {
    EXPECT_EQ(0, pthread_join(t[i], 0));
  }
}

static const struct {
  uint64_t i;
  const char *f;
  const char *s;
} Vd[] = {
    {0x3ff3ae147ae147ae, "%.8a", "0x1.3ae147aep+0"},  //
    {0x3ff3ae147ae147ae, "%.7a", "0x1.3ae147bp+0"},   //
    {0x3ff3ae147ae147ae, "%.2a", "0x1.3bp+0"},        //
};

TEST(printf, double) {
  int i;
  for (i = 0; i < ARRAYLEN(Vd); ++i) {
    ++g_testlib_ran;
    snprintf(buf, sizeof(buf), Vd[i].f, DUB(Vd[i].i));
    if (strcmp(Vd[i].s, buf)) {
      fprintf(stderr,
              "TEST FAILED\n"
              "EXPECT_STREQ(%`'s, FMT(%`'s, DUB(0x%016lx)))\n"
              "     BUT GOT %`'s\n",
              Vd[i].s, Vd[i].f, Vd[i].i, buf);
      testlib_incrementfailed();
    }
  }
}

#if LDBL_MANT_DIG == 64

static const struct {
  const char *s;
  const char *f;
  union {
    uint16_t i[5];
    long double x;
  } u;
} Vx[] = {
    {"1.23000000000000000002", "%.21Lg", DUBBLE(3fff, 9d70, a3d7, a3d, 70a4)},
    {"123000000000000000000", "%.21Lg", DUBBLE(4041, d55e, f90a, 2da1, 8000)},
    {"1.22999999999999999997e-20", "%.21Lg",
     DUBBLE(3fbc, e857, 267b, b3a9, 84f2)},
    {"1.23456789000000000003", "%.21Lg", DUBBLE(3fff, 9e06, 5214, 1ef0, dbf6)},
    {"123456589000000000000", "%.21Lg", DUBBLE(4041, d629, bd33, 5cc, ba00)},
    {"1.22999999999999999999e+30", "%.21Lg",
     DUBBLE(4062, f865, 8274, 7dbc, 824a)},
    {"1.22999999999999999999e-30", "%.21Lg",
     DUBBLE(3f9b, c794, 337a, 8085, 54eb)},
    {"1.23456788999999999998e-20", "%.21Lg",
     DUBBLE(3fbc, e934, a38, f3d6, d352)},
    {"1.23456788999999999999e-30", "%.21Lg",
     DUBBLE(3f9b, c851, f19d, decc, a8fc)},
    {"1.23456789012345678899", "%.21Lg", DUBBLE(3fff, 9e06, 5214, 62cf, db8d)},
    {"1.22999999999999999997e+306", "%.21Lg",
     DUBBLE(43f7, e033, b668, e30f, a6d5)},
    {"1.23000000000000000002e-306", "%.21Lg",
     DUBBLE(3c06, dd1d, c2ed, 1cb7, 3f25)},
    {"1.23000000000000000002e-320", "%.21Lg",
     DUBBLE(3bd8, 9b98, c371, 844c, 3f1a)},
    {"0xap-3", "%.La", DUBBLE(3fff, 9d70, a3d7, a3d, 70a4)},
    //   cosmo prints 0x9.d70a3d70a3d70a400000p-3
    //   glibc prints 0x9.d70a3d70a3d70a400000p-3
    // openbsd prints 0x9.d70a3d70a3d70a400000p-3
    //   apple prints 0x9.d70a3d70a3d70a400000p-3
    //    musl prints 0x1.3ae147ae147ae1480000p+0
    // freebsd prints 0x1.3ae147ae147ae1480000p+0
    {"0x9.d70a3d70a3d70a400000p-3", "%.20La",
     DUBBLE(3fff, 9d70, a3d7, 0a3d, 70a4)},
    {"0x9.b18ab5df7180b6cp+88", "%La", DUBBLE(405a, 9b18, ab5d, f718, b6c)},
    {"0xa.fc6a015291b4024p+87", "%La", DUBBLE(4059, afc6, a015, 291b, 4024)},
};

TEST(printf, longdouble) {
  if (IsGenuineBlink()) {
    return;  // TODO(jart): long double precision in blink
  }
  int i;
  for (i = 0; i < ARRAYLEN(Vx); ++i) {
    ++g_testlib_ran;
    snprintf(buf, sizeof(buf), Vx[i].f, Vx[i].u.x);
    if (strcmp(Vx[i].s, buf)) {
      fprintf(stderr,
              "TEST FAILED\n"
              "\t{%`'s, %`'s, DUBBLE(%x, %x, %x, %x, %x)}\n"
              "\t→%`'s\n",
              Vx[i].s, Vx[i].f, Vx[i].u.i[4], Vx[i].u.i[3], Vx[i].u.i[2],
              Vx[i].u.i[1], Vx[i].u.i[0], buf);
      testlib_incrementfailed();
    }
  }
}

#endif  // LDBL_MANT_DIG == 64
