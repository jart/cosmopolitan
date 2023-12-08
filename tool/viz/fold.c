/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "third_party/getopt/getopt.internal.h"

int column_;

wontreturn void usage(int rc, FILE *f) {
  fputs("Usage: ", f);
  fputs(program_invocation_name, f);
  fputs(" [-w COLS] [FILE...]\n", f);
  exit(rc);
}

void getopts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "?hw:")) != -1) {
    switch (opt) {
      case 'w':
        column_ = strtol(optarg, NULL, 0);
        break;
      case '?':
      case 'h':
        usage(EXIT_SUCCESS, stdout);
      default:
        usage(EX_USAGE, stderr);
    }
  }
  if (column_ <= 0) {
    column_ = 64;
  }
}

void fold(FILE *fin, FILE *fout) {
  unsigned column = 0;
  for (;;) {
    wint_t wc = fgetwc(fin);
    if (wc != -1) {
      fputwc(wc, fout);
      if (wc == '\n') {
        column = 0;
      } else if (++column == column_) {
        fputc('\n', fout);
        column = 0;
      }
    } else {
      if (feof(fin)) {
        return;
      } else {
        fprintf(stderr, "%s: %d\n", "fgetwc", errno);
        exit(1);
      }
    }
  }
}

int main(int argc, char *argv[]) {
  getopts(argc, argv);
  fold(stdin, stdout);
  return 0;
}
