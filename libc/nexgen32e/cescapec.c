/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/str/str.h"

/**
 * Converts byte to word-encoded C string literal representation.
 */
int _cescapec(int c) {
  switch ((c &= 255)) {
    case '\a':
      return '\\' | 'a' << 8;
    case '\b':
      return '\\' | 'b' << 8;
    case '\t':
      return '\\' | 't' << 8;
    case '\n':
      return '\\' | 'n' << 8;
    case '\v':
      return '\\' | 'v' << 8;
    case '\f':
      return '\\' | 'f' << 8;
    case '\r':
      return '\\' | 'r' << 8;
    case '"':
      return '\\' | '"' << 8;
    case '\'':
      return '\\' | '\'' << 8;
    case '\\':
      return '\\' | '\\' << 8;
    default:
      if (' ' <= c && c <= '~') {
        return c;
      } else {
        return '\\' |                          //
               ('0' + (c >> 6)) << 8 |         //
               ('0' + ((c >> 3) & 7)) << 16 |  //
               ('0' + (c & 7)) << 24;
      }
  }
}
