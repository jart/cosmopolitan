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
#include "libc/calls/calls.h"
#include "libc/log/log.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/fileno.h"
#include "third_party/getopt/getopt.h"

/**
 * @fileoverview Bing: Binary → Glyphs.
 * Intended for oldskool data science.
 */

int ispipe_;
int newlines_;

wontreturn void ShowUsage(FILE *f, int rc) {
  fputs(program_invocation_name, f);
  fputs(": [-p] [-n] [FILE...]\n", f);
  exit(rc);
}

void GetOpts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "hpn")) != -1) {
    switch (opt) {
      case 'p':
        ispipe_ = true;
        break;
      case 'n':
        newlines_ = true;
        break;
      case 'h':
        ShowUsage(stdout, EXIT_SUCCESS);
      default:
        ShowUsage(stderr, EX_USAGE);
    }
  }
}

void bing(FILE *f) {
  int c, c2;
  while ((c = fgetc(f)) != -1) {
    c2 = c == '\n' && newlines_ ? '\n' : kCp437[c & 0xff];
    if (ispipe_) {
      fputc(c, stdout);
      fputwc(c2, stderr);
    } else {
      fputwc(c2, stdout);
    }
  }
  fputc('\n', ispipe_ ? stderr : stdout);
}

int main(int argc, char *argv[]) {
  FILE *f;
  size_t i;
  GetOpts(argc, argv);
  if (optind < argc) {
    for (i = optind; i < argc; ++i) {
      if (!(f = fopen(argv[i], "rb"))) {
        perror(argv[i]);
        exit(EXIT_FAILURE);
      }
      bing(f);
      fclose(f);
    }
  } else {
    bing(stdin);
  }
  return 0;
}
