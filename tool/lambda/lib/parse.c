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
#include "tool/lambda/lib/blc.h"

/**
 * Parses binary lambda calculus closed expression from stream.
 */
struct Parse Parse(int ignored, FILE* f) {
  int t, start;
  char bit, need;
  struct Parse p;
  for (need = 0, start = end;;) {
    if (end + 2 > TERMS) Error(5, "OUT OF TERMS");
    if ((bit = GetBit(f)) == -1) {
      if (!need) break;
      fflush(stdout);
      fputs("---\n", stderr);
      Print(start, 0, 0, stderr);
      Error(9, "UNFINISHED EXPRESSION");
    } else if (bit) {
      for (t = 0; NeedBit(f);) ++t;
      mem[end++] = VAR;
      mem[end++] = t;
      break;
    } else if (NeedBit(f)) {
      t = end;
      end += 2;
      mem[t] = APP;
      p = Parse(0, f);
      mem[t + 1] = p.n;
      need = 1;
    } else {
      mem[end++] = ABS;
    }
  }
  p.i = start;
  p.n = end - start;
  return p;
}
