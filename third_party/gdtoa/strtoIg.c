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

int
__gdtoa_strtoIg(const char *s00, char **se, const FPI *fpi, Long *exp, Bigint **B, int *rvp)
{
	Bigint *b, *b1;
	int i, nb, nw, nw1, rv, rv1, swap;
	unsigned int nb1, nb11;
	Long e1;
	ThInfo *TI = 0;
	b = *B;
	rv = strtodg(s00, se, fpi, exp, b->x);
	if (!(rv & STRTOG_Inexact)) {
		B[1] = 0;
		return *rvp = rv;
	}
	e1 = exp[0];
	rv1 = rv ^ STRTOG_Inexact;
	b1 = __gdtoa_Balloc(b->k, &TI);
	Bcopy(b1, b);
	nb = fpi->nbits;
	nb1 = nb & 31;
	nb11 = (nb1 - 1) & 31;
	nw = b->wds;
	nw1 = nw - 1;
	if (rv & STRTOG_Inexlo) {
		swap = 0;
		b1 = __gdtoa_increment(b1, &TI);
		if ((rv & STRTOG_Retmask) == STRTOG_Zero) {
			if (fpi->sudden_underflow) {
				b1->x[0] = 0;
				b1->x[nw1] = 1L << nb11;
				rv1 += STRTOG_Normal - STRTOG_Zero;
				rv1 &= ~STRTOG_Underflow;
				goto swapcheck;
			}
			rv1 &= STRTOG_Inexlo | STRTOG_Underflow | STRTOG_Zero | STRTOG_Neg;
			rv1 |= STRTOG_Inexhi | STRTOG_Denormal;
			goto swapcheck;
		}
		if (b1->wds > nw
		    || (nb1 && b1->x[nw1] & 1L << nb1)) {
			if (++e1 > fpi->emax)
				rv1 = STRTOG_Infinite | STRTOG_Inexhi;
			__gdtoa_rshift(b1, 1);
		}
		else if ((rv & STRTOG_Retmask) == STRTOG_Denormal) {
			if (b1->x[nw1] & 1L << nb11) {
				rv1 += STRTOG_Normal - STRTOG_Denormal;
				rv1 &= ~STRTOG_Underflow;
			}
		}
	}
	else {
		swap = STRTOG_Neg;
		if ((rv & STRTOG_Retmask) == STRTOG_Infinite) {
			b1 = __gdtoa_set_ones(b1, nb, &TI);
			e1 = fpi->emax;
			rv1 = STRTOG_Normal | STRTOG_Inexlo | (rv & STRTOG_Neg);
			goto swapcheck;
		}
		__gdtoa_decrement(b1);
		if ((rv & STRTOG_Retmask) == STRTOG_Denormal) {
			for(i = nw1; !b1->x[i]; --i)
				if (!i) {
					rv1 = STRTOG_Zero | STRTOG_Inexlo | (rv & STRTOG_Neg);
					break;
				}
			goto swapcheck;
		}
		if (!(b1->x[nw1] & 1L << nb11)) {
			if (e1 == fpi->emin) {
				if (fpi->sudden_underflow)
					rv1 += STRTOG_Zero - STRTOG_Normal;
				else
					rv1 += STRTOG_Denormal - STRTOG_Normal;
				rv1 |= STRTOG_Underflow;
			}
			else {
				b1 = __gdtoa_lshift(b1, 1, &TI);
				b1->x[0] |= 1;
				--e1;
			}
		}
	}
swapcheck:
	if (swap ^ (rv & STRTOG_Neg)) {
		rvp[0] = rv1;
		rvp[1] = rv;
		B[0] = b1;
		B[1] = b;
		exp[1] = exp[0];
		exp[0] = e1;
	}
	else {
		rvp[0] = rv;
		rvp[1] = rv1;
		B[1] = b1;
		exp[1] = e1;
	}
	return rv;
}
