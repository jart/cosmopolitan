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
#include "libc/mem/gc.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"
#include "net/http/escape.h"

size_t i, n, m;
char *p, *q, b[32];

TEST(DecodeBase64, paddingIsOptional) {
  EXPECT_BINEQ(u" ", gc(DecodeBase64("", 0, 0)));
  EXPECT_BINEQ(u"  ", gc(DecodeBase64("AA", 2, 0)));
  EXPECT_BINEQ(u"♦ ", gc(DecodeBase64("BB", 2, 0)));
  EXPECT_BINEQ(u"   ", gc(DecodeBase64("AAA", 3, 0)));
  EXPECT_BINEQ(u"    ", gc(DecodeBase64("AAAA", 4, 0)));
  EXPECT_BINEQ(u"λ ", gc(DecodeBase64("/w", 2, 0)));
  EXPECT_BINEQ(u"λλ ", gc(DecodeBase64("//8", 3, 0)));
  EXPECT_BINEQ(u"λλλ ", gc(DecodeBase64("////", 4, 0)));
  EXPECT_BINEQ(u"hello ", gc(DecodeBase64("aGVsbG8", 7, 0)));
  EXPECT_BINEQ(u"hello ", gc(DecodeBase64("aGVsbG8", -1, 0)));
}

TEST(DecodeBase64, altUrlFormat) {
  EXPECT_BINEQ(u"λ ", gc(DecodeBase64("_w", 2, 0)));
  EXPECT_BINEQ(u"λλ ", gc(DecodeBase64("__8", 3, 0)));
  EXPECT_BINEQ(u"λλλ ", gc(DecodeBase64("____", 4, 0)));
}

TEST(DecodeBase64, test) {
  EXPECT_BINEQ(u" ", gc(DecodeBase64("", 0, 0)));
  EXPECT_BINEQ(u"  ", gc(DecodeBase64("AA==", 4, 0)));
  EXPECT_BINEQ(u"♦ ", gc(DecodeBase64("BB==", 4, 0)));
  EXPECT_BINEQ(u"   ", gc(DecodeBase64("AAA=", 4, 0)));
  EXPECT_BINEQ(u"    ", gc(DecodeBase64("AAAA", 4, 0)));
  EXPECT_BINEQ(u"λ ", gc(DecodeBase64("/w==", 4, 0)));
  EXPECT_BINEQ(u"λλ ", gc(DecodeBase64("//8=", 4, 0)));
  EXPECT_BINEQ(u"λλλ ", gc(DecodeBase64("////", 4, 0)));
  EXPECT_BINEQ(u"hello ", gc(DecodeBase64("aGVsbG8=", 8, 0)));
  EXPECT_BINEQ(u"hello ", gc(DecodeBase64("aGVsbG8=", -1, 0)));
  EXPECT_EQ(
      0,
      memcmp(
          "\000\001\002\003\004\005\006\007\010\011\012\013\014\015\016\017\020"
          "\021\022\023\024\025\026\027\030\031\032\033\034\035\036\037\040\041"
          "\042\043\044\045\046\047\050\051\052\053\054\055\056\057\060\061\062"
          "\063\064\065\066\067\070\071\072\073\074\075\076\077\100\101\102\103"
          "\104\105\106\107\110\111\112\113\114\115\116\117\120\121\122\123\124"
          "\125\126\127\130\131\132\133\134\135\136\137\140\141\142\143\144\145"
          "\146\147\150\151\152\153\154\155\156\157\160\161\162\163\164\165\166"
          "\167\170\171\172\173\174\175\176\177\200\201\202\203\204\205\206\207"
          "\210\211\212\213\214\215\216\217\220\221\222\223\224\225\226\227\230"
          "\231\232\233\234\235\236\237\240\241\242\243\244\245\246\247\250\251"
          "\252\253\254\255\256\257\260\261\262\263\264\265\266\267\270\271\272"
          "\273\274\275\276\277\300\301\302\303\304\305\306\307\310\311\312\313"
          "\314\315\316\317\320\321\322\323\324\325\326\327\330\331\332\333\334"
          "\335\336\337\340\341\342\343\344\345\346\347\350\351\352\353\354\355"
          "\356\357\360\361\362\363\364\365\366\367\370\371\372\373\374\375\376"
          "\377",
          gc(DecodeBase64(
              "AAECAwQFBgcICQoLDA0ODxAREhMUFRYXGBkaGxwdHh8gISIjJCUm\r\n"
              "JygpKissLS4vMDEy\r\n"
              "MzQ1Njc4OTo7PD0+\r\n"
              "P0BBQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWltcXV5fYGFiY2Rl\r\n"
              "ZmdoaWprbG1ub3BxcnN0dXZ3eHl6e3x9fn+\r\n"
              "AgYKDhIWGh4iJiouMjY6PkJGSk5SVlpeY\r\n"
              "mZqbnJ2en6ChoqOkpaanqKmqq6ytrq+wsbKztLW2t7i5uru8vb6/\r\n"
              "wMHCw8TFxsfIycrL\r\n"
              "zM3Oz9DR0tPU1dbX2Nna29zd3t/\r\n"
              "g4eLj5OXm5+jp6uvs7e7v8PHy8/T19vf4+fr7/P3+\r\n"
              "/w==\r\n",
              -1, &n)),
          256));
  EXPECT_EQ(256, n);
}

TEST(DecodeBase64, testSeparators_skipsOverThemAtAnyState) {
  EXPECT_BINEQ(u" ", gc(DecodeBase64(" ", 1, 0)));
  EXPECT_BINEQ(u"  ", gc(DecodeBase64(" A A = = ", 9, 0)));
  EXPECT_BINEQ(u"♦ ", gc(DecodeBase64(" B B = = ", 4, 0)));
  EXPECT_BINEQ(u"hello ", gc(DecodeBase64("a\nG\nV\ns\nb\nG\n8\n=\n", 16, 0)));
}

TEST(DecodeBase64, testInvalidSequences_skipsOverThem) {
  EXPECT_BINEQ(u" ", gc(DecodeBase64("A===", 4, 0)));
  EXPECT_BINEQ(u" ", gc(DecodeBase64("B===", 4, 0)));
  EXPECT_BINEQ(u"♦ ", gc(DecodeBase64("B===BB==", 8, 0)));
  EXPECT_BINEQ(u"♦ ", gc(DecodeBase64("====BB==", 8, 0)));
}

TEST(DecodeBase64, testOom_returnsNullAndSetsSizeToZero) {
  n = 31337;
  EXPECT_EQ(NULL, DecodeBase64("hello", 0x1000000000000, &n));
  EXPECT_EQ(0, n);
}
