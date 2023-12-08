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
#include "libc/calls/struct/rlimit.h"
#include "libc/runtime/runtime.h"
#include "libc/str/locale.h"
#include "libc/sysv/consts/rlimit.h"
#include "third_party/getopt/getopt.internal.h"
#include "tool/lambda/lib/blc.h"

/**
 * @fileoverview Binary Lambda Calculus Dump Utility, e.g.
 *
 *     $ echo 0000001110 | o//blcdump -b 2>/dev/null
 *     (λ (λ (λ 2)))
 *
 * The term rom is printed to stderr along with all skewed overlapping
 * perspectives on the in-memory representation.
 *
 *     $ echo 0000001110 | o//blcdump -b >/dev/null
 *         .long  ABS     # 0=3: (λ (λ (λ 2)))
 *         .long  ABS     # 1=3: (λ (λ 2))
 *         .long  ABS     # 2=3: (λ 2)
 *         .long  VAR     # 3=1: 2
 *         .long  2       # 4=2: (⋯ ⋯)
 */

#define USAGE \
  " [-?hbBnNlS] [FILE...] <stdin >expr.txt 2>memory.txt\n\
Binary Lambda Calculus Dump Tool\n\
\n\
FLAGS\n\
\n\
  -h      Help\n\
  -b      8-bit binary mode\n\
  -B      debug print binary\n\
  -l      print lambda notation\n\
  -n      disables name rewriting rules\n\
  -N      disables most unicode symbolism\n"

void PrintUsage(const char *prog, int rc, FILE *f) {
  fputs("Usage: ", f);
  fputs(prog, f);
  fputs(USAGE, f);
  exit(rc);
}

void LoadFlags(int argc, char *argv[]) {
  int i;
  const char *prog;
  prog = argc ? argv[0] : "blcdump";
  while ((i = getopt(argc, argv, "?hubBnNlS")) != -1) {
    switch (i) {
      case 'b':
        binary = 1;
        break;
      case 'n':
        noname = 1;
        break;
      case 'N':
        asciiname = 1;
        break;
      case 'l':
        style = 1;
        break;
      case 'B':
        style = 2;
        break;
      case 'S':
        safer = 1;
        break;
      case '?':
      case 'h':
        PrintUsage(prog, 0, stdout);
      default:
        PrintUsage(prog, 1, stderr);
    }
  }
}

void Expand(int c) {
  if (end >= TERMS) Error(5, "OUT OF TERMS");
  mem[end++] = c;
}

void ExpandBit(int b) {
  Expand(ABS);
  Expand(ABS);
  Expand(VAR);
  Expand(b);
}

void ExpandList(int next) {
  Expand(ABS);
  Expand(APP);
  Expand(next);
  Expand(APP);
  Expand(2);
  Expand(VAR);
  Expand(0);
}

void ExpandItem(int b) {
  ExpandList(8);
  ExpandBit(b);
}

void ExpandByte(int b) {
  ExpandList(4 + 8 * (7 + 4));
  ExpandItem((b >> 0) & 1);
  ExpandItem((b >> 1) & 1);
  ExpandItem((b >> 2) & 1);
  ExpandItem((b >> 3) & 1);
  ExpandItem((b >> 4) & 1);
  ExpandItem((b >> 5) & 1);
  ExpandItem((b >> 6) & 1);
  ExpandItem((b >> 7) & 1);
  ExpandBit(0);
}

int main(int argc, char *argv[]) {
  struct Parse p;
  struct rlimit rlim = {512 * 1024 * 1024, 512 * 1024 * 1024};
  setrlimit(RLIMIT_AS, &rlim);
  setlocale(LC_ALL, "");
  setvbuf(stdout, 0, _IOFBF, 0);
  setvbuf(stderr, 0, _IOLBF, 0);
  LoadFlags(argc, argv);
#if DEBUG
  logh = fopen("o//log", "w");
  fprintf(logh, "      IP      END     HEAP %-*s NOM MESSAGE\n", LOC, "LOC");
  setvbuf(logh, 0, _IOLBF, 0);
#endif
  end = 32;
  for (; !feof(stdin); ip = end) {
    p = Parse(1, stdin);
    if (p.n) {
      Print(p.i, 1, 0, stdout);
      fputc('\n', stdout);
      Dump(p.i, p.i + p.n, stderr);
    }
  }
}
