// Copyright (C) 2009-2025 Free Software Foundation, Inc.
// This file is free software; the Free Software Foundation
// gives unlimited permission to copy and/or distribute it,
// with or without modifications, as long as this notice is preserved.
// This file is offered as-is, without any warranty.

#include <errno.h>
#include <stdlib.h>

extern char **environ;

char entry1[] = "a=1";
char entry2[] = "b=2";
char *env[] = {entry1, entry2, NULL};

int main(void) {
  if (putenv((char *)"a=1"))
    return 1;
  if (putenv(entry2))
    return 2;
  entry2[0] = 'a';
  unsetenv("a");
  if (getenv("a"))
    return 3;
  if (!unsetenv("") || errno != EINVAL)
    return 4;
  entry2[0] = 'b';
  environ = env;
  if (!getenv("a"))
    return 5;
  entry2[0] = 'a';
  unsetenv("a");
  if (getenv("a"))
    return 6;
}
