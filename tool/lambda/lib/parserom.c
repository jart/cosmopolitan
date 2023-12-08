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
#include "libc/str/str.h"
#include "tool/lambda/lib/blc.h"

static struct Parse ParseImpl(int tail, int need, FILE *f) {
  struct Parse p, q;
  int b, i, j, t, start;
  for (start = end;;) {
    if (end + 2 > TERMS) Error(5, "OUT OF TERMS");
    if ((b = GetBit(f)) == -1) {
      if (need) Error(9, "UNFINISHED EXPRESSION");
      break;
    } else if (b) {
      for (t = 0; NeedBit(f);) ++t;
      mem[end++] = VAR;
      mem[end++] = t;
      break;
    } else if (NeedBit(f)) {
      t = end;
      end += 2;
      p = ParseImpl(0, 1, f);
      q = ParseImpl(t + 2, 1, f);
      mem[t + 0] = APP;
      mem[t + 1] = q.i - (t + 2);
      break;
    } else {
      mem[end++] = ABS;
    }
  }
  p.i = start;
  p.n = end - start;
  if (p.n && tail) {
    /* find backwards overlaps within 8-bit displacement */
    i = tail - 32768;
    j = start - p.n;
    for (i = i < 0 ? 0 : i; i <= j; ++i) {
      if (!memcmp(mem + i, mem + p.i, p.n * sizeof(*mem))) {
        memset(mem + start, -1, p.n * sizeof(*mem));
        end = start;
        p.i = i;
        break;
      }
    }
  }
  return p;
}

/**
 * Parses binary lambda calculus closed expression from stream.
 *
 * If `tail` is non-zero then this subroutine will perform expensive
 * deduplication so that optimal ROMs may be computed ahead of time.
 */
struct Parse Parse(int tail, FILE *f) {
  return ParseImpl(tail, 0, f);
}
