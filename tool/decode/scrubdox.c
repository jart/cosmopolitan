/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
╚──────────────────────────────────────────────────────────────────────────────┘

  THIS PROGRAM TURNS TEXT LIKE THIS

   +------------------------------------------------------------------------+
   | Button     | Name        | Go to                          | From 1.2.3 |
   |            |             |                                | go to      |
   |------------+-------------+--------------------------------+------------|
   |   [ < ]    |    Back     | previous section in reading    | 1.2.2      |
   |            |             | order                          |            |
   |------------+-------------+--------------------------------+------------|
   |   [ > ]    |   Forward   | next section in reading order  | 1.2.4      |
   |------------+-------------+--------------------------------+------------|
   |   [ << ]   |  FastBack   | previous or up-and-previous    | 1.1        |
   |            |             | section                        |            |
   |------------+-------------+--------------------------------+------------|
   |   [ Up ]   |     Up      | up section                     | 1.2        |
   |------------+-------------+--------------------------------+------------|
   |   [ >> ]   | FastForward | next or up-and-next section    | 1.3        |
   |------------+-------------+--------------------------------+------------|
   |   [Top]    |     Top     | cover (top) of document        |            |
   |------------+-------------+--------------------------------+------------|
   | [Contents] |  Contents   | table of contents              |            |
   |------------+-------------+--------------------------------+------------|
   |  [Index]   |    Index    | concept index                  |            |
   |------------+-------------+--------------------------------+------------|
   |   [ ? ]    |    About    | this page                      |            |
   +------------------------------------------------------------------------+

  INTO THIS

   ┌────────────┬─────────────┬────────────────────────────────┬────────────┐
   │ Button     │ Name        │ Go to                          │ From 1.2.3 │
   │            │             │                                │ go to      │
   ├────────────┼─────────────┼────────────────────────────────┼────────────┤
   │   [ < ]    │    Back     │ previous section in reading    │ 1.2.2      │
   │            │             │ order                          │            │
   ├────────────┼─────────────┼────────────────────────────────┼────────────┤
   │   [ > ]    │   Forward   │ next section in reading order  │ 1.2.4      │
   ├────────────┼─────────────┼────────────────────────────────┼────────────┤
   │   [ << ]   │  FastBack   │ previous or up─and─previous    │ 1.1        │
   │            │             │ section                        │            │
   ├────────────┼─────────────┼────────────────────────────────┼────────────┤
   │   [ Up ]   │     Up      │ up section                     │ 1.2        │
   ├────────────┼─────────────┼────────────────────────────────┼────────────┤
   │   [ >> ]   │ FastForward │ next or up─and─next section    │ 1.3        │
   ├────────────┼─────────────┼────────────────────────────────┼────────────┤
   │   [Top]    │     Top     │ cover (top) of document        │            │
   ├────────────┼─────────────┼────────────────────────────────┼────────────┤
   │ [Contents] │  Contents   │ table of contents              │            │
   ├────────────┼─────────────┼────────────────────────────────┼────────────┤
   │  [Index]   │    Index    │ concept index                  │            │
   ├────────────┼─────────────┼────────────────────────────────┼────────────┤
   │   [ ? ]    │    About    │ this page                      │            │
   └────────────┴─────────────┴────────────────────────────────┴────────────┘ */
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/mem/gc.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/str/strwidth.h"
#include "libc/x/x.h"
#include "libc/x/xasprintf.h"
#include "libc/x/xgetline.h"

#define IsSpace(C)  ((C) == ' ')
#define IsPipe(C)   ((C) == '|' || (C) == u'│')
#define IsPlus(C)   ((C) == '+' || (C) == u'┼')
#define IsHyphen(C) ((C) == '-' || (C) == u'─')
#define IsTick(C)   ((C) == '`' || (C) == u'└')
#define IsPipe(C)   ((C) == '|' || (C) == u'│')
#define IsEquals(C) ((C) == '=' || (C) == u'═')

int n;
int yn;
int xn;

FILE *f;
bool *V;
char **T;
char16_t **L;

static void DoIt(int y, int x) {
  if (V[y * (xn + 1) + x]) return;
  V[y * (xn + 1) + x] = 1;
  if (IsPipe(L[y - 1][x]) && IsHyphen(L[y][x - 1]) && IsPlus(L[y][x]) &&
      IsHyphen(L[y][x + 1]) && IsPipe(L[y + 1][x])) {
    L[y][x] = u'┼';
  } else if (IsPipe(L[y - 1][x]) && IsEquals(L[y][x - 1]) && IsPlus(L[y][x]) &&
             IsEquals(L[y][x + 1]) && IsPipe(L[y + 1][x])) {
    L[y][x] = u'╪';
  } else if (IsSpace(L[y - 1][x]) && IsHyphen(L[y][x - 1]) &&
             IsHyphen(L[y][x]) && IsHyphen(L[y][x + 1]) &&
             IsPipe(L[y + 1][x])) {
    L[y][x] = u'┬';
  } else if (IsPipe(L[y - 1][x]) && IsHyphen(L[y][x - 1]) &&
             IsHyphen(L[y][x]) && IsHyphen(L[y][x + 1]) &&
             IsSpace(L[y + 1][x])) {
    L[y][x] = u'┴';
  } else if (IsPipe(L[y - 1][x]) && IsSpace(L[y][x - 1]) && IsPipe(L[y][x]) &&
             IsHyphen(L[y][x + 1]) && IsPipe(L[y + 1][x])) {
    L[y][x] = u'├';
  } else if (IsPipe(L[y - 1][x]) && IsHyphen(L[y][x - 1]) && IsPipe(L[y][x]) &&
             IsSpace(L[y][x + 1]) && IsPipe(L[y + 1][x])) {
    L[y][x] = u'┤';
  } else if (IsSpace(L[y - 1][x]) && IsSpace(L[y][x - 1]) && IsPlus(L[y][x]) &&
             IsHyphen(L[y][x + 1]) && IsPipe(L[y + 1][x])) {
    L[y][x] = u'┌';
  } else if (IsPipe(L[y - 1][x]) && IsHyphen(L[y][x - 1]) && IsPlus(L[y][x]) &&
             IsSpace(L[y][x + 1]) && IsSpace(L[y + 1][x])) {
    L[y][x] = u'┘';
  } else if (IsSpace(L[y - 1][x]) && IsHyphen(L[y][x - 1]) && IsPlus(L[y][x]) &&
             IsSpace(L[y][x + 1]) && IsPipe(L[y + 1][x])) {
    L[y][x] = u'┐';
  } else if (IsPipe(L[y - 1][x]) && IsSpace(L[y][x - 1]) && IsPlus(L[y][x]) &&
             IsHyphen(L[y][x + 1]) && IsSpace(L[y + 1][x])) {
    L[y][x] = u'└';
  } else if (IsTick(L[y][x]) && IsPipe(L[y - 1][x]) && IsHyphen(L[y][x + 1]) &&
             IsSpace(L[y + 1][x]) && IsSpace(L[y][x - 1])) {
    L[y][x] = u'└';
  } else if (IsHyphen(L[y][x])) {
    L[y][x] = u'─';
  } else if (IsPipe(L[y][x])) {
    L[y][x] = u'│';
  } else if (IsEquals(L[y][x])) {
    L[y][x] = u'═';
  } else {
    return;
  }
  DoIt(y - 1, x + 0);
  DoIt(y + 1, x + 0);
  DoIt(y + 0, x - 1);
  DoIt(y + 0, x + 1);
}

int main(int argc, char *argv[]) {
  char *s;
  int y, x;
  ShowCrashReports();
  f = stdin;
  while ((s = _chomp(xgetline(f)))) {
    n = strwidth(s, 0);
    xn = MAX(xn, n);
    T = xrealloc(T, ++yn * sizeof(*T));
    T[yn - 1] = s;
  }
  xn += 1000;
  L = xmalloc((yn + 2) * sizeof(*L));
  L[0] = utf8to16(_gc(xasprintf(" %*s ", xn, " ")), -1, 0);
  for (y = 0; y < yn; ++y) {
    s = xasprintf(" %s%*s ", T[y], xn - n, " ");
    L[y + 1] = utf8to16(s, -1, 0);
    free(T[y]);
    free(s);
  }
  L[yn + 2 - 1] = utf8to16(_gc(xasprintf(" %*s ", xn, " ")), -1, 0);
  free(T);
  V = xcalloc((yn + 1) * (xn + 1), 1);
  for (y = 1; y <= yn; ++y) {
    for (x = 1; x <= xn; ++x) {
      if (IsPipe(L[y - 1][x]) && IsHyphen(L[y][x - 1]) && IsPlus(L[y][x]) &&
          IsHyphen(L[y][x + 1]) && IsPipe(L[y + 1][x])) {
        DoIt(y, x);
      }
      if (IsTick(L[y][x]) && IsPipe(L[y - 1][x]) && IsHyphen(L[y][x + 1]) &&
          IsSpace(L[y + 1][x]) && IsSpace(L[y][x - 1])) {
        DoIt(y, x);
      }
    }
  }
  for (y = 1; y + 1 < yn; ++y) {
    s = utf16to8(L[y], -1, 0);
    n = strlen(s);
    while (n && s[n - 1] == ' ') s[n - 1] = 0, --n;
    puts(s + 1);
    free(s);
  }
  for (y = 0; y < yn; ++y) {
    free(L[y]);
  }
  free(L);
  free(V);
  return 0;
}
