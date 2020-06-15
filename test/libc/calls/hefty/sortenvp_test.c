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
#include "libc/runtime/gc.h"
#include "libc/calls/hefty/ntspawn.h"
#include "libc/testlib/testlib.h"

TEST(sortenvp, test) {
  char *envp[] = {"u=b", "c=d", "韩=非", "uh=d", "hduc=d", NULL};
  char **sortedenvp = gc(sortenvp(envp));
  EXPECT_STREQ("c=d", sortedenvp[0]);
  EXPECT_STREQ("hduc=d", sortedenvp[1]);
  EXPECT_STREQ("u=b", sortedenvp[2]);
  EXPECT_STREQ("uh=d", sortedenvp[3]);
  EXPECT_STREQ("韩=非", sortedenvp[4]);
  EXPECT_EQ(NULL, sortedenvp[5]);
}
