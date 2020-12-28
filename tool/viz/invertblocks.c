/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/log/check.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "third_party/getopt/getopt.h"

static FILE *fi_, *fo_;
static char *inpath_, *outpath_;

wontreturn void usage(int rc, FILE *f) {
  fprintf(f, "%s%s%s\n", "Usage: ", program_invocation_name,
          " [-o FILE] [FILE...]\n");
  exit(rc);
}

void getopts(int *argc, char *argv[]) {
  int opt;
  outpath_ = "-";
  while ((opt = getopt(*argc, argv, "?ho:")) != -1) {
    switch (opt) {
      case 'o':
        outpath_ = optarg;
        break;
      case '?':
      case 'h':
        usage(EXIT_SUCCESS, stdout);
      default:
        usage(EX_USAGE, stderr);
    }
  }
  if (optind == *argc) {
    argv[(*argc)++] = "-";
  }
}

void processfile(void) {
  wint_t wc;
  while ((wc = fgetwc(fi_)) != -1) {
    switch (wc) {
      case L'█':
        wc = L' ';
        break;
      case L'▓':
        wc = L'░';
        break;
      case L'▒':
        wc = L'▒';
        break;
      case L'░':
        wc = L'▓';
        break;
        /* █ */
      case L'▐':
        wc = L'▌';
        break;
      case L'▌':
        wc = L'▐';
        break;
      case L'▀':
        wc = L'▄';
        break;
      case L'▄':
        wc = L'▀';
        break;
      case L'☺':
        wc = L'☻';
        break;
      case L'☻':
        wc = L'☺';
        break;
      case L'○':
        wc = L'◙';
        break;
      case L'◙':
        wc = L'○';
        break;
      default:
        break;
    }
    fputwc(wc, fo_);
  }
}

int main(int argc, char *argv[]) {
  size_t i;
  getopts(&argc, argv);
  CHECK_NOTNULL((fo_ = fopen(outpath_, "w")));
  for (i = optind; i < argc; ++i) {
    CHECK_NOTNULL((fi_ = fopen((inpath_ = argv[i]), "r")));
    processfile();
    CHECK_NE(-1, fclose_s(&fi_));
  }
  CHECK_NE(-1, fclose_s(&fo_));
  return 0;
}
