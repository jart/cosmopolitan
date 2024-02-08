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
#include "libc/mem/mem.h"
#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"
#include "net/http/escape.h"
#include "third_party/mbedtls/base64.h"

size_t i, n, m;
char *p, *q, b[32];

TEST(EncodeBase64, test) {
  EXPECT_STREQ("", gc(EncodeBase64("", 0, 0)));
  EXPECT_STREQ("AA==", gc(EncodeBase64("\0", 1, 0)));
  EXPECT_STREQ("AAA=", gc(EncodeBase64("\0\0", 2, 0)));
  EXPECT_STREQ("AAAA", gc(EncodeBase64("\0\0\0", 3, 0)));
  EXPECT_STREQ("/w==", gc(EncodeBase64("\377", 1, 0)));
  EXPECT_STREQ("//8=", gc(EncodeBase64("\377\377", 2, 0)));
  EXPECT_STREQ("////", gc(EncodeBase64("\377\377\377", 3, 0)));
  EXPECT_STREQ("aGVsbG8=", gc(EncodeBase64("hello", 5, 0)));
  EXPECT_STREQ(
      "AAECAwQFBgcICQoLDA0ODxAREhMUFRYXGBkaGxwdHh8gISIjJCUmJygpKissLS4vMDEy"
      "MzQ1Njc4OTo7PD0+P0BBQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWltcXV5fYGFiY2Rl"
      "ZmdoaWprbG1ub3BxcnN0dXZ3eHl6e3x9fn+AgYKDhIWGh4iJiouMjY6PkJGSk5SVlpeY"
      "mZqbnJ2en6ChoqOkpaanqKmqq6ytrq+wsbKztLW2t7i5uru8vb6/wMHCw8TFxsfIycrL"
      "zM3Oz9DR0tPU1dbX2Nna29zd3t/g4eLj5OXm5+jp6uvs7e7v8PHy8/T19vf4+fr7/P3+"
      "/w==",
      gc(EncodeBase64(
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
          256, &n)));
  EXPECT_EQ(344, n);
}

TEST(EncodeBase64, testOom_returnsNullAndSetsSizeToZero) {
  n = 31337;
  EXPECT_EQ(NULL, EncodeBase64("hello", 0x1000000000000, &n));
  EXPECT_EQ(0, n);
}

TEST(Base64, RoundTrip) {
  for (i = 0; i < 1000; ++i) {
    n = rand() % 32;
    rngset(b, n, _rand64, -1);
    p = EncodeBase64(b, n, &m);
    q = DecodeBase64(p, m, &m);
    ASSERT_EQ(n, m);
    ASSERT_EQ(0, memcmp(b, q, n));
    free(p);
    free(q);
  }
}

TEST(Base64, Fuzz) {
  for (i = 0; i < 1000; ++i) {
    n = rand() % 32;
    rngset(b, n, _rand64, -1);
    free(DecodeBase64(p, m, 0));
  }
}

BENCH(EncodeBase64, bench) {
  EZBENCH2("EncodeBase64", donothing,
           free(EncodeBase64(kHyperion, kHyperionSize, 0)));
  p = gc(EncodeBase64(kHyperion, kHyperionSize, &n));
  EZBENCH2("DecodeBase64", donothing, free(DecodeBase64(p, n, 0)));
}

BENCH(MbedtlsEncodeBase64, bench) {
  size_t olen;
  size_t dlen = kHyperionSize * 4;
  uint8_t *dst = gc(malloc(dlen));
  uint8_t *res = gc(malloc(dlen));
  EZBENCH2("mbedtls_base64_encode", donothing,
           mbedtls_base64_encode(dst, dlen, &olen, (void *)kHyperion,
                                 kHyperionSize));
  EZBENCH2("mbedtls_base64_decode", donothing,
           mbedtls_base64_decode(res, dlen, &olen, dst, olen));
  ASSERT_EQ(0, memcmp(res, kHyperion, olen));
}
