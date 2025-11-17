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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
  FILE *fp;
  char testfile[] = "/tmp/stdio_fputc_readonly_test.XXXXXX";
  int fd;

  fd = mkstemp(testfile);
  if (fd < 0)
    exit(1);
  close(fd);

  fp = fopen(testfile, "r");
  if (!fp)
    exit(2);

  if (fputc('X', fp) != EOF)
    exit(3);

  if (errno != EBADF)
    exit(4);

  if (!ferror(fp))
    exit(5);

  fclose(fp);
  unlink(testfile);

  return 0;
}
