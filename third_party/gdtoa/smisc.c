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

Bigint *
__gdtoa_s2b(const char *s, int nd0, int nd, ULong y9, int dplen, ThInfo **PTI)
{
	Bigint *b;
	int i, k;
	Long x, y;
	x = (nd + 8) / 9;
	for(k = 0, y = 1; x > y; y <<= 1, k++) ;
	b = __gdtoa_Balloc(k, PTI);
	b->x[0] = y9;
	b->wds = 1;
	i = 9;
	if (9 < nd0) {
		s += 9;
		do b = __gdtoa_multadd(b, 10, *s++ - '0', PTI);
		while(++i < nd0);
		s += dplen;
	}
	else
		s += dplen + 9;
	for(; i < nd; i++)
		b = __gdtoa_multadd(b, 10, *s++ - '0', PTI);
	return b;
}

double
__gdtoa_ratio(Bigint *a, Bigint *b)
{
	U da, db;
	int k, ka, kb;
	dval(&da) = __gdtoa_b2d(a, &ka);
	dval(&db) = __gdtoa_b2d(b, &kb);
	k = ka - kb + ULbits*(a->wds - b->wds);
	if (k > 0)
		word0(&da) += k*Exp_msk1;
	else {
		k = -k;
		word0(&db) += k*Exp_msk1;
	}
	return dval(&da) / dval(&db);
}

int
__gdtoa_match(const char **sp, char *t)
{
	int c, d;
	const char *s = *sp;
	while( (d = *t++) !=0) {
		if ((c = *++s) >= 'A' && c <= 'Z')
			c += 'a' - 'A';
		if (c != d)
			return 0;
	}
	*sp = s + 1;
	return 1;
}

void
__gdtoa_copybits(ULong *c, int n, Bigint *b)
{
	ULong *ce, *x, *xe;
	ce = c + ((n-1) >> kshift) + 1;
	x = b->x;
	xe = x + b->wds;
	while(x < xe)
		*c++ = *x++;
	while(c < ce)
		*c++ = 0;
}

ULong
__gdtoa_any_on(Bigint *b, int k)
{
	int n, nwds;
	ULong *x, *x0, x1, x2;
	x = b->x;
	nwds = b->wds;
	n = k >> kshift;
	if (n > nwds)
		n = nwds;
	else if (n < nwds && (k &= kmask)) {
		x1 = x2 = x[n];
		x1 >>= k;
		x1 <<= k;
		if (x1 != x2)
			return 1;
	}
	x0 = x;
	x += n;
	while(x > x0)
		if (*--x)
			return 1;
	return 0;
}
