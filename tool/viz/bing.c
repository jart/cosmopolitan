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
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/log/log.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/str/tab.internal.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/fileno.h"
#include "third_party/getopt/getopt.internal.h"

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
