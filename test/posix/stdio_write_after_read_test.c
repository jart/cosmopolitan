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
#include <string.h>
#include <unistd.h>

// Tests that a write after a read on an update stream is handled
// consistently with glibc (i.e., it succeeds and does not set ferror).
// POSIX says this shouldn't be done without an intervening seek, but
// doesn't specify a failure mode. Both glibc and cosmo allow this.
int main(void) {
  FILE *fp;
  char testfile[] = "/tmp/stdio_write_after_read_test.XXXXXX";
  int fd;
  char buf[10];

  fd = mkstemp(testfile);
  if (fd < 0)
    exit(1);

  fp = fdopen(fd, "w+");
  if (!fp)
    exit(2);

  if (fputs("test", fp) < 0)
    exit(3);

  rewind(fp);

  if (fread(buf, 1, 4, fp) != 4)
    exit(4);

  // POSIX says we shouldn't do this, but glibc and cosmo both allow it.
  // The write succeeds and the error indicator is not set.
  if (fwrite("test", 1, 4, fp) != 4)
    exit(5);

  if (ferror(fp))
    exit(6);

  fclose(fp);
  unlink(testfile);

  return 0;
}