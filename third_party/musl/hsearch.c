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
#include "third_party/musl/search.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2020 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");

/*
open addressing hash table with 2^n table size
quadratic probing is used in case of hash collision
tab indices and hash are size_t
after resize fails with ENOMEM the state of tab is still usable

with the posix api items cannot be iterated and length cannot be queried
*/

#define MINSIZE 8
#define MAXSIZE ((size_t)-1/2 + 1)

struct __tab {
	ENTRY *entries;
	size_t mask;
	size_t used;
};

static struct hsearch_data htab;

static int __hcreate_r(size_t, struct hsearch_data *);
static void __hdestroy_r(struct hsearch_data *);
static int __hsearch_r(ENTRY, ACTION, ENTRY **, struct hsearch_data *);

static size_t keyhash(char *k)
{
	unsigned char *p = (void *)k;
	size_t h = 0;

	while (*p)
		h = 31*h + *p++;
	return h;
}

static int resize(size_t nel, struct hsearch_data *htab)
{
	size_t newsize;
	size_t i, j;
	size_t oldsize = htab->__tab->mask + 1;
	ENTRY *e, *newe;
	ENTRY *oldtab = htab->__tab->entries;

	if (nel > MAXSIZE)
		nel = MAXSIZE;
	for (newsize = MINSIZE; newsize < nel; newsize *= 2);
	htab->__tab->entries = calloc(newsize, sizeof *htab->__tab->entries);
	if (!htab->__tab->entries) {
		htab->__tab->entries = oldtab;
		return 0;
	}
	htab->__tab->mask = newsize - 1;
	if (!oldtab)
		return 1;
	for (e = oldtab; e < oldtab + oldsize; e++)
		if (e->key) {
			for (i=keyhash(e->key),j=1; ; i+=j++) {
				newe = htab->__tab->entries + (i & htab->__tab->mask);
				if (!newe->key)
					break;
			}
			*newe = *e;
		}
	free(oldtab);
	return 1;
}

int hcreate(size_t nel)
{
	return __hcreate_r(nel, &htab);
}

void hdestroy(void)
{
	__hdestroy_r(&htab);
}

static ENTRY *lookup(char *key, size_t hash, struct hsearch_data *htab)
{
	size_t i, j;
	ENTRY *e;

	for (i=hash,j=1; ; i+=j++) {
		e = htab->__tab->entries + (i & htab->__tab->mask);
		if (!e->key || strcmp(e->key, key) == 0)
			break;
	}
	return e;
}

ENTRY *hsearch(ENTRY item, ACTION action)
{
	ENTRY *e;

	__hsearch_r(item, action, &e, &htab);
	return e;
}

static int __hcreate_r(size_t nel, struct hsearch_data *htab)
{
	int r;

	htab->__tab = calloc(1, sizeof *htab->__tab);
	if (!htab->__tab)
		return 0;
	r = resize(nel, htab);
	if (r == 0) {
		free(htab->__tab);
		htab->__tab = 0;
	}
	return r;
}


int hcreate_r(size_t nel, struct hsearch_data *htab) {
    return __hcreate_r(nel, htab);
}

static void __hdestroy_r(struct hsearch_data *htab)
{
	if (htab->__tab) free(htab->__tab->entries);
	free(htab->__tab);
	htab->__tab = 0;
}

void hdestroy_r(struct hsearch_data *htab) {
    __hdestroy_r(htab);
}

static int __hsearch_r(ENTRY item, ACTION action, ENTRY **retval, struct hsearch_data *htab)
{
	size_t hash = keyhash(item.key);
	ENTRY *e = lookup(item.key, hash, htab);

	if (e->key) {
		*retval = e;
		return 1;
	}
	if (action == FIND) {
		*retval = 0;
		return 0;
	}
	*e = item;
	if (++htab->__tab->used > htab->__tab->mask - htab->__tab->mask/4) {
		if (!resize(2*htab->__tab->used, htab)) {
			htab->__tab->used--;
			e->key = 0;
			*retval = 0;
			return 0;
		}
		e = lookup(item.key, hash, htab);
	}
	*retval = e;
	return 1;
}

int hsearch_r(ENTRY item, ACTION action, ENTRY **retval, struct hsearch_data *htab)
{
    return __hsearch_r(item, action, retval, htab);
}
