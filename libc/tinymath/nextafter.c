/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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

/* clang-format off */

static inline void force_eval(double x)
{
	volatile double y;
	y = x;
}

double nextafter(double x, double y)
{
	union {double f; uint64_t i;} ux={x}, uy={y};
	uint64_t ax, ay;
	int e;

	if (isnan(x) || isnan(y))
		return x + y;
	if (ux.i == uy.i)
		return y;
	ax = ux.i & -1ULL/2;
	ay = uy.i & -1ULL/2;
	if (ax == 0) {
		if (ay == 0)
			return y;
		ux.i = (uy.i & 1ULL<<63) | 1;
	} else if (ax > ay || ((ux.i ^ uy.i) & 1ULL<<63))
		ux.i--;
	else
		ux.i++;
	e = ux.i >> 52 & 0x7ff;
	/* raise overflow if ux.f is infinite and x is finite */
	if (e == 0x7ff)
		force_eval(x+x);
	/* raise underflow if ux.f is subnormal or zero */
	if (e == 0)
		force_eval(x*x + ux.f*ux.f);
	return ux.f;
}
