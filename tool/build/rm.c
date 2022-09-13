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
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/ok.h"
#include "third_party/getopt/getopt.h"

#define USAGE \
  " FILE...\n\
\n\
SYNOPSIS\n\
\n\
  Removes Files\n\
\n\
FLAGS\n\
\n\
  -?\n\
  -h      help\n\
  -f      force\n\
\n"

bool force;
const char *prog;

wontreturn void PrintUsage(int rc, FILE *f) {
  fputs("usage: ", f);
  fputs(prog, f);
  fputs(USAGE, f);
  exit(rc);
}

void GetOpts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "?hf")) != -1) {
    switch (opt) {
      case 'f':
        force = true;
        break;
      case 'h':
      case '?':
        PrintUsage(EXIT_SUCCESS, stdout);
      default:
        PrintUsage(EX_USAGE, stderr);
    }
  }
}

void Remove(const char *path) {
  const char *s;
  if (!force && access(path, W_OK) == -1) goto OnFail;
  if (unlink(path) == -1) goto OnFail;
  return;
OnFail:
  if (force && errno == ENOENT) return;
  s = _strerdoc(errno);
  fputs(prog, stderr);
  fputs(": cannot remove '", stderr);
  fputs(path, stderr);
  fputs("': ", stderr);
  fputs(s, stderr);
  fputs("\n", stderr);
  exit(1);
}

int main(int argc, char *argv[]) {
  int i;
  prog = argc > 0 ? argv[0] : "rm.com";

  if (argc < 2) {
    fputs(prog, stderr);
    fputs(": missing operand\n"
          "Try 'rm -h' for more information.\n",
          stderr);
    exit(1);
  }

  GetOpts(argc, argv);

  for (i = optind; i < argc; ++i) {
    Remove(argv[i]);
  }
}
