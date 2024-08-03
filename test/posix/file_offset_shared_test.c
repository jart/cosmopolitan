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

#include <stdatomic.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

// test that file offset is shared across multiple processes

atomic_int *phase;

int main() {

  if ((phase = mmap(0, sizeof(atomic_int), PROT_READ | PROT_WRITE,
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED)
    return 1;

  int fd;
  char path[] = "/tmp/fd_test.XXXXXX";
  if ((fd = mkstemp(path)) == -1)
    return 2;
  if (lseek(fd, 0, SEEK_CUR) != 0)
    return 22;

  if (write(fd, "0", 1) != 1)
    return 3;
  if (lseek(fd, 0, SEEK_CUR) != 1)
    return 33;

  int pid;
  if ((pid = fork()) == -1)
    return 4;

  if (!pid) {
    if (write(fd, "1", 1) != 1)
      _Exit(100);
    if (lseek(fd, 0, SEEK_CUR) != 2)
      _Exit(101);

    *phase = 1;
    for (;;)
      if (*phase == 2)
        break;

    if (write(fd, "3", 1) != 1)
      _Exit(102);
    if (lseek(fd, 0, SEEK_CUR) != 4)
      _Exit(103);
    *phase = 3;
    _Exit(0);
  }

  for (;;)
    if (*phase == 1)
      break;

  if (write(fd, "2", 1) != 1)
    return 5;
  if (lseek(fd, 0, SEEK_CUR) != 3)
    return 55;

  *phase = 2;
  for (;;)
    if (*phase == 3)
      break;

  if (write(fd, "4", 1) != 1)
    return 6;
  if (lseek(fd, 0, SEEK_CUR) != 5)
    return 66;

  int ws;
  if (wait(&ws) == -1)
    return 7;
  if (!WIFEXITED(ws))
    return 8;
  if (WEXITSTATUS(ws))
    return WEXITSTATUS(ws);

  char buf[16] = {0};
  if (pread(fd, buf, 15, 0) != 5)
    return 12;
  if (lseek(fd, 0, SEEK_CUR) != 5)
    return 77;

  if (close(fd))
    return 13;

  if (unlink(path))
    return 14;

  if (strcmp(buf, "01234"))
    return 15;
}
