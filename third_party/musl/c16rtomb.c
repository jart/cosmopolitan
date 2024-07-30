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
#include <uchar.h>
#include <errno.h>
#include <wchar.h>
__static_yoink("musl_libc_notice");

#pragma GCC diagnostic ignored "-Wparentheses"

size_t c16rtomb(char *restrict s, char16_t c16, mbstate_t *restrict ps)
{
	static unsigned internal_state;
	if (!ps) ps = (void *)&internal_state;
	unsigned *x = (unsigned *)ps;
	wchar_t wc;

	if (!s) {
		if (*x) goto ilseq;
		return 1;
	}

	if (!*x && c16 - 0xd800u < 0x400) {
		*x = c16 - 0xd7c0 << 10;
		return 0;
	}

	if (*x) {
		if (c16 - 0xdc00u >= 0x400) goto ilseq;
		else wc = *x + c16 - 0xdc00;
		*x = 0;
	} else {
		wc = c16;
	}
	return wcrtomb(s, wc, 0);

ilseq:
	*x = 0;
	errno = EILSEQ;
	return -1;
}
