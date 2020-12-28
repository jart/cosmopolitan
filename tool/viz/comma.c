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
  " [FLAGS] [PATH|FLEXDEC...] [<<<FLEXDEC...\\n...]\n\
Turns numbers into decimal with commas.\n\
\n\
Values are tokenized by spaces. Anything that isn't an integer is\n\
passed through. We parse integers the same way the C compiler does\n\
so 0x,0b,0,etc. prefixes are fine. Unicode spacing characters are\n\
supported.\n\
\n\
Flags:\n\
  -o PATH    output path\n\
  -F STR\n\
  -T STR     sets field tokenization charset [default is whitespace]\n\
  -w FLEXDEC\n\
  -n FLEXDEC set fixed number of columns [default is variable, based\n\
             on line breaks]\n\
  -?\n\
  -h         shows this information\n\
\n"

static size_t linecap_;
static FILE *in_, *out_;
static const char16_t *fieldtoks_;
static char *inpath_, *outpath_, *line_;

void PrintUsage(int rc, FILE *f) {
  fputs("Usage: ", f);
  fputs(program_invocation_name, f);
  fputs(USAGE, f);
  exit(rc);
}

void GetOpts(int *argc, char *argv[]) {
  int opt;
  outpath_ = "-";
  fieldtoks_ = u" \t\v\n\r\f             　";
  while ((opt = getopt(*argc, argv, "?ho:F:T:w:n:")) != -1) {
    switch (opt) {
      case 'o':
        outpath_ = optarg;
        break;
      case 'F':
      case 'T':
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
  while ((getline(&line_, &linecap_, in_)) != -1) {
    // TODO(jart)
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
  free(line_);
  return 0;
}
