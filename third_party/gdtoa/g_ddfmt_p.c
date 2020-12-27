#include "third_party/gdtoa/gdtoa.internal.h"

/* clang-format off */
/****************************************************************

The author of this software is David M. Gay.

Copyright (C) 1998 by Lucent Technologies
All Rights Reserved

Permission to use, copy, modify, and distribute this software and
its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appear in all
copies and that both that the copyright notice and this
permission notice and warranty disclaimer appear in supporting
documentation, and that the name of Lucent or any of its entities
not be used in advertising or publicity pertaining to
distribution of the software without specific, written prior
permission.

LUCENT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
IN NO EVENT SHALL LUCENT OR ANY OF ITS ENTITIES BE LIABLE FOR ANY
SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
THIS SOFTWARE.

****************************************************************/

/* Please send bug reports to David M. Gay (dmg@acm.org). */

extern ULong __gdtoa_NanDflt_d[2];

char *
g_ddfmt_p(char *buf, double *dd0, int ndig, size_t bufsize, int nik)
{
	FPI fpi;
	char *b, *s, *se;
	ULong *L, bits0[4], *bits, sign, *zx;
	int bx, by, decpt, ex, ey, i, j, mode;
	Bigint *x, *y, *z;
	U *dd, ddx[2];
#ifdef MULTIPLE_THREADS
	ThInfo *TI = 0;
#endif
#ifdef Honor_FLT_ROUNDS /*{{*/
	int Rounding;
#ifdef Trust_FLT_ROUNDS /*{{ only define this if FLT_ROUNDS really works! */
	Rounding = Flt_Rounds;
#else /*}{*/
	Rounding = 1;
	switch(fegetround()) {
	  case FE_TOWARDZERO:	Rounding = 0; break;
	  case FE_UPWARD:	Rounding = 2; break;
	  case FE_DOWNWARD:	Rounding = 3;
	  }
#endif /*}}*/
#else /*}{*/
#define Rounding FPI_Round_near
#endif /*}}*/

	if (bufsize < 10 || bufsize < (size_t)(ndig + 8))
		return 0;

	dd = (U*)dd0;
	L = dd->L;
	sign = L[_0] & L[2+_0] & 0x80000000L;
	if (nik < 0 || nik > 35)
		nik = 0;
	if ((L[_0] & 0x7ff00000L) == 0x7ff00000L) {
		/* Infinity or NaN */
		if (L[_0] & 0xfffff || L[_1]) {
 nanret:
			b = buf;
			if (sign && nik < 18)
				*b++ = '-';
			b = strcp(b, NanName[nik%3]);
			if (nik > 5 && (nik < 12
					|| L[_1] != __gdtoa_NanDflt_d[0]
					|| (L[_0] ^ __gdtoa_NanDflt_d[1]) & 0xfffff
					||  L[2+_1] != __gdtoa_NanDflt_d[0]
					|| (L[2+_0] ^ __gdtoa_NanDflt_d[1]) & 0xfffff)) {
				bits0[0] = L[2+_1];
				bits0[1] = (L[2+_0] & 0xfffff) | (L[_1] << 20);
				bits0[2] = (L[_1] >> 12) | (L[_0] << 20);
				bits0[3] = (L[_0] >> 12) & 0xff;
				b = add_nanbits(b, bufsize - (b-buf), bits0, 4);
				}
			return b;
			}
		if ((L[2+_0] & 0x7ff00000) == 0x7ff00000) {
			if (L[2+_0] & 0xfffff || L[2+_1])
				goto nanret;
			if ((L[_0] ^ L[2+_0]) & 0x80000000L)
				goto nanret;	/* Infinity - Infinity */
			}
 infret:
		b = buf;
		if (L[_0] & 0x80000000L)
			*b++ = '-';
		return strcp(b, InfName[nik%6]);
		}
	if ((L[2+_0] & 0x7ff00000) == 0x7ff00000) {
		L += 2;
		if (L[_0] & 0xfffff || L[_1])
			goto nanret;
		goto infret;
		}
	if (dval(&dd[0]) + dval(&dd[1]) == 0.) {
		b = buf;
#ifndef IGNORE_ZERO_SIGN
		if (sign)
			*b++ = '-';
#endif
		*b++ = '0';
		*b = 0;
		return b;
		}
	if ((L[_0] & 0x7ff00000L) < (L[2+_0] & 0x7ff00000L)) {
		dval(&ddx[1]) = dval(&dd[0]);
		dval(&ddx[0]) = dval(&dd[1]);
		dd = ddx;
		L = dd->L;
		}
	z = d2b(dval(&dd[0]), &ex, &bx MTb);
	if (dval(&dd[1]) == 0.)
		goto no_y;
	x = z;
	y = d2b(dval(&dd[1]), &ey, &by MTb);
	if ( (i = ex - ey) !=0) {
		if (i > 0) {
			x = lshift(x, i MTb);
			ex = ey;
			}
		else
			y = lshift(y, -i MTb);
		}
	if ((L[_0] ^ L[2+_0]) & 0x80000000L) {
		z = diff(x, y MTb);
		if (L[_0] & 0x80000000L)
			z->sign = 1 - z->sign;
		}
	else {
		z = sum(x, y MTb);
		if (L[_0] & 0x80000000L)
			z->sign = 1;
		}
	Bfree(x MTb);
	Bfree(y MTb);
 no_y:
	bits = zx = z->x;
	for(i = 0; !*zx; zx++)
		i += 32;
	i += lo0bits(zx);
	if (i) {
		rshift(z, i);
		ex += i;
		}
	fpi.nbits = z->wds * 32 - hi0bits(z->x[j = z->wds-1]);
	if (fpi.nbits < 106) {
		fpi.nbits = 106;
		if (j < 3) {
			for(i = 0; i <= j; i++)
				bits0[i] = bits[i];
			while(i < 4)
				bits0[i++] = 0;
			bits = bits0;
			}
		}
	mode = 2;
	if (ndig <= 0) {
		if (bufsize < (size_t)(fpi.nbits * .301029995664) + 10) {
			Bfree(z MTb);
			return 0;
			}
		mode = 0;
		}
	fpi.emin = 1-1023-53+1;
	fpi.emax = 2046-1023-106+1;
	fpi.rounding = Rounding;
	fpi.sudden_underflow = 0;
	fpi.int_max = Int_max;
	i = STRTOG_Normal;
	s = gdtoa(&fpi, ex, bits, &i, mode, ndig, &decpt, &se);
	b = g__fmt(buf, s, se, decpt, z->sign, bufsize);
	Bfree(z MTb);
	return b;
	}
