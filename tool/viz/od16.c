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
#include "libc/fmt/conv.h"
#include "libc/fmt/fmt.h"
#include "libc/log/check.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "third_party/getopt/getopt.h"

#define USAGE \
  " [FLAGS] [PATH...]\n\
this program prints binary as signed 16-bit decimal numbers\n\
\n\
Flags:\n\
  -n INT\n\
  -c INT\n\
  -w INT     width (aka cols) [default 8]\n\
  -o PATH    output path [default -]\n\
  -h         shows this information\n\
\n"

static long width_;
static FILE *in_, *out_;
static char *inpath_, *outpath_;

void PrintUsage(int rc, FILE *f) {
  fputs("Usage: ", f);
  fputs(program_invocation_name, f);
  fputs(USAGE, f);
  exit(rc);
}

void GetOpts(int *argc, char *argv[]) {
  int opt;
  outpath_ = "-";
  while ((opt = getopt(*argc, argv, "?ho:w:c:n:")) != -1) {
    switch (opt) {
      case 'o':
        outpath_ = optarg;
        break;
      case 'n':
      case 'c':
      case 'w':
        width_ = strtol(optarg, NULL, 0);
        break;
      case '?':
      case 'h':
        PrintUsage(EXIT_SUCCESS, stdout);
      default:
        PrintUsage(EX_USAGE, stderr);
    }
  }
  if (optind == *argc) {
    argv[(*argc)++] = "-";
  }
}

void ProcessFile(void) {
  int a, b;
  unsigned n;
  n = 0;
  while ((a = fgetc(in_)) != -1 && (b = fgetc(in_)) != -1) {
    if (n) {
      fputc(',', out_);
      fputc(' ', out_);
    }
    fprintf(out_, "%7hd", b << 010 | a);
    if (++n == width_) {
      n = 0;
      fputc('\n', out_);
    }
  }
  if (n) {
    fputc('\n', out_);
  }
}

int main(int argc, char *argv[]) {
  size_t i;
  GetOpts(&argc, argv);
  CHECK_NOTNULL((out_ = fopen(outpath_, "w")));
  for (i = optind; i < argc; ++i) {
    CHECK_NOTNULL((in_ = fopen((inpath_ = argv[i]), "r")));
    ProcessFile();
    CHECK_NE(-1, fclose_s(&in_));
  }
  CHECK_NE(-1, fclose_s(&out_));
  return 0;
}
