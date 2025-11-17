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
#include "libc/intrin/kprintf.h"

int main() {
  int fd;
  pid_t pid;
  struct flock lock;
  char path[] = "/tmp/fcntl_lock_test.XXXXXX";

  if ((fd = mkstemp(path)) == -1)
    return 1;

  lock.l_type = F_WRLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;

  if (fcntl(fd, F_SETLK, &lock) != 0) {
    kprintf("%m\n");
    return 2;
  }

  pid = fork();
  if (pid == 0) {
    int fd2 = open(path, O_RDWR);
    if (fd2 < 0)
      _exit(10);

    if (fcntl(fd2, F_SETLK, &lock) == 0)
      _exit(11);

    if (errno != EACCES && errno != EAGAIN)
      _exit(12);

    close(fd2);
    _exit(0);
  } else if (pid > 0) {
    int status;
    wait(&status);
    if (WEXITSTATUS(status) != 0)
      return 3;
  } else {
    return 4;
  }

  lock.l_type = F_UNLCK;
  if (fcntl(fd, F_SETLK, &lock) != 0)
    return 5;

  close(fd);
  unlink(path);

  return 0;
}
