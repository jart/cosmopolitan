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
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

TEST(strcpy, test) {
  char buf[64];
  EXPECT_STREQ("hello", strcpy(buf, "hello"));
  EXPECT_STREQ("hello there what's up", strcpy(buf, "hello there what's up"));
}

BENCH(strcpy, bench) {
  extern char *strcpy_(char *, const char *) asm("strcpy");
  static char buf[1024], buf2[1024];
  memset(buf2, -1, sizeof(buf2) - 1);
  EZBENCH2("strcpy 1", donothing, strcpy_(buf, ""));
  EZBENCH2("strcpy 2", donothing, strcpy_(buf, "1"));
  EZBENCH2("strcpy 7", donothing, strcpy_(buf, "123456"));
  EZBENCH2("strcpy 8", donothing, strcpy_(buf, "1234567"));
  EZBENCH2("strcpy 9", donothing, strcpy_(buf, "12345678"));
  EZBENCH2("strcpy 16", donothing, strcpy_(buf, "123456781234567"));
  EZBENCH2("strcpy 1023", donothing, strcpy_(buf, buf2));
}
