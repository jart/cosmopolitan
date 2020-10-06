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
#include "net/http/http.h"

unsigned ParseHttpVersion(const char *p, size_t n) {
  unsigned x;
  if (n >= 8 && READ32LE(p) == ('H' | 'T' << 8 | 'T' << 16 | 'P' << 24)) {
    if (READ32LE(p + 4) == ('/' | '1' << 8 | '.' << 16 | '1' << 24)) {
      return 101;
    } else if (READ32LE(p + 4) == ('/' | '1' << 8 | '.' << 16 | '0' << 24)) {
      return 100;
    }
  }
  return -1;
}
