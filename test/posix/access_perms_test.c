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

  if (access("/dev/null", F_OK) != 0)
    return 1;

  if (access("/dev/null", R_OK) != 0)
    return 2;

  if (access("/dev/null", W_OK) != 0)
    return 3;

  if (access("/nonexistent/path/that/should/not/exist", F_OK) == 0)
    return 4;

  if (errno != ENOENT)
    return 5;

  if (access("", F_OK) == 0)
    return 6;

  if (errno != ENOENT)
    return 7;

  int fd;
  char path[] = "/tmp/access_perms_test.XXXXXX";
  if ((fd = mkstemp(path)) == -1)
    return 8;
  close(fd);

  if (access(path, F_OK) != 0)
    return 9;

  if (access(path, R_OK) != 0)
    return 10;

  if (access(path, W_OK) != 0)
    return 11;

  unlink(path);

  return 0;
}
