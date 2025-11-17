// Copyright 2025 Justine Alexandra Roberts Tunney
//
// Permission to use, copy, modify, and/or distribute this software for
// any purpose with or without fee is hereby granted, provided that the
// above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

// test open(), openat(), etc. system calls

void test_dir_write(void) {
  char dir[] = "/tmp/open_test.XXXXXX";
  if (!mkdtemp(dir))
    exit(1);
  if (open(dir, O_WRONLY) != -1)
    exit(2);
  if (errno != EISDIR)
    exit(3);
  if (rmdir(dir))
    exit(8);
}

void test_eloop(void) {
  char dir[] = "/tmp/open_test.XXXXXX";
  if (!mkdtemp(dir))
    exit(10);
  int dirfd;
  if ((dirfd = open(dir, O_RDONLY)) == -1)
    exit(11);
  if (symlinkat("sym", dirfd, "sym"))
    exit(12);
  if (openat(dirfd, "sym", O_RDONLY) != -1)
    exit(13);
  if (errno != ELOOP)
    exit(14);
  if (unlinkat(dirfd, "sym", 0))
    exit(15);
  if (close(dirfd))
    exit(16);
  if (rmdir(dir))
    exit(17);
}

void test_dirfd_not_dir(void) {
  int fd;
  char path[] = "/tmp/open_test.XXXXXX";
  if ((fd = mkstemp(path)) == -1)
    exit(20);
  if (openat(fd, "relpath", O_WRONLY | O_CREAT | O_TRUNC, 0644) != -1)
    exit(21);
  if (errno != ENOTDIR)
    exit(22);
  if (close(fd))
    exit(23);
  if (unlink(path))
    exit(24);
}

const char *bad_pointer = (const char *)77;

void test_efault(void) {
  if (open(0, 0) != -1)
    exit(30);
  if (errno != EFAULT)
    exit(31);
  if (open(bad_pointer, 0) != -1)
    exit(32);
  if (errno != EFAULT)
    exit(33);
}

void test_dirfd__not_dir_abspath__dirfd_ignored(void) {
  int fd;
  char path[] = "/tmp/open_test.XXXXXX";
  if ((fd = mkstemp(path)) == -1)
    exit(40);
  int rootfd;
  if ((rootfd = openat(fd, "/", O_RDONLY)) == -1)
    exit(41);
  if (close(rootfd))
    exit(42);
  if (close(fd))
    exit(43);
  if (unlink(path))
    exit(44);
}

void test_dirfd__not_dir_relpath__raises_enotdir(void) {
  int fd;
  char path[] = "/tmp/open_test.XXXXXX";
  if ((fd = mkstemp(path)) == -1)
    exit(45);
  if (openat(fd, "foo", O_RDONLY) != -1)
    exit(46);
  if (errno != ENOTDIR)
    exit(47);
  if (close(fd))
    exit(48);
  if (unlink(path))
    exit(49);
}

void test_dots_under_dirfd(void) {
  char root[] = "/tmp/open_test.XXXXXX";
  if (!mkdtemp(root))
    exit(50);
  int rootfd;
  if ((rootfd = open(root, O_RDONLY)) == -1)
    exit(51);
  // create this directory structure
  //
  //     a/
  //     a/aa/
  //     b/
  //
  // then statat(open("b"), "../a/aa")
  if (mkdirat(rootfd, "a", 0700))
    exit(42);
  int afd;
  if ((afd = openat(rootfd, "a", O_RDONLY)) == -1)
    exit(53);
  if (mkdirat(afd, "aa", 0700))
    exit(54);
  if (mkdirat(rootfd, "b", 0700))
    exit(55);
  int bfd;
  if ((bfd = openat(rootfd, "b", O_RDONLY)) == -1)
    exit(56);
  int aaafd;
  if ((aaafd = openat(bfd, "../a/aa", O_RDONLY | O_DIRECTORY)) == -1)
    exit(57);
  if (close(aaafd))
    exit(58);
  if (close(bfd))
    exit(59);
  if (unlinkat(afd, "aa", AT_REMOVEDIR))
    exit(60);
  if (close(afd))
    exit(61);
  if (unlinkat(rootfd, "a", AT_REMOVEDIR))
    exit(62);
  if (unlinkat(rootfd, "b", AT_REMOVEDIR))
    exit(63);
  if (close(rootfd))
    exit(64);
  if (rmdir(root))
    exit(65);
}

int main(void) {
  test_dir_write();
  test_eloop();
  test_dirfd_not_dir();
  test_efault();
  test_dirfd__not_dir_abspath__dirfd_ignored();
  test_dirfd__not_dir_relpath__raises_enotdir();
  test_dots_under_dirfd();
}
