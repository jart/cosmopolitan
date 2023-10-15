#ifndef COSMOPOLITAN_THIRDPARTY_MUSL_TSEARCH_H
#define COSMOPOLITAN_THIRDPARTY_MUSL_TSEARCH_H

#include "third_party/musl/search.h"

/* AVL tree height < 1.44*log2(nodes+2)-0.3, MAXH is a safe upper bound.  */
#define MAXH (sizeof(void*)*8*3/2)

struct node {
	const void *key;
	void *a[2];
	int h;
};

int __tsearch_balance(void **);

#endif
