/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/calls.h"
#include "libc/calls/struct/dirent.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/mem/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/dt.h"
#include "libc/testlib/testlib.h"
#include "libc/x/xasprintf.h"
#include "libc/x/xiso8601.h"

STATIC_YOINK("zip_uri_support");
STATIC_YOINK("usr/share/zoneinfo/New_York");

char testlib_enable_tmp_setup_teardown;

DIR *dir;
struct dirent *ent;

void SetUp(void) {
  dir = 0;
  ent = 0;
}

TEST(opendir, efault) {
  ASSERT_SYS(EFAULT, NULL, opendir(0));
  if (!IsAsan()) return;  // not possible
  ASSERT_SYS(EFAULT, NULL, opendir((void *)77));
}

TEST(opendir, enoent) {
  ASSERT_SYS(ENOENT, NULL, opendir(""));
  ASSERT_SYS(ENOENT, NULL, opendir("o/foo"));
}

TEST(opendir, enotdir) {
  ASSERT_SYS(0, 0, close(creat("yo", 0644)));
  ASSERT_SYS(ENOTDIR, NULL, opendir("yo/there"));
}

TEST(opendir, zipTest_fake) {
  ASSERT_NE(NULL, (dir = opendir("/zip")));
  EXPECT_NE(NULL, (ent = readdir(dir)));
  EXPECT_STREQ(".cosmo", ent->d_name);
  EXPECT_NE(NULL, (ent = readdir(dir)));
  EXPECT_STREQ("echo", ent->d_name);
  EXPECT_NE(NULL, (ent = readdir(dir)));
  EXPECT_STREQ("usr", ent->d_name);
  EXPECT_EQ(NULL, (ent = readdir(dir)));
  EXPECT_EQ(0, closedir(dir));
  ASSERT_NE(NULL, (dir = opendir("/zip/")));
  EXPECT_NE(NULL, (ent = readdir(dir)));
  EXPECT_STREQ(".cosmo", ent->d_name);
  EXPECT_NE(NULL, (ent = readdir(dir)));
  EXPECT_STREQ("echo", ent->d_name);
  EXPECT_NE(NULL, (ent = readdir(dir)));
  EXPECT_STREQ("usr", ent->d_name);
  EXPECT_EQ(NULL, (ent = readdir(dir)));
  EXPECT_EQ(0, closedir(dir));
  ASSERT_NE(NULL, (dir = opendir("/zip/usr")));
  EXPECT_NE(NULL, (ent = readdir(dir)));
  EXPECT_STREQ("share", ent->d_name);
  EXPECT_EQ(NULL, (ent = readdir(dir)));
  EXPECT_EQ(0, closedir(dir));
  ASSERT_NE(NULL, (dir = opendir("/zip/usr/")));
  EXPECT_NE(NULL, (ent = readdir(dir)));
  EXPECT_STREQ("share", ent->d_name);
  EXPECT_EQ(NULL, (ent = readdir(dir)));
  EXPECT_EQ(0, closedir(dir));
  EXPECT_EQ(NULL, (dir = opendir("/zip/us")));
  EXPECT_EQ(NULL, (dir = opendir("/zip/us/")));
}

TEST(dirstream, testDots) {
  int hasdot = 0;
  int hasdotdot = 0;
  ASSERT_SYS(0, 0, close(creat("foo", 0644)));
  ASSERT_NE(NULL, (dir = opendir(".")));
  while ((ent = readdir(dir))) {
    if (!strcmp(ent->d_name, ".")) {
      ++hasdot;
      EXPECT_EQ(DT_DIR, ent->d_type);
    }
    if (!strcmp(ent->d_name, "..")) {
      ++hasdotdot;
      EXPECT_EQ(DT_DIR, ent->d_type);
    }
  }
  EXPECT_EQ(1, hasdot);
  EXPECT_EQ(1, hasdotdot);
  EXPECT_SYS(0, 0, closedir(dir));
}

TEST(dirstream, test) {
  bool hasfoo = false;
  bool hasbar = false;
  char *dpath, *file1, *file2;
  dpath = _gc(xasprintf("%s.%d", "dirstream", rand()));
  file1 = _gc(xasprintf("%s/%s", dpath, "foo"));
  file2 = _gc(xasprintf("%s/%s", dpath, "bar"));
  EXPECT_NE(-1, mkdir(dpath, 0755));
  EXPECT_NE(-1, touch(file1, 0644));
  EXPECT_NE(-1, touch(file2, 0644));
  EXPECT_TRUE(NULL != (dir = opendir(dpath)));
  while ((ent = readdir(dir))) {
    if (!strcmp(ent->d_name, "foo")) {
      EXPECT_EQ(DT_REG, ent->d_type);
      hasfoo = true;
    }
    if (!strcmp(ent->d_name, "bar")) {
      EXPECT_EQ(DT_REG, ent->d_type);
      hasbar = true;
    }
  }
  EXPECT_TRUE(hasfoo);
  EXPECT_TRUE(hasbar);
  EXPECT_NE(-1, closedir(dir));
  EXPECT_NE(-1, unlink(file2));
  EXPECT_NE(-1, unlink(file1));
  EXPECT_NE(-1, rmdir(dpath));
}

TEST(dirstream, zipTest) {
  bool foundNewYork = false;
  const char *path = "/zip/usr/share/zoneinfo/";
  ASSERT_NE(0, _gc(xiso8601ts(NULL)));
  ASSERT_NE(NULL, (dir = opendir(path)));
  while ((ent = readdir(dir))) {
    foundNewYork |= !strcmp(ent->d_name, "New_York");
  }
  closedir(dir);
  EXPECT_TRUE(foundNewYork);
}

TEST(rewinddir, test) {
  bool hasfoo = false;
  bool hasbar = false;
  char *dpath, *file1, *file2;
  dpath = _gc(xasprintf("%s.%d", "dirstream", rand()));
  file1 = _gc(xasprintf("%s/%s", dpath, "foo"));
  file2 = _gc(xasprintf("%s/%s", dpath, "bar"));
  EXPECT_NE(-1, mkdir(dpath, 0755));
  EXPECT_NE(-1, touch(file1, 0644));
  EXPECT_NE(-1, touch(file2, 0644));
  EXPECT_TRUE(NULL != (dir = opendir(dpath)));
  readdir(dir);
  readdir(dir);
  readdir(dir);
  rewinddir(dir);
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

TEST(dirstream, zipTest_notDir) {
  ASSERT_EQ(NULL, opendir("/zip/usr/share/zoneinfo/New_York"));
  ASSERT_EQ(ENOTDIR, errno);
}
