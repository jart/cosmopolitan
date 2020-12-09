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
#include "libc/testlib/testlib.h"

TEST(cescapec, test) {
  EXPECT_EQ(' ', cescapec(0x20));
  EXPECT_EQ('~', cescapec(0x7E));
  EXPECT_EQ('\\' | 'r' << 8, cescapec('\r'));
  EXPECT_EQ('\\' | 'n' << 8, cescapec('\n'));
  EXPECT_EQ('\\' | '0' << 8 | '0' << 16 | '0' << 24, cescapec(0));
  EXPECT_EQ('\\' | '0' << 8 | '3' << 16 | '3' << 24, cescapec('\e'));
  EXPECT_EQ('\\' | '1' << 8 | '7' << 16 | '7' << 24, cescapec(0x7F));
  EXPECT_EQ('\\' | '3' << 8 | '7' << 16 | '7' << 24, cescapec(0xFF));
  EXPECT_EQ('\\' | '3' << 8 | '7' << 16 | '7' << 24, cescapec(0xFFFF));
  EXPECT_EQ('\\' | '3' << 8 | '7' << 16 | '7' << 24, cescapec(-1));
}
