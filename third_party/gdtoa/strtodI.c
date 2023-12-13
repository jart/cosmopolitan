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

static double
__gdtoa_ulpdown(U *d)
{
	double u;
	ULong *L = d->L;
	u = __gdtoa_ulp(d);
	if (!(L[0] | (L[1] & 0xfffff))
	    && (L[1] & 0x7ff00000) > 0x00100000)
		u *= 0.5;
	return u;
}

int
strtodI(const char *s, char **sp, double *dd)
{
	static const FPI fpi = { 53, 1-1023-53+1, 2046-1023-53+1, 1, SI, 0 /*unused*/ };
	ULong bits[2], sign;
	Long exp;
	int j, k;
	U *u;
	k = strtodg(s, sp, &fpi, &exp, bits);
	u = (U*)dd;
	sign = k & STRTOG_Neg ? 0x80000000L : 0;
	switch(k & STRTOG_Retmask) {
	case STRTOG_NoNumber:
		dval(&u[0]) = dval(&u[1]) = 0.;
		break;
	case STRTOG_Zero:
		dval(&u[0]) = dval(&u[1]) = 0.;
		goto contain;
	case STRTOG_Denormal:
		word1(&u[0]) = bits[0];
		word0(&u[0]) = bits[1];
		goto contain;
	case STRTOG_Normal:
		word1(&u[0]) = bits[0];
		word0(&u[0]) = (bits[1] & ~0x100000) | ((exp + 0x3ff + 52) << 20);
	contain:
		j = k & STRTOG_Inexact;
		if (sign) {
			word0(&u[0]) |= sign;
			j = STRTOG_Inexact - j;
		}
		switch(j) {
		case STRTOG_Inexlo:
			dval(&u[1]) = dval(&u[0]) + __gdtoa_ulp(&u[0]);
			break;
		case STRTOG_Inexhi:
			dval(&u[1]) = dval(&u[0]);
			dval(&u[0]) -= __gdtoa_ulpdown(u);
			break;
		default:
			dval(&u[1]) = dval(&u[0]);
		}
		break;
	case STRTOG_Infinite:
		word0(&u[0]) = word0(&u[1]) = sign | 0x7ff00000;
		word1(&u[0]) = word1(&u[1]) = 0;
		if (k & STRTOG_Inexact) {
			if (sign) {
				word0(&u[1]) = 0xffefffffL;
				word1(&u[1]) = 0xffffffffL;
			}
			else {
				word0(&u[0]) = 0x7fefffffL;
				word1(&u[0]) = 0xffffffffL;
			}
		}
		break;
	case STRTOG_NaN:
		u->L[0] = (u+1)->L[0] = d_QNAN0;
		u->L[1] = (u+1)->L[1] = d_QNAN1;
		break;
	case STRTOG_NaNbits:
		word0(&u[0]) = word0(&u[1]) = 0x7ff00000 | sign | bits[1];
		word1(&u[0]) = word1(&u[1]) = bits[0];
	}
	return k;
}
