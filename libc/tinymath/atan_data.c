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
#include "libc/tinymath/atan_data.internal.h"

asm(".ident\t\"\\n\\n\
Optimized Routines (MIT License)\\n\
Copyright 2022 ARM Limited\"");
asm(".include \"libc/disclaimer.inc\"");
/* clang-format off */

const struct atan_poly_data __atan_poly_data = {
  .poly = {/* Coefficients of polynomial P such that atan(x)~x+x*P(x^2) on
	      [2**-1022, 1.0]. See atan.sollya for details of how these were
	      generated.  */
	   -0x1.5555555555555p-2,  0x1.99999999996c1p-3,  -0x1.2492492478f88p-3,
	   0x1.c71c71bc3951cp-4,   -0x1.745d160a7e368p-4, 0x1.3b139b6a88ba1p-4,
	   -0x1.11100ee084227p-4,  0x1.e1d0f9696f63bp-5,  -0x1.aebfe7b418581p-5,
	   0x1.842dbe9b0d916p-5,   -0x1.5d30140ae5e99p-5, 0x1.338e31eb2fbbcp-5,
	   -0x1.00e6eece7de8p-5,   0x1.860897b29e5efp-6,  -0x1.0051381722a59p-6,
	   0x1.14e9dc19a4a4ep-7,   -0x1.d0062b42fe3bfp-9, 0x1.17739e210171ap-10,
	   -0x1.ab24da7be7402p-13, 0x1.358851160a528p-16}};
