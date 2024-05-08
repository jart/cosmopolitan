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
#include "libc/cosmo.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/internal.h"
#include "libc/fmt/itoa.h"
#include "libc/limits.h"
#include "libc/stdalign.internal.h"
#include "libc/stdckdint.h"
#include "libc/str/str.h"
#include "libc/str/tab.internal.h"

__notice(cpp_demangle_notice, "\
Cosmopolitan FreeBSD libcxxrt C++ Demangle (BSD-2)\n\
Copyright (c) 2007 Hyogeol Lee <hyogeollee@gmail.com>\n\
Copyright (c) 2015-2017 Kai Wang <kaiwang27@gmail.com>\n\
Copyright (c) 2024 Justine Tunney <jtunney@gmail.com>");
// https://github.com/freebsd/freebsd-src/blob/2176c9ab71c85efd90a6c7af4a9e04fe8e3d49ca/contrib/libcxxrt/libelftc_dem_gnu3.c
// clang-format off

/**
 * @file demangle.c
 * @brief Decode IA-64 C++ ABI style implementation.
 *
 * IA-64 standard ABI(Itanium C++ ABI) references.
 *
 * http://www.codesourcery.com/cxx-abi/abi.html#mangling \n
 * http://www.codesourcery.com/cxx-abi/abi-mangling.html
 */

#define DEMANGLE_DEMO
#define DEMANGLE_NO_FLOATING_POINT

#define ASSERT(x)	    (void)0

#define BUFFER_GROWFACTOR   1.618
#define BUFFER_GROW(x)	    (((x) + 0.5) * BUFFER_GROWFACTOR)

#define ELFTC_FAILURE	    0
#define ELFTC_ISDIGIT(C)    ('0' <= (C) && (C) <= '9')
#define ELFTC_SUCCESS	    1

#define VECTOR_DEF_CAPACITY 8

struct stack_str {
	char *str;
	int len;
	int cap;
	char buf[128];
};

struct vector_str {
	size_t size;
	size_t capacity;
	char **container;
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
	size_t size, capacity;
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
	size_t size, capacity;
	struct read_cmd_item *r_container;
};

enum push_qualifier {
	PUSH_ALL_QUALIFIER,
	PUSH_CV_QUALIFIER,
	PUSH_NON_CV_QUALIFIER,
};

struct cpp_demangle_data {
	uintptr_t heap;
	size_t hoff;
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
	enum type_qualifier ref_qualifier_type; /* ref qualifier type */
	enum push_qualifier push_qualifier;	/* which qualifiers to push */
	int func_type;
	const char *cur;	/* current mangled name ptr */
	const char *last_sname; /* last source name */
};

struct type_delimit {
	bool paren;
	bool firstp;
};

#define CPP_DEMANGLE_TRY_LIMIT	128
#define FLOAT_SPRINTF_TRY_LIMIT 5
#define FLOAT_QUADRUPLE_BYTES	16
#define FLOAT_EXTENED_BYTES	10

#define SIMPLE_HASH(x, y)	(64 * x + y)
#define DEM_PUSH_STR(d, s)	cpp_demangle_push_str((d), (s), strlen(s))
#define VEC_PUSH_STR(d, s)	vector_str_push(ddata, (d), (s), strlen(s))
static int cpp_demangle_read_encoding(struct cpp_demangle_data *);
static int cpp_demangle_read_expr_primary(struct cpp_demangle_data *);
static int cpp_demangle_read_expression(struct cpp_demangle_data *);
static int cpp_demangle_read_expression_flat(struct cpp_demangle_data *,
    char **);
static int cpp_demangle_read_expression_binary(struct cpp_demangle_data *,
    const char *, size_t);
static int cpp_demangle_read_expression_unary(struct cpp_demangle_data *,
    const char *, size_t);
static int cpp_demangle_read_expression_trinary(struct cpp_demangle_data *,
    const char *, size_t, const char *, size_t);
static int cpp_demangle_read_function(struct cpp_demangle_data *, int *,
    struct vector_type_qualifier *);
static int cpp_demangle_local_source_name(struct cpp_demangle_data *ddata);
static int cpp_demangle_read_local_name(struct cpp_demangle_data *);
static int cpp_demangle_read_name(struct cpp_demangle_data *);
static int cpp_demangle_read_name_flat(struct cpp_demangle_data *, char **);
static int cpp_demangle_read_nested_name(struct cpp_demangle_data *);
static int cpp_demangle_read_number(struct cpp_demangle_data *, long *);
static int cpp_demangle_read_number_as_string(struct cpp_demangle_data *,
    char **);
static int cpp_demangle_read_nv_offset(struct cpp_demangle_data *);
static int cpp_demangle_read_offset(struct cpp_demangle_data *);
static int cpp_demangle_read_offset_number(struct cpp_demangle_data *);
static int cpp_demangle_read_pointer_to_member(struct cpp_demangle_data *,
    struct vector_type_qualifier *);
static int cpp_demangle_read_sname(struct cpp_demangle_data *);
static int cpp_demangle_read_subst(struct cpp_demangle_data *);
static int cpp_demangle_read_subst_std(struct cpp_demangle_data *);
static int cpp_demangle_read_subst_stdtmpl(struct cpp_demangle_data *,
    const char *);
static int cpp_demangle_read_tmpl_arg(struct cpp_demangle_data *);
static int cpp_demangle_read_tmpl_args(struct cpp_demangle_data *);
static int cpp_demangle_read_tmpl_param(struct cpp_demangle_data *);
static int cpp_demangle_read_type(struct cpp_demangle_data *,
    struct type_delimit *);
static int cpp_demangle_read_type_flat(struct cpp_demangle_data *, char **);
static int cpp_demangle_read_uqname(struct cpp_demangle_data *);
static int cpp_demangle_read_v_offset(struct cpp_demangle_data *);
static struct read_cmd_item *vector_read_cmd_find(struct vector_read_cmd *,
    enum read_cmd);
static int vector_read_cmd_push(struct cpp_demangle_data *,
    struct vector_read_cmd *, enum read_cmd, void *);
static int vector_type_qualifier_init(struct cpp_demangle_data *,
    struct vector_type_qualifier *);
static int vector_type_qualifier_push(struct cpp_demangle_data *,
    struct vector_type_qualifier *, enum type_qualifier);

static void *
alloc(struct cpp_demangle_data *h, int a, size_t n)
{
	uintptr_t ptr;
	if (n > h->hoff)
		return 0;
	ptr = h->heap + h->hoff - n;
	while (ptr & (a - 1)) {
		if (ptr == h->heap)
			return 0;
		--ptr;
	}
	h->hoff = ptr - h->heap;
	return (void *)ptr;
}

static char *
dupstr(struct cpp_demangle_data *h, const char *s)
{
	char *d = 0;
	if (s) {
		size_t n = strlen(s) + 1;
		if ((d = (char *)alloc(h, 1, n)))
			memcpy(d, s, n);
	}
	return d;
}

static void
stack_str_init(struct stack_str *ss)
{
	ss->str = ss->buf;
	ss->buf[0] = 0;
	ss->len = 0;
	ss->cap = sizeof(ss->buf);
}

static bool
stack_str_append(struct cpp_demangle_data *h, struct stack_str *ss,
    const char *str, size_t len)
{
	int len2 = ss->len + len;
	int need = len2 + 1;
	if (need > ss->cap) {
		int cap2;
		char *str2;
		cap2 = need + (ss->cap >> 1);
		if (!(str2 = (char *)alloc(h, 1, cap2)))
			return false;
		memcpy(str2, ss->str, ss->len);
		ss->str = str2;
		ss->cap = cap2;
	}
	memcpy(ss->str + ss->len, str, len);
	ss->str[len2] = 0;
	ss->len = len2;
	return true;
}

#define stack_str_append_str(h, ss, s) stack_str_append(h, ss, s, strlen(s))

static size_t
get_strlen_sum(const struct vector_str *v)
{
	size_t i, len = 0;
	if (!v)
		return 0;
	ASSERT(v->size > 0);
	for (i = 0; i < v->size; ++i)
		len += strlen(v->container[i]);
	return len;
}

/**
 * @brief Find string in vector_str.
 * @param v Destination vector.
 * @param o String to find.
 * @param l Length of the string.
 * @return -1 at failed, 0 at not found, 1 at found.
 */
static int
vector_str_find(const struct vector_str *v, const char *o, size_t l)
{
	size_t i;

	if (!v || !o)
		return -1;

	for (i = 0; i < v->size; ++i)
		if (!strncmp(v->container[i], o, l))
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
static char *
vector_str_get_flat(struct cpp_demangle_data *ddata, const struct vector_str *v,
    size_t *l)
{
	size_t i;
	char *rtn, *p;
	ssize_t rtn_size;

	if (!v || !v->size)
		return 0;

	if (!(rtn_size = get_strlen_sum(v)))
		return 0;

	if (!(rtn = (char *)alloc(ddata, 1, rtn_size + 1)))
		return 0;

	p = rtn;
	for (i = 0; i < v->size; ++i)
		p = stpcpy(p, v->container[i]);

	if (l)
		*l = rtn_size;

	return rtn;
}

static bool
vector_str_grow(struct cpp_demangle_data *ddata, struct vector_str *v)
{
	size_t i, tmp_cap;
	char **tmp_ctn;

	if (!v)
		return false;

	ASSERT(v->capacity > 0);

	tmp_cap = BUFFER_GROW(v->capacity);

	ASSERT(tmp_cap > v->capacity);

	if (!(tmp_ctn = (char **)alloc(ddata, alignof(char *),
		  sizeof(char *) * tmp_cap)))
		return false;

	for (i = 0; i < v->size; ++i)
		tmp_ctn[i] = v->container[i];

	v->container = tmp_ctn;
	v->capacity = tmp_cap;

	return true;
}

/**
 * @brief Initialize vector_str.
 * @return false at failed, true at success.
 */
static bool
vector_str_init(struct cpp_demangle_data *ddata, struct vector_str *v)
{
	if (!v)
		return false;

	v->size = 0;
	v->capacity = VECTOR_DEF_CAPACITY;

	ASSERT(v->capacity > 0);

	if (!(v->container = (char **)alloc(ddata, alignof(char *),
		  sizeof(char *) * v->capacity)))
		return false;

	ASSERT(v->container);

	return true;
}

/**
 * @brief Remove last element in vector_str.
 * @return false at failed, true at success.
 */
static bool
vector_str_pop(struct vector_str *v)
{
	if (!v)
		return false;

	if (!v->size)
		return true;

	--v->size;

	v->container[v->size] = NULL;

	return true;
}

/**
 * @brief Push back string to vector.
 * @return false at failed, true at success.
 */
static bool
vector_str_push(struct cpp_demangle_data *ddata, struct vector_str *v,
    const char *str, size_t len)
{
	if (!v || !str)
		return false;

	if (v->size == v->capacity && !vector_str_grow(ddata, v))
		return false;

	if (!(v->container[v->size] = (char *)alloc(ddata, 1, len + 1)))
		return false;

	strlcpy(v->container[v->size], str, len + 1);

	++v->size;

	return true;
}

/**
 * @brief Push front org vector to det vector.
 * @return false at failed, true at success.
 */
static bool
vector_str_push_vector_head(struct cpp_demangle_data *ddata,
    struct vector_str *dst, struct vector_str *org)
{
	size_t i, tmp_cap;
	char **tmp_ctn;

	if (!dst || !org)
		return false;

	tmp_cap = BUFFER_GROW(dst->size + org->size);

	if (!(tmp_ctn = (char **)alloc(ddata, alignof(char *),
		  sizeof(char *) * tmp_cap)))
		return false;

	for (i = 0; i < org->size; ++i)
		if (!(tmp_ctn[i] = dupstr(ddata, org->container[i])))
			return false;

	for (i = 0; i < dst->size; ++i)
		tmp_ctn[i + org->size] = dst->container[i];

	dst->container = tmp_ctn;
	dst->capacity = tmp_cap;
	dst->size += org->size;

	return true;
}

/**
 * @brief Push org vector to the tail of det vector.
 * @return false at failed, true at success.
 */
static bool
vector_str_push_vector(struct cpp_demangle_data *ddata, struct vector_str *dst,
    struct vector_str *org)
{
	size_t i, tmp_cap;
	char **tmp_ctn;

	if (!dst || !org)
		return false;

	tmp_cap = BUFFER_GROW(dst->size + org->size);

	if (!(tmp_ctn = (char **)alloc(ddata, alignof(char *),
		  sizeof(char *) * tmp_cap)))
		return false;

	for (i = 0; i < dst->size; ++i)
		tmp_ctn[i] = dst->container[i];

	for (i = 0; i < org->size; ++i)
		if (!(tmp_ctn[i + dst->size] = dupstr(ddata,
			  org->container[i])))
			return false;

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
static char *
vector_str_substr(struct cpp_demangle_data *ddata, const struct vector_str *v,
    size_t begin, size_t end, size_t *r_len)
{
	char *rtn, *p;
	size_t i, len;

	if (!v || begin > end)
		return 0;

	len = 0;
	for (i = begin; i < end + 1; ++i)
		len += strlen(v->container[i]);

	if (!(rtn = (char *)alloc(ddata, 1, len + 1)))
		return 0;

	if (r_len)
		*r_len = len;

	p = rtn;
	for (i = begin; i < end + 1; ++i)
		p = stpcpy(p, v->container[i]);

	return rtn;
}

static int
vector_read_cmd_pop(struct vector_read_cmd *v)
{
	if (!v || !v->size)
		return 0;

	--v->size;
	v->r_container[v->size].cmd = READ_FAIL;
	v->r_container[v->size].data = NULL;

	return 1;
}

static int
vector_read_cmd_init(struct cpp_demangle_data *ddata, struct vector_read_cmd *v)
{
	if (!v)
		return 0;

	v->size = 0;
	v->capacity = VECTOR_DEF_CAPACITY;

	if (!(v->r_container = (struct read_cmd_item *)alloc(ddata,
		  alignof(*v->r_container),
		  sizeof(*v->r_container) * v->capacity)))
		return 0;

	return 1;
}

static int
cpp_demangle_data_init(struct cpp_demangle_data *d, const char *cur)
{
	if (!d || !cur)
		return 0;

	if (!vector_str_init(d, &d->output))
		return 0;
	if (!vector_str_init(d, &d->subst))
		return 0;
	if (!vector_str_init(d, &d->tmpl))
		return 0;
	if (!vector_str_init(d, &d->class_type))
		return 0;
	if (!vector_read_cmd_init(d, &d->cmd))
		return 0;

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
	d->push_qualifier = PUSH_ALL_QUALIFIER;
	d->func_type = 0;
	d->cur = cur;
	d->cur_output = &d->output;
	d->last_sname = NULL;

	return 1;
}

static int
cpp_demangle_push_str(struct cpp_demangle_data *ddata, const char *str,
    size_t len)
{
	if (!ddata || !str || !len)
		return 0;

	/*
	 * is_tmpl is used to check if the type (function arg) is right next
	 * to template args, and should always be cleared whenever new string
	 * pushed.
	 */
	ddata->is_tmpl = false;

	return vector_str_push(ddata, ddata->cur_output, str, len);
}

#ifndef DEMANGLE_NO_FLOATING_POINT
static int
cpp_demangle_push_fp(struct cpp_demangle_data *ddata,
    char *decoder(struct cpp_demangle_data *, const char *, size_t))
{
	size_t len;
	int rtn;
	const char *fp;
	char *f;

	if (!ddata || !decoder)
		return 0;

	fp = ddata->cur;
	while (*ddata->cur != 'E')
		++ddata->cur;

	if (!(f = decoder(ddata, fp, ddata->cur - fp)))
		return 0;

	rtn = 0;
	if ((len = strlen(f)) > 0)
		rtn = cpp_demangle_push_str(ddata, f, len);

	++ddata->cur;

	return rtn;
}
#endif // DEMANGLE_NO_FLOATING_POINT

static int
cpp_demangle_pop_str(struct cpp_demangle_data *ddata)
{
	if (!ddata)
		return 0;

	return vector_str_pop(ddata->cur_output);
}

static int
cpp_demangle_push_subst(struct cpp_demangle_data *ddata, const char *str,
    size_t len)
{
	if (!ddata || !str || !len)
		return 0;

	if (!vector_str_find(&ddata->subst, str, len))
		return vector_str_push(ddata, &ddata->subst, str, len);

	return 1;
}

static int
cpp_demangle_push_subst_v(struct cpp_demangle_data *ddata, struct vector_str *v)
{
	int rtn;
	char *str;
	size_t str_len;

	if (!ddata || !v)
		return 0;

	if (!(str = vector_str_get_flat(ddata, v, &str_len)))
		return 0;

	rtn = cpp_demangle_push_subst(ddata, str, str_len);

	return rtn;
}

static int
cpp_demangle_push_type_qualifier(struct cpp_demangle_data *ddata,
    struct vector_type_qualifier *v, const char *type_str)
{
	struct stack_str subst_v;
	enum type_qualifier t;
	size_t idx, e_idx, e_len;
	char *buf;
	bool cv;

	if (!ddata || !v)
		return 0;

	if (!(idx = v->size))
		return 1;

	if (type_str) {
		stack_str_init(&subst_v);
		if (!stack_str_append_str(ddata, &subst_v, type_str))
			return 0;
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
				if (!stack_str_append_str(ddata, &subst_v, "*"))
					return 0;
				if (!cpp_demangle_push_subst(ddata, subst_v.str,
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
				if (!stack_str_append_str(ddata, &subst_v, "&"))
					return 0;
				if (!cpp_demangle_push_subst(ddata, subst_v.str,
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
				if (!stack_str_append_str(ddata, &subst_v,
					"&&"))
					return 0;
				if (!cpp_demangle_push_subst(ddata, subst_v.str,
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
				if (!stack_str_append_str(ddata, &subst_v,
					" complex"))
					return 0;
				if (!cpp_demangle_push_subst(ddata, subst_v.str,
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
				if (!stack_str_append_str(ddata, &subst_v,
					" imaginary"))
					return 0;
				if (!cpp_demangle_push_subst(ddata, subst_v.str,
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
			if (!(e_len = strlen(v->ext_name.container[e_idx])))
				return 0;
			if (!(buf = (char *)alloc(ddata, 1, e_len + 2)))
				return 0;
			buf[0] = ' ';
			memcpy(buf + 1, v->ext_name.container[e_idx],
			    e_len + 1);

			if (!DEM_PUSH_STR(ddata, buf))
				return 0;

			if (type_str) {
				if (!stack_str_append_str(ddata, &subst_v, buf))
					return 0;
				if (!cpp_demangle_push_subst(ddata, subst_v.str,
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
				if (!stack_str_append_str(ddata, &subst_v,
					" restrict"))
					return 0;
				if (idx - 1 > 0) {
					t = v->q_container[idx - 2];
					if (t == TYPE_RST || t == TYPE_VAT ||
					    t == TYPE_CST)
						break;
				}
				if (!cpp_demangle_push_subst(ddata, subst_v.str,
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
				if (!stack_str_append_str(ddata, &subst_v,
					" volatile"))
					return 0;
				if (idx - 1 > 0) {
					t = v->q_container[idx - 2];
					if (t == TYPE_RST || t == TYPE_VAT ||
					    t == TYPE_CST)
						break;
				}
				if (!cpp_demangle_push_subst(ddata, subst_v.str,
					subst_v.len))
					return 0;
			}
			break;

		case TYPE_CST:
			if (ddata->push_qualifier == PUSH_NON_CV_QUALIFIER &&
			    cv)
				break;
			if (ddata->push_qualifier == PUSH_CV_QUALIFIER && !cv)
				break;
			if (!DEM_PUSH_STR(ddata, " const"))
				return 0;
			if (type_str) {
				if (!stack_str_append_str(ddata, &subst_v,
					" const"))
					return 0;
				if (idx - 1 > 0) {
					t = v->q_container[idx - 2];
					if (t == TYPE_RST || t == TYPE_VAT ||
					    t == TYPE_CST)
						break;
				}
				if (!cpp_demangle_push_subst(ddata, subst_v.str,
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
			if (!(e_len = strlen(v->ext_name.container[e_idx])))
				return 0;
			if (!(buf = (char *)alloc(ddata, 1, e_len + 12)))
				return 0;
			p = buf;
			p = stpcpy(p, " __vector(");
			p = (char *)mempcpy(p, v->ext_name.container[e_idx],
			    e_len);
			p = stpcpy(p, ")");
			if (!DEM_PUSH_STR(ddata, buf))
				return 0;
			if (type_str) {
				if (!stack_str_append_str(ddata, &subst_v, buf))
					return 0;
				if (!cpp_demangle_push_subst(ddata, subst_v.str,
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

static int
cpp_demangle_get_subst(struct cpp_demangle_data *ddata, size_t idx)
{
	size_t len;

	if (!ddata || ddata->subst.size <= idx)
		return 0;
	if (!(len = strlen(ddata->subst.container[idx])))
		return 0;
	if (!cpp_demangle_push_str(ddata, ddata->subst.container[idx], len))
		return 0;

	/* skip '_' */
	++ddata->cur;

	return 1;
}

static int
cpp_demangle_get_tmpl_param(struct cpp_demangle_data *ddata, size_t idx)
{
	size_t len;

	if (!ddata || ddata->tmpl.size <= idx)
		return 0;
	if (!(len = strlen(ddata->tmpl.container[idx])))
		return 0;
	if (!cpp_demangle_push_str(ddata, ddata->tmpl.container[idx], len))
		return 0;

	++ddata->cur;

	return 1;
}

static int
cpp_demangle_read_array(struct cpp_demangle_data *ddata)
{
	size_t i, num_len, exp_len, p_idx, idx;
	const char *num;
	char *exp;

	if (!ddata || *(++ddata->cur) == '\0')
		return 0;

	if (*ddata->cur == '_') {
		if (*(++ddata->cur) == '\0')
			return 0;

		if (!cpp_demangle_read_type(ddata, NULL))
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
			if (!cpp_demangle_read_type(ddata, NULL))
				return 0;
			if (!DEM_PUSH_STR(ddata, " ["))
				return 0;
			if (!cpp_demangle_push_str(ddata, num, num_len))
				return 0;
			if (!DEM_PUSH_STR(ddata, "]"))
				return 0;
		} else {
			p_idx = ddata->output.size;
			if (!cpp_demangle_read_expression(ddata))
				return 0;
			if (!(exp = vector_str_substr(ddata, &ddata->output,
				  p_idx, ddata->output.size - 1, &exp_len)))
				return 0;
			idx = ddata->output.size;
			for (i = p_idx; i < idx; ++i)
				if (!vector_str_pop(&ddata->output))
					return 0;
			if (*ddata->cur != '_')
				return 0;
			++ddata->cur;
			if (*ddata->cur == '\0')
				return 0;
			if (!cpp_demangle_read_type(ddata, NULL))
				return 0;
			if (!DEM_PUSH_STR(ddata, " ["))
				return 0;
			if (!cpp_demangle_push_str(ddata, exp, exp_len))
				return 0;
			if (!DEM_PUSH_STR(ddata, "]"))
				return 0;
		}
	}

	return 1;
}

#ifndef DEMANGLE_NO_FLOATING_POINT

/* Simple hex to integer function used by decode_to_* function. */
static int
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
static char *
decode_fp_to_double(struct cpp_demangle_data *ddata, const char *p, size_t len)
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
	if (!(rtn = (char *)alloc(ddata, alignof(char),
		  sizeof(char) * rtn_len)))
		return 0;

	if (snprintf(rtn, rtn_len, "%fld", f) >= (int)rtn_len) {
		if (limit++ > FLOAT_SPRINTF_TRY_LIMIT)
			return 0;
		rtn_len *= BUFFER_GROWFACTOR;
		goto again;
	}

	return rtn;
}

static char *
decode_fp_to_float(struct cpp_demangle_data *ddata, const char *p, size_t len)
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
	if (!(rtn = (char *)alloc(ddata, alignof(char),
		  sizeof(char) * rtn_len)))
		return 0;

	if (snprintf(rtn, rtn_len, "%ff", f) >= (int)rtn_len) {
		if (limit++ > FLOAT_SPRINTF_TRY_LIMIT)
			return 0;
		rtn_len *= BUFFER_GROWFACTOR;
		goto again;
	}

	return rtn;
}

static char *
decode_fp_to_long_double(struct cpp_demangle_data *ddata, const char *p,
    size_t len)
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
	if (!(rtn = (char *)alloc(ddata, alignof(char),
		  sizeof(char) * rtn_len)))
		return 0;

	if (snprintf(rtn, rtn_len, "%Lfd", f) >= (int)rtn_len) {
		if (limit++ > FLOAT_SPRINTF_TRY_LIMIT)
			return 0;
		rtn_len *= BUFFER_GROWFACTOR;
		goto again;
	}

	return rtn;
}

static char *
decode_fp_to_float128(struct cpp_demangle_data *ddata, const char *p,
    size_t len)
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
		memcpy(&f, buf, FLOAT_EXTENED_BYTES);
#else  /* ELFTC_BYTE_ORDER != ELFTC_BYTE_ORDER_LITTLE_ENDIAN */
		memcpy(&f, buf + 6, FLOAT_EXTENED_BYTES);
#endif /* ELFTC_BYTE_ORDER == ELFTC_BYTE_ORDER_LITTLE_ENDIAN */

		rtn_len = 256;
		limit = 0;
	again:
		if (!(rtn = (char *)alloc(ddata, alignof(char),
			  sizeof(char) * rtn_len)))
			return 0;

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

static char *
decode_fp_to_float80(struct cpp_demangle_data *ddata, const char *p, size_t len)
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
		memcpy(&f, buf, FLOAT_EXTENED_BYTES);
#else  /* ELFTC_BYTE_ORDER != ELFTC_BYTE_ORDER_LITTLE_ENDIAN */
		memcpy((unsigned char *)(&f) + 6, buf, FLOAT_EXTENED_BYTES);
#endif /* ELFTC_BYTE_ORDER == ELFTC_BYTE_ORDER_LITTLE_ENDIAN */

		rtn_len = 256;
		limit = 0;
	again:
		if (!(rtn = (char *)alloc(ddata, alignof(char),
			  sizeof(char) * rtn_len)))
			return 0;

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

static int
cpp_demangle_read_expr_primary(struct cpp_demangle_data *ddata)
{
	const char *num;

	if (!ddata || *(++ddata->cur) == '\0')
		return 0;

	if (*ddata->cur == '_' && *(ddata->cur + 1) == 'Z') {
		ddata->cur += 2;
		if (*ddata->cur == '\0')
			return 0;
		if (!cpp_demangle_read_encoding(ddata))
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
		return cpp_demangle_push_fp(ddata, decode_fp_to_double);

	case 'e':
		++ddata->cur;
		if (sizeof(long double) == 10)
			return cpp_demangle_push_fp(ddata, decode_fp_to_double);
		return cpp_demangle_push_fp(ddata, decode_fp_to_float80);

	case 'f':
		++ddata->cur;
		return cpp_demangle_push_fp(ddata, decode_fp_to_float);

	case 'g':
		++ddata->cur;
		if (sizeof(long double) == 16)
			return cpp_demangle_push_fp(ddata, decode_fp_to_double);
		return cpp_demangle_push_fp(ddata, decode_fp_to_float128);

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
		return cpp_demangle_push_str(ddata, num, ddata->cur - num - 1);

	default:
		return 0;
	}
}

static int
cpp_demangle_read_expression(struct cpp_demangle_data *ddata)
{
	if (!ddata || *ddata->cur == '\0')
		return 0;

	switch (SIMPLE_HASH(*ddata->cur, *(ddata->cur + 1))) {
	case SIMPLE_HASH('s', 't'):
		ddata->cur += 2;
		return cpp_demangle_read_type(ddata, NULL);

	case SIMPLE_HASH('s', 'r'):
		ddata->cur += 2;
		if (!cpp_demangle_read_type(ddata, NULL))
			return 0;
		if (!cpp_demangle_read_uqname(ddata))
			return 0;
		if (*ddata->cur == 'I')
			return cpp_demangle_read_tmpl_args(ddata);
		return 1;

	case SIMPLE_HASH('a', 'a'):
		/* operator && */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, "&&", 2);

	case SIMPLE_HASH('a', 'd'):
		/* operator & (unary) */
		ddata->cur += 2;
		return cpp_demangle_read_expression_unary(ddata, "&", 1);

	case SIMPLE_HASH('a', 'n'):
		/* operator & */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, "&", 1);

	case SIMPLE_HASH('a', 'N'):
		/* operator &= */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, "&=", 2);

	case SIMPLE_HASH('a', 'S'):
		/* operator = */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, "=", 1);

	case SIMPLE_HASH('c', 'l'):
		/* operator () */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, "()", 2);

	case SIMPLE_HASH('c', 'm'):
		/* operator , */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, ",", 1);

	case SIMPLE_HASH('c', 'o'):
		/* operator ~ */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, "~", 1);

	case SIMPLE_HASH('c', 'v'):
		/* operator (cast) */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, "(cast)", 6);

	case SIMPLE_HASH('d', 'a'):
		/* operator delete [] */
		ddata->cur += 2;
		return cpp_demangle_read_expression_unary(ddata, "delete []",
		    9);

	case SIMPLE_HASH('d', 'e'):
		/* operator * (unary) */
		ddata->cur += 2;
		return cpp_demangle_read_expression_unary(ddata, "*", 1);

	case SIMPLE_HASH('d', 'l'):
		/* operator delete */
		ddata->cur += 2;
		return cpp_demangle_read_expression_unary(ddata, "delete", 6);

	case SIMPLE_HASH('d', 'v'):
		/* operator / */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, "/", 1);

	case SIMPLE_HASH('d', 'V'):
		/* operator /= */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, "/=", 2);

	case SIMPLE_HASH('e', 'o'):
		/* operator ^ */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, "^", 1);

	case SIMPLE_HASH('e', 'O'):
		/* operator ^= */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, "^=", 2);

	case SIMPLE_HASH('e', 'q'):
		/* operator == */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, "==", 2);

	case SIMPLE_HASH('g', 'e'):
		/* operator >= */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, ">=", 2);

	case SIMPLE_HASH('g', 't'):
		/* operator > */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, ">", 1);

	case SIMPLE_HASH('i', 'x'):
		/* operator [] */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, "[]", 2);

	case SIMPLE_HASH('l', 'e'):
		/* operator <= */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, "<=", 2);

	case SIMPLE_HASH('l', 's'):
		/* operator << */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, "<<", 2);

	case SIMPLE_HASH('l', 'S'):
		/* operator <<= */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, "<<=", 3);

	case SIMPLE_HASH('l', 't'):
		/* operator < */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, "<", 1);

	case SIMPLE_HASH('m', 'i'):
		/* operator - */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, "-", 1);

	case SIMPLE_HASH('m', 'I'):
		/* operator -= */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, "-=", 2);

	case SIMPLE_HASH('m', 'l'):
		/* operator * */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, "*", 1);

	case SIMPLE_HASH('m', 'L'):
		/* operator *= */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, "*=", 2);

	case SIMPLE_HASH('m', 'm'):
		/* operator -- */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, "--", 2);

	case SIMPLE_HASH('n', 'a'):
		/* operator new[] */
		ddata->cur += 2;
		return cpp_demangle_read_expression_unary(ddata, "new []", 6);

	case SIMPLE_HASH('n', 'e'):
		/* operator != */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, "!=", 2);

	case SIMPLE_HASH('n', 'g'):
		/* operator - (unary) */
		ddata->cur += 2;
		return cpp_demangle_read_expression_unary(ddata, "-", 1);

	case SIMPLE_HASH('n', 't'):
		/* operator ! */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, "!", 1);

	case SIMPLE_HASH('n', 'w'):
		/* operator new */
		ddata->cur += 2;
		return cpp_demangle_read_expression_unary(ddata, "new", 3);

	case SIMPLE_HASH('o', 'o'):
		/* operator || */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, "||", 2);

	case SIMPLE_HASH('o', 'r'):
		/* operator | */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, "|", 1);

	case SIMPLE_HASH('o', 'R'):
		/* operator |= */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, "|=", 2);

	case SIMPLE_HASH('p', 'l'):
		/* operator + */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, "+", 1);

	case SIMPLE_HASH('p', 'L'):
		/* operator += */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, "+=", 2);

	case SIMPLE_HASH('p', 'm'):
		/* operator ->* */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, "->*", 3);

	case SIMPLE_HASH('p', 'p'):
		/* operator ++ */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, "++", 2);

	case SIMPLE_HASH('p', 's'):
		/* operator + (unary) */
		ddata->cur += 2;
		return cpp_demangle_read_expression_unary(ddata, "+", 1);

	case SIMPLE_HASH('p', 't'):
		/* operator -> */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, "->", 2);

	case SIMPLE_HASH('q', 'u'):
		/* operator ? */
		ddata->cur += 2;
		return cpp_demangle_read_expression_trinary(ddata, "?", 1, ":",
		    1);

	case SIMPLE_HASH('r', 'm'):
		/* operator % */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, "%", 1);

	case SIMPLE_HASH('r', 'M'):
		/* operator %= */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, "%=", 2);

	case SIMPLE_HASH('r', 's'):
		/* operator >> */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, ">>", 2);

	case SIMPLE_HASH('r', 'S'):
		/* operator >>= */
		ddata->cur += 2;
		return cpp_demangle_read_expression_binary(ddata, ">>=", 3);

	case SIMPLE_HASH('r', 'z'):
		/* operator sizeof */
		ddata->cur += 2;
		return cpp_demangle_read_expression_unary(ddata, "sizeof", 6);

	case SIMPLE_HASH('s', 'v'):
		/* operator sizeof */
		ddata->cur += 2;
		return cpp_demangle_read_expression_unary(ddata, "sizeof", 6);
	}

	switch (*ddata->cur) {
	case 'L':
		return cpp_demangle_read_expr_primary(ddata);
	case 'T':
		return cpp_demangle_read_tmpl_param(ddata);
	}

	return 0;
}

static int
cpp_demangle_read_expression_flat(struct cpp_demangle_data *ddata, char **str)
{
	struct vector_str *output;
	size_t i, p_idx, idx, exp_len;
	char *exp;

	output = &ddata->output;

	p_idx = output->size;

	if (!cpp_demangle_read_expression(ddata))
		return 0;

	if (!(exp = vector_str_substr(ddata, output, p_idx, output->size - 1,
		  &exp_len)))
		return 0;

	idx = output->size;
	for (i = p_idx; i < idx; ++i)
		if (!vector_str_pop(output))
			return 0;

	*str = exp;

	return 1;
}

static int
cpp_demangle_read_expression_binary(struct cpp_demangle_data *ddata,
    const char *name, size_t len)
{
	if (!ddata || !name || !len)
		return 0;
	if (!cpp_demangle_read_expression(ddata))
		return 0;
	if (!cpp_demangle_push_str(ddata, name, len))
		return 0;

	return cpp_demangle_read_expression(ddata);
}

static int
cpp_demangle_read_expression_unary(struct cpp_demangle_data *ddata,
    const char *name, size_t len)
{
	if (!ddata || !name || !len)
		return 0;
	if (!cpp_demangle_read_expression(ddata))
		return 0;

	return cpp_demangle_push_str(ddata, name, len);
}

static int
cpp_demangle_read_expression_trinary(struct cpp_demangle_data *ddata,
    const char *name1, size_t len1, const char *name2, size_t len2)
{
	if (!ddata || !name1 || !len1 || !name2 || !len2)
		return 0;

	if (!cpp_demangle_read_expression(ddata))
		return 0;
	if (!cpp_demangle_push_str(ddata, name1, len1))
		return 0;
	if (!cpp_demangle_read_expression(ddata))
		return 0;
	if (!cpp_demangle_push_str(ddata, name2, len2))
		return 0;

	return cpp_demangle_read_expression(ddata);
}

static int
cpp_demangle_read_function(struct cpp_demangle_data *ddata, int *ext_c,
    struct vector_type_qualifier *v)
{
	struct type_delimit td;
	struct read_cmd_item *rc;
	size_t class_type_size, class_type_len, limit;
	const char *class_type;
	int i;
	bool paren, non_cv_qualifier;

	if (!ddata || *ddata->cur != 'F' || !v)
		return 0;

	++ddata->cur;
	if (*ddata->cur == 'Y') {
		if (ext_c)
			*ext_c = 1;
		++ddata->cur;
	}

	/* Return type */
	if (!cpp_demangle_read_type(ddata, NULL))
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
		rc = vector_read_cmd_find(&ddata->cmd, READ_PTRMEM);
		if (non_cv_qualifier || rc != NULL) {
			if (!DEM_PUSH_STR(ddata, "("))
				return 0;
			paren = true;
		}

		/* Push non-cv qualifiers. */
		ddata->push_qualifier = PUSH_NON_CV_QUALIFIER;
		if (!cpp_demangle_push_type_qualifier(ddata, v, NULL))
			return 0;

		if (rc) {
			if (non_cv_qualifier && !DEM_PUSH_STR(ddata, " "))
				return 0;
			if (!(class_type_size = ddata->class_type.size))
				return 0;
			class_type =
			    ddata->class_type.container[class_type_size - 1];
			if (!class_type)
				return 0;
			if (!(class_type_len = strlen(class_type)))
				return 0;
			if (!cpp_demangle_push_str(ddata, class_type,
				class_type_len))
				return 0;
			if (!DEM_PUSH_STR(ddata, "::*"))
				return 0;
			/* Push pointer-to-member qualifiers. */
			ddata->push_qualifier = PUSH_ALL_QUALIFIER;
			if (!cpp_demangle_push_type_qualifier(ddata,
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
			if (!cpp_demangle_read_type(ddata, &td))
				return 0;
			if (*ddata->cur == 'E')
				break;
			if (limit++ > CPP_DEMANGLE_TRY_LIMIT)
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
		if (!cpp_demangle_push_type_qualifier(ddata, v, NULL))
			return 0;

		ddata->push_qualifier = PUSH_ALL_QUALIFIER;

		/* Release type qualifier vector. */
		if (!vector_type_qualifier_init(ddata, v))
			return 0;

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

/* read encoding, encoding are function name, data name, special-name */
static int
cpp_demangle_read_encoding(struct cpp_demangle_data *ddata)
{
	char *name, *type, *num_str;
	long offset;

	if (!ddata || *ddata->cur == '\0')
		return 0;

	/* special name */
	switch (SIMPLE_HASH(*ddata->cur, *(ddata->cur + 1))) {
	case SIMPLE_HASH('G', 'A'):
		if (!DEM_PUSH_STR(ddata, "hidden alias for "))
			return 0;
		ddata->cur += 2;
		if (*ddata->cur == '\0')
			return 0;
		return cpp_demangle_read_encoding(ddata);

	case SIMPLE_HASH('G', 'R'):
		if (!DEM_PUSH_STR(ddata, "reference temporary #"))
			return 0;
		ddata->cur += 2;
		if (*ddata->cur == '\0')
			return 0;
		if (!cpp_demangle_read_name_flat(ddata, &name))
			return 0;
		if (!cpp_demangle_read_number_as_string(ddata, &num_str))
			return 0;
		if (!DEM_PUSH_STR(ddata, num_str))
			return 0;
		if (!DEM_PUSH_STR(ddata, " for "))
			return 0;
		if (!DEM_PUSH_STR(ddata, name))
			return 0;
		return 1;

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
		return cpp_demangle_read_encoding(ddata);

	case SIMPLE_HASH('G', 'V'):
		/* sentry object for 1 time init */
		if (!DEM_PUSH_STR(ddata, "guard variable for "))
			return 0;
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
		if (!cpp_demangle_read_offset(ddata))
			return 0;
		if (!cpp_demangle_read_offset(ddata))
			return 0;
		return cpp_demangle_read_encoding(ddata);

	case SIMPLE_HASH('T', 'C'):
		/* construction vtable */
		if (!DEM_PUSH_STR(ddata, "construction vtable for "))
			return 0;
		ddata->cur += 2;
		if (*ddata->cur == '\0')
			return 0;
		if (!cpp_demangle_read_type_flat(ddata, &type))
			return 0;
		if (!cpp_demangle_read_number(ddata, &offset))
			return 0;
		if (*ddata->cur++ != '_')
			return 0;
		if (!cpp_demangle_read_type(ddata, NULL))
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
		return cpp_demangle_read_type(ddata, NULL);

	case SIMPLE_HASH('T', 'h'):
		/* virtual function non-virtual override thunk */
		if (!DEM_PUSH_STR(ddata,
			"virtual function non-virtual override "))
			return 0;
		ddata->cur += 2;
		if (*ddata->cur == '\0')
			return 0;
		if (!cpp_demangle_read_nv_offset(ddata))
			return 0;
		return cpp_demangle_read_encoding(ddata);

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
		return cpp_demangle_read_type(ddata, NULL);

	case SIMPLE_HASH('T', 'J'):
		/* java class */
		if (!DEM_PUSH_STR(ddata, "java Class for "))
			return 0;
		ddata->cur += 2;
		if (*ddata->cur == '\0')
			return 0;
		return cpp_demangle_read_type(ddata, NULL);

	case SIMPLE_HASH('T', 'S'):
		/* RTTI name (NTBS) */
		if (!DEM_PUSH_STR(ddata, "typeinfo name for "))
			return 0;
		ddata->cur += 2;
		if (*ddata->cur == '\0')
			return 0;
		return cpp_demangle_read_type(ddata, NULL);

	case SIMPLE_HASH('T', 'T'):
		/* VTT table */
		if (!DEM_PUSH_STR(ddata, "VTT for "))
			return 0;
		ddata->cur += 2;
		if (*ddata->cur == '\0')
			return 0;
		return cpp_demangle_read_type(ddata, NULL);

	case SIMPLE_HASH('T', 'v'):
		/* virtual function virtual override thunk */
		if (!DEM_PUSH_STR(ddata, "virtual function virtual override "))
			return 0;
		ddata->cur += 2;
		if (*ddata->cur == '\0')
			return 0;
		if (!cpp_demangle_read_v_offset(ddata))
			return 0;
		return cpp_demangle_read_encoding(ddata);

	case SIMPLE_HASH('T', 'V'):
		/* virtual table */
		if (!DEM_PUSH_STR(ddata, "vtable for "))
			return 0;
		ddata->cur += 2;
		if (*ddata->cur == '\0')
			return 0;
		return cpp_demangle_read_type(ddata, NULL);

	case SIMPLE_HASH('T', 'W'):
		/* TLS wrapper function */
		if (!DEM_PUSH_STR(ddata, "TLS wrapper function for "))
			return 0;
		ddata->cur += 2;
		if (*ddata->cur == '\0')
			return 0;
		break;
	}

	return cpp_demangle_read_name(ddata);
}

static int
cpp_demangle_read_local_name(struct cpp_demangle_data *ddata)
{
	struct vector_str local_name;
	struct type_delimit td;
	size_t limit;
	bool more_type;

	if (!ddata)
		return 0;
	if (*(++ddata->cur) == '\0')
		return 0;

	if (!vector_str_init(ddata, &local_name))
		return 0;
	ddata->cur_output = &local_name;

	if (!cpp_demangle_read_encoding(ddata))
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
		if (!cpp_demangle_read_type(ddata, NULL))
			return 0;

		more_type = true;
	}

	/* Now we can push the name after possible return type is handled. */
	if (!vector_str_push_vector(ddata, &ddata->output, &local_name))
		return 0;

	while (*ddata->cur != '\0') {
		if (!cpp_demangle_read_type(ddata, &td))
			return 0;
		if (more_type)
			more_type = false;
		if (*ddata->cur == 'E')
			break;
		if (limit++ > CPP_DEMANGLE_TRY_LIMIT)
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
		if (!cpp_demangle_read_name(ddata))
			return 0;
	}
	if (*ddata->cur == '_') {
		++ddata->cur;
		while (ELFTC_ISDIGIT(*ddata->cur))
			++ddata->cur;
	}

	return 1;
}

static int
cpp_demangle_read_name(struct cpp_demangle_data *ddata)
{
	struct stack_str v;
	struct vector_str *output;
	size_t p_idx, subst_str_len;
	char *subst_str;

	if (!ddata || *ddata->cur == '\0')
		return 0;

	output = ddata->cur_output;

	subst_str = NULL;

	switch (*ddata->cur) {
	case 'S':
		return cpp_demangle_read_subst(ddata);
	case 'N':
		return cpp_demangle_read_nested_name(ddata);
	case 'Z':
		return cpp_demangle_read_local_name(ddata);
	}

	stack_str_init(&v);

	p_idx = output->size;
	if (!cpp_demangle_read_uqname(ddata))
		return 0;
	if (!(subst_str = vector_str_substr(ddata, output, p_idx,
		  output->size - 1, &subst_str_len)))
		return 0;
	if (subst_str_len > 8 && strstr(subst_str, "operator"))
		return 1;
	if (!stack_str_append(ddata, &v, subst_str, subst_str_len))
		return 0;
	if (!cpp_demangle_push_subst(ddata, v.str, v.len))
		return 0;

	if (*ddata->cur == 'I') {
		p_idx = output->size;
		if (!cpp_demangle_read_tmpl_args(ddata))
			return 0;
		if (!(subst_str = vector_str_substr(ddata, output, p_idx,
			  output->size - 1, &subst_str_len)))
			return 0;
		if (!stack_str_append(ddata, &v, subst_str, subst_str_len))
			return 0;
		if (!cpp_demangle_push_subst(ddata, v.str, v.len))
			return 0;
	}

	return 1;
}

static int
cpp_demangle_read_type_flat(struct cpp_demangle_data *ddata, char **str)
{
	struct vector_str *output;
	size_t i, p_idx, idx, type_len;
	char *type;

	output = ddata->cur_output;

	p_idx = output->size;

	if (!cpp_demangle_read_type(ddata, NULL))
		return 0;

	if (!(type = vector_str_substr(ddata, output, p_idx, output->size - 1,
		  &type_len)))
		return 0;

	idx = output->size;
	for (i = p_idx; i < idx; ++i)
		if (!vector_str_pop(output))
			return 0;

	*str = type;

	return 1;
}

static int
cpp_demangle_read_name_flat(struct cpp_demangle_data *ddata, char **str)
{
	struct vector_str *output;
	size_t i, p_idx, idx, name_len;
	char *name;

	output = ddata->cur_output;

	p_idx = output->size;

	if (!cpp_demangle_read_name(ddata))
		return 0;

	if (!(name = vector_str_substr(ddata, output, p_idx, output->size - 1,
		  &name_len)))
		return 0;

	idx = output->size;
	for (i = p_idx; i < idx; ++i)
		if (!vector_str_pop(output))
			return 0;

	*str = name;

	return 1;
}

static int
cpp_demangle_read_nested_name(struct cpp_demangle_data *ddata)
{
	struct stack_str v;
	struct vector_str *output;
	size_t limit, p_idx, subst_str_len;
	char *subst_str;

	if (!ddata || *ddata->cur != 'N')
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
	stack_str_init(&v);

	limit = 0;
	for (;;) {
		p_idx = output->size;
		switch (*ddata->cur) {
		case 'I':
			if (!cpp_demangle_read_tmpl_args(ddata))
				return 0;
			break;
		case 'S':
			if (!cpp_demangle_read_subst(ddata))
				return 0;
			break;
		case 'T':
			if (!cpp_demangle_read_tmpl_param(ddata))
				return 0;
			break;
		default:
			if (!cpp_demangle_read_uqname(ddata))
				return 0;
		}

		if (p_idx == output->size)
			goto next_comp;
		if (!(subst_str = vector_str_substr(ddata, output, p_idx,
			  output->size - 1, &subst_str_len)))
			return 0;
		if (!stack_str_append(ddata, &v, subst_str, subst_str_len))
			return 0;

		if (!cpp_demangle_push_subst(ddata, v.str, v.len))
			return 0;

	next_comp:
		if (*ddata->cur == 'E')
			break;
		else if (*ddata->cur != 'I' && *ddata->cur != 'C' &&
		    *ddata->cur != 'D' && p_idx != output->size) {
			if (!DEM_PUSH_STR(ddata, "::"))
				return 0;
			if (!stack_str_append_str(ddata, &v, "::"))
				return 0;
		}
		if (limit++ > CPP_DEMANGLE_TRY_LIMIT)
			return 0;
	}

	++ddata->cur;
	return 1;
}

static bool
cpp_demangle_strtol(long *res, const char *s, int base)
{
	int c;
	long x = 0;
	while ((c = kBase36[*s++ & 255]) && --c < base)
		if (ckd_mul(&x, x, base) || ckd_add(&x, x, c))
			return false;
	*res = x;
	return true;
}

/*
 * read number
 * number ::= [n] <decimal>
 */
static int
cpp_demangle_read_number(struct cpp_demangle_data *ddata, long *rtn)
{
	long len, negative_factor;

	if (!ddata || !rtn)
		return 0;

	negative_factor = 1;
	if (*ddata->cur == 'n') {
		negative_factor = -1;

		++ddata->cur;
	}
	if (!ELFTC_ISDIGIT(*ddata->cur))
		return 0;

	if (!cpp_demangle_strtol(&len, ddata->cur, 10))
		return 0;

	while (ELFTC_ISDIGIT(*ddata->cur))
		++ddata->cur;

	ASSERT(len >= 0);
	ASSERT(negative_factor == 1 || negative_factor == -1);

	*rtn = len * negative_factor;

	return 1;
}

static int
cpp_demangle_read_number_as_string(struct cpp_demangle_data *ddata, char **str)
{
	long n;
	char buf[21];

	if (!cpp_demangle_read_number(ddata, &n)) {
		*str = NULL;
		return 0;
	}

	FormatInt64(buf, n);
	if (!(*str = dupstr(ddata, buf)))
		return 0;

	return 1;
}

static int
cpp_demangle_read_nv_offset(struct cpp_demangle_data *ddata)
{
	if (!ddata)
		return 0;

	if (!DEM_PUSH_STR(ddata, "offset : "))
		return 0;

	return cpp_demangle_read_offset_number(ddata);
}

/* read offset, offset are nv-offset, v-offset */
static int
cpp_demangle_read_offset(struct cpp_demangle_data *ddata)
{
	if (!ddata)
		return 0;

	if (*ddata->cur == 'h') {
		++ddata->cur;
		return cpp_demangle_read_nv_offset(ddata);
	} else if (*ddata->cur == 'v') {
		++ddata->cur;
		return cpp_demangle_read_v_offset(ddata);
	}

	return 0;
}

static int
cpp_demangle_read_offset_number(struct cpp_demangle_data *ddata)
{
	bool negative;
	const char *start;

	if (!ddata || *ddata->cur == '\0')
		return 0;

	/* offset could be negative */
	if (*ddata->cur == 'n') {
		negative = true;
		start = ddata->cur + 1;
	} else {
		negative = false;
		start = ddata->cur;
	}

	while (*ddata->cur != '_')
		++ddata->cur;

	if (negative && !DEM_PUSH_STR(ddata, "-"))
		return 0;

	ASSERT(start);

	if (!cpp_demangle_push_str(ddata, start, ddata->cur - start))
		return 0;
	if (!DEM_PUSH_STR(ddata, " "))
		return 0;

	++ddata->cur;

	return 1;
}

static int
cpp_demangle_read_pointer_to_member(struct cpp_demangle_data *ddata,
    struct vector_type_qualifier *v)
{
	size_t class_type_len, i, idx, p_idx;
	int p_func_type, rtn;
	char *class_type;

	if (!ddata || *ddata->cur != 'M' || *(++ddata->cur) == '\0')
		return 0;

	p_idx = ddata->output.size;
	if (!cpp_demangle_read_type(ddata, NULL))
		return 0;

	if (!(class_type = vector_str_substr(ddata, &ddata->output, p_idx,
		  ddata->output.size - 1, &class_type_len)))
		return 0;

	rtn = 0;
	idx = ddata->output.size;
	for (i = p_idx; i < idx; ++i)
		if (!vector_str_pop(&ddata->output))
			goto clean1;

	if (!vector_read_cmd_push(ddata, &ddata->cmd, READ_PTRMEM, v))
		goto clean1;

	if (!vector_str_push(ddata, &ddata->class_type, class_type,
		class_type_len))
		goto clean2;

	p_func_type = ddata->func_type;
	if (!cpp_demangle_read_type(ddata, NULL))
		goto clean3;

	if (p_func_type == ddata->func_type) {
		if (!DEM_PUSH_STR(ddata, " "))
			goto clean3;
		if (!cpp_demangle_push_str(ddata, class_type, class_type_len))
			goto clean3;
		if (!DEM_PUSH_STR(ddata, "::*"))
			goto clean3;
	}

	rtn = 1;
clean3:
	if (!vector_str_pop(&ddata->class_type))
		rtn = 0;
clean2:
	if (!vector_read_cmd_pop(&ddata->cmd))
		rtn = 0;
clean1:

	if (!vector_type_qualifier_init(ddata, v))
		return 0;

	return rtn;
}

/* read source-name, source-name is <len> <ID> */
static int
cpp_demangle_read_sname(struct cpp_demangle_data *ddata)
{
	long len;
	int err;

	if (!ddata || !cpp_demangle_read_number(ddata, &len) || len <= 0)
		return 0;

	if (len == 12 && !memcmp("_GLOBAL__N_1", ddata->cur, 12))
		err = DEM_PUSH_STR(ddata, "(anonymous namespace)");
	else
		err = cpp_demangle_push_str(ddata, ddata->cur, len);

	if (!err)
		return 0;

	ASSERT(ddata->cur_output->size > 0);
	if (!vector_read_cmd_find(&ddata->cmd, READ_TMPL))
		ddata->last_sname =
		    ddata->cur_output->container[ddata->cur_output->size - 1];

	ddata->cur += len;

	return 1;
}

static int
cpp_demangle_read_subst(struct cpp_demangle_data *ddata)
{
	long nth;

	if (!ddata || *ddata->cur == '\0')
		return 0;

	/* abbreviations of the form Sx */
	switch (SIMPLE_HASH(*ddata->cur, *(ddata->cur + 1))) {
	case SIMPLE_HASH('S', 'a'):
		/* std::allocator */
		if (!DEM_PUSH_STR(ddata, "std::allocator"))
			return 0;
		ddata->cur += 2;
		if (*ddata->cur == 'I')
			return cpp_demangle_read_subst_stdtmpl(ddata,
			    "std::allocator");
		return 1;

	case SIMPLE_HASH('S', 'b'):
		/* std::basic_string */
		if (!DEM_PUSH_STR(ddata, "std::basic_string"))
			return 0;
		ddata->cur += 2;
		if (*ddata->cur == 'I')
			return cpp_demangle_read_subst_stdtmpl(ddata,
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
			return cpp_demangle_read_subst_stdtmpl(ddata,
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
			return cpp_demangle_read_subst_stdtmpl(ddata,
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
			return cpp_demangle_read_subst_stdtmpl(ddata,
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
			return cpp_demangle_read_subst_stdtmpl(ddata,
			    "std::string");
		return 1;

	case SIMPLE_HASH('S', 't'):
		/* std:: */
		return cpp_demangle_read_subst_std(ddata);
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
		return cpp_demangle_get_subst(ddata, 0);
	else {
		/* substitution number is base 36 */
		if (!cpp_demangle_strtol(&nth, ddata->cur, 36))
			return 0;

		/* first was '_', so increase one */
		++nth;

		while (*ddata->cur != '_')
			++ddata->cur;

		ASSERT(nth > 0);

		return cpp_demangle_get_subst(ddata, nth);
	}

	/* NOTREACHED */
	return 0;
}

static int
cpp_demangle_read_subst_std(struct cpp_demangle_data *ddata)
{
	struct vector_str *output, v;
	size_t p_idx, subst_str_len;
	char *subst_str;

	if (!ddata)
		return 0;

	if (!vector_str_init(ddata, &v))
		return 0;

	subst_str = NULL;
	if (!DEM_PUSH_STR(ddata, "std::"))
		return 0;

	if (!VEC_PUSH_STR(&v, "std::"))
		return 0;

	ddata->cur += 2;

	output = ddata->cur_output;

	p_idx = output->size;
	if (!cpp_demangle_read_uqname(ddata))
		return 0;

	if (!(subst_str = vector_str_substr(ddata, output, p_idx,
		  output->size - 1, &subst_str_len)))
		return 0;

	if (!vector_str_push(ddata, &v, subst_str, subst_str_len))
		return 0;

	if (!cpp_demangle_push_subst_v(ddata, &v))
		return 0;

	if (*ddata->cur == 'I') {
		p_idx = output->size;
		if (!cpp_demangle_read_tmpl_args(ddata))
			return 0;
		if (!(subst_str = vector_str_substr(ddata, output, p_idx,
			  output->size - 1, &subst_str_len)))
			return 0;
		if (!vector_str_push(ddata, &v, subst_str, subst_str_len))
			return 0;
		if (!cpp_demangle_push_subst_v(ddata, &v))
			return 0;
	}

	return 1;
}

static int
cpp_demangle_read_subst_stdtmpl(struct cpp_demangle_data *ddata,
    const char *str)
{
	struct vector_str *output;
	size_t p_idx, substr_len, len;
	char *subst_str, *substr;

	if (!ddata || !str)
		return 0;

	if (!(len = strlen(str)))
		return 0;

	output = ddata->cur_output;

	p_idx = output->size;
	substr = NULL;
	subst_str = NULL;

	if (!cpp_demangle_read_tmpl_args(ddata))
		return 0;
	if (!(substr = vector_str_substr(ddata, output, p_idx, output->size - 1,
		  &substr_len)))
		return 0;

	if (!(subst_str = (char *)alloc(ddata, alignof(char),
		  sizeof(char) * (substr_len + len + 1))))
		return 0;

	memcpy(subst_str, str, len);
	memcpy(subst_str + len, substr, substr_len);
	subst_str[substr_len + len] = '\0';

	if (!cpp_demangle_push_subst(ddata, subst_str, substr_len + len))
		return 0;

	return 1;
}

static int
cpp_demangle_read_tmpl_arg(struct cpp_demangle_data *ddata)
{
	if (!ddata || *ddata->cur == '\0')
		return 0;

	switch (*ddata->cur) {
	case 'L':
		return cpp_demangle_read_expr_primary(ddata);
	case 'X':
		++ddata->cur;
		if (!cpp_demangle_read_expression(ddata))
			return 0;
		return *ddata->cur++ == 'E';
	}

	return cpp_demangle_read_type(ddata, NULL);
}

static int
cpp_demangle_read_tmpl_args(struct cpp_demangle_data *ddata)
{
	struct vector_str *v;
	size_t arg_len, idx, limit;
	char *arg;

	if (!ddata || *ddata->cur == '\0')
		return 0;

	++ddata->cur;

	if (!vector_read_cmd_push(ddata, &ddata->cmd, READ_TMPL, NULL))
		return 0;

	if (!DEM_PUSH_STR(ddata, "<"))
		return 0;

	limit = 0;
	v = ddata->cur_output;
	for (;;) {
		idx = v->size;
		if (!cpp_demangle_read_tmpl_arg(ddata))
			return 0;
		if (!(arg = vector_str_substr(ddata, v, idx, v->size - 1,
			  &arg_len)))
			return 0;
		if (!vector_str_find(&ddata->tmpl, arg, arg_len) &&
		    !vector_str_push(ddata, &ddata->tmpl, arg, arg_len))
			return 0;

		if (*ddata->cur == 'E') {
			++ddata->cur;
			if (!DEM_PUSH_STR(ddata, ">"))
				return 0;
			ddata->is_tmpl = true;
			break;
		} else if (*ddata->cur != 'I' && !DEM_PUSH_STR(ddata, ", "))
			return 0;

		if (limit++ > CPP_DEMANGLE_TRY_LIMIT)
			return 0;
	}

	return vector_read_cmd_pop(&ddata->cmd);
}

/*
 * Read template parameter that forms in 'T[number]_'.
 * This function much like to read_subst but only for types.
 */
static int
cpp_demangle_read_tmpl_param(struct cpp_demangle_data *ddata)
{
	long nth;

	if (!ddata || *ddata->cur != 'T')
		return 0;

	++ddata->cur;

	if (*ddata->cur == '_') {
		return cpp_demangle_get_tmpl_param(ddata, 0);
	} else {
		if (!cpp_demangle_strtol(&nth, ddata->cur, 36))
			return 0;

		/* T_ is first */
		++nth;

		while (*ddata->cur != '_')
			++ddata->cur;

		ASSERT(nth > 0);

		return cpp_demangle_get_tmpl_param(ddata, nth);
	}

	/* NOTREACHED */
	return 0;
}

static int
cpp_demangle_read_type(struct cpp_demangle_data *ddata, struct type_delimit *td)
{
	struct vector_type_qualifier v;
	struct vector_str *output;
	size_t p_idx, type_str_len, subst_str_len;
	int extern_c, is_builtin;
	long len;
	const char *p;
	char *type_str, *exp_str, *num_str, *subst_str;
	bool skip_ref_qualifier, omit_void;

	if (!ddata)
		return 0;

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

	if (!vector_type_qualifier_init(ddata, &v))
		return 0;

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
		if (!cpp_demangle_read_array(ddata))
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
		if (!vector_type_qualifier_push(ddata, &v, TYPE_CMX))
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
				if (!cpp_demangle_read_expression_flat(ddata,
					&exp_str))
					return 0;
				if (!VEC_PUSH_STR(&v.ext_name, exp_str))
					return 0;
			} else {
				if (!cpp_demangle_read_number_as_string(ddata,
					&num_str))
					return 0;
				if (!VEC_PUSH_STR(&v.ext_name, num_str))
					return 0;
			}
			if (*ddata->cur != '_')
				return 0;
			++ddata->cur;
			if (!vector_type_qualifier_push(ddata, &v, TYPE_VEC))
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
		/* unexpected end except ref-qualifiers */
		if (ddata->ref_qualifier && ddata->is_functype) {
			skip_ref_qualifier = true;
			/* Pop the delimiter. */
			cpp_demangle_pop_str(ddata);
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
		if (!cpp_demangle_read_function(ddata, &extern_c, &v))
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
		if (!vector_type_qualifier_push(ddata, &v, TYPE_IMG))
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
		if (!cpp_demangle_read_tmpl_args(ddata))
			return 0;
		if (!(subst_str = vector_str_substr(ddata, output, p_idx,
			  output->size - 1, &subst_str_len)))
			return 0;
		if (!cpp_demangle_push_subst(ddata, subst_str, subst_str_len))
			return 0;
		goto rtn;

	case 'j':
		/* unsigned int */
		if (!DEM_PUSH_STR(ddata, "unsigned int"))
			return 0;
		++ddata->cur;
		goto rtn;

	case 'K':
		/* const */
		if (!vector_type_qualifier_push(ddata, &v, TYPE_CST))
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
		if (!cpp_demangle_read_pointer_to_member(ddata, &v))
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
		if (!vector_type_qualifier_push(ddata, &v, TYPE_RREF))
			return 0;
		ddata->ref_qualifier = true;
		ddata->ref_qualifier_type = TYPE_RREF;
		++ddata->cur;
		if (td)
			td->firstp = false;
		goto again;

	case 'P':
		/* pointer */
		if (!vector_type_qualifier_push(ddata, &v, TYPE_PTR))
			return 0;
		++ddata->cur;
		if (td)
			td->firstp = false;
		goto again;

	case 'r':
		/* restrict */
		if (!vector_type_qualifier_push(ddata, &v, TYPE_RST))
			return 0;
		++ddata->cur;
		if (td)
			td->firstp = false;
		goto again;

	case 'R':
		/* reference */
		if (ddata->ref_qualifier)
			return 0;
		if (!vector_type_qualifier_push(ddata, &v, TYPE_REF))
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
		if (!cpp_demangle_read_subst(ddata))
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
		if (!cpp_demangle_read_tmpl_param(ddata))
			return 0;
		is_builtin = 0;
		goto rtn;

	case 'u':
		/* vendor extended builtin */
		++ddata->cur;
		if (!cpp_demangle_read_sname(ddata))
			return 0;
		is_builtin = 0;
		goto rtn;

	case 'U':
		/* vendor extended type qualifier */
		++ddata->cur;
		if (!cpp_demangle_read_number(ddata, &len))
			return 0;
		if (len <= 0)
			return 0;
		if (!vector_str_push(ddata, &v.ext_name, ddata->cur, len))
			return 0;
		ddata->cur += len;
		if (!vector_type_qualifier_push(ddata, &v, TYPE_EXT))
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
		if (!vector_type_qualifier_push(ddata, &v, TYPE_VAT))
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

	if (!cpp_demangle_read_name(ddata))
		return 0;

	is_builtin = 0;
rtn:

	type_str = vector_str_substr(ddata, output, p_idx, output->size - 1,
	    &type_str_len);

	if (!is_builtin) {
		if (!vector_str_find(&ddata->subst, type_str, type_str_len) &&
		    !vector_str_push(ddata, &ddata->subst, type_str,
			type_str_len))
			return 0;
	}

	if (!skip_ref_qualifier &&
	    !cpp_demangle_push_type_qualifier(ddata, &v, type_str))
		return 0;

	if (td)
		td->firstp = false;

	return 1;
}

/*
 * read unqualified-name, unqualified name are operator-name, ctor-dtor-name,
 * source-name
 */
static int
cpp_demangle_read_uqname(struct cpp_demangle_data *ddata)
{
	size_t len;

	if (!ddata || *ddata->cur == '\0')
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
		return cpp_demangle_read_type(ddata, NULL);

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
		if (!cpp_demangle_push_str(ddata, ddata->cur + 1, 1))
			return 0;
		ddata->cur += 2;
		return cpp_demangle_read_sname(ddata);
	}

	/* ctor-dtor-name */
	switch (SIMPLE_HASH(*ddata->cur, *(ddata->cur + 1))) {
	case SIMPLE_HASH('C', '1'):
	case SIMPLE_HASH('C', '2'):
	case SIMPLE_HASH('C', '3'):
		if (!ddata->last_sname)
			return 0;
		if (!(len = strlen(ddata->last_sname)))
			return 0;
		if (!DEM_PUSH_STR(ddata, "::"))
			return 0;
		if (!cpp_demangle_push_str(ddata, ddata->last_sname, len))
			return 0;
		ddata->cur += 2;
		return 1;

	case SIMPLE_HASH('D', '0'):
	case SIMPLE_HASH('D', '1'):
	case SIMPLE_HASH('D', '2'):
		if (!ddata->last_sname)
			return 0;
		if (!(len = strlen(ddata->last_sname)))
			return 0;
		if (!DEM_PUSH_STR(ddata, "::~"))
			return 0;
		if (!cpp_demangle_push_str(ddata, ddata->last_sname, len))
			return 0;
		ddata->cur += 2;
		return 1;
	}

	/* source name */
	if (ELFTC_ISDIGIT(*ddata->cur))
		return cpp_demangle_read_sname(ddata);

	/* local source name */
	if (*ddata->cur == 'L')
		return cpp_demangle_local_source_name(ddata);

	return 1;
}

/*
 * Read local source name.
 *
 * References:
 *   http://gcc.gnu.org/bugzilla/show_bug.cgi?id=31775
 *   http://gcc.gnu.org/viewcvs?view=rev&revision=124467
 */
static int
cpp_demangle_local_source_name(struct cpp_demangle_data *ddata)
{
	/* L */
	if (!ddata || *ddata->cur != 'L')
		return 0;
	++ddata->cur;

	/* source name */
	if (!cpp_demangle_read_sname(ddata))
		return 0;

	/* discriminator */
	if (*ddata->cur == '_') {
		++ddata->cur;
		while (ELFTC_ISDIGIT(*ddata->cur))
			++ddata->cur;
	}

	return 1;
}

static int
cpp_demangle_read_v_offset(struct cpp_demangle_data *ddata)
{
	if (!ddata)
		return 0;

	if (!DEM_PUSH_STR(ddata, "offset : "))
		return 0;

	if (!cpp_demangle_read_offset_number(ddata))
		return 0;

	if (!DEM_PUSH_STR(ddata, "virtual offset : "))
		return 0;

	return !cpp_demangle_read_offset_number(ddata);
}

static struct read_cmd_item *
vector_read_cmd_find(struct vector_read_cmd *v, enum read_cmd dst)
{
	int i;

	if (!v || dst == READ_FAIL)
		return 0;

	for (i = (int)v->size - 1; i >= 0; i--)
		if (v->r_container[i].cmd == dst)
			return &v->r_container[i];

	return 0;
}

static int
vector_read_cmd_push(struct cpp_demangle_data *ddata, struct vector_read_cmd *v,
    enum read_cmd cmd, void *data)
{
	struct read_cmd_item *tmp_r_ctn;
	size_t tmp_cap;
	size_t i;

	if (!v)
		return 0;

	if (v->size == v->capacity) {
		tmp_cap = BUFFER_GROW(v->capacity);
		if (!(tmp_r_ctn = (struct read_cmd_item *)alloc(ddata,
			  alignof(*tmp_r_ctn), sizeof(*tmp_r_ctn) * tmp_cap)))
			return 0;
		for (i = 0; i < v->size; ++i)
			tmp_r_ctn[i] = v->r_container[i];
		v->r_container = tmp_r_ctn;
		v->capacity = tmp_cap;
	}

	v->r_container[v->size].cmd = cmd;
	v->r_container[v->size].data = data;
	++v->size;

	return 1;
}

/* size, capacity, ext_name */
static int
vector_type_qualifier_init(struct cpp_demangle_data *ddata,
    struct vector_type_qualifier *v)
{
	if (!v)
		return 0;

	v->size = 0;
	v->capacity = VECTOR_DEF_CAPACITY;

	if (!(v->q_container = (enum type_qualifier *)alloc(ddata,
		  alignof(enum type_qualifier),
		  sizeof(enum type_qualifier) * v->capacity)))
		return 0;

	ASSERT(v->q_container);

	if (!vector_str_init(ddata, &v->ext_name))
		return 0;

	return 1;
}

static int
vector_type_qualifier_push(struct cpp_demangle_data *ddata,
    struct vector_type_qualifier *v, enum type_qualifier t)
{
	enum type_qualifier *tmp_ctn;
	size_t tmp_cap;
	size_t i;

	if (!v)
		return 0;

	if (v->size == v->capacity) {
		tmp_cap = BUFFER_GROW(v->capacity);
		if (!(tmp_ctn = (enum type_qualifier *)alloc(ddata,
			  alignof(enum type_qualifier),
			  sizeof(enum type_qualifier) * tmp_cap)))
			return 0;
		for (i = 0; i < v->size; ++i)
			tmp_ctn[i] = v->q_container[i];
		v->q_container = tmp_ctn;
		v->capacity = tmp_cap;
	}

	v->q_container[v->size] = t;
	++v->size;

	return 1;
}

static int
cpp_demangle_copy_output(char *buf, const struct vector_str *v, size_t buflen)
{
	size_t i, j, len = 0;
	if (!v)
		return -1;
	for (i = 0; i < v->size; ++i)
		for (j = 0; v->container[i][j]; ++j) {
			if (len < buflen)
				buf[len] = v->container[i][j];
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

static int
failure(char *buf, const char *org, size_t buflen)
{
	strlcpy(buf, org, buflen);
	return -1;
}

/**
 * Decodes IA-64 C++ ABI style symbol.
 *
 * This function is designed to be safe to call from asynchronous signal
 * handlers, since dynamic memory is allocated from the end of `buf`. As
 * such it's important to provide a very generous amount of memory here.
 *
 * If you expect your symbols to be 100 characters long, you should pass
 * at least 3000 bytes of buffer. If this function runs out of memory it
 * will still create a NUL-terminated string in buf. On complete failure
 * this will simply be the original string copied. On truncation it will
 * be a partially copied result. In both cases, -1 is returned. The size
 * of the output is only returned if this routine is fully succesful. To
 * successfully cover nearly all the test cases from libcxxabi use 65536
 * and to be able to print 99% of the symbols LLVM's libcxx.a, use 5632.
 *
 * It's important to call ismangled() before this, since non-c++ symbols
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
int
__demangle(char *buf, const char *org, size_t buflen)
{
	struct cpp_demangle_data ddata[1];
	struct vector_str ret_type;
	struct type_delimit td;
	ssize_t org_len;
	unsigned int limit;
	bool has_ret = false, more_type = false;

	ddata->heap = (uintptr_t)buf;
	ddata->hoff = buflen;

	org_len = strlen(org);

	/* Try demangling as a type for short encodings. */
	if ((org_len < 2) || (org[0] != '_' || org[1] != 'Z')) {
		if (!cpp_demangle_data_init(ddata, org))
			return failure(buf, org, buflen);
		if (!cpp_demangle_read_type(ddata, 0))
			return failure(buf, org, buflen);
		return cpp_demangle_copy_output(buf, &ddata->output, buflen);
	}
	if (org_len > 11 && !strncmp(org, "_GLOBAL__I_", 11)) {
		if (!vector_str_init(ddata, &ret_type))
			return failure(buf, org, buflen);
		if (!VEC_PUSH_STR(&ret_type, "global constructors keyed to "))
			return failure(buf, org, buflen);
		if (!VEC_PUSH_STR(&ret_type, org + 11))
			return failure(buf, org, buflen);
		return cpp_demangle_copy_output(buf, &ddata->output, buflen);
	}

	if (!cpp_demangle_data_init(ddata, org + 2))
		return failure(buf, org, buflen);

	if (!cpp_demangle_read_encoding(ddata))
		return failure(buf, org, buflen);

	/*
	 * Pop function name from substitution candidate list.
	 */
	if (*ddata->cur && ddata->subst.size >= 1) {
		if (!vector_str_pop(&ddata->subst))
			return failure(buf, org, buflen);
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
		if (!vector_str_init(ddata, &ret_type))
			return failure(buf, org, buflen);
		ddata->cur_output = &ret_type;
		has_ret = true;
	}

	while (*ddata->cur != '\0') {
		/*
		 * Breaking at some gcc info at tail. e.g) @@GLIBCXX_3.4
		 */
		if (*ddata->cur == '@' && *(ddata->cur + 1) == '@')
			break;

		if (has_ret) {
			/* Read return type */
			if (!cpp_demangle_read_type(ddata, NULL))
				return failure(buf, org, buflen);
		} else {
			/* Read function arg type */
			if (!cpp_demangle_read_type(ddata, &td))
				return failure(buf, org, buflen);
		}

		if (has_ret) {
			/* Push return type to the beginning */
			if (!VEC_PUSH_STR(&ret_type, " "))
				return failure(buf, org, buflen);
			if (!vector_str_push_vector_head(ddata, &ddata->output,
				&ret_type))
				return failure(buf, org, buflen);
			ddata->cur_output = &ddata->output;
			has_ret = false;
			more_type = true;
		} else if (more_type) {
			more_type = false;
		}
		if (limit++ > CPP_DEMANGLE_TRY_LIMIT)
			return failure(buf, org, buflen);
	}
	if (more_type)
		return failure(buf, org, buflen);

	if (!ddata->output.size)
		return failure(buf, org, buflen);
	if (td.paren && !VEC_PUSH_STR(&ddata->output, ")"))
		return failure(buf, org, buflen);
	if (ddata->mem_vat && !VEC_PUSH_STR(&ddata->output, " volatile"))
		return failure(buf, org, buflen);
	if (ddata->mem_cst && !VEC_PUSH_STR(&ddata->output, " const"))
		return failure(buf, org, buflen);
	if (ddata->mem_rst && !VEC_PUSH_STR(&ddata->output, " restrict"))
		return failure(buf, org, buflen);
	if (ddata->mem_ref && !VEC_PUSH_STR(&ddata->output, " &"))
		return failure(buf, org, buflen);
	if (ddata->mem_rref && !VEC_PUSH_STR(&ddata->output, " &&"))
		return failure(buf, org, buflen);
	if (ddata->hoff < 32)
		return failure(buf, org, buflen);
	return cpp_demangle_copy_output(buf, &ddata->output, ddata->hoff);
}

/**
 * Returns true if string is mangled by IA-64 C++ ABI style.
 *
 * This means it starts with either "_Z" or "_GLOBAL__I_".
 */
int
__is_mangled(const char *org)
{
	return startswith(org, "_Z") || startswith(org, "_GLOBAL__I_");
}
