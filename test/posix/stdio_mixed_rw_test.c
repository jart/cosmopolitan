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
  char testfile[] = "/tmp/stdio_mixed_rw.XXXXXX";
  char buffer[64];
  int fd, c;
  long pos;

  fd = mkstemp(testfile);
  if (fd < 0)
    return 1;
  close(fd);

  fp = fopen(testfile, "w+");
  if (!fp)
    return 2;

  if (fputs("0123456789", fp) == EOF)
    return 3;

  if (fseek(fp, 5, SEEK_SET) != 0)
    return 4;

  c = fgetc(fp);
  if (c != '5')
    return 5;

  pos = ftell(fp);
  if (pos != 6)
    return 6;

  if (fseek(fp, 0, SEEK_CUR) != 0)
    return 7;

  if (fputc('X', fp) == EOF)
    return 8;

  pos = ftell(fp);
  if (pos != 7)
    return 9;

  if (fseek(fp, -2, SEEK_CUR) != 0)
    return 10;

  c = fgetc(fp);
  if (c != '5')
    return 11;

  c = fgetc(fp);
  if (c != 'X')
    return 12;

  rewind(fp);
  if (!fgets(buffer, sizeof(buffer), fp))
    return 13;

  if (strcmp(buffer, "012345X789") != 0)
    return 14;

  if (fseek(fp, 3, SEEK_SET) != 0)
    return 15;

  if (fputs("ABC", fp) == EOF)
    return 16;

  if (fseek(fp, 0, SEEK_SET) != 0)
    return 17;

  if (!fgets(buffer, sizeof(buffer), fp))
    return 18;

  if (strcmp(buffer, "012ABCX789") != 0)
    return 19;

  if (fseek(fp, 0, SEEK_END) != 0)
    return 20;

  pos = ftell(fp);
  if (pos != 10)
    return 21;

  if (fputs("END", fp) == EOF)
    return 22;

  rewind(fp);
  if (!fgets(buffer, sizeof(buffer), fp))
    return 23;

  if (strcmp(buffer, "012ABCX789END") != 0)
    return 24;

  fclose(fp);
  unlink(testfile);

  return 0;
}
