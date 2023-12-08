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
#include "dsp/tty/tty.h"
#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

char p[16];
struct TtyCursor c;

void SetUp(void) {
  rngset(p, sizeof(p), _rand64, -1);
}

TEST(ttymove, sameCoord_doesNothing) {
  c.y = 0;
  c.x = 0;
  EXPECT_EQ(0, ttymove(&c, p, 0, 0) - p);
  EXPECT_STREQ("", p);
  EXPECT_EQ(0, c.y);
  EXPECT_EQ(0, c.x);
}

TEST(ttymove, crlf) {
  c.y = 0;
  c.x = 10;
  EXPECT_EQ(2, ttymove(&c, p, 1, 0) - p);
  EXPECT_STREQ("\r\n", p);
  EXPECT_EQ(1, c.y);
  EXPECT_EQ(0, c.x);
}

TEST(ttymove, cr) {
  c.y = 7;
  c.x = 10;
  EXPECT_EQ(1, ttymove(&c, p, 7, 0) - p);
  EXPECT_STREQ("\r", p);
  EXPECT_EQ(7, c.y);
  EXPECT_EQ(0, c.x);
}

TEST(ttymove, forwardOne) {
  c.y = 0;
  c.x = 10;
  EXPECT_EQ(3, ttymove(&c, p, 0, 11) - p);
  EXPECT_STREQ("\e[C", p);
  EXPECT_EQ(0, c.y);
  EXPECT_EQ(11, c.x);
}

TEST(ttymove, forwardTwo) {
  c.y = 0;
  c.x = 0;
  EXPECT_EQ(4, ttymove(&c, p, 0, 2) - p);
  EXPECT_STREQ("\e[2C", p);
  EXPECT_EQ(0, c.y);
  EXPECT_EQ(2, c.x);
}

TEST(ttymove, forwardHuge_moves255increments) {
  c.y = 0;
  c.x = 0;
  EXPECT_EQ(6 + 3, ttymove(&c, p, 0, 256) - p);
  EXPECT_STREQ("\e[255C\e[C", p);
  EXPECT_EQ(0, c.y);
  EXPECT_EQ(256, c.x);
}

TEST(ttymove, topleft) {
  c.y = 1000;
  c.x = 888;
  EXPECT_EQ(3, ttymove(&c, p, 0, 0) - p);
  EXPECT_STREQ("\e[H", p);
  EXPECT_EQ(0, c.y);
  EXPECT_EQ(0, c.x);
}

#define MOVE(WANT, Y, X)                             \
  EXPECT_EQ(strlen(WANT), ttymove(&c, p, Y, X) - p); \
  EXPECT_STREQ(WANT, p);                             \
  EXPECT_EQ(Y, c.y);                                 \
  EXPECT_EQ(X, c.x)

TEST(ttymove, absoluteYandX_is1indexed) {
  c.y = 1000;
  c.x = 888;
  MOVE("\e[4;3H", 3, 2);
}

TEST(ttymove, absoluteYandX_implicit1y) {
  c.y = 1000;
  c.x = 888;
  MOVE("\e[;3H", 0, 2);
}

TEST(ttymove, absoluteYandX_implicit1x) {
  c.y = 1000;
  c.x = 888;
  MOVE("\e[4H", 3, 0);
}

TEST(ttymove, up) {
  c.y = 70, c.x = 70;
  MOVE("\eM", 69, 70);
  c.y = 70, c.x = 70;
  MOVE("\e[2A", 68, 70);
}

TEST(ttymove, down) {
  c.y = 70, c.x = 70;
  MOVE("\eD", 71, 70);
  c.y = 70, c.x = 70;
  MOVE("\e[2B", 72, 70);
}

TEST(ttymove, right) {
  c.y = 70, c.x = 70;
  MOVE("\e[C", 70, 71);
  c.y = 70, c.x = 70;
  MOVE("\e[2C", 70, 72);
  c.y = 70, c.x = 0;
  MOVE("\e[123C", 70, 123);
}

TEST(ttymove, left) {
  c.y = 70, c.x = 70;
  MOVE("\e[D", 70, 69);
  c.y = 70, c.x = 70;
  MOVE("\e[2D", 70, 68);
}
