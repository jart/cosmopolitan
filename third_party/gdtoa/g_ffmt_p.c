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

extern ULong __gdtoa_NanDflt_f[1];

char*
g_ffmt_p(char *buf, float *f, int ndig, size_t bufsize, int nik)
{
	static const FPI fpi0 = { 24, 1-127-24+1,  254-127-24+1, 1, 0, 6 };
	char *b, *s, *se;
	ULong bits[1], *L, sign;
	int decpt, ex, i, mode;
#include "third_party/gdtoa/gdtoa_fltrnds.inc"
	if (ndig < 0)
		ndig = 0;
	if (bufsize < (size_t)(ndig + 10))
		return 0;
	L = (ULong*)f;
	sign = L[0] & 0x80000000L;
	if ((L[0] & 0x7f800000) == 0x7f800000) {
		/* Infinity or NaN */
		if (nik < 0 || nik > 35)
			nik = 0;
		if ((bits[0] = L[0] & 0x7fffff)) {
			b = buf;
			if (sign && nik < 18)
				*b++ = '-';
			b = stpcpy(b, __gdtoa_NanName[nik%3]);
			if (nik > 5 && (nik < 12
					|| (bits[0] ^ __gdtoa_NanDflt_f[0]) & 0x7fffff))
				b = __gdtoa_add_nanbits(b, bufsize - (b-buf), bits, 1);
			return b;
		}
		b = buf;
		if (sign)
			*b++ = '-';
		return stpcpy(b, __gdtoa_InfName[nik%6]);
	}
	if (*f == 0.) {
		b = buf;
		if (L[0] & 0x80000000L)
			*b++ = '-';
		*b++ = '0';
		*b = 0;
		return b;
	}
	bits[0] = L[0] & 0x7fffff;
	if ( (ex = (L[0] >> 23) & 0xff) !=0)
		bits[0] |= 0x800000;
	else
		ex = 1;
	ex -= 0x7f + 23;
	mode = 2;
	if (ndig <= 0) {
		if (bufsize < 16)
			return 0;
		mode = 0;
	}
	i = STRTOG_Normal;
	s = gdtoa(fpi, ex, bits, &i, mode, ndig, &decpt, &se);
	return __gdtoa_g__fmt(buf, s, se, decpt, sign, bufsize);
}
