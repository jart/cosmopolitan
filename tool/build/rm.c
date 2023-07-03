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
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ok.h"
#include "third_party/getopt/getopt.internal.h"

#define USAGE \
  " FILE...\n\
\n\
SYNOPSIS\n\
\n\
  Removes Files\n\
\n\
FLAGS\n\
\n\
  -h      help\n\
  -f      force\n\
\n"

static bool force;
static const char *prog;

static wontreturn void PrintUsage(int rc, int fd) {
  tinyprint(fd, "USAGE\n\n  ", prog, USAGE, NULL);
  exit(rc);
}

static void GetOpts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "hf")) != -1) {
    switch (opt) {
      case 'f':
        force = true;
        break;
      case 'h':
        PrintUsage(0, 1);
      default:
        PrintUsage(1, 2);
    }
  }
}

static void Remove(const char *path) {
  if (!force && access(path, W_OK) == -1) {
    perror(path);
    exit(1);
  }
  if (unlink(path) == -1) {
    if (force && errno == ENOENT) return;
    perror(path);
    exit(1);
  }
}

int main(int argc, char *argv[]) {
  int i;

  prog = argv[0];
  if (!prog) prog = "rm";

  if (argc < 2) {
    tinyprint(2, prog, ": missing operand\n", NULL);
    exit(1);
  }

  GetOpts(argc, argv);

  for (i = optind; i < argc; ++i) {
    Remove(argv[i]);
  }
}
