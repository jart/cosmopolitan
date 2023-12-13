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

static const int
fivesbits[] = {	  0,  3,  5,  7, 10, 12, 14, 17, 19, 21,
	         24, 26, 28, 31, 33, 35, 38, 40, 42, 45,
	         47, 49, 52 };

Bigint *
__gdtoa_increment(Bigint *b, ThInfo **PTI)
{
	ULong *x, *xe;
	Bigint *b1;
	x = b->x;
	xe = x + b->wds;
	do {
		if (*x < (ULong)0xffffffffL) {
			++*x;
			return b;
		}
		*x++ = 0;
	} while(x < xe);
	{
		if (b->wds >= b->maxwds) {
			b1 = __gdtoa_Balloc(b->k+1, PTI);
			Bcopy(b1,b);
			__gdtoa_Bfree(b, PTI);
			b = b1;
		}
		b->x[b->wds++] = 1;
	}
	return b;
}

void
__gdtoa_decrement(Bigint *b)
{
	ULong *x, *xe;
	x = b->x;
	xe = x + b->wds;
	do {
		if (*x) {
			--*x;
			break;
		}
		*x++ = 0xffffffffL;
	}
	while(x < xe);
}

static int
all_on(Bigint *b, int n)
{
	ULong *x, *xe;
	x = b->x;
	xe = x + (n >> kshift);
	while(x < xe)
		if ((*x++ & ALL_ON) != ALL_ON)
			return 0;
	if (n &= kmask)
		return ((*x | (ALL_ON << n)) & ALL_ON) == ALL_ON;
	return 1;
}

Bigint *
__gdtoa_set_ones(Bigint *b, int n, ThInfo **PTI)
{
	int k;
	ULong *x, *xe;
	k = (n + ((1 << kshift) - 1)) >> kshift;
	if (b->k < k) {
		__gdtoa_Bfree(b, PTI);
		b = __gdtoa_Balloc(k, PTI);
	}
	k = n >> kshift;
	if (n &= kmask)
		k++;
	b->wds = k;
	x = b->x;
	xe = x + k;
	while(x < xe)
		*x++ = ALL_ON;
	if (n)
		x[-1] >>= ULbits - n;
	return b;
}

static int
rvOK(U *d, const FPI *fpi, Long *exp, ULong *bits, int exact, int rd, int *irv, ThInfo **PTI)
{
	Bigint *b;
	ULong carry, inex, lostbits;
	int bdif, e, j, k, k1, nb, rv;
	carry = rv = 0;
	b = __gdtoa_d2b(dval(d), &e, &bdif, PTI);
	bdif -= nb = fpi->nbits;
	e += bdif;
	if (bdif <= 0) {
		if (exact)
			goto trunc;
		goto ret;
	}
	if (P == nb) {
		if (exact && fpi->rounding == FLT_ROUNDS) goto trunc;
		goto ret;
	}
	switch(rd) {
	case 1: /* round down (toward -Infinity) */
		goto trunc;
	case 2: /* round up (toward +Infinity) */
		break;
	default: /* round near */
		k = bdif - 1;
		if (k < 0)
			goto trunc;
		if (!k) {
			if (!exact)
				goto ret;
			if (b->x[0] & 2)
				break;
			goto trunc;
		}
		if (b->x[k>>kshift] & ((ULong)1 << (k & kmask)))
			break;
		goto trunc;
	}
	/* "break" cases: round up 1 bit, then truncate; bdif > 0 */
	carry = 1;
trunc:
	inex = lostbits = 0;
	if (bdif > 0) {
		if ( (lostbits = __gdtoa_any_on(b, bdif)) !=0)
			inex = STRTOG_Inexlo;
		__gdtoa_rshift(b, bdif);
		if (carry) {
			inex = STRTOG_Inexhi;
			b = __gdtoa_increment(b, PTI);
			if ( (j = nb & kmask) !=0)
				j = ULbits - j;
			if (hi0bits(b->x[b->wds - 1]) != j) {
				if (!lostbits)
					lostbits = b->x[0] & 1;
				__gdtoa_rshift(b, 1);
				e++;
			}
		}
	}
	else if (bdif < 0)
		b = __gdtoa_lshift(b, -bdif, PTI);
	if (e < fpi->emin) {
		k = fpi->emin - e;
		e = fpi->emin;
		if (k > nb || fpi->sudden_underflow) {
			b->wds = inex = 0;
			*irv = STRTOG_Underflow | STRTOG_Inexlo;
		}
		else {
			k1 = k - 1;
			if (k1 > 0 && !lostbits)
				lostbits = __gdtoa_any_on(b, k1);
			if (!lostbits && !exact)
				goto ret;
			lostbits |=
				carry = b->x[k1>>kshift] & (1 << (k1 & kmask));
			__gdtoa_rshift(b, k);
			*irv = STRTOG_Denormal;
			if (carry) {
				b = __gdtoa_increment(b, PTI);
				inex = STRTOG_Inexhi | STRTOG_Underflow;
			}
			else if (lostbits)
				inex = STRTOG_Inexlo | STRTOG_Underflow;
		}
	}
	else if (e > fpi->emax) {
		e = fpi->emax + 1;
		*irv = STRTOG_Infinite | STRTOG_Overflow | STRTOG_Inexhi;
		errno = ERANGE;
		b->wds = inex = 0;
	}
	*exp = e;
	__gdtoa_copybits(bits, nb, b);
	*irv |= inex;
	rv = 1;
ret:
	__gdtoa_Bfree(b, PTI);
	return rv;
}

static int
mantbits(U *d)
{
	ULong L;
	if ( (L = word1(d)) !=0)
		return P - lo0bits(&L);
	L = word0(d) | Exp_msk1;
	return P - 32 - lo0bits(&L);
}

int
strtodg(const char *s00, char **se, const FPI *fpi, Long *exp, ULong *bits)
{
	int abe, abits, asub;
	int bb0, bb2, bb5, bbe, bd2, bd5, bbbits, bs2, c, decpt, denorm;
	int dsign, e, e1, e2, emin, esign, finished, i, inex, irv, j, k;
	int nbits, nd, nd0, nf, nz, nz0, rd, rvbits, rve, rve1, sign;
	int sudden_underflow;
	const char *s, *s0, *s1;
	double adj0, tol;
	Long L;
	U adj, rv;
	ULong *b, *be, y, z;
	Bigint *ab, *bb, *bb1, *bd, *bd0, *bs, *delta, *rvb, *rvb0;
	ThInfo *TI = 0;
	irv = STRTOG_Zero;
	denorm = sign = nz0 = nz = 0;
	dval(&rv) = 0.;
	rvb = 0;
	nbits = fpi->nbits;
	for(s = s00;;s++) switch(*s) {
		case '-':
			sign = 1;
			/* no break */
		case '+':
			if (*++s)
				goto break2;
			/* no break */
		case 0:
			sign = 0;
			irv = STRTOG_NoNumber;
			s = s00;
			goto ret;
		case '\t':
		case '\n':
		case '\v':
		case '\f':
		case '\r':
		case ' ':
			continue;
		default:
			goto break2;
		}
break2:
	if (*s == '0') {
		switch(s[1]) {
		case 'x':
		case 'X':
			irv = __gdtoa_gethex(&s, fpi, exp, &rvb, sign, &TI);
			if (irv == STRTOG_NoNumber) {
				s = s00;
				sign = 0;
			}
			goto ret;
		}
		nz0 = 1;
		while(*++s == '0') ;
		if (!*s)
			goto ret;
	}
	sudden_underflow = fpi->sudden_underflow;
	s0 = s;
	y = z = 0;
	for(decpt = nd = nf = 0; (c = *s) >= '0' && c <= '9'; nd++, s++)
		if (nd < 9)
			y = 10*y + c - '0';
		else if (nd < DBL_DIG + 2)
			z = 10*z + c - '0';
	nd0 = nd;
	if (c == '.') {
		c = *++s;
		decpt = 1;
		if (!nd) {
			for(; c == '0'; c = *++s)
				nz++;
			if (c > '0' && c <= '9') {
				s0 = s;
				nf += nz;
				nz = 0;
				goto have_dig;
			}
			goto dig_done;
		}
		for(; c >= '0' && c <= '9'; c = *++s) {
		have_dig:
			nz++;
			if (c -= '0') {
				nf += nz;
				for(i = 1; i < nz; i++)
					if (nd++ < 9)
						y *= 10;
					else if (nd <= DBL_DIG + 2)
						z *= 10;
				if (nd++ < 9)
					y = 10*y + c;
				else if (nd <= DBL_DIG + 2)
					z = 10*z + c;
				nz = 0;
			}
		}
	}/*}*/
dig_done:
	e = 0;
	if (c == 'e' || c == 'E') {
		if (!nd && !nz && !nz0) {
			irv = STRTOG_NoNumber;
			s = s00;
			goto ret;
		}
		s00 = s;
		esign = 0;
		switch(c = *++s) {
		case '-':
			esign = 1;
		case '+':
			c = *++s;
		}
		if (c >= '0' && c <= '9') {
			while(c == '0')
				c = *++s;
			if (c > '0' && c <= '9') {
				L = c - '0';
				s1 = s;
				while((c = *++s) >= '0' && c <= '9')
					L = 10*L + c - '0';
				if (s - s1 > 8 || L > 19999)
					/* Avoid confusion from exponents
					 * so large that e might overflow.
					 */
					e = 19999; /* safe for 16 bit ints */
				else
					e = (int)L;
				if (esign)
					e = -e;
			}
			else
				e = 0;
		}
		else
			s = s00;
	}
	if (!nd) {
		if (!nz && !nz0) {
			/* Check for Nan and Infinity */
			if (!decpt)
				switch(c) {
				case 'i':
				case 'I':
					if (__gdtoa_match(&s,"nf")) {
						--s;
						if (!__gdtoa_match(&s,"inity"))
							++s;
						irv = STRTOG_Infinite;
						goto infnanexp;
					}
					break;
				case 'n':
				case 'N':
					if (__gdtoa_match(&s, "an")) {
						irv = STRTOG_NaN;
						*exp = fpi->emax + 1;
						if (*s == '(')
							irv = __gdtoa_hexnan(&s, fpi, bits);
						goto infnanexp;
					}
				}
			irv = STRTOG_NoNumber;
			s = s00;
		}
		goto ret;
	}
	irv = STRTOG_Normal;
	e1 = e -= nf;
	rd = 0;
	switch(fpi->rounding & 3) {
	case FPI_Round_up:
		rd = 2 - sign;
		break;
	case FPI_Round_zero:
		rd = 1;
		break;
	case FPI_Round_down:
		rd = 1 + sign;
	}
	/* Now we have nd0 digits, starting at s0, followed by a
	 * decimal point, followed by nd-nd0 digits.  The number we're
	 * after is the integer represented by those digits times
	 * 10**e */
	if (!nd0)
		nd0 = nd;
	k = nd < DBL_DIG + 2 ? nd : DBL_DIG + 2;
	dval(&rv) = y;
	if (k > 9)
		dval(&rv) = __gdtoa_tens[k - 9] * dval(&rv) + z;
	bd0 = 0;
	if (nbits <= P && nd <= DBL_DIG) {
		if (!e) {
			if (rvOK(&rv, fpi, exp, bits, 1, rd, &irv, &TI))
				goto ret;
		}
		else if (e > 0) {
			if (e <= Ten_pmax) {
				i = fivesbits[e] + mantbits(&rv) <= P;
				/* rv = */ rounded_product(dval(&rv), __gdtoa_tens[e]);
				if (rvOK(&rv, fpi, exp, bits, i, rd, &irv, &TI))
					goto ret;
				e1 -= e;
				goto rv_notOK;
			}
			i = DBL_DIG - nd;
			if (e <= Ten_pmax + i) {
				/* A fancier test would sometimes let us do
				 * this for larger i values.
				 */
				e2 = e - i;
				e1 -= i;
				dval(&rv) *= __gdtoa_tens[i];
				/* rv = */ rounded_product(dval(&rv), __gdtoa_tens[e2]);
				if (rvOK(&rv, fpi, exp, bits, 0, rd, &irv, &TI))
					goto ret;
				e1 -= e2;
			}
		}
		else if (e >= -Ten_pmax) {
			/* rv = */ rounded_quotient(dval(&rv), __gdtoa_tens[-e]);
			if (rvOK(&rv, fpi, exp, bits, 0, rd, &irv, &TI))
				goto ret;
			e1 -= e;
		}
	}
rv_notOK:
	e1 += nd - k;
	/* Get starting approximation = rv * 10**e1 */
	e2 = 0;
	if (e1 > 0) {
		if ( (i = e1 & 15) !=0)
			dval(&rv) *= __gdtoa_tens[i];
		if (e1 &= ~15) {
			e1 >>= 4;
			while(e1 >= (1 << (n___gdtoa_bigtens-1))) {
				e2 += ((word0(&rv) & Exp_mask)
				       >> Exp_shift1) - Bias;
				word0(&rv) &= ~Exp_mask;
				word0(&rv) |= Bias << Exp_shift1;
				dval(&rv) *= __gdtoa_bigtens[n___gdtoa_bigtens-1];
				e1 -= 1 << (n___gdtoa_bigtens-1);
			}
			e2 += ((word0(&rv) & Exp_mask) >> Exp_shift1) - Bias;
			word0(&rv) &= ~Exp_mask;
			word0(&rv) |= Bias << Exp_shift1;
			for(j = 0; e1 > 0; j++, e1 >>= 1)
				if (e1 & 1)
					dval(&rv) *= __gdtoa_bigtens[j];
		}
	}
	else if (e1 < 0) {
		e1 = -e1;
		if ( (i = e1 & 15) !=0)
			dval(&rv) /= __gdtoa_tens[i];
		if (e1 &= ~15) {
			e1 >>= 4;
			while(e1 >= (1 << (n___gdtoa_bigtens-1))) {
				e2 += ((word0(&rv) & Exp_mask)
				       >> Exp_shift1) - Bias;
				word0(&rv) &= ~Exp_mask;
				word0(&rv) |= Bias << Exp_shift1;
				dval(&rv) *= __gdtoa_tinytens[n___gdtoa_bigtens-1];
				e1 -= 1 << (n___gdtoa_bigtens-1);
			}
			e2 += ((word0(&rv) & Exp_mask) >> Exp_shift1) - Bias;
			word0(&rv) &= ~Exp_mask;
			word0(&rv) |= Bias << Exp_shift1;
			for(j = 0; e1 > 0; j++, e1 >>= 1)
				if (e1 & 1)
					dval(&rv) *= __gdtoa_tinytens[j];
		}
	}
	rvb = __gdtoa_d2b(dval(&rv), &rve, &rvbits, &TI);	/* rv = rvb * 2^rve */
	rve += e2;
	if ((j = rvbits - nbits) > 0) {
		__gdtoa_rshift(rvb, j);
		rvbits = nbits;
		rve += j;
	}
	bb0 = 0;	/* trailing zero bits in rvb */
	e2 = rve + rvbits - nbits;
	if (e2 > fpi->emax + 1)
		goto huge;
	rve1 = rve + rvbits - nbits;
	if (e2 < (emin = fpi->emin)) {
		denorm = 1;
		j = rve - emin;
		if (j > 0) {
			rvb = __gdtoa_lshift(rvb, j, &TI);
			rvbits += j;
		}
		else if (j < 0) {
			rvbits += j;
			if (rvbits <= 0) {
				if (rvbits < -1) {
				ufl:
					rvb->wds = 0;
					rvb->x[0] = 0;
					switch(fpi->rounding) {
					case FPI_Round_up:
						if (!sign)
							goto ret_tiny;
						break;
					case FPI_Round_down:
						if (sign) {
						ret_tiny:
							rvb->wds = rvb->x[0] = 1;
						}
					}
					*exp = emin;
					irv = STRTOG_Underflow | STRTOG_Inexlo;
					goto ret;
				}
				rvb->x[0] = rvb->wds = rvbits = 1;
			}
			else
				__gdtoa_rshift(rvb, -j);
		}
		rve = rve1 = emin;
		if (sudden_underflow && e2 + 1 < emin)
			goto ufl;
	}
	/* Now the hard part -- adjusting rv to the correct value.*/
	/* Put digits into bd: true value = bd * 10^e */
	bd0 = __gdtoa_s2b(s0, nd0, nd, y, 1, &TI);
	for(;;) {
		bd = __gdtoa_Balloc(bd0->k, &TI);
		Bcopy(bd, bd0);
		bb = __gdtoa_Balloc(rvb->k, &TI);
		Bcopy(bb, rvb);
		bbbits = rvbits - bb0;
		bbe = rve + bb0;
		bs = __gdtoa_i2b(1, &TI);
		if (e >= 0) {
			bb2 = bb5 = 0;
			bd2 = bd5 = e;
		}
		else {
			bb2 = bb5 = -e;
			bd2 = bd5 = 0;
		}
		if (bbe >= 0)
			bb2 += bbe;
		else
			bd2 -= bbe;
		bs2 = bb2;
		j = nbits + 1 - bbbits;
		i = bbe + bbbits - nbits;
		if (i < emin)	/* denormal */
			j += i - emin;
		bb2 += j;
		bd2 += j;
		i = bb2 < bd2 ? bb2 : bd2;
		if (i > bs2)
			i = bs2;
		if (i > 0) {
			bb2 -= i;
			bd2 -= i;
			bs2 -= i;
		}
		if (bb5 > 0) {
			bs = __gdtoa_pow5mult(bs, bb5, &TI);
			bb1 = __gdtoa_mult(bs, bb, &TI);
			__gdtoa_Bfree(bb, &TI);
			bb = bb1;
		}
		bb2 -= bb0;
		if (bb2 > 0)
			bb = __gdtoa_lshift(bb, bb2, &TI);
		else if (bb2 < 0)
			__gdtoa_rshift(bb, -bb2);
		if (bd5 > 0)
			bd = __gdtoa_pow5mult(bd, bd5, &TI);
		if (bd2 > 0)
			bd = __gdtoa_lshift(bd, bd2, &TI);
		if (bs2 > 0)
			bs = __gdtoa_lshift(bs, bs2, &TI);
		asub = 1;
		inex = STRTOG_Inexhi;
		delta = __gdtoa_diff(bb, bd, &TI);
		if (delta->wds <= 1 && !delta->x[0])
			break;
		dsign = delta->sign;
		delta->sign = finished = 0;
		L = 0;
		i = __gdtoa_cmp(delta, bs);
		if (rd && i <= 0) {
			irv = STRTOG_Normal;
			if ( (finished = dsign ^ (rd&1)) !=0) {
				if (dsign != 0) {
					irv |= STRTOG_Inexhi;
					goto adj1;
				}
				irv |= STRTOG_Inexlo;
				if (rve1 == emin)
					goto adj1;
				for(i = 0, j = nbits; j >= ULbits;
				    i++, j -= ULbits) {
					if (rvb->x[i] & ALL_ON)
						goto adj1;
				}
				if (j > 1 && lo0bits(rvb->x + i) < j - 1)
					goto adj1;
				rve = rve1 - 1;
				rvb = __gdtoa_set_ones(rvb, rvbits = nbits, &TI);
				break;
			}
			irv |= dsign ? STRTOG_Inexlo : STRTOG_Inexhi;
			break;
		}
		if (i < 0) {
			/* Error is less than half an __gdtoa_ulp -- check for
			 * special case of mantissa a power of two.
			 */
			irv = dsign
				? STRTOG_Normal | STRTOG_Inexlo
				: STRTOG_Normal | STRTOG_Inexhi;
			if (dsign || bbbits > 1 || denorm || rve1 == emin)
				break;
			delta = __gdtoa_lshift(delta,1,&TI);
			if (__gdtoa_cmp(delta, bs) > 0) {
				irv = STRTOG_Normal | STRTOG_Inexlo;
				goto drop_down;
			}
			break;
		}
		if (i == 0) {
			/* exactly half-way between */
			if (dsign) {
				if (denorm && all_on(rvb, rvbits)) {
					/*boundary case -- __gdtoa_increment exponent*/
					rvb->wds = 1;
					rvb->x[0] = 1;
					rve = emin + nbits - (rvbits = 1);
					irv = STRTOG_Normal | STRTOG_Inexhi;
					denorm = 0;
					break;
				}
				irv = STRTOG_Normal | STRTOG_Inexlo;
			}
			else if (bbbits == 1) {
				irv = STRTOG_Normal;
			drop_down:
				/* boundary case -- __gdtoa_decrement exponent */
				if (rve1 == emin) {
					irv = STRTOG_Normal | STRTOG_Inexhi;
					if (rvb->wds == 1 && rvb->x[0] == 1)
						sudden_underflow = 1;
					break;
				}
				rve -= nbits;
				rvb = __gdtoa_set_ones(rvb, rvbits = nbits, &TI);
				break;
			}
			else
				irv = STRTOG_Normal | STRTOG_Inexhi;
			if ((bbbits < nbits && !denorm) || !(rvb->x[0] & 1))
				break;
			if (dsign) {
				rvb = __gdtoa_increment(rvb, &TI);
				j = kmask & (ULbits - (rvbits & kmask));
				if (hi0bits(rvb->x[rvb->wds - 1]) != j)
					rvbits++;
				irv = STRTOG_Normal | STRTOG_Inexhi;
			}
			else {
				if (bbbits == 1)
					goto undfl;
				__gdtoa_decrement(rvb);
				irv = STRTOG_Normal | STRTOG_Inexlo;
			}
			break;
		}
		if ((dval(&adj) = __gdtoa_ratio(delta, bs)) <= 2.) {
		adj1:
			inex = STRTOG_Inexlo;
			if (dsign) {
				asub = 0;
				inex = STRTOG_Inexhi;
			}
			else if (denorm && bbbits <= 1) {
			undfl:
				rvb->wds = 0;
				rve = emin;
				irv = STRTOG_Underflow | STRTOG_Inexlo;
				if (fpi->rounding == 2) {
					rvb->wds = 1;
					rvb->x[0] = 1;
					irv = STRTOG_Underflow | STRTOG_Inexhi;
				}
				break;
			}
			adj0 = dval(&adj) = 1.;
		}
		else {
			adj0 = dval(&adj) *= 0.5;
			if (dsign) {
				asub = 0;
				inex = STRTOG_Inexlo;
			}
			if (dval(&adj) < 2147483647.) {
				L = adj0;
				adj0 -= L;
				switch(rd) {
				case 0:
					if (adj0 >= .5)
						goto inc_L;
					break;
				case 1:
					if (asub && adj0 > 0.)
						goto inc_L;
					break;
				case 2:
					if (!asub && adj0 > 0.) {
					inc_L:
						L++;
						inex = STRTOG_Inexact - inex;
					}
				}
				dval(&adj) = L;
			}
		}
		y = rve + rvbits;
		/* adj *= __gdtoa_ulp(dval(&rv)); */
		/* if (asub) rv -= adj; else rv += adj; */
		if (!denorm && rvbits < nbits) {
			rvb = __gdtoa_lshift(rvb, j = nbits - rvbits, &TI);
			rve -= j;
			rvbits = nbits;
		}
		ab = __gdtoa_d2b(dval(&adj), &abe, &abits, &TI);
		if (abe < 0)
			__gdtoa_rshift(ab, -abe);
		else if (abe > 0)
			ab = __gdtoa_lshift(ab, abe, &TI);
		rvb0 = rvb;
		if (asub) {
			/* rv -= adj; */
			j = hi0bits(rvb->x[rvb->wds-1]);
			rvb = __gdtoa_diff(rvb, ab, &TI);
			k = rvb0->wds - 1;
			if (denorm)
				/* do nothing */;
			else if (rvb->wds <= k
				 || hi0bits( rvb->x[k]) >
				 hi0bits(rvb0->x[k])) {
				/* unlikely; can only have lost 1 high bit */
				if (rve1 == emin) {
					--rvbits;
					denorm = 1;
				}
				else {
					rvb = __gdtoa_lshift(rvb, 1, &TI);
					--rve;
					--rve1;
					L = finished = 0;
				}
			}
		}
		else {
			rvb = __gdtoa_sum(rvb, ab, &TI);
			k = rvb->wds - 1;
			if (k >= rvb0->wds
			    || hi0bits(rvb->x[k]) < hi0bits(rvb0->x[k])) {
				if (denorm) {
					if (++rvbits == nbits)
						denorm = 0;
				}
				else {
					__gdtoa_rshift(rvb, 1);
					rve++;
					rve1++;
					L = 0;
				}
			}
		}
		__gdtoa_Bfree(ab, &TI);
		__gdtoa_Bfree(rvb0, &TI);
		if (finished)
			break;
		z = rve + rvbits;
		if (y == z && L) {
			/* Can we stop now? */
			tol = dval(&adj) * 5e-16; /* > max rel error */
			dval(&adj) = adj0 - .5;
			if (dval(&adj) < -tol) {
				if (adj0 > tol) {
					irv |= inex;
					break;
				}
			}
			else if (dval(&adj) > tol && adj0 < 1. - tol) {
				irv |= inex;
				break;
			}
		}
		bb0 = denorm ? 0 : __gdtoa_trailz(rvb);
		__gdtoa_Bfree(bb, &TI);
		__gdtoa_Bfree(bd, &TI);
		__gdtoa_Bfree(bs, &TI);
		__gdtoa_Bfree(delta, &TI);
	}
	if (!denorm && (j = nbits - rvbits)) {
		if (j > 0)
			rvb = __gdtoa_lshift(rvb, j, &TI);
		else
			__gdtoa_rshift(rvb, -j);
		rve -= j;
	}
	*exp = rve;
	__gdtoa_Bfree(bb, &TI);
	__gdtoa_Bfree(bd, &TI);
	__gdtoa_Bfree(bs, &TI);
	__gdtoa_Bfree(bd0, &TI);
	__gdtoa_Bfree(delta, &TI);
	if (rve > fpi->emax) {
	huge:
		__gdtoa_Bfree(rvb, &TI);
		rvb = 0;
		errno = ERANGE;
		switch(fpi->rounding & 3) {
		case FPI_Round_up:
			if (!sign)
				goto ret_inf;
			break;
		case FPI_Round_down:
			if (!sign)
				break;
		case FPI_Round_near:
		ret_inf:
			irv = STRTOG_Infinite | STRTOG_Overflow | STRTOG_Inexhi;
			k = nbits >> kshift;
			if (nbits & kmask)
				++k;
			bzero(bits, k*sizeof(ULong));
		infnanexp:
			*exp = fpi->emax + 1;
			goto ret;
		}
		/* Round to largest representable magnitude */
		irv = STRTOG_Normal | STRTOG_Inexlo;
		*exp = fpi->emax;
		b = bits;
		be = b + ((fpi->nbits + 31) >> 5);
		while(b < be)
			*b++ = -1;
		if ((j = fpi->nbits & 0x1f))
			*--be >>= (32 - j);
	}
ret:
	if (denorm) {
		if (sudden_underflow) {
			rvb->wds = 0;
			irv = STRTOG_Underflow | STRTOG_Inexlo;
			errno = ERANGE;
		}
		else  {
			irv = (irv & ~STRTOG_Retmask) |
				(rvb->wds > 0 ? STRTOG_Denormal : STRTOG_Zero);
			if (irv & STRTOG_Inexact) {
				irv |= STRTOG_Underflow;
				errno = ERANGE;
			}
		}
	}
	if (se)
		*se = (char *)s;
	if (sign)
		irv |= STRTOG_Neg;
	if (rvb) {
		__gdtoa_copybits(bits, nbits, rvb);
		__gdtoa_Bfree(rvb, &TI);
	}
	return irv;
}
