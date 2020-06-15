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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/calls.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

int64_t fd;
struct stat st;
const char *path;

TEST(ftruncate, test) {
  mkdir("o", 0755);
  mkdir("o/tmp", 0755);
  path = gc(xasprintf("o/tmp/%s.%d", program_invocation_short_name, getpid()));
  ASSERT_NE(-1, (fd = creat(path, 0755)));
  ASSERT_EQ(5, write(fd, "hello", 5));
  errno = 31337;
  ASSERT_NE(-1, ftruncate(fd, 31337));
  EXPECT_EQ(31337, errno);
  ASSERT_EQ(5, write(fd, "world", 5));
  ASSERT_NE(-1, close(fd));
  ASSERT_NE(-1, stat(path, &st));
  ASSERT_EQ(31337, st.st_size);
  ASSERT_BINEQ(u"helloworld", gc(slurp(path, NULL)));
  unlink(path);
}
