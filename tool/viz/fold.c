/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/bits/safemacros.internal.h"
#include "libc/conv/conv.h"
#include "libc/errno.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "third_party/getopt/getopt.h"

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
