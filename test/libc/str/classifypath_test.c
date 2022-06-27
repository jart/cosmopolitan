/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/dce.h"
#include "libc/str/path.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

TEST(isabspath, testUniversal) {
  ASSERT_TRUE(_isabspath("/home/jart/foo.txt"));
}

TEST(isabspath, testDosPaths) {
  if (!SupportsWindows()) return;
  ASSERT_FALSE(_isabspath("C:"));
  ASSERT_FALSE(_isabspath("C:foo.txt"));
  ASSERT_TRUE(_isabspath("C:/"));
  ASSERT_TRUE(_isabspath("C:/Users/jart/foo.txt"));
  ASSERT_TRUE(_isabspath("C:\\Users\\jart\\foo.txt"));
  ASSERT_TRUE(_isabspath("\\Users\\jart\\foo.txt"));
}

TEST(isabspath, testWin32Paths) {
  if (!SupportsWindows()) return;
  ASSERT_TRUE(_isabspath("\\\\?\\C:\\.."));
  ASSERT_TRUE(_isabspath("\\\\.\\C:\\Users\\jart\\foo.txt"));
}

TEST(isabspath, testNtPaths) {
  if (!SupportsWindows()) return;
  ASSERT_TRUE(_isabspath("\\??\\C:\\Users\\jart\\foo.txt"));
}

TEST(_classifypath, test) {
  if (!SupportsWindows()) return;
  EXPECT_EQ(0, _classifypath(""));
  EXPECT_EQ(0, _classifypath("xyz"));
  EXPECT_EQ(_kPathDos | _kPathDev, _classifypath("CON"));
  EXPECT_EQ(_kPathDos | _kPathDev, _classifypath("NUL"));
  EXPECT_EQ(0, _classifypath(":"));
  EXPECT_EQ(_kPathDos, _classifypath("::"));
  EXPECT_EQ(_kPathDos, _classifypath(":::"));
  EXPECT_EQ(_kPathDos, _classifypath("::::"));
  EXPECT_EQ(_kPathAbs | _kPathDos, _classifypath("::\\"));
  EXPECT_EQ(_kPathAbs, _classifypath("\\"));
  EXPECT_EQ(_kPathAbs, _classifypath("\\:"));
  EXPECT_EQ(_kPathAbs, _classifypath("\\C:"));
  EXPECT_EQ(_kPathAbs, _classifypath("\\C:\\"));
  EXPECT_EQ(_kPathAbs, _classifypath("/"));
  EXPECT_EQ(_kPathAbs, _classifypath("/:"));
  EXPECT_EQ(_kPathAbs, _classifypath("/C:"));
  EXPECT_EQ(_kPathAbs, _classifypath("/C:/"));
  EXPECT_EQ(0, _classifypath("C"));
  EXPECT_EQ(_kPathDos, _classifypath("C:"));
  EXPECT_EQ(_kPathDos, _classifypath("C:a"));
  EXPECT_EQ(_kPathDos, _classifypath("C:a\\"));
  EXPECT_EQ(_kPathAbs | _kPathDos, _classifypath("C:\\"));
  EXPECT_EQ(_kPathAbs | _kPathDos, _classifypath("C:/"));
  EXPECT_EQ(_kPathAbs | _kPathDos, _classifypath("C:\\a"));
  EXPECT_EQ(_kPathAbs | _kPathDos, _classifypath("C:/a"));
  EXPECT_EQ(_kPathAbs | _kPathDos, _classifypath("C:\\\\"));
  EXPECT_EQ(_kPathAbs | _kPathWin, _classifypath("\\\\"));
  EXPECT_EQ(_kPathAbs | _kPathWin, _classifypath("\\\\\\"));
  EXPECT_EQ(_kPathAbs | _kPathWin, _classifypath("\\\\;"));
  EXPECT_EQ(_kPathAbs | _kPathWin, _classifypath("\\\\f\\b\\"));
  EXPECT_EQ(_kPathAbs | _kPathWin, _classifypath("\\\\f\\b"));
  EXPECT_EQ(_kPathAbs | _kPathWin, _classifypath("\\\\f\\"));
  EXPECT_EQ(_kPathAbs | _kPathWin, _classifypath("\\\\f"));
  EXPECT_EQ(_kPathAbs | _kPathNt, _classifypath("\\??\\"));
  EXPECT_EQ(_kPathAbs | _kPathNt, _classifypath("\\??\\UNC"));
  EXPECT_EQ(_kPathAbs | _kPathNt, _classifypath("\\??\\UNC\\"));
  EXPECT_EQ(_kPathAbs, _classifypath("\\?"));
  EXPECT_EQ(_kPathAbs, _classifypath("\\?\\"));
  EXPECT_EQ(_kPathAbs, _classifypath("\\?\\UNC"));
  EXPECT_EQ(_kPathAbs, _classifypath("\\?\\UNC\\"));
  EXPECT_EQ(_kPathAbs | _kPathWin | _kPathDev, _classifypath("\\\\?\\UNC\\"));
  EXPECT_EQ(_kPathAbs | _kPathWin | _kPathDev | _kPathRoot,
            _classifypath("\\\\?"));
  EXPECT_EQ(_kPathAbs | _kPathWin, _classifypath("\\\\??"));
  EXPECT_EQ(_kPathAbs | _kPathWin, _classifypath("\\\\??\\"));
  EXPECT_EQ(_kPathAbs | _kPathWin, _classifypath("\\\\??\\C:\\"));
  EXPECT_EQ(_kPathAbs | _kPathWin | _kPathDev | _kPathRoot,
            _classifypath("\\\\."));
  EXPECT_EQ(_kPathAbs | _kPathWin | _kPathDev, _classifypath("\\\\.\\"));
  EXPECT_EQ(_kPathAbs | _kPathWin | _kPathDev, _classifypath("\\\\.\\C:\\"));
  EXPECT_EQ(_kPathAbs | _kPathWin, _classifypath("\\/"));
  EXPECT_EQ(_kPathAbs | _kPathWin, _classifypath("/\\"));
  EXPECT_EQ(_kPathAbs | _kPathWin, _classifypath("//"));
  EXPECT_EQ(_kPathAbs | _kPathWin, _classifypath("///"));
  EXPECT_EQ(_kPathAbs | _kPathWin, _classifypath("//;"));
  EXPECT_EQ(_kPathAbs | _kPathWin | _kPathDev | _kPathRoot,
            _classifypath("//?"));
  EXPECT_EQ(_kPathAbs | _kPathWin | _kPathDev | _kPathRoot,
            _classifypath("/\\?"));
  EXPECT_EQ(_kPathAbs | _kPathWin | _kPathDev | _kPathRoot,
            _classifypath("\\/?"));
  EXPECT_EQ(_kPathAbs | _kPathWin, _classifypath("//??"));
  EXPECT_EQ(_kPathAbs | _kPathWin | _kPathDev | _kPathRoot,
            _classifypath("//."));
  EXPECT_EQ(_kPathAbs | _kPathWin | _kPathDev | _kPathRoot,
            _classifypath("\\/."));
  EXPECT_EQ(_kPathAbs | _kPathWin | _kPathDev | _kPathRoot,
            _classifypath("/\\."));
  EXPECT_EQ(_kPathAbs | _kPathWin | _kPathDev, _classifypath("//./"));
  EXPECT_EQ(_kPathAbs | _kPathWin | _kPathDev, _classifypath("//./C:/"));
}
