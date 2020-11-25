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
#include "libc/bits/bits.h"
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/calls.h"
#include "libc/conv/conv.h"
#include "libc/errno.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/bsf.h"
#include "libc/nexgen32e/bsr.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/time/time.h"
#include "libc/x/x.h"
#include "o/tool/tags/tags.c.inc"
#include "o/tool/tags/tags.h.inc"
#include "tool/tags/tags.h"

static jmp_buf jb;
static int g_line;
static int g_column;
static const char *g_file;
static yyParser g_parser[1];

noreturn static void Error(const char *msg) {
  fprintf(stderr, "%s:%d:%d: %s\n", g_file, g_line, g_column, msg);
  longjmp(jb, 1);
}

noreturn static void SyntaxError(void) {
  Error("SYNTAX ERROR");
}

noreturn static void LexError(void) {
  Error("LEX ERROR");
}

noreturn static void MissingArgumentError(void) {
  Error("MISSING ARGUMENT");
}

noreturn static void MissingFunctionError(void) {
  Error("MISSING FUNCTION");
}

noreturn static void SyscallError(const char *name) {
  fprintf(stderr, "ERROR: %s[%s]: %d\n", name, g_file, errno);
  exit(1);
}

static void ExprsFree(struct Exprs *n) {
  if (n) {
    ExprsFree(n->n);
    free(n);
  }
}

static struct Exprs *ExprsAppend(struct Exprs *n, long double x) {
  struct Exprs *a;
  a = malloc(sizeof(struct Exprs));
  a->n = n;
  a->x = x;
  return a;
}

static long double ParseExpr(struct Token t) {
  char *ep;
  ep = t.s + t.n;
  if (t.s[0] == '0') {
    return strtoumax(t.s, &ep, 0);
  } else {
    return strtod(t.s, &ep);
  }
}

static long double CallFunction(struct Token fn, struct Exprs *args) {
  return 0;
}

static void Tokenize(const char *s, size_t size) {
  int kw;
  size_t n;
  char *se;
  for (se = s + size; s < se; s += n, ++g_column) {
    n = 1;
    switch (*s & 0xff) {
      case ' ':
      case '\t':
      case '\v':
      case '\r':
      case 0x0C:
        break;
      case '\n':
        ++g_line;
        g_column = 0;
        break;
      case 'A' ... 'Z':
      case 'a' ... 'z':
        n = strspn(s, "$"
                      "0123456789"
                      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                      "abcdefghijklmnopqrstuvwxyz");
        if ((kw = GetKeyword(s, n)) != -1) {
          Parse(g_parser, kw, (struct Token){s, n});
        } else {
          Parse(g_parser, TK_SYMBOL, (struct Token){s, n});
        }
        break;
      case '0':
        n = strspn(s, "xXbB0123456789abcdefABCDEF");
        Parse(g_parser, TK_I_CONSTANT, (struct Token){s, n});
        n += strspn(s + n, "LUlu");
        break;
      case '1' ... '9':
        n = strspn(s, "0123456789.");
        if (s[n] == 'e' || s[n] == 'E') {
          ++n;
          if (s[n] == '+' || s[n] == '-') ++n;
          n += strspn(s + n, "0123456789");
        }
        Parse(g_parser, memchr(s, '.', n) ? TK_F_CONSTANT : TK_I_CONSTANT,
              (struct Token){s, n});
        n += strspn(s + n, "LUlu");
        break;
      case ';':
        Parse(g_parser, TK_SEMI, (struct Token){0, 0});
        break;
      case '(':
        Parse(g_parser, TK_LP, (struct Token){0, 0});
        break;
      case ')':
        Parse(g_parser, TK_RP, (struct Token){0, 0});
        break;
      case '[':
        Parse(g_parser, TK_LSB, (struct Token){0, 0});
        break;
      case ']':
        Parse(g_parser, TK_RSB, (struct Token){0, 0});
        break;
      case '{':
        Parse(g_parser, TK_LCB, (struct Token){0, 0});
        break;
      case '}':
        Parse(g_parser, TK_RCB, (struct Token){0, 0});
        break;
      case '?':
        Parse(g_parser, TK_QUESTION, (struct Token){0, 0});
        break;
      case ':':
        Parse(g_parser, TK_COLON, (struct Token){0, 0});
        break;
      case ',':
        Parse(g_parser, TK_COMMA, (struct Token){0, 0});
        break;
      case '^':
        if (s[1] == '=') {
          Parse(g_parser, TK_XOR_ASSIGN, (struct Token){0, 0});
          ++n;
        } else {
          Parse(g_parser, TK_XOR, (struct Token){0, 0});
        }
        break;
      case '%':
        if (s[1] == '=') {
          Parse(g_parser, TK_REM_ASSIGN, (struct Token){0, 0});
          ++n;
        } else {
          Parse(g_parser, TK_REM, (struct Token){0, 0});
        }
        break;
      case '.':
        Parse(g_parser, TK_DOT, (struct Token){0, 0});
        break;
      case '+':
        if (s[1] == '=') {
          Parse(g_parser, TK_ADD_ASSIGN, (struct Token){0, 0});
          ++n;
        } else if (s[1] == '+') {
          Parse(g_parser, TK_INC, (struct Token){0, 0});
          ++n;
        } else {
          Parse(g_parser, TK_ADD, (struct Token){0, 0});
        }
        break;
      case '-':
        if (s[1] == '=') {
          Parse(g_parser, TK_SUB_ASSIGN, (struct Token){0, 0});
          ++n;
        } else if (s[1] == '-') {
          Parse(g_parser, TK_DEC, (struct Token){0, 0});
          ++n;
        } else if (s[1] == '>') {
          Parse(g_parser, TK_ARROW, (struct Token){0, 0});
          ++n;
        } else {
          Parse(g_parser, TK_SUB, (struct Token){0, 0});
        }
        break;
      case '~':
        Parse(g_parser, TK_TILDE, (struct Token){0, 0});
        break;
      case '/':
        if (s[1] == '=') {
          Parse(g_parser, TK_DIV_ASSIGN, (struct Token){0, 0});
          ++n;
        } else {
          Parse(g_parser, TK_DIV, (struct Token){0, 0});
        }
        break;
      case '*':
        if (s[1] == '=') {
          Parse(g_parser, TK_MUL_ASSIGN, (struct Token){0, 0});
          ++n;
        } else {
          Parse(g_parser, TK_STAR, (struct Token){0, 0});
        }
        break;
      case '|':
        if (s[1] == '|') {
          Parse(g_parser, TK_OR_LOGICAL, (struct Token){0, 0});
          ++n;
        } else if (s[1] == '=') {
          Parse(g_parser, TK_OR_ASSIGN, (struct Token){0, 0});
          ++n;
        } else {
          Parse(g_parser, TK_OR, (struct Token){0, 0});
        }
        break;
      case '&':
        if (s[1] == '&') {
          Parse(g_parser, TK_AND_LOGICAL, (struct Token){0, 0});
          ++n;
        } else if (s[1] == '=') {
          Parse(g_parser, TK_AND_ASSIGN, (struct Token){0, 0});
          ++n;
        } else {
          Parse(g_parser, TK_AND, (struct Token){0, 0});
        }
        break;
      case '!':
        if (s[1] == '=') {
          Parse(g_parser, TK_NOTEQUAL, (struct Token){0, 0});
          ++n;
        } else {
          Parse(g_parser, TK_EXCLAIM, (struct Token){0, 0});
        }
        break;
      case '=':
        if (s[1] == '=') {
          Parse(g_parser, TK_EQUAL, (struct Token){0, 0});
          ++n;
        } else {
          Parse(g_parser, TK_EQ, (struct Token){0, 0});
        }
        break;
      case '>':
        if (s[1] == '=') {
          Parse(g_parser, TK_GE, (struct Token){0, 0});
          ++n;
        } else if (s[1] == '>') {
          if (s[2] == '=') {
            Parse(g_parser, TK_SHR_ASSIGN, (struct Token){0, 0});
            ++n;
          } else {
            Parse(g_parser, TK_SHR, (struct Token){0, 0});
          }
          ++n;
        } else {
          Parse(g_parser, TK_GT, (struct Token){0, 0});
        }
        break;
      case '<':
        if (s[1] == '=') {
          Parse(g_parser, TK_LE, (struct Token){0, 0});
          ++n;
        } else if (s[1] == '<') {
          if (s[2] == '=') {
            Parse(g_parser, TK_SHL_ASSIGN, (struct Token){0, 0});
            ++n;
          } else {
            Parse(g_parser, TK_SHL, (struct Token){0, 0});
          }
          ++n;
        } else {
          Parse(g_parser, TK_LT, (struct Token){0, 0});
        }
        break;
      default:
        LexError();
    }
  }
}

int main(int argc, char *argv[]) {
  int i;
  int ec;
  int fd;
  size_t n;
  char *buf;
  ssize_t rc;
  size_t bufcap;
  bufcap = BIGPAGESIZE;
  buf = malloc(bufcap);
  if (!(ec = setjmp(jb))) {
    for (i = 1; i < argc; ++i) {
      g_file = argv[i];
      g_line = 0;
      g_column = 0;
      n = 0; /* wut */
      if ((fd = open(g_file, O_RDONLY)) == -1) SyscallError("open");
      ParseInit(g_parser);
      for (;;) {
        if ((rc = read(fd, buf, bufcap)) == -1) SyscallError("read");
        if (!(n = rc)) break;
        Tokenize(buf, n);
      }
      close(fd);
      Parse(g_parser, 0, (struct Token){0, 0});
      ParseFinalize(g_parser);
    }
  }
  free(buf);
  return ec;
}
