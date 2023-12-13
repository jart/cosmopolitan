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
#include "libc/fmt/conv.h"
#include "libc/runtime/fenv.h"
#include "third_party/gdtoa/gdtoa.internal.h"

#define Avoid_Underflow
#define dplen 1

/* The factor of 2^106 in tiny__gdtoa_tens[4] helps us avoid setting the underflow */
/* flag unnecessarily.  It leads to a song and dance at the end of strtod. */
static const double tiny__gdtoa_tens[] = {
	1e-16, 1e-32, 1e-64, 1e-128,
	9007199254740992.*9007199254740992.e-256
};

static double
__gdtoa_sulp(U *x, int scale)
{
	U u;
	int i;
	double rv;
	rv = __gdtoa_ulp(x);
	if (!scale || (i = 2*P + 1 - ((word0(x) & Exp_mask) >> Exp_shift)) <= 0)
		return rv; /* Is there an example where i <= 0 ? */
	word0(&u) = Exp_1 + (i << Exp_shift);
	word1(&u) = 0;
	return rv * u.d;
}

double
strtod(const char *s00, char **se)
{
	ThInfo *TI = 0;
	int scale;
	int bb2, bb5, bbe, bd2, bd5, bbbits, bs2, c, decpt, dsign,
		e, e1, esign, i, j, k, nd, nd0, nf, nz, nz0, sign;
	const char *s, *s0, *s1;
	double aadj;
	Long L;
	U adj, aadj1, rv, rv0;
	ULong y, z;
	Bigint *bb, *bb1, *bd, *bd0, *bs, *delta;
	ULong Lsb, Lsb1;
	int Rounding;
	Rounding = FLT_ROUNDS;
	sign = nz0 = nz = decpt = 0;
	dval(&rv) = 0.;
	for(s = s00;;s++) switch(*s) {
		case '-':
			sign = 1;
			/* no break */
		case '+':
			if (*++s)
				goto break2;
			/* no break */
		case 0:
			goto ret0;
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
		{
			static const FPI fpi = { 53, 1-1023-53+1, 2046-1023-53+1, 1, SI, 0 /*unused*/ };
			Long exp;
			ULong bits[2];
			switch(s[1]) {
			case 'x':
			case 'X':
			{
				FPI fpi1 = fpi;
				fpi1.rounding = Rounding;
				switch((i = __gdtoa_gethex(&s, &fpi1, &exp, &bb, sign, &TI)) & STRTOG_Retmask) {
				case STRTOG_NoNumber:
					s = s00;
					sign = 0;
				case STRTOG_Zero:
					break;
				default:
					if (bb) {
						__gdtoa_copybits(bits, fpi.nbits, bb);
						__gdtoa_Bfree(bb, &TI);
					}
					__gdtoa_ULtod(((U*)&rv)->L, bits, exp, i);
				}}
			goto ret;
			}
		}
		nz0 = 1;
		while(*++s == '0') ;
		if (!*s)
			goto ret;
	}
	s0 = s;
	y = z = 0;
	for(nd = nf = 0; (c = *s) >= '0' && c <= '9'; nd++, s++)
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
			goto ret0;
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
			ULong bits[2];
			static FPI fpinan =	/* only 52 explicit bits */
				{ 52, 1-1023-53+1, 2046-1023-53+1, 1, SI, 0 /*unused*/ };
			if (!decpt)
				switch(c) {
				case 'i':
				case 'I':
					if (__gdtoa_match(&s,"nf")) {
						--s;
						if (!__gdtoa_match(&s,"inity"))
							++s;
						word0(&rv) = 0x7ff00000;
						word1(&rv) = 0;
						goto ret;
					}
					break;
				case 'n':
				case 'N':
					if (__gdtoa_match(&s, "an")) {
						if (*s == '(' /*)*/
						    && __gdtoa_hexnan(&s, &fpinan, bits)
						    == STRTOG_NaNbits) {
							word0(&rv) = 0x7ff00000 | bits[1];
							word1(&rv) = bits[0];
						}
						else {
							word0(&rv) = 0x7ff80000;
							word1(&rv) = 0x0;
						}
						goto ret;
					}
				}
		ret0:
			s = s00;
			sign = 0;
		}
		goto ret;
	}
	e1 = e -= nf;
	/* Now we have nd0 digits, starting at s0, followed by a
	 * decimal point, followed by nd-nd0 digits.  The number we're
	 * after is the integer represented by those digits times
	 * 10**e */
	if (!nd0)
		nd0 = nd;
	k = nd < DBL_DIG + 2 ? nd : DBL_DIG + 2;
	dval(&rv) = y;
	if (k > 9) {
		dval(&rv) = __gdtoa_tens[k - 9] * dval(&rv) + z;
	}
	bd0 = 0;
	if (nd <= DBL_DIG) {
		if (!e)
			goto ret;
		if (e > 0) {
			if (e <= Ten_pmax) {
				/* round correctly FLT_ROUNDS = 2 or 3 */
				if (sign) {
					rv.d = -rv.d;
					sign = 0;
				}
				/* rv = */ rounded_product(dval(&rv), __gdtoa_tens[e]);
				goto ret;
			}
			i = DBL_DIG - nd;
			if (e <= Ten_pmax + i) {
				/* A fancier test would sometimes let us do
				 * this for larger i values.
				 */
				/* round correctly FLT_ROUNDS = 2 or 3 */
				if (sign) {
					rv.d = -rv.d;
					sign = 0;
				}
				e -= i;
				dval(&rv) *= __gdtoa_tens[i];
				/* rv = */ rounded_product(dval(&rv), __gdtoa_tens[e]);
				goto ret;
			}
		}
		else if (e >= -Ten_pmax) {
			/* round correctly FLT_ROUNDS = 2 or 3 */
			if (sign) {
				rv.d = -rv.d;
				sign = 0;
			}
			/* rv = */ rounded_quotient(dval(&rv), __gdtoa_tens[-e]);
			goto ret;
		}
	}
	e1 += nd - k;
	scale = 0;
	if (Rounding >= 2) {
		if (sign)
			Rounding = Rounding == 2 ? 0 : 2;
		else
			if (Rounding != 2)
				Rounding = 0;
	}
	/* Get starting approximation = rv * 10**e1 */
	if (e1 > 0) {
		if ( (i = e1 & 15) !=0)
			dval(&rv) *= __gdtoa_tens[i];
		if (e1 &= ~15) {
			if (e1 > DBL_MAX_10_EXP) {
			ovfl:
				/* Can't trust HUGE_VAL */
				switch(Rounding) {
				case 0: /* toward 0 */
				case 3: /* toward -infinity */
					word0(&rv) = Big0;
					word1(&rv) = Big1;
					break;
				default:
					word0(&rv) = Exp_mask;
					word1(&rv) = 0;
				}
			range_err:
				if (bd0) {
					__gdtoa_Bfree(bb, &TI);
					__gdtoa_Bfree(bd, &TI);
					__gdtoa_Bfree(bs, &TI);
					__gdtoa_Bfree(bd0, &TI);
					__gdtoa_Bfree(delta, &TI);
				}
				errno = ERANGE;
				goto ret;
			}
			e1 >>= 4;
			for(j = 0; e1 > 1; j++, e1 >>= 1)
				if (e1 & 1)
					dval(&rv) *= __gdtoa_bigtens[j];
			/* The last __gdtoa_multiplication could overflow. */
			word0(&rv) -= P*Exp_msk1;
			dval(&rv) *= __gdtoa_bigtens[j];
			if ((z = word0(&rv) & Exp_mask)
			    > Exp_msk1*(DBL_MAX_EXP+Bias-P))
				goto ovfl;
			if (z > Exp_msk1*(DBL_MAX_EXP+Bias-1-P)) {
				/* set to largest number */
				/* (Can't trust DBL_MAX) */
				word0(&rv) = Big0;
				word1(&rv) = Big1;
			}
			else
				word0(&rv) += P*Exp_msk1;
		}
	}
	else if (e1 < 0) {
		e1 = -e1;
		if ( (i = e1 & 15) !=0)
			dval(&rv) /= __gdtoa_tens[i];
		if (e1 >>= 4) {
			if (e1 >= 1 << n___gdtoa_bigtens)
				goto undfl;
			if (e1 & Scale_Bit)
				scale = 2*P;
			for(j = 0; e1 > 0; j++, e1 >>= 1)
				if (e1 & 1)
					dval(&rv) *= tiny__gdtoa_tens[j];
			if (scale && (j = 2*P + 1 - ((word0(&rv) & Exp_mask)
						     >> Exp_shift)) > 0) {
				/* scaled rv is denormal; zap j low bits */
				if (j >= 32) {
					word1(&rv) = 0;
					if (j >= 53)
						word0(&rv) = (P+2)*Exp_msk1;
					else
						word0(&rv) &= 0xffffffff << (j-32);
				}
				else
					word1(&rv) &= 0xffffffff << j;
			}
			if (!dval(&rv)) {
			undfl:
				dval(&rv) = 0.;
				if (Rounding == 2)
					word1(&rv) = 1;
				goto range_err;
			}
		}
	}
	/* Now the hard part -- adjusting rv to the correct value.*/
	/* Put digits into bd: true value = bd * 10^e */
	bd0 = __gdtoa_s2b(s0, nd0, nd, y, dplen, &TI);
	for(;;) {
		bd = __gdtoa_Balloc(bd0->k, &TI);
		Bcopy(bd, bd0);
		bb = __gdtoa_d2b(dval(&rv), &bbe, &bbbits, &TI);	/* rv = bb * 2^bbe */
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
		if (Rounding != 1)
			bs2++;
		Lsb = LSB;
		Lsb1 = 0;
		j = bbe - scale;
		i = j + bbbits - 1;	/* logb(rv) */
		j = P + 1 - bbbits;
		if (i < Emin) {	/* denormal */
			i = Emin - i;
			j -= i;
			if (i < 32)
				Lsb <<= i;
			else
				Lsb1 = Lsb << (i-32);
		}
		bb2 += j;
		bd2 += j;
		bd2 += scale;
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
		if (bb2 > 0)
			bb = __gdtoa_lshift(bb, bb2, &TI);
		if (bd5 > 0)
			bd = __gdtoa_pow5mult(bd, bd5, &TI);
		if (bd2 > 0)
			bd = __gdtoa_lshift(bd, bd2, &TI);
		if (bs2 > 0)
			bs = __gdtoa_lshift(bs, bs2, &TI);
		delta = __gdtoa_diff(bb, bd, &TI);
		dsign = delta->sign;
		delta->sign = 0;
		i = __gdtoa_cmp(delta, bs);
		if (Rounding != 1) {
			if (i < 0) {
				/* Error is less than an __gdtoa_ulp */
				if (!delta->x[0] && delta->wds <= 1) {
					/* exact */
					break;
				}
				if (Rounding) {
					if (dsign) {
						dval(&adj) = 1.;
						goto apply_adj;
					}
				}
				else if (!dsign) {
					dval(&adj) = -1.;
					if (!word1(&rv)
					    && !(word0(&rv) & Frac_mask)) {
						y = word0(&rv) & Exp_mask;
						if (!scale || y > 2*P*Exp_msk1)
						{
							delta = __gdtoa_lshift(delta,Log2P,&TI);
							if (__gdtoa_cmp(delta, bs) <= 0)
								dval(&adj) = -0.5;
						}
					}
				apply_adj:
					if (scale && (y = word0(&rv) & Exp_mask)
					    <= 2*P*Exp_msk1)
						word0(&adj) += (2*P+1)*Exp_msk1 - y;
					dval(&rv) += adj.d*__gdtoa_ulp(&rv);
				}
				break;
			}
			dval(&adj) = __gdtoa_ratio(delta, bs);
			if (adj.d < 1.)
				dval(&adj) = 1.;
			if (adj.d <= 0x7ffffffe) {
				/* dval(&adj) = Rounding ? ceil(&adj) : floor(&adj); */
				y = adj.d;
				if (y != adj.d) {
					if (!((Rounding>>1) ^ dsign))
						y++;
					dval(&adj) = y;
				}
			}
			if (scale && (y = word0(&rv) & Exp_mask) <= 2*P*Exp_msk1)
				word0(&adj) += (2*P+1)*Exp_msk1 - y;
			dval(&adj) *= __gdtoa_ulp(&rv); /* XXX */
			if (dsign) {
				if (word0(&rv) == Big0 && word1(&rv) == Big1)
					goto ovfl;
				dval(&rv) += adj.d;
			}
			else
				dval(&rv) -= adj.d;
			goto cont;
		}
		if (i < 0) {
			/* Error is less than half an __gdtoa_ulp -- check for
			 * special case of mantissa a power of two.
			 */
			if (dsign || word1(&rv) || word0(&rv) & Bndry_mask ||
			    (word0(&rv) & Exp_mask) <= (2*P+1)*Exp_msk1) {
				break;
			}
			if (!delta->x[0] && delta->wds <= 1) {
				/* exact result */
				break;
			}
			delta = __gdtoa_lshift(delta,Log2P,&TI);
			if (__gdtoa_cmp(delta, bs) > 0)
				goto drop_down;
			break;
		}
		if (i == 0) {
			/* exactly half-way between */
			if (dsign) {
				if ((word0(&rv) & Bndry_mask1) == Bndry_mask1
				    &&  word1(&rv) == (
					    (scale && (y = word0(&rv) & Exp_mask) <= 2*P*Exp_msk1)
					    ? (0xffffffff & (0xffffffff << (2*P+1-(y>>Exp_shift)))) :
					    0xffffffff)) {
					/*boundary case -- __gdtoa_increment exponent*/
					if (word0(&rv) == Big0 && word1(&rv) == Big1)
						goto ovfl;
					word0(&rv) = (word0(&rv) & Exp_mask)
						+ Exp_msk1
						;
					word1(&rv) = 0;
					dsign = 0;
					break;
				}
			}
			else if (!(word0(&rv) & Bndry_mask) && !word1(&rv)) {
			drop_down:
				/* boundary case -- __gdtoa_decrement exponent */
				if (scale) {
					L = word0(&rv) & Exp_mask;
					if (L <= (2*P+1)*Exp_msk1) {
						if (L > (P+2)*Exp_msk1)
							/* round even ==> */
							/* accept rv */
							break;
						/* rv = smallest denormal */
						goto undfl;
					}
				}
				L = (word0(&rv) & Exp_mask) - Exp_msk1;
				word0(&rv) = L | Bndry_mask1;
				word1(&rv) = 0xffffffff;
				break;
			}
			if (Lsb1) {
				if (!(word0(&rv) & Lsb1))
					break;
			}
			else if (!(word1(&rv) & Lsb))
				break;
			if (dsign)
				dval(&rv) += __gdtoa_sulp(&rv, scale);
			else {
				dval(&rv) -= __gdtoa_sulp(&rv, scale);
				if (!dval(&rv))
					goto undfl;
			}
			dsign = 1 - dsign;
			break;
		}
		if ((aadj = __gdtoa_ratio(delta, bs)) <= 2.) {
			if (dsign)
				aadj = dval(&aadj1) = 1.;
			else if (word1(&rv) || word0(&rv) & Bndry_mask) {
				if (word1(&rv) == Tiny1 && !word0(&rv))
					goto undfl;
				aadj = 1.;
				dval(&aadj1) = -1.;
			}
			else {
				/* special case -- power of FLT_RADIX to be */
				/* rounded down... */
				if (aadj < 2./FLT_RADIX)
					aadj = 1./FLT_RADIX;
				else
					aadj *= 0.5;
				dval(&aadj1) = -aadj;
			}
		}
		else {
			aadj *= 0.5;
			dval(&aadj1) = dsign ? aadj : -aadj;
			switch(Rounding) {
			case 2: /* towards +infinity */
				dval(&aadj1) -= 0.5;
				break;
			case 0: /* towards 0 */
			case 3: /* towards -infinity */
				dval(&aadj1) += 0.5;
			}
		}
		y = word0(&rv) & Exp_mask;
		/* Check for overflow */
		if (y == Exp_msk1*(DBL_MAX_EXP+Bias-1)) {
			dval(&rv0) = dval(&rv);
			word0(&rv) -= P*Exp_msk1;
			dval(&adj) = dval(&aadj1) * __gdtoa_ulp(&rv);
			dval(&rv) += dval(&adj);
			if ((word0(&rv) & Exp_mask) >=
			    Exp_msk1*(DBL_MAX_EXP+Bias-P)) {
				if (word0(&rv0) == Big0 && word1(&rv0) == Big1)
					goto ovfl;
				word0(&rv) = Big0;
				word1(&rv) = Big1;
				goto cont;
			}
			else
				word0(&rv) += P*Exp_msk1;
		}
		else {
			if (scale && y <= 2*P*Exp_msk1) {
				if (aadj <= 0x7fffffff) {
					if ((z = aadj) <= 0)
						z = 1;
					aadj = z;
					dval(&aadj1) = dsign ? aadj : -aadj;
				}
				word0(&aadj1) += (2*P+1)*Exp_msk1 - y;
			}
			dval(&adj) = dval(&aadj1) * __gdtoa_ulp(&rv);
			dval(&rv) += dval(&adj);
		}
		z = word0(&rv) & Exp_mask;
		if (!scale)
			if (y == z) {
				/* Can we stop now? */
				L = (Long)aadj;
				aadj -= L;
				/* The tolerances below are conservative. */
				if (dsign || word1(&rv) || word0(&rv) & Bndry_mask) {
					if (aadj < .4999999 || aadj > .5000001)
						break;
				}
				else if (aadj < .4999999/FLT_RADIX)
					break;
			}
	cont:
		__gdtoa_Bfree(bb, &TI);
		__gdtoa_Bfree(bd, &TI);
		__gdtoa_Bfree(bs, &TI);
		__gdtoa_Bfree(delta, &TI);
	}
	__gdtoa_Bfree(bb, &TI);
	__gdtoa_Bfree(bd, &TI);
	__gdtoa_Bfree(bs, &TI);
	__gdtoa_Bfree(bd0, &TI);
	__gdtoa_Bfree(delta, &TI);
	if (scale) {
		word0(&rv0) = Exp_1 - 2*P*Exp_msk1;
		word1(&rv0) = 0;
		dval(&rv) *= dval(&rv0);
		/* try to avoid the bug of testing an 8087 register value */
		if (!(word0(&rv) & Exp_mask))
			errno = ERANGE;
	}
ret:
	if (se)
		*se = (char *)s;
	return sign ? -dval(&rv) : dval(&rv);
}

__weak_reference(strtod, strtod_l);
