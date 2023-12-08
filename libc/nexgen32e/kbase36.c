/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/str/tab.internal.h"

_Alignas(uint8_t) const uint8_t kBase36[256] = {
    ['0'] = 1,   //
    ['1'] = 2,   //
    ['2'] = 3,   //
    ['3'] = 4,   //
    ['4'] = 5,   //
    ['5'] = 6,   //
    ['6'] = 7,   //
    ['7'] = 8,   //
    ['8'] = 9,   //
    ['9'] = 10,  //
    ['A'] = 11,  //
    ['B'] = 12,  //
    ['C'] = 13,  //
    ['D'] = 14,  //
    ['E'] = 15,  //
    ['F'] = 16,  //
    ['G'] = 17,  //
    ['H'] = 18,  //
    ['I'] = 19,  //
    ['J'] = 20,  //
    ['K'] = 21,  //
    ['L'] = 22,  //
    ['M'] = 23,  //
    ['N'] = 24,  //
    ['O'] = 25,  //
    ['P'] = 26,  //
    ['Q'] = 27,  //
    ['R'] = 28,  //
    ['S'] = 29,  //
    ['T'] = 30,  //
    ['U'] = 31,  //
    ['V'] = 32,  //
    ['W'] = 33,  //
    ['X'] = 34,  //
    ['Y'] = 35,  //
    ['Z'] = 36,  //
    ['a'] = 11,  //
    ['b'] = 12,  //
    ['c'] = 13,  //
    ['d'] = 14,  //
    ['e'] = 15,  //
    ['f'] = 16,  //
    ['g'] = 17,  //
    ['h'] = 18,  //
    ['i'] = 19,  //
    ['j'] = 20,  //
    ['k'] = 21,  //
    ['l'] = 22,  //
    ['m'] = 23,  //
    ['n'] = 24,  //
    ['o'] = 25,  //
    ['p'] = 26,  //
    ['q'] = 27,  //
    ['r'] = 28,  //
    ['s'] = 29,  //
    ['t'] = 30,  //
    ['u'] = 31,  //
    ['v'] = 32,  //
    ['w'] = 33,  //
    ['x'] = 34,  //
    ['y'] = 35,  //
    ['z'] = 36,  //
};
