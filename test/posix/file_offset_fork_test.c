// Copyright 2024 Justine Alexandra Roberts Tunney
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

#include <signal.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

// test that lseek() is shared across fork()

void on_unexpected_death(int sig) {
  int ws;
  if (wait(&ws) == -1)
    _Exit(33);
  if (!WIFEXITED(ws))
    _Exit(34);
  if (!(WEXITSTATUS(ws) & 255))
    _Exit(35);
  _Exit(WEXITSTATUS(ws));
}

int main() {
  signal(SIGCHLD, on_unexpected_death);

  atomic_int *phase;
  if ((phase = mmap(0, sizeof(atomic_int), PROT_READ | PROT_WRITE,
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED)
    return 2;

  int fd;
  char path[] = "/tmp/file_offset_fork_test.XXXXXX";
  if ((fd = mkstemp(path)) == -1)
    return 3;
  if (lseek(fd, 0, SEEK_CUR) != 0)
    return 4;

  if (write(fd, "0", 1) != 1)
    return 5;
  if (lseek(fd, 0, SEEK_CUR) != 1)
    return 6;

  int pid;
  if ((pid = fork()) == -1)
    return 7;

  if (!pid) {
    if (write(fd, "1", 1) != 1)
      _Exit(8);
    if (lseek(fd, 0, SEEK_CUR) != 2)
      _Exit(9);

    *phase = 1;
    for (;;)
      if (*phase == 2)
        break;

    if (write(fd, "3", 1) != 1)
      _Exit(10);
    if (lseek(fd, 0, SEEK_CUR) != 4)
      _Exit(11);

    *phase = 3;
    for (;;)
      if (*phase == 4)
        break;

    _Exit(0);
  }

  for (;;)
    if (*phase == 1)
      break;

  if (write(fd, "2", 1) != 1)
    return 12;
  if (lseek(fd, 0, SEEK_CUR) != 3)
    return 13;

  *phase = 2;
  for (;;)
    if (*phase == 3)
      break;

  if (write(fd, "4", 1) != 1)
    return 14;
  if (lseek(fd, 0, SEEK_CUR) != 5)
    return 15;

  signal(SIGCHLD, SIG_DFL);
  *phase = 4;

  int ws;
  if (wait(&ws) == -1)
    return 16;
  if (!WIFEXITED(ws))
    return 17;
  if (WEXITSTATUS(ws))
    return WEXITSTATUS(ws);

  char buf[16] = {0};
  if (pread(fd, buf, 15, 0) != 5)
    return 18;
  if (lseek(fd, 0, SEEK_CUR) != 5)
    return 19;

  if (close(fd))
    return 20;

  if (munmap(phase, sizeof(atomic_int)))
    return 21;

  if (unlink(path))
    return 22;

  if (strcmp(buf, "01234"))
    return 23;
}
