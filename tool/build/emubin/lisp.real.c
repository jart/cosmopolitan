/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/

#define TRACE  0
#define ERRORS 1
#define LONG   long
#define WORD   short
#define WORDS  2048

/*───────────────────────────────────────────────────────────────────────────│─╗
│ The LISP Challenge § 8086 PC BIOS / x86_64 Linux System Integration      ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define ATOM(x) /* a.k.a. !(x&1) */                          \
  ({                                                         \
    char IsAtom;                                             \
    asm("test%z1\t$1,%1" : "=@ccz"(IsAtom) : "Qm"((char)x)); \
    IsAtom;                                                  \
  })

#define OBJECT(t, v) /* a.k.a. v<<1|t */ \
  ({                                     \
    __typeof(v) Val = (v);               \
    asm("shl\t%0" : "+r"(Val));          \
    Val | (t);                           \
  })

#define SUB(x, y) /* a.k.a. x-y */           \
  ({                                         \
    __typeof(x) Reg = (x);                   \
    asm("sub\t%1,%0" : "+rm"(Reg) : "g"(y)); \
    Reg;                                     \
  })

#define STOS(di, c) asm("stos%z1" : "+D"(di), "=m"(*(di)) : "a"(c))
#define LODS(si)                                     \
  ({                                                 \
    typeof(*(si)) c;                                 \
    asm("lods%z2" : "+S"(si), "=a"(c) : "m"(*(si))); \
    c;                                               \
  })

#define REAL_READ(BASE, INDEX, DISP) /* a.k.a. b[i] */        \
  ({                                                          \
    __typeof(*(BASE)) Reg;                                    \
    if (__builtin_constant_p(INDEX) && !(INDEX)) {            \
      asm("mov\t%c2(%1),%0"                                   \
          : "=Q"(Reg)                                         \
          : "bDS"(BASE), "i"((DISP) * sizeof(*(BASE))));      \
    } else {                                                  \
      asm("mov\t%c3(%1,%2),%0"                                \
          : "=Q"(Reg)                                         \
          : "b"(BASE), "DS"((long)(INDEX) * sizeof(*(BASE))), \
            "i"((DISP) * sizeof(*(BASE))));                   \
    }                                                         \
    Reg;                                                      \
  })

#define REAL_READ_ARRAY_FIELD(OBJECT, MEMBER, INDEX, DISP) /* o->m[i] */  \
  ({                                                                      \
    __typeof(*(OBJECT->MEMBER)) Reg;                                      \
    if (!(OBJECT)) {                                                      \
      asm("mov\t%c2(%1),%0"                                               \
          : "=Q"(Reg)                                                     \
          : "bDS"((long)(INDEX) * sizeof(*(OBJECT->MEMBER))),             \
            "i"(__builtin_offsetof(__typeof(*(OBJECT)), MEMBER) +         \
                sizeof(*(OBJECT->MEMBER)) * (DISP)));                     \
    } else {                                                              \
      asm("mov\t%c3(%1,%2),%0"                                            \
          : "=Q"(Reg)                                                     \
          : "b"(OBJECT), "DS"((long)(INDEX) * sizeof(*(OBJECT->MEMBER))), \
            "i"(__builtin_offsetof(__typeof(*(OBJECT)), MEMBER) +         \
                sizeof(*(OBJECT->MEMBER)) * (DISP)));                     \
    }                                                                     \
    Reg;                                                                  \
  })

#define REAL_WRITE_ARRAY_FIELD(OBJECT, MEMBER, INDEX, DISP, VALUE)           \
  do {                                                                       \
    __typeof(*(OBJECT->MEMBER)) Reg;                                         \
    if (!(OBJECT)) {                                                         \
      asm volatile("mov\t%0,%c2(%1)"                                         \
                   : /* manual output */                                     \
                   : "Q"((__typeof(*(OBJECT->MEMBER)))(VALUE)),              \
                     "bDS"((long)(INDEX) * sizeof(*(OBJECT->MEMBER))),       \
                     "i"(__builtin_offsetof(__typeof(*(OBJECT)), MEMBER) +   \
                         sizeof(*(OBJECT->MEMBER)) * (DISP))                 \
                   : "memory");                                              \
    } else {                                                                 \
      asm volatile("mov\t%0,%c3(%1,%2)"                                      \
                   : /* manual output */                                     \
                   : "Q"((__typeof(*(OBJECT->MEMBER)))(VALUE)), "b"(OBJECT), \
                     "DS"((long)(INDEX) * sizeof(*(OBJECT->MEMBER))),        \
                     "i"(__builtin_offsetof(__typeof(*(OBJECT)), MEMBER) +   \
                         sizeof(*(OBJECT->MEMBER)) * (DISP))                 \
                   : "memory");                                              \
    }                                                                        \
  } while (0)

static void *SetMemory(void *di, int al, unsigned long cx) {
  asm("rep stosb"
      : "=D"(di), "=c"(cx), "=m"(*(char(*)[cx])di)
      : "0"(di), "1"(cx), "a"(al));
  return di;
}

static void *CopyMemory(void *di, void *si, unsigned long cx) {
  asm("rep movsb"
      : "=D"(di), "=S"(si), "=c"(cx), "=m"(*(char(*)[cx])di)
      : "0"(di), "1"(si), "2"(cx));
  return di;
}

static void RawMode(void) {
#ifndef __REAL_MODE__
  int rc;
  int c[14];
  asm volatile("syscall"
               : "=a"(rc)
               : "0"(0x10), "D"(0), "S"(0x5401), "d"(c)
               : "rcx", "r11", "memory");
  c[0] &= ~0b0000010111111000;  // INPCK|ISTRIP|PARMRK|INLCR|IGNCR|ICRNL|IXON
  c[2] &= ~0b0000000100110000;  // CSIZE|PARENB
  c[2] |= 0b00000000000110000;  // CS8
  c[3] &= ~0b1000000001011010;  // ECHONL|ECHO|ECHOE|IEXTEN|ICANON
  asm volatile("syscall"
               : "=a"(rc)
               : "0"(0x10), "D"(0), "S"(0x5402), "d"(c)
               : "rcx", "r11", "memory");
#endif
}

__attribute__((__noinline__)) static int PrintChar(LONG c) {
#ifdef __REAL_MODE__
  asm volatile("mov\t$0x0E,%%ah\n\t"
               "int\t$0x10"
               : /* no outputs */
               : "a"(c), "b"(7)
               : "memory");
  return 0;
#else
  static short buf;
  int rc;
  buf = c;
  asm volatile("syscall"
               : "=a"(rc)
               : "0"(1), "D"(1), "S"(&buf), "d"(1)
               : "rcx", "r11", "memory");
  return rc;
#endif
}

static void PrintString(char *s) {
  char c;
  for (;;) {
    if (!(c = REAL_READ(s, 0, 0))) break;
    PrintChar(c);
    ++s;
  }
}

static int XlatChar(LONG c) {
  if (c >= 'a') {
    asm volatile("" ::: "memory");
    if (c <= 'z') c -= 'a' - 'A';
  }
  return c;
}

static int EchoChar(LONG c) {
  if (c == '\b' || c == 0x7F) {
    PrintString("\b \b");
    return '\b';
  } else {
    PrintChar(c);
    if (c == '\r') {
      PrintChar('\n');
    }
    return c;
  }
}

static noinline int ReadChar(void) {
  int c;
#ifdef __REAL_MODE__
  asm volatile("int\t$0x16" : "=a"(c) : "0"(0) : "memory");
#else
  static int buf;
  asm volatile("syscall"
               : "=a"(c)
               : "0"(0), "D"(0), "S"(&buf), "d"(1)
               : "rcx", "r11", "memory");
  c = buf;
#endif
  return EchoChar(XlatChar(c));
}

/*───────────────────────────────────────────────────────────────────────────│─╗
│ The LISP Challenge § LISP Machine                                        ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define TYPE_ATOM 0
#define TYPE_CONS 1

#define ATOM_NIL    0
#define ATOM_T      8
#define ATOM_QUOTE  12
#define ATOM_ATOM   24
#define ATOM_EQ     34
#define ATOM_COND   40
#define ATOM_CAR    50
#define ATOM_CDR    58
#define ATOM_CONS   66
#define ATOM_LABEL  76
#define ATOM_LAMBDA 88
#define ATOM_SET    102
#define ATOM_DEFUN  110

#define Quote(x)   List(ATOM_QUOTE, x)
#define List(x, y) Cons(x, Cons(y, ATOM_NIL))
#define Caar(x)    Car(Car(x))    // ((A B C D) (E F G) H I) → A
#define Cdar(x)    Cdr(Car(x))    // ((A B C D) (E F G) H I) → (B C D)
#define Cadar(x)   Cadr(Car(x))   // ((A B C D) (E F G) H I) → B
#define Caddar(x)  Caddr(Car(x))  // ((A B C D) (E F G) H I) → C
#define Cadr(x)    Car(Cdr(x))    // ((A B C D) (E F G) H I) → (E F G)
#define Caddr(x)   Cadr(Cdr(x))   // ((A B C D) (E F G) H I) → H

#define BOOL(x)  ((x) ? ATOM_T : ATOM_NIL)
#define VALUE(x) ((x) >> 1)

struct Lisp {
  WORD memory[WORDS];
  unsigned char syntax[256];
  unsigned char look;
  char token[16];
  WORD globals;
  int index;
  char str[WORDS];
};

const char kSymbols[] aligned(1) = "\
NIL\0T\0QUOTE\0ATOM\0EQ\0COND\0CAR\0CDR\0CONS\0LABEL\0LAMBDA\0SET\0DEFUN\0";

#ifdef __REAL_MODE__
static struct Lisp *const q;
#else
static struct Lisp q[1];
#endif

static void Print(LONG);
static WORD GetList(void);
static WORD GetObject(void);
static void PrintObject(LONG);
static WORD Eval(LONG, LONG);

static void SetupSyntax(void) {
  q->syntax[' '] = ' ';
  q->syntax['\t'] = ' ';
  q->syntax['\r'] = ' ';
  q->syntax['\n'] = ' ';
  q->syntax['('] = '(';
  q->syntax[')'] = ')';
  q->syntax['.'] = '.';
  q->syntax['\''] = '\'';
}

forceinline WORD Car(LONG x) {
  return REAL_READ_ARRAY_FIELD(q, memory, VALUE(x), 0);
}

forceinline WORD Cdr(LONG x) {
  return REAL_READ_ARRAY_FIELD(q, memory, VALUE(x), 1);
}

static WORD Cons(WORD car, WORD cdr) {
  int i, c;
  i = q->index;
  REAL_WRITE_ARRAY_FIELD(q, memory, i, 0, car);
  REAL_WRITE_ARRAY_FIELD(q, memory, i, 1, cdr);
  q->index += 2;
  c = OBJECT(TYPE_CONS, i);
  return c;
}

static void SetupBuiltins(void) {
  CopyMemory(q->str, kSymbols, sizeof(kSymbols));
  q->globals =
      Cons(Cons(ATOM_NIL, ATOM_NIL), Cons(Cons(ATOM_T, ATOM_T), ATOM_NIL));
}

static char *StpCpy(char *d, char *s) {
  char c;
  do {
    c = LODS(s); /* a.k.a. c = *s++; */
    STOS(d, c);  /* a.k.a. *d++ = c; */
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
      if (c != REAL_READ(s, j, 0)) {
        break;
      }
      if (!c) {
        return OBJECT(TYPE_ATOM, z - q->str - j - 1);
      }
      c = LODS(z);
    }
    while (c) c = LODS(z);
    c = LODS(z);
  }
  --z;
  StpCpy(z, s);
  return OBJECT(TYPE_ATOM, SUB((long)z, q->str));
}

forceinline unsigned char XlatSyntax(unsigned char b) {
  return REAL_READ_ARRAY_FIELD(q, syntax, b, 0); /* a.k.a. q->syntax[b] */
}

static void GetToken(void) {
  char *t;
  unsigned char b;
  b = q->look;
  t = q->token;
  while (XlatSyntax(b) == ' ') {
    b = ReadChar();
  }
  if (XlatSyntax(b)) {
    STOS(t, b);
    b = ReadChar();
  } else {
    while (b && !XlatSyntax(b)) {
      if (b != '\b') {
        STOS(t, b);
      } else if (t > q->token) {
        --t;
      }
      b = ReadChar();
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
    case '\'':
      return AddList(GetQuote());
    case ')':
      return ATOM_NIL;
    case '.':
      return ConsumeObject();
  }
}

static WORD GetObject(void) {
  switch (*q->token & 0xFF) {
    default:
      return Intern(q->token);
    case '\'':
      return GetQuote();
    case '(':
      return GetList();
  }
}

static WORD ReadObject(void) {
  q->look = ReadChar();
  GetToken();
  return GetObject();
}

static WORD Read(void) {
  return ReadObject();
}

static void PrintAtom(LONG x) {
  PrintString(q->str + VALUE(x));
}

static void PrintList(LONG x) {
  PrintChar('(');
  PrintObject(Car(x));
  while ((x = Cdr(x))) {
    if (!ATOM(x)) {
      PrintChar(' ');
      PrintObject(Car(x));
    } else {
      PrintString(" . ");
      PrintObject(x);
    }
  }
  PrintChar(')');
}

static void PrintObject(LONG x) {
  if (ATOM(x)) {
    PrintAtom(x);
  } else {
    PrintList(x);
  }
}

static void Print(LONG i) {
  PrintObject(i);
  PrintString("\r\n");
}

__attribute__((__noreturn__)) static void Reset(void) {
  asm volatile("jmp\tRepl");
  __builtin_unreachable();
}

__attribute__((__noreturn__)) static void OnUndefined(LONG x) {
  PrintString("UNDEF! ");
  Print(x);
  Reset();
}

__attribute__((__noreturn__)) static void OnArity(void) {
  PrintString("ARITY!\n");
  Reset();
}

#if !ERRORS
#define OnUndefined(x) __builtin_unreachable()
#define OnArity()      __builtin_unreachable()
#endif

/*───────────────────────────────────────────────────────────────────────────│─╗
│ The LISP Challenge § Bootstrap John McCarthy's Metacircular Evaluator    ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

static WORD Atom(LONG x) {
  return BOOL(ATOM(x));
}

static WORD Null(LONG x) {
  return BOOL(!x);
}

static WORD Eq(LONG x, LONG y) {
  return BOOL(x == y); /* undefiled if !ATOM(x)||!ATOM(y) */
}

static WORD Assoc(LONG x, LONG y) {
  for (;;) {
    if (!y) OnUndefined(x);
    if (Eq(Caar(y), x)) break;
    y = Cdr(y);
  }
  return Cdar(y);
}

static WORD Append(LONG x, LONG y) {
  if (x) {
    return Cons(Car(x), Append(Cdr(x), y));
  } else {
    return y;
  }
}

/**
 * Gives list of pairs of corresponding elements of the lists x and y.
 * E.g. pair[(A,B,C);(X,(Y,Z),U)] = ((A.X),(B.(Y,Z)),(C.U))
 * @note recoded to make lists in dot notation
 * @note it's zip() basically
 */
static WORD Pair(LONG x, LONG y) {
  if (!x && !y) {
    return ATOM_NIL;
  } else if (!ATOM(x) && !ATOM(y)) {
    return Cons(Cons(Car(x), Car(y)), Pair(Cdr(x), Cdr(y)));
  } else {
    OnArity();
  }
}

static WORD Appq(long m) {
  if (m) {
    return Cons(List(ATOM_QUOTE, Car(m)), Appq(Cdr(m)));
  } else {
    return ATOM_NIL;
  }
}

static WORD Apply(long f, long a) {
  return Eval(Cons(f, Appq(a)), ATOM_NIL);
}

static WORD Evcon(LONG c, LONG a) {
  if (Eval(Caar(c), a)) {
    return Eval(Cadar(c), a);
  } else {
    return Evcon(Cdr(c), a);
  }
}

static WORD Evlis(LONG m, LONG a) {
  if (m) {
    return Cons(Eval(Car(m), a), Evlis(Cdr(m), a));
  } else {
    return ATOM_NIL;
  }
}

static WORD Set(LONG e) {
  WORD name, value;
  name = Car(e);
  value = Cadr(e);
  q->globals = Cons(Cons(name, value), q->globals);
  return value;
}

static WORD Defun(LONG e) {
  WORD name, args, body, lamb;
  name = Car(e);
  args = Cadr(e);
  body = Caddr(e);
  lamb = Cons(ATOM_LAMBDA, List(args, body));
  q->globals = Cons(Cons(name, lamb), q->globals);
  return name;
}

static WORD Evaluate(LONG e, LONG a) {
  if (ATOM(e)) {
    return Assoc(e, a);
  } else if (ATOM(Car(e))) {
    switch (Car(e)) {
      case ATOM_QUOTE:
        return Cadr(e);
      case ATOM_ATOM:
        return Atom(Eval(Cadr(e), a));
      case ATOM_EQ:
        return Eq(Eval(Cadr(e), a), Eval(Caddr(e), a));
      case ATOM_COND:
        return Evcon(Cdr(e), a);
      case ATOM_CAR:
        return Car(Eval(Cadr(e), a));
      case ATOM_CDR:
        return Cdr(Eval(Cadr(e), a));
      case ATOM_CONS:
        return Cons(Eval(Cadr(e), a), Eval(Caddr(e), a));
      case ATOM_DEFUN:
        return Defun(Cdr(e));
      case ATOM_SET:
        return Set(Cdr(e));
      default:
        return Eval(Cons(Assoc(Car(e), a), Evlis(Cdr(e), a)), a);
    }
  } else if (Eq(Caar(e), ATOM_LABEL)) {
    return Eval(Cons(Caddar(e), Cdr(e)), Cons(Cons(Cadar(e), Car(e)), a));
  } else if (Eq(Caar(e), ATOM_LAMBDA)) {
    return Eval(Caddar(e), Append(Pair(Cadar(e), Evlis(Cdr(e), a)), a));
  } else {
    OnUndefined(Caar(e));
  }
}

static WORD Eval(LONG e, LONG a) {
#if TRACE
  PrintString("->");
  Print(e);
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
    PrintString("* ");
    Print(Eval(Read(), q->globals));
  }
}

int main(int argc, char *argv[]) {
  RawMode();
  SetupSyntax();
  SetupBuiltins();
  PrintString("THE LISP CHALLENGE V1\r\n"
              "VISIT GITHUB.COM/JART\r\n");
  Repl();
  return 0;
}
