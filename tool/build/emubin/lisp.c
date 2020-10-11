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

#define TRACE   0  // print eval input output
#define RETRO   1  // auto capitalize input
#define ERRORS  1  // print messages or undefined behavior
#define DELETE  1  // allow backspace to rub out symbol
#define QUOTES  1  // allow 'X shorthand (QUOTE X)
#define MUTABLE 0  // allow setting globals
#define PROMPT  1  // show repl prompt
#define WORD    short
#define WORDS   8192

/*───────────────────────────────────────────────────────────────────────────│─╗
│ The LISP Challenge § LISP Machine                                        ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define ATOM 0
#define CONS 1

#define NIL         0
#define ATOM_T      8
#define ATOM_QUOTE  12
#define ATOM_ATOM   24
#define ATOM_EQ     34
#define ATOM_COND   40
#define ATOM_CAR    50
#define ATOM_CDR    58
#define ATOM_CONS   66
#define ATOM_LAMBDA 76
#define ATOM_SET    90

#define Quote(x)   List(ATOM_QUOTE, x)
#define List(x, y) Cons(x, Cons(y, NIL))
#define Caar(x)    Car(Car(x))    // ((A B C D) (E F G) H I) → A
#define Cdar(x)    Cdr(Car(x))    // ((A B C D) (E F G) H I) → (B C D)
#define Cadar(x)   Cadr(Car(x))   // ((A B C D) (E F G) H I) → B
#define Caddar(x)  Caddr(Car(x))  // ((A B C D) (E F G) H I) → C
#define Cadr(x)    Car(Cdr(x))    // ((A B C D) (E F G) H I) → (E F G)
#define Caddr(x)   Cadr(Cdr(x))   // ((A B C D) (E F G) H I) → H

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
  long jb[8];
  char str[WORDS];
};

_Static_assert(sizeof(struct Lisp) <= 0x7c00 - 0x600,
               "LISP Machine too large for real mode");

_Alignas(char) const char kSymbols[] = "NIL\0"
                                       "T\0"
                                       "QUOTE\0"
                                       "ATOM\0"
                                       "EQ\0"
                                       "COND\0"
                                       "CAR\0"
                                       "CDR\0"
                                       "CONS\0"
                                       "LAMBDA\0"
                                       "SET\0";

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

static void SetupBuiltins(void) {
  CopyMemory(q->str, kSymbols, sizeof(kSymbols));
  q->mem[0] = PTR(2);
  q->globals = PTR(0);
  q->index = 4;
}

static char *StpCpy(char *d, char *s) {
  char c;
  do {
    c = LODS(s); /* a.k.a. c = *s++; */
    STOS(d, c);  /* a.k.a. *d++ = c; */
  } while (c);
  return d;
}

WORD Intern(char *s) {
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

forceinline unsigned char XlatSyntax(unsigned char b) {
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
  unsigned char b;
  b = q->look;
  t = q->token;
  while (XlatSyntax(b) == ' ') {
    b = GetChar();
  }
  if (XlatSyntax(b)) {
    STOS(t, b);
    b = GetChar();
  } else {
    while (b && !XlatSyntax(b)) {
      if (!DELETE || b != '\b') {
        STOS(t, b);
      } else if (t > q->token) {
        PrintString("\b \b");
        if (t > q->token) --t;
      }
      b = GetChar();
    }
  }
  STOS(t, 0);
  q->look = b;
}

static WORD ConsumeObject(void) {
  GetToken();
  return GetObject();
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
    if (TYPE(x) == CONS) {
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
  if (TYPE(x) == ATOM) {
    PrintAtom(x);
  } else {
    PrintList(x);
  }
}

static void Print(long i) {
  PrintObject(i);
  PrintString("\r\n");
}

__attribute__((__noreturn__)) static void Reset(void) {
  longjmp(q->jb, 1);
}

__attribute__((__noreturn__)) static void OnArity(void) {
  PrintString("ARITY!\n");
  Reset();
}

__attribute__((__noreturn__)) static void OnUndefined(long x) {
  PrintString("UNDEF! ");
  Print(x);
  Reset();
}

#if !ERRORS
#define OnArity()      __builtin_unreachable()
#define OnUndefined(x) __builtin_unreachable()
#endif

/*───────────────────────────────────────────────────────────────────────────│─╗
│ The LISP Challenge § Bootstrap John McCarthy's Metacircular Evaluator    ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

static WORD Atom(long x) {
  return BOOL(TYPE(x) == ATOM);
}

static WORD Null(long x) {
  return BOOL(!x);
}

static WORD Eq(long x, long y) {
  return BOOL(x == y);
}

static WORD Arg1(long e, long a) {
  return Eval(Cadr(e), a);
}

static WORD Arg2(long e, long a) {
  return Eval(Caddr(e), a);
}

static WORD Append(long x, long y) {
  return Null(x) ? y : Cons(Car(x), Append(Cdr(x), y));
}

static WORD Evcon(long c, long a) {
  return Eval(Caar(c), a) ? Eval(Cadar(c), a) : Evcon(Cdr(c), a);
}

static WORD Evlis(long m, long a) {
  return m ? Cons(Eval(Car(m), a), Evlis(Cdr(m), a)) : NIL;
}

static WORD Assoc(long x, long y) {
  if (!y) OnUndefined(x);
  return Eq(Caar(y), x) ? Cdar(y) : Assoc(x, Cdr(y));
}

static WORD Pair(long x, long y) {
  if (Null(x) && Null(y)) {
    return NIL;
  } else if (!Atom(x) && !Atom(y)) {
    return Cons(Cons(Car(x), Car(y)), Pair(Cdr(x), Cdr(y)));
  } else {
    OnArity();
  }
}

static WORD Evaluate(long e, long a) {
  if (Atom(e)) {
    return Assoc(e, a);
  } else if (Atom(Car(e))) {
    switch (Car(e)) {
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
#if MUTABLE
      case ATOM_SET:
        return Cdar(Set(a, Cons(Arg1(e, a), Arg2(e, a)), Cons(Car(a), Cdr(a))));
#endif
      default:
        return Eval(Cons(Assoc(Car(e), a), Evlis(Cdr(e), a)), a);
    }
  } else if (Eq(Caar(e), ATOM_LAMBDA)) {
    return Eval(Caddar(e), Append(Pair(Cadar(e), Evlis(Cdr(e), a)), a));
  } else {
    OnUndefined(Caar(e));
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
#if ERRORS
  setjmp(q->jb);
#endif
  for (;;) {
#if PROMPT
    PrintString("* ");
#endif
    Print(Eval(Read(), q->globals));
  }
}

int main(int argc, char *argv[]) {
  /* RawMode(); */
  SetupSyntax();
  SetupBuiltins();
#if PROMPT
  PrintString("THE LISP CHALLENGE V1\r\n"
              "VISIT GITHUB.COM/JART\r\n");
#endif
  Repl();
  return 0;
}
