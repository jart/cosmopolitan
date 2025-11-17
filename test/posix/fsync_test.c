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
  int fd;
  char testfile[] = "/tmp/fsync_test.XXXXXX";
  const char *data = "test data for fsync";

  fd = mkstemp(testfile);
  if (fd < 0)
    return 1;

  if (write(fd, data, 19) != 19)
    return 2;

  if (fsync(fd) != 0)
    return 3;

  if (write(fd, " more data", 10) != 10)
    return 4;

  if (fdatasync(fd) != 0)
    return 5;

  if (fsync(fd) != 0)
    return 6;

  close(fd);
  unlink(testfile);

  return 0;
}