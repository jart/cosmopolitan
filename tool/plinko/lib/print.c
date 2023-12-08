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
#include "tool/build/lib/case.h"
#include "tool/plinko/lib/char.h"
#include "tool/plinko/lib/plinko.h"
#include "tool/plinko/lib/print.h"
#include "tool/plinko/lib/tree.h"

int PrintDot(int fd) {
  return PrintChar(fd, L'.');
}

int PrintSpace(int fd) {
  return PrintChar(fd, L' ');
}

void PrintNewline(int fd) {
  PrintChar(fd, L'\n');
}

int PrintIndent(int fd, int n) {
  int i;
  for (i = 0; n > 0; --n) {
    i += PrintSpace(fd);
  }
  return i;
}

int PrintZeroes(int fd, int n) {
  int i;
  for (i = 0; n > 0; --n) {
    i += PrintChar(fd, L'0');
  }
  return i;
}

int PrintDepth(int fd, int n) {
  int i, j;
  for (j = i = 0; n > 0; --n, ++j) {
    if (j < ARRAYLEN(g_depths)) {
      i += PrintChar(fd, g_depths[j][0]);
      i += PrintChar(fd, g_depths[j][1]);
      i += PrintChar(fd, g_depths[j][2]);
    }
  }
  return i;
}

int PrintListDot(int fd, int x) {
  int n;
  n = PrintSpace(fd);
  n += PrintDot(fd);
  n += PrintSpace(fd);
  n += Print(fd, x);
  return n;
}

bool ShouldForceDot(int x) {
  return Cdr(x) < 0 && (Car(Cdr(x)) == kClosure || Car(Cdr(x)) == kLambda ||
                        Car(Cdr(x)) == kMacro);
}

bool ShouldConcealClosure(int x) {
  return !logc &&
         (Car(x) == kClosure /*  && Cdr(x) < 0 && Car(Cdr(x)) < 0 && */
          /* Car(Car(Cdr(x))) == kLambda && CountAtoms(x, 256, 0) >= 256 */);
}

int PrintList(int fd, int x) {
  int n = 0;
  DCHECK_LE(x, 0);
  if (x < cx) {
    n += PrintChar(fd, L'!');
    n += PrintInt(fd, x, 0, 0, 0, 10, true);
  } else {
    if (ShouldConcealClosure(x)) {
      x = Car(Cdr(x));
    }
    n += PrintChar(fd, L'(');
    if (x < 0) {
      n += Print(fd, Car(x));
      if (ShouldForceDot(x)) {
        n += PrintListDot(fd, Cdr(x));
      } else {
        while ((x = Cdr(x))) {
          if (x < 0) {
            n += PrintSpace(fd);
            n += Print(fd, Car(x));
          } else {
            n += PrintListDot(fd, x);
            break;
          }
        }
      }
    }
    n += PrintChar(fd, L')');
  }
  return n;
}

bool HasWeirdCharacters(int x) {
  int c;
  dword e;
  do {
    e = Get(x);
    c = LO(e);
    x = HI(e);
    if (c == L'`' || c == L'"' || c == L'\'' || IsSpace(c) || IsParen(c) ||
        IsSpace(c) || IsLower(c) || IsC0(c)) {
      return true;
    }
  } while (x != TERM);
  return false;
}

int PrintString(int fd, int x) {
  dword e;
  int c, u, n;
  n = PrintChar(fd, L'"');
  do {
    e = Get(x);
    c = LO(e);
    x = HI(e);
    DCHECK_GT(c, 0);
    DCHECK_LT(c, TERM);
    if (c < 0200) {
      u = c;
      c = L'\\';
      switch (c) {
        CASE(L'\a', c |= L'a' << 010);
        CASE(L'\b', c |= L'b' << 010);
        CASE(00033, c |= L'e' << 010);
        CASE(L'\f', c |= L'f' << 010);
        CASE(L'\n', c |= L'n' << 010);
        CASE(L'\r', c |= L'r' << 010);
        CASE(L'\t', c |= L't' << 010);
        CASE(L'\v', c |= L'v' << 010);
        CASE(L'\\', c |= L'\\' << 010);
        CASE(L'"', c |= L'"' << 010);
        default:
          if (32 <= c && c < 127) {
            c = u;
          } else {
            c |= ((L'0' + c / 8 / 8 % 8) << 010 | (L'0' + c / 8 % 8) << 020 |
                  (L'0' + c % 8) << 030);
          }
          break;
      }
      do {
        n += PrintChar(fd, c & 255);
      } while ((c >>= 010));
    } else {
      n += PrintChar(fd, c);
    }
  } while (x != TERM);
  n += PrintChar(fd, L'"');
  return n;
}

int PrintAtom(int fd, int x) {
  dword e;
  int c, n = 0;
  DCHECK_GE(x, 0);
  if (x == 0) {
    if (symbolism && !literally) {
      n += PrintChar(fd, L'⊥');
    } else {
      n += PrintChar(fd, L'N');
      n += PrintChar(fd, L'I');
      n += PrintChar(fd, L'L');
    }
  } else if (x == 1) {
    if (symbolism && !literally) {
      n += PrintChar(fd, L'⊤');
    } else {
      n += PrintChar(fd, L'T');
    }
  } else if (x >= TERM) {
    n += PrintChar(fd, L'!');
    n += PrintInt(fd, x, 0, 0, 0, 10, true);
  } else if (symbolism && (c = Symbolize(x)) != -1) {
    n += PrintChar(fd, c);
  } else {
    if (!literally && HasWeirdCharacters(x)) {
      n += PrintString(fd, x);
    } else {
      do {
        e = Get(x);
        c = LO(e);
        n += PrintChar(fd, c);
      } while ((x = HI(e)) != TERM);
    }
  }
  return n;
}

int EnterPrint(int x) {
#if 0
  int i;
  if (x < 0) {
    DCHECK_GE(pdp, 0);
    DCHECK_LT(pdp, ARRAYLEN(g_print));
    if (x < 0) {
      for (i = 0; i < pdp; ++i) {
        if (x == g_print[i]) {
          x = kCycle;
          break;
        }
      }
    }
    g_print[pdp++] = x;
    if (pdp == ARRAYLEN(g_print)) {
      x = kTrench;
    }
  }
#endif
  return x;
}

void LeavePrint(int x) {
#if 0
  if (x < 0) {
    --pdp;
  }
#endif
}

int Print(int fd, int x) {
  int n;
  GetName(&x);
  x = EnterPrint(x);
  if (1. / x < 0) {
    n = PrintList(fd, x);
  } else {
    n = PrintAtom(fd, x);
  }
  LeavePrint(x);
  return n;
}
