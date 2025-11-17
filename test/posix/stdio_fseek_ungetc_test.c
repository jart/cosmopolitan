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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Tests that fseek() discards the ungetc() buffer, as specified by POSIX.
int main(void) {
  FILE *fp;
  char testfile[] = "/tmp/stdio_fseek_ungetc_test.XXXXXX";
  int fd;

  fd = mkstemp(testfile);
  if (fd < 0)
    exit(1);

  fp = fdopen(fd, "w+");
  if (!fp)
    exit(2);

  if (fputs("0123456789", fp) == EOF)
    exit(3);

  rewind(fp);

  if (fgetc(fp) != '0')
    exit(4);

  // Push 'X' onto the stream. The next read should be 'X'.
  if (ungetc('X', fp) != 'X')
    exit(5);

  // However, a seek should discard the pushed-back character.
  if (fseek(fp, 0, SEEK_SET) != 0)
    exit(6);

  // So the next character read should be the original '0', not 'X'.
  if (fgetc(fp) != '0')
    exit(7);

  fclose(fp);
  unlink(testfile);

  return 0;
}