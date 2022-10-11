/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│vi: set et ft=c ts=8 tw=8 fenc=utf-8                                       :vi│
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
#include "libc/tinymath/internal.h"
#include "third_party/libcxx/math.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");
// clang-format off

float floorf(float x)
{
	union {float f; uint32_t i;} u = {x};
	int e = (int)(u.i >> 23 & 0xff) - 0x7f;
	uint32_t m;

	if (e >= 23)
		return x;
	if (e >= 0) {
		m = 0x007fffff >> e;
		if ((u.i & m) == 0)
			return x;
		FORCE_EVAL(x + 0x1p120f);
		if (u.i >> 31)
			u.i += m;
		u.i &= ~m;
	} else {
		FORCE_EVAL(x + 0x1p120f);
		if (u.i >> 31 == 0)
			u.i = 0;
		else if (u.i << 1)
			u.f = -1.0;
	}
	return u.f;
}
