#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "printf.h"
#include <stdlib.h>
#include <string.h>
#include "char.h"
#include "plinko.h"
#include "print.h"

#define MAX_(X, Y) ((Y) < (X) ? (X) : (Y))

// this must be a macro, because va_list is passed by value on aarch64
// so calling va_arg() in a helper function wouldn't advance our cursor
#define GetVarInt(va, t) ((t) <= 0 ? va_arg(va, int) : va_arg(va, long))

static int PrintStr(int fd, const wchar_t *s, int cols) {
  int n, k = 0, i = 0;
  n = wcslen(s);
  k += PrintIndent(fd, +cols - n);
  while (i < n)
    k += PrintChar(fd, s[i++]);
  k += PrintIndent(fd, -cols - n);
  return k;
}

int Printf(const wchar_t *f, ...) {
  int n;
  va_list va;
  va_start(va, f);
  n = Vfnprintf(f, va, 1, 0);
  va_end(va);
  return n;
}

int Fprintf(int fd, const wchar_t *f, ...) {
  int n;
  va_list va;
  va_start(va, f);
  n = Vfnprintf(f, va, fd, 0);
  va_end(va);
  return n;
}

int Fnprintf(int fd, int n, const wchar_t *f, ...) {
  va_list va;
  va_start(va, f);
  n = Vfnprintf(f, va, fd, n);
  va_end(va);
  return n;
}

int Vfnprintf(const wchar_t *f, va_list va, int fd, int n) {
  enum { kPlain, kEsc, kCsi };
  static int recursive;
  dword t, u;
  const wchar_t *s;
  signed char type;
  wchar_t quot, ansi, gotr, pdot, zero;
  int b, c, x, y, si, prec, cols, sign;
  gotr = false;
  t = GetNanos();
  ++recursive;
  for (ansi = 0;;) {
    for (;;) {
      if (!(c = *f++ & 0377) || c == L'%')
        break;
      switch (ansi) {
        case kPlain:
          if (c == 033) {
            ansi = kEsc;
          } else if (c != L'\n' && c != L'\r') {
            n += GetMonospaceCharacterWidth(c);
          } else {
            n = 0;
          }
          break;
        case kEsc:
          if (c == '[') {
            ansi = kCsi;
          } else {
            ansi = kPlain;
          }
          break;
        case kCsi:
          if (0x40 <= c && c <= 0x7e) {
            ansi = kPlain;
          }
          break;
        default:
          abort();
      }
    EmitFormatByte:
      PrintChar(fd, c);
    }
    if (!c)
      break;
    prec = 0;
    pdot = 0;
    cols = 0;
    quot = 0;
    type = 0;
    zero = 0;
    sign = 1;
    for (;;) {
      switch ((c = *f++)) {
        default:
          goto EmitFormatByte;
        case L'n':
          PrintNewline(fd);
          n = 0;
          break;
        case L'l':
          ++type;
          continue;
        case L'0':
        case L'1':
        case L'2':
        case L'3':
        case L'4':
        case L'5':
        case L'6':
        case L'7':
        case L'8':
        case L'9':
          si = pdot ? prec : cols;
          si *= 10;
          si += c - '0';
          goto UpdateCols;
        case L'*':
          si = va_arg(va, int);
        UpdateCols:
          if (pdot) {
            prec = si;
          } else {
            if (si < 0) {
              si = -si;
              sign = -1;
            } else if (!si) {
              zero = 1;
            }
            cols = si;
          }
          continue;
        case L'-':
          sign = -1;
          continue;
        case L'.':
          pdot = 1;
          continue;
        case L'_':
        case L',':
        case L'\'':
          quot = c;
          continue;
        case L'I':
          if (depth >= 0) {
            n += PrintDepth(fd, depth);
          } else {
            n += PrintIndent(fd, sp * 2);
          }
          break;
        case L'J':
          if (depth >= 0) {
            n += PrintDepth(fd, depth - 1);
          } else {
            n += PrintIndent(fd, (sp - 1) * 2);
          }
          break;
        case L'V':
          y = depth >= 0 ? depth : sp;
          if (y) {
            n += PrintIndent(fd, (y - 1) * 2);
            n += PrintChar(fd, L'├');
            n += PrintChar(fd, L'─');
          }
          break;
        case L'W':
          y = depth >= 0 ? depth : sp;
          if (y) {
            n += PrintIndent(fd, (y - 1) * 2);
            n += PrintChar(fd, L'│');
            n += PrintChar(fd, L' ');
          }
          break;
        case L'X':
          y = depth >= 0 ? depth : sp;
          if (y) {
            n += PrintIndent(fd, (y - 1) * 2);
            n += PrintChar(fd, L'└');
            n += PrintChar(fd, L'─');
          }
          break;
        case L'p':
          if (simpler)
            goto SimplePrint;
          // fallthrough
        case L'P':
          n += PrettyPrint(fd, va_arg(va, int),
                           MAX_(0, depth > 0 ? n - depth * 3 : n));
          break;
        case L'T':
          PrintTree(fd, va_arg(va, int), n);
          break;
        case L'R':
          gotr = true;
          n += PrintInt(fd, t - tick, cols * sign, quot, zero, 10, true);
          break;
        case L'S':
        SimplePrint:
          n += Print(fd, va_arg(va, int));
          break;
        case L'A':
          y = va_arg(va, int);
          x = va_arg(va, int);
          n += PrintChar(fd, L'[');
          n += PrintArgs(fd, y, x, 0);
          n += PrintChar(fd, L']');
          break;
        case L'K':
          if ((b = va_arg(va, int)) < 0) {
            PrintChar(fd, L'(');
            for (;;) {
              n += Print(fd, Car(Car(b)));
              if ((b = Cdr(b)) >= 0)
                break;
              PrintChar(fd, L' ');
            }
            PrintChar(fd, L')');
          } else {
            n += Print(fd, b);
          }
          break;
        case L'd':
          n += PrintInt(fd, GetVarInt(va, type), cols * sign, quot, zero, 10,
                        true);
          break;
        case L'u':
          n +=
              PrintInt(fd, GetVarInt(va, type), cols * sign, quot, zero, 10, 0);
          break;
        case L'b':
          n += PrintInt(fd, GetVarInt(va, type), cols * sign, quot, zero, 2, 0);
          break;
        case L'o':
          n += PrintInt(fd, GetVarInt(va, type), cols * sign, quot, zero, 8, 0);
          break;
        case L'x':
          n +=
              PrintInt(fd, GetVarInt(va, type), cols * sign, quot, zero, 16, 0);
          break;
        case L's':
          s = va_arg(va, const wchar_t *);
          if (!s)
            s = L"NULL";
          n += PrintStr(fd, s, cols * sign);
          break;
        case L'c':
          n += PrintChar(fd, va_arg(va, int));
          break;
      }
      break;
    }
  }
  --recursive;
  if (!recursive) {
    // %R reports time elapsed since the last %R, minus time spent printing
    u = GetNanos();
    if (gotr) {
      tick = u;
    } else {
      tick += u - t;
    }
  }
  return n;
}
