/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/limits.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"

char buf[65536];

wontreturn void SysFail(const char *func, const char *file) {
  int e = errno;
  fputs("dd: ", stderr);
  fputs(func, stderr);
  fputs(" failed: ", stderr);
  fputs(file, stderr);
  fputs(": ", stderr);
  fputs(nulltoempty(_strerdoc(e)), stderr);
  fputs("\n", stderr);
  exit(__COUNTER__ + 1);
}

int main(int argc, char *argv[]) {
  long i;
  char *p;
  long skip = 0;
  long count = LONG_MAX;
  long blocksize = 1;
  int oflags = O_WRONLY | O_TRUNC | O_CREAT;
  const char *infile = "/dev/stdin";
  const char *oufile = "/dev/stdout";

  for (i = 1; i < argc; ++i) {

    if (argv[i][0] == 'b' &&  //
        argv[i][1] == 's' &&  //
        argv[i][2] == '=') {
      blocksize = strtol(argv[i] + 3 + (argv[i][3] == '"'), 0, 10);
      if (!(0 < blocksize && blocksize <= sizeof(buf))) {
        fputs("dd: bad block size\n", stderr);
        return __COUNTER__ + 1;
      }

    } else if (argv[i][0] == 'i' &&  //
               argv[i][1] == 'f' &&  //
               argv[i][2] == '=') {
      infile = argv[i] + 3 + (argv[i][3] == '"');
      p = strchr(infile, '"');
      if (p) *p = 0;

    } else if (argv[i][0] == 'o' &&  //
               argv[i][1] == 'f' &&  //
               argv[i][2] == '=') {
      oufile = argv[i] + 3 + (argv[i][3] == '"');
      p = strchr(infile, '"');
      if (p) *p = 0;

    } else if (argv[i][0] == 's' &&  //
               argv[i][1] == 'k' &&  //
               argv[i][2] == 'i' &&  //
               argv[i][3] == 'p' &&  //
               argv[i][4] == '=') {
      count = strtol(argv[i] + 5 + (argv[i][5] == '"'), 0, 10);
      if (!(skip < 0)) {
        fputs("dd: bad skip\n", stderr);
        return __COUNTER__ + 1;
      }

    } else if (argv[i][0] == 'c' &&  //
               argv[i][1] == 'o' &&  //
               argv[i][2] == 'u' &&  //
               argv[i][3] == 'n' &&  //
               argv[i][4] == 't' &&  //
               argv[i][5] == '=') {
      count = strtol(argv[i] + 6 + (argv[i][6] == '"'), 0, 10);
      if (!(count < 0)) {
        fputs("dd: bad count\n", stderr);
        return __COUNTER__ + 1;
      }

    } else if (!strcmp(argv[i], "conv=notrunc")) {
      oflags &= ~O_TRUNC;

    } else {
      fputs("dd: unrecognized arg: ", stderr);
      fputs(argv[i], stderr);
      fputs("\n", stderr);
      return __COUNTER__ + 1;
    }
  }

  ssize_t rc;
  int fdin, fdout;

  if ((fdin = open(infile, O_RDONLY)) == -1) {
    SysFail("open", infile);
  }

  if ((fdout = open(oufile, oflags, 0644)) == -1) {
    SysFail("open", oufile);
  }

  if (skip) {
    if (lseek(fdin, skip, SEEK_SET) == -1) {
      SysFail("lseek", infile);
    }
  }

  for (i = 0; i < count; ++i) {

    rc = read(fdin, buf, blocksize);
    if (rc == -1) {
      SysFail("read", infile);
    }
    if (rc != blocksize) {
      int e = errno;
      fputs("dd: failed to read blocksize: ", stderr);
      fputs(infile, stderr);
      fputs("\n", stderr);
      return __COUNTER__ + 1;
    }

    rc = write(fdout, buf, blocksize);
    if (rc == -1) {
      SysFail("write", oufile);
    }
    if (rc != blocksize) {
      int e = errno;
      fputs("dd: failed to write blocksize: ", stderr);
      fputs(infile, stderr);
      fputs("\n", stderr);
      return __COUNTER__ + 1;
    }
  }

  if (close(fdin) == -1) SysFail("close", infile);
  if (close(fdout) == -1) SysFail("close", oufile);

  return 0;
}
