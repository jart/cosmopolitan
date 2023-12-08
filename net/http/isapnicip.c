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
 * Returns true if IPv4 address is managed by APNIC.
 */
bool IsApnicIp(uint32_t x) {
  int a = (x & 0xff000000) >> 24;
  return a == 1 || a == 14 || a == 27 || a == 36 || a == 39 || a == 42 ||
         a == 43 || a == 49 || a == 58 || a == 59 || a == 60 || a == 61 ||
         a == 101 || a == 103 || a == 106 || a == 110 || a == 111 || a == 112 ||
         a == 113 || a == 114 || a == 115 || a == 116 || a == 117 || a == 118 ||
         a == 119 || a == 120 || a == 121 || a == 122 || a == 123 || a == 124 ||
         a == 125 || a == 126 || a == 133 || a == 150 || a == 153 || a == 163 ||
         a == 171 || a == 175 || a == 180 || a == 182 || a == 183 || a == 202 ||
         a == 203 || a == 210 || a == 211 || a == 218 || a == 219 || a == 220 ||
         a == 221 || a == 222 || a == 223;
}
