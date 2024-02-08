/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│ vi: set noet ft=c ts=8 sw=8 fenc=utf-8                                   :vi │
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
#include "libc/math.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");
// clang-format off

float hypotf(float x, float y)
{
	union {float f; uint32_t i;} ux = {x}, uy = {y}, ut;
	float_t z;

	ux.i &= -1U>>1;
	uy.i &= -1U>>1;
	if (ux.i < uy.i) {
		ut = ux;
		ux = uy;
		uy = ut;
	}

	x = ux.f;
	y = uy.f;
	if (uy.i == 0xff<<23)
		return y;
	if (ux.i >= 0xff<<23 || uy.i == 0 || ux.i - uy.i >= 25<<23)
		return x + y;

	z = 1;
	if (ux.i >= (0x7f+60)<<23) {
		z = 0x1p90f;
		x *= 0x1p-90f;
		y *= 0x1p-90f;
	} else if (uy.i < (0x7f-60)<<23) {
		z = 0x1p-90f;
		x *= 0x1p90f;
		y *= 0x1p90f;
	}
	return z*sqrtf((double)x*x + (double)y*y);
}
