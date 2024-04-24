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
#include "libc/mem/mem.h"
#include "libc/mem/gc.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/fastrandomstring.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"
#include "tool/build/lib/interner.h"

TEST(interner, test) {
  struct Interner *t = defer(freeinterner, newinterner());
  EXPECT_STREQ("hi", &t->p[intern(t, gc(strdup("hi")))]);
  EXPECT_STREQ("there", &t->p[intern(t, gc(strdup("there")))]);
  EXPECT_STREQ("hi", &t->p[intern(t, gc(strdup("hi")))]);
  EXPECT_STREQ("there", &t->p[intern(t, gc(strdup("there")))]);
  EXPECT_BINEQ(u"hi there  ", t->p);
  EXPECT_EQ(strlen("hi") + 1 + strlen("there") + 1, t->i);
}

TEST(isinterned, test) {
  struct Interner *t = defer(freeinterner, newinterner());
  ASSERT_FALSE(isinterned(t, "doge"));
  intern(t, "doge");
  ASSERT_TRUE(isinterned(t, "doge"));
}

TEST(interner, testWordCount) {
  struct Interner *t = defer(freeinterner, newinterner());
  size_t i, j;
  char word[16];
  for (i = 0, j = 0; i < kHyperionSize; ++i) {
    if (isalpha(kHyperion[i]) || kHyperion[i] == '\'') {
      word[j++] = tolower(kHyperion[i]);
    } else if (j) {
      word[j] = '\0';
      intern(t, word);
      j = 0;
    }
  }
  EXPECT_BINEQ(u"the fall of hyperion a dream", t->p);
  /* 1547 = grep -Po "['a-zA-Z]+" hyperion.txt | tr A-Z a-z | dedupe | wc -l */
  EXPECT_EQ(1548, interncount(t));
  EXPECT_EQ(10502, t->i);
  EXPECT_LT(t->i, t->n);
  EXPECT_EQ('\0', t->p[t->i]);
}

BENCH(interner, bench) {
  struct Interner *t = defer(freeinterner, newinterner());
  intern(t, "hellos");
  EZBENCH2("intern hit", donothing, intern(t, "hellos"));
  EZBENCH2("intern miss", donothing, intern(t, FastRandomString()));
  EZBENCH2("isinterned hit", donothing, isinterned(t, "hellos"));
  EZBENCH2("isinterned miss", donothing, isinterned(t, FastRandomString()));
}
