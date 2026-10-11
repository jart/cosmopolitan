#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include <errno.h>
#include <string.h>
#include <wchar.h>
#include "char.h"
#include "cons.h"
#include "error.h"
#include "plinko.h"
#include "printf.h"

static int Read1(void);
static int Read2(void);

static FILE *g_input;

int ReadRune(void) {
  wint_t c;
  for (;;) {
    if (!g_input) {
      if (*inputs) {
        if (!(g_input = fopen(*inputs++, "r"))) {
          ++fails;
          Fprintf(2, L"error: open(%s) %s%n", inputs[-1], strerror(errno));
          longjmp(exiter, 1);
        }
      } else if (wantrepl) {
        g_input = stdin;
      } else {
        longjmp(exiter, 1);
      }
    }
    if ((c = fgetwc(g_input)) != WEOF)
      return c;
    if (!feof(g_input)) {
      ++fails;
      Fprintf(2, L"error: read() %s%n", strerror(errno));
      longjmp(exiter, 1);
    }
    if (g_input == stdin)
      longjmp(exiter, 1);
    fclose(g_input);
    g_input = 0;
  }
}

static int ReadChar(void) {
  int a = dx;
  while ((dx = ReadRune()) == ';') {
    while (ReadRune() != '\n') {
    }
  }
  if (0 < a && a < TERM) {
    return ToUpper(a);
  }
  Error(L"character outside permitted range");
}

static int ReadListItem(int closer, int f(void)) {
  int x, y;
  if ((x = f()) > 0) {
    if (Get(x) == MAKE(closer, TERM))
      return -0;
    if (Get(x) == MAKE(L'.', TERM)) {
      x = f();
      if ((y = ReadListItem(closer, Read1))) {
        Error(L"multiple list items after dot: %S", y);
      }
      return x;
    }
  }
  return ShareCons(x, ReadListItem(closer, Read1));
}

static int ReadList(int closer) {
  int t;
  ++fails;
  t = ReadListItem(closer, Read2);
  --fails;
  return t;
}

static int TokenizeInteger(int b) {
  dword a;
  int c, i;
  (void)i;
  for (i = a = 0;; ++i) {
    if ((c = GetDiglet(ToUpper(dx))) != -1 && c < b) {
      a = (a * b) + c;
      ReadChar();
    } else {
      ax = TERM;
      return Intern(a, TERM);
    }
  }
}

static void ConsumeComment(void) {
  int c, t = 1;
  for (;;) {
    c = ReadChar();
    if (c == '#' && dx == '|')
      ++t;
    if (!t)
      return;
    if (c == '|' && dx == '#')
      --t;
  }
}

static int ReadAtomRest(int x) {
  int y;
  ax = y = TERM;
  if (x == L'\\')
    x = ReadChar();
  if (!IsSpace(dx) && !IsParen(dx) && !IsMathAlnum(x) && !IsMathAlnum(dx)) {
    y = ReadAtomRest(ReadChar());
  }
  return Intern(x, y);
}

static int ReadAtom(void) {
  int a, s, x;
  x = ReadChar();
  if ((s = Desymbolize(x)) != -1)
    return s;
  a = ReadAtomRest(x);
  if (LO(Get(a)) == L'T' && HI(Get(a)) == TERM) {
    a = 1;
  } else if (LO(Get(a)) == L'N' && HI(Get(a)) != TERM &&
             LO(Get(HI(Get(a)))) == L'I' && HI(Get(HI(Get(a)))) != TERM &&
             LO(Get(HI(Get(HI(Get(a)))))) == L'L' &&
             HI(Get(HI(Get(HI(Get(a)))))) == TERM) {
    a = 0;
  }
  return a;
}

static int TokenizeComplicated(void) {
  int c;
  ReadChar();
  switch ((c = ReadChar())) {
    case L'\'':
      return List(kFunction, Read());
    case L'B':
      return TokenizeInteger(2);
    case L'X':
      return TokenizeInteger(16);
    case L'Z':
      return TokenizeInteger(36);
    case L'O':
      return TokenizeInteger(8);
    case L'|':
      ConsumeComment();
      return Read();
    default:
      Error(L"unsuppported complicated syntax #%c [0x%x]", c, c);
  }
}

static int Read2(void) {
  int r, l;
  while (IsSpace((l = dx)))
    ReadChar();
  switch (dx) {
    case L'#':
      r = TokenizeComplicated();
      break;
    case L'\'':
      ReadChar();
      r = ShareList(kQuote, Read());
      break;
    case L'`':
      ReadChar();
      r = ShareList(kBackquote, Read());
      break;
    case L',':
      ReadChar();
      if (dx == L'@') {
        ReadChar();
        r = ShareList(kSplice, Read());
      } else {
        r = ShareList(kComma, Read());
      }
      break;
    case L'"':
      r = ShareList(kString, ReadString(ReadRune()));
      break;
    case L'(':
      ReadChar();
      r = ReadList(L')');
      break;
    case L'[':
      ReadChar();
      r = ShareList(kSquare, ReadList(L']'));
      break;
    case L'{':
      ReadChar();
      r = ShareList(kCurly, ReadList(L'}'));
      break;
    default:
      r = ReadAtom();
      break;
  }
  return r;
}

static int Read1(void) {
  while (IsSpace(dx))
    ReadChar();
  return Read2();
}

int Read(void) {
  int r;
  r = Read1();
  return r;
}
