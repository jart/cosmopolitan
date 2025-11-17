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
#include "libc/cosmotime.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/macros.h"
#include "libc/mem/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/pib.h"
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
}

TEST(open, enoent) {
  ASSERT_SYS(ENOENT, -1, open("", O_RDONLY));
  ASSERT_SYS(ENOENT, -1, open("doesnotexist", O_RDONLY));
  ASSERT_SYS(ENOENT, -1, open("o/doesnotexist", O_RDONLY));
}

TEST(open, enotdir) {
  ASSERT_SYS(0, 0, touch("o", 0644));
  ASSERT_SYS(ENOTDIR, -1, open("o/", O_RDONLY));
  if (!IsWindows())  // normalization doesn't do i/o on windows
    ASSERT_SYS(ENOTDIR, -1, open("o/.", O_RDONLY));
  if (!IsWindows())  // normalization doesn't do i/o on windows
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
  size_t n = 5000;
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

TEST(open, testOpenAppend_seekToBeginning_stillWritesToEnd) {
  char buf[16] = {0};
  ASSERT_SYS(0, 0, xbarf("hello.txt", "hell", -1));
  ASSERT_SYS(0, 3, open("hello.txt", O_WRONLY | O_APPEND));
  EXPECT_SYS(EBADF, -1, read(3, buf, 4));      // in O_WRONLY mode
  EXPECT_SYS(EBADF, -1, pread(3, buf, 4, 0));  // in O_WRONLY mode
  EXPECT_SYS(0, 1, lseek(3, 1, SEEK_SET));
  EXPECT_SYS(0, 1, write(3, "a", 1));
  EXPECT_SYS(0, 5, lseek(3, 0, SEEK_END));
  EXPECT_SYS(0, 1, write(3, "b", 1));
  EXPECT_SYS(0, 6, lseek(3, 0, SEEK_END));
  EXPECT_SYS(0, 7, lseek(3, 1, SEEK_CUR));
  EXPECT_SYS(0, 0, lseek(3, 0, SEEK_SET));
  EXPECT_SYS(0, 1, write(3, "c", 1));
  EXPECT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, open("hello.txt", O_RDONLY));
  EXPECT_SYS(0, 7, read(3, buf, 8));
  EXPECT_STREQ("hellabc", buf);
  EXPECT_SYS(0, 0, close(3));
}

TEST(open, testAppend_pread) {
  char buf[16] = {0};
  ASSERT_SYS(0, 0, xbarf("hello.txt", "hell", -1));
  ASSERT_SYS(0, 3, open("hello.txt", O_RDWR | O_APPEND));
  EXPECT_SYS(0, 3, pread(3, buf, 16, 1));
  EXPECT_STREQ("ell", buf);
  EXPECT_SYS(0, 0, close(3));
}

TEST(open, testAppend_pwrite) {
  // "If the O_APPEND flag of the file status flags is set, the file
  //  offset shall be set to the end of the file prior to each write
  //  and no intervening file modification operation shall occur
  //  between changing the file offset and the write operation."
  //                             ──Quoth POSIX.1-2018
  if (IsFreebsd() || IsXnu())
    return;  // broken
  char buf[16] = {0};
  ASSERT_SYS(0, 0, xbarf("hello.txt", "hell", -1));
  ASSERT_SYS(0, 3, open("hello.txt", O_WRONLY | O_APPEND));
  EXPECT_SYS(EBADF, -1, pread(3, buf, 4, 0));  // in O_WRONLY mode
  EXPECT_SYS(0, 1, lseek(3, 1, SEEK_SET));
  EXPECT_SYS(0, 1, write(3, "a", 1));
  EXPECT_SYS(0, 5, lseek(3, 0, SEEK_END));
  EXPECT_SYS(0, 1, write(3, "b", 1));
  EXPECT_SYS(0, 6, lseek(3, 0, SEEK_END));
  EXPECT_SYS(0, 7, lseek(3, 1, SEEK_CUR));
  EXPECT_SYS(0, 0, lseek(3, 0, SEEK_SET));
  EXPECT_SYS(0, 1, pwrite(3, "c", 1, 0));
  EXPECT_SYS(0, 7, lseek(3, 0, SEEK_END));
  EXPECT_SYS(0, 1, write(3, "d", 1));
  EXPECT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, open("hello.txt", O_RDONLY));
  EXPECT_SYS(0, 8, read(3, buf, 8));
  EXPECT_STREQ("hellabcd", buf);
  EXPECT_SYS(0, 0, close(3));
}

TEST(open, appendRwMode_readsStartZero_writesAlwaysEof) {
  char buf[8] = {0};
  ASSERT_SYS(0, 0, xbarf("hello.txt", "hell", -1));
  ASSERT_SYS(0, 3, open("hello.txt", O_RDWR | O_APPEND));
  ASSERT_SYS(0, 2, read(3, buf, 2));
  EXPECT_SYS(0, 2, lseek(3, 0, SEEK_CUR));
  ASSERT_SYS(0, 2, pread(3, buf + 2, 2, 2));
  EXPECT_SYS(0, 2, lseek(3, 0, SEEK_CUR));
  EXPECT_STREQ("hell", buf);
  EXPECT_SYS(0, 2, write(3, "o!", 2));
  EXPECT_SYS(0, 6, pread(3, buf, 8, 0));
  EXPECT_STREQ("hello!", buf);
  EXPECT_SYS(0, 0, close(3));
}

TEST(open, appendRwMode_readsStartZero_writesAlwaysEof2) {
  // "The pwrite() function shall be equivalent to write(), except that
  //  it writes into a given position and does not change the file
  //  offset (regardless of whether O_APPEND is set). The first three
  //  arguments to pwrite() are the same as write() with the addition of
  //  a fourth argument offset for the desired position inside the file.
  //  An attempt to perform a pwrite() on a file that is incapable of
  //  seeking shall result in an error."
  //                             ──Quoth POSIX.1-2018
  if (IsFreebsd() || IsXnu())
    return;  // broken
  char buf[8] = {0};
  ASSERT_SYS(0, 0, xbarf("hello.txt", "hell", -1));
  ASSERT_SYS(0, 3, open("hello.txt", O_RDWR | O_APPEND));
  ASSERT_SYS(0, 2, read(3, buf, 2));
  EXPECT_SYS(0, 2, lseek(3, 0, SEEK_CUR));
  ASSERT_SYS(0, 2, pread(3, buf + 2, 2, 2));
  EXPECT_SYS(0, 2, lseek(3, 0, SEEK_CUR));
  EXPECT_STREQ("hell", buf);
  EXPECT_SYS(0, 1, pwrite(3, "H", 1, 3));
  EXPECT_SYS(0, 2, lseek(3, 0, SEEK_CUR));
  EXPECT_SYS(0, 2, write(3, "o!", 2));
  EXPECT_SYS(0, 7, pread(3, buf, 8, 0));
  EXPECT_STREQ("hellHo!", buf);
  EXPECT_SYS(0, 0, close(3));
}

TEST(open, appendReadOnlyMode_appendIsIgnored) {
  char buf[8] = {0};
  ASSERT_SYS(0, 0, xbarf("hello.txt", "abcd", -1));
  ASSERT_SYS(0, 3, open("hello.txt", O_RDONLY | O_APPEND));
  EXPECT_SYS(EBADF, -1, write(3, "o!", 2));      // due to O_RDONLY
  EXPECT_SYS(EBADF, -1, pwrite(3, "o!", 2, 0));  // due to O_RDONLY
  EXPECT_SYS(0, 1, lseek(3, 1, SEEK_SET));
  ASSERT_SYS(0, 1, read(3, buf, 1));
  EXPECT_STREQ("b", buf);
  ASSERT_SYS(0, 1, read(3, buf + 1, 1));
  EXPECT_SYS(0, 3, lseek(3, 0, SEEK_CUR));
  EXPECT_STREQ("bc", buf);
  ASSERT_SYS(0, 4, pread(3, buf, 4, 0));
  EXPECT_STREQ("abcd", buf);
  EXPECT_SYS(0, 3, lseek(3, 0, SEEK_CUR));
  EXPECT_SYS(0, 0, close(3));
}

TEST(open, appendReadWriteMode_readAlwaysReturnsZero) {
  char buf[8] = {0};
  ASSERT_SYS(0, 0, xbarf("hello.txt", "abcd", -1));
  ASSERT_SYS(0, 3, open("hello.txt", O_RDWR | O_APPEND));
  EXPECT_SYS(0, 1, lseek(3, 1, SEEK_SET));
  ASSERT_SYS(0, 1, read(3, buf, 1));
  EXPECT_STREQ("b", buf);
  EXPECT_SYS(0, 1, lseek(3, 1, SEEK_SET));
  EXPECT_SYS(0, 2, write(3, "o!", 2));  // will go to end
  EXPECT_SYS(0, 6, lseek(3, 0, SEEK_CUR));
  ASSERT_SYS(0, 0, read(3, buf, 1));  // write repositioned pointer
  ASSERT_SYS(0, 0, read(3, buf, 1));
  ASSERT_SYS(0, 6, pread(3, buf, 7, 0));
  EXPECT_SYS(0, 1, lseek(3, 1, SEEK_SET));
  EXPECT_SYS(0, 2, write(3, "o!", 2));  // due to O_RDONLY
  EXPECT_STREQ("abcdo!", buf);
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
  if (!IsWindows())  // normalization doesn't do i/o on windows
    ASSERT_SYS(ENOTDIR, -1, open("fun//house//norm/.", O_RDONLY));
  if (!IsWindows())  // normalization doesn't do i/o on windows
    ASSERT_SYS(ENOTDIR, -1, open("fun//house//norm/./", O_RDONLY));
  ASSERT_SYS(0, 3, open("fun//house//", O_RDONLY | O_DIRECTORY));
  ASSERT_SYS(0, 0, close(3));
}

TEST(open, longNormDot) {
  if (IsWindows())
    return;  // todo: why won't long paths work on windows
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
  if (IsWindows())
    return;  // todo: why won't long paths work on windows
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
  if (1)
    return;  // linux 5.15.122-0-lts creates file and returns error D:
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

TEST(open, nameWithOverlongNul_doesntCreateTruncatedName) {
  if (!SupportsXnu())
    return;
  // XNU is the only one that thought to restrict this. XNU chose
  // EILSEQ which makes the most sense. Linux says it'll raise EINVAL
  // if invalid characters are detected. Not sure yet which characters
  // those are. POSIX says nothing about invalid charaters in open.
  ASSERT_SYS(EILSEQ, -1, touch("hi\300\200there", 0755));
}

int CountFds(void) {
  int i, count;
  for (count = i = 0; i < __get_pib()->fds.n; ++i) {
    if (__get_pib()->fds.p[i].kind) {
      ++count;
    }
  }
  return count;
}

TEST(open, lotsOfFds) {
  if (!IsWindows())
    return;
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

// "If O_CREAT is set and the file did not previously exist, upon
//  successful completion, open() shall mark for update the last data
//  access, last data modification, and last file status change
//  timestamps of the file and the last data modification and last
//  file status change timestamps of the parent directory." -POSIX
TEST(open, creatFile_touchesDirectory) {
  if (1)
    return;  // TODO(jart): explain the rare flakes
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

TEST(open, newlineInPath) {
  ASSERT_SYS(EILSEQ, -1, creat("\n", 0644));
}

TEST(open, controlCharacterInPath) {
  if (!SupportsWindows())
    return;
  for (int i = 1; i < 32; ++i) {
    char buf[2] = {i};
    ASSERT_SYS(EILSEQ, -1, creat(buf, 0644));
  }
}

TEST(open, pathHasBadUtf8) {
  if (!SupportsXnu())
    return;
  ASSERT_SYS(EILSEQ, -1, creat("\300\200", 0644));  // overlong nul
}

TEST(open, pathHasTrailingDotsAndPeriods) {
  if (!SupportsWindows())
    return;
  ASSERT_SYS(EILSEQ, -1, creat("hi.", 0644));
  ASSERT_SYS(EILSEQ, -1, creat("hi ", 0644));
}
