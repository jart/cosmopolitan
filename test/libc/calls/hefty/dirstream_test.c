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
#include "libc/calls/struct/dirent.h"
#include "libc/rand/rand.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

TEST(dirstream, test) {
  DIR *dir;
  struct dirent *ent;
  char *dpath, *file1, *file2;
  dpath = gc(xasprintf("%s%s%lu", kTmpPath, "dirstream", rand32()));
  file1 = gc(xasprintf("%s/%s", dpath, "foo"));
  file2 = gc(xasprintf("%s/%s", dpath, "bar"));
  EXPECT_NE(-1, mkdir(dpath, 0755));
  EXPECT_NE(-1, touch(file1, 0644));
  EXPECT_NE(-1, touch(file2, 0644));

  EXPECT_TRUE(NULL != (dir = opendir(dpath)));
  bool hasfoo = false;
  bool hasbar = false;
  while ((ent = readdir(dir))) {
    if (strcmp(ent->d_name, "foo")) hasfoo = true;
    if (strcmp(ent->d_name, "bar")) hasbar = true;
  }
  EXPECT_TRUE(hasfoo);
  EXPECT_TRUE(hasbar);
  EXPECT_NE(-1, closedir(dir));

  EXPECT_NE(-1, unlink(file2));
  EXPECT_NE(-1, unlink(file1));
  EXPECT_NE(-1, rmdir(dpath));
}
