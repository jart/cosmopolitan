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
#include <sys/wait.h>
#include <unistd.h>

int main() {
  int fd1, fd2;
  struct flock lock1, lock2;
  pid_t pid;
  char testfile[] = "/tmp/fcntl_advisory_test.XXXXXX";

  fd1 = mkstemp(testfile);
  if (fd1 < 0)
    return 1;

  if (write(fd1, "test data for locking", 21) != 21)
    return 2;

  lock1.l_type = F_WRLCK;
  lock1.l_whence = SEEK_SET;
  lock1.l_start = 0;
  lock1.l_len = 10;

  if (fcntl(fd1, F_SETLK, &lock1) != 0)
    return 3;

  pid = fork();
  if (pid == 0) {
    fd2 = open(testfile, O_RDWR);
    if (fd2 < 0)
      return 4;

    lock2.l_type = F_WRLCK;
    lock2.l_whence = SEEK_SET;
    lock2.l_start = 0;
    lock2.l_len = 5;

    if (fcntl(fd2, F_SETLK, &lock2) == 0)
      return 5;

    if (errno != EACCES && errno != EAGAIN)
      return 6;

    lock2.l_type = F_WRLCK;
    lock2.l_whence = SEEK_SET;
    lock2.l_start = 15;
    lock2.l_len = 5;

    if (fcntl(fd2, F_SETLK, &lock2) != 0)
      return 7;

    lock2.l_type = F_UNLCK;
    lock2.l_whence = SEEK_SET;
    lock2.l_start = 15;
    lock2.l_len = 5;

    if (fcntl(fd2, F_SETLK, &lock2) != 0)
      return 8;

    close(fd2);
    return 0;
  } else if (pid > 0) {
    int status;
    wait(&status);

    if (WEXITSTATUS(status) != 0)
      return 9;

    lock1.l_type = F_UNLCK;
    lock1.l_whence = SEEK_SET;
    lock1.l_start = 0;
    lock1.l_len = 10;

    if (fcntl(fd1, F_SETLK, &lock1) != 0)
      return 10;

    close(fd1);
    unlink(testfile);
  } else {
    return 11;
  }

  return 0;
}