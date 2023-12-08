/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/fmt/conv.h"
#include "libc/limits.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"

char buf[65536];
const char *prog;

static wontreturn void Die(const char *reason) {
  tinyprint(2, prog, ": ", reason, "\n", NULL);
  exit(1);
}

static wontreturn void DieSys(const char *thing) {
  perror(thing);
  exit(1);
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

  prog = argv[0];
  if (!prog) prog = "dd";

  for (i = 1; i < argc; ++i) {

    if (argv[i][0] == 'b' &&  //
        argv[i][1] == 's' &&  //
        argv[i][2] == '=') {
      blocksize = strtol(argv[i] + 3 + (argv[i][3] == '"'), 0, 10);
      if (!(0 < blocksize && blocksize <= sizeof(buf))) {
        Die("bad block size");
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
        Die("bad skip");
      }

    } else if (argv[i][0] == 'c' &&  //
               argv[i][1] == 'o' &&  //
               argv[i][2] == 'u' &&  //
               argv[i][3] == 'n' &&  //
               argv[i][4] == 't' &&  //
               argv[i][5] == '=') {
      count = strtol(argv[i] + 6 + (argv[i][6] == '"'), 0, 10);
      if (!(count < 0)) {
        Die("bad count");
      }

    } else if (!strcmp(argv[i], "conv=notrunc")) {
      oflags &= ~O_TRUNC;

    } else {
      Die("unrecognized arg");
    }
  }

  ssize_t rc;
  int fdin, fdout;

  if ((fdin = open(infile, O_RDONLY)) == -1) {
    DieSys(infile);
  }

  if ((fdout = open(oufile, oflags, 0644)) == -1) {
    DieSys(oufile);
  }

  if (skip) {
    if (lseek(fdin, skip, SEEK_SET) == -1) {
      DieSys(infile);
    }
  }

  for (i = 0; i < count; ++i) {

    rc = read(fdin, buf, blocksize);
    if (rc == -1) {
      DieSys(infile);
    }
    if (rc != blocksize) {
      Die("failed to read full blocksize");
    }

    rc = write(fdout, buf, blocksize);
    if (rc == -1) {
      DieSys(oufile);
    }
    if (rc != blocksize) {
      Die("failed to write full blocksize");
    }
  }

  if (close(fdin) == -1) {
    DieSys(infile);
  }
  if (close(fdout) == -1) {
    DieSys(oufile);
  }

  return 0;
}
