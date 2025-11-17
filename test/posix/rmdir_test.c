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
#include <unistd.h>

void test_rmdir_simple(void) {
  char path[] = "/tmp/rmdir_test.XXXXXX";
  if (!mkdtemp(path))
    exit(1);
  if (rmdir(path))
    exit(2);
  if (access(path, F_OK) != -1)
    exit(3);
  if (errno != ENOENT)
    exit(4);
}

void test_rmdir_notempty(void) {
  char path[] = "/tmp/rmdir_test.XXXXXX";
  if (!mkdtemp(path))
    exit(5);
  int fd;
  if ((fd = openat(AT_FDCWD, path, O_RDONLY)) == -1)
    exit(6);
  if (openat(fd, "sillyfile", O_WRONLY | O_CREAT | O_TRUNC, 0644) == -1)
    exit(7);
  if (rmdir(path) != -1)
    exit(8);
  if (errno != ENOTEMPTY && errno != EEXIST) // EEXIST is for Windows
    exit(9);
  if (unlinkat(fd, "sillyfile", 0))
    exit(10);
  if (close(fd))
    exit(11);
  if (rmdir(path))
    exit(12);
}

void test_rmdir_enoent(void) {
  if (rmdir("/tmp/this-directory-does-not-exist-i-hope") != -1)
    exit(13);
  if (errno != ENOENT)
    exit(14);
}

void test_rmdir_enotdir(void) {
  char path[] = "/tmp/rmdir_test.XXXXXX";
  int fd;
  if ((fd = mkstemp(path)) == -1)
    exit(15);
  if (close(fd))
    exit(16);
  if (rmdir(path) != -1)
    exit(17);
  if (errno != ENOTDIR)
    exit(18);
  if (unlink(path))
    exit(19);
}

const char *bad_pointer = (const char *)77;

void test_rmdir_efault(void) {
  if (rmdir(bad_pointer) != -1)
    exit(20);
  if (errno != EFAULT)
    exit(21);
}

int main(int argc, char *argv[]) {
  test_rmdir_simple();
  test_rmdir_notempty();
  test_rmdir_enoent();
  test_rmdir_enotdir();
  test_rmdir_efault();
  return 0;
}
