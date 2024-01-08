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
#include "libc/stdio/rand.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

int rando;

void SetUp(void) {
  rando = rand() & 0xffff;
}

TEST(fmodl, test) {
  EXPECT_STREQ("0", gc(xdtoal(fmodl(0, rando))));
  EXPECT_STREQ("NAN", gc(xdtoal(fmodl(1, NAN))));
  EXPECT_STREQ("NAN", gc(xdtoal(fmodl(NAN, 1))));
  EXPECT_TRUE(isnan(fmodl(INFINITY, 1)));
  EXPECT_TRUE(isnan(fmodl(1, 0)));
  EXPECT_STREQ("8", gc(xdtoal(fmodl(8, 32))));
  EXPECT_STREQ("8e+100", gc(xdtoal(fmodl(8e100, 32e100))));
  EXPECT_STREQ("5.319372648326541e+255",
               gc(xdtoal(ldexpl(6381956970095103, 797))));
  EXPECT_STREQ(".09287409360354737",
               gc(xdtoal(fmodl(ldexpl(6381956970095103, 797), M_2_PI))));
}

TEST(fmod, test) {
  EXPECT_STREQ("0", gc(xdtoa(fmod(0, rando))));
  EXPECT_STREQ("NAN", gc(xdtoa(fmod(1, NAN))));
  EXPECT_STREQ("NAN", gc(xdtoa(fmod(NAN, 1))));
  EXPECT_TRUE(isnan(fmod(INFINITY, 1)));
  EXPECT_TRUE(isnan(fmod(1, 0)));
  EXPECT_STREQ("8", gc(xdtoa(fmod(8, 32))));
  EXPECT_STREQ("8e+100", gc(xdtoa(fmod(8e100, 32e100))));
  EXPECT_STREQ("5.31937264832654e+255",
               gc(xdtoa(ldexp(6381956970095103, 797))));
  EXPECT_STREQ(".0928740936035474",
               gc(xdtoa(fmod(ldexp(6381956970095103, 797), M_2_PI))));
}

TEST(fmodf, test) {
  EXPECT_STREQ("0", gc(xdtoaf(fmodf(0, rando))));
  EXPECT_STREQ("NAN", gc(xdtoaf(fmodf(1, NAN))));
  EXPECT_STREQ("NAN", gc(xdtoaf(fmodf(NAN, 1))));
  EXPECT_TRUE(isnan(fmodf(INFINITY, 1)));
  EXPECT_TRUE(isnan(fmodf(1, 0)));
  EXPECT_STREQ("8", gc(xdtoaf(fmodf(8, 32))));
  EXPECT_STREQ("8e+20", gc(xdtoaf(fmodf(8e20, 32e20))));
}

BENCH(fmodf, bench) {
  EZBENCH2("fmodf eze", donothing, fmodf(8, 32));
  EZBENCH2("fmodf big", donothing, fmodf(5.319372648326541e+255, M_2_PI));
}

BENCH(fmod, bench) {
  // fmod-tiny.S goes slow in the average case, fast for big case
  EZBENCH2("fmod eze", donothing, fmod(8, 32));
  // fmod.c goes fast for the average case very slow for big case
  EZBENCH2("fmod big", donothing, fmod(5.319372648326541e+255, M_2_PI));
}

BENCH(fmodl, bench) {
  EZBENCH2("fmodl eze", donothing, fmodl(8, 32));
  EZBENCH2("fmodl big", donothing, fmodl(5.319372648326541e+255, M_2_PI));
}
