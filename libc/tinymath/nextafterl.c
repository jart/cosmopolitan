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

union ldshape {
	long double f;
	struct {
		uint64_t m;
		uint16_t se;
	} i;
};

static inline void force_eval(long double x)
{
	volatile long double y;
	y = x;
}

long double nextafterl(long double x, long double y)
{
	union ldshape ux, uy;

	if (isnan(x) || isnan(y))
		return x + y;
	if (x == y)
		return y;
	ux.f = x;
	if (x == 0) {
		uy.f = y;
		ux.i.m = 1;
		ux.i.se = uy.i.se & 0x8000;
	} else if ((x < y) == !(ux.i.se & 0x8000)) {
		ux.i.m++;
		if (ux.i.m << 1 == 0) {
			ux.i.m = 1ULL << 63;
			ux.i.se++;
		}
	} else {
		if (ux.i.m << 1 == 0) {
			ux.i.se--;
			if (ux.i.se)
				ux.i.m = 0;
		}
		ux.i.m--;
	}
	/* raise overflow if ux is infinite and x is finite */
	if ((ux.i.se & 0x7fff) == 0x7fff)
		return x + x;
	/* raise underflow if ux is subnormal or zero */
	if ((ux.i.se & 0x7fff) == 0)
		force_eval(x*x + ux.f*ux.f);
	return ux.f;
}
