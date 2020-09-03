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
#include "libc/dce.h"
#include "libc/mem/mem.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

uint64_t i;
char pathbuf[PATH_MAX];
const char *oldpath, *bindir, *homedir, *binsh, *sh;

TEST(commandv_00, todo) { /* TODO(jart): Improve this on Windows. */
  if (IsWindows()) exit(0);
}

TEST(commandv_001, setupFiles) {
  mkdir("o", 0755);
  mkdir("o/tmp", 0755);
  oldpath = strdup(getenv("PATH"));
  homedir = xasprintf("o/tmp/home.%d", getpid());
  bindir = xasprintf("o/tmp/bin.%d", getpid());
  binsh = xasprintf("%s/sh.com", bindir);
  ASSERT_NE(-1, mkdir(homedir, 0755));
  ASSERT_NE(-1, mkdir(bindir, 0755));
  ASSERT_NE(-1, touch(binsh, 0755));
  ASSERT_NE(-1, setenv("PATH", bindir, true));
}

TEST(commandv_010, testSlashes_wontSearchPath_butChecksAccess) {
  sh = defer(unlink, gc(xasprintf("%s/sh.com", homedir)));
  EXPECT_NE(-1, touch(sh, 0755));
  i = g_syscount;
  EXPECT_STREQ(sh, commandv(sh, pathbuf));
  if (!IsWindows()) EXPECT_EQ(i + 1 /* access() */, g_syscount);
}

TEST(commandv_999, teardown) {
  setenv("PATH", oldpath, true);
  unlink(binsh);
  rmdir(bindir);
  rmdir(homedir);
  free(bindir);
  free(binsh);
  free(homedir);
  free(oldpath);
}
