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
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"

#define T short

#define CONS    0
#define ATOM    1
#define INTEGER 2

#define NIL          OBJECT(CONS, 0)
#define TYPE(x)      ((x)&3)
#define VALUE(x)     ((x) >> 2)
#define OBJECT(t, v) ((v) << 2 | (t))

static T Mi, Si;
static T M[2048];
static char S[2048];

static noreturn void Die(void) {
  abort();
}

static int Parse(const char *s) {
  int x, j;
  for (;;) {
    switch (*s & 0xff) {
      case ' ':
      case '\t':
      case '\r':
      case '\n':
      case '\v':
        ++s;
        break;
      case 'A' ... 'Z':
        x = Si;
        do {
          S[Si++] = *s++;
        } while ('A' <= *s && *s <= 'Z');
        S[Si++] = 0;
        M[++Mi] = OBJECT(ATOM, x);
        return Mi;
      case '0' ... '9':
        x = 0;
        do {
          x *= 10;
          x += *s++ - '0';
        } while ('0' <= *s && *s <= '9');
        M[++Mi] = OBJECT(INTEGER, x);
        return Mi;
      default:
        Die();
    }
  }
}

static int PrintChar(int c) {
  return putc(c, stdout);
}

static void PrintString(const char *s) {
  while (*s) PrintChar(*s++);
}

static void PrintInteger(int x) {
  int q, r;
  q = x / 10;
  r = x % 10;
  if (q) PrintInteger(q);
  PrintChar('0' + r);
}

static void PrintObject(int i) {
  int j, x;
  switch (TYPE(M[i])) {
    case CONS:
      if ((i = VALUE(M[i]))) {
        PrintChar('(');
        PrintObject(i);
        for (;;) {
          if (TYPE(M[i + 1]) == CONS) {
            if (!(i = VALUE(M[i + 1]))) break;
            PrintChar(' ');
            PrintObject(i);
          } else {
            PrintString(" . ");
            PrintObject(i + 1);
            break;
          }
        }
        PrintChar(')');
      } else {
        PrintString("NIL");
      }
      break;
    case ATOM:
      for (j = VALUE(M[i]); S[j]; ++j) {
        PrintChar(S[j]);
      }
      break;
    case INTEGER:
      PrintInteger(VALUE(M[i]));
      break;
    default:
      unreachable;
  }
}

static void Print(int i) {
  PrintObject(i);
  PrintChar('\n');
}

int main(int argc, char *argv[]) {
  int i;

  M[1] = OBJECT(CONS, 2);
  M[2] = OBJECT(INTEGER, 123);
  M[3] = OBJECT(CONS, 4);
  M[4] = OBJECT(INTEGER, 456);
  M[5] = OBJECT(CONS, 6);
  M[6] = OBJECT(INTEGER, 789);
  M[7] = NIL;
  Print(1);

  return 0;
}
