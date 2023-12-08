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
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/str/str.h"
#include "libc/testlib/blocktronics.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"

void SetUpOnce(void) {
  GetSymbolTable();
}

TEST(isutf8, good) {
  ASSERT_TRUE(isutf8("\0\1\2\3", 4));
  EXPECT_TRUE(isutf8(kHyperion, kHyperionSize));
  EXPECT_TRUE(isutf8("𐌰𐌱𐌲𐌳𐌴𐌵𐌶𐌷▒▒▒▒▒▒▒▒▒▒▒▒", -1));
  EXPECT_TRUE(isutf8("天地玄黄 宇宙洪荒 日月盈昃 辰宿列张 寒来暑往 秋收冬藏"
                     "闰馀成岁 律吕调阳 云腾致雨 露结为霜 金生丽水 玉出昆冈"
                     "剑号巨阙 珠称夜光 果珍李柰 菜重芥姜 海咸河淡 鳞潜羽翔"
                     "龙师火帝 鸟官人皇 始制文字 乃服衣裳 推位让国 有虞陶唐",
                     -1));
}

TEST(isutf8, bad) {
  ASSERT_FALSE(isutf8("\300\200", -1));              // overlong nul
  ASSERT_FALSE(isutf8("\200\300", -1));              // latin1 c1 control code
  ASSERT_FALSE(isutf8("\300\300", -1));              // missing continuation
  ASSERT_FALSE(isutf8("\377\200\200\200\200", -1));  // thompson-pike varint
}

TEST(isutf8, oob) {
  int n;
  char *p;
  for (n = 0; n < 32; ++n) {
    p = memset(malloc(n), 'a', n);
    ASSERT_TRUE(isutf8(p, n));
    free(p);
  }
}

BENCH(isutf8, bench) {
  EZBENCH_N("isutf8", 0, isutf8(0, 0));
  EZBENCH_N("isutf8", 5, isutf8("hello", 5));
  EZBENCH_N("isutf8 ascii", kHyperionSize, isutf8(kHyperion, kHyperionSize));
  EZBENCH_N("isutf8 unicode", kBlocktronicsSize,
            isutf8(kBlocktronics, kBlocktronicsSize));
}
