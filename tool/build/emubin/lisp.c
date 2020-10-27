/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "tool/build/emubin/lisp.h"

#define TRACE  0  // print eval input output
#define RETRO  0  // auto capitalize input
#define DELETE 0  // allow backspace to rub out symbol
#define QUOTES 0  // allow 'X shorthand (QUOTE X)
#define PROMPT 0  // show repl prompt
#define WORD   short
#define WORDS  8192

/*───────────────────────────────────────────────────────────────────────────│─╗
│ The LISP Challenge § LISP Machine                                        ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define ATOM 0
#define CONS 1

#define NIL         0
#define UNDEFINED   8
#define ATOM_T      30
#define ATOM_QUOTE  34
#define ATOM_ATOM   46
#define ATOM_EQ     56
#define ATOM_COND   62
#define ATOM_CAR    72
#define ATOM_CDR    80
#define ATOM_CONS   88
#define ATOM_LAMBDA 98

#define BOOL(x)  ((x) ? ATOM_T : NIL)
#define VALUE(x) ((x) >> 1)
#define PTR(i)   ((i) << 1 | CONS)

struct Lisp {
  WORD mem[WORDS];
  unsigned char syntax[256];
  WORD look;
  WORD globals;
  WORD index;
  char token[128];
  char str[WORDS];
};

_Static_assert(sizeof(struct Lisp) <= 0x7c00 - 0x600,
               "LISP Machine too large for real mode");

_Alignas(char) const char kSymbols[] = "NIL\0"
                                       "*UNDEFINED\0"
                                       "T\0"
                                       "QUOTE\0"
                                       "ATOM\0"
                                       "EQ\0"
                                       "COND\0"
                                       "CAR\0"
                                       "CDR\0"
                                       "CONS\0"
                                       "LAMBDA\0";

#ifdef __REAL_MODE__
static struct Lisp *const q;
#else
static struct Lisp q[1];
#endif

static void Print(long);
static WORD GetList(void);
static WORD GetObject(void);
static void PrintObject(long);
static WORD Eval(long, long);

static void SetupSyntax(void) {
  unsigned char *syntax = q->syntax;
  asm("" : "+bSD"(syntax));
  syntax[' '] = ' ';
  syntax['\r'] = ' ';
  syntax['\n'] = ' ';
  syntax['('] = '(';
  syntax[')'] = ')';
  syntax['.'] = '.';
#if QUOTES
  syntax['\''] = '\'';
#endif
}

static void SetupBuiltins(void) {
  CopyMemory(q->str, kSymbols, sizeof(kSymbols));
}

static inline WORD Car(long x) {
  return PEEK_ARRAY(q, mem, VALUE(x), 0);
}

static inline WORD Cdr(long x) {
  return PEEK_ARRAY(q, mem, VALUE(x), 1);
}

static WORD Set(long i, long k, long v) {
  POKE_ARRAY(q, mem, VALUE(i), 0, k);
  POKE_ARRAY(q, mem, VALUE(i), 1, v);
  return i;
}

static WORD Cons(WORD car, WORD cdr) {
  int i, cell;
  i = q->index;
  POKE_ARRAY(q, mem, i, 0, car);
  POKE_ARRAY(q, mem, i, 1, cdr);
  q->index = i + 2;
  cell = OBJECT(CONS, i);
  return cell;
}

static char *StpCpy(char *d, char *s) {
  char c;
  do {
    c = LODS(s);  // a.k.a. c = *s++
    STOS(d, c);   // a.k.a. *d++ = c
  } while (c);
  return d;
}

static WORD Intern(char *s) {
  int j, cx;
  char c, *z, *t;
  z = q->str;
  c = LODS(z);
  while (c) {
    for (j = 0;; ++j) {
      if (c != PEEK(s, j, 0)) {
        break;
      }
      if (!c) {
        return OBJECT(ATOM, z - q->str - j - 1);
      }
      c = LODS(z);
    }
    while (c) c = LODS(z);
    c = LODS(z);
  }
  --z;
  StpCpy(z, s);
  return OBJECT(ATOM, SUB((long)z, q->str));
}

static unsigned char XlatSyntax(unsigned char b) {
  return PEEK_ARRAY(q, syntax, b, 0);
}

static void PrintString(char *s) {
  char c;
  for (;;) {
    if (!(c = PEEK(s, 0, 0))) break;
    PrintChar(c);
    ++s;
  }
}

static int GetChar(void) {
  int c;
  c = ReadChar();
#if RETRO
  if (c >= 'a') {
    CompilerBarrier();
    if (c <= 'z') c -= 'a' - 'A';
  }
#endif
#if DELETE
  if (c == '\b') return c;
#endif
  PrintChar(c);
  if (c == '\r') PrintChar('\n');
  return c;
}

static void GetToken(void) {
  char *t;
  unsigned char b, x;
  b = q->look;
  t = q->token;
  for (;;) {
    x = XlatSyntax(b);
    if (x != ' ') break;
    b = GetChar();
  }
  if (x) {
    STOS(t, b);
    b = GetChar();
  } else {
    while (b && !x) {
      if (!DELETE || b != '\b') {
        STOS(t, b);
      } else if (t > q->token) {
        PrintString("\b \b");
        if (t > q->token) --t;
      }
      b = GetChar();
      x = XlatSyntax(b);
    }
  }
  STOS(t, 0);
  q->look = b;
}

static WORD ConsumeObject(void) {
  GetToken();
  return GetObject();
}

static WORD Cadr(long x) {
  return Car(Cdr(x));  // ((A B C D) (E F G) H I) → (E F G)
}

static WORD List(long x, long y) {
  return Cons(x, Cons(y, NIL));
}

static WORD Quote(long x) {
  return List(ATOM_QUOTE, x);
}

static WORD GetQuote(void) {
  return Quote(ConsumeObject());
}

static WORD AddList(WORD x) {
  return Cons(x, GetList());
}

static WORD GetList(void) {
  GetToken();
  switch (*q->token & 0xFF) {
    default:
      return AddList(GetObject());
    case ')':
      return NIL;
    case '.':
      return ConsumeObject();
#if QUOTES
    case '\'':
      return AddList(GetQuote());
#endif
  }
}

static WORD GetObject(void) {
  switch (*q->token & 0xFF) {
    default:
      return Intern(q->token);
    case '(':
      return GetList();
#if QUOTES
    case '\'':
      return GetQuote();
#endif
  }
}

static WORD ReadObject(void) {
  q->look = GetChar();
  GetToken();
  return GetObject();
}

static WORD Read(void) {
  return ReadObject();
}

static void PrintAtom(long x) {
  PrintString(q->str + VALUE(x));
}

static void PrintList(long x) {
#if QUOTES
  if (Car(x) == ATOM_QUOTE) {
    PrintChar('\'');
    PrintObject(Cadr(x));
    return;
  }
#endif
  PrintChar('(');
  PrintObject(Car(x));
  while ((x = Cdr(x))) {
    if (!ISATOM(x)) {
      PrintChar(' ');
      PrintObject(Car(x));
    } else {
      PrintString(" . ");
      PrintObject(x);
      break;
    }
  }
  PrintChar(')');
}

static void PrintObject(long x) {
  if (ISATOM(x)) {
    PrintAtom(x);
  } else {
    PrintList(x);
  }
}

static void Print(long i) {
  PrintObject(i);
  PrintString("\r\n");
}

/*───────────────────────────────────────────────────────────────────────────│─╗
│ The LISP Challenge § Bootstrap John McCarthy's Metacircular Evaluator    ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

static WORD Atom(long x) {
  return BOOL(ISATOM(x));
}

WORD Eq(long x, long y) {
  return BOOL(x == y);
}

static WORD Caar(long x) {
  return Car(Car(x));  // ((A B C D) (E F G) H I) → A
}

static WORD Cdar(long x) {
  return Cdr(Car(x));  // ((A B C D) (E F G) H I) → (B C D)
}

static WORD Cadar(long x) {
  return Cadr(Car(x));  // ((A B C D) (E F G) H I) → B
}

static WORD Caddr(long x) {
  return Cadr(Cdr(x));  // ((A B C D) (E F G) H I) → H
}

static WORD Caddar(long x) {
  return Caddr(Car(x));  // ((A B C D) (E F G) H I) → C
}

static WORD Arg1(long e, long a) {
  return Eval(Cadr(e), a);
}

static WORD Arg2(long e, long a) {
  return Eval(Caddr(e), a);
}

static WORD Append(long x, long y) {
  return x ? Cons(Car(x), Append(Cdr(x), y)) : y;
}

static WORD Evcon(long c, long a) {
  return Eval(Caar(c), a) ? Eval(Cadar(c), a) : Evcon(Cdr(c), a);
}

static WORD Bind(long v, long a, long e) {
  return v ? Cons(Cons(Car(v), Eval(Car(a), e)), Bind(Cdr(v), Cdr(a), e)) : e;
}

static WORD Assoc(long x, long y) {
  return y ? Eq(Caar(y), x) ? Cdar(y) : Assoc(x, Cdr(y)) : NIL;
}

static WORD Pair(long x, long y) {
  if (Atom(x) || Atom(y)) {
    return NIL;
  } else {
    return Cons(Cons(Car(x), Car(y)), Pair(Cdr(x), Cdr(y)));
  }
}

__attribute__((__noinline__)) static WORD Evaluate(long e, long a) {
  if (Atom(e)) {
    return Assoc(e, a);
  } else if (Atom(Car(e))) {
    switch (Car(e)) {
      case NIL:
        return UNDEFINED;
      case ATOM_QUOTE:
        return Cadr(e);
      case ATOM_ATOM:
        return Atom(Arg1(e, a));
      case ATOM_EQ:
        return Eq(Arg1(e, a), Arg2(e, a));
      case ATOM_COND:
        return Evcon(Cdr(e), a);
      case ATOM_CAR:
        return Car(Arg1(e, a));
      case ATOM_CDR:
        return Cdr(Arg1(e, a));
      case ATOM_CONS:
        return Cons(Arg1(e, a), Arg2(e, a));
      default:
        return Eval(Cons(Assoc(Car(e), a), Cdr(e)), a);
    }
  } else if (Eq(Caar(e), ATOM_LAMBDA)) {
    return Eval(Caddar(e), Bind(Cadar(e), Cdr(e), a));
  } else {
    return UNDEFINED;
  }
}

static WORD Eval(long e, long a) {
  WORD r;
#if TRACE
  PrintString("->");
  Print(e);
  PrintString("  ");
  Print(a);
#endif
  e = Evaluate(e, a);
#if TRACE
  PrintString("<-");
  Print(e);
#endif
  return e;
}

/*───────────────────────────────────────────────────────────────────────────│─╗
│ The LISP Challenge § User Interface                                      ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

void Repl(void) {
  for (;;) {
#if PROMPT
    PrintString("* ");
#endif
    Print(Eval(Read(), q->globals));
  }
}

int main(int argc, char *argv[]) {
  RawMode();
  SetupSyntax();
  SetupBuiltins();
#if PROMPT
  PrintString("THE LISP CHALLENGE V1\r\n"
              "VISIT GITHUB.COM/JART\r\n");
#endif
  Repl();
  return 0;
}
