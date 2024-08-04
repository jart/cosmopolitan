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

// subprogram for testing that lseek() is shared across execve()

atomic_int *phase;

int main(int argc, char *argv[]) {

  if (argc != 3)
    return 101;

  int fd = atoi(argv[1]);
  int mapfd = atoi(argv[2]);

  if ((phase = mmap(0, sizeof(atomic_int), PROT_READ | PROT_WRITE, MAP_SHARED,
                    mapfd, 0)) == MAP_FAILED)
    return 102;

  if (write(fd, "1", 1) != 1)
    return 103;
  if (lseek(fd, 0, SEEK_CUR) != 2)
    return 104;

  *phase = 1;
  for (;;)
    if (*phase == 2)
      break;

  if (write(fd, "3", 1) != 1)
    return 105;
  if (lseek(fd, 0, SEEK_CUR) != 4)
    return 106;

  *phase = 3;
  for (;;)
    if (*phase == 4)
      break;

  if (munmap(phase, sizeof(atomic_int)))
    return 107;
  if (close(mapfd))
    return 108;
  if (close(fd))
    return 109;
}
