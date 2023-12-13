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

#define dlen 0
#define HEXDIG "0123456789abcdef"
#define ldus_QNAN0 0x7fff
#define ldus_QNAN1 0xc000
#define ldus_QNAN2 0
#define ldus_QNAN3 0
#define ldus_QNAN4 0

const char *const __gdtoa_InfName[6] = { "Infinity", "infinity", "INFINITY", "Inf", "inf", "INF" };
const char *const __gdtoa_NanName[3] = { "NaN", "nan", "NAN" };
const ULong __gdtoa_NanDflt_Q[4] = { 0xffffffff, 0xffffffff, 0xffffffff, 0x7fffffff };
const ULong __gdtoa_NanDflt_d[2] = { d_QNAN1, d_QNAN0 };
const ULong __gdtoa_NanDflt_f[1] = { f_QNAN };
const ULong __gdtoa_NanDflt_xL[3] = { 1, 0x80000000, 0x7fff0000 };
const UShort __gdtoa_NanDflt_ldus[5] = { ldus_QNAN4, ldus_QNAN3, ldus_QNAN2, ldus_QNAN1, ldus_QNAN0 };

char *
__gdtoa_g__fmt(char *b, char *s, char *se, int decpt, ULong sign, size_t blen)
{
	int i, j, k;
	char *be, *s0;
	size_t len;
	s0 = s;
	len = (se-s) + dlen + 6; /* 6 = sign + e+dd + trailing null */
	if (blen < len)
		goto ret0;
	be = b + blen - 1;
	if (sign)
		*b++ = '-';
	if (decpt <= -4 || decpt > se - s + 5) {
		*b++ = *s++;
		if (*s) {
			*b++ = '.';
			while((*b = *s++) !=0)
				b++;
		}
		*b++ = 'e';
		/* sprintf(b, "%+.2d", decpt - 1); */
		if (--decpt < 0) {
			*b++ = '-';
			decpt = -decpt;
		}
		else
			*b++ = '+';
		for(j = 2, k = 10; 10*k <= decpt; j++, k *= 10){}
		for(;;) {
			i = decpt / k;
			if (b >= be)
				goto ret0;
			*b++ = i + '0';
			if (--j <= 0)
				break;
			decpt -= i*k;
			decpt *= 10;
		}
		*b = 0;
	}
	else if (decpt <= 0) {
		*b++ = '.';
		if (be < b - decpt + (se - s))
			goto ret0;
		for(; decpt < 0; decpt++)
			*b++ = '0';
		while((*b = *s++) != 0)
			b++;
	}
	else {
		while((*b = *s++) != 0) {
			b++;
			if (--decpt == 0 && *s) {
				*b++ = '.';
			}
		}
		if (b + decpt > be) {
		ret0:
			b = 0;
			goto ret;
		}
		for(; decpt > 0; decpt--)
			*b++ = '0';
		*b = 0;
	}
ret:
	freedtoa(s0);
	return b;
}

char *
__gdtoa_add_nanbits(char *b, size_t blen, ULong *bits, int nb)
{
	ULong t;
	char *rv;
	int i, j;
	size_t L;
	while(!bits[--nb])
		if (!nb)
			return b;
	L = 8*nb + 3;
	t = bits[nb];
	do ++L; while((t >>= 4));
	if (L > blen)
		return b;
	b += L;
	*--b = 0;
	rv = b;
	*--b = ')';
	for(i = 0; i < nb; ++i) {
		t = bits[i];
		for(j = 0; j < 8; ++j, t >>= 4)
			*--b = HEXDIG[t & 0xf];
	}
	t = bits[nb];
	do *--b = HEXDIG[t & 0xf]; while(t >>= 4);
	*--b = '(';
	return rv;
}
