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
#include "libc/errno.h"
#include "libc/intrin/strace.internal.h"
#include "libc/log/check.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "tool/plinko/lib/char.h"
#include "tool/plinko/lib/cons.h"
#include "tool/plinko/lib/error.h"
#include "tool/plinko/lib/plinko.h"
#include "tool/plinko/lib/printf.h"

static int Read1(int);
static int Read2(int);

dontinstrument int ReadSpaces(int fd) {
  size_t n;
  ssize_t rc;
  for (;;) {
    rc = read(fd, g_buffer[fd], sizeof(g_buffer[fd]) - 1);
    if (rc != -1) {
      if ((n = rc)) {
        g_buffer[fd][n] = 0;
        bp[fd] = 1;
        return g_buffer[fd][0] & 255;
      } else if (fd == 0 && *inputs) {
        close(0);
        if (open(*inputs++, O_RDONLY) == -1) {
          ++fails;
          Flush(1);
          Fprintf(2, "error: open(%s) %s%n", inputs[-1], strerror(errno));
          longjmp(exiter, 1);
        }
      } else {
        Flush(1);
        Flush(2);
        longjmp(exiter, 1);
      }
    } else if (errno != EINTR) {
      ++fails;
      Flush(1);
      Fprintf(2, "error: read(%d) %s%n", fd, strerror(errno));
      longjmp(exiter, 1);
    }
  }
}

dontinstrument int ReadByte(int fd) {
  int c;
  if ((c = g_buffer[fd][bp[fd]++] & 255)) return c;
  return ReadSpaces(fd);
}

dontinstrument int ReadChar(int fd) {
  int b, a = dx;
  for (;;) {
    dx = ReadByte(fd);
    if (dx != ';') {
      break;
    } else {
      do b = ReadByte(fd);
      while ((b != '\n'));
    }
  }
  if (a >= 0300) {
    for (b = 0200; a & b; b >>= 1) {
      a ^= b;
    }
    while ((dx & 0300) == 0200) {
      a <<= 6;
      a |= dx & 0177;
      dx = ReadByte(fd);
    }
  }
  if (0 < a && a < TERM) {
    return ToUpper(a);
  }
  Error("thompson-pike varint outside permitted range");
}

static int ReadListItem(int fd, int closer, int f(int)) {
  int x, y;
  if ((x = f(fd)) > 0) {
    if (Get(x) == MAKE(closer, TERM)) return -0;
    if (Get(x) == MAKE(L'.', TERM)) {
      x = f(fd);
      if ((y = ReadListItem(fd, closer, Read1))) {
        Error("multiple list items after dot: %S", y);
      }
      return x;
    }
  }
  return ShareCons(x, ReadListItem(fd, closer, Read1));
}

static int ReadList(int fd, int closer) {
  int t;
  ++fails;
  t = ReadListItem(fd, closer, Read2);
  --fails;
  return t;
}

static int TokenizeInteger(int fd, int b) {
  dword a;
  int c, i;
  for (i = a = 0;; ++i) {
    if ((c = GetDiglet(ToUpper(dx))) != -1 && c < b) {
      a = (a * b) + c;
      ReadChar(fd);
    } else {
      ax = TERM;
      return Intern(a, TERM);
    }
  }
}

static void ConsumeComment(int fd) {
  int c, t = 1;
  for (;;) {
    c = ReadChar(fd);
    if (c == '#' && dx == '|') ++t;
    if (!t) return;
    if (c == '|' && dx == '#') --t;
  }
}

static int ReadAtomRest(int fd, int x) {
  int y;
  ax = y = TERM;
  if (x == L'\\') x = ReadChar(fd);
  if (!IsSpace(dx) && !IsParen(dx) && !IsMathAlnum(x) && !IsMathAlnum(dx)) {
    y = ReadAtomRest(fd, ReadChar(fd));
  }
  return Intern(x, y);
}

static int ReadAtom(int fd) {
  int a, s, x;
  x = ReadChar(fd);
  if ((s = Desymbolize(x)) != -1) return s;
  a = ReadAtomRest(fd, x);
  if (LO(Get(a)) == L'T' && HI(Get(a)) == TERM) {
    a = 1;
  } else if (LO(Get(a)) == L'N' && HI(Get(a)) != TERM &&
             LO(Get(HI(Get(a)))) == L'I' && HI(Get(HI(Get(a)))) != TERM &&
             LO(Get(HI(Get(HI(Get(a)))))) == L'L' &&
             HI(Get(HI(Get(HI(Get(a)))))) == TERM) {
    a = 0;
  }
  return a;
}

static int TokenizeComplicated(int fd) {
  int c;
  ReadChar(fd);
  switch ((c = ReadChar(fd))) {
    case L'\'':
      return List(kFunction, Read(fd));
    case L'B':
      return TokenizeInteger(fd, 2);
    case L'X':
      return TokenizeInteger(fd, 16);
    case L'Z':
      return TokenizeInteger(fd, 36);
    case L'O':
      return TokenizeInteger(fd, 8);
    case L'|':
      ConsumeComment(fd);
      return Read(fd);
    default:
      Error("unsuppported complicated syntax #%c [0x%x]", c, c);
  }
}

static int Read2(int fd) {
  int r, l;
  while (IsSpace((l = dx))) ReadChar(fd);
  switch (dx) {
    case L'#':
      r = TokenizeComplicated(fd);
      break;
    case L'\'':
      ReadChar(fd);
      r = ShareList(kQuote, Read(fd));
      break;
    case L'`':
      ReadChar(fd);
      r = ShareList(kBackquote, Read(fd));
      break;
    case L',':
      ReadChar(fd);
      if (dx == L'@') {
        ReadChar(fd);
        r = ShareList(kSplice, Read(fd));
      } else {
        r = ShareList(kComma, Read(fd));
      }
      break;
    case L'"':
      r = ShareList(kString, ReadString(fd, ReadByte(fd)));
      break;
    case L'(':
      ReadChar(fd);
      r = ReadList(fd, L')');
      break;
    case L'[':
      ReadChar(fd);
      r = ShareList(kSquare, ReadList(fd, L']'));
      break;
    case L'{':
      ReadChar(fd);
      r = ShareList(kCurly, ReadList(fd, L'}'));
      break;
    default:
      r = ReadAtom(fd);
      break;
  }
  return r;
}

static int ReadLambda(int fd, int n) {
  int a, c, r, q = 0;
  do {
    c = ReadChar(fd);
    if (c == L'λ') {
      for (a = 0; (c = ReadChar(fd)) != '.';) {
        a = Cons(Intern(c, TERM), a);
      }
      for (r = ReadLambda(fd, n); a; a = Cdr(a)) {
        r = List3(kLambda, Cons(Car(a), 0), r);
      }
    } else if (c == L'(') {
      r = ReadLambda(fd, n + 1);
    } else if (c == L')') {
      break;
    } else if (IsSpace(c)) {
      Raise(kRead);
    } else {
      r = Intern(c, TERM);
    }
    if (!q) {
      q = r;
    } else {
      q = List(q, r);
    }
    if (!n && dx == L')') break;
  } while (!IsSpace(dx));
  return q;
}

static int Read1(int fd) {
  while (IsSpace(dx)) ReadChar(fd);
  // todo: fix horrible i/o
  if (dx == 0xCE && (g_buffer[fd][bp[fd]] & 255) == 0xbb) {
    return ReadLambda(fd, 0);
  }
  return Read2(fd);
}

int Read(int fd) {
  int r;
  ftrace_enabled(-1);
  strace_enabled(-1);
  r = Read1(fd);
  strace_enabled(+1);
  ftrace_enabled(+1);
  return r;
}
