// Copyright (C) 2002-2004, 2009-2025 Free Software Foundation, Inc.
// This file is free software; the Free Software Foundation gives
// unlimited permission to copy and/or distribute it, with or without
// modifications, as long as this notice is preserved. This file is
// offered as-is, without any warranty.

#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include "libc/runtime/runtime.h"

int main(void) {
  int result = 0;
  char *fence = NULL;
  const int flags = MAP_ANONYMOUS | MAP_PRIVATE;
  const int fd = -1;
  {
    long int pagesize = sysconf(_SC_PAGESIZE);
    char *two_pages =
        (char *)mmap(NULL, 2 * pagesize, PROT_READ | PROT_WRITE, flags, fd, 0);
    if (two_pages != (char *)(-1) &&
        mprotect(two_pages + pagesize, pagesize, PROT_NONE) == 0)
      fence = two_pages + pagesize;
  }
  if (fence) {
    /* Test against bugs on glibc systems.  */
    if (memchr(fence, 0, 0))
      result |= 1;
    strcpy(fence - 9, "12345678");
    if (memchr(fence - 9, 0, 79) != fence - 1)
      result |= 2;
    if (memchr(fence - 1, 0, 3) != fence - 1)
      result |= 4;
    /* Test against bug on AIX 7.2.  */
    if (memchr(fence - 4, '6', 16) != fence - 4)
      result |= 8;
  }
  /* Test against bug on Android 4.3.  */
  {
    char input[3];
    input[0] = 'a';
    input[1] = 'b';
    input[2] = 'c';
    if (memchr(input, 0x789abc00 | 'b', 3) != input + 1)
      result |= 16;
  }
  return result;
}
