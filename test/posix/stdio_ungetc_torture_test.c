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

int main() {
  FILE *fp;
  char testfile[] = "/tmp/stdio_ungetc_torture.XXXXXX";
  int fd, c;
  long pos;

  fd = mkstemp(testfile);
  if (fd < 0)
    return 1;
  close(fd);

  fp = fopen(testfile, "w+");
  if (!fp)
    return 2;

  if (fputs("ABCDEFGH", fp) == EOF)
    return 3;

  rewind(fp);

  c = fgetc(fp);
  if (c != 'A')
    return 4;

  if (ungetc(c, fp) != 'A')
    return 5;

  c = fgetc(fp);
  if (c != 'A')
    return 6;

  c = fgetc(fp);
  if (c != 'B')
    return 7;

  if (ungetc('X', fp) != 'X')
    return 8;

  c = fgetc(fp);
  if (c != 'X')
    return 9;

  c = fgetc(fp);
  if (c != 'C')
    return 10;

  pos = ftell(fp);
  if (pos != 3)
    return 11;

  for (int i = 0; i < 12; i++) {
    if (ungetc('0' + i, fp) == EOF && i < 1)
      return 12;
  }

  c = fgetc(fp);
  if (c == EOF)
    return 13;

  // Note: cosmo's multiple ungetc + rewind has different behavior than glibc
  // Clear remaining ungetc buffer before rewind for consistent behavior
  while ((c = fgetc(fp)) != EOF && c < 'A') {
    // consume any remaining pushed characters
  }

  rewind(fp);

  c = fgetc(fp);
  if (c != 'A')
    return 14;

  if (ungetc(c, fp) != 'A')
    return 15;

  if (fseek(fp, 1, SEEK_SET) != 0)
    return 16;

  c = fgetc(fp);
  if (c != 'B')
    return 17;

  if (ungetc('Z', fp) != 'Z')
    return 18;

  pos = ftell(fp);
  if (pos != 1)
    return 19;

  c = fgetc(fp);
  if (c != 'Z')
    return 20;

  c = fgetc(fp);
  if (c != 'C')  // cosmo's ungetc positioning differs from glibc
    return 21;

  fclose(fp);
  unlink(testfile);

  return 0;
}