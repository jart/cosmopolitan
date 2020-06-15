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
#include "libc/dns/dns.h"
#include "libc/errno.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

TEST(pascalifydnsname, testEmpty) {
  uint8_t *buf = tmalloc(1);
  char *name = tstrdup("");
  EXPECT_EQ(0, pascalifydnsname(buf, 1, name));
  EXPECT_BINEQ(u" ", buf);
  tfree(name);
  tfree(buf);
}

TEST(pascalifydnsname, testOneLabel) {
  uint8_t *buf = tmalloc(1 + 3 + 1);
  char *name = tstrdup("foo");
  EXPECT_EQ(1 + 3, pascalifydnsname(buf, 1 + 3 + 1, name));
  EXPECT_BINEQ(u"♥foo ", buf);
  tfree(name);
  tfree(buf);
}

TEST(pascalifydnsname, testTwoLabels) {
  uint8_t *buf = tmalloc(1 + 3 + 1 + 3 + 1);
  char *name = tstrdup("foo.bar");
  EXPECT_EQ(1 + 3 + 1 + 3, pascalifydnsname(buf, 1 + 3 + 1 + 3 + 1, name));
  EXPECT_BINEQ(u"♥foo♥bar ", buf);
  tfree(name);
  tfree(buf);
}

TEST(pascalifydnsname, testFqdnDot_isntIncluded) {
  uint8_t *buf = tmalloc(1 + 3 + 1 + 3 + 1);
  char *name = tstrdup("foo.bar.");
  EXPECT_EQ(1 + 3 + 1 + 3, pascalifydnsname(buf, 1 + 3 + 1 + 3 + 1, name));
  EXPECT_BINEQ(u"♥foo♥bar ", buf);
  tfree(name);
  tfree(buf);
}

TEST(pascalifydnsname, testTooLong) {
  uint8_t *buf = tmalloc(1);
  char *name = tmalloc(1000);
  memset(name, '.', 999);
  name[999] = '\0';
  EXPECT_EQ(-1, pascalifydnsname(buf, 1, name));
  EXPECT_EQ(ENAMETOOLONG, errno);
  tfree(name);
  tfree(buf);
}

TEST(pascalifydnsname, testNoSpace) {
  uint8_t *buf = tmalloc(1);
  char *name = tstrdup("foo");
  EXPECT_EQ(-1, pascalifydnsname(buf, 1, name));
  EXPECT_EQ(ENOSPC, errno);
  tfree(name);
  tfree(buf);
}
