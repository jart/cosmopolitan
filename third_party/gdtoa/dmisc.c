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
#include "third_party/gdtoa/gdtoa.internal.h"

void
freedtoa(char *s)
{
	ThInfo *TI = 0;
	Bigint *b = (Bigint *)((int *)s - 1);
	b->maxwds = 1 << (b->k = *(int *)b);
	__gdtoa_Bfree(b, &TI);
}

char *
__gdtoa_rv_alloc(int i, ThInfo **PTI)
{
	int j, k, *r;
	j = sizeof(ULong);
	for (k = 0; (int)(sizeof(Bigint) - sizeof(ULong) - sizeof(int)) + j <= i;
	     j <<= 1)
		k++;
	r = (int *)__gdtoa_Balloc(k, PTI);
	*r = k;
	return (char *)(r + 1);
}

char *
__gdtoa_nrv_alloc(char *s, char **rve, int n, ThInfo **PTI)
{
	char *rv, *t;
	t = rv = __gdtoa_rv_alloc(n, PTI);
	while ((*t = *s++) != 0)
		t++;
	if (rve)
		*rve = t;
	return rv;
}

int
__gdtoa_quorem(Bigint *b, Bigint *S)
{
	int n;
	ULong *bx, *bxe, q, *sx, *sxe;
	ULLong borrow, carry, y, ys;
	n = S->wds;
#ifdef DEBUG
	if (b->wds > n)
		Bug("oversize b in __gdtoa_quorem");
#endif
	if (b->wds < n)
		return 0;
	sx = S->x;
	sxe = sx + --n;
	bx = b->x;
	bxe = bx + n;
	q = *bxe / (*sxe + 1);	/* ensure q <= true quotient */
#ifdef DEBUG
	if (q > 9)
		Bug("oversized quotient in __gdtoa_quorem");
#endif
	if (q) {
		borrow = 0;
		carry = 0;
		do {
			ys = *sx++ * (ULLong)q + carry;
			carry = ys >> 32;
			y = *bx - (ys & 0xffffffff) - borrow;
			borrow = y >> 32 & 1UL;
			*bx++ = y & 0xffffffff;
		}
		while(sx <= sxe);
		if (!*bxe) {
			bx = b->x;
			while(--bxe > bx && !*bxe)
				--n;
			b->wds = n;
		}
	}
	if (__gdtoa_cmp(b, S) >= 0) {
		q++;
		borrow = 0;
		carry = 0;
		bx = b->x;
		sx = S->x;
		do {
			ys = *sx++ + carry;
			carry = ys >> 32;
			y = *bx - (ys & 0xffffffff) - borrow;
			borrow = y >> 32 & 1UL;
			*bx++ = y & 0xffffffff;
		}
		while(sx <= sxe);
		bx = b->x;
		bxe = bx + n;
		if (!*bxe) {
			while(--bxe > bx && !*bxe)
				--n;
			b->wds = n;
		}
	}
	return q;
}
