/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│ vi: set noet ft=c ts=8 sw=8 fenc=utf-8                                   :vi │
╚──────────────────────────────────────────────────────────────────────────────╝
│                                                                              │
│  Musl Libc                                                                   │
│  Copyright © 2005-2014 Rich Felker, et al.                                   │
│                                                                              │
│  Permission is hereby granted, free of charge, to any person obtaining       │
│  a copy of this software and associated documentation files (the             │
│  "Software"), to deal in the Software without restriction, including         │
│  without limitation the rights to use, copy, modify, merge, publish,         │
│  distribute, sublicense, and/or sell copies of the Software, and to          │
│  permit persons to whom the Software is furnished to do so, subject to       │
│  the following conditions:                                                   │
│                                                                              │
│  The above copyright notice and this permission notice shall be              │
│  included in all copies or substantial portions of the Software.             │
│                                                                              │
│  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,             │
│  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF          │
│  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.      │
│  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY        │
│  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,        │
│  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE           │
│  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                      │
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/str/str.h"
#include "libc/str/locale.internal.h"
__static_yoink("musl_libc_notice");

#pragma GCC diagnostic ignored "-Wparentheses"

static inline uint32_t swapc(uint32_t x, int c)
{
	return c ? x>>24 | x>>8&0xff00 | x<<8&0xff0000 | x<<24 : x;
}

const char *__mo_lookup(const void *p, size_t size, const char *s)
{
	const uint32_t *mo = p;
	int sw = *mo - 0x950412de;
	uint32_t b = 0, n = swapc(mo[2], sw);
	uint32_t o = swapc(mo[3], sw);
	uint32_t t = swapc(mo[4], sw);
	if (n>=size/4 || o>=size-4*n || t>=size-4*n || ((o|t)%4))
		return 0;
	o/=4;
	t/=4;
	for (;;) {
		uint32_t ol = swapc(mo[o+2*(b+n/2)], sw);
		uint32_t os = swapc(mo[o+2*(b+n/2)+1], sw);
		if (os >= size || ol >= size-os || ((char *)p)[os+ol])
			return 0;
		int sign = strcmp(s, (char *)p + os);
		if (!sign) {
			uint32_t tl = swapc(mo[t+2*(b+n/2)], sw);
			uint32_t ts = swapc(mo[t+2*(b+n/2)+1], sw);
			if (ts >= size || tl >= size-ts || ((char *)p)[ts+tl])
				return 0;
			return (char *)p + ts;
		}
		else if (n == 1) return 0;
		else if (sign < 0)
			n /= 2;
		else {
			b += n/2;
			n -= n/2;
		}
	}
	return 0;
}
