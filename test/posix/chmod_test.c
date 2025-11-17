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

// test chmod(), fchmodat(), etc. system calls

void test_readonlydir_isrecursive(void) {
  if (!getuid())
    return;
  char root[] = "/tmp/chmod_test.XXXXXX";
  if (!mkdtemp(root))
    exit(1);
  if (fchmodat(AT_FDCWD, root, 0500, 0))
    exit(3);
  int rootfd;
  if ((rootfd = open(root, O_RDONLY)) == -1)
    exit(4);
  if (openat(rootfd, "reg", O_WRONLY | O_CREAT | O_TRUNC, 0644) != -1)
    exit(5);
  if (errno != EACCES)
    exit(6);
  if (close(rootfd))
    exit(7);
  if (rmdir(root))
    exit(8);
}

void test_mkdir_readonly(void) {
  if (!getuid())
    return;
  char root[] = "/tmp/chmod_test.XXXXXX";
  if (!mkdtemp(root))
    exit(9);
  int rootfd;
  if ((rootfd = open(root, O_RDONLY)) == -1)
    exit(10);
  if (mkdirat(rootfd, "dir", 0444))
    exit(11);
  if (!faccessat(rootfd, "dir", W_OK, 0))
    exit(12);
  int dirfd;
  if ((dirfd = openat(rootfd, "dir", O_RDONLY)) == -1)
    exit(13);
  if (!mkdirat(dirfd, "subdir", 0644))
    exit(14);
  if (errno != EACCES)
    exit(15);
  if (close(dirfd))
    exit(16);
  if (unlinkat(rootfd, "dir", AT_REMOVEDIR))
    exit(17);
  if (close(rootfd))
    exit(18);
  if (rmdir(root))
    exit(19);
}

void test_eloop(void) {
  char dir[] = "/tmp/chmod_test.XXXXXX";
  if (!mkdtemp(dir))
    exit(20);
  int dirfd;
  if ((dirfd = open(dir, O_RDONLY)) == -1)
    exit(21);
  if (symlinkat("sym", dirfd, "sym"))
    exit(22);
  if (fchmodat(dirfd, "sym", 0644, 0) != -1)
    exit(23);
  if (errno != ELOOP)
    exit(24);
  if (unlinkat(dirfd, "sym", 0))
    exit(25);
  if (close(dirfd))
    exit(26);
  if (rmdir(dir))
    exit(27);
}

const char *bad_pointer = (const char *)77;

void test_efault(void) {
  if (chmod(0, 0) != -1)
    exit(30);
  if (errno != EFAULT)
    exit(31);
  if (chmod(bad_pointer, 0) != -1)
    exit(32);
  if (errno != EFAULT)
    exit(33);
}

int main(void) {
  test_readonlydir_isrecursive();
  test_mkdir_readonly();
  test_eloop();
  test_efault();
}
