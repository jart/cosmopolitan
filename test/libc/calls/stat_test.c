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
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/nt/files.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

char *pathname;
struct stat st;

TEST(stat_000, setupFiles) {
  mkdir("o", 0755);
  mkdir("o/tmp", 0755);
}

TEST(stat_010, testEmptyFile_sizeIsZero) {
  pathname = defer(
      unlink,
      gc(xasprintf("o/tmp/%s.%d", program_invocation_short_name, getpid())));
  ASSERT_NE(-1, touch(pathname, 0755));
  EXPECT_NE(-1, stat(pathname, &st));
  EXPECT_EQ(0, st.st_size);
}
