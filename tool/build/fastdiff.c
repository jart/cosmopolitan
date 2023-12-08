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
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

/**
 * @fileoverview scalable diff tool
 *
 * The normal `diff` command can take hours to diff text files that are
 * hundreds of megabytes in size. This tool is a useful replacement for
 * use cases like comparing a log of CPU registers.
 */

char line1[4096];
char line2[4096];

int main(int argc, char *argv[]) {
  int line;
  char *l1, *l2;
  FILE *f1, *f2;
  if (argc < 3) {
    fprintf(stderr, "usage: %s FILE1 FILE2\n", argv[0]);
    exit(1);
  }
  if (!(f1 = fopen(argv[1], "r"))) {
    perror(argv[1]);
    exit(1);
  }
  if (!(f2 = fopen(argv[2], "r"))) {
    perror(argv[2]);
    exit(1);
  }
  for (line = 1;; ++line) {
    l1 = fgets(line1, sizeof(line1), f1);
    l2 = fgets(line2, sizeof(line2), f2);
    if (!l1 && !l2) {
      exit(0);
    }
    if (l1) chomp(l1);
    if (l2) chomp(l2);
    if (!l1 || !l2) {
      printf("> %s\n", l1 ? l1 : "EOF");
      printf("< %s\n", l2 ? l2 : "EOF");
      exit(0);
    }
    if (!strcmp(l1, l2)) {
      printf("%s\n", l1);
    } else {
      printf("# line %d differed!\n", line);
      printf("> %s\n", l1);
      printf("< %s\n", l2);
    }
  }
}
