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
#include "third_party/musl/rand48.h"
#include "libc/str/str.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");

static unsigned short __seed48[7] = { 0, 0, 0, 0xe66d, 0xdeec, 0x5, 0xb };

static uint64_t __rand48_step(unsigned short *xi, unsigned short *lc)
{
	uint64_t a, x;
	x = xi[0] | (xi[1]+0U)<<16 | (xi[2]+0ULL)<<32;
	a = lc[0] | (lc[1]+0U)<<16 | (lc[2]+0ULL)<<32;
	x = a*x + lc[3];
	xi[0] = x;
	xi[1] = x>>16;
	xi[2] = x>>32;
	return x & 0xffffffffffffull;
}

double erand48(unsigned short s[3])
{
	union {
		uint64_t u;
		double f;
	} x = { 0x3ff0000000000000ULL | __rand48_step(s, __seed48+3)<<4 };
	return x.f - 1.0;
}

double drand48(void)
{
	return erand48(__seed48);
}

void lcong48(unsigned short p[7])
{
	memcpy(__seed48, p, sizeof __seed48);
}

long nrand48(unsigned short s[3])
{
	return __rand48_step(s, __seed48+3) >> 17;
}

long lrand48(void)
{
	return nrand48(__seed48);
}

long jrand48(unsigned short s[3])
{
	return (int32_t)(__rand48_step(s, __seed48+3) >> 16);
}

long mrand48(void)
{
	return jrand48(__seed48);
}

unsigned short *seed48(unsigned short s[3])
{
	static unsigned short p[3];
	memcpy(p, __seed48, sizeof p);
	memcpy(__seed48, s, sizeof p);
	return p;
}

void srand48(long seed)
{
	seed48((unsigned short [3]){ 0x330e, seed, seed>>16 });
}
