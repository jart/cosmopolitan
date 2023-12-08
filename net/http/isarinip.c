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
 * Returns true if IPv4 address is managed by ARIN.
 */
bool IsArinIp(uint32_t x) {
  int a = (x & 0xff000000) >> 24;
  return a == 3 || a == 4 || a == 8 || a == 9 || a == 13 || a == 15 ||
         a == 16 || a == 18 || a == 20 || a == 23 || a == 24 || a == 32 ||
         a == 34 || a == 35 || a == 40 || a == 44 || a == 45 || a == 47 ||
         a == 50 || a == 52 || a == 54 || a == 63 || a == 64 || a == 65 ||
         a == 66 || a == 67 || a == 68 || a == 69 || a == 70 || a == 71 ||
         a == 72 || a == 74 || a == 75 || a == 76 || a == 96 || a == 97 ||
         a == 98 || a == 99 || a == 100 || a == 104 || a == 107 || a == 108 ||
         a == 128 || a == 129 || a == 130 || a == 131 || a == 132 || a == 134 ||
         a == 135 || a == 136 || a == 137 || a == 138 || a == 139 || a == 140 ||
         a == 142 || a == 143 || a == 144 || a == 146 || a == 147 || a == 148 ||
         a == 149 || a == 152 || a == 155 || a == 156 || a == 157 || a == 158 ||
         a == 159 || a == 160 || a == 161 || a == 162 || a == 164 || a == 165 ||
         a == 166 || a == 167 || a == 168 || a == 169 || a == 170 || a == 172 ||
         a == 173 || a == 174 || a == 184 || a == 192 || a == 198 || a == 199 ||
         a == 204 || a == 205 || a == 206 || a == 207 || a == 208 || a == 209 ||
         a == 216;
}
