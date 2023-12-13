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

extern ULong __gdtoa_NanDflt_d[2];

void
__gdtoa_ULtodd(ULong *L, ULong *bits, Long exp, int k)
{
	int i, j;
	switch(k & STRTOG_Retmask) {
	case STRTOG_NoNumber:
	case STRTOG_Zero:
		L[0] = L[1] = L[2] = L[3] = 0;
		break;
	case STRTOG_Normal:
		L[0] = (bits[1] >> 21 | bits[2] << 11) & (ULong)0xffffffffL;
		L[1] = (bits[2] >> 21) | (bits[3] << 11 & 0xfffff)
			| ((exp + 0x3ff + 105) << 20);
		exp += 0x3ff + 52;
		if (bits[1] &= 0x1fffff) {
			i = hi0bits(bits[1]) - 11;
			if (i >= exp) {
				i = exp - 1;
				exp = 0;
			}
			else
				exp -= i;
			if (i > 0) {
				bits[1] = bits[1] << i | bits[0] >> (32-i);
				bits[0] = bits[0] << i & (ULong)0xffffffffL;
			}
		}
		else if (bits[0]) {
			i = hi0bits(bits[0]) + 21;
			if (i >= exp) {
				i = exp - 1;
				exp = 0;
			}
			else
				exp -= i;
			if (i < 32) {
				bits[1] = bits[0] >> (32 - i);
				bits[0] = bits[0] << i & (ULong)0xffffffffL;
			}
			else {
				bits[1] = bits[0] << (i - 32);
				bits[0] = 0;
			}
		}
		else {
			L[2] = L[3] = 0;
			break;
		}
		L[2+0] = bits[0];
		L[2+1] = (bits[1] & 0xfffff) | (exp << 20);
		break;
	case STRTOG_Denormal:
		if (bits[3])
			goto nearly_normal;
		if (bits[2])
			goto partly_normal;
		if (bits[1] & 0xffe00000)
			goto hardly_normal;
		/* completely denormal */
		L[2] = L[3] = 0;
		L[0] = bits[0];
		L[1] = bits[1];
		break;
	nearly_normal:
		i = hi0bits(bits[3]) - 11;	/* i >= 12 */
		j = 32 - i;
		L[1] = ((bits[3] << i | bits[2] >> j) & 0xfffff)
			| ((65 - i) << 20);
		L[0] = (bits[2] << i | bits[1] >> j) & 0xffffffffL;
		L[2+1] = bits[1] & (((ULong)1L << j) - 1);
		L[2+0] = bits[0];
		break;
	partly_normal:
		i = hi0bits(bits[2]) - 11;
		if (i < 0) {
			j = -i;
			i += 32;
			L[1] = (bits[2] >> j & 0xfffff) | ((33 + j) << 20);
			L[0] = (bits[2] << i | bits[1] >> j) & 0xffffffffL;
			L[2+1] = bits[1] & (((ULong)1L << j) - 1);
			L[2+0] = bits[0];
			break;
		}
		if (i == 0) {
			L[1] = (bits[2] & 0xfffff) | (33 << 20);
			L[0] = bits[1];
			L[2+1] = 0;
			L[2+0] = bits[0];
			break;
		}
		j = 32 - i;
		L[1] = (((bits[2] << i) | (bits[1] >> j)) & 0xfffff)
			| ((j + 1) << 20);
		L[0] = (bits[1] << i | bits[0] >> j) & 0xffffffffL;
		L[2+1] = 0;
		L[2+0] = bits[0] & ((1L << j) - 1);
		break;
	hardly_normal:
		j = 11 - hi0bits(bits[1]);
		i = 32 - j;
		L[1] = (bits[1] >> j & 0xfffff) | ((j + 1) << 20);
		L[0] = (bits[1] << i | bits[0] >> j) & 0xffffffffL;
		L[2+1] = 0;
		L[2+0] = bits[0] & (((ULong)1L << j) - 1);
		break;
	case STRTOG_Infinite:
		L[1] = L[2+1] = 0x7ff00000;
		L[0] = L[2+0] = 0;
		break;
	case STRTOG_NaN:
		L[1] = L[1+2] = __gdtoa_NanDflt_d[1];
		L[0] = L[0+2] = __gdtoa_NanDflt_d[0];
		break;
	case STRTOG_NaNbits:
		L[0] = (bits[1] >> 20 | bits[2] << 12) & (ULong)0xffffffffL;
		L[1] = bits[2] >> 20 | bits[3] << 12;
		L[1] |= (L[0] | L[1]) ? (ULong)0x7ff00000L : (ULong)0x7ff80000L;
		L[2+0] = bits[0] & (ULong)0xffffffffL;
		L[2+1] = bits[1] & 0xfffffL;
		L[2+1] |= (L[2+0] | L[2+1]) ? (ULong)0x7ff00000L : (ULong)0x7ff80000L;
	}
	if (k & STRTOG_Neg) {
		L[1] |= 0x80000000L;
		L[2+1] |= 0x80000000L;
	}
}

int
strtordd(const char *s, char **sp, int rounding, double *dd)
{
	static const FPI fpi0 = { 106, 1-1023-53+1, 2046-1023-106+1, 1, 0, 0 /*unused*/ };
	FPI fpi1;
	const FPI *fpi;
	ULong bits[4];
	Long exp;
	int k;
	fpi = &fpi0;
	if (rounding != FPI_Round_near) {
		fpi1 = fpi0;
		fpi1.rounding = rounding;
		fpi = &fpi1;
	}
	k = strtodg(s, sp, fpi, &exp, bits);
	__gdtoa_ULtodd((ULong*)dd, bits, exp, k);
	return k;
}
