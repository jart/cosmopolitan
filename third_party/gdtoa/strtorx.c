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

extern UShort __gdtoa_NanDflt_ldus[5];

void
__gdtoa_ULtox(UShort *L, ULong *bits, Long exp, int k)
{
	switch(k & STRTOG_Retmask) {
	case STRTOG_NoNumber:
	case STRTOG_Zero:
		L[0] = L[1] = L[2] = L[3] = L[4] = 0;
		break;
	case STRTOG_Denormal:
		L[4] = 0;
		goto normal_bits;
	case STRTOG_Normal:
	case STRTOG_NaNbits:
		L[4] = exp + 0x3fff + 63;
	normal_bits:
		L[0] = (UShort)bits[0];
		L[1] = (UShort)(bits[0] >> 16);
		L[2] = (UShort)bits[1];
		L[3] = (UShort)(bits[1] >> 16);
		break;
	case STRTOG_Infinite:
		L[4] = 0x7fff;
		L[3] = 0x8000;
		L[2] = L[1] = L[0] = 0;
		break;
	case STRTOG_NaN:
		L[0] = __gdtoa_NanDflt_ldus[0];
		L[1] = __gdtoa_NanDflt_ldus[1];
		L[2] = __gdtoa_NanDflt_ldus[2];
		L[3] = __gdtoa_NanDflt_ldus[3];
		L[4] = __gdtoa_NanDflt_ldus[4];
	}
	if (k & STRTOG_Neg)
		L[4] |= 0x8000;
}

int
strtorx(const char *s, char **sp, int rounding, void *L)
{
	static const FPI fpi0 = { 64, 1-16383-64+1, 32766 - 16383 - 64 + 1, 1, SI, 0 /*unused*/ };
	FPI fpi1;
	const FPI *fpi;
	ULong bits[2];
	Long exp;
	int k;
	fpi = &fpi0;
	if (rounding != FPI_Round_near) {
		fpi1 = fpi0;
		fpi1.rounding = rounding;
		fpi = &fpi1;
	}
	k = strtodg(s, sp, fpi, &exp, bits);
	__gdtoa_ULtox((UShort*)L, bits, exp, k);
	return k;
}
