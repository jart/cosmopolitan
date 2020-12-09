#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "dsp/tty/tty.h"
#include "libc/alg/arraylist2.internal.h"
#include "libc/assert.h"
#include "libc/bits/bits.h"
#include "libc/bits/morton.h"
#include "libc/bits/popcnt.h"
#include "libc/calls/calls.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/fmt.h"
#include "libc/fmt/itoa.h"
#include "libc/limits.h"
#include "libc/log/color.internal.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/rand/rand.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/sig.h"
#include "libc/tinymath/emodl.h"
#include "libc/x/x.h"
#include "third_party/gdtoa/gdtoa.h"
#include "third_party/getopt/getopt.h"

#define INT     intmax_t
#define FLOAT   long double
#define EPSILON 1e-16l

#define BANNER \
  "\
Reverse Polish Notation Calculator\n\
Copyright 2020 Justine Alexandra Roberts Tunney\n\
This is fast portable lightweight software. You have the freedom to build\n\
software just like it painlessly. See http://github.com/jart/cosmopolitan\n\
"

#define USAGE1 \
  "SYNOPSIS\n\
\n\
  Reverse Polish Notation Calculator\n\
\n\
USAGE\n\
\n"

#define USAGE2 \
  " [FLAGS] [FILE...]\n\
\n\
FLAGS\n\
\n\
  -h\n\
  -?               shows this information\n\
  -i               force interactive mode\n\
\n\
KEYBOARD SHORTCUTS\n\
\n\
  CTRL-D           Closes standard input\n\
  CTRL-C           Sends SIGINT to program\n\
  CTRL-U           Redraw line\n\
  CTRL-L           Redraw display\n\
\n\
FUNCTIONS\n\
\n"

#define USAGE3 \
  "\n\
EXAMPLES\n\
\n\
  calculator.com\n\
  40 2 +\n\
  42\n\
\n\
  echo '2 2 + . cr' | calculator.com\n\
  4\n\
\n\
  calculator.com <<EOF\n\
  true assert\n\
  -1 ~ 0 = assert\n\
  3 2 / 1.5 = assert\n\
  81 3 // 27 = assert\n\
  2 8 ** 256 = assert\n\
  pi cos -1 = assert\n\
  pi sqrt pi sqrt * pi - abs epsilon < assert\n\
  nan isnormal ! assert\n\
  0b1010101 0b0110101 ^ 0b1100000 = assert\n\
  0b1010101 popcnt 4 = assert\n\
  EOF\n\
\n\
CONTACT\n\
\n\
  Justine Tunney <jtunney@gmail.com>\n\
  https://github.com/jart/cosmooplitan\n\
\n\
"

#define CTRL(C)      ((C) ^ 0100)
#define Fatal(...)   Log(kFatal, __VA_ARGS__)
#define Warning(...) Log(kWarning, __VA_ARGS__)

enum Severity {
  kFatal,
  kWarning,
};

enum Exception {
  kUnderflow = 1,
  kDivideError,
};

struct Bytes {
  size_t i, n;
  char *p;
};

struct History {
  size_t i, n;
  struct Bytes *p;
};

struct Value {
  enum Type {
    kInt,
    kFloat,
  } t;
  union {
    INT i;
    FLOAT f;
  };
};

struct Function {
  const char sym[16];
  void (*fun)(void);
  const char *doc;
};

jmp_buf thrower;
const char *file;
struct Bytes token;
struct History history;
struct Value stack[128];
int sp, comment, line, column, interactive;

INT Popcnt(INT x) {
  uintmax_t word = x;
  return popcnt(word >> 64) + popcnt(word);
}

char *Repr(struct Value x) {
  static char buf[64];
  if (x.t == kFloat) {
    g_xfmt_p(buf, &x.f, 16, sizeof(buf), 0);
  } else {
    sprintf(buf, "%jd", x.i);
  }
  return buf;
}

char *ReprStack(void) {
  int i, j, l;
  char *s, *p;
  static char buf[80];
  p = memset(buf, 0, sizeof(buf));
  for (i = 0; i < sp; ++i) {
    s = Repr(stack[i]);
    l = strlen(s);
    if (p + l + 2 > buf + sizeof(buf)) break;
    p = mempcpy(p, s, l);
    *p++ = ' ';
  }
  return buf;
}

void ShowStack(void) {
  if (interactive) {
    printf("\r\e[K");
    fputs(ReprStack(), stdout);
  }
}

void Cr(FILE *f) {
  fputs(interactive || IsWindows() ? "\r\n" : "\n", f);
}

void Log(enum Severity l, const char *fmt, ...) {
  va_list va;
  const char *severity[] = {"FATAL", "WARNING"};
  if (interactive) fflush(/* key triggering throw not echo'd yet */ stdout);
  fprintf(stderr, "%s:%d:%d:%s: ", file, line + 1, column, severity[l & 1]);
  va_start(va, fmt);
  vfprintf(stderr, fmt, va);
  va_end(va);
  Cr(stderr);
  if (l == kFatal) exit(EXIT_FAILURE);
  if (interactive) ShowStack();
}

void __on_arithmetic_overflow(void) {
  Warning("arithmetic overflow");
}

void OnDivideError(void) {
  longjmp(thrower, kDivideError);
}

struct Value Push(struct Value x) {
  if (sp >= ARRAYLEN(stack)) Fatal("stack overflow");
  return (stack[sp++] = x);
}

struct Value Pop(void) {
  if (sp) {
    return stack[--sp];
  } else {
    longjmp(thrower, kUnderflow);
  }
}

INT Popi(void) {
  struct Value x = Pop();
  return x.t == kInt ? x.i : x.f;
}

void Pushi(INT i) {
  struct Value x;
  x.t = kInt;
  x.i = i;
  Push(x);
}

FLOAT Popf(void) {
  struct Value x = Pop();
  return x.t == kInt ? x.i : x.f;
}

void Pushf(FLOAT f) {
  struct Value x;
  x.t = kFloat;
  x.f = f;
  Push(x);
}

void OpDrop(void) {
  Pop();
}

void OpDup(void) {
  Push(Push(Pop()));
}

void OpExit(void) {
  exit(Popi());
}

void OpSrand(void) {
  srand(Popi());
}

void OpEmit(void) {
  fputwc(Popi(), stdout);
}

void OpCr(void) {
  Cr(stdout);
}

void OpPrint(void) {
  printf("%s ", Repr(Pop()));
}

void OpComment(void) {
  comment = true;
}

void Glue0f(FLOAT fn(void)) {
  Pushf(fn());
}

void Glue0i(INT fn(void)) {
  Pushi(fn());
}

void Glue1f(FLOAT fn(FLOAT)) {
  Pushf(fn(Popf()));
}

void Glue1i(INT fn(INT)) {
  Pushi(fn(Popi()));
}

void OpSwap(void) {
  struct Value a, b;
  b = Pop();
  a = Pop();
  Push(b);
  Push(a);
}

void OpOver(void) {
  struct Value a, b;
  b = Pop();
  a = Pop();
  Push(a);
  Push(b);
  Push(a);
}

void OpKey(void) {
  wint_t c;
  ttyraw(kTtyCursor | kTtySigs | kTtyLfToCrLf);
  c = fgetwc(stdin);
  ttyraw(-1);
  if (c != -1) Pushi(c);
}

void OpAssert(void) {
  if (!Popi()) Fatal("assert failed");
}

void OpExpect(void) {
  if (!Popi()) Warning("expect failed");
}

void OpMeminfo(void) {
  OpCr();
  OpCr();
  fflush(stdout);
  meminfo(fileno(stdout));
}

void Glue2f(FLOAT fn(FLOAT, FLOAT)) {
  FLOAT x, y;
  y = Popf();
  x = Popf();
  Pushf(fn(x, y));
}

void Glue2i(INT fn(INT, INT)) {
  INT x, y;
  y = Popi();
  x = Popi();
  Pushi(fn(x, y));
}

void Glue1g(FLOAT fnf(FLOAT), INT fni(INT)) {
  struct Value x;
  x = Pop();
  switch (x.t) {
    case kInt:
      Pushi(fni(x.i));
      break;
    case kFloat:
      Pushf(fnf(x.f));
      break;
    default:
      Warning("type mismatch");
  }
}

void Glue2g(FLOAT fnf(FLOAT, FLOAT), INT fni(INT, INT)) {
  struct Value x, y;
  y = Pop();
  x = Pop();
  if (x.t == kInt && y.t == kInt) {
    Pushi(fni(x.i, y.i));
  } else if (x.t == kFloat && y.t == kFloat) {
    Pushf(fnf(x.f, y.f));
  } else if (x.t == kInt && y.t == kFloat) {
    Pushf(fnf(x.i, y.f));
  } else if (x.t == kFloat && y.t == kInt) {
    Pushf(fnf(x.f, y.i));
  } else {
    Warning("type mismatch");
  }
}

#define LB                  {
#define RB                  }
#define SEMI                ;
#define FNTYPEi             INT
#define FNTYPEf             FLOAT
#define FORM(F)             LB F SEMI RB
#define FNPL0(T)            void
#define FNPL1(T)            FNTYPE##T x
#define FNPL2(T)            FNTYPE##T x, FNTYPE##T y
#define FNDEF(A, T, S, C)   FNTYPE##T Fn##S##T(FNPL##A(T)) FORM(return C)
#define FNDEFf(A, S, C)     FNDEF(A, f, S, C)
#define FNDEFi(A, S, C)     FNDEF(A, i, S, C)
#define FNDEFg(A, S, C)     FNDEF(A, f, S, C) FNDEF(A, i, S, C)
#define OPDEF(A, T, S, C)   void Op##S(void) FORM(Glue##A##T(Fn##S##T))
#define OPDEFf(A, S, C)     OPDEF(A, f, S, C)
#define OPDEFi(A, S, C)     OPDEF(A, i, S, C)
#define OPDEFg(A, S, C)     void Op##S(void) FORM(Glue##A##g(Fn##S##f, Fn##S##i))
#define M(A, T, N, S, C, D) FNDEF##T(A, S, C) OPDEF##T(A, S, C)
#include "tool/build/calculator.inc"
#undef M

const struct Function kFunctions[] = {
    {".", OpPrint, "pops prints value repr"},
    {"#", OpComment, "line comment"},
#define M(A, T, N, S, C, D) {N, Op##S, D},
#include "tool/build/calculator.inc"
#undef M
    {"dup", OpDup, "pushes copy of last item on stack"},
    {"drop", OpDrop, "pops and discards"},
    {"swap", OpSwap, "swaps last two items on stack"},
    {"over", OpOver, "pushes second item on stack"},
    {"cr", OpCr, "prints newline"},
    {"key", OpKey, "reads and pushes unicode character from keyboard"},
    {"emit", OpEmit, "pops and writes unicode character to output"},
    {"assert", OpAssert, "crashes if top of stack isn't nonzero"},
    {"expect", OpExpect, "prints warning if top of stack isn't nonzero"},
    {"meminfo", OpMeminfo, "prints memory mappings"},
    {"exit", OpExit, "exits program with status"},
};

bool CallFunction(const char *sym) {
  int i;
  char s[16];
  strncpy(s, sym, sizeof(s));
  for (i = 0; i < ARRAYLEN(kFunctions); ++i) {
    if (memcmp(kFunctions[i].sym, s, sizeof(s)) == 0) {
      kFunctions[i].fun();
      return true;
    }
  }
  return false;
}

bool ConsumeLiteral(const char *literal) {
  char *e;
  struct Value x;
  x.t = kInt;
  x.i = strtoimax(literal, &e, 0);
  if (!e || *e) {
    x.t = kFloat;
    x.f = strtod(literal, &e);
    if (!e || *e) return false;
  }
  Push(x);
  return true;
}

void ConsumeToken(void) {
  enum Exception ex;
  if (!token.i) return;
  token.p[token.i] = 0;
  token.i = 0;
  if (history.i) history.p[history.i - 1].i = 0;
  if (comment) return;
  if (startswith(token.p, "#!")) return;
  switch (setjmp(thrower)) {
    default:
      if (CallFunction(token.p)) return;
      if (ConsumeLiteral(token.p)) return;
      Warning("bad token: %s", token.p);
      break;
    case kUnderflow:
      Warning("stack underflow");
      break;
    case kDivideError:
      Warning("divide error");
      break;
  }
}

void CleanupRepl(void) {
  if (sp && interactive) {
    Cr(stdout);
  }
}

void AppendByte(struct Bytes *a, char b) {
  APPEND(&a->p, &a->i, &a->n, &b);
}

void AppendHistory(void) {
  struct Bytes line;
  if (interactive) {
    memset(&line, 0, sizeof(line));
    APPEND(&history.p, &history.i, &history.n, &line);
  }
}

void AppendLine(char b) {
  if (interactive) {
    if (!history.i) AppendHistory();
    AppendByte(&history.p[history.i - 1], b);
  }
}

void Echo(int c) {
  if (interactive) {
    fputc(c, stdout);
  }
}

void Backspace(int c) {
  struct Bytes *line;
  if (interactive) {
    if (history.i) {
      line = &history.p[history.i - 1];
      if (line->i && token.i) {
        do {
          line->i--;
          token.i--;
        } while (line->i && token.i && (line->p[line->i] & 0x80));
        fputs("\e[D \e[D", stdout);
      }
    }
  }
}

void NewLine(void) {
  line++;
  column = 0;
  comment = false;
  if (interactive) {
    Cr(stdout);
    AppendHistory();
    ShowStack();
  }
}

void KillLine(void) {
  if (interactive) {
    if (history.i) {
      history.p[history.i - 1].i--;
    }
  }
}

void ClearLine(void) {
  if (interactive) {
    if (token.i) sp = 0;
    ShowStack();
  }
}

void RedrawDisplay(void) {
  int i;
  struct Bytes *line;
  if (interactive) {
    printf("\e[H\e[2J%s", BANNER);
    ShowStack();
    if (history.i) {
      line = &history.p[history.i - 1];
      for (i = 0; i < line->i; ++i) {
        fputc(line->p[i], stdout);
      }
    }
  }
}

void GotoStartOfLine(void) {
  if (interactive) {
    printf("\r");
  }
}

void GotoEndOfLine(void) {
}

void GotoPrevLine(void) {
}

void GotoNextLine(void) {
}

void GotoPrevChar(void) {
}

void GotoNextChar(void) {
}

void Calculator(FILE *f) {
  int c;
  while (!feof(f)) {
    switch ((c = getc(f))) {
      case -1:
      case CTRL('D'):
        goto Done;
      case CTRL('@'):
        break;
      case ' ':
        column++;
        Echo(c);
        AppendLine(c);
        ConsumeToken();
        break;
      case '\t':
        column = ROUNDUP(column, 8);
        Echo(c);
        AppendLine(c);
        ConsumeToken();
        break;
      case '\r':
      case '\n':
        ConsumeToken();
        NewLine();
        break;
      case CTRL('A'):
        GotoStartOfLine();
        break;
      case CTRL('E'):
        GotoEndOfLine();
        break;
      case CTRL('P'):
        GotoPrevLine();
        break;
      case CTRL('N'):
        GotoNextLine();
        break;
      case CTRL('B'):
        GotoPrevChar();
        break;
      case CTRL('F'):
        GotoNextChar();
        break;
      case CTRL('L'):
        RedrawDisplay();
        break;
      case CTRL('U'):
        ClearLine();
        break;
      case CTRL('K'):
        KillLine();
        break;
      case CTRL('?'):
      case CTRL('H'):
        Backspace(c);
        break;
      default:
        column++;
        /* fallthrough */
      case 0x80 ... 0xff:
        Echo(c);
        AppendLine(c);
        AppendByte(&token, c);
        break;
    }
    fflush(/* needed b/c this is event loop */ stdout);
  }
Done:
  CleanupRepl();
}

int Calculate(const char *path) {
  FILE *f;
  file = path;
  line = column = 0;
  if (!(f = fopen(file, "r"))) Fatal("file not found: %s", file);
  Calculator(f);
  return fclose(f);
}

void CleanupTerminal(void) {
  ttyraw(-1);
}

void StartInteractive(void) {
  if (!interactive && !isterminalinarticulate() && isatty(fileno(stdin)) &&
      isatty(fileno(stdout)) && cancolor()) {
    interactive = true;
  }
  if (interactive) {
    fputs(BANNER, stdout);
    fflush(/* needed b/c entering tty mode */ stdout);
    ttyraw(kTtyCursor | kTtySigs);
    atexit(CleanupTerminal);
  }
}

void PrintUsage(int rc, FILE *f) {
  char c;
  int i, j;
  fprintf(f, "%s  %s%s", USAGE1, program_invocation_name, USAGE2);
  for (i = 0; i < ARRAYLEN(kFunctions); ++i) {
    fputs("  ", f);
    for (j = 0; j < ARRAYLEN(kFunctions[i].sym); ++j) {
      c = kFunctions[i].sym[j];
      fputc(c ? c : ' ', f);
    }
    fprintf(f, " %s\n", kFunctions[i].doc);
  }
  fputs(USAGE3, f);
  exit(rc);
}

void GetOpts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "?hi")) != -1) {
    switch (opt) {
      case 'i':
        interactive = true;
        break;
      case 'h':
      case '?':
        PrintUsage(EXIT_SUCCESS, stdout);
      default:
        PrintUsage(EX_USAGE, stderr);
    }
  }
}

int main(int argc, char *argv[]) {
  int i, rc;
  showcrashreports();
  GetOpts(argc, argv);
  xsigaction(SIGFPE, OnDivideError, 0, 0, 0);
  if (optind == argc) {
    file = "/dev/stdin";
    StartInteractive();
    Calculator(stdin);
    return fclose(stdin);
  } else {
    for (i = optind; i < argc; ++i) {
      if (Calculate(argv[i]) == -1) {
        return -1;
      }
    }
  }
  return 0;
}
