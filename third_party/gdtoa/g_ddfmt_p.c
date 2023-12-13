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
#include "libc/runtime/fenv.h"
#include "libc/str/str.h"
#include "third_party/gdtoa/gdtoa.internal.h"

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
	ThInfo *TI = 0;
	if (bufsize < 10 || bufsize < (size_t)(ndig + 8))
		return 0;
	dd = (U*)dd0;
	L = dd->L;
	sign = L[1] & L[2+1] & 0x80000000L;
	if (nik < 0 || nik > 35)
		nik = 0;
	if ((L[1] & 0x7ff00000L) == 0x7ff00000L) {
		/* Infinity or NaN */
		if (L[1] & 0xfffff || L[0]) {
		nanret:
			b = buf;
			if (sign && nik < 18)
				*b++ = '-';
			b = stpcpy(b, __gdtoa_NanName[nik%3]);
			if (nik > 5 && (nik < 12
					|| L[0] != __gdtoa_NanDflt_d[0]
					|| (L[1] ^ __gdtoa_NanDflt_d[1]) & 0xfffff
					||  L[2+0] != __gdtoa_NanDflt_d[0]
					|| (L[2+1] ^ __gdtoa_NanDflt_d[1]) & 0xfffff)) {
				bits0[0] = L[2+0];
				bits0[1] = (L[2+1] & 0xfffff) | (L[0] << 20);
				bits0[2] = (L[0] >> 12) | (L[1] << 20);
				bits0[3] = (L[1] >> 12) & 0xff;
				b = __gdtoa_add_nanbits(b, bufsize - (b-buf), bits0, 4);
			}
			return b;
		}
		if ((L[2+1] & 0x7ff00000) == 0x7ff00000) {
			if (L[2+1] & 0xfffff || L[2+0])
				goto nanret;
			if ((L[1] ^ L[2+1]) & 0x80000000L)
				goto nanret;	/* Infinity - Infinity */
		}
	infret:
		b = buf;
		if (L[1] & 0x80000000L)
			*b++ = '-';
		return stpcpy(b, __gdtoa_InfName[nik%6]);
	}
	if ((L[2+1] & 0x7ff00000) == 0x7ff00000) {
		L += 2;
		if (L[1] & 0xfffff || L[0])
			goto nanret;
		goto infret;
	}
	if (dval(&dd[0]) + dval(&dd[1]) == 0.) {
		b = buf;
		if (sign)
			*b++ = '-';
		*b++ = '0';
		*b = 0;
		return b;
	}
	if ((L[1] & 0x7ff00000L) < (L[2+1] & 0x7ff00000L)) {
		dval(&ddx[1]) = dval(&dd[0]);
		dval(&ddx[0]) = dval(&dd[1]);
		dd = ddx;
		L = dd->L;
	}
	z = __gdtoa_d2b(dval(&dd[0]), &ex, &bx, &TI);
	if (dval(&dd[1]) == 0.)
		goto no_y;
	x = z;
	y = __gdtoa_d2b(dval(&dd[1]), &ey, &by, &TI);
	if ( (i = ex - ey) !=0) {
		if (i > 0) {
			x = __gdtoa_lshift(x, i, &TI);
			ex = ey;
		}
		else
			y = __gdtoa_lshift(y, -i, &TI);
	}
	if ((L[1] ^ L[2+1]) & 0x80000000L) {
		z = __gdtoa_diff(x, y, &TI);
		if (L[1] & 0x80000000L)
			z->sign = 1 - z->sign;
	}
	else {
		z = __gdtoa_sum(x, y, &TI);
		if (L[1] & 0x80000000L)
			z->sign = 1;
	}
	__gdtoa_Bfree(x, &TI);
	__gdtoa_Bfree(y, &TI);
no_y:
	bits = zx = z->x;
	for(i = 0; !*zx; zx++)
		i += 32;
	i += lo0bits(zx);
	if (i) {
		__gdtoa_rshift(z, i);
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
			__gdtoa_Bfree(z, &TI);
			return 0;
		}
		mode = 0;
	}
	fpi.emin = 1-1023-53+1;
	fpi.emax = 2046-1023-106+1;
	fpi.rounding = FLT_ROUNDS;
	fpi.sudden_underflow = 0;
	fpi.int_max = Int_max;
	i = STRTOG_Normal;
	s = gdtoa(&fpi, ex, bits, &i, mode, ndig, &decpt, &se);
	b = __gdtoa_g__fmt(buf, s, se, decpt, z->sign, bufsize);
	__gdtoa_Bfree(z, &TI);
	return b;
}
