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
#include <string.h>
#include <unistd.h>
#include "libc/intrin/kprintf.h"

// test chdir(), fchdir(), etc.

void test_relative(void) {
  char root[] = "/tmp/chdir_test.XXXXXX";
  if (!mkdtemp(root))
    exit(1);
  if (chdir(root))
    exit(2);
  if (mkdir("dir", 0700))
    exit(3);
  if (chdir("dir"))
    exit(4);
  if (chdir("/"))  // needed on windows
    exit(4);
  char dir[PATH_MAX];
  strcpy(dir, root);
  strcat(dir, "/dir");
  if (rmdir(dir))
    exit(5);
  if (rmdir(root))
    exit(6);
}

void test_dots_work_against_cwd(void) {
  char root[] = "/tmp/chdir_test.XXXXXX";
  if (!mkdtemp(root))
    exit(7);
  if (chdir(root))
    exit(8);
  if (mkdir("a", 0700))
    exit(9);
  if (mkdir("b", 0700))
    exit(10);
  if (chdir("a"))
    exit(11);
  if (rmdir("../b"))
    exit(12);
  if (chdir(".."))
    exit(13);
  if (rmdir("a"))
    exit(14);
  if (chdir("/"))  // needed on windows
    exit(15);
  if (rmdir(root))
    exit(16);
}

void test_fchdir(void) {
  char root[] = "/tmp/chdir_test.XXXXXX";
  if (!mkdtemp(root))
    exit(17);
  int rootfd;
  if ((rootfd = open(root, O_RDONLY)) == -1)
    exit(18);
  if (fchdir(rootfd))
    exit(19);
  if (close(rootfd))
    exit(20);
  if (mkdir("a", 0700))
    exit(21);
  if (mkdir("b", 0700))
    exit(22);
  if (chdir("a"))
    exit(23);
  if (rmdir("../b"))
    exit(24);
  if (chdir(".."))
    exit(25);
  if (rmdir("a"))
    exit(26);
  if (chdir("/"))  // needed on windows
    exit(27);
  if (rmdir(root))
    exit(28);
}

void test_getcwd_is_same_as_chdir(void) {
  char root[] = "/tmp/chdir_test.XXXXXX";
  if (!mkdtemp(root))
    exit(29);
  if (chdir(root))
    exit(30);
  char cwd[PATH_MAX];
  if (!getcwd(cwd, PATH_MAX))
    exit(31);
  // xnu turns /tmp into /private/tmp somehow
  char *cwdp = cwd;
  if (!strncmp(cwdp, "/private/", 9))
    cwdp += 8;
  if (strcmp(cwdp, root)) {
    printf("cwdp = %s\n", cwdp);
    printf("root = %s\n", root);
    exit(32);
  }
  if (chdir("/"))  // needed on windows
    exit(33);
  if (rmdir(root))
    exit(34);
}

int main(void) {
  test_relative();
  test_dots_work_against_cwd();
  test_fchdir();
  test_getcwd_is_same_as_chdir();
}
