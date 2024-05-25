/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

/*
 * This test was contributed by @ahgamut
 * https://github.com/jart/cosmopolitan/issues/61#issuecomment-792214575
 */

char path[PATH_MAX];

void teardown(void) {
  unlink(path);
}

int setup(void) {
  int fd;
  const char* tmpdir;
  if ((tmpdir = getenv("TMPDIR")))
    strlcpy(path, tmpdir, sizeof(path));
  else
    strlcpy(path, "/tmp", sizeof(path));
  strlcat(path, "/freopen_test.XXXXXX", sizeof(path));
  if ((fd = mkstemp(path)) == -1)
    return 1;
  if (write(fd, "cosmopolitan libc\n", 18) != 18)
    return 2;
  if (close(fd))
    return 3;
  return 0;
}

int test(void) {
  FILE* fp;
  char buf[20];
  if (!(fp = fopen(path, "r")))
    return 4;
  if (fgetc(fp) != 'c')
    return 5;
  if (!(fp = freopen(path, "rb", fp)))
    return 6;
  if (fread(buf, 1, 20, fp) != 18)
    return 7;
  if (memcmp(buf, "cosmopolitan libc\n", 18))
    return 8;
  if (fclose(fp))
    return 9;
  return 0;
}

int main(int argc, char* argv[]) {
  int rc;
  if ((rc = setup())) {
    perror(path);
    teardown();
    return rc;
  }
  rc = test();
  teardown();
  return rc;
}
