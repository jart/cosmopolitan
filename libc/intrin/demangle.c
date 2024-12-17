/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│ vi: set noet ft=c ts=8 sw=8 fenc=utf-8                                   :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright (c) 2007 Hyogeol Lee <hyogeollee@gmail.com>                        │
│ Copyright (c) 2015-2017 Kai Wang <kaiwang27@gmail.com>                       │
│ Copyright (c) 2024 Justine Tunney <jtunney@gmail.com>                        │
│ All rights reserved.                                                         │
│                                                                              │
│ Redistribution and use in source and binary forms, with or without           │
│ modification, are permitted provided that the following conditions           │
│ are met:                                                                     │
│ 1. Redistributions of source code must retain the above copyright            │
│    notice, this list of conditions and the following disclaimer              │
│    in this position and unchanged.                                           │
│ 2. Redistributions in binary form must reproduce the above copyright         │
│    notice, this list of conditions and the following disclaimer in the       │
│    documentation and/or other materials provided with the distribution.      │
│                                                                              │
│ THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR        │
│ IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES    │
│ OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.      │
│ IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,             │
│ INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT     │
│ NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,    │
│ DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY        │
│ THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT          │
│ (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF     │
│ THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.            │
╚─────────────────────────────────────────────────────────────────────────────*/
#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

__notice(demangle_notice, "\
Cosmopolitan libelftc demangle (BSD-2)\n\
Copyright (c) 2007 Hyogeol Lee <hyogeollee@gmail.com>\n\
Copyright (c) 2015-2017 Kai Wang <kaiwang27@gmail.com>\n\
Copyright (c) 2024 Justine Tunney <jtunney@gmail.com>");
// https://sourceforge.net/p/elftoolchain/code/HEAD/tree/trunk/libelftc/libelftc_dem_gnu3.c
// https://github.com/freebsd/freebsd-src/blob/2176c9ab71c85efd90a6c7af4a9e04fe8e3d49ca/contrib/libcxxrt/libelftc_dem_gnu3.c
// clang-format off

/**
 * @file demangle.c
 * @brief Decode IA-64 C++ ABI style implementation.
 *
 * See the following IA-64 standard ABI(Itanium C++ ABI) references:
 *
 * - http://www.codesourcery.com/cxx-abi/abi.html#mangling
 * - http://www.codesourcery.com/cxx-abi/abi-mangling.html
 *
 * This file has been forked and modified by Cosmopolitan Libc. We found
 * it in the FreeBSD codebase, who got it from libcxxrt, who got it from
 * libelftc on SourceForge. Here's what we changed:
 *
 * - We fixed a memory corruption bug (since upstreamed to libcxxrt)
 *
 * - We ensured slightly better conformance to libcxxabi test suites.
 *
 * - We renovated this module to not use malloc(). Our modified version
 *   uses a new __demangle() API we defined with an strlcpy()-style. The
 *   output buffer provided by the caller is used internally as a heap.
 *
 * - This is now the only C++ demangler that's asynchronous signal safe.
 *   You now have the power to print C++ backtraces from signal handlers
 *   without needing to block signals, because this code is reentrant,
 *   and 100% lockless, and uses absolutely no writable static memory.
 *   That makes it also great for kernel and embedded development.
 *
 * - We made it go 2x faster. It's almost as fast as libcxxabi now. The
 *   lightweight Dennis Ritchie style demangle_malloc() implementation
 *   helped. What also helped is introducing stack_str and strlcpy().
 *
 * - We made it use 3x less memory. This came with the tradeoff of
 *   imposing limitations similar to embedded software. Rather than
 *   using pointers, we use 16-bit indexes into a heap that can grow no
 *   larger than 64kb. Please note that a buffer size of 20kb is more
 *   than sufficient to pass all the libcxxabi test cases.
 *
 * - We made it have zero dependencies. If you compile this file with
 *   GCC -ffreestanding flag then it won't link a single libc function.
 *   Using heavyweight libc strings library functions doesn't make any
 *   sense for a C++ demangler, which wokrs on strings a few chars long.
 *
 * - The floating point support is disabled by default, but it can be
 *   re-enabled by commenting out `DEMANGLE_NO_FLOATING_POINT`. Although
 *   Doing this will undermine the kernel / signal safety guarantees.
 *
 * Linking this demangler introduces at least 20kb of binary footprint.
 *
 */

#define ABI privileged optimizesize

#define DEMANGLE_NO_FLOATING_POINT

#define ASSERT(x)	    (void)0

#define BUFFER_GROWFACTOR   2
#define BUFFER_GROW(x)	    (((x) + 1) * BUFFER_GROWFACTOR)

#define ELFTC_FAILURE	    0
#define ELFTC_ISDIGIT(C)    ('0' <= (C) && (C) <= '9')
#define ELFTC_SUCCESS	    1

#define VECTOR_DEF_CAPACITY 1
#define MAX_DEPTH           20

typedef unsigned short index_t;

struct stack_str {
	char *str;
	int len;
	int cap;
	char buf[128];
};

struct vector_str {
	int size;
	int capacity;
	index_t *container;
};

enum type_qualifier {
	TYPE_PTR,
	TYPE_REF,
	TYPE_CMX,
	TYPE_IMG,
	TYPE_EXT,
	TYPE_RST,
	TYPE_VAT,
	TYPE_CST,
	TYPE_VEC,
	TYPE_RREF
};

struct vector_type_qualifier {
	int size, capacity;
	enum type_qualifier *q_container;
	struct vector_str ext_name;
};

enum read_cmd {
	READ_FAIL,
	READ_NEST,
	READ_TMPL,
	READ_EXPR,
	READ_EXPL,
	READ_LOCAL,
	READ_TYPE,
	READ_FUNC,
	READ_PTRMEM
};

struct read_cmd_item {
	enum read_cmd cmd;
	void *data;
};

struct vector_read_cmd {
	int size, capacity;
	struct read_cmd_item *r_container;
};

enum push_qualifier {
	PUSH_ALL_QUALIFIER,
	PUSH_CV_QUALIFIER,
	PUSH_NON_CV_QUALIFIER,
};

struct demangle_data {
	int hoff;
	index_t free;
	uintptr_t heap;
	struct vector_str output; /* output string vector */
	struct vector_str subst;  /* substitution string vector */
	struct vector_str tmpl;
	struct vector_str class_type;
	struct vector_str *cur_output; /* ptr to current output vec */
	struct vector_read_cmd cmd;
	bool mem_rst;				/* restrict member function */
	bool mem_vat;				/* volatile member function */
	bool mem_cst;				/* const member function */
	bool mem_ref;				/* lvalue-ref member func */
	bool mem_rref;				/* rvalue-ref member func */
	bool is_tmpl;				/* template args */
	bool is_functype;			/* function type */
	bool ref_qualifier;			/* ref qualifier */
	bool is_guard_variable;			/* guarden varis */
	enum type_qualifier ref_qualifier_type; /* ref qualifier type */
	enum push_qualifier push_qualifier;	/* which qualifiers to push */
	int func_type;
	int depth;
	const char *cur;	/* current mangled name ptr */
	const char *last_sname; /* last source name */
	intptr_t jmpbuf[5];
};

struct type_delimit {
	bool paren;
	bool firstp;
};

#define DEMANGLE_TRY_LIMIT	128
#define FLOAT_SPRINTF_TRY_LIMIT 5
#define FLOAT_QUADRUPLE_BYTES	16
#define FLOAT_EXTENED_BYTES	10

#define SIMPLE_HASH(x, y)	(64 * x + y)
#define DEM_PUSH_STR(d, s)	demangle_push_str((d), (s), demangle_strlen(s))
#define VEC_PUSH_STR(d, s) \
	demangle_vector_str_push(ddata, (d), (s), demangle_strlen(s))
#define VEC_STR(d, v, i) ((char *)((d)->heap + (v)->container[i]))

static int demangle_read_encoding(struct demangle_data *);
static int demangle_read_expr_primary(struct demangle_data *);
static int demangle_read_expression(struct demangle_data *);
static int demangle_read_function(struct demangle_data *, int *,
    struct vector_type_qualifier *);
static int demangle_read_name(struct demangle_data *);
static int demangle_read_name_flat(struct demangle_data *, char **);
static int demangle_read_sname(struct demangle_data *);
static int demangle_read_subst(struct demangle_data *);
static int demangle_read_type(struct demangle_data *, struct type_delimit *);

ABI static size_t
demangle_strlen(const char *s)
{
	size_t n = 0;
	while (*s++) {
		asm volatile("" ::: "memory");
		++n;
	}
	return n;
}

ABI static char *
demangle_stpcpy(char *d, const char *s)
{
	size_t i = 0;
	for (;;) {
		if (!(d[i] = s[i]))
			return d + i;
		++i;
	}
}

ABI static void *
demangle_mempcpy(void *a, const void *b, size_t n)
{
	char *d = a;
	const char *s = b;
	while (n--)
		*d++ = *s++;
	return d;
}

ABI static void *
demangle_memcpy(void *a, const void *b, size_t n)
{
	demangle_mempcpy(a, b, n);
	return a;
}

ABI static int
demangle_strncmp(const char *a, const char *b, size_t n)
{
	size_t i = 0;
	if (!n-- || a == b)
		return 0;
	while (i < n && a[i] == b[i] && b[i])
		++i;
	return (a[i] & 0xff) - (b[i] & 0xff);
}

ABI static int
demangle_memcmp(const void *a, const void *b, size_t n)
{
	int c;
	const unsigned char *p, *q;
	if ((p = a) == (q = b) || !n)
		return 0;
	if ((c = *p - *q))
		return c;
	for (; n; ++p, ++q, --n)
		if ((c = *p - *q))
			return c;
	return 0;
}

ABI static void
demangle_strlcpy(char *dst, const char *src, size_t dsize)
{
	size_t remain;
	if ((remain = dsize))
		while (--remain)
			if (!(*dst++ = *src++))
				break;
	if (!remain && dsize)
		*dst = 0;
}

ABI static long
demangle_strtol(const char *s, int base)
{
	static const uint8_t demangle_base36[80] = { 1, 2, 3, 4, 5, 6, 7, 8, 9,
		10, 0, 0, 0, 0, 0, 0, 0, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
		21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
		0, 0, 0, 0, 0, 0, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
		22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 0,
		0, 0, 0, 0 };
	int c;
	long x = 0;
	while ((c = *s++ & 255) < 128 && c - '0' >= 0 &&
	    (c = demangle_base36[c - '0']) && --c < base)
		x = x * base + c;
	return x;
}

ABI static char *
demangle_strstr(const char *haystack, const char *needle)
{
	size_t i;
	if (haystack == needle || !*needle)
		return (void *)haystack;
	for (;;) {
		for (i = 0;; ++i) {
			if (!needle[i])
				return (/*unconst*/ char *)haystack;
			if (!haystack[i])
				break;
			if (needle[i] != haystack[i])
				break;
		}
		if (!*haystack++)
			break;
	}
	return 0;
}

ABI static char *
demangle_utoa(char *p, unsigned long long x)
{
	char t;
	size_t i, a, b;
	i = 0;
	do {
		p[i++] = x % 10 + '0';
		x = x / 10;
	} while (x > 0);
	p[i] = '\0';
	if (i) {
		for (a = 0, b = i - 1; a < b; ++a, --b) {
			t = p[a];
			p[a] = p[b];
			p[b] = t;
		}
	}
	return p + i;
}

ABI static char *
demangle_itoa(char *p, long long x)
{
	if (x < 0)
		*p++ = '-', x = -(unsigned long long)x;
	return demangle_utoa(p, x);
}

ABI static void
demangle_free(struct demangle_data *h, void *ptr)
{
	index_t base;
	uintptr_t mem;
	if (ptr) {
		mem = (uintptr_t)ptr;
		base = mem - h->heap;
		if (base - sizeof(index_t) == h->hoff) {
			h->hoff += ((index_t *)mem)[-1];
		} else {
			*(index_t *)mem = h->free;
			h->free = base;
		}
	}
}

ABI static returnspointerwithnoaliases returnsnonnull void *
demangle_malloc(struct demangle_data *h, long a, long n)
{
	long rem;
	uintptr_t ptr;
	index_t next, next2;
	index_t *link, *link2;
	long b = sizeof(index_t);

	if (n < 0 || n >= 32768)
		__builtin_longjmp(h->jmpbuf, 1);

	/* Roundup size. */
	n += a - 1;
	n &= -a;
	n += b - 1;
	n &= -b;

	/* Try free list. */
	next = h->free;
	link = &h->free;
	while (next) {
		link2 = (index_t *)(h->heap + next);
		next2 = *link2;
		if (!(next & (a - 1)) && (rem = link2[-1] - n) >= 0) {
			if (rem < (b << 1)) {
				*link = next2;
			} else {
				/* Split chunk. */
				link2[-1] = n;
				*link = next + n + b;
				link = (index_t *)(h->heap + next + n + b);
				link[-1] = rem - b;
				link[0] = next2;
			}
			return link2;
		}
		next = next2;
		link = link2;
	}

	/* Allocate new memory. */
	rem = h->hoff;
	h->hoff -= n;
	h->hoff &= -a;
	h->hoff &= -b;
	if (h->hoff >= (b << 1)) {
		n = rem - h->hoff;
		ptr = h->heap + h->hoff;
		h->hoff -= b;
		((index_t *)ptr)[-1] = n;
		return (void *)ptr;
	} else {
		__builtin_longjmp(h->jmpbuf, 1);
	}
}

ABI static returnspointerwithnoaliases char *
demangle_strdup(struct demangle_data *h, const char *s)
{
	char *d = 0;
	if (s) {
		size_t n = demangle_strlen(s) + 1;
		d = (char *)demangle_malloc(h, 1, n);
		demangle_memcpy(d, s, n);
	}
	return d;
}

ABI static void
demangle_vector_str_dest(struct demangle_data *h, struct vector_str *v)
{
	int i;
	for (i = 0; i < v->size; ++i)
		demangle_free(h, VEC_STR(h, v, v->size - i - 1));
	demangle_free(h, v->container);
}

ABI static void
demangle_vector_type_qualifier_dest(struct demangle_data *d,
    struct vector_type_qualifier *v)
{
	demangle_free(d, v->q_container);
	demangle_vector_str_dest(d, &v->ext_name);
}

ABI static void
demangle_stack_str_init(struct stack_str *ss)
{
	ss->str = ss->buf;
	ss->buf[0] = 0;
	ss->len = 0;
	ss->cap = sizeof(ss->buf);
}

ABI static void
demangle_stack_str_append(struct demangle_data *h, struct stack_str *ss,
    const char *str, size_t len)
{
	int len2 = ss->len + len;
	int need = len2 + 1;
	if (need > ss->cap) {
		int cap2;
		char *str2;
		cap2 = need + (ss->cap >> 1);
		str2 = (char *)demangle_malloc(h, 1, cap2);
		demangle_memcpy(str2, ss->str, ss->len);
		ss->str = str2;
		ss->cap = cap2;
	}
	demangle_memcpy(ss->str + ss->len, str, len);
	ss->str[len2] = 0;
	ss->len = len2;
}

#define demangle_stack_str_append_str(h, ss, s) \
	demangle_stack_str_append(h, ss, s, demangle_strlen(s))

ABI static size_t
demangle_get_strlen_sum(struct demangle_data *h, const struct vector_str *v)
{
	size_t i, len = 0;
	ASSERT(v->size > 0);
	for (i = 0; i < v->size; ++i)
		len += demangle_strlen(VEC_STR(h, v, i));
	return len;
}

ABI static int
demangle_demangle_strncmp(const char *a, const char *b, size_t n)
{
	size_t i = 0;
	if (!n-- || a == b)
		return 0;
	while (i < n && a[i] == b[i] && b[i])
		++i;
	return (a[i] & 0xff) - (b[i] & 0xff);
}

/**
 * @brief Find string in vector_str.
 * @param v Destination vector.
 * @param o String to find.
 * @param l Length of the string.
 * @return -1 at failed, 0 at not found, 1 at found.
 */
ABI static int
demangle_vector_str_find(struct demangle_data *h, const struct vector_str *v,
    const char *o, size_t l)
{
	size_t i;

	if (!o)
		return -1;

	for (i = 0; i < v->size; ++i)
		if (!demangle_demangle_strncmp(VEC_STR(h, v, i), o, l))
			return 1;

	return 0;
}

/**
 * @brief Get new allocated flat string from vector.
 *
 * If l is not NULL, return length of the string.
 * @param v Destination vector.
 * @param l Length of the string.
 * @return NULL at failed or NUL terminated new allocated string.
 */
ABI static char *
demangle_vector_str_get_flat(struct demangle_data *ddata,
    const struct vector_str *v, size_t *l)
{
	size_t i;
	char *rtn, *p;
	size_t rtn_size;

	if (!v->size)
		return 0;

	if (!(rtn_size = demangle_get_strlen_sum(ddata, v)))
		return 0;

	rtn = (char *)demangle_malloc(ddata, 1, rtn_size + 1);

	p = rtn;
	for (i = 0; i < v->size; ++i)
		p = demangle_stpcpy(p, VEC_STR(ddata, v, i));

	if (l)
		*l = rtn_size;

	return rtn;
}

ABI static void
demangle_vector_str_grow(struct demangle_data *ddata, struct vector_str *v)
{
	size_t i, tmp_cap;
	index_t *tmp_ctn;

	ASSERT(v->capacity > 0);

	tmp_cap = BUFFER_GROW(v->capacity);

	ASSERT(tmp_cap > v->capacity);

	tmp_ctn = (index_t *)demangle_malloc(ddata, alignof(index_t),
	    sizeof(index_t) * tmp_cap);

	for (i = 0; i < v->size; ++i)
		tmp_ctn[i] = v->container[i];

	demangle_free(ddata, v->container);

	v->container = tmp_ctn;
	v->capacity = tmp_cap;
}

/**
 * @brief Initialize vector_str.
 * @return false at failed, true at success.
 */
ABI static void
demangle_vector_str_init(struct demangle_data *ddata, struct vector_str *v)
{
	v->size = 0;
	v->capacity = VECTOR_DEF_CAPACITY;

	ASSERT(v->capacity > 0);

	v->container = (index_t *)demangle_malloc(ddata, alignof(index_t),
	    sizeof(index_t) * v->capacity);
}

/**
 * @brief Remove last element in vector_str.
 * @return false at failed, true at success.
 */
ABI static bool
demangle_vector_str_pop(struct vector_str *v)
{
	if (!v)
		return false;

	if (!v->size)
		return true;

	--v->size;

	v->container[v->size] = 0;

	return true;
}

/**
 * @brief Push back string to vector.
 * @return false at failed, true at success.
 */
ABI static bool
demangle_vector_str_push(struct demangle_data *ddata, struct vector_str *v,
    const char *str, size_t len)
{
	if (!v || !str)
		return false;

	if (v->size == v->capacity)
		demangle_vector_str_grow(ddata, v);

	v->container[v->size] = (uintptr_t)demangle_malloc(ddata, 1, len + 1) -
	    ddata->heap;

	demangle_strlcpy(VEC_STR(ddata, v, v->size), str, len + 1);

	++v->size;

	return true;
}

/**
 * @brief Push front org vector to det vector.
 * @return false at failed, true at success.
 */
ABI static bool
demangle_vector_str_push_vector_head(struct demangle_data *ddata,
    struct vector_str *dst, struct vector_str *org)
{
	size_t i, tmp_cap;
	index_t *tmp_ctn;

	tmp_cap = BUFFER_GROW(dst->size + org->size);

	tmp_ctn = (index_t *)demangle_malloc(ddata, alignof(index_t),
	    sizeof(index_t) * tmp_cap);

	for (i = 0; i < org->size; ++i)
		tmp_ctn[i] = (uintptr_t)demangle_strdup(ddata,
				 VEC_STR(ddata, org, i)) -
		    ddata->heap;

	for (i = 0; i < dst->size; ++i)
		tmp_ctn[i + org->size] = dst->container[i];

	demangle_free(ddata, dst->container);

	dst->container = tmp_ctn;
	dst->capacity = tmp_cap;
	dst->size += org->size;

	return true;
}

/**
 * @brief Push org vector to the tail of det vector.
 * @return false at failed, true at success.
 */
ABI static bool
demangle_vector_str_push_vector(struct demangle_data *ddata,
    struct vector_str *dst, struct vector_str *org)
{
	size_t i, tmp_cap;
	index_t *tmp_ctn;

	if (!dst || !org)
		return false;

	tmp_cap = BUFFER_GROW(dst->size + org->size);

	tmp_ctn = (index_t *)demangle_malloc(ddata, alignof(index_t),
	    sizeof(index_t) * tmp_cap);

	for (i = 0; i < dst->size; ++i)
		tmp_ctn[i] = dst->container[i];

	for (i = 0; i < org->size; ++i)
		tmp_ctn[i + dst->size] = (uintptr_t)demangle_strdup(ddata,
					     VEC_STR(ddata, org, i)) -
		    ddata->heap;

	demangle_free(ddata, dst->container);

	dst->container = tmp_ctn;
	dst->capacity = tmp_cap;
	dst->size += org->size;

	return true;
}

/**
 * @brief Get new allocated flat string from vector between begin and end.
 *
 * If r_len is not NULL, string length will be returned.
 * @return NULL at failed or NUL terminated new allocated string.
 */
ABI static returnspointerwithnoaliases char *
demangle_vector_str_substr(struct demangle_data *ddata,
    const struct vector_str *v, size_t begin, size_t end, size_t *r_len)
{
	char *rtn, *p;
	size_t i, len;

	if (!v || begin > end)
		return 0;

	len = 0;
	for (i = begin; i < end + 1; ++i)
		len += demangle_strlen(VEC_STR(ddata, v, i));

	rtn = (char *)demangle_malloc(ddata, 1, len + 1);

	if (r_len)
		*r_len = len;

	p = rtn;
	for (i = begin; i < end + 1; ++i)
		p = demangle_stpcpy(p, VEC_STR(ddata, v, i));

	return rtn;
}

ABI static int
demangle_vector_read_cmd_pop(struct vector_read_cmd *v)
{
	if (!v->size)
		return 0;

	--v->size;
	v->r_container[v->size].cmd = READ_FAIL;
	v->r_container[v->size].data = NULL;

	return 1;
}

ABI static void
demangle_vector_read_cmd_init(struct demangle_data *ddata,
    struct vector_read_cmd *v)
{
	v->size = 0;
	v->capacity = VECTOR_DEF_CAPACITY;

	v->r_container = (struct read_cmd_item *)demangle_malloc(ddata,
	    alignof(*v->r_container), sizeof(*v->r_container) * v->capacity);
}

ABI static void
demangle_data_init(struct demangle_data *d, const char *cur)
{
	demangle_vector_str_init(d, &d->output);
	demangle_vector_str_init(d, &d->subst);
	demangle_vector_str_init(d, &d->tmpl);
	demangle_vector_str_init(d, &d->class_type);
	demangle_vector_read_cmd_init(d, &d->cmd);

	ASSERT(d->output.container);
	ASSERT(d->subst.container);
	ASSERT(d->tmpl.container);
	ASSERT(d->class_type.container);

	d->mem_rst = false;
	d->mem_vat = false;
	d->mem_cst = false;
	d->mem_ref = false;
	d->mem_rref = false;
	d->is_tmpl = false;
	d->is_functype = false;
	d->ref_qualifier = false;
	d->is_guard_variable = false;
	d->push_qualifier = PUSH_ALL_QUALIFIER;
	d->func_type = 0;
	d->cur = cur;
	d->cur_output = &d->output;
	d->last_sname = NULL;
}

ABI static int
demangle_push_str(struct demangle_data *ddata, const char *str, size_t len)
{
	if (!str || !len)
		return 0;

	/*
	 * is_tmpl is used to check if the type (function arg) is right next
	 * to template args, and should always be cleared whenever new string
	 * pushed.
	 */
	ddata->is_tmpl = false;

	return demangle_vector_str_push(ddata, ddata->cur_output, str, len);
}

#ifndef DEMANGLE_NO_FLOATING_POINT
ABI static int
demangle_push_fp(struct demangle_data *ddata,
    char *decoder(struct demangle_data *, const char *, size_t))
{
	size_t len;
	int rtn;
	const char *fp;
	char *f;

	if (!decoder)
		return 0;

	fp = ddata->cur;
	while (*ddata->cur != 'E')
		++ddata->cur;

	if (!(f = decoder(ddata, fp, ddata->cur - fp)))
		return 0;

	rtn = 0;
	if ((len = demangle_strlen(f)) > 0)
		rtn = demangle_push_str(ddata, f, len);

	++ddata->cur;

	return rtn;
}
#endif // DEMANGLE_NO_FLOATING_POINT

ABI static int
demangle_pop_str(struct demangle_data *ddata)
{
	return demangle_vector_str_pop(ddata->cur_output);
}

ABI static int
demangle_push_subst(struct demangle_data *ddata, const char *str, size_t len)
{
	if (!str || !len)
		return 0;

	if (!demangle_vector_str_find(ddata, &ddata->subst, str, len))
		return demangle_vector_str_push(ddata, &ddata->subst, str, len);

	return 1;
}

ABI static int
demangle_push_subst_v(struct demangle_data *ddata, struct vector_str *v)
{
	int rtn;
	char *str;
	size_t str_len;

	if (!v)
		return 0;

	if (!(str = demangle_vector_str_get_flat(ddata, v, &str_len)))
		return 0;

	rtn = demangle_push_subst(ddata, str, str_len);

	demangle_free(ddata, str);

	return rtn;
}

ABI static int
demangle_push_type_qualifier(struct demangle_data *ddata,
    struct vector_type_qualifier *v, const char *type_str)
{
	struct stack_str subst_v;
	enum type_qualifier t;
	size_t idx, e_idx, e_len;
	char *buf;
	bool cv;

	if (!v)
		return 0;

	if (!(idx = v->size))
		return 1;

	if (type_str) {
		demangle_stack_str_init(&subst_v);
		demangle_stack_str_append_str(ddata, &subst_v, type_str);
	}

	cv = true;
	e_idx = 0;
	while (idx > 0) {
		switch (v->q_container[idx - 1]) {
		case TYPE_PTR:
			cv = false;
			if (ddata->push_qualifier == PUSH_CV_QUALIFIER)
				break;
			if (!DEM_PUSH_STR(ddata, "*"))
				return 0;
			if (type_str) {
				demangle_stack_str_append_str(ddata, &subst_v,
				    "*");
				if (!demangle_push_subst(ddata, subst_v.str,
					subst_v.len))
					return 0;
			}
			break;

		case TYPE_REF:
			cv = false;
			if (ddata->push_qualifier == PUSH_CV_QUALIFIER)
				break;
			if (!DEM_PUSH_STR(ddata, "&"))
				return 0;
			if (type_str) {
				demangle_stack_str_append_str(ddata, &subst_v,
				    "&");
				if (!demangle_push_subst(ddata, subst_v.str,
					subst_v.len))
					return 0;
			}
			break;

		case TYPE_RREF:
			cv = false;
			if (ddata->push_qualifier == PUSH_CV_QUALIFIER)
				break;
			if (!DEM_PUSH_STR(ddata, "&&"))
				return 0;
			if (type_str) {
				demangle_stack_str_append_str(ddata, &subst_v,
				    "&&");
				if (!demangle_push_subst(ddata, subst_v.str,
					subst_v.len))
					return 0;
			}
			break;

		case TYPE_CMX:
			cv = false;
			if (ddata->push_qualifier == PUSH_CV_QUALIFIER)
				break;
			if (!DEM_PUSH_STR(ddata, " complex"))
				return 0;
			if (type_str) {
				demangle_stack_str_append_str(ddata, &subst_v,
				    " complex");
				if (!demangle_push_subst(ddata, subst_v.str,
					subst_v.len))
					return 0;
			}
			break;

		case TYPE_IMG:
			cv = false;
			if (ddata->push_qualifier == PUSH_CV_QUALIFIER)
				break;
			if (!DEM_PUSH_STR(ddata, " imaginary"))
				return 0;
			if (type_str) {
				demangle_stack_str_append_str(ddata, &subst_v,
				    " imaginary");
				if (!demangle_push_subst(ddata, subst_v.str,
					subst_v.len))
					return 0;
			}
			break;

		case TYPE_EXT:
			cv = false;
			if (ddata->push_qualifier == PUSH_CV_QUALIFIER)
				break;
			if (!v->ext_name.size || e_idx > v->ext_name.size - 1)
				return 0;
			if (!(e_len = demangle_strlen(
				  VEC_STR(ddata, (&v->ext_name), e_idx))))
				return 0;
			buf = (char *)demangle_malloc(ddata, 1, e_len + 2);
			buf[0] = ' ';
			demangle_memcpy(buf + 1,
			    VEC_STR(ddata, &v->ext_name, e_idx), e_len + 1);

			if (!DEM_PUSH_STR(ddata, buf))
				return 0;

			if (type_str) {
				demangle_stack_str_append_str(ddata, &subst_v,
				    buf);
				if (!demangle_push_subst(ddata, subst_v.str,
					subst_v.len))
					return 0;
			}
			++e_idx;
			break;

		case TYPE_RST:
			if (ddata->push_qualifier == PUSH_NON_CV_QUALIFIER &&
			    cv)
				break;
			if (ddata->push_qualifier == PUSH_CV_QUALIFIER && !cv)
				break;
			if (!DEM_PUSH_STR(ddata, " restrict"))
				return 0;
			if (type_str) {
				demangle_stack_str_append_str(ddata, &subst_v,
				    " restrict");
				if (idx - 1 > 0) {
					t = v->q_container[idx - 2];
					if (t == TYPE_RST || t == TYPE_VAT ||
					    t == TYPE_CST)
						break;
				}
				if (!demangle_push_subst(ddata, subst_v.str,
					subst_v.len))
					return 0;
			}
			break;

		case TYPE_VAT:
			if (ddata->push_qualifier == PUSH_NON_CV_QUALIFIER &&
			    cv)
				break;
			if (ddata->push_qualifier == PUSH_CV_QUALIFIER && !cv)
				break;
			if (!DEM_PUSH_STR(ddata, " volatile"))
				return 0;
			if (type_str) {
				demangle_stack_str_append_str(ddata, &subst_v,
				    " volatile");
				if (idx - 1 > 0) {
					t = v->q_container[idx - 2];
					if (t == TYPE_RST || t == TYPE_VAT ||
					    t == TYPE_CST)
						break;
				}
				if (!demangle_push_subst(ddata, subst_v.str,
					subst_v.len))
					return 0;
			}
			break;

		case TYPE_CST:
			if (ddata->push_qualifier == PUSH_NON_CV_QUALIFIER &&
			    cv && !ddata->is_functype) { /* [jart] upstream */
				break;
			}
			if (ddata->push_qualifier == PUSH_CV_QUALIFIER && !cv) {
				break;
			}
			if (!DEM_PUSH_STR(ddata, " const"))
				return 0;
			if (type_str) {
				demangle_stack_str_append_str(ddata, &subst_v,
				    " const");
				if (idx - 1 > 0) {
					t = v->q_container[idx - 2];
					if (t == TYPE_RST || t == TYPE_VAT ||
					    t == TYPE_CST)
						break;
				}
				if (!demangle_push_subst(ddata, subst_v.str,
					subst_v.len))
					return 0;
			}
			break;

		case TYPE_VEC: {
			char *p;
			cv = false;
			if (ddata->push_qualifier == PUSH_CV_QUALIFIER)
				break;
			if (!v->ext_name.size || e_idx > v->ext_name.size - 1)
				return 0;
			if (!(e_len = demangle_strlen(
				  VEC_STR(ddata, &v->ext_name, e_idx))))
				return 0;
			buf = (char *)demangle_malloc(ddata, 1, e_len + 12);
			p = buf;
			p = demangle_stpcpy(p, " __vector(");
			p = (char *)demangle_mempcpy(p,
			    VEC_STR(ddata, &v->ext_name, e_idx), e_len);
			p = demangle_stpcpy(p, ")");
			if (!DEM_PUSH_STR(ddata, buf))
				return 0;
			if (type_str) {
				demangle_stack_str_append_str(ddata, &subst_v,
				    buf);
				if (!demangle_push_subst(ddata, subst_v.str,
					subst_v.len))
					return 0;
			}
			++e_idx;
			break;
		}
		}
		--idx;
	}

	return 1;
}

ABI static int
demangle_get_subst(struct demangle_data *ddata, size_t idx)
{
	size_t len;

	if (ddata->subst.size <= idx)
		return 0;
	if (!(len = demangle_strlen(VEC_STR(ddata, &ddata->subst, idx))))
		return 0;
	if (!demangle_push_str(ddata, VEC_STR(ddata, &ddata->subst, idx), len))
		return 0;

	/* skip '_' */
	++ddata->cur;

	return 1;
}

ABI static int
demangle_get_tmpl_param(struct demangle_data *ddata, size_t idx)
{
	size_t len;

	if (ddata->tmpl.size <= idx)
		return 0;
	if (!(len = demangle_strlen(VEC_STR(ddata, &ddata->tmpl, idx))))
		return 0;
	if (!demangle_push_str(ddata, VEC_STR(ddata, &ddata->tmpl, idx), len))
		return 0;

	++ddata->cur;

	return 1;
}

ABI static int
demangle_read_array(struct demangle_data *ddata)
{
	size_t i, num_len, exp_len, p_idx, idx;
	const char *num;
	char *exp;

	if (!*(++ddata->cur))
		return 0;

	if (*ddata->cur == '_') {
		if (!*(++ddata->cur))
			return 0;

		if (!demangle_read_type(ddata, NULL))
			return 0;

		if (!DEM_PUSH_STR(ddata, " []"))
			return 0;
	} else {
		if (ELFTC_ISDIGIT(*ddata->cur)) {
			num = ddata->cur;
			while (ELFTC_ISDIGIT(*ddata->cur))
				++ddata->cur;
			if (*ddata->cur != '_')
				return 0;
			num_len = ddata->cur - num;
			ASSERT(num_len > 0);
			if (*(++ddata->cur) == '\0')
				return 0;
			if (!demangle_read_type(ddata, NULL))
				return 0;
			if (!DEM_PUSH_STR(ddata, " ["))
				return 0;
			if (!demangle_push_str(ddata, num, num_len))
				return 0;
			if (!DEM_PUSH_STR(ddata, "]"))
				return 0;
		} else {
			p_idx = ddata->output.size;
			if (!demangle_read_expression(ddata))
				return 0;
			if (!(exp = demangle_vector_str_substr(ddata,
				  &ddata->output, p_idx, ddata->output.size - 1,
				  &exp_len)))
				return 0;
			idx = ddata->output.size;
			for (i = p_idx; i < idx; ++i)
				if (!demangle_vector_str_pop(&ddata->output))
					return 0;
			if (*ddata->cur != '_')
				return 0;
			++ddata->cur;
			if (*ddata->cur == '\0')
				return 0;
			if (!demangle_read_type(ddata, NULL))
				return 0;
			if (!DEM_PUSH_STR(ddata, " ["))
				return 0;
			if (!demangle_push_str(ddata, exp, exp_len))
				return 0;
			if (!DEM_PUSH_STR(ddata, "]"))
				return 0;
		}
	}

	return 1;
}

#ifndef DEMANGLE_NO_FLOATING_POINT

/* Simple hex to integer function used by decode_to_* function. */
ABI static int
hex_to_dec(char c)
{
	switch (c) {
	case '0':
		return 0;
	case '1':
		return 1;
	case '2':
		return 2;
	case '3':
		return 3;
	case '4':
		return 4;
	case '5':
		return 5;
	case '6':
		return 6;
	case '7':
		return 7;
	case '8':
		return 8;
	case '9':
		return 9;
	case 'a':
		return 10;
	case 'b':
		return 11;
	case 'c':
		return 12;
	case 'd':
		return 13;
	case 'e':
		return 14;
	case 'f':
		return 15;
	default:
		return -1;
	}
}

/*
 * Decode floating point representation to string
 * Return new allocated string or NULL
 *
 * Todo
 * Replace these functions to macro.
 */
ABI static returnspointerwithnoaliases char *
decode_fp_to_double(struct demangle_data *ddata, const char *p, size_t len)
{
	double f;
	size_t rtn_len, limit, i;
	int byte;
	char *rtn;

	if (!p || !len || len % 2 || len / 2 > sizeof(double))
		return 0;

	memset(&f, 0, sizeof(double));

	for (i = 0; i < len / 2; ++i) {
		byte = hex_to_dec(p[len - i * 2 - 1]) +
		    hex_to_dec(p[len - i * 2 - 2]) * 16;

		if (byte < 0 || byte > 255)
			return 0;

#if ELFTC_BYTE_ORDER == ELFTC_BYTE_ORDER_LITTLE_ENDIAN
		((unsigned char *)&f)[i] = (unsigned char)(byte);
#else  /* ELFTC_BYTE_ORDER != ELFTC_BYTE_ORDER_LITTLE_ENDIAN */
		((unsigned char *)&f)[sizeof(double) - i - 1] =
		    (unsigned char)(byte);
#endif /* ELFTC_BYTE_ORDER == ELFTC_BYTE_ORDER_LITTLE_ENDIAN */
	}

	rtn_len = 64;
	limit = 0;
again:
	rtn = (char *)demangle_malloc(ddata, alignof(char),
	    sizeof(char) * rtn_len);

	if (snprintf(rtn, rtn_len, "%fld", f) >= (int)rtn_len) {
		if (limit++ > FLOAT_SPRINTF_TRY_LIMIT)
			return 0;
		rtn_len *= BUFFER_GROWFACTOR;
		goto again;
	}

	return rtn;
}

ABI static returnspointerwithnoaliases char *
decode_fp_to_float(struct demangle_data *ddata, const char *p, size_t len)
{
	size_t i, rtn_len, limit;
	float f;
	int byte;
	char *rtn;

	if (!p || !len || len % 2 || len / 2 > sizeof(float))
		return 0;

	memset(&f, 0, sizeof(float));

	for (i = 0; i < len / 2; ++i) {
		byte = hex_to_dec(p[len - i * 2 - 1]) +
		    hex_to_dec(p[len - i * 2 - 2]) * 16;
		if (byte < 0 || byte > 255)
			return 0;
#if ELFTC_BYTE_ORDER == ELFTC_BYTE_ORDER_LITTLE_ENDIAN
		((unsigned char *)&f)[i] = (unsigned char)(byte);
#else  /* ELFTC_BYTE_ORDER != ELFTC_BYTE_ORDER_LITTLE_ENDIAN */
		((unsigned char *)&f)[sizeof(float) - i - 1] =
		    (unsigned char)(byte);
#endif /* ELFTC_BYTE_ORDER == ELFTC_BYTE_ORDER_LITTLE_ENDIAN */
	}

	rtn_len = 64;
	limit = 0;
again:
	rtn = (char *)demangle_malloc(ddata, alignof(char),
	    sizeof(char) * rtn_len);

	if (snprintf(rtn, rtn_len, "%ff", f) >= (int)rtn_len) {
		if (limit++ > FLOAT_SPRINTF_TRY_LIMIT)
			return 0;
		rtn_len *= BUFFER_GROWFACTOR;
		goto again;
	}

	return rtn;
}

ABI static returnspointerwithnoaliases char *
decode_fp_to_long_double(struct demangle_data *ddata, const char *p, size_t len)
{
	long double f;
	size_t rtn_len, limit, i;
	int byte;
	char *rtn;

	if (!p || !len || len % 2 || len / 2 > sizeof(long double))
		return 0;

	memset(&f, 0, sizeof(long double));

	for (i = 0; i < len / 2; ++i) {
		byte = hex_to_dec(p[len - i * 2 - 1]) +
		    hex_to_dec(p[len - i * 2 - 2]) * 16;

		if (byte < 0 || byte > 255)
			return 0;

#if ELFTC_BYTE_ORDER == ELFTC_BYTE_ORDER_LITTLE_ENDIAN
		((unsigned char *)&f)[i] = (unsigned char)(byte);
#else  /* ELFTC_BYTE_ORDER != ELFTC_BYTE_ORDER_LITTLE_ENDIAN */
		((unsigned char *)&f)[sizeof(long double) - i - 1] =
		    (unsigned char)(byte);
#endif /* ELFTC_BYTE_ORDER == ELFTC_BYTE_ORDER_LITTLE_ENDIAN */
	}

	rtn_len = 256;
	limit = 0;
again:
	rtn = (char *)demangle_malloc(ddata, alignof(char),
	    sizeof(char) * rtn_len);

	if (snprintf(rtn, rtn_len, "%Lfd", f) >= (int)rtn_len) {
		if (limit++ > FLOAT_SPRINTF_TRY_LIMIT)
			return 0;
		rtn_len *= BUFFER_GROWFACTOR;
		goto again;
	}

	return rtn;
}

ABI static returnspointerwithnoaliases char *
decode_fp_to_float128(struct demangle_data *ddata, const char *p, size_t len)
{
	long double f;
	size_t rtn_len, limit, i;
	int byte;
	unsigned char buf[FLOAT_QUADRUPLE_BYTES];
	char *rtn;

	switch (sizeof(long double)) {
	case FLOAT_QUADRUPLE_BYTES:
		return decode_fp_to_long_double(ddata, p, len);
	case FLOAT_EXTENED_BYTES:
		if (!p || !len || len % 2 || len / 2 > FLOAT_QUADRUPLE_BYTES)
			return 0;

		memset(buf, 0, FLOAT_QUADRUPLE_BYTES);

		for (i = 0; i < len / 2; ++i) {
			byte = hex_to_dec(p[len - i * 2 - 1]) +
			    hex_to_dec(p[len - i * 2 - 2]) * 16;
			if (byte < 0 || byte > 255)
				return 0;
#if ELFTC_BYTE_ORDER == ELFTC_BYTE_ORDER_LITTLE_ENDIAN
			buf[i] = (unsigned char)(byte);
#else  /* ELFTC_BYTE_ORDER != ELFTC_BYTE_ORDER_LITTLE_ENDIAN */
			buf[FLOAT_QUADRUPLE_BYTES - i - 1] =
			    (unsigned char)(byte);
#endif /* ELFTC_BYTE_ORDER == ELFTC_BYTE_ORDER_LITTLE_ENDIAN */
		}
		memset(&f, 0, FLOAT_EXTENED_BYTES);

#if ELFTC_BYTE_ORDER == ELFTC_BYTE_ORDER_LITTLE_ENDIAN
		demangle_memcpy(&f, buf, FLOAT_EXTENED_BYTES);
#else  /* ELFTC_BYTE_ORDER != ELFTC_BYTE_ORDER_LITTLE_ENDIAN */
		demangle_memcpy(&f, buf + 6, FLOAT_EXTENED_BYTES);
#endif /* ELFTC_BYTE_ORDER == ELFTC_BYTE_ORDER_LITTLE_ENDIAN */

		rtn_len = 256;
		limit = 0;
	again:
		rtn = (char *)demangle_malloc(ddata, alignof(char),
		    sizeof(char) * rtn_len);

		if (snprintf(rtn, rtn_len, "%Lfd", f) >= (int)rtn_len) {
			if (limit++ > FLOAT_SPRINTF_TRY_LIMIT)
				return 0;
			rtn_len *= BUFFER_GROWFACTOR;
			goto again;
		}

		return rtn;
	default:
		return 0;
	}
}

ABI static returnspointerwithnoaliases char *
decode_fp_to_float80(struct demangle_data *ddata, const char *p, size_t len)
{
	long double f;
	size_t rtn_len, limit, i;
	int byte;
	unsigned char buf[FLOAT_EXTENED_BYTES];
	char *rtn;

	switch (sizeof(long double)) {
	case FLOAT_QUADRUPLE_BYTES:
		if (!p || !len || len % 2 || len / 2 > FLOAT_EXTENED_BYTES)
			return 0;

		memset(buf, 0, FLOAT_EXTENED_BYTES);

		for (i = 0; i < len / 2; ++i) {
			byte = hex_to_dec(p[len - i * 2 - 1]) +
			    hex_to_dec(p[len - i * 2 - 2]) * 16;

			if (byte < 0 || byte > 255)
				return 0;

#if ELFTC_BYTE_ORDER == ELFTC_BYTE_ORDER_LITTLE_ENDIAN
			buf[i] = (unsigned char)(byte);
#else  /* ELFTC_BYTE_ORDER != ELFTC_BYTE_ORDER_LITTLE_ENDIAN */
			buf[FLOAT_EXTENED_BYTES - i - 1] =
			    (unsigned char)(byte);
#endif /* ELFTC_BYTE_ORDER == ELFTC_BYTE_ORDER_LITTLE_ENDIAN */
		}

		memset(&f, 0, FLOAT_QUADRUPLE_BYTES);

#if ELFTC_BYTE_ORDER == ELFTC_BYTE_ORDER_LITTLE_ENDIAN
		demangle_memcpy(&f, buf, FLOAT_EXTENED_BYTES);
#else  /* ELFTC_BYTE_ORDER != ELFTC_BYTE_ORDER_LITTLE_ENDIAN */
		demangle_memcpy((unsigned char *)(&f) + 6, buf,
		    FLOAT_EXTENED_BYTES);
#endif /* ELFTC_BYTE_ORDER == ELFTC_BYTE_ORDER_LITTLE_ENDIAN */

		rtn_len = 256;
		limit = 0;
	again:
		rtn = (char *)demangle_malloc(ddata, alignof(char),
		    sizeof(char) * rtn_len);

		if (snprintf(rtn, rtn_len, "%Lfd", f) >= (int)rtn_len) {
			if (limit++ > FLOAT_SPRINTF_TRY_LIMIT)
				return 0;
			rtn_len *= BUFFER_GROWFACTOR;
			goto again;
		}

		return rtn;
	case FLOAT_EXTENED_BYTES:
		return decode_fp_to_long_double(ddata, p, len);
	default:
		return 0;
	}
}

#endif // DEMANGLE_NO_FLOATING_POINT

ABI static int
demangle_read_expr_primary(struct demangle_data *ddata)
{
	const char *num;

	if (*(++ddata->cur) == '\0')
		return 0;

	if (*ddata->cur == '_' && *(ddata->cur + 1) == 'Z') {
		ddata->cur += 2;
		if (*ddata->cur == '\0')
			return 0;
		if (!demangle_read_encoding(ddata))
			return 0;
		++ddata->cur;
		return 1;
	}

	switch (*ddata->cur) {
	case 'b':
		if (*(ddata->cur + 2) != 'E')
			return 0;
		switch (*(++ddata->cur)) {
		case '0':
			ddata->cur += 2;
			return DEM_PUSH_STR(ddata, "false");
		case '1':
			ddata->cur += 2;
			return DEM_PUSH_STR(ddata, "true");
		default:
			return 0;
		}

#ifndef DEMANGLE_NO_FLOATING_POINT

	case 'd':
		++ddata->cur;
		return demangle_push_fp(ddata, decode_fp_to_double);

	case 'e':
		++ddata->cur;
		if (sizeof(long double) == 10)
			return demangle_push_fp(ddata, decode_fp_to_double);
		return demangle_push_fp(ddata, decode_fp_to_float80);

	case 'f':
		++ddata->cur;
		return demangle_push_fp(ddata, decode_fp_to_float);

	case 'g':
		++ddata->cur;
		if (sizeof(long double) == 16)
			return demangle_push_fp(ddata, decode_fp_to_double);
		return demangle_push_fp(ddata, decode_fp_to_float128);

#endif // DEMANGLE_NO_FLOATING_POINT

	case 'i':
	case 'j':
	case 'l':
	case 'm':
	case 'n':
	case 's':
	case 't':
	case 'x':
	case 'y':
		if (*(++ddata->cur) == 'n') {
			if (!DEM_PUSH_STR(ddata, "-"))
				return 0;
			++ddata->cur;
		}
		num = ddata->cur;
		while (*ddata->cur != 'E') {
			if (!ELFTC_ISDIGIT(*ddata->cur))
				return 0;
			++ddata->cur;
		}
		++ddata->cur;
		return demangle_push_str(ddata, num, ddata->cur - num - 1);

	default:
		return 0;
	}
}

/*
 * Read local source name.
 *
 * References:
 *   http://gcc.gnu.org/bugzilla/show_bug.cgi?id=31775
 *   http://gcc.gnu.org/viewcvs?view=rev&revision=124467
 */
ABI static int
demangle_local_source_name(struct demangle_data *ddata)
{
	/* L */
	if (*ddata->cur != 'L')
		return 0;
	++ddata->cur;

	/* source name */
	if (!demangle_read_sname(ddata))
		return 0;

	/* discriminator */
	if (*ddata->cur == '_') {
		++ddata->cur;
		while (ELFTC_ISDIGIT(*ddata->cur))
			++ddata->cur;
	}

	return 1;
}

/*
 * read unqualified-name, unqualified name are operator-name, ctor-dtor-name,
 * source-name
 */
ABI static int
demangle_read_uqname(struct demangle_data *ddata)
{
	size_t len;

	if (*ddata->cur == '\0')
		return 0;

	/* operator name */
	switch (SIMPLE_HASH(*ddata->cur, *(ddata->cur + 1))) {
	case SIMPLE_HASH('a', 'a'):
		/* operator && */
		if (!DEM_PUSH_STR(ddata, "operator&&"))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('a', 'd'):
		/* operator & (unary) */
		if (!DEM_PUSH_STR(ddata, "operator&"))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('a', 'n'):
		/* operator & */
		if (!DEM_PUSH_STR(ddata, "operator&"))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('a', 'N'):
		/* operator &= */
		if (!DEM_PUSH_STR(ddata, "operator&="))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('a', 'S'):
		/* operator = */
		if (!DEM_PUSH_STR(ddata, "operator="))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('c', 'l'):
		/* operator () */
		if (!DEM_PUSH_STR(ddata, "operator()"))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('c', 'm'):
		/* operator , */
		if (!DEM_PUSH_STR(ddata, "operator,"))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('c', 'o'):
		/* operator ~ */
		if (!DEM_PUSH_STR(ddata, "operator~"))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('c', 'v'):
		/* operator (cast) */
		if (!DEM_PUSH_STR(ddata, "operator(cast)"))
			return 0;
		ddata->cur += 2;
		return demangle_read_type(ddata, NULL);

	case SIMPLE_HASH('d', 'a'):
		/* operator delete [] */
		if (!DEM_PUSH_STR(ddata, "operator delete []"))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('d', 'e'):
		/* operator * (unary) */
		if (!DEM_PUSH_STR(ddata, "operator*"))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('d', 'l'):
		/* operator delete */
		if (!DEM_PUSH_STR(ddata, "operator delete"))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('d', 'v'):
		/* operator / */
		if (!DEM_PUSH_STR(ddata, "operator/"))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('d', 'V'):
		/* operator /= */
		if (!DEM_PUSH_STR(ddata, "operator/="))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('e', 'o'):
		/* operator ^ */
		if (!DEM_PUSH_STR(ddata, "operator^"))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('e', 'O'):
		/* operator ^= */
		if (!DEM_PUSH_STR(ddata, "operator^="))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('e', 'q'):
		/* operator == */
		if (!DEM_PUSH_STR(ddata, "operator=="))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('g', 'e'):
		/* operator >= */
		if (!DEM_PUSH_STR(ddata, "operator>="))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('g', 't'):
		/* operator > */
		if (!DEM_PUSH_STR(ddata, "operator>"))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('i', 'x'):
		/* operator [] */
		if (!DEM_PUSH_STR(ddata, "operator[]"))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('l', 'e'):
		/* operator <= */
		if (!DEM_PUSH_STR(ddata, "operator<="))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('l', 's'):
		/* operator << */
		if (!DEM_PUSH_STR(ddata, "operator<<"))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('l', 'S'):
		/* operator <<= */
		if (!DEM_PUSH_STR(ddata, "operator<<="))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('l', 't'):
		/* operator < */
		if (!DEM_PUSH_STR(ddata, "operator<"))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('m', 'i'):
		/* operator - */
		if (!DEM_PUSH_STR(ddata, "operator-"))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('m', 'I'):
		/* operator -= */
		if (!DEM_PUSH_STR(ddata, "operator-="))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('m', 'l'):
		/* operator * */
		if (!DEM_PUSH_STR(ddata, "operator*"))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('m', 'L'):
		/* operator *= */
		if (!DEM_PUSH_STR(ddata, "operator*="))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('m', 'm'):
		/* operator -- */
		if (!DEM_PUSH_STR(ddata, "operator--"))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('n', 'a'):
		/* operator new[] */
		if (!DEM_PUSH_STR(ddata, "operator new []"))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('n', 'e'):
		/* operator != */
		if (!DEM_PUSH_STR(ddata, "operator!="))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('n', 'g'):
		/* operator - (unary) */
		if (!DEM_PUSH_STR(ddata, "operator-"))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('n', 't'):
		/* operator ! */
		if (!DEM_PUSH_STR(ddata, "operator!"))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('n', 'w'):
		/* operator new */
		if (!DEM_PUSH_STR(ddata, "operator new"))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('o', 'o'):
		/* operator || */
		if (!DEM_PUSH_STR(ddata, "operator||"))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('o', 'r'):
		/* operator | */
		if (!DEM_PUSH_STR(ddata, "operator|"))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('o', 'R'):
		/* operator |= */
		if (!DEM_PUSH_STR(ddata, "operator|="))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('p', 'l'):
		/* operator + */
		if (!DEM_PUSH_STR(ddata, "operator+"))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('p', 'L'):
		/* operator += */
		if (!DEM_PUSH_STR(ddata, "operator+="))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('p', 'm'):
		/* operator ->* */
		if (!DEM_PUSH_STR(ddata, "operator->*"))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('p', 'p'):
		/* operator ++ */
		if (!DEM_PUSH_STR(ddata, "operator++"))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('p', 's'):
		/* operator + (unary) */
		if (!DEM_PUSH_STR(ddata, "operator+"))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('p', 't'):
		/* operator -> */
		if (!DEM_PUSH_STR(ddata, "operator->"))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('q', 'u'):
		/* operator ? */
		if (!DEM_PUSH_STR(ddata, "operator?"))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('r', 'm'):
		/* operator % */
		if (!DEM_PUSH_STR(ddata, "operator%"))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('r', 'M'):
		/* operator %= */
		if (!DEM_PUSH_STR(ddata, "operator%="))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('r', 's'):
		/* operator >> */
		if (!DEM_PUSH_STR(ddata, "operator>>"))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('r', 'S'):
		/* operator >>= */
		if (!DEM_PUSH_STR(ddata, "operator>>="))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('r', 'z'):
		/* operator sizeof */
		if (!DEM_PUSH_STR(ddata, "operator sizeof "))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('s', 'r'):
		/* scope resolution operator */
		if (!DEM_PUSH_STR(ddata, "scope resolution operator "))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('s', 'v'):
		/* operator sizeof */
		if (!DEM_PUSH_STR(ddata, "operator sizeof "))
			return 0;
		ddata->cur += 2;
		return 1;
	}

	/* vendor extened operator */
	if (*ddata->cur == 'v' && ELFTC_ISDIGIT(*(ddata->cur + 1))) {
		if (!DEM_PUSH_STR(ddata, "vendor extened operator "))
			return 0;
		if (!demangle_push_str(ddata, ddata->cur + 1, 1))
			return 0;
		ddata->cur += 2;
		return demangle_read_sname(ddata);
	}

	/* ctor-dtor-name */
	switch (SIMPLE_HASH(*ddata->cur, *(ddata->cur + 1))) {
	case SIMPLE_HASH('C', '1'):
	case SIMPLE_HASH('C', '2'):
	case SIMPLE_HASH('C', '3'):
	case SIMPLE_HASH('C', '4'):
	case SIMPLE_HASH('C', '5'):
		if (!ddata->last_sname)
			return 0;
		if (!(len = demangle_strlen(ddata->last_sname)))
			return 0;
		if (!DEM_PUSH_STR(ddata, "::"))
			return 0;
		if (!demangle_push_str(ddata, ddata->last_sname, len))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('D', '0'):
	case SIMPLE_HASH('D', '1'):
	case SIMPLE_HASH('D', '2'):
	case SIMPLE_HASH('D', '3'):
	case SIMPLE_HASH('D', '4'):
	case SIMPLE_HASH('D', '5'):
		if (!ddata->last_sname)
			return 0;
		if (!(len = demangle_strlen(ddata->last_sname)))
			return 0;
		if (!DEM_PUSH_STR(ddata, "::~"))
			return 0;
		if (!demangle_push_str(ddata, ddata->last_sname, len))
			return 0;
		ddata->cur += 2;
		return 1;
	}

	/* source name */
	if (ELFTC_ISDIGIT(*ddata->cur))
		return demangle_read_sname(ddata);

	/* local source name */
	if (*ddata->cur == 'L')
		return demangle_local_source_name(ddata);

	return 1;
}

/*
 * Read template parameter that forms in 'T[number]_'.
 * This function much like to read_subst but only for types.
 */
ABI static int
demangle_read_tmpl_param(struct demangle_data *ddata)
{
	long nth;

	if (*ddata->cur != 'T')
		return 0;

	++ddata->cur;

	if (*ddata->cur == '_') {
		return demangle_get_tmpl_param(ddata, 0);
	} else {
		nth = demangle_strtol(ddata->cur, 36);

		/* T_ is first */
		++nth;

		while (*ddata->cur && *ddata->cur != '_')
			++ddata->cur;

		if (nth <= 0)
			return 0;

		return demangle_get_tmpl_param(ddata, nth);
	}

	/* NOTREACHED */
	return 0;
}

ABI static int
demangle_vector_read_cmd_push(struct demangle_data *ddata,
    struct vector_read_cmd *v, enum read_cmd cmd, void *data)
{
	struct read_cmd_item *tmp_r_ctn;
	size_t tmp_cap;
	size_t i;

	if (!v)
		return 0;

	if (v->size == v->capacity) {
		tmp_cap = BUFFER_GROW(v->capacity);
		tmp_r_ctn = (struct read_cmd_item *)demangle_malloc(ddata,
		    alignof(*tmp_r_ctn), sizeof(*tmp_r_ctn) * tmp_cap);
		for (i = 0; i < v->size; ++i)
			tmp_r_ctn[i] = v->r_container[i];
		demangle_free(ddata, v->r_container);
		v->r_container = tmp_r_ctn;
		v->capacity = tmp_cap;
	}

	v->r_container[v->size].cmd = cmd;
	v->r_container[v->size].data = data;
	++v->size;

	return 1;
}

ABI static int
demangle_read_tmpl_arg(struct demangle_data *ddata)
{
	if (*ddata->cur == '\0')
		return 0;

	switch (*ddata->cur) {
	case 'L':
		return demangle_read_expr_primary(ddata);
	case 'X':
		++ddata->cur;
		if (!demangle_read_expression(ddata))
			return 0;
		return *ddata->cur++ == 'E';
	}

	return demangle_read_type(ddata, NULL);
}

ABI static int
demangle_read_tmpl_args(struct demangle_data *ddata)
{
	struct vector_str *v;
	size_t arg_len, idx, limit;
	char *arg;

	if (*ddata->cur == '\0')
		return 0;

	++ddata->cur;

	if (!demangle_vector_read_cmd_push(ddata, &ddata->cmd, READ_TMPL, NULL))
		return 0;

	if (!DEM_PUSH_STR(ddata, "<"))
		return 0;

	limit = 0;
	v = ddata->cur_output;
	for (;;) {
		idx = v->size;
		if (!demangle_read_tmpl_arg(ddata))
			return 0;
		if (!(arg = demangle_vector_str_substr(ddata, v, idx,
			  v->size - 1, &arg_len)))
			return 0;
		if (!demangle_vector_str_find(ddata, &ddata->tmpl, arg,
			arg_len) &&
		    !demangle_vector_str_push(ddata, &ddata->tmpl, arg,
			arg_len)) {
			demangle_free(ddata, arg);
			return 0;
		}

		demangle_free(ddata, arg);

		if (*ddata->cur == 'E') {
			++ddata->cur;
			if (!DEM_PUSH_STR(ddata, ">"))
				return 0;
			ddata->is_tmpl = true;
			break;
		} else if (*ddata->cur != 'I' && !DEM_PUSH_STR(ddata, ", "))
			return 0;

		if (limit++ > DEMANGLE_TRY_LIMIT)
			return 0;
	}

	return demangle_vector_read_cmd_pop(&ddata->cmd);
}

ABI static int
demangle_read_expression_trinary(struct demangle_data *ddata, const char *name1,
    size_t len1, const char *name2, size_t len2)
{
	if (!name1 || !len1 || !name2 || !len2)
		return 0;

	if (!demangle_read_expression(ddata))
		return 0;
	if (!demangle_push_str(ddata, name1, len1))
		return 0;
	if (!demangle_read_expression(ddata))
		return 0;
	if (!demangle_push_str(ddata, name2, len2))
		return 0;

	return demangle_read_expression(ddata);
}

ABI static int
demangle_read_expression_unary(struct demangle_data *ddata, const char *name,
    size_t len)
{
	if (!name || !len)
		return 0;
	if (!demangle_read_expression(ddata))
		return 0;

	return demangle_push_str(ddata, name, len);
}

ABI static int
demangle_read_expression_binary(struct demangle_data *ddata, const char *name,
    size_t len)
{
	if (!name || !len)
		return 0;
	if (!demangle_read_expression(ddata))
		return 0;
	if (!demangle_push_str(ddata, name, len))
		return 0;

	return demangle_read_expression(ddata);
}

ABI static int
demangle_read_expression_impl(struct demangle_data *ddata)
{
	if (*ddata->cur == '\0')
		return 0;

	switch (SIMPLE_HASH(*ddata->cur, *(ddata->cur + 1))) {
	case SIMPLE_HASH('s', 't'):
		ddata->cur += 2;
		return demangle_read_type(ddata, NULL);

	case SIMPLE_HASH('s', 'r'):
		ddata->cur += 2;
		if (!demangle_read_type(ddata, NULL))
			return 0;
		if (!demangle_read_uqname(ddata))
			return 0;
		if (*ddata->cur == 'I')
			return demangle_read_tmpl_args(ddata);
		return 1;

	case SIMPLE_HASH('a', 'a'):
		/* operator && */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, "&&", 2);

	case SIMPLE_HASH('a', 'd'):
		/* operator & (unary) */
		ddata->cur += 2;
		return demangle_read_expression_unary(ddata, "&", 1);

	case SIMPLE_HASH('a', 'n'):
		/* operator & */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, "&", 1);

	case SIMPLE_HASH('a', 'N'):
		/* operator &= */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, "&=", 2);

	case SIMPLE_HASH('a', 'S'):
		/* operator = */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, "=", 1);

	case SIMPLE_HASH('c', 'l'):
		/* operator () */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, "()", 2);

	case SIMPLE_HASH('c', 'm'):
		/* operator , */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, ",", 1);

	case SIMPLE_HASH('c', 'o'):
		/* operator ~ */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, "~", 1);

	case SIMPLE_HASH('c', 'v'):
		/* operator (cast) */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, "(cast)", 6);

	case SIMPLE_HASH('d', 'a'):
		/* operator delete [] */
		ddata->cur += 2;
		return demangle_read_expression_unary(ddata, "delete []", 9);

	case SIMPLE_HASH('d', 'e'):
		/* operator * (unary) */
		ddata->cur += 2;
		return demangle_read_expression_unary(ddata, "*", 1);

	case SIMPLE_HASH('d', 'l'):
		/* operator delete */
		ddata->cur += 2;
		return demangle_read_expression_unary(ddata, "delete", 6);

	case SIMPLE_HASH('d', 'v'):
		/* operator / */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, "/", 1);

	case SIMPLE_HASH('d', 'V'):
		/* operator /= */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, "/=", 2);

	case SIMPLE_HASH('e', 'o'):
		/* operator ^ */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, "^", 1);

	case SIMPLE_HASH('e', 'O'):
		/* operator ^= */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, "^=", 2);

	case SIMPLE_HASH('e', 'q'):
		/* operator == */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, "==", 2);

	case SIMPLE_HASH('g', 'e'):
		/* operator >= */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, ">=", 2);

	case SIMPLE_HASH('g', 't'):
		/* operator > */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, ">", 1);

	case SIMPLE_HASH('i', 'x'):
		/* operator [] */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, "[]", 2);

	case SIMPLE_HASH('l', 'e'):
		/* operator <= */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, "<=", 2);

	case SIMPLE_HASH('l', 's'):
		/* operator << */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, "<<", 2);

	case SIMPLE_HASH('l', 'S'):
		/* operator <<= */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, "<<=", 3);

	case SIMPLE_HASH('l', 't'):
		/* operator < */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, "<", 1);

	case SIMPLE_HASH('m', 'i'):
		/* operator - */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, "-", 1);

	case SIMPLE_HASH('m', 'I'):
		/* operator -= */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, "-=", 2);

	case SIMPLE_HASH('m', 'l'):
		/* operator * */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, "*", 1);

	case SIMPLE_HASH('m', 'L'):
		/* operator *= */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, "*=", 2);

	case SIMPLE_HASH('m', 'm'):
		/* operator -- */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, "--", 2);

	case SIMPLE_HASH('n', 'a'):
		/* operator new[] */
		ddata->cur += 2;
		return demangle_read_expression_unary(ddata, "new []", 6);

	case SIMPLE_HASH('n', 'e'):
		/* operator != */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, "!=", 2);

	case SIMPLE_HASH('n', 'g'):
		/* operator - (unary) */
		ddata->cur += 2;
		return demangle_read_expression_unary(ddata, "-", 1);

	case SIMPLE_HASH('n', 't'):
		/* operator ! */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, "!", 1);

	case SIMPLE_HASH('n', 'w'):
		/* operator new */
		ddata->cur += 2;
		return demangle_read_expression_unary(ddata, "new", 3);

	case SIMPLE_HASH('o', 'o'):
		/* operator || */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, "||", 2);

	case SIMPLE_HASH('o', 'r'):
		/* operator | */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, "|", 1);

	case SIMPLE_HASH('o', 'R'):
		/* operator |= */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, "|=", 2);

	case SIMPLE_HASH('p', 'l'):
		/* operator + */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, "+", 1);

	case SIMPLE_HASH('p', 'L'):
		/* operator += */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, "+=", 2);

	case SIMPLE_HASH('p', 'm'):
		/* operator ->* */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, "->*", 3);

	case SIMPLE_HASH('p', 'p'):
		/* operator ++ */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, "++", 2);

	case SIMPLE_HASH('p', 's'):
		/* operator + (unary) */
		ddata->cur += 2;
		return demangle_read_expression_unary(ddata, "+", 1);

	case SIMPLE_HASH('p', 't'):
		/* operator -> */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, "->", 2);

	case SIMPLE_HASH('q', 'u'):
		/* operator ? */
		ddata->cur += 2;
		return demangle_read_expression_trinary(ddata, "?", 1, ":", 1);

	case SIMPLE_HASH('r', 'm'):
		/* operator % */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, "%", 1);

	case SIMPLE_HASH('r', 'M'):
		/* operator %= */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, "%=", 2);

	case SIMPLE_HASH('r', 's'):
		/* operator >> */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, ">>", 2);

	case SIMPLE_HASH('r', 'S'):
		/* operator >>= */
		ddata->cur += 2;
		return demangle_read_expression_binary(ddata, ">>=", 3);

	case SIMPLE_HASH('r', 'z'):
		/* operator sizeof */
		ddata->cur += 2;
		return demangle_read_expression_unary(ddata, "sizeof", 6);

	case SIMPLE_HASH('s', 'v'):
		/* operator sizeof */
		ddata->cur += 2;
		return demangle_read_expression_unary(ddata, "sizeof", 6);
	}

	switch (*ddata->cur) {
	case 'L':
		return demangle_read_expr_primary(ddata);
	case 'T':
		return demangle_read_tmpl_param(ddata);
	}

	return 0;
}

ABI static int
demangle_read_expression(struct demangle_data *ddata)
{
	if (ddata->depth == MAX_DEPTH)
		__builtin_longjmp(ddata->jmpbuf, 1);
	++ddata->depth;
	int res = demangle_read_expression_impl(ddata);
	--ddata->depth;
	return res;
}

ABI static int
demangle_read_expression_flat(struct demangle_data *ddata, char **str)
{
	struct vector_str *output;
	size_t i, p_idx, idx, exp_len;
	char *exp;

	output = &ddata->output;

	p_idx = output->size;

	if (!demangle_read_expression(ddata))
		return 0;

	if (!(exp = demangle_vector_str_substr(ddata, output, p_idx,
		  output->size - 1, &exp_len)))
		return 0;

	idx = output->size;
	for (i = p_idx; i < idx; ++i)
		if (!demangle_vector_str_pop(output))
			return 0;

	*str = exp;

	return 1;
}

/* size, capacity, ext_name */
ABI static void
demangle_vector_type_qualifier_init(struct demangle_data *ddata,
    struct vector_type_qualifier *v)
{
	v->size = 0;
	v->capacity = VECTOR_DEF_CAPACITY;

	v->q_container = (enum type_qualifier *)demangle_malloc(ddata,
	    alignof(enum type_qualifier),
	    sizeof(enum type_qualifier) * v->capacity);

	ASSERT(v->q_container);

	demangle_vector_str_init(ddata, &v->ext_name);
}

ABI static struct read_cmd_item *
demangle_vector_read_cmd_find(struct vector_read_cmd *v, enum read_cmd dst)
{
	int i;

	if (!v || dst == READ_FAIL)
		return 0;

	for (i = (int)v->size - 1; i >= 0; i--)
		if (v->r_container[i].cmd == dst)
			return &v->r_container[i];

	return 0;
}

ABI static int
demangle_read_function(struct demangle_data *ddata, int *ext_c,
    struct vector_type_qualifier *v)
{
	struct type_delimit td;
	struct read_cmd_item *rc;
	size_t class_type_size, class_type_len, limit;
	const char *class_type;
	int i;
	bool paren, non_cv_qualifier;

	if (*ddata->cur != 'F')
		return 0;

	++ddata->cur;
	if (*ddata->cur == 'Y') {
		if (ext_c)
			*ext_c = 1;
		++ddata->cur;
	}

	/* Return type */
	if (!demangle_read_type(ddata, NULL))
		return 0;

	if (*ddata->cur != 'E') {
		if (!DEM_PUSH_STR(ddata, " "))
			return 0;

		non_cv_qualifier = false;
		if (v->size > 0) {
			for (i = 0; (size_t)i < v->size; i++) {
				if (v->q_container[i] != TYPE_RST &&
				    v->q_container[i] != TYPE_VAT &&
				    v->q_container[i] != TYPE_CST) {
					non_cv_qualifier = true;
					break;
				}
			}
		}

		paren = false;
		rc = demangle_vector_read_cmd_find(&ddata->cmd, READ_PTRMEM);
		if (non_cv_qualifier || rc != NULL) {
			if (!DEM_PUSH_STR(ddata, "("))
				return 0;
			paren = true;
		}

		/* Push non-cv qualifiers. */
		ddata->push_qualifier = PUSH_NON_CV_QUALIFIER;
		if (!demangle_push_type_qualifier(ddata, v, NULL))
			return 0;

		if (rc) {
			if (non_cv_qualifier && !DEM_PUSH_STR(ddata, " "))
				return 0;
			if (!(class_type_size = ddata->class_type.size))
				return 0;
			class_type = VEC_STR(ddata, &ddata->class_type,
			    class_type_size - 1);
			if (!class_type)
				return 0;
			if (!(class_type_len = demangle_strlen(class_type)))
				return 0;
			if (!demangle_push_str(ddata, class_type,
				class_type_len))
				return 0;
			if (!DEM_PUSH_STR(ddata, "::*"))
				return 0;
			/* Push pointer-to-member qualifiers. */
			ddata->push_qualifier = PUSH_ALL_QUALIFIER;
			if (!demangle_push_type_qualifier(ddata,
				(struct vector_type_qualifier *)rc->data, NULL))
				return 0;
			++ddata->func_type;
		}

		if (paren) {
			if (!DEM_PUSH_STR(ddata, ")"))
				return 0;
			paren = false;
		}

		td.paren = false;
		td.firstp = true;
		limit = 0;
		ddata->is_functype = true;
		for (;;) {
			if (!demangle_read_type(ddata, &td))
				return 0;
			if (*ddata->cur == 'E')
				break;
			if (limit++ > DEMANGLE_TRY_LIMIT)
				return 0;
		}
		ddata->is_functype = false;
		if (td.paren) {
			if (!DEM_PUSH_STR(ddata, ")"))
				return 0;
			td.paren = false;
		}

		/* Push CV qualifiers. */
		ddata->push_qualifier = PUSH_CV_QUALIFIER;
		if (!demangle_push_type_qualifier(ddata, v, NULL))
			return 0;

		ddata->push_qualifier = PUSH_ALL_QUALIFIER;

		/* Release type qualifier vector. */
		demangle_vector_type_qualifier_init(ddata, v);

		/* Push ref-qualifiers. */
		if (ddata->ref_qualifier) {
			switch (ddata->ref_qualifier_type) {
			case TYPE_REF:
				if (!DEM_PUSH_STR(ddata, " &"))
					return 0;
				break;
			case TYPE_RREF:
				if (!DEM_PUSH_STR(ddata, " &&"))
					return 0;
				break;
			default:
				return 0;
			}
			ddata->ref_qualifier = false;
		}
	}

	++ddata->cur;

	return 1;
}

ABI static int
demangle_read_offset_number(struct demangle_data *ddata)
{
	bool negative;
	const char *start;

	if (*ddata->cur == '\0')
		return 0;

	/* offset could be negative */
	if (*ddata->cur == 'n') {
		negative = true;
		start = ddata->cur + 1;
	} else {
		negative = false;
		start = ddata->cur;
	}

	while (*ddata->cur && *ddata->cur != '_')
		++ddata->cur;

	if (negative && !DEM_PUSH_STR(ddata, "-"))
		return 0;

	ASSERT(start);

	if (!demangle_push_str(ddata, start, ddata->cur - start))
		return 0;
	if (!DEM_PUSH_STR(ddata, " "))
		return 0;

	++ddata->cur;

	return 1;
}

ABI static int
demangle_read_nv_offset(struct demangle_data *ddata)
{
	if (!DEM_PUSH_STR(ddata, "offset : "))
		return 0;

	return demangle_read_offset_number(ddata);
}

ABI static int
demangle_read_v_offset(struct demangle_data *ddata)
{
	if (!DEM_PUSH_STR(ddata, "offset : "))
		return 0;

	if (!demangle_read_offset_number(ddata))
		return 0;

	if (!DEM_PUSH_STR(ddata, "virtual offset : "))
		return 0;

	return !demangle_read_offset_number(ddata);
}

/* read offset, offset are nv-offset, v-offset */
ABI static int
demangle_read_offset(struct demangle_data *ddata)
{
	if (*ddata->cur == 'h') {
		++ddata->cur;
		return demangle_read_nv_offset(ddata);
	} else if (*ddata->cur == 'v') {
		++ddata->cur;
		return demangle_read_v_offset(ddata);
	}

	return 0;
}

ABI static int
demangle_read_type_flat(struct demangle_data *ddata, char **str)
{
	struct vector_str *output;
	size_t i, p_idx, idx, type_len;
	char *type;

	output = ddata->cur_output;

	p_idx = output->size;

	if (!demangle_read_type(ddata, NULL))
		return 0;

	if (!(type = demangle_vector_str_substr(ddata, output, p_idx,
		  output->size - 1, &type_len)))
		return 0;

	idx = output->size;
	for (i = p_idx; i < idx; ++i)
		if (!demangle_vector_str_pop(output))
			return 0;

	*str = type;

	return 1;
}

/*
 * read number
 * number ::= [n] <decimal>
 */
ABI static int
demangle_read_number(struct demangle_data *ddata, long *rtn)
{
	long len, negative_factor;

	if (!rtn)
		return 0;

	negative_factor = 1;
	if (*ddata->cur == 'n') {
		negative_factor = -1;

		++ddata->cur;
	}
	if (!ELFTC_ISDIGIT(*ddata->cur))
		return 0;

	len = demangle_strtol(ddata->cur, 10);
	if (len < 0)
		__builtin_longjmp(ddata->jmpbuf, 1);

	while (ELFTC_ISDIGIT(*ddata->cur))
		++ddata->cur;

	*rtn = len * negative_factor;

	return 1;
}

ABI static int
demangle_read_number_as_string(struct demangle_data *ddata, char **str)
{
	long n;
	char buf[21];

	if (!demangle_read_number(ddata, &n)) {
		*str = NULL;
		return 0;
	}

	demangle_itoa(buf, n);
	*str = demangle_strdup(ddata, buf);

	return 1;
}

ABI static int
demangle_read_encoding_impl(struct demangle_data *ddata)
{
	char *name, *type, *num_str;
	long offset;
	int rtn;

	if (*ddata->cur == '\0')
		return 0;

	/* special name */
	switch (SIMPLE_HASH(*ddata->cur, *(ddata->cur + 1))) {
	case SIMPLE_HASH('G', 'A'):
		if (!DEM_PUSH_STR(ddata, "hidden alias for "))
			return 0;
		ddata->cur += 2;
		if (*ddata->cur == '\0')
			return 0;
		return demangle_read_encoding(ddata);

	case SIMPLE_HASH('G', 'R'):
		if (!DEM_PUSH_STR(ddata, "reference temporary #"))
			return 0;
		ddata->cur += 2;
		if (*ddata->cur == '\0')
			return 0;
		if (!demangle_read_name_flat(ddata, &name))
			return 0;
		rtn = 0;
		if (!demangle_read_number_as_string(ddata, &num_str))
			goto clean1;
		if (!DEM_PUSH_STR(ddata, num_str))
			goto clean2;
		if (!DEM_PUSH_STR(ddata, " for "))
			goto clean2;
		if (!DEM_PUSH_STR(ddata, name))
			goto clean2;
		rtn = 1;
	clean2:
		demangle_free(ddata, num_str);
	clean1:
		demangle_free(ddata, name);
		return rtn;

	case SIMPLE_HASH('G', 'T'):
		ddata->cur += 2;
		if (*ddata->cur == '\0')
			return 0;
		switch (*ddata->cur) {
		case 'n':
			if (!DEM_PUSH_STR(ddata, "non-transaction clone for "))
				return 0;
			break;
		case 't':
		default:
			if (!DEM_PUSH_STR(ddata, "transaction clone for "))
				return 0;
			break;
		}
		++ddata->cur;
		return demangle_read_encoding(ddata);

	case SIMPLE_HASH('G', 'V'):
		/* sentry object for 1 time init */
		if (!DEM_PUSH_STR(ddata, "guard variable for "))
			return 0;
		ddata->is_guard_variable = true;
		ddata->cur += 2;
		break;

	case SIMPLE_HASH('T', 'c'):
		/* virtual function covariant override thunk */
		if (!DEM_PUSH_STR(ddata,
			"virtual function covariant override "))
			return 0;
		ddata->cur += 2;
		if (*ddata->cur == '\0')
			return 0;
		if (!demangle_read_offset(ddata))
			return 0;
		if (!demangle_read_offset(ddata))
			return 0;
		return demangle_read_encoding(ddata);

	case SIMPLE_HASH('T', 'C'):
		/* construction vtable */
		if (!DEM_PUSH_STR(ddata, "construction vtable for "))
			return 0;
		ddata->cur += 2;
		if (*ddata->cur == '\0')
			return 0;
		if (!demangle_read_type_flat(ddata, &type))
			return 0;
		if (!demangle_read_number(ddata, &offset))
			return 0;
		if (*ddata->cur++ != '_')
			return 0;
		if (!demangle_read_type(ddata, NULL))
			return 0;
		if (!DEM_PUSH_STR(ddata, "-in-"))
			return 0;
		if (!DEM_PUSH_STR(ddata, type))
			return 0;
		return 1;

	case SIMPLE_HASH('T', 'D'):
		/* typeinfo common proxy */
		break;

	case SIMPLE_HASH('T', 'F'):
		/* typeinfo fn */
		if (!DEM_PUSH_STR(ddata, "typeinfo fn for "))
			return 0;
		ddata->cur += 2;
		if (*ddata->cur == '\0')
			return 0;
		return demangle_read_type(ddata, NULL);

	case SIMPLE_HASH('T', 'h'):
		/* virtual function non-virtual override thunk */
		if (!DEM_PUSH_STR(ddata,
			"virtual function non-virtual override "))
			return 0;
		ddata->cur += 2;
		if (*ddata->cur == '\0')
			return 0;
		if (!demangle_read_nv_offset(ddata))
			return 0;
		return demangle_read_encoding(ddata);

	case SIMPLE_HASH('T', 'H'):
		/* TLS init function */
		if (!DEM_PUSH_STR(ddata, "TLS init function for "))
			return 0;
		ddata->cur += 2;
		if (*ddata->cur == '\0')
			return 0;
		break;

	case SIMPLE_HASH('T', 'I'):
		/* typeinfo structure */
		if (!DEM_PUSH_STR(ddata, "typeinfo for "))
			return 0;
		ddata->cur += 2;
		if (*ddata->cur == '\0')
			return 0;
		return demangle_read_type(ddata, NULL);

	case SIMPLE_HASH('T', 'J'):
		/* java class */
		if (!DEM_PUSH_STR(ddata, "java Class for "))
			return 0;
		ddata->cur += 2;
		if (*ddata->cur == '\0')
			return 0;
		return demangle_read_type(ddata, NULL);

	case SIMPLE_HASH('T', 'S'):
		/* RTTI name (NTBS) */
		if (!DEM_PUSH_STR(ddata, "typeinfo name for "))
			return 0;
		ddata->cur += 2;
		if (*ddata->cur == '\0')
			return 0;
		return demangle_read_type(ddata, NULL);

	case SIMPLE_HASH('T', 'T'):
		/* VTT table */
		if (!DEM_PUSH_STR(ddata, "VTT for "))
			return 0;
		ddata->cur += 2;
		if (*ddata->cur == '\0')
			return 0;
		return demangle_read_type(ddata, NULL);

	case SIMPLE_HASH('T', 'v'):
		/* virtual function virtual override thunk */
		if (!DEM_PUSH_STR(ddata, "virtual function virtual override "))
			return 0;
		ddata->cur += 2;
		if (*ddata->cur == '\0')
			return 0;
		if (!demangle_read_v_offset(ddata))
			return 0;
		return demangle_read_encoding(ddata);

	case SIMPLE_HASH('T', 'V'):
		/* virtual table */
		if (!DEM_PUSH_STR(ddata, "vtable for "))
			return 0;
		ddata->cur += 2;
		if (*ddata->cur == '\0')
			return 0;
		return demangle_read_type(ddata, NULL);

	case SIMPLE_HASH('T', 'W'):
		/* TLS wrapper function */
		if (!DEM_PUSH_STR(ddata, "TLS wrapper function for "))
			return 0;
		ddata->cur += 2;
		if (*ddata->cur == '\0')
			return 0;
		break;
	}

	return demangle_read_name(ddata);
}

/* read encoding, encoding are function name, data name, special-name */
ABI static int
demangle_read_encoding(struct demangle_data *ddata)
{
	if (ddata->depth == MAX_DEPTH)
		__builtin_longjmp(ddata->jmpbuf, 1);
	++ddata->depth;
	int res = demangle_read_encoding_impl(ddata);
	--ddata->depth;
	return res;
}

ABI static int
demangle_read_local_name(struct demangle_data *ddata)
{
	struct vector_str local_name;
	struct type_delimit td;
	size_t limit;
	bool more_type;

	if (*(++ddata->cur) == '\0')
		return 0;

	demangle_vector_str_init(ddata, &local_name);
	ddata->cur_output = &local_name;

	if (!demangle_read_encoding(ddata))
		return 0;

	ddata->cur_output = &ddata->output;

	td.paren = false;
	td.firstp = true;
	more_type = false;
	limit = 0;

	/*
	 * The first type is a return type if we just demangled template
	 * args. (the template args is right next to the function name,
	 * which means it's a template function)
	 */
	if (ddata->is_tmpl) {
		ddata->is_tmpl = false;

		/* Read return type */
		if (!demangle_read_type(ddata, NULL))
			return 0;

		more_type = true;
	}

	/* Now we can push the name after possible return type is handled. */
	if (!demangle_vector_str_push_vector(ddata, &ddata->output,
		&local_name))
		return 0;

	while (*ddata->cur != '\0') {
		if (!demangle_read_type(ddata, &td))
			return 0;
		if (more_type)
			more_type = false;
		if (*ddata->cur == 'E')
			break;
		if (limit++ > DEMANGLE_TRY_LIMIT)
			return 0;
	}
	if (more_type)
		return 0;

	if (*(++ddata->cur) == '\0')
		return 0;
	if (td.paren) {
		if (!DEM_PUSH_STR(ddata, ")"))
			return 0;
		td.paren = false;
	}
	if (*ddata->cur == 's')
		++ddata->cur;
	else {
		if (!DEM_PUSH_STR(ddata, "::"))
			return 0;
		if (!demangle_read_name(ddata))
			return 0;
	}
	if (*ddata->cur == '_') {
		++ddata->cur;
		while (ELFTC_ISDIGIT(*ddata->cur))
			++ddata->cur;
	}

	return 1;
}

ABI static int
demangle_read_nested_name(struct demangle_data *ddata)
{
	struct stack_str v;
	struct vector_str *output;
	size_t limit, p_idx, subst_str_len;
	char *subst_str;

	if (*ddata->cur != 'N')
		return 0;
	if (*(++ddata->cur) == '\0')
		return 0;

	do {
		switch (*ddata->cur) {
		case 'r':
			ddata->mem_rst = true;
			break;
		case 'V':
			ddata->mem_vat = true;
			break;
		case 'K':
			ddata->mem_cst = true;
			break;
		case 'R':
			ddata->mem_ref = true;
			break;
		case 'O':
			ddata->mem_rref = true;
			break;
		default:
			goto next;
		}
	} while (*(++ddata->cur));

next:
	output = ddata->cur_output;
	demangle_stack_str_init(&v);

	limit = 0;
	for (;;) {
		p_idx = output->size;
		switch (*ddata->cur) {
		case 'I':
			if (!demangle_read_tmpl_args(ddata))
				return 0;
			break;
		case 'S':
			if (!demangle_read_subst(ddata))
				return 0;
			break;
		case 'T':
			if (!demangle_read_tmpl_param(ddata))
				return 0;
			break;
		default:
			if (!demangle_read_uqname(ddata))
				return 0;
		}

		if (p_idx == output->size)
			goto next_comp;
		if (!(subst_str = demangle_vector_str_substr(ddata, output,
			  p_idx, output->size - 1, &subst_str_len)))
			return 0;
		demangle_stack_str_append(ddata, &v, subst_str, subst_str_len);
		demangle_free(ddata, subst_str);

		if (!demangle_push_subst(ddata, v.str, v.len))
			return 0;

	next_comp:
		if (*ddata->cur == 'E')
			break;
		else if (*ddata->cur != 'I' && *ddata->cur != 'C' &&
		    *ddata->cur != 'D' && p_idx != output->size) {
			if (!DEM_PUSH_STR(ddata, "::"))
				return 0;
			demangle_stack_str_append_str(ddata, &v, "::");
		}
		if (limit++ > DEMANGLE_TRY_LIMIT)
			return 0;
	}

	++ddata->cur;
	return 1;
}

ABI static int
demangle_read_name_impl(struct demangle_data *ddata)
{
	struct stack_str v;
	struct vector_str *output;
	size_t p_idx, subst_str_len;
	char *subst_str;
	int rtn;

	if (*ddata->cur == '\0')
		return 0;

	output = ddata->cur_output;

	subst_str = NULL;

	switch (*ddata->cur) {
	case 'S':
		return demangle_read_subst(ddata);
	case 'N':
		return demangle_read_nested_name(ddata);
	case 'Z':
		return demangle_read_local_name(ddata);
	}

	demangle_stack_str_init(&v);

	p_idx = output->size;
	rtn = 0;
	if (!demangle_read_uqname(ddata))
		goto clean;
	if (!(subst_str = demangle_vector_str_substr(ddata, output, p_idx,
		  output->size - 1, &subst_str_len)))
		goto clean;
	if (subst_str_len > 8 && demangle_strstr(subst_str, "operator")) {
		rtn = 1;
		goto clean;
	}
	demangle_stack_str_append(ddata, &v, subst_str, subst_str_len);
	if (!demangle_push_subst(ddata, v.str, v.len))
		goto clean;

	if (*ddata->cur == 'I') {
		p_idx = output->size;
		if (!demangle_read_tmpl_args(ddata))
			goto clean;
		if (!(subst_str = demangle_vector_str_substr(ddata, output,
			  p_idx, output->size - 1, &subst_str_len)))
			goto clean;
		demangle_stack_str_append(ddata, &v, subst_str, subst_str_len);
		if (!demangle_push_subst(ddata, v.str, v.len))
			goto clean;
	}

	rtn = 1;

clean:
	demangle_free(ddata, subst_str);

	return rtn;
}

ABI static int
demangle_read_name(struct demangle_data *ddata)
{
	if (ddata->depth == MAX_DEPTH)
		__builtin_longjmp(ddata->jmpbuf, 1);
	++ddata->depth;
	int res = demangle_read_name_impl(ddata);
	--ddata->depth;
	return res;
}

ABI static int
demangle_read_name_flat(struct demangle_data *ddata, char **str)
{
	struct vector_str *output;
	size_t i, p_idx, idx, name_len;
	char *name;

	output = ddata->cur_output;

	p_idx = output->size;

	if (!demangle_read_name(ddata))
		return 0;

	if (!(name = demangle_vector_str_substr(ddata, output, p_idx,
		  output->size - 1, &name_len)))
		return 0;

	idx = output->size;
	for (i = p_idx; i < idx; ++i)
		if (!demangle_vector_str_pop(output))
			return 0;

	*str = name;

	return 1;
}

ABI static int
demangle_read_pointer_to_member(struct demangle_data *ddata,
    struct vector_type_qualifier *v)
{
	size_t class_type_len, i, idx, p_idx;
	int p_func_type, rtn;
	char *class_type;

	if (*ddata->cur != 'M' || *(++ddata->cur) == '\0')
		return 0;

	p_idx = ddata->output.size;
	if (!demangle_read_type(ddata, NULL))
		return 0;

	if (!(class_type = demangle_vector_str_substr(ddata, &ddata->output,
		  p_idx, ddata->output.size - 1, &class_type_len)))
		return 0;

	rtn = 0;
	idx = ddata->output.size;
	for (i = p_idx; i < idx; ++i)
		if (!demangle_vector_str_pop(&ddata->output))
			goto clean1;

	if (!demangle_vector_read_cmd_push(ddata, &ddata->cmd, READ_PTRMEM, v))
		goto clean1;

	if (!demangle_vector_str_push(ddata, &ddata->class_type, class_type,
		class_type_len))
		goto clean2;

	p_func_type = ddata->func_type;
	if (!demangle_read_type(ddata, NULL))
		goto clean3;

	if (p_func_type == ddata->func_type) {
		if (!DEM_PUSH_STR(ddata, " "))
			goto clean3;
		if (!demangle_push_str(ddata, class_type, class_type_len))
			goto clean3;
		if (!DEM_PUSH_STR(ddata, "::*"))
			goto clean3;
	}

	rtn = 1;
clean3:
	if (!demangle_vector_str_pop(&ddata->class_type))
		rtn = 0;
clean2:
	if (!demangle_vector_read_cmd_pop(&ddata->cmd))
		rtn = 0;
clean1:

	demangle_vector_type_qualifier_init(ddata, v);

	return rtn;
}

/* read source-name, source-name is <len> <ID> */
ABI static int
demangle_read_sname(struct demangle_data *ddata)
{
	size_t lim;
	long len;
	int err;

	if (!demangle_read_number(ddata, &len) || len <= 0)
		return 0;

	if (len == 12 && !demangle_memcmp("_GLOBAL__N_1", ddata->cur, 12))
		err = DEM_PUSH_STR(ddata, "(anonymous namespace)");
	else
		err = demangle_push_str(ddata, ddata->cur, len);

	if (!err)
		return 0;

	ASSERT(ddata->cur_output->size > 0);
	if (!demangle_vector_read_cmd_find(&ddata->cmd, READ_TMPL))
		ddata->last_sname = VEC_STR(ddata, ddata->cur_output,
		    ddata->cur_output->size - 1);

	lim = demangle_strlen(ddata->cur);
	if (len > lim)
		len = lim;
	ddata->cur += len;

	return 1;
}

ABI static int
demangle_read_subst_stdtmpl(struct demangle_data *ddata, const char *str)
{
	struct vector_str *output;
	size_t p_idx, substr_len, len;
	char *subst_str, *substr;

	if (!str)
		return 0;

	if (!(len = demangle_strlen(str)))
		return 0;

	output = ddata->cur_output;

	p_idx = output->size;
	substr = NULL;
	subst_str = NULL;

	if (!demangle_read_tmpl_args(ddata))
		return 0;
	if (!(substr = demangle_vector_str_substr(ddata, output, p_idx,
		  output->size - 1, &substr_len)))
		return 0;

	subst_str = (char *)demangle_malloc(ddata, alignof(char),
	    sizeof(char) * (substr_len + len + 1));

	demangle_memcpy(subst_str, str, len);
	demangle_memcpy(subst_str + len, substr, substr_len);
	subst_str[substr_len + len] = '\0';

	if (!demangle_push_subst(ddata, subst_str, substr_len + len))
		return 0;

	return 1;
}

ABI static int
demangle_read_subst_std(struct demangle_data *ddata)
{
	struct vector_str *output, v;
	size_t p_idx, subst_str_len;
	char *subst_str;

	demangle_vector_str_init(ddata, &v);

	subst_str = NULL;
	if (!DEM_PUSH_STR(ddata, "std::"))
		return 0;

	if (!VEC_PUSH_STR(&v, "std::"))
		return 0;

	ddata->cur += 2;

	output = ddata->cur_output;

	p_idx = output->size;
	if (!demangle_read_uqname(ddata))
		return 0;

	if (!(subst_str = demangle_vector_str_substr(ddata, output, p_idx,
		  output->size - 1, &subst_str_len)))
		return 0;

	if (!demangle_vector_str_push(ddata, &v, subst_str, subst_str_len))
		return 0;

	if (!demangle_push_subst_v(ddata, &v))
		return 0;

	if (*ddata->cur == 'I') {
		p_idx = output->size;
		if (!demangle_read_tmpl_args(ddata))
			return 0;
		if (!(subst_str = demangle_vector_str_substr(ddata, output,
			  p_idx, output->size - 1, &subst_str_len)))
			return 0;
		if (!demangle_vector_str_push(ddata, &v, subst_str,
			subst_str_len))
			return 0;
		if (!demangle_push_subst_v(ddata, &v))
			return 0;
	}

	return 1;
}

ABI static int
demangle_read_subst(struct demangle_data *ddata)
{
	long nth;

	if (*ddata->cur == '\0')
		return 0;

	/* abbreviations of the form Sx */
	switch (SIMPLE_HASH(*ddata->cur, *(ddata->cur + 1))) {
	case SIMPLE_HASH('S', 'a'):
		/* std::allocator */
		if (!DEM_PUSH_STR(ddata, "std::allocator"))
			return 0;
		ddata->cur += 2;
		if (*ddata->cur == 'I')
			return demangle_read_subst_stdtmpl(ddata,
			    "std::allocator");
		return 1;

	case SIMPLE_HASH('S', 'b'):
		/* std::basic_string */
		if (!DEM_PUSH_STR(ddata, "std::basic_string"))
			return 0;
		ddata->cur += 2;
		if (*ddata->cur == 'I')
			return demangle_read_subst_stdtmpl(ddata,
			    "std::basic_string");
		return 1;

	case SIMPLE_HASH('S', 'd'):
		/* std::basic_iostream<char, std::char_traits<char>> */
		if (!DEM_PUSH_STR(ddata,
			"std::basic_iostream<char, "
			"std::char_traits<char>>"))
			return 0;
		ddata->last_sname = "basic_iostream";
		ddata->cur += 2;
		if (*ddata->cur == 'I')
			return demangle_read_subst_stdtmpl(ddata,
			    "std::basic_iostream<char, std::char_traits"
			    "<char>>");
		return 1;

	case SIMPLE_HASH('S', 'i'):
		/* std::basic_istream<char, std::char_traits<char>> */
		if (!DEM_PUSH_STR(ddata,
			"std::basic_istream<char, "
			"std::char_traits<char>>"))
			return 0;
		ddata->last_sname = "basic_istream";
		ddata->cur += 2;
		if (*ddata->cur == 'I')
			return demangle_read_subst_stdtmpl(ddata,
			    "std::basic_istream<char, std::char_traits"
			    "<char>>");
		return 1;

	case SIMPLE_HASH('S', 'o'):
		/* std::basic_ostream<char, std::char_traits<char>> */
		if (!DEM_PUSH_STR(ddata,
			"std::basic_ostream<char, "
			"std::char_traits<char>>"))
			return 0;
		ddata->last_sname = "basic_ostream";
		ddata->cur += 2;
		if (*ddata->cur == 'I')
			return demangle_read_subst_stdtmpl(ddata,
			    "std::basic_ostream<char, std::char_traits"
			    "<char>>");
		return 1;

	case SIMPLE_HASH('S', 's'):
		/*
		 * std::string<char, std::char_traits<char>,
		 * std::allocator<char>>
		 *
		 * a.k.a std::string
		 *
		 * [jart] made it std::string for consistency with libcxxabi
		 */
		if (!DEM_PUSH_STR(ddata, "std::string"))
			return 0;
		ddata->last_sname = "string";
		ddata->cur += 2;
		if (*ddata->cur == 'I')
			return demangle_read_subst_stdtmpl(ddata,
			    "std::string");
		return 1;

	case SIMPLE_HASH('S', 't'):
		/* std:: */
		return demangle_read_subst_std(ddata);
	}

	if (*(++ddata->cur) == '\0')
		return 0;

	/* Skip unknown substitution abbreviations. */
	if (!(*ddata->cur >= '0' && *ddata->cur <= '9') &&
	    !(*ddata->cur >= 'A' && *ddata->cur <= 'Z') && *ddata->cur != '_') {
		++ddata->cur;
		return 1;
	}

	/* substitution */
	if (*ddata->cur == '_')
		return demangle_get_subst(ddata, 0);
	else {
		/* substitution number is base 36 */
		nth = demangle_strtol(ddata->cur, 36);

		/* first was '_', so increase one */
		++nth;

		while (*ddata->cur && *ddata->cur != '_')
			++ddata->cur;

		if (nth <= 0)
			return 0;

		return demangle_get_subst(ddata, nth);
	}

	/* NOTREACHED */
	return 0;
}

ABI static int
demangle_vector_type_qualifier_push(struct demangle_data *ddata,
    struct vector_type_qualifier *v, enum type_qualifier t)
{
	enum type_qualifier *tmp_ctn;
	size_t tmp_cap;
	size_t i;

	if (!v)
		return 0;

	if (v->size == v->capacity) {
		tmp_cap = BUFFER_GROW(v->capacity);
		tmp_ctn = (enum type_qualifier *)demangle_malloc(ddata,
		    alignof(enum type_qualifier),
		    sizeof(enum type_qualifier) * tmp_cap);
		for (i = 0; i < v->size; ++i)
			tmp_ctn[i] = v->q_container[i];
		demangle_free(ddata, v->q_container);
		v->q_container = tmp_ctn;
		v->capacity = tmp_cap;
	}

	v->q_container[v->size] = t;
	++v->size;

	return 1;
}

ABI static int
demangle_read_type_impl(struct demangle_data *ddata, struct type_delimit *td)
{
	struct vector_type_qualifier v;
	struct vector_str *output, sv;
	size_t p_idx, type_str_len, subst_str_len;
	int extern_c, is_builtin;
	long len;
	const char *p;
	char *type_str, *exp_str, *num_str, *subst_str;
	bool skip_ref_qualifier, omit_void;

	output = ddata->cur_output;
	if (td) {
		if (td->paren == false) {
			if (!DEM_PUSH_STR(ddata, "("))
				return 0;
			if (ddata->output.size < 2)
				return 0;
			td->paren = true;
		}

		if (!td->firstp) {
			if (*ddata->cur != 'I') {
				if (!DEM_PUSH_STR(ddata, ", "))
					return 0;
			}
		}
	}

	ASSERT(output);
	/*
	 * [r, V, K] [P, R, O, C, G, U] builtin, function, class-enum, array
	 * pointer-to-member, template-param, template-template-param, subst
	 */

	demangle_vector_type_qualifier_init(ddata, &v);

	extern_c = 0;
	is_builtin = 1;
	p_idx = output->size;
	type_str = exp_str = num_str = NULL;
	skip_ref_qualifier = false;

again:

	/* Clear ref-qualifier flag */
	if (*ddata->cur != 'R' && *ddata->cur != 'O' && *ddata->cur != 'E')
		ddata->ref_qualifier = false;

	/* builtin type */
	switch (*ddata->cur) {
	case 'a':
		/* signed char */
		if (!DEM_PUSH_STR(ddata, "signed char"))
			return 0;
		++ddata->cur;
		goto rtn;

	case 'A':
		/* array type */
		if (!demangle_read_array(ddata))
			return 0;
		is_builtin = 0;
		goto rtn;

	case 'b':
		/* bool */
		if (!DEM_PUSH_STR(ddata, "bool"))
			return 0;
		++ddata->cur;
		goto rtn;

	case 'C':
		/* complex pair */
		if (!demangle_vector_type_qualifier_push(ddata, &v, TYPE_CMX))
			return 0;
		++ddata->cur;
		if (td)
			td->firstp = false;
		goto again;

	case 'c':
		/* char */
		if (!DEM_PUSH_STR(ddata, "char"))
			return 0;
		++ddata->cur;
		goto rtn;

	case 'd':
		/* double */
		if (!DEM_PUSH_STR(ddata, "double"))
			return 0;
		++ddata->cur;
		goto rtn;

	case 'D':
		++ddata->cur;
		switch (*ddata->cur) {
		case 'a':
			/* auto */
			if (!DEM_PUSH_STR(ddata, "auto"))
				return 0;
			++ddata->cur;
			break;
		case 'c':
			/* decltype(auto) */
			if (!DEM_PUSH_STR(ddata, "decltype(auto)"))
				return 0;
			++ddata->cur;
			break;
		case 'd':
			/* IEEE 754r decimal floating point (64 bits) */
			if (!DEM_PUSH_STR(ddata, "decimal64"))
				return 0;
			++ddata->cur;
			break;
		case 'e':
			/* IEEE 754r decimal floating point (128 bits) */
			if (!DEM_PUSH_STR(ddata, "decimal128"))
				return 0;
			++ddata->cur;
			break;
		case 'f':
			/* IEEE 754r decimal floating point (32 bits) */
			if (!DEM_PUSH_STR(ddata, "decimal32"))
				return 0;
			++ddata->cur;
			break;
		case 'h':
			/* IEEE 754r half-precision floating point (16 bits) */
			if (!DEM_PUSH_STR(ddata, "half"))
				return 0;
			++ddata->cur;
			break;
		case 'i':
			/* char32_t */
			if (!DEM_PUSH_STR(ddata, "char32_t"))
				return 0;
			++ddata->cur;
			break;
		case 'n':
			/* std::nullptr_t (i.e., decltype(nullptr)) */
			if (!DEM_PUSH_STR(ddata, "decltype(nullptr)"))
				return 0;
			++ddata->cur;
			break;
		case 's':
			/* char16_t */
			if (!DEM_PUSH_STR(ddata, "char16_t"))
				return 0;
			++ddata->cur;
			break;
		case 'v':
			/* gcc vector_size extension. */
			++ddata->cur;
			if (*ddata->cur == '_') {
				++ddata->cur;
				if (!demangle_read_expression_flat(ddata,
					&exp_str))
					return 0;
				if (!VEC_PUSH_STR(&v.ext_name, exp_str))
					return 0;
			} else {
				if (!demangle_read_number_as_string(ddata,
					&num_str))
					return 0;
				if (!VEC_PUSH_STR(&v.ext_name, num_str))
					return 0;
			}
			if (*ddata->cur != '_')
				return 0;
			++ddata->cur;
			if (!demangle_vector_type_qualifier_push(ddata, &v,
				TYPE_VEC))
				return 0;
			if (td)
				td->firstp = false;
			goto again;
		default:
			return 0;
		}
		goto rtn;

	case 'e':
		/* long double */
		if (!DEM_PUSH_STR(ddata, "long double"))
			return 0;
		++ddata->cur;
		goto rtn;

	case 'E':
		/* unexpected end (except some things) */
		if (td && ddata->is_guard_variable)
			td->paren = false;
		if (ddata->is_guard_variable ||
		    (ddata->ref_qualifier && ddata->is_functype)) {
			skip_ref_qualifier = true;
			/* Pop the delimiter. */
			demangle_pop_str(ddata);
			goto rtn;
		}
		return 0;

	case 'f':
		/* float */
		if (!DEM_PUSH_STR(ddata, "float"))
			return 0;
		++ddata->cur;
		goto rtn;

	case 'F':
		/* function */
		if (!demangle_read_function(ddata, &extern_c, &v))
			return 0;
		is_builtin = 0;
		goto rtn;

	case 'g':
		/* __float128 */
		if (!DEM_PUSH_STR(ddata, "__float128"))
			return 0;
		++ddata->cur;
		goto rtn;

	case 'G':
		/* imaginary */
		if (!demangle_vector_type_qualifier_push(ddata, &v, TYPE_IMG))
			return 0;
		++ddata->cur;
		if (td)
			td->firstp = false;
		goto again;

	case 'h':
		/* unsigned char */
		if (!DEM_PUSH_STR(ddata, "unsigned char"))
			return 0;
		++ddata->cur;
		goto rtn;

	case 'i':
		/* int */
		if (!DEM_PUSH_STR(ddata, "int"))
			return 0;
		++ddata->cur;
		goto rtn;

	case 'I':
		/* template args. */
		/* handles <substitute><template-args> */
		p_idx = output->size;
		if (!demangle_read_tmpl_args(ddata))
			goto clean;
		if (!(subst_str = demangle_vector_str_substr(ddata, output,
			  p_idx, output->size - 1, &subst_str_len)))
			goto clean;
		demangle_vector_str_init(ddata, &sv);
		if (!demangle_vector_str_push(ddata, &sv, subst_str,
			subst_str_len)) {
			demangle_free(ddata, subst_str);
			demangle_vector_str_dest(ddata, &sv);
			goto clean;
		}
		demangle_free(ddata, subst_str);
		if (!demangle_push_subst_v(ddata, &sv)) {
			demangle_vector_str_dest(ddata, &sv);
			goto clean;
		}
		demangle_vector_str_dest(ddata, &sv);
		goto rtn;

	case 'j':
		/* unsigned int */
		if (!DEM_PUSH_STR(ddata, "unsigned int"))
			return 0;
		++ddata->cur;
		goto rtn;

	case 'K':
		/* const */
		if (!demangle_vector_type_qualifier_push(ddata, &v, TYPE_CST))
			return 0;
		++ddata->cur;
		if (td)
			td->firstp = false;
		goto again;

	case 'l':
		/* long */
		if (!DEM_PUSH_STR(ddata, "long"))
			return 0;
		++ddata->cur;
		goto rtn;

	case 'm':
		/* unsigned long */
		if (!DEM_PUSH_STR(ddata, "unsigned long"))
			return 0;

		++ddata->cur;

		goto rtn;
	case 'M':
		/* pointer to member */
		if (!demangle_read_pointer_to_member(ddata, &v))
			return 0;
		is_builtin = 0;
		goto rtn;

	case 'n':
		/* __int128 */
		if (!DEM_PUSH_STR(ddata, "__int128"))
			return 0;
		++ddata->cur;
		goto rtn;

	case 'o':
		/* unsigned __int128 */
		if (!DEM_PUSH_STR(ddata, "unsigned __int128"))
			return 0;
		++ddata->cur;
		goto rtn;

	case 'O':
		/* rvalue reference */
		if (ddata->ref_qualifier)
			return 0;
		if (!demangle_vector_type_qualifier_push(ddata, &v, TYPE_RREF))
			return 0;
		ddata->ref_qualifier = true;
		ddata->ref_qualifier_type = TYPE_RREF;
		++ddata->cur;
		if (td)
			td->firstp = false;
		goto again;

	case 'P':
		/* pointer */
		if (!demangle_vector_type_qualifier_push(ddata, &v, TYPE_PTR))
			return 0;
		++ddata->cur;
		if (td)
			td->firstp = false;
		goto again;

	case 'r':
		/* restrict */
		if (!demangle_vector_type_qualifier_push(ddata, &v, TYPE_RST))
			return 0;
		++ddata->cur;
		if (td)
			td->firstp = false;
		goto again;

	case 'R':
		/* reference */
		if (ddata->ref_qualifier)
			return 0;
		if (!demangle_vector_type_qualifier_push(ddata, &v, TYPE_REF))
			return 0;
		ddata->ref_qualifier = true;
		ddata->ref_qualifier_type = TYPE_REF;
		++ddata->cur;
		if (td)
			td->firstp = false;
		goto again;

	case 's':
		/* short, local string */
		if (!DEM_PUSH_STR(ddata, "short"))
			return 0;
		++ddata->cur;
		goto rtn;

	case 'S':
		/* substitution */
		if (!demangle_read_subst(ddata))
			return 0;
		is_builtin = 0;
		goto rtn;

	case 't':
		/* unsigned short */
		if (!DEM_PUSH_STR(ddata, "unsigned short"))
			return 0;
		++ddata->cur;
		goto rtn;

	case 'T':
		/* template parameter */
		if (!demangle_read_tmpl_param(ddata))
			return 0;
		is_builtin = 0;
		goto rtn;

	case 'u':
		/* vendor extended builtin */
		++ddata->cur;
		if (!demangle_read_sname(ddata))
			return 0;
		is_builtin = 0;
		goto rtn;

	case 'U':
		/* vendor extended type qualifier */
		++ddata->cur;
		if (!demangle_read_number(ddata, &len))
			return 0;
		if (len <= 0)
			return 0;
		if (!demangle_vector_str_push(ddata, &v.ext_name, ddata->cur,
			len))
			return 0;
		if (len > demangle_strlen(ddata->cur))
			len = demangle_strlen(ddata->cur);
		ddata->cur += len;
		if (!demangle_vector_type_qualifier_push(ddata, &v, TYPE_EXT))
			return 0;
		if (td)
			td->firstp = false;
		goto again;

	case 'v':
		/* void */
		omit_void = false;
		if (td && td->firstp) {
			/*
			 * peek into next bytes and see if we should omit
			 * the "void".
			 */
			omit_void = true;
			for (p = ddata->cur + 1; *p != '\0'; p++) {
				if (*p == 'E')
					break;
				if (*p != 'R' && *p != 'O') {
					omit_void = false;
					break;
				}
			}
		}
		if (!omit_void && !DEM_PUSH_STR(ddata, "void"))
			return 0;
		++ddata->cur;
		goto rtn;

	case 'V':
		/* volatile */
		if (!demangle_vector_type_qualifier_push(ddata, &v, TYPE_VAT))
			return 0;
		++ddata->cur;
		if (td)
			td->firstp = false;
		goto again;

	case 'w':
		/* wchar_t */
		if (!DEM_PUSH_STR(ddata, "wchar_t"))
			return 0;
		++ddata->cur;
		goto rtn;

	case 'x':
		/* long long */
		if (!DEM_PUSH_STR(ddata, "long long"))
			return 0;
		++ddata->cur;
		goto rtn;

	case 'y':
		/* unsigned long long */
		if (!DEM_PUSH_STR(ddata, "unsigned long long"))
			return 0;
		++ddata->cur;
		goto rtn;

	case 'z':
		/* ellipsis */
		if (!DEM_PUSH_STR(ddata, "..."))
			return 0;
		++ddata->cur;
		goto rtn;
	}

	if (!demangle_read_name(ddata))
		return 0;

	is_builtin = 0;
rtn:

	type_str = demangle_vector_str_substr(ddata, output, p_idx,
	    output->size - 1, &type_str_len);

	if (is_builtin == 0) {
		if (!demangle_vector_str_find(ddata, &ddata->subst, type_str,
			type_str_len) &&
		    !demangle_vector_str_push(ddata, &ddata->subst, type_str,
			type_str_len))
			goto clean;
	}

	if (!skip_ref_qualifier &&
	    !demangle_push_type_qualifier(ddata, &v, type_str))
		goto clean;

	if (td)
		td->firstp = false;

	demangle_free(ddata, type_str);
	demangle_free(ddata, exp_str);
	demangle_free(ddata, num_str);
	demangle_vector_type_qualifier_dest(ddata, &v);

	return 1;
clean:
	demangle_free(ddata, type_str);
	demangle_free(ddata, exp_str);
	demangle_free(ddata, num_str);
	demangle_vector_type_qualifier_dest(ddata, &v);

	return 0;
}

ABI static int
demangle_read_type(struct demangle_data *ddata, struct type_delimit *td)
{
	if (ddata->depth == MAX_DEPTH)
		__builtin_longjmp(ddata->jmpbuf, 1);
	++ddata->depth;
	int res = demangle_read_type_impl(ddata, td);
	--ddata->depth;
	return res;
}

ABI static int
demangle_copy_output(struct demangle_data *ddata, char *buf,
    const struct vector_str *v, size_t buflen)
{
	size_t i, j, len = 0;
	for (i = 0; i < v->size; ++i)
		for (j = 0; VEC_STR(ddata, v, i)[j]; ++j) {
			if (len < buflen)
				buf[len] = VEC_STR(ddata, v, i)[j];
			++len;
		}
	if (buflen) {
		i = len;
		if (i >= buflen)
			i = buflen - 1;
		buf[i] = 0;
	}
	if (len < buflen)
		return len;
	else
		return -1;
}

ABI static int
demangle_failure(char *buf, const char *org, size_t buflen)
{
	demangle_strlcpy(buf, org, buflen);
	return -1;
}

ABI static int
demangle(struct demangle_data *ddata, char *buf, const char *org, size_t buflen)
{
	struct vector_str ret_type;
	struct type_delimit td;
	size_t org_len;
	unsigned int limit;
	bool has_ret = false, more_type = false;

	/* Use output buffer as heap memory. */
	int max_align = sizeof(char *);
	buflen = buflen > 0xfff0 ? 0xfff0 : buflen;
	ddata->free = 0;
	ddata->heap = ((uintptr_t)buf + max_align - 1) & -max_align;
	ddata->hoff = (buflen & -max_align) - (ddata->heap - (uintptr_t)buf);

	/* Try demangling as a type for short encodings. */
	org_len = demangle_strlen(org);
	if ((org_len < 2) || (org[0] != '_' || org[1] != 'Z')) {
		demangle_data_init(ddata, org);
		if (!demangle_read_type(ddata, 0))
			return demangle_failure(buf, org, buflen);
		return demangle_copy_output(ddata, buf, &ddata->output, buflen);
	}
	if (org_len > 11 && !demangle_strncmp(org, "_GLOBAL__I_", 11)) {
		demangle_vector_str_init(ddata, &ret_type);
		if (!VEC_PUSH_STR(&ret_type, "global constructors keyed to "))
			return demangle_failure(buf, org, buflen);
		if (!VEC_PUSH_STR(&ret_type, org + 11))
			return demangle_failure(buf, org, buflen);
		return demangle_copy_output(ddata, buf, &ddata->output, buflen);
	}

	demangle_data_init(ddata, org + 2);

	if (!demangle_read_encoding(ddata))
		return demangle_failure(buf, org, buflen);

	/*
	 * Pop function name from substitution candidate list.
	 */
	if (*ddata->cur && ddata->subst.size >= 1) {
		if (!demangle_vector_str_pop(&ddata->subst))
			return demangle_failure(buf, org, buflen);
	}

	td.paren = false;
	td.firstp = true;
	limit = 0;

	/*
	 * The first type is a return type if we just demangled template
	 * args. (the template args is right next to the function name,
	 * which means it's a template function)
	 */
	if (ddata->is_tmpl) {
		ddata->is_tmpl = false;
		demangle_vector_str_init(ddata, &ret_type);
		ddata->cur_output = &ret_type;
		has_ret = true;
	}

	while (*ddata->cur) {
		/*
		 * GCC optimizations make symbols like foobu.constprop.1
		 */
		if (*ddata->cur == '.')
			break;

		/*
		 * Breaking at some gcc info at tail. e.g) @@GLIBCXX_3.4
		 */
		if (*ddata->cur == '@' && *(ddata->cur + 1) == '@')
			break;

		if (has_ret) {
			/* Read return type */
			if (!demangle_read_type(ddata, NULL))
				return demangle_failure(buf, org, buflen);
		} else {
			/* Read function arg type */
			if (!demangle_read_type(ddata, &td))
				return demangle_failure(buf, org, buflen);
		}

		if (has_ret) {
			/* Push return type to the beginning */
			if (!VEC_PUSH_STR(&ret_type, " "))
				return demangle_failure(buf, org, buflen);
			if (!demangle_vector_str_push_vector_head(ddata,
				&ddata->output, &ret_type))
				return demangle_failure(buf, org, buflen);
			ddata->cur_output = &ddata->output;
			has_ret = false;
			more_type = true;
		} else if (more_type) {
			more_type = false;
		}
		if (limit++ > DEMANGLE_TRY_LIMIT)
			return demangle_failure(buf, org, buflen);
	}
	if (more_type)
		return demangle_failure(buf, org, buflen);

	if (!ddata->output.size)
		return demangle_failure(buf, org, buflen);
	if (td.paren && !VEC_PUSH_STR(&ddata->output, ")"))
		return demangle_failure(buf, org, buflen);
	if (ddata->mem_vat && !VEC_PUSH_STR(&ddata->output, " volatile"))
		return demangle_failure(buf, org, buflen);
	if (ddata->mem_cst && !VEC_PUSH_STR(&ddata->output, " const"))
		return demangle_failure(buf, org, buflen);
	if (ddata->mem_rst && !VEC_PUSH_STR(&ddata->output, " restrict"))
		return demangle_failure(buf, org, buflen);
	if (ddata->mem_ref && !VEC_PUSH_STR(&ddata->output, " &"))
		return demangle_failure(buf, org, buflen);
	if (ddata->mem_rref && !VEC_PUSH_STR(&ddata->output, " &&"))
		return demangle_failure(buf, org, buflen);
	if (ddata->hoff < 32)
		return demangle_failure(buf, org, buflen);
	return demangle_copy_output(ddata, buf, &ddata->output, ddata->hoff);
}

/**
 * Decodes IA-64 C++ ABI style symbol.
 *
 * This function is designed to be safe to call from asynchronous signal
 * handlers, since dynamic memory is allocated from the end of `buf`. As
 * such it's important to provide a very generous amount of memory here.
 *
 * If you expect your symbols to be 100 characters long, you should pass
 * at least 2000 bytes of buffer. If this function runs out of memory it
 * will still create a NUL-terminated string in buf. On complete failure
 * this will simply be the original string copied. On truncation it will
 * be a partially copied result. In both cases, -1 is returned. The size
 * of the output is only returned if this routine is fully succesful. To
 * successfully cover nearly all the test cases from libcxxabi use 65536
 * and to be able to print 99% of the symbols LLVM's libcxx.a, use 5632.
 *
 * It's important to call __is_mangled() before this, since some symbols
 * have a special meaning; for example, "g" will return "__float128". It
 * should be noted that this routine won't decode c++ symbols containing
 * floating point numbers.
 *
 * **WARNING**: This implementation isn't as mature as __cxa_demangle().
 *              It passes 27,124 out of the 29,798 libcxxabi test cases.
 *
 * @param buf is where nul-terminated output string is stored
 * @param buflen should have at least 8192 bytes of memory
 * @return bytes of output name or -1 upon error or truncation
 * @asyncsignalsafe
 */
ABI int
__demangle(char *buf, const char *org, size_t buflen)
{
	struct demangle_data ddata[1];
	if (!__builtin_setjmp(ddata->jmpbuf))
		return demangle(ddata, buf, org, buflen);
	return demangle_failure(buf, org, buflen);
}

/**
 * Returns true if string is mangled by IA-64 C++ ABI style.
 *
 * This means it starts with either "_Z" or "_GLOBAL__I_".
 */
ABI int
__is_mangled(const char *org)
{
	if (!org)
		return false;
	if (org[0] != '_')
		return false;
	if (org[1] == 'Z')
		return true;
	return !demangle_strncmp(org, "_GLOBAL__I_", 11);
}
