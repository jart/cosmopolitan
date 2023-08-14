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
#include "libc/fmt/conv.h"
#include "libc/log/check.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/fileno.h"
#include "third_party/getopt/getopt.internal.h"

#define USAGE \
  " [FLAGS] [PATH...]\n\
this program prints binary as signed 16-bit decimal numbers\n\
\n\
Flags:\n\
  -n INT\n\
  -c INT\n\
  -w INT     width (aka cols) [default 8]\n\
  -o PATH    output path [default -]\n\
  -h or -?   shows this information\n\
\n"

static long width_;
static FILE *in_, *out_;
static char *inpath_, *outpath_;

void PrintUsage(int rc, int fd) {
  tinyprint(fd, "Usage: ", program_invocation_name, USAGE, NULL);
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
      case 'h':
      case '?':
      default:
        if (opt == optopt) {
          PrintUsage(EXIT_SUCCESS, STDOUT_FILENO);
        } else {
          PrintUsage(EX_USAGE, STDERR_FILENO);
        }
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
    CHECK_NE(-1, fclose(in_));
    in_ = 0;
  }
  CHECK_NE(-1, fclose(out_));
  out_ = 0;
  return 0;
}
