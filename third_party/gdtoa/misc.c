/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│ vi: set noet ft=c ts=8 sw=8 fenc=utf-8                                   :vi │
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
#include "libc/calls/calls.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#include "third_party/gdtoa/gdtoa.internal.h"

static ThInfo TI0;
static pthread_mutex_t __gdtoa_lock_obj;
static pthread_mutex_t __gdtoa_lock1_obj;

static void
__gdtoa_lock(void)
{
	pthread_mutex_lock(&__gdtoa_lock_obj);
}

static void
__gdtoa_unlock(void)
{
	pthread_mutex_unlock(&__gdtoa_lock_obj);
}

static void
__gdtoa_initlock(void)
{
	pthread_mutex_init(&__gdtoa_lock_obj, 0);
}

static void
__gdtoa_lock1(void)
{
	pthread_mutex_lock(&__gdtoa_lock1_obj);
}

static void
__gdtoa_unlock1(void)
{
	pthread_mutex_unlock(&__gdtoa_lock1_obj);
}

static void
__gdtoa_initlock1(void)
{
	pthread_mutex_init(&__gdtoa_lock1_obj, 0);
}

static void
__gdtoa_Brelease(Bigint *rv)
{
	if (!rv) return;
	__gdtoa_Brelease(rv->next);
	free(rv);
}

static void
__gdtoa_Bclear(void)
{
	int i;
	__gdtoa_lock();
	for (i = 0; i < ARRAYLEN(TI0.Freelist); ++i)
		__gdtoa_Brelease(TI0.Freelist[i]);
	__gdtoa_lock1();
	__gdtoa_Brelease(TI0.P5s);
	__gdtoa_unlock1();
	bzero(&TI0, sizeof(TI0));
	__gdtoa_unlock();
}

__attribute__((__constructor__)) static void
__gdtoa_Binit(void)
{
	__gdtoa_initlock();
	__gdtoa_initlock1();
	atexit(__gdtoa_Bclear);
	pthread_atfork(__gdtoa_lock1, __gdtoa_unlock1, __gdtoa_initlock1);
	pthread_atfork(__gdtoa_lock, __gdtoa_unlock, __gdtoa_initlock);
}

static ThInfo *
__gdtoa_get_TI(void)
{
	return &TI0;
}

Bigint *
__gdtoa_Balloc(int k, ThInfo **PTI)
{
	int x;
	Bigint *rv;
	ThInfo *TI;
	if (!(TI = *PTI))
		*PTI = TI = __gdtoa_get_TI();
	if (TI == &TI0)
		__gdtoa_lock();
	if (k <= Kmax && (rv = TI->Freelist[k]) != 0) {
		TI->Freelist[k] = rv->next;
	} else {
		x = 1 << k;
		rv = malloc(sizeof(Bigint) + (x-1)*sizeof(ULong));
		rv->k = k;
		rv->maxwds = x;
	}
	if (TI == &TI0)
		__gdtoa_unlock();
	rv->sign = rv->wds = 0;
	return rv;
}

void
__gdtoa_Bfree(Bigint *v, ThInfo **PTI)
{
	ThInfo *TI;
	if (v) {
		if (v->k > Kmax)
			free((void *)v);
		else {
			if (!(TI = *PTI))
				*PTI = TI = __gdtoa_get_TI();
			if (TI == &TI0)
				__gdtoa_lock();
			v->next = TI->Freelist[v->k];
			TI->Freelist[v->k] = v;
			if (TI == &TI0)
				__gdtoa_unlock();
		}
	}
}

Bigint *
__gdtoa_multadd(Bigint *b, int m, int a, ThInfo **PTI)
{
	int i, wds;
	ULong *x;
	uint64_t carry, y;
	Bigint *b1;
	wds = b->wds;
	x = b->x;
	i = 0;
	carry = a;
	do {
		y = *x * (uint64_t)m + carry;
		carry = y >> 32;
		*x++ = y & 0xffffffffUL;
	} while (++i < wds);
	if (carry) {
		if (wds >= b->maxwds) {
			b1 = __gdtoa_Balloc(b->k + 1, PTI);
			memcpy(&b1->sign, &b->sign, b->wds * sizeof(ULong) + 2 * sizeof(int));
			__gdtoa_Bfree(b, PTI);
			b = b1;
		}
		b->x[wds++] = carry;
		b->wds = wds;
	}
	return b;
}

Bigint *
__gdtoa_i2b(int i, ThInfo **PTI)
{
	Bigint *b;
	b = __gdtoa_Balloc(1, PTI);
	b->x[0] = i;
	b->wds = 1;
	return b;
}

Bigint *
__gdtoa_mult(Bigint *a, Bigint *b, ThInfo **PTI)
{
	Bigint *c;
	int k, wa, wb, wc;
	ULong *x, *xa, *xae, *xb, *xbe, *xc, *xc0;
	ULong y;
	uint64_t carry, z;
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
	c = __gdtoa_Balloc(k, PTI);
	for (x = c->x, xa = x + wc; x < xa; x++)
		*x = 0;
	xa = a->x;
	xae = xa + wa;
	xb = b->x;
	xbe = xb + wb;
	xc0 = c->x;
	for (; xb < xbe; xc0++) {
		if ((y = *xb++) != 0) {
			x = xa;
			xc = xc0;
			carry = 0;
			do {
				z = *x++ * (uint64_t)y + *xc + carry;
				carry = z >> 32;
				*xc++ = z & 0xffffffffUL;
			} while (x < xae);
			*xc = carry;
		}
	}
	for (xc0 = c->x, xc = xc0 + wc; wc > 0 && !*--xc; --wc)
		;
	c->wds = wc;
	return c;
}

Bigint *
__gdtoa_pow5mult(Bigint *b, int k, ThInfo **PTI)
{
	Bigint *b1, *p5, *p51;
	ThInfo *TI;
	int i;
	static int p05[3] = {5, 25, 125};
	if ((i = k & 3) != 0)
		b = __gdtoa_multadd(b, p05[i - 1], 0, PTI);
	if (!(k >>= 2))
		return b;
	if (!(TI = *PTI))
		*PTI = TI = __gdtoa_get_TI();
	if ((p5 = TI->P5s) == 0) {
		if (!(TI = *PTI))
			*PTI = TI = __gdtoa_get_TI();
		if (TI == &TI0)
			__gdtoa_lock1();
		if (!(p5 = TI->P5s)) {
			p5 = TI->P5s = __gdtoa_i2b(625, PTI);
			p5->next = 0;
		}
		if (TI == &TI0)
			__gdtoa_unlock1();
	}
	for (;;) {
		if (k & 1) {
			b1 = __gdtoa_mult(b, p5, PTI);
			__gdtoa_Bfree(b, PTI);
			b = b1;
		}
		if (!(k >>= 1))
			break;
		if ((p51 = p5->next) == 0) {
			if (!TI && !(TI = *PTI))
				*PTI = TI = __gdtoa_get_TI();
			if (TI == &TI0)
				__gdtoa_lock1();
			if (!(p51 = p5->next)) {
				p51 = p5->next = __gdtoa_mult(p5, p5, PTI);
				p51->next = 0;
			}
			if (TI == &TI0)
				__gdtoa_unlock1();
		}
		p5 = p51;
	}
	return b;
}

Bigint *
__gdtoa_lshift(Bigint *b, int k, ThInfo **PTI)
{
	int i, k1, n, n1;
	Bigint *b1;
	ULong *x, *x1, *xe, z;
	n = k >> 5;
	k1 = b->k;
	n1 = n + b->wds + 1;
	for (i = b->maxwds; n1 > i; i <<= 1)
		k1++;
	b1 = __gdtoa_Balloc(k1, PTI);
	x1 = b1->x;
	for (i = 0; i < n; i++)
		*x1++ = 0;
	x = b->x;
	xe = x + b->wds;
	if (k &= 31) {
		k1 = 32 - k;
		z = 0;
		do {
			*x1++ = *x << k | z;
			z = *x++ >> k1;
		} while (x < xe);
		if ((*x1 = z) != 0)
			++n1;
	} else
		do
			*x1++ = *x++;
		while (x < xe);
	b1->wds = n1 - 1;
	__gdtoa_Bfree(b, PTI);
	return b1;
}

int
__gdtoa_cmp(Bigint *a, Bigint *b)
{
	ULong *xa, *xa0, *xb, *xb0;
	int i, j;
	i = a->wds;
	j = b->wds;
	if (i -= j)
		return i;
	xa0 = a->x;
	xa = xa0 + j;
	xb0 = b->x;
	xb = xb0 + j;
	for (;;) {
		if (*--xa != *--xb)
			return *xa < *xb ? -1 : 1;
		if (xa <= xa0)
			break;
	}
	return 0;
}

Bigint *
__gdtoa_diff(Bigint *a, Bigint *b, ThInfo **PTI)
{
	Bigint *c;
	int i, wa, wb;
	ULong *xa, *xae, *xb, *xbe, *xc;
	uint64_t borrow, y;
	i = __gdtoa_cmp(a, b);
	if (!i) {
		c = __gdtoa_Balloc(0, PTI);
		c->wds = 1;
		c->x[0] = 0;
		return c;
	}
	if (i < 0) {
		c = a;
		a = b;
		b = c;
		i = 1;
	} else
		i = 0;
	c = __gdtoa_Balloc(a->k, PTI);
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
		y = (uint64_t)*xa++ - *xb++ - borrow;
		borrow = y >> 32 & 1UL;
		*xc++ = y & 0xffffffffUL;
	} while (xb < xbe);
	while (xa < xae) {
		y = *xa++ - borrow;
		borrow = y >> 32 & 1UL;
		*xc++ = y & 0xffffffffUL;
	}
	while (!*--xc)
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
	k = hi0bits((ULong)(y));
	*e = 32 - k;
	if (k < 11) {
		(&d)->L[1] = 0x3ff00000 | y >> (11 - k);
		w = xa > xa0 ? *--xa : 0;
		(&d)->L[0] = y << ((32 - 11) + k) | w >> (11 - k);
		goto ret_d;
	}
	z = xa > xa0 ? *--xa : 0;
	if (k -= 11) {
		(&d)->L[1] = 0x3ff00000 | y << k | z >> (32 - k);
		y = xa > xa0 ? *--xa : 0;
		(&d)->L[0] = z << k | y >> (32 - k);
	} else {
		(&d)->L[1] = 0x3ff00000 | y;
		(&d)->L[0] = z;
	}
ret_d:
	return (&d)->d;
}

Bigint *
__gdtoa_d2b(double dd, int *e, int *bits, ThInfo **PTI)
{
	Bigint *b;
	U d;
	int i;
	int de, k;
	ULong *x, y, z;
	d.d = dd;
	b = __gdtoa_Balloc(1, PTI);
	x = b->x;
	z = (&d)->L[1] & 0xfffff;
	(&d)->L[1] &= 0x7fffffff;
	if ((de = (int)((&d)->L[1] >> 20)) != 0)
		z |= 0x100000;
	if ((y = (&d)->L[0]) != 0) {
		if ((k = lo0bits(&y)) != 0) {
			x[0] = y | z << (32 - k);
			z >>= k;
		} else
			x[0] = y;
		i = b->wds = (x[1] = z) != 0 ? 2 : 1;
	} else {
		k = lo0bits(&z);
		x[0] = z;
		i = b->wds = 1;
		k += 32;
	}
	if (de) {
		*e = de - 1023 - (53 - 1) + k;
		*bits = 53 - k;
	} else {
		*e = de - 1023 - (53 - 1) + 1 + k;
		*bits = 32 * i - hi0bits((ULong)(x[i - 1]));
	}
	return b;
}

const double __gdtoa_bigtens[] = {1e16, 1e32, 1e64, 1e128, 1e256};
const double __gdtoa_tinytens[] = {1e-16, 1e-32, 1e-64, 1e-128, 1e-256};
const double __gdtoa_tens[] = {1e0,  1e1,  1e2,  1e3,  1e4,  1e5,  1e6,  1e7,
                               1e8,  1e9,  1e10, 1e11, 1e12, 1e13, 1e14, 1e15,
                               1e16, 1e17, 1e18, 1e19, 1e20, 1e21, 1e22};
