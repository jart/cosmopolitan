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
#include "third_party/getopt/getopt.internal.h"
#include "tool/lambda/lib/blc.h"

#define USAGE \
  " [-?h] [FILE...] <binary.txt >binary.bin\n\
Converts ASCII binary to ACTUAL binary, e.g.\n\
\n\
    $ { printf 'λx.x' | o/lam2bin | o/asc2bin; printf abc; } | o/Blc\n\
    abc\n\
\n\
    $ printf '\n\
    (00 (01 (01 10 ((01 (00 (01 10 10))\n\
                        (00000000 (01 (01 110 ((01 11110 11110)))\n\
                                      (00 (01 (01 10 11110) 110)))))))\n\
            (0000 10)))\n\
    ' | asc2bin | xxd -b\n\
    00000000: 00010110 01000110 10000000 00010111 00111110 11110000  .F..>.\n\
    00000006: 10110111 10110000 01000000                             ..@\n\
\n\
FLAGS\n\
\n\
  -h      Help\n\
  -?      Help\n"

void LoadFlags(int argc, char *argv[]) {
  int i;
  const char *prog;
  prog = argc ? argv[0] : "asc2bin";
  while ((i = getopt(argc, argv, "?h")) != -1) {
    switch (i) {
      case '?':
      case 'h':
        fputs("Usage: ", stdout);
        fputs(prog, stdout);
        fputs(USAGE, stdout);
        exit(0);
      default:
        fputs("Usage: ", stderr);
        fputs(prog, stderr);
        fputs(USAGE, stderr);
        exit(1);
    }
  }
}

int main(int argc, char *argv[]) {
  int i, b, c, n;
  LoadFlags(argc, argv);
  n = c = i = 0;
  while ((b = GetBit(stdin)) != -1) {
    c |= b << (7 - n);
    if (++n == 8) {
      fputc(c, stdout);
      c = 0;
      n = 0;
    }
  }
  if (n) {
    fputc(c, stdout);
  }
}
