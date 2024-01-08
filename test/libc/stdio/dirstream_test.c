/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/struct/stat.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/kprintf.h"
#include "libc/limits.h"
#include "libc/mem/critbit0.h"
#include "libc/mem/gc.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/append.h"
#include "libc/stdio/ftw.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/dt.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/s.h"
#include "libc/testlib/testlib.h"
#include "libc/x/xasprintf.h"
#include "libc/x/xiso8601.h"

__static_yoink("zipos");
__static_yoink("usr/share/zoneinfo/");
__static_yoink("usr/share/zoneinfo/New_York");
__static_yoink("libc/testlib/hyperion.txt");
__static_yoink("libc/testlib/moby.txt");

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

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

TEST(opendir, openSyntheticDirEntry) {
  struct stat st;
  ASSERT_SYS(0, 3, open("/zip", O_RDONLY | O_DIRECTORY));
  ASSERT_SYS(0, 0, fstat(3, &st));
  ASSERT_TRUE(S_ISDIR(st.st_mode));
  EXPECT_SYS(EISDIR, -1, read(3, 0, 0));
  ASSERT_NE(NULL, (dir = fdopendir(3)));
  EXPECT_NE(NULL, (ent = readdir(dir)));
  EXPECT_EQ(0, closedir(dir));
}

TEST(opendir, openRealDirEntry) {
  struct stat st;
  ASSERT_SYS(0, 3, open("/zip/usr/share/zoneinfo", O_RDONLY | O_DIRECTORY));
  ASSERT_SYS(0, 0, fstat(3, &st));
  ASSERT_TRUE(S_ISDIR(st.st_mode));
  EXPECT_SYS(EISDIR, -1, read(3, 0, 0));
  ASSERT_NE(NULL, (dir = fdopendir(3)));
  EXPECT_NE(NULL, (ent = readdir(dir)));
  EXPECT_EQ(0, closedir(dir));
}

TEST(fdopendir, test) {
  ASSERT_SYS(0, 0, touch("foo", 0644));
  ASSERT_SYS(0, 3, open("foo", O_RDONLY));
  ASSERT_SYS(ENOTDIR, 0, fdopendir(3));
  ASSERT_SYS(0, 0, close(3));
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
  dpath = gc(xasprintf("%s.%d", "dirstream", rand()));
  file1 = gc(xasprintf("%s/%s", dpath, "foo"));
  file2 = gc(xasprintf("%s/%s", dpath, "bar"));
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
  ASSERT_NE(NULL, (dir = opendir(path)));
  while ((ent = readdir(dir))) {
    foundNewYork |= !strcmp(ent->d_name, "New_York");
  }
  EXPECT_SYS(0, 0, closedir(dir));
  EXPECT_TRUE(foundNewYork);
}

TEST(rewinddir, test) {
  bool hasfoo = false;
  bool hasbar = false;
  char *dpath, *file1, *file2;
  dpath = gc(xasprintf("%s.%d", "dirstream", rand()));
  file1 = gc(xasprintf("%s/%s", dpath, "foo"));
  file2 = gc(xasprintf("%s/%s", dpath, "bar"));
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

TEST(dirstream, ino) {
  ASSERT_SYS(0, 0, mkdir("boop", 0755));
  EXPECT_SYS(0, 0, touch("boop/a", 0644));
  EXPECT_SYS(0, 0, touch("boop/b", 0644));
  EXPECT_SYS(0, 0, touch("boop/c", 0644));
  ASSERT_NE(NULL, (dir = opendir("boop")));
  ASSERT_NE(NULL, (ent = readdir(dir)));  // #1
  ASSERT_NE(NULL, (ent = readdir(dir)));  // #2
  long pos = telldir(dir);
  ASSERT_NE(NULL, (ent = readdir(dir)));  // #3
  char name[32];
  strlcpy(name, ent->d_name, sizeof(name));
  ASSERT_NE(NULL, (ent = readdir(dir)));  // #4
  ASSERT_NE(NULL, (ent = readdir(dir)));  // #5
  ASSERT_EQ(NULL, (ent = readdir(dir)));  // eod
  seekdir(dir, pos);
  ASSERT_NE(NULL, (ent = readdir(dir)));  // #2
  ASSERT_STREQ(name, ent->d_name);
  ASSERT_NE(NULL, (ent = readdir(dir)));  // #3
  ASSERT_NE(NULL, (ent = readdir(dir)));  // #4
  ASSERT_EQ(NULL, (ent = readdir(dir)));  // eod
  ASSERT_EQ(NULL, (ent = readdir(dir)));  // eod
  ASSERT_SYS(0, 0, closedir(dir));
}

TEST(dirstream, seek) {
  ASSERT_SYS(0, 0, mkdir("boop", 0755));
  EXPECT_SYS(0, 0, touch("boop/a", 0644));
  EXPECT_SYS(0, 0, touch("boop/b", 0644));
  EXPECT_SYS(0, 0, touch("boop/c", 0644));
  ASSERT_NE(NULL, (dir = opendir("boop")));
  ASSERT_NE(NULL, (ent = readdir(dir)));  // #1
  ASSERT_NE(NULL, (ent = readdir(dir)));  // #2
  long pos = telldir(dir);
  ASSERT_NE(NULL, (ent = readdir(dir)));  // #3
  char name[32];
  strlcpy(name, ent->d_name, sizeof(name));
  ASSERT_NE(NULL, (ent = readdir(dir)));  // #4
  ASSERT_NE(NULL, (ent = readdir(dir)));  // #5
  ASSERT_EQ(NULL, (ent = readdir(dir)));  // eod
  seekdir(dir, pos);
  ASSERT_NE(NULL, (ent = readdir(dir)));  // #2
  ASSERT_STREQ(name, ent->d_name);
  ASSERT_NE(NULL, (ent = readdir(dir)));  // #3
  ASSERT_NE(NULL, (ent = readdir(dir)));  // #4
  ASSERT_EQ(NULL, (ent = readdir(dir)));  // eod
  ASSERT_EQ(NULL, (ent = readdir(dir)));  // eod
  ASSERT_SYS(0, 0, closedir(dir));
}

TEST(dirstream, seeky) {
  char name[256];
  char path[512];
  struct stat golden;
  int i, j, n = 1000;
  int goodindex = 500;
  struct critbit0 tree = {0};
  ASSERT_SYS(0, 0, mkdir("boop", 0755));
  ASSERT_EQ(1, critbit0_insert(&tree, "."));
  ASSERT_EQ(1, critbit0_insert(&tree, ".."));
  for (i = 0; i < n; ++i) {
    for (j = 0; j < 255; ++j) {
      name[j] = '0' + rand() % 10;
    }
    // TODO(jart): why does Windows croak with 255
    name[100] = 0;
    strcpy(path, "boop/");
    strcat(path, name);
    path[255] = 0;
    *FormatInt32(path + 5, i) = '-';
    ASSERT_EQ(1, critbit0_insert(&tree, path + 5));
    ASSERT_SYS(0, 0, touch(path, 0644));
    if (i == goodindex) {
      ASSERT_SYS(0, 0, stat(path, &golden));
    }
  }
  // do a full pass
  {
    ASSERT_NE(NULL, (dir = opendir("boop")));
    long tell = -1;
    long prev = telldir(dir);
    while ((ent = readdir(dir))) {
      if (atoi(ent->d_name) == goodindex) {
        ASSERT_EQ(golden.st_ino, ent->d_ino);
        tell = prev;
      }
      prev = telldir(dir);
      ASSERT_EQ(1, critbit0_delete(&tree, ent->d_name));
    }
    ASSERT_EQ(NULL, tree.root);  // all entries were found
    ASSERT_NE(-1, tell);
    seekdir(dir, tell);
    ASSERT_NE(NULL, (ent = readdir(dir)));
    ASSERT_EQ(goodindex, atoi(ent->d_name));
    ASSERT_EQ(golden.st_ino, ent->d_ino);
    ASSERT_SYS(0, 0, closedir(dir));
  }
  // do a partial pass, and seek midway
  {
    ASSERT_NE(NULL, (dir = opendir("boop")));
    int abort = 700;
    long tell = -1;
    bool foundit = false;
    long prev = telldir(dir);
    while ((ent = readdir(dir))) {
      if (atoi(ent->d_name) == goodindex) {
        ASSERT_EQ(golden.st_ino, ent->d_ino);
        tell = prev;
        foundit = true;
      }
      prev = telldir(dir);
      if (--abort <= 0 && foundit) {
        break;
      }
    }
    ASSERT_NE(-1, tell);
    seekdir(dir, tell);
    ASSERT_NE(NULL, (ent = readdir(dir)));
    ASSERT_EQ(goodindex, atoi(ent->d_name));
    ASSERT_EQ(golden.st_ino, ent->d_ino);
    ASSERT_SYS(0, 0, closedir(dir));
  }
}

TEST(dirstream, dots) {
  bool got_dot = false;
  bool got_dot_dot = false;
  ASSERT_NE(NULL, (dir = opendir(".")));
  while ((ent = readdir(dir))) {
    got_dot |= !strcmp(ent->d_name, ".");
    got_dot_dot |= !strcmp(ent->d_name, "..");
  }
  ASSERT_SYS(0, 0, closedir(dir));
  ASSERT_TRUE(got_dot_dot);
  ASSERT_TRUE(got_dot);
}

TEST(dirstream, inoFile_isConsistentWithStat) {
  struct stat st;
  bool gotsome = false;
  EXPECT_SYS(0, 0, touch("foo", 0644));
  EXPECT_SYS(0, 0, stat("foo", &st));
  ASSERT_NE(NULL, (dir = opendir(".")));
  while ((ent = readdir(dir))) {
    if (!strcmp(ent->d_name, "foo")) {
      ASSERT_EQ(st.st_ino, ent->d_ino);
      gotsome = true;
    }
  }
  ASSERT_SYS(0, 0, closedir(dir));
  ASSERT_TRUE(gotsome);
}

TEST(dirstream_zipos, inoFile_isConsistentWithStat) {
  struct stat st;
  const char *dirpath;
  char filename[PATH_MAX];
  dirpath = "/zip/usr/share/zoneinfo";
  ASSERT_NE(NULL, (dir = opendir(dirpath)));
  while ((ent = readdir(dir))) {
    snprintf(filename, sizeof(filename), "%s/%s", dirpath, ent->d_name);
    EXPECT_SYS(0, 0, stat(filename, &st));
    ASSERT_EQ(st.st_ino, ent->d_ino);
  }
  ASSERT_SYS(0, 0, closedir(dir));
}

static const char *DescribeDt(int dt) {
  static char buf[12];
  switch (dt) {
    case DT_UNKNOWN:
      return "DT_UNKNOWN";
    case DT_FIFO:
      return "DT_FIFO";
    case DT_CHR:
      return "DT_CHR";
    case DT_DIR:
      return "DT_DIR";
    case DT_BLK:
      return "DT_BLK";
    case DT_REG:
      return "DT_REG";
    case DT_LNK:
      return "DT_LNK";
    case DT_SOCK:
      return "DT_SOCK";
    default:
      FormatInt32(buf, dt);
      return buf;
  }
}

static const char *DescribeFtw(int dt) {
  static char buf[12];
  switch (dt) {
    case FTW_F:
      return "FTW_F";
    case FTW_D:
      return "FTW_D";
    case FTW_DNR:
      return "FTW_DNR";
    case FTW_NS:
      return "FTW_NS";
    case FTW_SL:
      return "FTW_SL";
    case FTW_DP:
      return "FTW_DP";
    case FTW_SLN:
      return "FTW_SLN";
    default:
      FormatInt32(buf, dt);
      return buf;
  }
}

char *b;

static int walk(const char *fpath,      //
                const struct stat *st,  //
                int typeflag,           //
                struct FTW *ftwbuf) {   //
  appendf(&b, "%-6s %s\n", DescribeFtw(typeflag), fpath);
  return 0;
}

TEST(dirstream, walk) {
  ASSERT_SYS(0, 0, nftw("/zip", walk, 128, FTW_PHYS | FTW_DEPTH));
  ASSERT_STREQ("FTW_F  /zip/echo.com\n"
               "FTW_F  /zip/libc/testlib/hyperion.txt\n"
               "FTW_F  /zip/libc/testlib/moby.txt\n"
               "FTW_DP /zip/libc/testlib\n"
               "FTW_DP /zip/libc\n"
               "FTW_F  /zip/usr/share/zoneinfo/New_York\n"
               "FTW_DP /zip/usr/share/zoneinfo\n"
               "FTW_DP /zip/usr/share\n"
               "FTW_DP /zip/usr\n"
               "FTW_F  /zip/.cosmo\n"
               "FTW_DP /zip\n",
               b);
  free(b);
  b = 0;
}
