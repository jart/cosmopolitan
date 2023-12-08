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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/log/log.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/locale.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/rlimit.h"
#include "libc/sysv/consts/sig.h"
#include "third_party/getopt/getopt.internal.h"
#include "tool/lambda/lib/blc.h"

#define USAGE \
  " [-?hubBdsarvnNlS] <stdin >expr.txt\n\
Binary Lambda Calculus Virtual Machine\n\
\n\
FLAGS\n\
\n\
  -h      help\n\
  -r      rex log\n\
  -b      binary 8-bit i/o\n\
  -B      debug print binary\n\
  -l      print lambda notation\n\
  -a      action log [implies -r]\n\
  -v      variable log [implies -r]\n\
  -s      full machine state logging\n\
  -n      disables name rewriting rules\n\
  -N      disables most unicode symbolism\n\
  -d      dump terms on successful exit\n"

#define NIL   23
#define TRUE  27
#define FALSE 23

#define REF(c) (++(c)->refs, c)

static const char kRom[] = {
    APP, 0,  //  0 (λ 0 λ 0 (λ 0 wr0 wr1) put) (main gro)
    ABS,     //  2 λ 0 λ 0 (λ 0 wr0 wr1) put
    APP, 0,  //  3
    VAR, 0,  //  5
    ABS,     //  7
    APP,     //  8
    ABS,     //  9 λ 0 λ 0 wr0 wr1
    APP, 2,  // 10
    VAR,     // 12
    IOP,     // 13
    ABS,     // 14 λ 0 wr0 wr1
    APP, 4,  // 15
    APP, 1,  // 17
    VAR,     // 19
    IOP,     // 20 wr0
    IOP, 0,  // 21 wr1
    ABS,     // 23 (λλ 0) a.k.a. nil
    ABS,     // 24 exit
    VAR,     // 25
    0,       // 26 exit[0]
    ABS,     // 27 (λλ 1) a.k.a. true
    ABS,     // 28
    VAR, 1,  // 29
};

static int postdump;
static int kLazy[256];

void Quit(int sig) {
  Dump(0, end, stderr);
  exit(128 + sig);
}

void PrintUsage(const char *prog, int rc, FILE *f) {
  fputs("Usage: ", f);
  fputs(prog, f);
  fputs(USAGE, f);
  exit(rc);
}

int Backref(int x) {
  return x - (end + 1);
}

static inline void Expand(int c) {
  if (end >= TERMS) Error(5, "OUT OF TERMS");
  mem[end++] = c;
}

void Gc(struct Closure *p) {
  struct Closure *t;
  while (p && p != &root) {
    if (--p->refs) break;
    Gc(p->next);
    t = p->envp;
    p->envp = 0;
    p->next = frep;
    frep = p;
    p = t;
  }
}

void Var(void) {
  int i, x;
  struct Closure *t, *e;
  e = t = envp;
  x = mem[ip + 1];
  for (i = 0; i < x && e != &root; ++i) e = e->next;
  if (e == &root) Error(10 + x, "UNDEFINED VARIABLE %d", x);
  ip = e->term;
  envp = REF(e->envp);
  Gc(t);
}

void Gro(void) {
  int c = fgetc(stdin);
  if (c != -1) {
    Expand(ABS);
    Expand(APP);
    Expand(4);
    Expand(APP);
    Expand(Backref(binary ? kLazy[c] : c & 1 ? FALSE : TRUE));
    Expand(VAR);
    Expand(0);
  } else {
    Expand(ABS);
    Expand(ABS);
    Expand(VAR);
    Expand(0);
  }
}

void Put(void) {
  int bit;
  long newip;
  if (!binary) {
    co = '0' + (ip & 1);
    fputc(co, stdout);
    newip = 2;
  } else if (mem[ip + 1] & 1) {  // ip ∈ {6,13}
    fputc(co, stdout);
    newip = 2;
  } else {      // ip ∈ {20,21}
    newip = 9;  // (λ 0 (λ 0 wr1 wr0))
    bit = ip & 1;
    co = (co * 2) | bit;
  }
  if (ferror(stdout)) {
    exit(55);
  }
  ip = newip;
}

void Bye(void) {
  int rc = mem[ip + 2];  // (λ 0) [exitcode]
  if (rc) Error(rc, "CONTINUATIONS EXHAUSTED");
  if (postdump && !rc) Dump(0, end, stderr);
  exit(0);
}

// pops continuation and pushes it to environment
void Abs(void) {
  if (!contp) Bye();
  struct Closure *t = contp;
  contp = t->next;
  t->next = envp;
  envp = t;
  ++ip;
}

struct Closure *Alloc(void) {
  struct Closure *t;
  if (!(t = frep)) {
    if (!(t = Calloc(1, sizeof(struct Closure)))) {
      Error(6, "OUT OF HEAP");
    }
  }
  frep = t->next;
  t->refs = 1;
  ++heap;
  return t;
}

// pushes continuation for argument
void App(void) {
  int x = mem[ip + 1];
  struct Closure *t = Alloc();
  t->term = ip + 2 + x;
  t->envp = t->term > 21 && t->term != end ? REF(envp) : &root;
  t->next = contp;
  contp = t;
  ip += 2;
}

int LoadByte(int c) {
  int i, r = end;
  for (i = 7; i >= 0; --i) {
    Expand(ABS);
    Expand(APP);
    Expand(i ? +4 : Backref(NIL));
    Expand(APP);
    Expand(Backref(c & (1 << i) ? FALSE : TRUE));
    Expand(VAR);
    Expand(0);
  }
  return r;
}

void LoadRom(void) {
  long i;
  for (; end < sizeof(kRom) / sizeof(*kRom); ++end) {
    mem[end] = kRom[end];
  }
  mem[4] = binary ? 2 : 9;
  if (binary) {
    for (i = 0; i < 256; ++i) {
      kLazy[i] = LoadByte(i);
    }
  }
  mem[1] = end - 2;
}

void Iop(void) {
  if (ip == end) {
    Gro();
  } else {
    Put();  // ip ∈ {6,13,20,21}
  }
  Gc(envp);
  envp = &root;
}

static void Rex(void) {
  if (slog) PrintMachineState(stderr);
  if (rlog && (alog || mem[ip] != APP)) {
    PrintExpressions(stderr, alog, vlog);
  }
  switch (mem[ip]) {
    case VAR:
      Var();
      break;
    case APP:
      App();
      break;
    case ABS:
      Abs();
      break;
    case IOP:
      Iop();
      break;
    default:
      Error(7, "CORRUPT TERM");
  }
}

void Krivine(void) {
  int main;
  long gotoget;
  LoadRom();
  mem[end++] = APP;
  gotoget = end++;
  main = end;
  mem[gotoget] = Parse(1, stdin).n;
  if (rlog) {
    Print(main, 1, 0, stderr);
    fputs("\n", stderr);
    if (alog) {
      fputs("⟿ wrap[", stderr);
      Print(0, 1, 0, stderr);
      fputs("]\n", stderr);
    }
  }
  for (;;) Rex();
}

void LoadFlags(int argc, char *argv[]) {
  int i;
  const char *prog;
  prog = argc ? argv[0] : "cblc";
  while ((i = getopt(argc, argv, "?hubBdsarvnNlS")) != -1) {
    switch (i) {
      case 'b':
        binary = 1;
        break;
      case 'S':
        safer = 1;
        break;
      case 'n':
        noname = 1;
        break;
      case 'N':
        asciiname = 1;
        break;
      case 'B':
        style = 2;
        break;
      case 'l':
        style = 1;
        break;
      case 's':
        slog = 1;
        break;
      case 'r':
        rlog = 1;
        break;
      case 'a':
        rlog = 1;
        alog = 1;
        break;
      case 'v':
        rlog = 1;
        vlog = 1;
        break;
      case 'd':
        postdump = 1;
        break;
      case '?':
      case 'h':
        PrintUsage(prog, 0, stdout);
      default:
        PrintUsage(prog, 1, stderr);
    }
  }
}

int main(int argc, char *argv[]) {
  struct rlimit rlim = {512 * 1024 * 1024, 512 * 1024 * 1024};
  setrlimit(RLIMIT_AS, &rlim);
  signal(SIGQUIT, Quit);
  signal(SIGPIPE, Quit);
  LoadFlags(argc, argv);
  setlocale(LC_ALL, "");
  setvbuf(stdout, 0, _IOLBF, 0);
  setvbuf(stderr, 0, _IOLBF, 0);
  Krivine();
}
