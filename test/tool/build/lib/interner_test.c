/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/mem/mem.h"
#include "libc/runtime/gc.h"
#include "libc/str/str.h"
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
