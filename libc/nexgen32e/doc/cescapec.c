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
#include "libc/str/internal.h"

unsigned cescapec(int c) {
  unsigned char ch = c;
  switch (ch) {
    case '\a':
      return '\\' | 'a' << 8;
    case '\b':
      return '\\' | 'b' << 8;
    case '\v':
      return '\\' | 'v' << 8;
    case '\f':
      return '\\' | 'f' << 8;
    case '\?':
      return '\\' | '?' << 8;
    case '\n':
      return '\\' | 'n' << 8;
    case '\r':
      return '\\' | 'r' << 8;
    case '\t':
      return '\\' | 't' << 8;
    case '\"':
      return '\\' | '"' << 8;
    case '\'':
      return '\\' | '\'' << 8;
    case '\\':
      return '\\' | '\\' << 8;
    default: {
      if (ch >= 0x80 || !isprint(ch)) {
        return '\\' | (ch / 64 + '0') << 8 | (ch % 64 / 8 + '0') << 16 |
               (ch % 8 + '0') << 24;
      } else {
        return ch;
      }
    }
  }
}
