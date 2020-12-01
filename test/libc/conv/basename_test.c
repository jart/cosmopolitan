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
#include "libc/bits/bits.h"
#include "libc/conv/conv.h"
#include "libc/mem/mem.h"
#include "libc/testlib/testlib.h"

TEST(basename, test) {
  EXPECT_STREQ("", basename(""));
  EXPECT_STREQ("/", basename("/"));
  EXPECT_STREQ("hello", basename("hello"));
  EXPECT_STREQ("there", basename("hello/there"));
  EXPECT_STREQ("yo", basename("hello/there/yo"));
}

TEST(basename, testTrailingSlash_isIgnored) {
  /* should be "foo" but basename() doesn't allocate memory */
  EXPECT_STREQ("foo/", basename("foo/"));
  EXPECT_STREQ("foo//", basename("foo//"));
}

TEST(basename, testOnlySlashes_oneSlashOnlyVasily) {
  EXPECT_STREQ("/", basename("///"));
}

TEST(basename, testWindows_isGrantedRespect) {
  EXPECT_STREQ("there", basename("hello\\there"));
  EXPECT_STREQ("yo", basename("hello\\there\\yo"));
}

TEST(dirname, test) {
  EXPECT_STREQ("/usr", dirname(strdup("/usr/lib")));
  EXPECT_STREQ("usr", dirname(strdup("usr/lib")));
  EXPECT_STREQ("/", dirname(strdup("/usr/")));
  EXPECT_STREQ("/", dirname(strdup("/")));
  EXPECT_STREQ(".", dirname(strdup("hello")));
  EXPECT_STREQ(".", dirname(strdup(".")));
  EXPECT_STREQ(".", dirname(strdup("..")));
}
