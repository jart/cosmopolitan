/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "net/http/ip.h"

/**
 * Returns true if IPv4 address is managed by RIPE NCC.
 */
bool IsRipeIp(uint32_t x) {
  int a = (x & 0xff000000) >> 24;
  return a == 2 || a == 5 || a == 25 || a == 31 || a == 37 || a == 46 ||
         a == 51 || a == 53 || a == 57 || a == 62 || a == 77 || a == 78 ||
         a == 79 || a == 80 || a == 81 || a == 82 || a == 83 || a == 84 ||
         a == 85 || a == 86 || a == 87 || a == 88 || a == 89 || a == 90 ||
         a == 91 || a == 92 || a == 93 || a == 94 || a == 95 || a == 109 ||
         a == 141 || a == 145 || a == 151 || a == 176 || a == 178 || a == 185 ||
         a == 188 || a == 193 || a == 194 || a == 195 || a == 212 || a == 213 ||
         a == 217;
}
