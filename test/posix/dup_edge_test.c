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

int main() {
  int fd1, fd2, fd3;

  fd1 = open("/dev/null", O_RDONLY);
  if (fd1 < 0)
    return 1;

  fd2 = dup(fd1);
  if (fd2 < 0)
    return 2;

  if (fd2 == fd1)
    return 3;

  fd3 = dup2(fd1, fd2);
  if (fd3 != fd2)
    return 4;

  if (dup(-1) != -1 || errno != EBADF)
    return 5;

  if (dup2(-1, 10) != -1 || errno != EBADF)
    return 6;

  if (dup2(fd1, -1) != -1 || errno != EBADF)
    return 7;

  close(fd1);
  close(fd2);

  return 0;
}
