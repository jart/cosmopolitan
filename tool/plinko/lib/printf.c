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
#include "tool/plinko/lib/printf.h"
#include "libc/intrin/strace.internal.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/time/clockstonanos.internal.h"
#include "tool/plinko/lib/char.h"
#include "tool/plinko/lib/plinko.h"
#include "tool/plinko/lib/print.h"

static inline long GetVarInt(va_list va, signed char t) {
  if (t <= 0) return va_arg(va, int);
  return va_arg(va, long);
}

static int PrintStr(int fd, const char *s, int cols) {
  int n, k = 0, i = 0;
  n = strlen(s);
  k += PrintIndent(fd, +cols - n);
  while (i < n) k += PrintChar(fd, s[i++]);
  k += PrintIndent(fd, -cols - n);
  return k;
}

int Printf(const char *f, ...) {
  int n;
  va_list va;
  va_start(va, f);
  n = Vfnprintf(f, va, 1, 0);
  va_end(va);
  return n;
}

int Fprintf(int fd, const char *f, ...) {
  int n;
  va_list va;
  va_start(va, f);
  n = Vfnprintf(f, va, fd, 0);
  va_end(va);
  return n;
}

int Fnprintf(int fd, int n, const char *f, ...) {
  va_list va;
  va_start(va, f);
  n = Vfnprintf(f, va, fd, n);
  va_end(va);
  return n;
}

int Vfprintf(const char *f, va_list va, int fd) {
  return Vfnprintf(f, va, fd, 0);
}

int Vfnprintf(const char *f, va_list va, int fd, int n) {
  enum { kPlain, kEsc, kCsi };
  static int recursive;
  dword t, u;
  const char *s;
  signed char type;
  char quot, ansi, gotr, pdot, zero;
  int b, c, x, y, si, prec, cols, sign;
  gotr = false;
  t = rdtsc();
  ftrace_enabled(-1);
  strace_enabled(-1);
  ++recursive;
  for (ansi = 0;;) {
    for (;;) {
      if (!(c = *f++ & 0377) || c == L'%') break;
      if (c >= 0300) {
        for (b = 0200; c & b; b >>= 1) {
          c ^= b;
        }
        while ((*f & 0300) == 0200) {
          c <<= 6;
          c |= *f++ & 0177;
        }
      }
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
          __builtin_unreachable();
      }
    EmitFormatByte:
      PrintChar(fd, c);
    }
    if (!c) break;
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
          if (simpler) goto SimplePrint;
          // fallthrough
        case L'P':
          n += PrettyPrint(fd, va_arg(va, int),
                           MAX(0, depth > 0 ? n - depth * 3 : n));
          break;
        case L'T':
          PrintTree(fd, va_arg(va, int), n);
          break;
        case L'R':
          gotr = true;
          n += PrintInt(fd, ClocksToNanos(tick, t), cols * sign, quot, zero, 10,
                        true);
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
              if ((b = Cdr(b)) >= 0) break;
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
          s = va_arg(va, const char *);
          if (!s) s = "NULL";
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
  ftrace_enabled(+1);
  strace_enabled(+1);
  if (!recursive) {
    u = rdtsc();
    if (gotr) {
      tick = u;
    } else {
      tick -= u >= t ? u - t : ~t + u + 1;
    }
  }
  return n;
}
