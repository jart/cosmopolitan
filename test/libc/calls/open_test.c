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
#include "libc/calls/internal.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/macros.internal.h"
#include "libc/mem/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/s.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "libc/x/xasprintf.h"

#define abs(rel) gc(xasprintf("%s/%s", gc(getcwd(0, 0)), rel))

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
  ASSERT_SYS(0, 0, pledge("stdio rpath wpath cpath fattr proc id", 0));
}

TEST(open, efault) {
  ASSERT_SYS(EFAULT, -1, open(0, O_RDONLY));
  if (IsWindows() || !IsAsan()) return;  // not possible
  ASSERT_SYS(EFAULT, -1, open((void *)77, O_RDONLY));
}

TEST(open, enoent) {
  ASSERT_SYS(ENOENT, -1, open("", O_RDONLY));
  ASSERT_SYS(ENOENT, -1, open("doesnotexist", O_RDONLY));
  ASSERT_SYS(ENOENT, -1, open("o/doesnotexist", O_RDONLY));
}

TEST(open, enotdir) {
  ASSERT_SYS(0, 0, touch("o", 0644));
  ASSERT_SYS(ENOTDIR, -1, open("o/", O_RDONLY));
  ASSERT_SYS(ENOTDIR, -1, open("o/.", O_RDONLY));
  ASSERT_SYS(ENOTDIR, -1, open("o/./", O_RDONLY));
  ASSERT_SYS(ENOTDIR, -1, open("o/doesnotexist", O_RDONLY));
}

TEST(open, eexist) {
  ASSERT_SYS(0, 0, touch("exists", 0644));
  ASSERT_SYS(EEXIST, -1, open("exists", O_WRONLY | O_CREAT | O_EXCL, 0644));
}

TEST(open, doubleSlash_worksAndGetsNormalizedOnWindows) {
  ASSERT_SYS(0, 0, mkdir("o", 0755));
  ASSERT_SYS(0, 3,
             open(gc(xjoinpaths(gc(getcwd(0, 0)), "o//deleteme")),
                  O_WRONLY | O_CREAT | O_TRUNC, 0644));
  ASSERT_SYS(0, 0, close(3));
}

TEST(open, fdWillBeInheritedByExecutedPrograms) {
  ASSERT_SYS(0, 3, open("x", O_RDWR | O_CREAT | O_TRUNC, 0666));
  ASSERT_SYS(0, 0, fcntl(3, F_GETFD));
  ASSERT_SYS(0, 0, close(3));
}

TEST(open, fdWillBeClosedByExecveAutomatically) {
  ASSERT_SYS(0, 3, open("x", O_RDWR | O_CREAT | O_TRUNC | O_CLOEXEC, 0666));
  ASSERT_SYS(0, FD_CLOEXEC, fcntl(3, F_GETFD));
  ASSERT_SYS(0, 0, close(3));
}

TEST(open, enametoolong) {
  size_t n = 260;
  char *s = gc(xcalloc(1, n + 1));
  memset(s, 'J', n);
  ASSERT_SYS(ENAMETOOLONG, -1, creat(s, 0644));
}

TEST(open, testSpaceInFilename) {
  char buf[8] = {0};
  ASSERT_SYS(0, 0, xbarf("hello txt", "hello", -1));
  ASSERT_SYS(0, 3, open("hello txt", O_WRONLY));
  EXPECT_SYS(0, 1, write(3, "H", 1));
  EXPECT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, open("hello txt", O_RDONLY));
  EXPECT_SYS(0, 5, read(3, buf, 7));
  EXPECT_STREQ("Hello", buf);
  EXPECT_SYS(0, 0, close(3));
}

TEST(open, testOpenExistingForWriteOnly_seeksToStart) {
  char buf[8] = {0};
  ASSERT_SYS(0, 0, xbarf("hello.txt", "hello", -1));
  ASSERT_SYS(0, 3, open("hello.txt", O_WRONLY));
  EXPECT_SYS(0, 1, write(3, "H", 1));
  EXPECT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, open("hello.txt", O_RDONLY));
  EXPECT_SYS(0, 5, read(3, buf, 7));
  EXPECT_STREQ("Hello", buf);
  EXPECT_SYS(0, 0, close(3));
}

TEST(open, testOpenExistingForReadWrite_seeksToStart) {
  char buf[8] = {0};
  ASSERT_SYS(0, 0, xbarf("hello.txt", "hello", -1));
  ASSERT_SYS(0, 3, open("hello.txt", O_RDWR));
  EXPECT_SYS(0, 1, write(3, "H", 1));
  EXPECT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, open("hello.txt", O_RDONLY));
  EXPECT_SYS(0, 5, read(3, buf, 7));
  EXPECT_STREQ("Hello", buf);
  EXPECT_SYS(0, 0, close(3));
}

TEST(open, testOpenExistingForAppendWriteOnly_seeksToEnd) {
  char buf[16] = {0};
  ASSERT_SYS(0, 0, xbarf("hello.txt", "hell", -1));
  ASSERT_SYS(0, 3, open("hello.txt", O_WRONLY | O_APPEND));
  EXPECT_SYS(EBADF, -1, pread(3, buf, 4, 0));  // in O_WRONLY mode
  EXPECT_SYS(0, 1, write(3, "o", 1));
  EXPECT_SYS(0, 0, lseek(3, 0, SEEK_SET));
  EXPECT_SYS(0, 1, write(3, "!", 1));
  EXPECT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, open("hello.txt", O_RDONLY));
  EXPECT_SYS(0, 6, read(3, buf, 8));
  EXPECT_STREQ("hello!", buf);
  EXPECT_SYS(0, 0, close(3));
}

TEST(open, appendRwMode_readsStartZero_writesAlwaysEof) {
  char buf[8] = {0};
  ASSERT_SYS(0, 0, xbarf("hello.txt", "hell", -1));
  ASSERT_SYS(0, 3, open("hello.txt", O_RDWR | O_APPEND));
  ASSERT_SYS(0, 2, read(3, buf, 2));
  ASSERT_SYS(0, 2, read(3, buf + 2, 2));
  EXPECT_STREQ("hell", buf);
  EXPECT_SYS(0, 2, write(3, "o!", 2));
  ASSERT_SYS(0, 6, pread(3, buf, 8, 0));
  EXPECT_STREQ("hello!", buf);
  EXPECT_SYS(0, 0, close(3));
}

TEST(open, appendReadOnlyMode_appendIsIgnored) {
  char buf[8] = {0};
  ASSERT_SYS(0, 0, xbarf("hello.txt", "hell", -1));
  ASSERT_SYS(0, 3, open("hello.txt", O_RDONLY | O_APPEND));
  EXPECT_SYS(EBADF, -1, write(3, "o!", 2));  // due to O_RDONLY
  ASSERT_EQ(0, errno);
  ASSERT_SYS(0, 2, read(3, buf, 2));
  ASSERT_SYS(0, 2, read(3, buf + 2, 2));
  EXPECT_STREQ("hell", buf);
  ASSERT_SYS(0, 4, pread(3, buf, 4, 0));
  EXPECT_SYS(0, 0, close(3));
}

TEST(open, truncReadWriteMode_getsTruncated) {
  char buf[8] = {0};
  ASSERT_FALSE(fileexists("hello.txt"));
  ASSERT_SYS(ENOENT, -1, open("hello.txt", O_RDWR | O_TRUNC));
  ASSERT_SYS(ENOENT, -1, open("hello.txt/there", O_RDWR | O_TRUNC));
  ASSERT_SYS(0, 0, xbarf("hello.txt", "hell", -1));
  ASSERT_SYS(ENOTDIR, -1, open("hello.txt/there", O_RDWR | O_TRUNC));
  ASSERT_SYS(0, 3, open("hello.txt", O_RDWR | O_TRUNC));
  ASSERT_SYS(0, 0, read(3, buf, 8));
  EXPECT_STREQ("", buf);
  ASSERT_SYS(0, 8, write(3, buf, 8));
  EXPECT_SYS(0, 0, close(3));
}

TEST(open, truncReadOnlyMode_wontTruncate) {
  ASSERT_SYS(EINVAL, -1, open("hello.txt", O_RDONLY | O_TRUNC));
}

TEST(open, testRelativePath_opensRelativeToDirFd) {
  ASSERT_SYS(0, 0, mkdir("foo", 0755));
  ASSERT_SYS(0, 3, open("foo", O_RDONLY | O_DIRECTORY));
  EXPECT_SYS(0, 4, openat(3, "bar", O_WRONLY | O_TRUNC | O_CREAT, 0755));
  EXPECT_TRUE(fileexists("foo/bar"));
  EXPECT_SYS(0, 0, close(4));
  EXPECT_SYS(0, 0, close(3));
}

TEST(open, eloop) {
  ASSERT_SYS(0, 0, symlink("froot", "link"));
  ASSERT_TRUE(issymlink("link"));
  ASSERT_SYS(ELOOP, -1, open("link", O_RDONLY | O_NOFOLLOW));
}

TEST(open, norm) {
  ASSERT_SYS(0, 0, mkdir("fun", 0755));
  ASSERT_SYS(0, 0, mkdir("fun/house", 0755));
  ASSERT_SYS(0, 0, touch("fun/house/norm", 0644));
  ASSERT_SYS(0, 3, open("fun//house//norm", O_RDONLY));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, open(abs("fun//house//norm"), O_RDONLY));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, open("fun//house/./norm", O_RDONLY));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, open(abs("fun//house/./norm"), O_RDONLY));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, open("fun//house/../house/norm", O_RDONLY));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, open(abs("fun//house/../house/norm"), O_RDONLY));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(ENOTDIR, -1, open("fun//house//norm/", O_RDONLY));
  ASSERT_SYS(ENOTDIR, -1, open("fun//house//norm/.", O_RDONLY));
  ASSERT_SYS(ENOTDIR, -1, open("fun//house//norm/./", O_RDONLY));
  ASSERT_SYS(0, 3, open("fun//house//", O_RDONLY | O_DIRECTORY));
  ASSERT_SYS(0, 0, close(3));
}

TEST(open, longNormDot) {
  if (IsWindows()) return;  // todo: why won't long paths work on windows
#define NAME                                                                   \
  "funfunfunfunfunfunfunfunfunfunfunfunfunfunfunfunfunfunfunfunfunfunfunfunfu" \
  "nfunfunfunfunfunfunnfunfunfunfunfunfunnfunfunfunfunfunfununfunfunfunfunfun"
  ASSERT_SYS(0, 0, mkdir(NAME, 0755));
  ASSERT_SYS(0, 0, mkdir(abs(NAME "/" NAME), 0755));
  ASSERT_SYS(0, 3, creat(abs(NAME "//" NAME "/./norm"), 0644));
  ASSERT_TRUE(fileexists(abs(NAME "//" NAME "/norm")));
  ASSERT_SYS(0, 0, close(3));
}

TEST(open, longNormDotDot) {
  if (IsWindows()) return;  // todo: why won't long paths work on windows
#define NAME                                                                   \
  "funfunfunfunfunfunfunfunfunfunfunfunfunfunfunfunfunfunfunfunfunfunfunfunfu" \
  "nfunfunfunfunfunfunnfunfunfunfunfunfunnfunfunfunfunfunfununfunfunfunfunfun"
  ASSERT_SYS(0, 0, mkdir(NAME, 0755));
  ASSERT_SYS(0, 0, mkdir(abs(NAME "/" NAME), 0755));
  ASSERT_SYS(0, 0, mkdir(abs(NAME "/" NAME "/" NAME), 0755));
  ASSERT_SYS(0, 3, creat(abs(NAME "//" NAME "//" NAME "/../norm"), 0644));
  ASSERT_TRUE(fileexists(abs(NAME "//" NAME "/norm")));
  ASSERT_SYS(0, 0, close(3));
}

TEST(open, creat_directory) {
  ASSERT_SYS(ENOENT, -1, open("fun", O_WRONLY | O_DIRECTORY));
  ASSERT_FALSE(fileexists("fun"));
  if (1) return;  // linux 5.15.122-0-lts creates file and returns error D:
  ASSERT_SYS(ENOTDIR, -1, open("fun", O_CREAT | O_WRONLY | O_DIRECTORY, 0644));
  ASSERT_TRUE(fileexists("fun"));
}

TEST(open, O_DIRECTORY_preventsOpeningRegularFiles) {
  ASSERT_SYS(0, 0, touch("file", 0644));
  ASSERT_SYS(ENOTDIR, -1, open("file", O_WRONLY | O_DIRECTORY));
}

TEST(open, O_DIRECTORY_isNotARequirementToOpenDirectory) {
  ASSERT_SYS(0, 0, mkdir("dir", 0755));
  ASSERT_SYS(0, 3, open("dir", O_RDONLY));
  ASSERT_SYS(0, 0, close(3));
}

TEST(open, openExistingDirectoryForWriting_raisesError) {
  ASSERT_SYS(0, 0, mkdir("dir", 0755));
  ASSERT_SYS(EISDIR, -1, open("dir", O_WRONLY));
}

TEST(open, nameWithControlCode) {
  if (IsWindows()) {
    ASSERT_SYS(EINVAL, -1, touch("hi\1there", 0755));
  } else {
    ASSERT_SYS(0, 0, touch("hi\1there", 0755));
  }
}

TEST(open, nameWithOverlongNul_doesntCreateTruncatedName) {
  if (IsXnu() || IsWindows()) {
    // XNU is the only one that thought to restrict this. XNU chose
    // EILSEQ which makes the most sense. Linux says it'll raise EINVAL
    // if invalid characters are detected. Not sure yet which characters
    // those are. POSIX says nothing about invalid charaters in open.
    ASSERT_SYS(EILSEQ, -1, touch("hi\300\200there", 0755));
  } else {
    ASSERT_SYS(0, 0, touch("hi\300\200there", 0755));
    ASSERT_TRUE(fileexists("hi\300\200there"));
    ASSERT_FALSE(fileexists("hi"));
  }
}

int CountFds(void) {
  int i, count;
  for (count = i = 0; i < g_fds.n; ++i) {
    if (g_fds.p[i].kind) {
      ++count;
    }
  }
  return count;
}

TEST(open, lotsOfFds) {
  if (!IsWindows()) return;
  int i, n = 200;
  ASSERT_SYS(0, 0, xbarf("hello.txt", "hello", -1));
  for (i = 3; i < n; ++i) {
    EXPECT_EQ(i, CountFds());
    EXPECT_SYS(0, i, open("hello.txt", O_RDONLY));
  }
  for (i = 3; i < n; ++i) {
    EXPECT_SYS(0, 0, close(i));
  }
}

static int64_t GetInode(const char *path) {
  struct stat st;
  ASSERT_SYS(0, 0, stat(path, &st));
  return st.st_ino;
}

TEST(open, drive) {
  if (!IsWindows()) return;
  ASSERT_NE(GetInode("/"), GetInode("."));
  ASSERT_EQ(GetInode("/"), GetInode("/c"));  // sorry you have to run on c:/
  ASSERT_EQ(GetInode("/"), GetInode("/c/"));
  ASSERT_SYS(0, 3, open("/", O_RDONLY));
  ASSERT_SYS(0, 0, close(3));
}

TEST(open, readOnlyCreatMode) {
  char buf[8];
  struct stat st;
  ASSERT_SYS(0, 3, open("x", O_RDWR | O_CREAT | O_TRUNC, 0500));
  ASSERT_SYS(0, 2, pwrite(3, "MZ", 2, 0));
  ASSERT_SYS(0, 2, pread(3, buf, 8, 0));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 0, stat("x", &st));
  ASSERT_EQ(0100500, st.st_mode);
  if (getuid()) {
    ASSERT_SYS(EACCES, -1, open("x", O_RDWR));
    ASSERT_SYS(EACCES, -1, open("x", O_RDWR | O_CREAT));
  } else {
    // root is invulnerable to eacces
    ASSERT_SYS(0, 3, open("x", O_RDWR));
    ASSERT_SYS(0, 0, close(3));
    ASSERT_SYS(0, 3, open("x", O_RDWR | O_CREAT));
    ASSERT_SYS(0, 0, close(3));
    SPAWN(fork);
    setuid(1000);
    setgid(1000);
    ASSERT_SYS(EACCES, -1, open("x", O_RDWR));
    ASSERT_SYS(EACCES, -1, open("x", O_RDWR | O_CREAT));
    EXITS(0);
  }
}

TEST(open, parentSymlink) {
  struct stat st;
  ASSERT_SYS(0, 0, mkdir("parent", 0755));
  // create directory symlink
  ASSERT_SYS(0, 0, symlink("parent", "parent-link"));
  // test the symlink we just made is a symlink
  ASSERT_SYS(0, 0, lstat("parent-link", &st));
  ASSERT_TRUE(S_ISLNK(st.st_mode));
  // create regular file when parent component is symlink dir
  ASSERT_SYS(0, 0, touch("parent-link/regular", 0644));
  // test stat works
  ASSERT_SYS(0, 0, stat("parent-link/regular", &st));
  ASSERT_TRUE(S_ISREG(st.st_mode));
  // test open works
  ASSERT_SYS(0, 3, open("parent-link/regular", O_RDONLY));
  ASSERT_SYS(0, 0, fstat(3, &st));
  ASSERT_TRUE(S_ISREG(st.st_mode));
  ASSERT_SYS(0, 0, close(3));
  // test O_NOFOLLOW doesn't apply to parent components
  ASSERT_SYS(0, 3, open("parent-link/regular", O_RDONLY | O_NOFOLLOW));
  ASSERT_SYS(0, 0, fstat(3, &st));
  ASSERT_TRUE(S_ISREG(st.st_mode));
  ASSERT_SYS(0, 0, close(3));
  // create regular symlink
  ASSERT_SYS(0, 0, symlink("regular", "parent-link/regular-link"));
  // test stat works
  ASSERT_SYS(0, 0, stat("parent-link/regular-link", &st));
  ASSERT_TRUE(S_ISREG(st.st_mode));
  ASSERT_SYS(0, 0, lstat("parent-link/regular-link", &st));
  ASSERT_TRUE(S_ISLNK(st.st_mode));
  // test open works
  ASSERT_SYS(0, 3, open("parent-link/regular-link", O_RDONLY));
  ASSERT_SYS(0, 0, fstat(3, &st));
  ASSERT_TRUE(S_ISREG(st.st_mode));
  ASSERT_SYS(0, 0, close(3));
  // test O_NOFOLLOW applies to last component
  ASSERT_SYS(ELOOP, -1,
             open("parent-link/regular-link", O_RDONLY | O_NOFOLLOW));
}

TEST(open, readonlyCreateMode_dontChangeStatusIfExists) {
  char buf[8];
  struct stat st;
  ASSERT_SYS(0, 3, creat("wut", 0700));
  ASSERT_SYS(0, 2, pwrite(3, "MZ", 2, 0));
  ASSERT_SYS(0, 0, close(3));
  // since the file already exists, unix doesn't change read-only
  ASSERT_SYS(0, 3, open("wut", O_CREAT | O_TRUNC | O_RDWR, 0500));
  ASSERT_SYS(0, 0, pread(3, buf, 8, 0));
  ASSERT_SYS(0, 0, fstat(3, &st));
  ASSERT_EQ(0100600, st.st_mode & 0700666);
  ASSERT_SYS(0, 0, close(3));
}

TEST(open, creatRdonly) {
  char buf[8];
  ASSERT_SYS(EINVAL, -1, open("foo", O_CREAT | O_TRUNC | O_RDONLY, 0700));
  ASSERT_SYS(0, 3, open("foo", O_CREAT | O_RDONLY, 0700));
  ASSERT_SYS(EBADF, -1, pwrite(3, "MZ", 2, 0));
  ASSERT_SYS(0, 0, pread(3, buf, 8, 0));
  ASSERT_SYS(0, 0, close(3));
}

TEST(open, sequentialRandom_EINVAL) {
  if (!IsWindows()) return;
  ASSERT_SYS(
      EINVAL, -1,
      open("foo", O_CREAT | O_TRUNC | O_RDWR | O_SEQUENTIAL | O_RANDOM, 0700));
}

// "If O_CREAT is set and the file did not previously exist, upon
//  successful completion, open() shall mark for update the last data
//  access, last data modification, and last file status change
//  timestamps of the file and the last data modification and last
//  file status change timestamps of the parent directory." -POSIX
TEST(open, creatFile_touchesDirectory) {
  if (1) return;  // TODO(jart): explain the rare flakes
  struct stat st;
  struct timespec birth;
  ASSERT_SYS(0, 0, mkdir("dir", 0755));
  ASSERT_SYS(0, 0, stat("dir", &st));
  birth = st.st_ctim;
  // check we can read time without changing it
  sleep(2);
  ASSERT_SYS(0, 0, stat("dir", &st));
  EXPECT_EQ(0, timespec_cmp(st.st_ctim, birth));
  EXPECT_EQ(0, timespec_cmp(st.st_mtim, birth));
  EXPECT_EQ(0, timespec_cmp(st.st_atim, birth));
  // check that the directory time changes when file is made
  sleep(2);
  ASSERT_SYS(0, 0, touch("dir/file", 0644));
  ASSERT_SYS(0, 0, stat("dir", &st));
  EXPECT_EQ(1, timespec_cmp(st.st_ctim, birth));
  EXPECT_EQ(1, timespec_cmp(st.st_mtim, birth));
  // TODO: Maybe statfs() for noatime / relative?
  // EXPECT_EQ(1, timespec_cmp(st.st_atim, birth));
}

// "If O_TRUNC is set and the file did previously exist, upon successful
//  completion, open() shall mark for update the last data modification
//  and last file status change timestamps of the file." -POSIX
TEST(open, trunc_touchesMtimCtim) {
  struct stat st;
  struct timespec birth;
  ASSERT_SYS(0, 0, touch("regular", 0755));
  ASSERT_SYS(0, 0, stat("regular", &st));
  birth = st.st_ctim;
  sleep(2);
  ASSERT_SYS(0, 3, open("regular", O_RDWR | O_TRUNC));
  ASSERT_SYS(0, 0, fstat(3, &st));
  EXPECT_EQ(1, timespec_cmp(st.st_ctim, birth));
  EXPECT_EQ(1, timespec_cmp(st.st_mtim, birth));
  ASSERT_SYS(0, 0, close(3));
}

TEST(open, mereOpen_doesntTouch) {
  struct stat st;
  struct timespec birth;
  ASSERT_SYS(0, 0, touch("regular", 0755));
  ASSERT_SYS(0, 0, stat("regular", &st));
  birth = st.st_ctim;
  sleep(2);
  ASSERT_SYS(0, 3, open("regular", O_RDWR));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 0, stat("regular", &st));
  EXPECT_EQ(0, timespec_cmp(st.st_ctim, birth));
  EXPECT_EQ(0, timespec_cmp(st.st_mtim, birth));
  EXPECT_EQ(0, timespec_cmp(st.st_atim, birth));
}

TEST(open, canTruncateExistingFile) {
  struct stat st;
  ASSERT_SYS(0, 0, xbarf("foo", "hello", -1));
  ASSERT_SYS(0, 0, stat("foo", &st));
  ASSERT_EQ(5, st.st_size);
  ASSERT_SYS(0, 3, open("foo", O_RDWR | O_TRUNC));
  ASSERT_SYS(0, 0, fstat(3, &st));
  ASSERT_EQ(0, st.st_size);
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 0, stat("foo", &st));
  ASSERT_EQ(0, st.st_size);
}
