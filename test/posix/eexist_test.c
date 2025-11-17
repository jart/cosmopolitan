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

// test open(O_CREAT|O_EXCL) raises EEXIST when file exists

void test_regular(void) {
  int fd;
  char path[] = "/tmp/eexist_test.XXXXXX";
  if (!(fd = mkstemp(path)))
    exit(1);
  if (close(fd))
    exit(2);
  if (open(path, O_CREAT | O_EXCL, 0644) != -1)
    exit(3);
  if (errno != EEXIST)
    exit(4);
  if (unlink(path))
    exit(5);
}

void test_directory(void) {
  char path[] = "/tmp/eexist_test.XXXXXX";
  if (!mkdtemp(path))
    exit(6);
  if (open(path, O_CREAT | O_EXCL, 0644) != -1)
    exit(7);
  if (errno != EEXIST)
    exit(8);
  if (rmdir(path))
    exit(9);
}

int main(void) {
  test_regular();
  test_directory();
}
