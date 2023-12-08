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
#include "tool/build/lib/case.h"
#include "tool/plinko/lib/char.h"
#include "tool/plinko/lib/plinko.h"

int ReadString(int fd, unsigned x) {
  int i, n, y, z;
  ax = y = TERM;
  if (x == L'"') {
    dx = ReadByte(fd);
    return ax;
  } else {
    z = ReadByte(fd);
    if (x == L'\\') {
      x = z;
      z = ReadByte(fd);
      switch (x) {
        CASE(L'a', x = L'\a');
        CASE(L'b', x = L'\b');
        CASE(L'e', x = 00033);
        CASE(L'f', x = L'\f');
        CASE(L'n', x = L'\n');
        CASE(L'r', x = L'\r');
        CASE(L't', x = L'\t');
        CASE(L'v', x = L'\v');
        case L'x':
          n = 2;
          goto ReadHexEscape;
        case L'u':
          n = 4;
          goto ReadHexEscape;
        case L'U':
          n = 8;
          goto ReadHexEscape;
        default:
          if (IsDigit(x)) {
            x = GetDiglet(x);
            for (i = 0; IsDigit(z) && i < 2; ++i) {
              x *= 8;
              x += GetDiglet(z);
              z = ReadByte(fd);
            }
          }
          break;
        ReadHexEscape:
          for (x = i = 0; IsHex(z) && i < n; ++i) {
            x *= 16;
            x += GetDiglet(z);
            z = ReadByte(fd);
          }
          break;
      }
    }
    y = ReadString(fd, z);
  }
  return Intern(x, y);
}
