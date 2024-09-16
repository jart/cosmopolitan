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

#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  // ensure most file descriptors are closed
  for (int fildes = 3; fildes < 80; ++fildes)
    close(fildes);

  // create new file descriptor
  if (open("/dev/urandom", O_RDONLY) != 3)
    return 2;

  // copy file descriptor to higher number
  if (fcntl(3, F_DUPFD, 70) != 70)
    return 3;

  // new file descriptor should go for lowest number
  int fd;
  if ((fd = open("/dev/urandom", O_RDONLY)) != 4)
    return 4;

  // move file descriptor to higher number
  if (close(3))
    return 5;

  // new file descriptor should go for lowest number
  if (open("/dev/urandom", O_RDONLY) != 3)
    return 6;
}
