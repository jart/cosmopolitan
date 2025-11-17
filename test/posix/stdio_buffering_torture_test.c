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
  char testfile[] = "/tmp/stdio_buffering_torture.XXXXXX";
  char buffer[64];
  int fd, c;

  fd = mkstemp(testfile);
  if (fd < 0)
    return 1;
  close(fd);

  fp = fopen(testfile, "w+");
  if (!fp)
    return 2;

  if (setvbuf(fp, NULL, _IOFBF, 32) != 0)
    return 3;

  if (fputs("Hello", fp) == EOF)
    return 4;

  if (fflush(fp) != 0)
    return 5;

  rewind(fp);
  c = fgetc(fp);
  if (c != 'H')
    return 6;

  if (fseek(fp, 0, SEEK_END) != 0)
    return 7;

  if (fputs(" World", fp) == EOF)
    return 8;

  if (fseek(fp, 0, SEEK_SET) != 0)
    return 9;

  if (!fgets(buffer, sizeof(buffer), fp))
    return 10;

  if (strcmp(buffer, "Hello World") != 0)
    return 11;

  fclose(fp);

  fp = fopen(testfile, "w+");
  if (!fp)
    return 12;

  if (setvbuf(fp, NULL, _IONBF, 0) != 0)
    return 13;

  if (fputs("Unbuffered", fp) == EOF)
    return 14;

  rewind(fp);
  if (!fgets(buffer, sizeof(buffer), fp))
    return 15;

  if (strcmp(buffer, "Unbuffered") != 0)
    return 16;

  fclose(fp);

  fp = fopen(testfile, "w+");
  if (!fp)
    return 17;

  if (setvbuf(fp, NULL, _IOLBF, 64) != 0)
    return 18;

  if (fputs("Line1", fp) == EOF)
    return 19;

  if (fputs("\nLine2\n", fp) == EOF)
    return 20;

  rewind(fp);
  if (!fgets(buffer, sizeof(buffer), fp))
    return 21;

  if (strcmp(buffer, "Line1\n") != 0)
    return 22;

  fclose(fp);
  unlink(testfile);

  return 0;
}