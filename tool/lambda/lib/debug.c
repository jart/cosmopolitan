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
#include "libc/fmt/itoa.h"
#include "libc/intrin/kprintf.h"
#include "libc/str/str.h"
#include "tool/lambda/lib/blc.h"

const char *GetOpName(int x) {
  switch (x) {
    case VAR:
      return "var";
    case APP:
      return "app";
    case ABS:
      return "abs";
    case IOP:
      return "iop";
    default:
      return "wut";
  }
}

int GetDepth(struct Closure *env) {
  int i;
  for (i = 0; env && env != &root; ++i) {
    env = env->next;
  }
  return i;
}

void PrintClosure(struct Closure *c, const char *name, int indent, FILE *f) {
  int j;
  char ibuf[21];
  while (c && c != &root) {
    for (j = 0; j < indent; ++j) {
      if (j) {
        fputs("│ ", f);
      } else {
        fputs("  ", f);
      }
    }
    fputs(name, f);
    fputs(": ", f);
    Print(c->term, 0, GetDepth(c->envp), f);
    fputs(" +", f);
    FormatInt64(ibuf, c->refs);
    fputs(ibuf, f);
    fputc('\n', f);
    PrintClosure(c->envp, "envp", indent + 1, f);
    c = c->next;
  }
}

void PrintMachineState(FILE *f) {
  int i;
  char buf[256];
  static int op;
  fputc('\n', f);
  for (i = 0; i < 80; ++i) fputwc(L'─', f);
  ksnprintf(buf, sizeof(buf),
            "%d\n   ip      %ld | op %d %s | arg %d | end %ld\n", op++, ip,
            mem[ip], GetOpName(mem[ip]), mem[ip + 1], end);
  fputs(buf, f);
  fputs(" term      ", f);
  Print(ip, 0, GetDepth(envp), f);
  fputc('\n', f);
  fputc('\n', f);
  PrintClosure(contp, "contp", 1, f);
  fputc('\n', f);
  PrintClosure(envp, "envp", 1, f);
  fputc('\n', f);
  PrintClosure(frep, "frep", 1, f);
}

void PrintExpressions(FILE *f, char alog, char vlog) {
  int i, d;
  char buf[48];
  struct Closure *p, ps;
  ps.term = ip;
  ps.next = contp;
  ps.envp = envp;
  for (p = &ps; p; p = p->next) {
    Print(p->term, 1, GetDepth(p->envp), f);
    if (p->next) fputc(' ', f);
  }
  if (alog) {
    fputs(" ⟹ ", f);
    switch (mem[ip]) {
      case VAR:
        ksnprintf(buf, sizeof(buf), "var[%d]", mem[ip + 1]);
        fputs(buf, f);
        break;
      case APP:
        fputs("app[", f);
        Print(ip + 2 + mem[ip + 1], 1, GetDepth(envp), f);
        fputc(']', f);
        break;
      case ABS:
        if (contp) {
          fputs("abs[", f);
          Print(ip + 1, 1, GetDepth(envp), f);
          fputc(']', f);
        } else {
          ksnprintf(buf, sizeof(buf), "bye[%d]", mem[ip + 2]);
          fputs(buf, f);
        }
        break;
      case IOP:
        if (ip < 22) {
          if (!binary) {
            ksnprintf(buf, sizeof(buf), "put[%c]", '0' + (int)(ip & 1));
          } else if (mem[ip + 1] & 1) {
            ksnprintf(buf, sizeof(buf), "put[0%hho '%c']", co,
                      isprint(co) ? co : '.');
          } else {
            ksnprintf(buf, sizeof(buf), "wr%d[0%hho]", (int)(ip & 1), co);
          }
          fputs(buf, f);
        } else {
          fputs("gro", f);
        }
        break;
      default:
        break;
    }
  }
  if (vlog) {
    d = GetDepth(envp);
    for (i = 0, p = envp; p->term != -1; ++i, p = p->next) {
      fputc('\n', f);
      fputc('\t', f);
      PrintVar(style != 1 ? i : d - 1 - i, f);
      fputc('=', f);
      Print(p->term, 0, GetDepth(p), f);
    }
  }
  fputc('\n', f);
}
