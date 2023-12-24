/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=8 sts=2 sw=2 fenc=utf-8                               :vi │
╚──────────────────────────────────────────────────────────────────────────────╝
│                                                                              │
│  Optimized Routines                                                          │
│  Copyright (c) 1999-2022, Arm Limited.                                       │
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
#include "libc/tinymath/sincosf.internal.h"

asm(".ident\t\"\\n\\n\
Optimized Routines (MIT License)\\n\
Copyright 2022 ARM Limited\"");
asm(".include \"libc/disclaimer.inc\"");
/* clang-format off */

/* The constants and polynomials for sine and cosine.  The 2nd entry
   computes -cos (x) rather than cos (x) to get negation for free.  */
const sincos_t __sincosf_table[2] =
{
  {
    { 1.0, -1.0, -1.0, 1.0 },
#if TOINT_INTRINSICS
    0x1.45F306DC9C883p-1,
#else
    0x1.45F306DC9C883p+23,
#endif
    0x1.921FB54442D18p0,
    0x1p0,
    -0x1.ffffffd0c621cp-2,
    0x1.55553e1068f19p-5,
    -0x1.6c087e89a359dp-10,
    0x1.99343027bf8c3p-16,
    -0x1.555545995a603p-3,
    0x1.1107605230bc4p-7,
    -0x1.994eb3774cf24p-13
  },
  {
    { 1.0, -1.0, -1.0, 1.0 },
#if TOINT_INTRINSICS
    0x1.45F306DC9C883p-1,
#else
    0x1.45F306DC9C883p+23,
#endif
    0x1.921FB54442D18p0,
    -0x1p0,
    0x1.ffffffd0c621cp-2,
    -0x1.55553e1068f19p-5,
    0x1.6c087e89a359dp-10,
    -0x1.99343027bf8c3p-16,
    -0x1.555545995a603p-3,
    0x1.1107605230bc4p-7,
    -0x1.994eb3774cf24p-13
  }
};

/* Table with 4/PI to 192 bit precision.  To avoid unaligned accesses
   only 8 new bits are added per entry, making the table 4 times larger.  */
const uint32_t __inv_pio4[24] =
{
  0xa2,       0xa2f9,	  0xa2f983,   0xa2f9836e,
  0xf9836e4e, 0x836e4e44, 0x6e4e4415, 0x4e441529,
  0x441529fc, 0x1529fc27, 0x29fc2757, 0xfc2757d1,
  0x2757d1f5, 0x57d1f534, 0xd1f534dd, 0xf534ddc0,
  0x34ddc0db, 0xddc0db62, 0xc0db6295, 0xdb629599,
  0x6295993c, 0x95993c43, 0x993c4390, 0x3c439041
};
