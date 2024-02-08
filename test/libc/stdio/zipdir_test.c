/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/stat.h"
#include "libc/errno.h"
#include "libc/macros.internal.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/dt.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/s.h"
#include "libc/testlib/testlib.h"

__static_yoink("zipos");
__static_yoink("libc/testlib/hyperion.txt");
__static_yoink("libc/testlib/moby.txt");
__static_yoink("libc/testlib-test.txt");
__static_yoink("usr/share/zoneinfo/");
__static_yoink("usr/share/zoneinfo/New_York");

DIR *dir;
struct dirent *ent;

TEST(zipdir, testDirectoryThatHasRealZipEntry) {
  const char *path = "/zip/libc/testlib///";
  ASSERT_NE(NULL, (dir = opendir(path)));
  ASSERT_EQ(0, telldir(dir));
  ASSERT_NE(NULL, (ent = readdir(dir)));
  ASSERT_STREQ(".", ent->d_name);
  ASSERT_EQ(DT_DIR, ent->d_type);
  ASSERT_NE(NULL, (ent = readdir(dir)));
  ASSERT_STREQ("..", ent->d_name);
  ASSERT_EQ(DT_DIR, ent->d_type);
  ASSERT_NE(NULL, (ent = readdir(dir)));
  ASSERT_STREQ("hyperion.txt", ent->d_name);
  ASSERT_EQ(DT_REG, ent->d_type);
  long pos = telldir(dir);
  ASSERT_NE(NULL, (ent = readdir(dir)));
  ASSERT_STREQ("moby.txt", ent->d_name);
  ASSERT_EQ(DT_REG, ent->d_type);
  ASSERT_EQ(NULL, (ent = readdir(dir)));
  seekdir(dir, pos);
  ASSERT_NE(NULL, (ent = readdir(dir)));
  ASSERT_STREQ("moby.txt", ent->d_name);
  ASSERT_EQ(DT_REG, ent->d_type);
  ASSERT_EQ(NULL, (ent = readdir(dir)));
  ASSERT_EQ(NULL, (ent = readdir(dir)));
  ASSERT_SYS(0, 0, closedir(dir));
}

TEST(zipdir, testDirectoryWithSyntheticOrMissingEntry) {
  const char *path = "/zip/libc/";
  ASSERT_NE(NULL, (dir = opendir(path)));
  ASSERT_EQ(0, telldir(dir));
  ASSERT_NE(NULL, (ent = readdir(dir)));
  ASSERT_STREQ(".", ent->d_name);
  ASSERT_EQ(DT_DIR, ent->d_type);
  ASSERT_NE(NULL, (ent = readdir(dir)));
  ASSERT_STREQ("..", ent->d_name);
  ASSERT_EQ(DT_DIR, ent->d_type);
  ASSERT_NE(NULL, (ent = readdir(dir)));
  ASSERT_STREQ("testlib", ent->d_name);
  ASSERT_EQ(DT_DIR, ent->d_type);
  ASSERT_NE(NULL, (ent = readdir(dir)));
  ASSERT_STREQ("testlib-test.txt", ent->d_name);
  ASSERT_EQ(DT_REG, ent->d_type);
  ASSERT_EQ(NULL, (ent = readdir(dir)));
  ASSERT_SYS(0, 0, closedir(dir));
}

TEST(zipdir, testListZip) {
  const char *path = "/zip";
  ASSERT_NE(NULL, (dir = opendir(path)));
  ASSERT_EQ(0, telldir(dir));
  ASSERT_NE(NULL, (ent = readdir(dir)));
  ASSERT_STREQ(".", ent->d_name);
  ASSERT_EQ(DT_DIR, ent->d_type);
  ASSERT_NE(NULL, (ent = readdir(dir)));
  ASSERT_STREQ("..", ent->d_name);
  ASSERT_EQ(DT_DIR, ent->d_type);
  ASSERT_NE(NULL, (ent = readdir(dir)));
  ASSERT_STREQ("echo.com", ent->d_name);
  ASSERT_EQ(DT_REG, ent->d_type);
  ASSERT_NE(NULL, (ent = readdir(dir)));
  ASSERT_STREQ("libc", ent->d_name);
  ASSERT_EQ(DT_DIR, ent->d_type);
  ASSERT_NE(NULL, (ent = readdir(dir)));
  ASSERT_STREQ("usr", ent->d_name);
  ASSERT_EQ(DT_DIR, ent->d_type);
  ASSERT_NE(NULL, (ent = readdir(dir)));
  ASSERT_STREQ(".cosmo", ent->d_name);
  ASSERT_EQ(DT_REG, ent->d_type);
  ASSERT_EQ(NULL, (ent = readdir(dir)));
  ASSERT_SYS(0, 0, closedir(dir));
}

TEST(dirstream, hasDirectoryEntry) {
  struct stat st;
  bool gotsome = false;
  const char *path = "/zip/usr/share/zoneinfo";
  ASSERT_SYS(0, 0, fstatat(AT_FDCWD, path, &st, 0));
  ASSERT_TRUE(S_ISDIR(st.st_mode));
  ASSERT_NE(NULL, (dir = opendir(path)));
  while ((ent = readdir(dir))) {
    gotsome = true;
  }
  ASSERT_SYS(0, 0, closedir(dir));
  EXPECT_TRUE(gotsome);
}

TEST(__zipos_normpath, emptyBuf_wontNulTerminate) {
  __zipos_normpath(0, "hello", 0);
}

TEST(__zipos_normpath, overflows_willNulTerminate) {
  char buf[2];
  ASSERT_EQ(2, __zipos_normpath(buf, "hello", 2));
  ASSERT_STREQ("h", buf);
}

TEST(__zipos_normpath, vectors) {
  static const char V[][2][128] = {
      {"", ""},
      {"/", ""},
      {"/..", ""},
      {"/../", ""},
      {".", ""},
      {"./", ""},
      {"..", ""},
      {"../", ""},
      {"./foo/", "foo/"},
      {"foo/", "foo/"},
      {"../abc/def", "abc/def"},
      {"../abc/def/..", "abc/"},
      {"../abc/././././def/..", "abc/"},
      {"////../abc/def", "abc/def"},
      {"/../def", "def"},
      {"../def", "def"},
      {"/abc////../def", "def"},
      {"abc/../def/ghi", "def/ghi"},
      {"/abc/def/../ghi", "abc/ghi"},
      {"/abc/..abc////../def", "abc/def"},
      {"/abc/..abc/../def", "abc/def"},
      {"/abc/..abc/def", "abc/..abc/def"},
      {"abc/../def", "def"},
      {"abc/../../def", "def"},
      {"abc/../../../def", "def"},
      {"././", ""},
      {"abc/..", ""},
      {"abc/../", ""},
      {"abc/../..", ""},
      {"abc/../../", ""},
      {"a/..", ""},
      {"a/../", ""},
      {"a/../..", ""},
      {"a/../../", ""},
      {"../../a", "a"},
      {"../../../a", "a"},
      {"../a../../a", "a"},
      {"cccc/abc////..////.//../", ""},
      {"aaaa/cccc/abc////..////.//../", "aaaa/"},
      {"..//////.///..////..////.//////abc////.////..////def//abc/..", "def/"},
      {"////////////..//////.///..////..////.//////abc////.////..////def//abc/"
       "..",
       "def/"},
  };
  int fails = 0;

  // test non-overlapping arguments
  // duplicate input string for better asan checking
  for (int i = 0; i < ARRAYLEN(V); ++i) {
    char tmp[128];
    __zipos_normpath(tmp, gc(strdup(V[i][0])), sizeof(tmp));
    if (strcmp(tmp, V[i][1])) {
      if (++fails < 8) {
        fprintf(stderr,
                "\n%s: zipos path normalization test failed\n"
                "\tinput = %`'s\n"
                "\t want = %`'s\n"
                "\t  got = %`'s\n"
                "\t    i = %d\n",
                program_invocation_name, V[i][0], V[i][1], tmp, i);
      }
    }
  }

  // test overlapping arguments
  if (!fails) {
    for (int i = 0; i < ARRAYLEN(V); ++i) {
      char tmp[128];
      strcpy(tmp, V[i][0]);
      __zipos_normpath(tmp, tmp, sizeof(tmp));
      if (strcmp(tmp, V[i][1])) {
        if (++fails < 8) {
          fprintf(stderr,
                  "\n%s: zipos path normalization breaks w/ overlapping args\n"
                  "\tinput = %`'s\n"
                  "\t want = %`'s\n"
                  "\t  got = %`'s\n"
                  "\t    i = %d\n",
                  program_invocation_name, V[i][0], V[i][1], tmp, i);
        }
      }
    }
  }

  if (fails) {
    fprintf(stderr, "\n%d / %zd zipos path norm tests failed\n", fails,
            ARRAYLEN(V));
    exit(1);
  }
}
