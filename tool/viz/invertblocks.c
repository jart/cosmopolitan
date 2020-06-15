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
#include "libc/log/check.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "third_party/getopt/getopt.h"

static FILE *fi_, *fo_;
static char *inpath_, *outpath_;

noreturn void usage(int rc, FILE *f) {
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
