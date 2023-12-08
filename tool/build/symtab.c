/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/runtime/symbols.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/o.h"
#include "third_party/getopt/getopt.internal.h"

/**
 * @fileoverview elf to symbol table file dump tool
 */

void PrintUsage(FILE *f) {
  fprintf(f, "%s%s%s\n", "usage: ", program_invocation_name,
          " [-?h] -o PATH COMDBG");
}

int main(int argc, char *argv[]) {
  int fd, opt;
  const char *outpath;
  struct SymbolTable *tab;

  outpath = 0;

  while ((opt = getopt(argc, argv, "?ho:")) != -1) {
    switch (opt) {
      case 'o':
        outpath = optarg;
        break;
      case '?':
      case 'h':
        PrintUsage(stdout);
        return 0;
      default:
        PrintUsage(stderr);
        return EX_USAGE;
    }
  }

  if (!outpath) {
    fprintf(stderr, "error: need output path\n");
    PrintUsage(stderr);
    return 1;
  }

  if (optind + 1 != argc) {
    fprintf(stderr, "error: need exactly one input path\n");
    PrintUsage(stderr);
    return 2;
  }

  if (!(tab = OpenSymbolTable(argv[optind]))) {
    fprintf(stderr, "error: %s(%`'s) failed %m\n", "OpenSymbolTable",
            argv[optind]);
    return 3;
  }

  tab->names = 0;
  tab->name_base = 0;

  if ((fd = open(outpath, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1) {
    fprintf(stderr, "error: %s(%`'s) failed %m\n", "open", outpath);
    return 4;
  }

  if (write(fd, (const char *)tab, tab->size) != tab->size) {
    fprintf(stderr, "error: %s(%`'s) failed %m\n", "write", outpath);
    return 5;
  }

  if (close(fd) == -1) {
    fprintf(stderr, "error: %s(%`'s) failed %m\n", "close", outpath);
    return 6;
  }

  CloseSymbolTable(&tab);
  return 0;
}
