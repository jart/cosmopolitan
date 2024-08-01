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
#define _BSD_SOURCE
#include <nl_types.h>
#include <endian.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
__static_yoink("musl_libc_notice");

#define V(p) be32toh(*(uint32_t *)(p))

static int cmp(const void *a, const void *b)
{
	uint32_t x = V(a), y = V(b);
	return x<y ? -1 : x>y ? 1 : 0;
}

char *catgets (nl_catd catd, int set_id, int msg_id, const char *s)
{
	const char *map = (const char *)catd;
	uint32_t nsets = V(map+4);
	const char *sets = map+20;
	const char *msgs = map+20+V(map+12);
	const char *strings = map+20+V(map+16);
	uint32_t set_id_be = htobe32(set_id);
	uint32_t msg_id_be = htobe32(msg_id);
	const char *set = bsearch(&set_id_be, sets, nsets, 12, cmp);
	if (!set) {
		errno = ENOMSG;
		return (char *)s;
	}
	uint32_t nmsgs = V(set+4);
	msgs += 12*V(set+8);
	const char *msg = bsearch(&msg_id_be, msgs, nmsgs, 12, cmp);
	if (!msg) {
		errno = ENOMSG;
		return (char *)s;
	}
	return (char *)(strings + V(msg+8));
}
