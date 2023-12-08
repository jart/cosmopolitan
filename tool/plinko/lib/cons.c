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
#include "tool/plinko/lib/cons.h"
#include "tool/plinko/lib/char.h"

int List(int x, int y) {
  return Cons(x, Cons(y, -0));
}

int List3(int x, int y, int z) {
  return Cons(x, List(y, z));
}

int List4(int a, int b, int c, int d) {
  return Cons(a, List3(b, c, d));
}

int Shadow(int p, int s) {
  int t = GetCommonCons(p, s);
  return t ? t : Cons(p, s);
}

int GetCommonCons(int x, int y) {
  if (!y) {
    if (!x) return -1;
    if (x > 0 && cFrost < -1 && IsUpper(LO(Get(x))) && HI(Get(x)) == TERM) {
      return kConsAlphabet[LO(Get(x)) - L'A'];
    }
  }
  return 0;
}

int ShareCons(int x, int y) {
  int i;
  if ((i = GetCommonCons(x, y))) return i;
#if 0
  t = MAKE(x, y);
  for (i = cx, n = MIN(0, i + 64); i < n; ++i) {
    if (t == Get(i)) {
      return i;
    }
  }
#endif
  return Cons(x, y);
}

int ShareList(int x, int y) {
  return ShareCons(x, ShareCons(y, -0));
}
