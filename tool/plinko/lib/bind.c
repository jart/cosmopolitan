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
#include "tool/plinko/lib/plinko.h"

struct Binding Bind(int x, int y, int a, int u, dword p1, dword p2) {
  int k, v;
  dword a1 = 0;
  while (x) {
    if (x < 0) {
      if (y <= 0) {
        k = Car(x), x = Cdr(x);
        v = Car(y), y = Cdr(y);
        if (k) {
          if (k > 0) {
            if (!a1) {
              a1 = MAKE(k, FasterRecurse(v, a, p1, p2));
            } else {
              u = Alist(k, FasterRecurse(v, a, p1, p2), u);
            }
          } else {
            u = pairlis(k, FasterRecurse(v, a, p1, p2), u);
          }
        }
      } else {
        u = pairlis(x, FasterRecurse(y, a, p1, p2), u);
        y = 0;
        break;
      }
    } else {
      u = Alist(x, evlis(y, a, p1, p2), u);
      y = 0;
      break;
    }
  }
  if (y < 0) {
    Error("bind: too many arguments x=%S y=%S", x, y);
  }
  return (struct Binding){u, a1};
}
