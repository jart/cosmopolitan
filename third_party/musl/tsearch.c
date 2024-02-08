/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│ vi: set noet ft=c ts=8 sw=8 fenc=utf-8                                   :vi │
╚──────────────────────────────────────────────────────────────────────────────╝
│                                                                              │
│  Musl Libc                                                                   │
│  Copyright © 2005-2020 Rich Felker, et al.                                   │
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
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "third_party/musl/tsearch.internal.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2020 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");

static inline int height(struct node *n) { return n ? n->h : 0; }

static int rot(void **p, struct node *x, int dir /* deeper side */)
{
	struct node *y = x->a[dir];
	struct node *z = y->a[!dir];
	int hx = x->h;
	int hz = height(z);
	if (hz > height(y->a[dir])) {
		/*
		 *   x
		 *  / \ dir          z
		 * A   y            / \
		 *    / \   -->    x   y
		 *   z   D        /|   |\
		 *  / \          A B   C D
		 * B   C
		 */
		x->a[dir] = z->a[!dir];
		y->a[!dir] = z->a[dir];
		z->a[!dir] = x;
		z->a[dir] = y;
		x->h = hz;
		y->h = hz;
		z->h = hz+1;
	} else {
		/*
		 *   x               y
		 *  / \             / \
		 * A   y    -->    x   D
		 *    / \         / \
		 *   z   D       A   z
		 */
		x->a[dir] = z;
		y->a[!dir] = x;
		x->h = hz+1;
		y->h = hz+2;
		z = y;
	}
	*p = z;
	return z->h - hx;
}

/* balance *p, return 0 if height is unchanged.  */
int __tsearch_balance(void **p)
{
	struct node *n = *p;
	int h0 = height(n->a[0]);
	int h1 = height(n->a[1]);
	if (h0 - h1 + 1u < 3u) {
		int old = n->h;
		n->h = h0<h1 ? h1+1 : h0+1;
		return n->h - old;
	}
	return rot(p, n, h0<h1);
}

void *tsearch(const void *key, void **rootp,
	int (*cmp)(const void *, const void *))
{
	if (!rootp)
		return 0;

	void **a[MAXH];
	struct node *n = *rootp;
	struct node *r;
	int i=0;
	a[i++] = rootp;
	for (;;) {
		if (!n)
			break;
		int c = cmp(key, n->key);
		if (!c)
			return n;
		a[i++] = &n->a[c>0];
		n = n->a[c>0];
	}
	r = malloc(sizeof *r);
	if (!r)
		return 0;
	r->key = key;
	r->a[0] = r->a[1] = 0;
	r->h = 1;
	/* insert new node, rebalance ancestors.  */
	*a[--i] = r;
	while (i && __tsearch_balance(a[--i]));
	return r;
}
