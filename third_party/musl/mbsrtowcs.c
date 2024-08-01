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
#include <stdint.h>
#include <wchar.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "multibyte.h"
__static_yoink("musl_libc_notice");

#pragma GCC diagnostic ignored "-Wparentheses"

size_t mbsrtowcs(wchar_t *restrict ws, const char **restrict src, size_t wn, mbstate_t *restrict st)
{
	const unsigned char *s = (const void *)*src;
	size_t wn0 = wn;
	unsigned c = 0;

	if (st && (c = *(unsigned *)st)) {
		if (ws) {
			*(unsigned *)st = 0;
			goto resume;
		} else {
			goto resume0;
		}
	}

	if (MB_CUR_MAX==1) {
		if (!ws) return strlen((const char *)s);
		for (;;) {
			if (!wn) {
				*src = (const void *)s;
				return wn0;
			}
			if (!*s) break;
			c = *s++;
			*ws++ = CODEUNIT(c);
			wn--;
		}
		*ws = 0;
		*src = 0;
		return wn0-wn;
	}

	if (!ws) for (;;) {
#ifdef __GNUC__
		typedef uint32_t __attribute__((__may_alias__)) w32;
		if (*s-1u < 0x7f && (uintptr_t)s%4 == 0) {
			while (!(( *(w32*)s | *(w32*)s-0x01010101) & 0x80808080)) {
				s += 4;
				wn -= 4;
			}
		}
#endif
		if (*s-1u < 0x7f) {
			s++;
			wn--;
			continue;
		}
		if (*s-SA > SB-SA) break;
		c = bittab[*s++-SA];
resume0:
		if (OOB(c,*s)) { s--; break; }
		s++;
		if (c&(1U<<25)) {
			if (*s-0x80u >= 0x40) { s-=2; break; }
			s++;
			if (c&(1U<<19)) {
				if (*s-0x80u >= 0x40) { s-=3; break; }
				s++;
			}
		}
		wn--;
		c = 0;
	} else for (;;) {
		if (!wn) {
			*src = (const void *)s;
			return wn0;
		}
#ifdef __GNUC__
		typedef uint32_t __attribute__((__may_alias__)) w32;
		if (*s-1u < 0x7f && (uintptr_t)s%4 == 0) {
			while (wn>=5 && !(( *(w32*)s | *(w32*)s-0x01010101) & 0x80808080)) {
				*ws++ = *s++;
				*ws++ = *s++;
				*ws++ = *s++;
				*ws++ = *s++;
				wn -= 4;
			}
		}
#endif
		if (*s-1u < 0x7f) {
			*ws++ = *s++;
			wn--;
			continue;
		}
		if (*s-SA > SB-SA) break;
		c = bittab[*s++-SA];
resume:
		if (OOB(c,*s)) { s--; break; }
		c = (c<<6) | *s++-0x80;
		if (c&(1U<<31)) {
			if (*s-0x80u >= 0x40) { s-=2; break; }
			c = (c<<6) | *s++-0x80;
			if (c&(1U<<31)) {
				if (*s-0x80u >= 0x40) { s-=3; break; }
				c = (c<<6) | *s++-0x80;
			}
		}
		*ws++ = c;
		wn--;
		c = 0;
	}

	if (!c && !*s) {
		if (ws) {
			*ws = 0;
			*src = 0;
		}
		return wn0-wn;
	}
	errno = EILSEQ;
	if (ws) *src = (const void *)s;
	return -1;
}
