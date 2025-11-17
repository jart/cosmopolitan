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
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

int main(void) {

  int fd;
  char path[] = "/tmp/unlink_before_close_test.XXXXXX";
  if ((fd = mkstemp(path)) == -1)
    return 1;

  // TODO(jart): mmap(fd) makes unlink() fail on windows
  if (unlink(path))
    return 2;

  struct stat st;
  if (!stat(path, &st))
    return 4;
  if (errno != ENOENT)
    return 5;

  if (close(fd))
    return 3;
}
