/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/runtime/runtime.h"
#include "libc/serialize.h"
#include "libc/stdio/ftw.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/o.h"

// finds ape executables
// usage: findelf PATH...

static int OnFile(const char *fpath, const struct stat *st, int typeflag,
                  struct FTW *ftwbuf) {
  if (typeflag == FTW_F) {
    char buf[8] = {0};
    int fd = open(fpath, O_RDONLY);
    if (fd != -1) {
      pread(fd, buf, sizeof(buf), 0);
      close(fd);
      if (READ64LE(buf) == READ64LE("MZqFpD='") ||
          READ64LE(buf) == READ64LE("jartsr='") ||
          READ64LE(buf) == READ64LE("APEDBG='")) {
        tinyprint(1, fpath, "\n", NULL);
      }
    } else {
      perror(fpath);
    }
  }
  return 0;
}

static void HandleArg(const char *path) {
  if (nftw(path, OnFile, 128, FTW_PHYS | FTW_DEPTH)) {
    perror(path);
    exit(1);
  }
}

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    HandleArg(".");
  } else {
    for (int i = 1; i < argc; ++i) {
      HandleArg(argv[i]);
    }
  }
}
