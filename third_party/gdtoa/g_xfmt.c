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

char*
g_xfmt(char *buf, void *V, int ndig, size_t bufsize)
{
	static const FPI fpi0 = { 64, 1-16383-64+1, 32766 - 16383 - 64 + 1, 1, 0, Int_max };
	char *b, *s, *se;
	ULong bits[2], sign;
	UShort *L;
	int decpt, ex, i, mode;
#include "third_party/gdtoa/gdtoa_fltrnds.inc"
	if (ndig < 0)
		ndig = 0;
	if (bufsize < (size_t)(ndig + 10))
		return 0;
	L = (UShort *)V;
	sign = L[4] & 0x8000;
	bits[1] = ((ULong)L[3] << 16) | L[2];
	bits[0] = ((ULong)L[1] << 16) | L[0];
	if ( (ex = L[4] & 0x7fff) !=0) {
		if (ex == 0x7fff) {
			/* Infinity or NaN */
			if (!bits[0] && bits[1]== 0x80000000) {
				b = buf;
				if (sign)
					*b++ = '-';
				b = stpcpy(b, "Infinity");
			}
			else
				b = stpcpy(buf, "NaN");
			return b;
		}
		i = STRTOG_Normal;
	}
	else if (bits[0] | bits[1]) {
		i = STRTOG_Denormal;
		ex = 1;
	}
	else {
		b = buf;
		if (sign)
			*b++ = '-';
		*b++ = '0';
		*b = 0;
		return b;
	}
	ex -= 0x3fff + 63;
	mode = 2;
	if (ndig <= 0) {
		if (bufsize < 32)
			return 0;
		mode = 0;
	}
	s = gdtoa(fpi, ex, bits, &i, mode, ndig, &decpt, &se);
	return __gdtoa_g__fmt(buf, s, se, decpt, sign, bufsize);
}
