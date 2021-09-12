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
#include "libc/dce.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

TEST(iswupper, test) {
  EXPECT_TRUE(iswupper(L'𝐵'));
}

TEST(towupper, test) {
  EXPECT_EQ(u'!', towupper(u'!'));
  EXPECT_EQ(u'A', towupper(u'a'));
  EXPECT_EQ(u'À', towupper(u'à'));
  EXPECT_EQ(L'𝛥', towupper(L'𝛿'));
  EXPECT_EQ(L'Ｂ', towupper(L'ｂ'));
  EXPECT_EQ(u'Ꭰ', towupper(u'ꭰ'));
}

TEST(towlower, test) {
  EXPECT_EQ(u'!', towlower(u'!'));
  EXPECT_EQ(u'a', towlower(u'A'));
  EXPECT_EQ(u'à', towlower(u'À'));
  EXPECT_EQ(L'𝛿', towlower(L'𝛥'));
  EXPECT_EQ(L'ｂ', towlower(L'Ｂ'));
  EXPECT_EQ(u'ꭰ', towlower(u'Ꭰ'));
}

BENCH(towupper, bench) {
  EZBENCH2("towupper ascii", donothing, EXPROPRIATE(towupper(VEIL("r", L'a'))));
  EZBENCH2("towupper latin1", donothing,
           EXPROPRIATE(towupper(VEIL("r", u'A'))));
  EZBENCH2("towupper watinc", donothing,
           EXPROPRIATE(towupper(VEIL("r", u'Ỿ'))));
  EZBENCH2("towupper greek", donothing, EXPROPRIATE(towupper(VEIL("r", u'α'))));
  EZBENCH2("towupper astral", donothing,
           EXPROPRIATE(towupper(VEIL("r", L'𝛿'))));
}

BENCH(towlower, bench) {
  EZBENCH2("towlower ascii", donothing, EXPROPRIATE(towlower(VEIL("r", L'a'))));
  EZBENCH2("towlower latin1", donothing,
           EXPROPRIATE(towlower(VEIL("r", u'A'))));
  EZBENCH2("towlower watinc", donothing,
           EXPROPRIATE(towlower(VEIL("r", u'Ỿ'))));
  EZBENCH2("towlower greek", donothing, EXPROPRIATE(towupper(VEIL("r", u'α'))));
  EZBENCH2("towlower astral", donothing,
           EXPROPRIATE(towlower(VEIL("r", L'𝛿'))));
}

BENCH(iswupper, bench) {
  EZBENCH2("iswupper ascii", donothing, EXPROPRIATE(iswupper(VEIL("r", L'A'))));
  EZBENCH2("iswupper latin1", donothing,
           EXPROPRIATE(iswupper(VEIL("r", u'A'))));
  EZBENCH2("iswupper astral", donothing,
           EXPROPRIATE(iswupper(VEIL("r", L'𝛿'))));
}

BENCH(iswlower, bench) {
  EZBENCH2("iswlower ascii", donothing, EXPROPRIATE(iswlower(VEIL("r", L'a'))));
  EZBENCH2("iswlower latin1", donothing,
           EXPROPRIATE(iswlower(VEIL("r", u'A'))));
  EZBENCH2("iswlower astral", donothing,
           EXPROPRIATE(iswlower(VEIL("r", L'𝛿'))));
}
