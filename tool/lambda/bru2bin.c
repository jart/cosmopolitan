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
#include "libc/calls/calls.h"
#include "libc/intrin/kprintf.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/locale.h"
#include "libc/str/str.h"
#include "third_party/getopt/getopt.internal.h"

#define USAGE \
  " [-?h01] <lambda.txt >binary.txt\n\
Converts de Bruijn notation to ASCII binary, e.g.\n\
\n\
    $ printf 'λ (λ 1 (0 0)) (λ 1 (0 0)))' | lam2bin\n\
    000100011100110100001110011010\n\
\n\
FLAGS\n\
\n\
  -h      Help\n\
  -?      Help\n\
  -0      0-based indexing\n\
  -1      1-based indexing\n"

struct Node {
  int t, x;
  struct Node *l, *r;
};

int sp;
int end;
int unget;
int indexing;
const char *str;

static void LoadFlags(int argc, char *argv[]) {
  int i;
  const char *prog;
  prog = argc ? argv[0] : "lam2bin";
  while ((i = getopt(argc, argv, "?h01")) != -1) {
    switch (i) {
      case '0':
        indexing = 0;
        break;
      case '1':
        indexing = 1;
        break;
      case '?':
      case 'h':
        fputs("Usage: ", stdout);
        fputs(prog, stdout);
        fputs(USAGE, stdout);
        exit(0);
      default:
        fputs("Usage: ", stderr);
        fputs(prog, stderr);
        fputs(USAGE, stderr);
        exit(1);
    }
  }
}

wontreturn static void Error(int rc, const char *s, ...) {
  va_list va;
  fflush(stdout);
  fputs("\33[1;31merror\33[37m: ", stderr);
  fflush(stderr);
  va_start(va, s);
  kvprintf(s, va);
  va_end(va);
  fputc('\n', stderr);
  exit(rc);
}

static struct Node *NewNode(int t, int x, struct Node *l, struct Node *r) {
  struct Node *n;
  n = malloc(sizeof(struct Node));
  n->t = t;
  n->x = x;
  n->l = l;
  n->r = r;
  return n;
}

static int Greed(void) {
  int c, t;
  for (t = 0;;) {
    if (unget) {
      c = unget;
      unget = 0;
    } else if (str) {
      if (*str) {
        c = *str++;
      } else {
        str = 0;
        c = fgetwc(stdin);
      }
    } else {
      c = fgetwc(stdin);
    }
    if (c == EOF) return c;
    if (!t) {
      if (c == '#' || c == ';') {
        t = 1;
        continue;
      }
    } else {
      if (c == '\n') {
        t = 0;
      }
      continue;
    }
    if (!str) {
      switch (c) {
        case L'⊥':
          str = "(\\ab.b)";
          continue;
        case L'⊤':
          str = "(\\ab.a)";
          continue;
#if 0
        case L'0':
          str = "(\\ab.b)";
          continue;
        case L'1':
          str = "(\\ab.ab)";
          continue;
        case L'2':
          str = "(\\ab.a(ab))";
          continue;
        case L'3':
          str = "(\\ab.a(a(ab)))";
          continue;
        case L'4':
          str = "(\\ab.a(a(a(ab))))";
          continue;
        case L'5':
          str = "(\\ab.a(a(a(a(ab)))))";
          continue;
        case L'6':
          str = "(\\ab.a(a(a(a(a(ab))))))";
          continue;
        case L'7':
          str = "(\\ab.a(a(a(a(a(a(ab)))))))";
          continue;
        case L'8':
          str = "(\\ab.a(a(a(a(a(a(a(ab))))))))";
          continue;
        case L'9':
          str = "(\\ab.a(a(a(a(a(a(a(a(ab)))))))))";
          continue;
#endif
        case L'ω':
          str = "(\\x.xx)";
          continue;
        case L'Ω':
          str = "((\\x.xx)(\\x.xx))";
          continue;
        case L'Y':
          str = "(\\f.(\\x.f(xx))(\\x.f(xx)))";
          continue;
        case L'∧':
          str = "(\\ab.aba)";
          continue;
        case L'∨':
          str = "(\\ab.aab)";
          continue;
        case L'⊻':
          str = "(\\ab.a((\\c.c(\\de.e)(\\de.d))b)b)";
          continue;
        case L'¬':
          str = "(\\a.a(\\bc.c)(\\bc.b))";
          continue;
        case L'+':
          str = "(\\abcd.ac(bcd))";
          continue;
        case L'*':
          str = "(\\abc.a(bc))";
          continue;
        case L'^':
          str = "(\\ab.ba)";
          continue;
        case L'-':
          str = "(\\ab.b(\\cde.c(\\fg.g(fd))(\\f.e)(\\f.f))a)";
          continue;
        case L'/':
          str = "(\\a.(\\b.(\\c.cc)(\\c.b(cc)))(\\bcdef.(\\g.(\\h.h(\\ijk.k)("
                "\\ij.i))g((\\hi.i)ef)(e(bgdef)))((\\gh.h(\\ijk.i(\\lm.m(lj))("
                "\\l.k)(\\l.l))g)cd))((\\bcd.c(bcd))a))";
          continue;
        case L'Я':
          str = "(\\a.a((\\b.bb)(\\bcde.d(bb)(\\f.fce)))(\\bc.c))";
          continue;
        default:
          break;
      }
    }
    return c;
  }
}

static int Need(void) {
  int c;
  if ((c = Greed()) != EOF) return c;
  Error(1, "unfinished expression");
}

static struct Node *Parse1(void) {
  wint_t c;
  int i, oldsp;
  struct Node *r, *p, *q;
  do {
    if ((c = Greed()) == EOF) return 0;
  } while (iswspace(c));
  if (c == L'λ' || c == '\\') {
    oldsp = sp;
    p = r = NewNode(0, 0, 0, 0);
    ++sp;
    for (;;) {
      c = Need();
      if (c == L'λ' || c == '\\') {
        p = p->l = NewNode(0, 0, 0, 0);
        ++sp;
        continue;
      } else {
        unget = c;
        break;
      }
    }
    q = Parse1();
    if (!q) Error(4, "lambda needs body");
    p->l = q;
    while ((q = Parse1())) {
      p->l = NewNode(2, 0, p->l, q);
    }
    sp = oldsp;
    return r;
  } else if (c == L'!') {
    // intentionally trigger undefined variable
    return NewNode(1, sp, 0, 0);
  } else if (iswdigit(c)) {
    i = 0;
    for (;;) {
      i *= 10;
      i += c - '0';
      c = Greed();
      if (c == EOF) break;
      if (!iswdigit(c)) {
        unget = c;
        break;
      }
    }
    i -= indexing;
    if (i < 0) Error(5, "undefined variable: %lc", c);
    return NewNode(1, i, 0, 0);
  } else if (c == '(') {
    p = r = Parse1();
    if (!p) Error(6, "empty parenthesis");
    while ((q = Parse1())) {
      r = NewNode(2, 0, r, q);
    }
    c = Need();
    if (c != ')') Error(7, "expected closing parenthesis");
    return r;
  } else if (c == ')') {
    unget = c;
    return 0;
  } else {
    Error(8, "unexpected character: 0x%04x %lc", c, c);
  }
}

static struct Node *Parse(void) {
  struct Node *r, *p, *q;
  p = r = Parse1();
  if (!p) Error(6, "empty expression");
  while ((q = Parse1())) {
    r = NewNode(2, 0, r, q);
  }
  return r;
}

static void Print(struct Node *p) {
  int i;
  if (p->t == 0) {
    fputc('0', stdout);
    fputc('0', stdout);
    Print(p->l);
  } else if (p->t == 1) {
    for (i = -1; i < p->x; ++i) {
      fputc('1', stdout);
    }
    fputc('0', stdout);
  } else if (p->t == 2) {
    fputc('0', stdout);
    fputc('1', stdout);
    Print(p->l);
    Print(p->r);
  } else {
    abort();
  }
}

int main(int argc, char *argv[]) {
  setlocale(LC_ALL, "");
  LoadFlags(argc, argv);
  Print(Parse());
}
