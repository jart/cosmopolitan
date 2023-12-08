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
#include "tool/plinko/lib/char.h"

pureconst bool IsHex(int c) {
  return ((L'0' <= c && c <= L'9') || (L'A' <= c && c <= L'F') ||
          (L'a' <= c && c <= L'f'));
}

pureconst int GetDiglet(int c) {
  if (IsDigit(c)) return c - L'0';
  if (IsUpper(c)) return c - L'A' + 10;
  if (IsLower(c)) return c - L'a' + 10;
  return -1;
}

pureconst bool IsSpace(int c) {
  switch (c) {
    case L' ':
    case L'\t':
    case L'\n':
    case L'\f':
    case L'\v':
    case L'\r':
      return true;
    default:
      return false;
  }
}

pureconst bool IsParen(int c) {
  switch (c) {
    case L'(':
    case L')':
    case L'[':
    case L']':
    case L'{':
    case L'}':
      return true;
    default:
      return false;
  }
}
