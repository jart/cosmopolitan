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
  char testfile[] = "/tmp/stdio_seek_tell_test.XXXXXX";
  long pos;
  char buffer[32];
  int fd;

  fd = mkstemp(testfile);
  if (fd < 0)
    return 1;

  close(fd);

  fp = fopen(testfile, "w+");
  if (!fp)
    return 2;

  if (fprintf(fp, "Hello World") != 11)
    return 3;

  pos = ftell(fp);
  if (pos != 11)
    return 4;

  if (fseek(fp, 0, SEEK_SET) != 0)
    return 5;

  pos = ftell(fp);
  if (pos != 0)
    return 6;

  if (fread(buffer, 1, 5, fp) != 5)
    return 7;

  pos = ftell(fp);
  if (pos != 5)
    return 8;

  if (fseek(fp, 6, SEEK_SET) != 0)
    return 9;

  pos = ftell(fp);
  if (pos != 6)
    return 10;

  if (fread(buffer, 1, 5, fp) != 5)
    return 11;

  buffer[5] = '\0';
  if (strcmp(buffer, "World") != 0)
    return 12;

  if (fseek(fp, 0, SEEK_END) != 0)
    return 13;

  pos = ftell(fp);
  if (pos != 11)
    return 14;

  if (fseek(fp, -5, SEEK_CUR) != 0)
    return 15;

  pos = ftell(fp);
  if (pos != 6)
    return 16;

  fclose(fp);
  unlink(testfile);

  return 0;
}
