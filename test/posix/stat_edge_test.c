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

int main() {
  struct stat st;
  int fd;

  if (stat("", &st) == 0)
    return 1;

  if (errno != ENOENT)
    return 2;

  if (stat("/dev/null", &st) != 0)
    return 3;

  if (!S_ISCHR(st.st_mode))
    return 4;

  if (stat(".", &st) != 0)
    return 5;

  if (!S_ISDIR(st.st_mode))
    return 6;

  fd = open("/dev/null", O_RDONLY);
  if (fd < 0)
    return 7;

  if (fstat(fd, &st) != 0)
    return 8;

  if (!S_ISCHR(st.st_mode))
    return 9;

  close(fd);

  if (fstat(-1, &st) == 0)
    return 10;

  if (errno != EBADF)
    return 11;

  return 0;
}