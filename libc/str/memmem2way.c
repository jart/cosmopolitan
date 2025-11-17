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
#include "libc/macros.h"
#include "libc/str/str.h"
__static_yoink("musl_libc_notice");
// clang-format off

#define BITOP(a, b, op) \
	((a)[(size_t)(b) / (8 * sizeof *(a))] op \
	    (size_t)1 << ((size_t)(b) % (8 * sizeof *(a))))

/*
 * Two Way string search algorithm, with a bad shift table applied to the last
 * byte of the window. A bit array marks which entries in the shift table are
 * initialized to avoid fully initializing a 1kb/2kb table.
 *
 * Reference: CROCHEMORE M., PERRIN D., 1991, Two-way string-matching,
 * Journal of the ACM 38(3):651-675
 */
relegated char *
__memmem2way(const unsigned char *h, const unsigned char *z,
    const unsigned char *n, size_t l)
{
	size_t i, ip, jp, k, p, ms, p0, mem, mem0;
	size_t byteset[32 / sizeof(size_t)] = { 0 };
	size_t shift[256];

	/* Computing length of needle and fill shift table */
	for (i = 0; i < l; i++)
		BITOP(byteset, n[i], |=), shift[n[i]] = i + 1;

	/* Compute maximal suffix */
	ip = -1;
	jp = 0;
	k = p = 1;
	while (jp + k < l) {
		if (n[ip + k] == n[jp + k]) {
			if (k == p) {
				jp += p;
				k = 1;
			} else
				k++;
		} else if (n[ip + k] > n[jp + k]) {
			jp += k;
			k = 1;
			p = jp - ip;
		} else {
			ip = jp++;
			k = p = 1;
		}
	}
	ms = ip;
	p0 = p;

	/* And with the opposite comparison */
	ip = -1;
	jp = 0;
	k = p = 1;
	while (jp + k < l) {
		if (n[ip + k] == n[jp + k]) {
			if (k == p) {
				jp += p;
				k = 1;
			} else
				k++;
		} else if (n[ip + k] < n[jp + k]) {
			jp += k;
			k = 1;
			p = jp - ip;
		} else {
			ip = jp++;
			k = p = 1;
		}
	}
	if (ip + 1 > ms + 1)
		ms = ip;
	else
		p = p0;

	/* Periodic needle? */
	if (memcmp(n, n + p, ms + 1)) {
		mem0 = 0;
		p = MAX(ms, l - ms - 1) + 1;
	} else
		mem0 = l - p;
	mem = 0;

	/* Search loop */
	for (;;) {
		/* If remainder of haystack is shorter than needle, done */
		if (z - h < l)
			return 0;

		/* Check last byte first; advance by shift on mismatch */
		if (BITOP(byteset, h[l - 1], &)) {
			k = l - shift[h[l - 1]];
			if (k) {
				if (k < mem)
					k = mem;
				h += k;
				mem = 0;
				continue;
			}
		} else {
			h += l;
			mem = 0;
			continue;
		}

		/* Compare right half */
		for (k = MAX(ms + 1, mem); k < l && n[k] == h[k]; k++)
			;
		if (k < l) {
			h += k - ms;
			mem = 0;
			continue;
		}
		/* Compare left half */
		for (k = ms + 1; k > mem && n[k - 1] == h[k - 1]; k--)
			;
		if (k <= mem)
			return (char *)h;
		h += p;
		mem = mem0;
	}
}
