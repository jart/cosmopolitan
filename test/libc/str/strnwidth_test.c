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
#include "libc/assert.h"
#include "libc/str/strwidth.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"

TEST(strwidth, test) {
  EXPECT_EQ(5, strwidth("hello", 0));
  EXPECT_EQ(5, strwidth("\1he\e[0;0mllo\e#8", 0));
  EXPECT_EQ(10, strwidth("ＨＥＬＬＯ", 0));
}

TEST(strwidth, tab) {
  EXPECT_EQ(8, strwidth("\t", 0));
  EXPECT_EQ(8, strwidth(" \t", 0));
  EXPECT_EQ(8, strwidth("       \t", 0));
  EXPECT_EQ(16, strwidth("        \t", 0));
}

TEST(strwidth, hyperion) {
  ASSERT_EQ(22304, strwidth(kHyperion, 0));
}

TEST(strwidth, chinese) {
  ASSERT_EQ(53 * 4,
            strwidth("天地玄黄 宇宙洪荒 日月盈昃 辰宿列张 寒来暑往 秋收冬藏"
                     "闰馀成岁 律吕调阳 云腾致雨 露结为霜 金生丽水 玉出昆冈"
                     "剑号巨阙 珠称夜光 果珍李柰 菜重芥姜 海咸河淡 鳞潜羽翔"
                     "龙师火帝 鸟官人皇 始制文字 乃服衣裳 推位让国 有虞陶唐",
                     0));
}

TEST(strwidth, cjk) {
  ASSERT_EQ(37, strwidth("거주하는 사람들 중에서 한국어를 주로 ", 0));
}

BENCH(strnwidth, bench) {
  EZBENCH2("strnwidth", donothing, __expropriate(strnwidth(kHyperion, -1, 0)));
}
