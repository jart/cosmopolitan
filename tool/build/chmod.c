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
#include "libc/calls/struct/dirent.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "third_party/getopt/getopt.h"

#define USAGE \
  " OCTAL DST...\n\
\n\
SYNOPSIS\n\
\n\
  Changes File Mode Bits\n\
\n\
FLAGS\n\
\n\
  -?\n\
  -h      help\n\
\n"

const char *prog;

wontreturn void PrintUsage(int rc, FILE *f) {
  fputs("usage: ", f);
  fputs(prog, f);
  fputs(USAGE, f);
  exit(rc);
}

void GetOpts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "?h")) != -1) {
    switch (opt) {
      case 'h':
      case '?':
        PrintUsage(EXIT_SUCCESS, stdout);
      default:
        PrintUsage(EX_USAGE, stderr);
    }
  }
}

int main(int argc, char *argv[]) {
  int i, mode;
  char buf[PATH_MAX];

  {
    printf("curdir %s\n", getcwd(buf, sizeof(buf)));
    printf("tmp:");
    struct dirent *e;
    DIR *d;
    if ((d = opendir("tmp"))) {
      while ((e = readdir(d))) {
        printf(" %s", e->d_name);
      }
      closedir(d);
    } else {
      printf(" dir not found");
    }
    printf("\n");
  }

  {
    printf("curdir %s\n", getcwd(buf, sizeof(buf)));
    printf("bin:");
    struct dirent *e;
    DIR *d;
    if ((d = opendir("bin"))) {
      while ((e = readdir(d))) {
        printf(" %s", e->d_name);
      }
      closedir(d);
    } else {
      printf(" dir not found");
    }
    printf("\n");
  }

  prog = argc > 0 ? argv[0] : "mv.com";
  GetOpts(argc, argv);
  if (argc - optind < 2) {
    PrintUsage(EX_USAGE, stderr);
  }
  mode = strtol(argv[optind], 0, 8) & 07777;
  for (i = optind + 1; i < argc; ++i) {
    if (chmod(argv[i], mode) == -1) {
      const char *s = _strerdoc(errno);
      fputs(prog, stderr);
      fputs(": ", stderr);
      fputs(argv[i], stderr);
      fputs(": ", stderr);
      fputs(s, stderr);
      fputs("\n", stderr);
      exit(1);
    }
  }
  return 0;
}
