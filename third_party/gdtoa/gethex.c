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
#include "libc/errno.h"
#include "third_party/gdtoa/gdtoa.internal.h"

int
__gdtoa_gethex(const char **sp, const FPI *fpi,
	       Long *exp, Bigint **bp, int sign, ThInfo **PTI)
{
	Bigint *b;
	const unsigned char *decpt, *s0, *s, *s1;
	int big, esign, havedig, irv, j, k, n, n0, nbits, up, zret;
	ULong L, lostbits, *x;
	Long e, e1;
	/**** if (!__gdtoa_hexdig['0']) __gdtoa_hexdig_init(); ****/
	*bp = 0;
	havedig = 0;
	s0 = *(const unsigned char **)sp + 2;
	while(s0[havedig] == '0')
		havedig++;
	s0 += havedig;
	s = s0;
	decpt = 0;
	zret = 0;
	e = 0;
	if (__gdtoa_hexdig[*s])
		havedig++;
	else {
		zret = 1;
		if (*s != '.')
			goto pcheck;
		decpt = ++s;
		if (!__gdtoa_hexdig[*s])
			goto pcheck;
		while(*s == '0')
			s++;
		if (__gdtoa_hexdig[*s])
			zret = 0;
		havedig = 1;
		s0 = s;
	}
	while(__gdtoa_hexdig[*s])
		s++;
	if (*s == '.' && !decpt) {
		decpt = ++s;
		while(__gdtoa_hexdig[*s])
			s++;
	}/*}*/
	if (decpt)
		e = -(((Long)(s-decpt)) << 2);
pcheck:
	s1 = s;
	big = esign = 0;
	switch(*s) {
	case 'p':
	case 'P':
		switch(*++s) {
		case '-':
			esign = 1;
			/* no break */
		case '+':
			s++;
		}
		if ((n = __gdtoa_hexdig[*s]) == 0 || n > 0x19) {
			s = s1;
			break;
		}
		e1 = n - 0x10;
		while((n = __gdtoa_hexdig[*++s]) !=0 && n <= 0x19) {
			if (e1 & 0xf8000000)
				big = 1;
			e1 = 10*e1 + n - 0x10;
		}
		if (esign)
			e1 = -e1;
		e += e1;
	}
	*sp = (char*)s;
	if (!havedig)
		*sp = (char*)s0 - 1;
	if (zret)
		return STRTOG_Zero;
	if (big) {
		if (esign) {
			switch(fpi->rounding) {
			case FPI_Round_up:
				if (sign)
					break;
				goto ret_tiny;
			case FPI_Round_down:
				if (!sign)
					break;
				goto ret_tiny;
			}
			goto retz;
		ret_tiny:
			b = __gdtoa_Balloc(0, PTI);
			b->wds = 1;
			b->x[0] = 1;
			goto dret;
		}
		switch(fpi->rounding) {
		case FPI_Round_near:
			goto ovfl1;
		case FPI_Round_up:
			if (!sign)
				goto ovfl1;
			goto ret_big;
		case FPI_Round_down:
			if (sign)
				goto ovfl1;
		}
	ret_big:
		nbits = fpi->nbits;
		n0 = n = nbits >> kshift;
		if (nbits & kmask)
			++n;
		for(j = n, k = 0; j >>= 1; ++k);
		*bp = b = __gdtoa_Balloc(k, PTI);
		b->wds = n;
		for(j = 0; j < n0; ++j)
			b->x[j] = ALL_ON;
		if (n > n0)
			b->x[j] = ALL_ON >> (ULbits - (nbits & kmask));
		*exp = fpi->emax;
		return STRTOG_Normal | STRTOG_Inexlo;
	}
	n = s1 - s0 - 1;
	for(k = 0; n > (1 << (kshift-2)) - 1; n >>= 1)
		k++;
	b = __gdtoa_Balloc(k, PTI);
	x = b->x;
	n = 0;
	L = 0;
	while(s1 > s0) {
		if (*--s1 == '.')
			continue;
		if (n == ULbits) {
			*x++ = L;
			L = 0;
			n = 0;
		}
		L |= (__gdtoa_hexdig[*s1] & 0x0f) << n;
		n += 4;
	}
	*x++ = L;
	b->wds = n = x - b->x;
	n = ULbits*n - hi0bits(L);
	nbits = fpi->nbits;
	lostbits = 0;
	x = b->x;
	if (n > nbits) {
		n -= nbits;
		if (__gdtoa_any_on(b,n)) {
			lostbits = 1;
			k = n - 1;
			if (x[k>>kshift] & 1 << (k & kmask)) {
				lostbits = 2;
				if (k > 0 && __gdtoa_any_on(b,k))
					lostbits = 3;
			}
		}
		__gdtoa_rshift(b, n);
		e += n;
	}
	else if (n < nbits) {
		n = nbits - n;
		b = __gdtoa_lshift(b, n, PTI);
		e -= n;
		x = b->x;
	}
	if (e > fpi->emax) {
	ovfl:
		__gdtoa_Bfree(b, PTI);
	ovfl1:
		errno = ERANGE;
		switch (fpi->rounding) {
		case FPI_Round_zero:
			goto ret_big;
		case FPI_Round_down:
			if (!sign)
				goto ret_big;
			break;
		case FPI_Round_up:
			if (sign)
				goto ret_big;
		}
		return STRTOG_Infinite | STRTOG_Overflow | STRTOG_Inexhi;
	}
	irv = STRTOG_Normal;
	if (e < fpi->emin) {
		irv = STRTOG_Denormal;
		n = fpi->emin - e;
		if (n >= nbits) {
			switch (fpi->rounding) {
			case FPI_Round_near:
				if (n == nbits && (n < 2 || lostbits || __gdtoa_any_on(b,n-1)))
					goto one_bit;
				break;
			case FPI_Round_up:
				if (!sign)
					goto one_bit;
				break;
			case FPI_Round_down:
				if (sign) {
				one_bit:
					x[0] = b->wds = 1;
				dret:
					*bp = b;
					*exp = fpi->emin;
					errno = ERANGE;
					return STRTOG_Denormal | STRTOG_Inexhi
						| STRTOG_Underflow;
				}
			}
			__gdtoa_Bfree(b, PTI);
		retz:
			errno = ERANGE;
			return STRTOG_Zero | STRTOG_Inexlo | STRTOG_Underflow;
		}
		k = n - 1;
		if (lostbits)
			lostbits = 1;
		else if (k > 0)
			lostbits = __gdtoa_any_on(b,k);
		if (x[k>>kshift] & 1 << (k & kmask))
			lostbits |= 2;
		nbits -= n;
		__gdtoa_rshift(b,n);
		e = fpi->emin;
	}
	if (lostbits) {
		up = 0;
		switch(fpi->rounding) {
		case FPI_Round_zero:
			break;
		case FPI_Round_near:
			if (lostbits & 2
			    && (lostbits | x[0]) & 1)
				up = 1;
			break;
		case FPI_Round_up:
			up = 1 - sign;
			break;
		case FPI_Round_down:
			up = sign;
		}
		if (up) {
			k = b->wds;
			b = __gdtoa_increment(b, PTI);
			x = b->x;
			if (irv == STRTOG_Denormal) {
				if (nbits == fpi->nbits - 1
				    && x[nbits >> kshift] & 1 << (nbits & kmask))
					irv =  STRTOG_Normal;
			}
			else if (b->wds > k
				 || ((n = nbits & kmask) !=0
				     && hi0bits(x[k-1]) < 32-n)) {
				__gdtoa_rshift(b,1);
				if (++e > fpi->emax)
					goto ovfl;
			}
			irv |= STRTOG_Inexhi;
		}
		else
			irv |= STRTOG_Inexlo;
	}
	*bp = b;
	*exp = e;
	return irv;
}
