/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│vi: set et ft=c ts=8 tw=8 fenc=utf-8                                       :vi│
╚──────────────────────────────────────────────────────────────────────────────╝
│                                                                              │
│  The author of this software is David M. Gay.                                │
│  Please send bug reports to David M. Gay <dmg@acm.org>                       │
│                          or Justine Tunney <jtunney@gmail.com>               │
│                                                                              │
│  Copyright (C) 1998, 1999 by Lucent Technologies                             │
│  All Rights Reserved                                                         │
│                                                                              │
│  Permission to use, copy, modify, and distribute this software and           │
│  its documentation for any purpose and without fee is hereby                 │
│  granted, provided that the above copyright notice appear in all             │
│  copies and that both that the copyright notice and this                     │
│  permission notice and warranty disclaimer appear in supporting              │
│  documentation, and that the name of Lucent or any of its entities           │
│  not be used in advertising or publicity pertaining to                       │
│  distribution of the software without specific, written prior                │
│  permission.                                                                 │
│                                                                              │
│  LUCENT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,               │
│  INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.            │
│  IN NO EVENT SHALL LUCENT OR ANY OF ITS ENTITIES BE LIABLE FOR ANY           │
│  SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES                   │
│  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER             │
│  IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,              │
│  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF              │
│  THIS SOFTWARE.                                                              │
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/assert.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "third_party/gdtoa/gdtoa.internal.h"
/* clang-format off */

static ThInfo TI0;

static void
__gdtoa_Brelease(Bigint *rv)
{
	if (!rv) return;
	__gdtoa_Brelease(rv->next);
	free(rv);
}

static void
Bclear(void)
{
	int i;
	for (i = 0; i < ARRAYLEN(TI0.Freelist); ++i)
		__gdtoa_Brelease(TI0.Freelist[i]);
	bzero(&TI0.Freelist, sizeof(TI0.Freelist));
}

Bigint *
__gdtoa_Balloc(int k)
{
#if 0
	int x;
	Bigint *rv;
	x = 1 << k;
	rv = (Bigint *)malloc(sizeof(Bigint) + (x-1)*sizeof(ULong));
	rv->k = k;
	rv->maxwds = x;
	rv->sign = 0;
	rv->wds = 0;
	return rv;
#else
	int x;
	Bigint *rv;
	static char once;
	if (!once) {
		atexit(Bclear);
		once = 1;
	}
	if (k <= Kmax && (rv = TI0.Freelist[k]) != 0) {
		TI0.Freelist[k] = rv->next;
	} else {
		x = 1 << k;
		rv = (Bigint *)malloc(sizeof(Bigint) + (x-1)*sizeof(ULong));
		rv->k = k;
		rv->maxwds = x;
	}
	rv->sign = 0;
	rv->wds = 0;
	return rv;
#endif
}

void
__gdtoa_Bfree(Bigint *v)
{
#if 0
	free(v);
#else
	if (v) {
		if (v->k > Kmax) {
			free((void*)v);
		} else {
			v->next = TI0.Freelist[v->k];
			TI0.Freelist[v->k] = v;
		}
	}
#endif
}

Bigint *
__gdtoa_multadd(Bigint *b, int m, int a)	/* multiply by m and add a */
{
	int i, wds;
	ULong *x;
	ULLong carry, y;
	Bigint *b1;
	wds = b->wds;
	x = b->x;
	i = 0;
	carry = a;
	do {
		y = *x * (ULLong)m + carry;
		carry = y >> 32;
		*x++ = y & 0xffffffffUL;
	}
	while(++i < wds);
	if (carry) {
		if (wds >= b->maxwds) {
			b1 = __gdtoa_Balloc(b->k+1);
			Bcopy(b1, b);
			__gdtoa_Bfree(b);
			b = b1;
		}
		b->x[wds++] = carry;
		b->wds = wds;
	}
	return b;
}

Bigint *
__gdtoa_i2b(int i)
{
	Bigint *b;
	b = __gdtoa_Balloc(1);
	b->x[0] = i;
	b->wds = 1;
	return b;
}

Bigint *
__gdtoa_mult(Bigint *a, Bigint *b)
{
	Bigint *c;
	int k, wa, wb, wc;
	ULong *x, *xa, *xae, *xb, *xbe, *xc, *xc0;
	ULong y;
	ULLong carry, z;
	if (a->wds < b->wds) {
		c = a;
		a = b;
		b = c;
	}
	k = a->k;
	wa = a->wds;
	wb = b->wds;
	wc = wa + wb;
	if (wc > a->maxwds)
		k++;
	c = __gdtoa_Balloc(k);
	for(x = c->x, xa = x + wc; x < xa; x++)
		*x = 0;
	xa = a->x;
	xae = xa + wa;
	xb = b->x;
	xbe = xb + wb;
	xc0 = c->x;
	for(; xb < xbe; xc0++) {
		if ( (y = *xb++) !=0) {
			x = xa;
			xc = xc0;
			carry = 0;
			do {
				z = *x++ * (ULLong)y + *xc + carry;
				carry = z >> 32;
				*xc++ = z & 0xffffffffUL;
			}
			while(x < xae);
			*xc = carry;
		}
	}
	for(xc0 = c->x, xc = xc0 + wc; wc > 0 && !*--xc; --wc) ;
	c->wds = wc;
	return c;
}

static void
__gdtoa_pow5mult_destroy(void) {
	__gdtoa_Brelease(TI0.P5s);
	TI0.P5s = 0;
}

Bigint *
__gdtoa_pow5mult(Bigint *b, int k)
{
	Bigint *b1, *p5, *p51;
	int i;
	static const int p05[3] = { 5, 25, 125 };
	if ((i = k & 3))
		b = __gdtoa_multadd(b, p05[i-1], 0);
	if (!(k >>= 2))
		return b;
	if (!(p5 = TI0.P5s)) {
		p5 = TI0.P5s = __gdtoa_i2b(625);
		p5->next = 0;
		atexit(__gdtoa_pow5mult_destroy);
	}
	for(;;) {
		if (k & 1) {
			b1 = __gdtoa_mult(b, p5);
			__gdtoa_Bfree(b);
			b = b1;
		}
		if (!(k >>= 1))
			break;
		if ((p51 = p5->next) == 0) {
			p51 = p5->next = __gdtoa_mult(p5,p5);
			p51->next = 0;
		}
		p5 = p51;
	}
	return b;
}

Bigint *
__gdtoa_lshift(Bigint *b, int k)
{
	int i, k1, n, n1;
	Bigint *b1;
	ULong *x, *x1, *xe, z;
	n = k >> kshift;
	k1 = b->k;
	n1 = n + b->wds + 1;
	for(i = b->maxwds; n1 > i; i <<= 1)
		k1++;
	b1 = __gdtoa_Balloc(k1);
	x1 = b1->x;
	for(i = 0; i < n; i++)
		*x1++ = 0;
	x = b->x;
	xe = x + b->wds;
	if (k &= kmask) {
		k1 = 32 - k;
		z = 0;
		do {
			*x1++ = *x << k | z;
			z = *x++ >> k1;
		}
		while(x < xe);
		if ((*x1 = z) !=0)
			++n1;
	}
	else do
		     *x1++ = *x++;
		while(x < xe);
	b1->wds = n1 - 1;
	__gdtoa_Bfree(b);
	return b1;
}

int
__gdtoa_cmp(Bigint *a, Bigint *b)
{
	ULong *xa, *xa0, *xb, *xb0;
	int i, j;
	i = a->wds;
	j = b->wds;
#ifdef DEBUG
	if (i > 1 && !a->x[i-1])
		Bug("__gdtoa_cmp called with a->x[a->wds-1] == 0");
	if (j > 1 && !b->x[j-1])
		Bug("__gdtoa_cmp called with b->x[b->wds-1] == 0");
#endif
	if (i -= j)
		return i;
	xa0 = a->x;
	xa = xa0 + j;
	xb0 = b->x;
	xb = xb0 + j;
	for(;;) {
		if (*--xa != *--xb)
			return *xa < *xb ? -1 : 1;
		if (xa <= xa0)
			break;
	}
	return 0;
}

Bigint *
__gdtoa_diff(Bigint *a, Bigint *b)
{
	Bigint *c;
	int i, wa, wb;
	ULong *xa, *xae, *xb, *xbe, *xc;
	ULLong borrow, y;
	i = __gdtoa_cmp(a,b);
	if (!i) {
		c = __gdtoa_Balloc(0);
		c->wds = 1;
		c->x[0] = 0;
		return c;
	}
	if (i < 0) {
		c = a;
		a = b;
		b = c;
		i = 1;
	}
	else
		i = 0;
	c = __gdtoa_Balloc(a->k);
	c->sign = i;
	wa = a->wds;
	xa = a->x;
	xae = xa + wa;
	wb = b->wds;
	xb = b->x;
	xbe = xb + wb;
	xc = c->x;
	borrow = 0;
	do {
		y = (ULLong)*xa++ - *xb++ - borrow;
		borrow = y >> 32 & 1UL;
		*xc++ = y & 0xffffffffUL;
	}
	while(xb < xbe);
	while(xa < xae) {
		y = *xa++ - borrow;
		borrow = y >> 32 & 1UL;
		*xc++ = y & 0xffffffffUL;
	}
	while(!*--xc)
		wa--;
	c->wds = wa;
	return c;
}

double
__gdtoa_b2d(Bigint *a, int *e)
{
	ULong *xa, *xa0, w, y, z;
	int k;
	U d;
	xa0 = a->x;
	xa = xa0 + a->wds;
	y = *--xa;
#ifdef DEBUG
	if (!y) Bug("zero y in __gdtoa_b2d");
#endif
	k = hi0bits(y);
	*e = 32 - k;
	if (k < Ebits) {
		word0(&d) = Exp_1 | y >> (Ebits - k);
		w = xa > xa0 ? *--xa : 0;
		word1(&d) = y << ((32-Ebits) + k) | w >> (Ebits - k);
		goto ret_d;
	}
	z = xa > xa0 ? *--xa : 0;
	if (k -= Ebits) {
		word0(&d) = Exp_1 | y << k | z >> (32 - k);
		y = xa > xa0 ? *--xa : 0;
		word1(&d) = z << k | y >> (32 - k);
	}
	else {
		word0(&d) = Exp_1 | y;
		word1(&d) = z;
	}
ret_d:
	return dval(&d);
}

Bigint *
__gdtoa_d2b(double dd, int *e, int *bits)
{
	Bigint *b;
	U d;
	int i;
	int de, k;
	ULong *x, y, z;
	d.d = dd;
	b = __gdtoa_Balloc(1);
	x = b->x;
	z = word0(&d) & Frac_mask;
	word0(&d) &= 0x7fffffff;	/* clear sign bit, which we ignore */
	if ( (de = (int)(word0(&d) >> Exp_shift)) !=0)
		z |= Exp_msk1;
	if ( (y = word1(&d)) !=0) {
		if ( (k = lo0bits(&y)) !=0) {
			x[0] = y | z << (32 - k);
			z >>= k;
		}
		else
			x[0] = y;
		i = b->wds = (x[1] = z) !=0 ? 2 : 1;
	}
	else {
		k = lo0bits(&z);
		x[0] = z;
		i = b->wds = 1;
		k += 32;
	}
	if (de) {
		*e = de - Bias - (P-1) + k;
		*bits = P - k;
	}
	else {
		*e = de - Bias - (P-1) + 1 + k;
		*bits = 32*i - hi0bits(x[i-1]);
	}
	return b;
}

const double
__gdtoa_bigtens[] = { 1e16, 1e32, 1e64, 1e128, 1e256 };

const double
__gdtoa_tinytens[] = { 1e-16, 1e-32, 1e-64, 1e-128, 1e-256 };

const double
__gdtoa_tens[] = {
	1e0, 1e1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9,
	1e10, 1e11, 1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19,
	1e20, 1e21, 1e22
};
