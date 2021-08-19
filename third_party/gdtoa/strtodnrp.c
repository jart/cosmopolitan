/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│vi: set et ft=c ts=8 tw=8 fenc=utf-8                                       :vi│
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
/* clang-format off */

/* This is a variant of strtod that works on Intel ia32 systems */
/* with the default extended-precision arithmetic -- it does not */
/* require setting the precision control to 53 bits.  */

double
strtod(const char *s, char **sp)
{
	static const FPI fpi = { 53, 1-1023-53+1, 2046-1023-53+1, 1, SI, 0 /*unused*/ };
	ULong bits[2];
	Long exp;
	int k;
	union { ULong L[2]; double d; } u;
	k = strtodg(s, sp, &fpi, &exp, bits);
	switch(k & STRTOG_Retmask) {
	case STRTOG_NoNumber:
	case STRTOG_Zero:
		u.L[0] = u.L[1] = 0;
		break;
	case STRTOG_Normal:
		u.L[0] = bits[0];
		u.L[1] = (bits[1] & ~0x100000) | ((exp + 0x3ff + 52) << 20);
		break;
	case STRTOG_Denormal:
		u.L[0] = bits[0];
		u.L[1] = bits[1];
		break;
	case STRTOG_Infinite:
		u.L[1] = 0x7ff00000;
		u.L[0] = 0;
		break;
	case STRTOG_NaN:
		u.L[0] = d_QNAN0;
		u.L[1] = d_QNAN1;
		break;
	case STRTOG_NaNbits:
		u.L[1] = 0x7ff00000 | bits[1];
		u.L[0] = bits[0];
	}
	if (k & STRTOG_Neg)
		u.L[1] |= 0x80000000L;
	return u.d;
}
