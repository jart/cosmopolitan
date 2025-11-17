// Copyright (C) 2009-2025 Free Software Foundation, Inc.
// This file is free software; the Free Software Foundation
// gives unlimited permission to copy and/or distribute it,
// with or without modifications, as long as this notice is preserved.
// This file is offered as-is, without any warranty.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {

  int fd;
  char path[] = "/tmp/ungetc_test.XXXXXX";
  if ((fd = mkstemp(path)) == -1)
    return 15;
  close(fd);

  FILE *f;
  if (!(f = fopen(path, "w+")))
    return 1;
  if (fputs("abc", f) < 0)
    return 2;
  rewind(f);
  if (fgetc(f) != 'a')
    return 3;
  if (fgetc(f) != 'b')
    return 4;
  if (ungetc('d', f) != 'd')
    return 5;
  if (ftell(f) != 1)
    return 6;
  if (fgetc(f) != 'd')
    return 7;
  if (ftell(f) != 2)
    return 8;
  if (fseek(f, 0, SEEK_CUR) != 0)
    return 9;
  if (ftell(f) != 2)
    return 10;
  if (fgetc(f) != 'c')
    return 11;
  fclose(f);
  remove(path);
}
