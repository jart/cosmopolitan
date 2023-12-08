/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╚──────────────────────────────────────────────────────────────────────────────╝
│                                                                              │
│  Musl Libc                                                                   │
│  Copyright © 2005-2014 Rich Felker, et al.                                   │
│                                                                              │
│  Permission is hereby granted, free of charge, to any person obtaining       │
│  a copy of this software and associated documentation files (the             │
│  "Software"), to deal in the Software without restriction, including         │
│  without limitation the rights to use, copy, modify, merge, publish,         │
│  distribute, sublicense, and/or sell copies of the Software, and to          │
│  permit persons to whom the Software is furnished to do so, subject to       │
│  the following conditions:                                                   │
│                                                                              │
│  The above copyright notice and this permission notice shall be              │
│  included in all copies or substantial portions of the Software.             │
│                                                                              │
│  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,             │
│  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF          │
│  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.      │
│  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY        │
│  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,        │
│  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE           │
│  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                      │
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/str/mb.internal.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");

#define C(x) (x < 2 ? -1 : (R(0x80, 0xc0) | x))
#define D(x) C((x + 16))
#define E(x)                      \
  ((x == 0     ? R(0xa0, 0xc0)    \
    : x == 0xd ? R(0x80, 0xa0)    \
               : R(0x80, 0xc0)) | \
   (R(0x80, 0xc0) >> 6) | x)
#define F(x)                    \
  ((x >= 5   ? 0                \
    : x == 0 ? R(0x90, 0xc0)    \
    : x == 4 ? R(0x80, 0x90)    \
             : R(0x80, 0xc0)) | \
   (R(0x80, 0xc0) >> 6) | (R(0x80, 0xc0) >> 12) | x)

const uint32_t kMbBittab[51 /* ?! */] = {
    C(0x2), C(0x3), C(0x4), C(0x5), C(0x6), C(0x7), C(0x8), C(0x9), C(0xa),
    C(0xb), C(0xc), C(0xd), C(0xe), C(0xf), D(0x0), D(0x1), D(0x2), D(0x3),
    D(0x4), D(0x5), D(0x6), D(0x7), D(0x8), D(0x9), D(0xa), D(0xb), D(0xc),
    D(0xd), D(0xe), D(0xf), E(0x0), E(0x1), E(0x2), E(0x3), E(0x4), E(0x5),
    E(0x6), E(0x7), E(0x8), E(0x9), E(0xa), E(0xb), E(0xc), E(0xd), E(0xe),
    E(0xf), F(0x0), F(0x1), F(0x2), F(0x3), F(0x4),
};
