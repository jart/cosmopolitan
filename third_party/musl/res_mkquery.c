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
#include "libc/sysv/consts/clock.h"
#include "libc/stdio/rand.h"
#include "third_party/musl/resolv.internal.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");

int __res_mkquery(int op, const char *dname, int class, int type,
	const unsigned char *data, int datalen,
	const unsigned char *newrr, unsigned char *buf, int buflen)
{
	int id, i, j;
	unsigned char q[280];
	size_t l = strnlen(dname, 255);
	int n;

	if (l && dname[l-1]=='.') l--;
	if (l && dname[l-1]=='.') return -1;
	n = 17+l+!!l;
	if (l>253 || buflen<n || op>15u || class>255u || type>255u)
		return -1;

	/* Construct query template - ID will be filled later */
	memset(q, 0, n);
	q[2] = op*8 + 1;
	q[3] = 32; /* AD */
	q[5] = 1;
	memcpy((char *)q+13, dname, l);
	for (i=13; q[i]; i=j+1) {
		for (j=i; q[j] && q[j] != '.'; j++);
		if (j-i-1u > 62u) return -1;
		q[i-1] = j-i;
	}
	q[i+1] = type;
	q[i+3] = class;

	/* Make a reasonably unpredictable id */
	id = _rand64();
	q[0] = id >> 8;
	q[1] = id;

	memcpy(buf, q, n);
	return n;
}

__weak_reference(__res_mkquery, res_mkquery);
