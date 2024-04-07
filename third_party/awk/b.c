/****************************************************************
Copyright (C) Lucent Technologies 1997
All Rights Reserved

Permission to use, copy, modify, and distribute this software and
its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appear in all
copies and that both that the copyright notice and this
permission notice and warranty disclaimer appear in supporting
documentation, and that the name Lucent Technologies or any of
its entities not be used in advertising or publicity pertaining
to distribution of the software without specific, written prior
permission.

LUCENT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
IN NO EVENT SHALL LUCENT OR ANY OF ITS ENTITIES BE LIABLE FOR ANY
SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
THIS SOFTWARE.
****************************************************************/

/* lasciate ogne speranza, voi ch'intrate. */

#define	DEBUG

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "awk.h"
#include "awkgram.tab.h"

#define MAXLIN 22

#define type(v)		(v)->nobj	/* badly overloaded here */
#define info(v)		(v)->ntype	/* badly overloaded here */
#define left(v)		(v)->narg[0]
#define right(v)	(v)->narg[1]
#define parent(v)	(v)->nnext

#define LEAF	case CCL: case NCCL: case CHAR: case DOT: case FINAL: case ALL:
#define ELEAF	case EMPTYRE:		/* empty string in regexp */
#define UNARY	case STAR: case PLUS: case QUEST:

/* encoding in tree Nodes:
	leaf (CCL, NCCL, CHAR, DOT, FINAL, ALL, EMPTYRE):
		left is index, right contains value or pointer to value
	unary (STAR, PLUS, QUEST): left is child, right is null
	binary (CAT, OR): left and right are children
	parent contains pointer to parent
*/


int	*setvec;
int	*tmpset;
int	maxsetvec = 0;

int	rtok;		/* next token in current re */
int	rlxval;
static const uschar	*rlxstr;
static const uschar	*prestr;	/* current position in current re */
static const uschar	*lastre;	/* origin of last re */
static const uschar	*lastatom;	/* origin of last Atom */
static const uschar	*starttok;
static const uschar 	*basestr;	/* starts with original, replaced during
				   repetition processing */
static const uschar 	*firstbasestr;

static	int setcnt;
static	int poscnt;

const char	*patbeg;
int	patlen;

#define	NFA	128	/* cache this many dynamic fa's */
fa	*fatab[NFA];
int	nfatab	= 0;	/* entries in fatab */

extern int u8_nextlen(const char *s);


/* utf-8 mechanism:

   For most of Awk, utf-8 strings just "work", since they look like
   null-terminated sequences of 8-bit bytes.

   Functions like length(), index(), and substr() have to operate
   in units of utf-8 characters.  The u8_* functions in run.c
   handle this.

   Regular expressions are more complicated, since the basic
   mechanism of the goto table used 8-bit byte indices into the
   gototab entries to compute the next state.  Unicode is a lot
   bigger, so the gototab entries are now structs with a character
   and a next state. These are sorted by code point and binary
   searched.

   Throughout the RE mechanism in b.c, utf-8 characters are
   converted to their utf-32 value.  This mostly shows up in
   cclenter, which expands character class ranges like a-z and now
   alpha-omega.  The size of a gototab array is still about 256.
   This should be dynamic, but for now things work ok for a single
   code page of Unicode, which is the most likely case.

   The code changes are localized in run.c and b.c.  I have added a
   handful of functions to somewhat better hide the implementation,
   but a lot more could be done.

 */

static int entry_cmp(const void *l, const void *r);
static int get_gototab(fa*, int, int);
static int set_gototab(fa*, int, int, int);
static void clear_gototab(fa*, int);
extern int u8_rune(int *, const char *);

static int *
intalloc(size_t n, const char *f)
{
	int *p = (int *) calloc(n, sizeof(int));
	if (p == NULL)
		overflo(f);
	return p;
}

static void
resizesetvec(const char *f)
{
	if (maxsetvec == 0)
		maxsetvec = MAXLIN;
	else
		maxsetvec *= 4;
	setvec = (int *) realloc(setvec, maxsetvec * sizeof(*setvec));
	tmpset = (int *) realloc(tmpset, maxsetvec * sizeof(*tmpset));
	if (setvec == NULL || tmpset == NULL)
		overflo(f);
}

static void
resize_state(fa *f, int state)
{
	gtt *p;
	uschar *p2;
	int **p3;
	int i, new_count;

	if (++state < f->state_count)
		return;

	new_count = state + 10; /* needs to be tuned */

	p = (gtt *) realloc(f->gototab, new_count * sizeof(gtt));
	if (p == NULL)
		goto out;
	f->gototab = p;

	p2 = (uschar *) realloc(f->out, new_count * sizeof(f->out[0]));
	if (p2 == NULL)
		goto out;
	f->out = p2;

	p3 = (int **) realloc(f->posns, new_count * sizeof(f->posns[0]));
	if (p3 == NULL)
		goto out;
	f->posns = p3;

	for (i = f->state_count; i < new_count; ++i) {
		f->gototab[i].entries = (gtte *) calloc(NCHARS, sizeof(gtte));
		if (f->gototab[i].entries == NULL)
			goto out;
		f->gototab[i].allocated = NCHARS;
		f->gototab[i].inuse = 0;
		f->out[i] = 0;
		f->posns[i] = NULL;
	}
	f->state_count = new_count;
	return;
out:
	overflo(__func__);
}

fa *makedfa(const char *s, bool anchor)	/* returns dfa for reg expr s */
{
	int i, use, nuse;
	fa *pfa;
	static int now = 1;

	if (setvec == NULL) {	/* first time through any RE */
		resizesetvec(__func__);
	}

	if (compile_time != RUNNING)	/* a constant for sure */
		return mkdfa(s, anchor);
	for (i = 0; i < nfatab; i++)	/* is it there already? */
		if (fatab[i]->anchor == anchor
		  && strcmp((const char *) fatab[i]->restr, s) == 0) {
			fatab[i]->use = now++;
			return fatab[i];
		}
	pfa = mkdfa(s, anchor);
	if (nfatab < NFA) {	/* room for another */
		fatab[nfatab] = pfa;
		fatab[nfatab]->use = now++;
		nfatab++;
		return pfa;
	}
	use = fatab[0]->use;	/* replace least-recently used */
	nuse = 0;
	for (i = 1; i < nfatab; i++)
		if (fatab[i]->use < use) {
			use = fatab[i]->use;
			nuse = i;
		}
	freefa(fatab[nuse]);
	fatab[nuse] = pfa;
	pfa->use = now++;
	return pfa;
}

fa *mkdfa(const char *s, bool anchor)	/* does the real work of making a dfa */
				/* anchor = true for anchored matches, else false */
{
	Node *p, *p1;
	fa *f;

	firstbasestr = (const uschar *) s;
	basestr = firstbasestr;
	p = reparse(s);
	p1 = op2(CAT, op2(STAR, op2(ALL, NIL, NIL), NIL), p);
		/* put ALL STAR in front of reg.  exp. */
	p1 = op2(CAT, p1, op2(FINAL, NIL, NIL));
		/* put FINAL after reg.  exp. */

	poscnt = 0;
	penter(p1);	/* enter parent pointers and leaf indices */
	if ((f = (fa *) calloc(1, sizeof(fa) + poscnt * sizeof(rrow))) == NULL)
		overflo(__func__);
	f->accept = poscnt-1;	/* penter has computed number of positions in re */
	cfoll(f, p1);	/* set up follow sets */
	freetr(p1);
	resize_state(f, 1);
	f->posns[0] = intalloc(*(f->re[0].lfollow), __func__);
	f->posns[1] = intalloc(1, __func__);
	*f->posns[1] = 0;
	f->initstat = makeinit(f, anchor);
	f->anchor = anchor;
	f->restr = (uschar *) tostring(s);
	if (firstbasestr != basestr) {
		if (basestr)
			xfree(basestr);
	}
	return f;
}

int makeinit(fa *f, bool anchor)
{
	int i, k;

	f->curstat = 2;
	f->out[2] = 0;
	k = *(f->re[0].lfollow);
	xfree(f->posns[2]);
	f->posns[2] = intalloc(k + 1,  __func__);
	for (i = 0; i <= k; i++) {
		(f->posns[2])[i] = (f->re[0].lfollow)[i];
	}
	if ((f->posns[2])[1] == f->accept)
		f->out[2] = 1;
	clear_gototab(f, 2);
	f->curstat = cgoto(f, 2, HAT);
	if (anchor) {
		*f->posns[2] = k-1;	/* leave out position 0 */
		for (i = 0; i < k; i++) {
			(f->posns[0])[i] = (f->posns[2])[i];
		}

		f->out[0] = f->out[2];
		if (f->curstat != 2)
			--(*f->posns[f->curstat]);
	}
	return f->curstat;
}

void penter(Node *p)	/* set up parent pointers and leaf indices */
{
	switch (type(p)) {
	ELEAF
	LEAF
		info(p) = poscnt;
		poscnt++;
		break;
	UNARY
		penter(left(p));
		parent(left(p)) = p;
		break;
	case CAT:
	case OR:
		penter(left(p));
		penter(right(p));
		parent(left(p)) = p;
		parent(right(p)) = p;
		break;
	case ZERO:
		break;
	default:	/* can't happen */
		FATAL("can't happen: unknown type %d in penter", type(p));
		break;
	}
}

void freetr(Node *p)	/* free parse tree */
{
	switch (type(p)) {
	ELEAF
	LEAF
		xfree(p);
		break;
	UNARY
	case ZERO:
		freetr(left(p));
		xfree(p);
		break;
	case CAT:
	case OR:
		freetr(left(p));
		freetr(right(p));
		xfree(p);
		break;
	default:	/* can't happen */
		FATAL("can't happen: unknown type %d in freetr", type(p));
		break;
	}
}

/* in the parsing of regular expressions, metacharacters like . have */
/* to be seen literally;  \056 is not a metacharacter. */

int hexstr(const uschar **pp, int max)	/* find and eval hex string at pp, return new p */
{			/* only pick up one 8-bit byte (2 chars) */
	const uschar *p;
	int n = 0;
	int i;

	for (i = 0, p = *pp; i < max && isxdigit(*p); i++, p++) {
		if (isdigit((int) *p))
			n = 16 * n + *p - '0';
		else if (*p >= 'a' && *p <= 'f')
			n = 16 * n + *p - 'a' + 10;
		else if (*p >= 'A' && *p <= 'F')
			n = 16 * n + *p - 'A' + 10;
	}
	*pp = p;
	return n;
}



#define isoctdigit(c) ((c) >= '0' && (c) <= '7')	/* multiple use of arg */

int quoted(const uschar **pp)	/* pick up next thing after a \\ */
			/* and increment *pp */
{
	const uschar *p = *pp;
	int c;

/* BUG: should advance by utf-8 char even if makes no sense */

	if ((c = *p++) == 't') {
		c = '\t';
	} else if (c == 'n') {
		c = '\n';
	} else if (c == 'f') {
		c = '\f';
	} else if (c == 'r') {
		c = '\r';
	} else if (c == 'b') {
		c = '\b';
	} else if (c == 'v') {
		c = '\v';
	} else if (c == 'a') {
		c = '\a';
	} else if (c == '\\') {
		c = '\\';
	} else if (c == 'x') {	/* 2 hex digits follow */
		c = hexstr(&p, 2);	/* this adds a null if number is invalid */
	} else if (c == 'u') {	/* unicode char number up to 8 hex digits */
		c = hexstr(&p, 8);
	} else if (isoctdigit(c)) {	/* \d \dd \ddd */
		int n = c - '0';
		if (isoctdigit(*p)) {
			n = 8 * n + *p++ - '0';
			if (isoctdigit(*p))
				n = 8 * n + *p++ - '0';
		}
		c = n;
	} /* else */
		/* c = c; */
	*pp = p;
	return c;
}

int *cclenter(const char *argp)	/* add a character class */
{
	int i, c, c2;
	int n;
	const uschar *p = (const uschar *) argp;
	int *bp, *retp;
	static int *buf = NULL;
	static int bufsz = 100;

	if (buf == NULL && (buf = (int *) calloc(bufsz, sizeof(int))) == NULL)
		FATAL("out of space for character class [%.10s...] 1", p);
	bp = buf;
	for (i = 0; *p != 0; ) {
		n = u8_rune(&c, (const char *) p);
		p += n;
		if (c == '\\') {
			c = quoted(&p);
		} else if (c == '-' && i > 0 && bp[-1] != 0) {
			if (*p != 0) {
				c = bp[-1];
				/* c2 = *p++; */
				n = u8_rune(&c2, (const char *) p);
				p += n;
				if (c2 == '\\')
					c2 = quoted(&p); /* BUG: sets p, has to be u8 size */
				if (c > c2) {	/* empty; ignore */
					bp--;
					i--;
					continue;
				}
				while (c < c2) {
					if (i >= bufsz) {
						bufsz *= 2;
						buf = (int *) realloc(buf, bufsz * sizeof(int));
						if (buf == NULL)
							FATAL("out of space for character class [%.10s...] 2", p);
						bp = buf + i;
					}
					*bp++ = ++c;
					i++;
				}
				continue;
			}
		}
		if (i >= bufsz) {
			bufsz *= 2;
			buf = (int *) realloc(buf, bufsz * sizeof(int));
			if (buf == NULL)
				FATAL("out of space for character class [%.10s...] 2", p);
			bp = buf + i;
		}
		*bp++ = c;
		i++;
	}
	*bp = 0;
	/* DPRINTF("cclenter: in = |%s|, out = |%s|\n", op, buf); BUG: can't print array of int */
	/* xfree(op);  BUG: what are we freeing here? */
	retp = (int *) calloc(bp-buf+1, sizeof(int));
	for (i = 0; i < bp-buf+1; i++)
		retp[i] = buf[i];
	return retp;
}

void overflo(const char *s)
{
	FATAL("regular expression too big: out of space in %.30s...", s);
}

void cfoll(fa *f, Node *v)	/* enter follow set of each leaf of vertex v into lfollow[leaf] */
{
	int i;
	int *p;

	switch (type(v)) {
	ELEAF
	LEAF
		f->re[info(v)].ltype = type(v);
		f->re[info(v)].lval.np = right(v);
		while (f->accept >= maxsetvec) {	/* guessing here! */
			resizesetvec(__func__);
		}
		for (i = 0; i <= f->accept; i++)
			setvec[i] = 0;
		setcnt = 0;
		follow(v);	/* computes setvec and setcnt */
		p = intalloc(setcnt + 1, __func__);
		f->re[info(v)].lfollow = p;
		*p = setcnt;
		for (i = f->accept; i >= 0; i--)
			if (setvec[i] == 1)
				*++p = i;
		break;
	UNARY
		cfoll(f,left(v));
		break;
	case CAT:
	case OR:
		cfoll(f,left(v));
		cfoll(f,right(v));
		break;
	case ZERO:
		break;
	default:	/* can't happen */
		FATAL("can't happen: unknown type %d in cfoll", type(v));
	}
}

int first(Node *p)	/* collects initially active leaves of p into setvec */
			/* returns 0 if p matches empty string */
{
	int b, lp;

	switch (type(p)) {
	ELEAF
	LEAF
		lp = info(p);	/* look for high-water mark of subscripts */
		while (setcnt >= maxsetvec || lp >= maxsetvec) {	/* guessing here! */
			resizesetvec(__func__);
		}
		if (type(p) == EMPTYRE) {
			setvec[lp] = 0;
			return(0);
		}
		if (setvec[lp] != 1) {
			setvec[lp] = 1;
			setcnt++;
		}
		if (type(p) == CCL && (*(int *) right(p)) == 0)
			return(0);		/* empty CCL */
		return(1);
	case PLUS:
		if (first(left(p)) == 0)
			return(0);
		return(1);
	case STAR:
	case QUEST:
		first(left(p));
		return(0);
	case CAT:
		if (first(left(p)) == 0 && first(right(p)) == 0) return(0);
		return(1);
	case OR:
		b = first(right(p));
		if (first(left(p)) == 0 || b == 0) return(0);
		return(1);
	case ZERO:
		return 0;
	}
	FATAL("can't happen: unknown type %d in first", type(p));	/* can't happen */
	return(-1);
}

void follow(Node *v)	/* collects leaves that can follow v into setvec */
{
	Node *p;

	if (type(v) == FINAL)
		return;
	p = parent(v);
	switch (type(p)) {
	case STAR:
	case PLUS:
		first(v);
		follow(p);
		return;

	case OR:
	case QUEST:
		follow(p);
		return;

	case CAT:
		if (v == left(p)) {	/* v is left child of p */
			if (first(right(p)) == 0) {
				follow(p);
				return;
			}
		} else		/* v is right child */
			follow(p);
		return;
	}
}

int member(int c, int *sarg)	/* is c in s? */
{
	int *s = (int *) sarg;

	while (*s)
		if (c == *s++)
			return(1);
	return(0);
}

static void resize_gototab(fa *f, int state)
{
	size_t new_size = f->gototab[state].allocated * 2;
	gtte *p = (gtte *) realloc(f->gototab[state].entries, new_size * sizeof(gtte));
	if (p == NULL)
		overflo(__func__);

	// need to initialized the new memory to zero
	size_t orig_size = f->gototab[state].allocated;		// 2nd half of new mem is this size
	memset(p + orig_size, 0, orig_size * sizeof(gtte));	// clean it out

	f->gototab[state].allocated = new_size;			// update gototab info
	f->gototab[state].entries = p;
}

static int get_gototab(fa *f, int state, int ch) /* hide gototab implementation */
{
	gtte key;
	gtte *item;

	key.ch = ch;
	key.state = 0;	/* irrelevant */
	item = (gtte *) bsearch(& key, f->gototab[state].entries,
			f->gototab[state].inuse, sizeof(gtte),
			entry_cmp);

	if (item == NULL)
		return 0;
	else
		return item->state;
}

static int entry_cmp(const void *l, const void *r)
{
	const gtte *left, *right;

	left = (const gtte *) l;
	right = (const gtte *) r;

	return left->ch - right->ch;
}

static int set_gototab(fa *f, int state, int ch, int val) /* hide gototab implementation */
{
	if (f->gototab[state].inuse == 0) {
		f->gototab[state].entries[0].ch = ch;
		f->gototab[state].entries[0].state = val;
		f->gototab[state].inuse++;
		return val;
	} else if (ch > f->gototab[state].entries[f->gototab[state].inuse-1].ch) {
		// not seen yet, insert and return
		gtt *tab = & f->gototab[state];
		if (tab->inuse + 1 >= tab->allocated)
			resize_gototab(f, state);

		f->gototab[state].entries[f->gototab[state].inuse-1].ch = ch;
		f->gototab[state].entries[f->gototab[state].inuse-1].state = val;
		f->gototab[state].inuse++;
		return val;
	} else {
		// maybe we have it, maybe we don't
		gtte key;
		gtte *item;

		key.ch = ch;
		key.state = 0;	/* irrelevant */
		item = (gtte *) bsearch(& key, f->gototab[state].entries,
				f->gototab[state].inuse, sizeof(gtte),
				entry_cmp);

		if (item != NULL) {
			// we have it, update state and return
			item->state = val;
			return item->state;
		}
		// otherwise, fall through to insert and reallocate.
	}

	gtt *tab = & f->gototab[state];
	if (tab->inuse + 1 >= tab->allocated)
		resize_gototab(f, state);
	++tab->inuse;
	f->gototab[state].entries[tab->inuse].ch = ch;
	f->gototab[state].entries[tab->inuse].state = val;

	qsort(f->gototab[state].entries,
		f->gototab[state].inuse, sizeof(gtte), entry_cmp);

	return val; /* not used anywhere at the moment */
}

static void clear_gototab(fa *f, int state)
{
	memset(f->gototab[state].entries, 0,
		f->gototab[state].allocated * sizeof(gtte));
	f->gototab[state].inuse = 0;
}

int match(fa *f, const char *p0)	/* shortest match ? */
{
	int s, ns;
	int n;
	int rune;
	const uschar *p = (const uschar *) p0;

	/* return pmatch(f, p0); does it matter whether longest or shortest? */

	s = f->initstat;
	assert (s < f->state_count);

	if (f->out[s])
		return(1);
	do {
		/* assert(*p < NCHARS); */
		n = u8_rune(&rune, (const char *) p);
		if ((ns = get_gototab(f, s, rune)) != 0)
			s = ns;
		else
			s = cgoto(f, s, rune);
		if (f->out[s])
			return(1);
		if (*p == 0)
			break;
		p += n;
	} while (1);  /* was *p++ != 0 */
	return(0);
}

int pmatch(fa *f, const char *p0)	/* longest match, for sub */
{
	int s, ns;
	int n;
	int rune;
	const uschar *p = (const uschar *) p0;
	const uschar *q;

	s = f->initstat;
	assert(s < f->state_count);

	patbeg = (const char *)p;
	patlen = -1;
	do {
		q = p;
		do {
			if (f->out[s])		/* final state */
				patlen = q-p;
			/* assert(*q < NCHARS); */
			n = u8_rune(&rune, (const char *) q);
			if ((ns = get_gototab(f, s, rune)) != 0)
				s = ns;
			else
				s = cgoto(f, s, rune);

			assert(s < f->state_count);

			if (s == 1) {	/* no transition */
				if (patlen >= 0) {
					patbeg = (const char *) p;
					return(1);
				}
				else
					goto nextin;	/* no match */
			}
			if (*q == 0)
				break;
			q += n;
		} while (1);
		q++;  /* was *q++ */
		if (f->out[s])
			patlen = q-p-1;	/* don't count $ */
		if (patlen >= 0) {
			patbeg = (const char *) p;
			return(1);
		}
	nextin:
		s = 2;
		if (*p == 0)
			break;
		n = u8_rune(&rune, (const char *) p);
		p += n;
	} while (1); /* was *p++ */
	return (0);
}

int nematch(fa *f, const char *p0)	/* non-empty match, for sub */
{
	int s, ns;
        int n;
        int rune;
	const uschar *p = (const uschar *) p0;
	const uschar *q;

	s = f->initstat;
	assert(s < f->state_count);

	patbeg = (const char *)p;
	patlen = -1;
	while (*p) {
		q = p;
		do {
			if (f->out[s])		/* final state */
				patlen = q-p;
			/* assert(*q < NCHARS); */
			n = u8_rune(&rune, (const char *) q);
			if ((ns = get_gototab(f, s, rune)) != 0)
				s = ns;
			else
				s = cgoto(f, s, rune);
			if (s == 1) {	/* no transition */
				if (patlen > 0) {
					patbeg = (const char *) p;
					return(1);
				} else
					goto nnextin;	/* no nonempty match */
			}
			if (*q == 0)
				break;
			q += n;
		} while (1);
		q++;
		if (f->out[s])
			patlen = q-p-1;	/* don't count $ */
		if (patlen > 0 ) {
			patbeg = (const char *) p;
			return(1);
		}
	nnextin:
		s = 2;
		p++;
	}
	return (0);
}


/*
 * NAME
 *     fnematch
 *
 * DESCRIPTION
 *     A stream-fed version of nematch which transfers characters to a
 *     null-terminated buffer. All characters up to and including the last
 *     character of the matching text or EOF are placed in the buffer. If
 *     a match is found, patbeg and patlen are set appropriately.
 *
 * RETURN VALUES
 *     false    No match found.
 *     true     Match found.
 */

bool fnematch(fa *pfa, FILE *f, char **pbuf, int *pbufsize, int quantum)
{
	char *i, *j, *k, *buf = *pbuf;
	int bufsize = *pbufsize;
	int c, n, ns, s;

	s = pfa->initstat;
	patlen = 0;

	/*
	 * buf <= i <= j <= k <= buf+bufsize
	 *
	 * i: origin of active substring
	 * j: current character
	 * k: destination of the next getc
	 */

	i = j = k = buf;

	do {
		/*
		 * Call u8_rune with at least awk_mb_cur_max ahead in
		 * the buffer until EOF interferes.
		 */
		if (k - j < awk_mb_cur_max) {
			if (k + awk_mb_cur_max > buf + bufsize) {
				char *obuf = buf;
				adjbuf((char **) &buf, &bufsize,
				    bufsize + awk_mb_cur_max,
				    quantum, 0, "fnematch");

				/* buf resized, maybe moved. update pointers */
				*pbufsize = bufsize;
				if (obuf != buf) {
					i = buf + (i - obuf);
					j = buf + (j - obuf);
					k = buf + (k - obuf);
					*pbuf = buf;
					if (patlen)
						patbeg = buf + (patbeg - obuf);
				}
			}
			for (n = awk_mb_cur_max ; n > 0; n--) {
				*k++ = (c = getc(f)) != EOF ? c : 0;
				if (c == EOF) {
					if (ferror(f))
						FATAL("fnematch: getc error");
					break;
				}
			}
		}

		j += u8_rune(&c, j);

		if ((ns = get_gototab(pfa, s, c)) != 0)
			s = ns;
		else
			s = cgoto(pfa, s, c);

		if (pfa->out[s]) {	/* final state */
			patbeg = i;
			patlen = j - i;
			if (c == 0)	/* don't count $ */
				patlen--;
		}

		if (c && s != 1)
			continue;  /* origin i still viable, next j */
		if (patlen)
			break;     /* best match found */

		/* no match at origin i, next i and start over */
		i += u8_rune(&c, i);
		if (c == 0)
			break;    /* no match */
		j = i;
		s = 2;
	} while (1);

	if (patlen) {
		/*
		 * Under no circumstances is the last character fed to
		 * the automaton part of the match. It is EOF's nullbyte,
		 * or it sent the automaton into a state with no further
		 * transitions available (s==1), or both. Room for a
		 * terminating nullbyte is guaranteed.
		 *
		 * ungetc any chars after the end of matching text
		 * (except for EOF's nullbyte, if present) and null
		 * terminate the buffer.
		 */
		do
			if (*--k && ungetc(*k, f) == EOF)
				FATAL("unable to ungetc '%c'", *k);
		while (k > patbeg + patlen);
		*k = '\0';
		return true;
	}
	else
		return false;
}

Node *reparse(const char *p)	/* parses regular expression pointed to by p */
{			/* uses relex() to scan regular expression */
	Node *np;

	DPRINTF("reparse <%s>\n", p);
	lastre = prestr = (const uschar *) p;	/* prestr points to string to be parsed */
	rtok = relex();
	/* GNU compatibility: an empty regexp matches anything */
	if (rtok == '\0') {
		/* FATAL("empty regular expression"); previous */
		return(op2(EMPTYRE, NIL, NIL));
	}
	np = regexp();
	if (rtok != '\0')
		FATAL("syntax error in regular expression %s at %s", lastre, prestr);
	return(np);
}

Node *regexp(void)	/* top-level parse of reg expr */
{
	return (alt(concat(primary())));
}

Node *primary(void)
{
	Node *np;
	int savelastatom;

	switch (rtok) {
	case CHAR:
		lastatom = starttok;
		np = op2(CHAR, NIL, itonp(rlxval));
		rtok = relex();
		return (unary(np));
	case ALL:
		rtok = relex();
		return (unary(op2(ALL, NIL, NIL)));
	case EMPTYRE:
		rtok = relex();
		return (unary(op2(EMPTYRE, NIL, NIL)));
	case DOT:
		lastatom = starttok;
		rtok = relex();
		return (unary(op2(DOT, NIL, NIL)));
	case CCL:
		np = op2(CCL, NIL, (Node*) cclenter((const char *) rlxstr));
		lastatom = starttok;
		rtok = relex();
		return (unary(np));
	case NCCL:
		np = op2(NCCL, NIL, (Node *) cclenter((const char *) rlxstr));
		lastatom = starttok;
		rtok = relex();
		return (unary(np));
	case '^':
		rtok = relex();
		return (unary(op2(CHAR, NIL, itonp(HAT))));
	case '$':
		rtok = relex();
		return (unary(op2(CHAR, NIL, NIL)));
	case '(':
		lastatom = starttok;
		savelastatom = starttok - basestr; /* Retain over recursion */
		rtok = relex();
		if (rtok == ')') {	/* special pleading for () */
			rtok = relex();
			return unary(op2(CCL, NIL, (Node *) cclenter("")));
		}
		np = regexp();
		if (rtok == ')') {
			lastatom = basestr + savelastatom; /* Restore */
			rtok = relex();
			return (unary(np));
		}
		else
			FATAL("syntax error in regular expression %s at %s", lastre, prestr);
	default:
		FATAL("illegal primary in regular expression %s at %s", lastre, prestr);
	}
	return 0;	/*NOTREACHED*/
}

Node *concat(Node *np)
{
	switch (rtok) {
	case CHAR: case DOT: case ALL: case CCL: case NCCL: case '$': case '(':
		return (concat(op2(CAT, np, primary())));
	case EMPTYRE:
		rtok = relex();
		return (concat(op2(CAT, op2(CCL, NIL, (Node *) cclenter("")),
				primary())));
	}
	return (np);
}

Node *alt(Node *np)
{
	if (rtok == OR) {
		rtok = relex();
		return (alt(op2(OR, np, concat(primary()))));
	}
	return (np);
}

Node *unary(Node *np)
{
	switch (rtok) {
	case STAR:
		rtok = relex();
		return (unary(op2(STAR, np, NIL)));
	case PLUS:
		rtok = relex();
		return (unary(op2(PLUS, np, NIL)));
	case QUEST:
		rtok = relex();
		return (unary(op2(QUEST, np, NIL)));
	case ZERO:
		rtok = relex();
		return (unary(op2(ZERO, np, NIL)));
	default:
		return (np);
	}
}

/*
 * Character class definitions conformant to the POSIX locale as
 * defined in IEEE P1003.1 draft 7 of June 2001, assuming the source
 * and operating character sets are both ASCII (ISO646) or supersets
 * thereof.
 *
 * Note that to avoid overflowing the temporary buffer used in
 * relex(), the expanded character class (prior to range expansion)
 * must be less than twice the size of their full name.
 */

/* Because isblank doesn't show up in any of the header files on any
 * system i use, it's defined here.  if some other locale has a richer
 * definition of "blank", define HAS_ISBLANK and provide your own
 * version.
 * the parentheses here are an attempt to find a path through the maze
 * of macro definition and/or function and/or version provided.  thanks
 * to nelson beebe for the suggestion; let's see if it works everywhere.
 */

/* #define HAS_ISBLANK */
#ifndef HAS_ISBLANK

int (xisblank)(int c)
{
	return c==' ' || c=='\t';
}

#endif

static const struct charclass {
	const char *cc_name;
	int cc_namelen;
	int (*cc_func)(int);
} charclasses[] = {
	{ "alnum",	5,	isalnum },
	{ "alpha",	5,	isalpha },
#ifndef HAS_ISBLANK
	{ "blank",	5,	xisblank },
#else
	{ "blank",	5,	isblank },
#endif
	{ "cntrl",	5,	iscntrl },
	{ "digit",	5,	isdigit },
	{ "graph",	5,	isgraph },
	{ "lower",	5,	islower },
	{ "print",	5,	isprint },
	{ "punct",	5,	ispunct },
	{ "space",	5,	isspace },
	{ "upper",	5,	isupper },
	{ "xdigit",	6,	isxdigit },
	{ NULL,		0,	NULL },
};

#define REPEAT_SIMPLE		0
#define REPEAT_PLUS_APPENDED	1
#define REPEAT_WITH_Q		2
#define REPEAT_ZERO		3

static int
replace_repeat(const uschar *reptok, int reptoklen, const uschar *atom,
	       int atomlen, int firstnum, int secondnum, int special_case)
{
	int i, j;
	uschar *buf = 0;
	int ret = 1;
	int init_q = (firstnum == 0);		/* first added char will be ? */
	int n_q_reps = secondnum-firstnum;	/* m>n, so reduce until {1,m-n} left  */
	int prefix_length = reptok - basestr;	/* prefix includes first rep	*/
	int suffix_length = strlen((const char *) reptok) - reptoklen;	/* string after rep specifier	*/
	int size = prefix_length +  suffix_length;

	if (firstnum > 1) {	/* add room for reps 2 through firstnum */
		size += atomlen*(firstnum-1);
	}

	/* Adjust size of buffer for special cases */
	if (special_case == REPEAT_PLUS_APPENDED) {
		size++;		/* for the final + */
	} else if (special_case == REPEAT_WITH_Q) {
		size += init_q + (atomlen+1)* (n_q_reps-init_q);
	} else if (special_case == REPEAT_ZERO) {
		size += 2;	/* just a null ERE: () */
	}
	if ((buf = (uschar *) malloc(size + 1)) == NULL)
		FATAL("out of space in reg expr %.10s..", lastre);
	memcpy(buf, basestr, prefix_length);	/* copy prefix	*/
	j = prefix_length;
	if (special_case == REPEAT_ZERO) {
		j -= atomlen;
		buf[j++] = '(';
		buf[j++] = ')';
	}
	for (i = 1; i < firstnum; i++) {	/* copy x reps 	*/
		memcpy(&buf[j], atom, atomlen);
		j += atomlen;
	}
	if (special_case == REPEAT_PLUS_APPENDED) {
		buf[j++] = '+';
	} else if (special_case == REPEAT_WITH_Q) {
		if (init_q)
			buf[j++] = '?';
		for (i = init_q; i < n_q_reps; i++) {	/* copy x? reps */
			memcpy(&buf[j], atom, atomlen);
			j += atomlen;
			buf[j++] = '?';
		}
	}
	memcpy(&buf[j], reptok+reptoklen, suffix_length);
	j += suffix_length;
	buf[j] = '\0';
	/* free old basestr */
	if (firstbasestr != basestr) {
		if (basestr)
			xfree(basestr);
	}
	basestr = buf;
	prestr  = buf + prefix_length;
	if (special_case == REPEAT_ZERO) {
		prestr  -= atomlen;
		ret++;
	}
	return ret;
}

static int repeat(const uschar *reptok, int reptoklen, const uschar *atom,
		  int atomlen, int firstnum, int secondnum)
{
	/*
	   In general, the repetition specifier or "bound" is replaced here
	   by an equivalent ERE string, repeating the immediately previous atom
	   and appending ? and + as needed. Note that the first copy of the
	   atom is left in place, except in the special_case of a zero-repeat
	   (i.e., {0}).
	 */
	if (secondnum < 0) {	/* means {n,} -> repeat n-1 times followed by PLUS */
		if (firstnum < 2) {
			/* 0 or 1: should be handled before you get here */
			FATAL("internal error");
		} else {
			return replace_repeat(reptok, reptoklen, atom, atomlen,
				firstnum, secondnum, REPEAT_PLUS_APPENDED);
		}
	} else if (firstnum == secondnum) {	/* {n} or {n,n} -> simply repeat n-1 times */
		if (firstnum == 0) {	/* {0} or {0,0} */
			/* This case is unusual because the resulting
			   replacement string might actually be SMALLER than
			   the original ERE */
			return replace_repeat(reptok, reptoklen, atom, atomlen,
					firstnum, secondnum, REPEAT_ZERO);
		} else {		/* (firstnum >= 1) */
			return replace_repeat(reptok, reptoklen, atom, atomlen,
					firstnum, secondnum, REPEAT_SIMPLE);
		}
	} else if (firstnum < secondnum) {	/* {n,m} -> repeat n-1 times then alternate  */
		/*  x{n,m}  =>  xx...x{1, m-n+1}  =>  xx...x?x?x?..x?	*/
		return replace_repeat(reptok, reptoklen, atom, atomlen,
					firstnum, secondnum, REPEAT_WITH_Q);
	} else {	/* Error - shouldn't be here (n>m) */
		FATAL("internal error");
	}
	return 0;
}

int relex(void)		/* lexical analyzer for reparse */
{
	int c, n;
	int cflag;
	static uschar *buf = NULL;
	static int bufsz = 100;
	uschar *bp;
	const struct charclass *cc;
	int i;
	int num, m;
	bool commafound, digitfound;
	const uschar *startreptok;
	static int parens = 0;

rescan:
	starttok = prestr;

	if ((n = u8_rune(&rlxval, (const char *) prestr)) > 1) {
		prestr += n;
		starttok = prestr;
		return CHAR;
	}

	switch (c = *prestr++) {
	case '|': return OR;
	case '*': return STAR;
	case '+': return PLUS;
	case '?': return QUEST;
	case '.': return DOT;
	case '\0': prestr--; return '\0';
	case '^':
	case '$':
		return c;
	case '(':
		parens++;
		return c;
	case ')':
		if (parens) {
			parens--;
			return c;
		}
		/* unmatched close parenthesis; per POSIX, treat as literal */
		rlxval = c;
		return CHAR;
	case '\\':
		rlxval = quoted(&prestr);
		return CHAR;
	default:
		rlxval = c;
		return CHAR;
	case '[':
		if (buf == NULL && (buf = (uschar *) malloc(bufsz)) == NULL)
			FATAL("out of space in reg expr %.10s..", lastre);
		bp = buf;
		if (*prestr == '^') {
			cflag = 1;
			prestr++;
		}
		else
			cflag = 0;
		n = 5 * strlen((const char *) prestr)+1; /* BUG: was 2.  what value? */
		if (!adjbuf((char **) &buf, &bufsz, n, n, (char **) &bp, "relex1"))
			FATAL("out of space for reg expr %.10s...", lastre);
		for (; ; ) {
			if ((n = u8_rune(&rlxval, (const char *) prestr)) > 1) {
				for (i = 0; i < n; i++)
					*bp++ = *prestr++;
				continue;
			}
			if ((c = *prestr++) == '\\') {
				*bp++ = '\\';
				if ((c = *prestr++) == '\0')
					FATAL("nonterminated character class %.20s...", lastre);
				*bp++ = c;
			/* } else if (c == '\n') { */
			/* 	FATAL("newline in character class %.20s...", lastre); */
			} else if (c == '[' && *prestr == ':') {
				/* POSIX char class names, Dag-Erling Smorgrav, des@ofug.org */
				for (cc = charclasses; cc->cc_name; cc++)
					if (strncmp((const char *) prestr + 1, (const char *) cc->cc_name, cc->cc_namelen) == 0)
						break;
				if (cc->cc_name != NULL && prestr[1 + cc->cc_namelen] == ':' &&
				    prestr[2 + cc->cc_namelen] == ']') {
					prestr += cc->cc_namelen + 3;
					/*
					 * BUG: We begin at 1, instead of 0, since we
					 * would otherwise prematurely terminate the
					 * string for classes like [[:cntrl:]]. This
					 * means that we can't match the NUL character,
					 * not without first adapting the entire
					 * program to track each string's length.
					 */
					for (i = 1; i <= UCHAR_MAX; i++) {
						if (!adjbuf((char **) &buf, &bufsz, bp-buf+2, 100, (char **) &bp, "relex2"))
						    FATAL("out of space for reg expr %.10s...", lastre);
						if (cc->cc_func(i)) {
							/* escape backslash */
							if (i == '\\') {
								*bp++ = '\\';
								n++;
							}

							*bp++ = i;
							n++;
						}
					}
				} else
					*bp++ = c;
			} else if (c == '[' && *prestr == '.') {
				char collate_char;
				prestr++;
				collate_char = *prestr++;
				if (*prestr == '.' && prestr[1] == ']') {
					prestr += 2;
					/* Found it: map via locale TBD: for
					   now, simply return this char.  This
					   is sufficient to pass conformance
					   test awk.ex 156
					 */
					if (*prestr == ']') {
						prestr++;
						rlxval = collate_char;
						return CHAR;
					}
				}
			} else if (c == '[' && *prestr == '=') {
				char equiv_char;
				prestr++;
				equiv_char = *prestr++;
				if (*prestr == '=' && prestr[1] == ']') {
					prestr += 2;
					/* Found it: map via locale TBD: for now
					   simply return this char. This is
					   sufficient to pass conformance test
					   awk.ex 156
					 */
					if (*prestr == ']') {
						prestr++;
						rlxval = equiv_char;
						return CHAR;
					}
				}
			} else if (c == '\0') {
				FATAL("nonterminated character class %.20s", lastre);
			} else if (bp == buf) {	/* 1st char is special */
				*bp++ = c;
			} else if (c == ']') {
				*bp++ = 0;
				rlxstr = (uschar *) tostring((char *) buf);
				if (cflag == 0)
					return CCL;
				else
					return NCCL;
			} else
				*bp++ = c;
		}
		break;
	case '{':
		if (isdigit((int) *(prestr))) {
			num = 0;	/* Process as a repetition */
			n = -1; m = -1;
			commafound = false;
			digitfound = false;
			startreptok = prestr-1;
			/* Remember start of previous atom here ? */
		} else {        	/* just a { char, not a repetition */
			rlxval = c;
			return CHAR;
                }
		for (; ; ) {
			if ((c = *prestr++) == '}') {
				if (commafound) {
					if (digitfound) { /* {n,m} */
						m = num;
						if (m < n)
							FATAL("illegal repetition expression: class %.20s",
								lastre);
						if (n == 0 && m == 1) {
							return QUEST;
						}
					} else {	/* {n,} */
						if (n == 0)
							return STAR;
						else if (n == 1)
							return PLUS;
					}
				} else {
					if (digitfound) { /* {n} same as {n,n} */
						n = num;
						m = num;
					} else {	/* {} */
						FATAL("illegal repetition expression: class %.20s",
							lastre);
					}
				}
				if (repeat(starttok, prestr-starttok, lastatom,
					   startreptok - lastatom, n, m) > 0) {
					if (n == 0 && m == 0) {
						return ZERO;
					}
					/* must rescan input for next token */
					goto rescan;
				}
				/* Failed to replace: eat up {...} characters
				   and treat like just PLUS */
				return PLUS;
			} else if (c == '\0') {
				FATAL("nonterminated character class %.20s",
					lastre);
			} else if (isdigit(c)) {
				num = 10 * num + c - '0';
				digitfound = true;
			} else if (c == ',') {
				if (commafound)
					FATAL("illegal repetition expression: class %.20s",
						lastre);
				/* looking for {n,} or {n,m} */
				commafound = true;
				n = num;
				digitfound = false; /* reset */
				num = 0;
			} else {
				FATAL("illegal repetition expression: class %.20s",
					lastre);
			}
		}
		break;
	}
}

int cgoto(fa *f, int s, int c)
{
	int *p, *q;
	int i, j, k;

	/* assert(c == HAT || c < NCHARS);  BUG: seg fault if disable test */
	while (f->accept >= maxsetvec) {	/* guessing here! */
		resizesetvec(__func__);
	}
	for (i = 0; i <= f->accept; i++)
		setvec[i] = 0;
	setcnt = 0;
	resize_state(f, s);
	/* compute positions of gototab[s,c] into setvec */
	p = f->posns[s];
	for (i = 1; i <= *p; i++) {
		if ((k = f->re[p[i]].ltype) != FINAL) {
			if ((k == CHAR && c == ptoi(f->re[p[i]].lval.np))
			 || (k == DOT && c != 0 && c != HAT)
			 || (k == ALL && c != 0)
			 || (k == EMPTYRE && c != 0)
			 || (k == CCL && member(c, (int *) f->re[p[i]].lval.rp))
			 || (k == NCCL && !member(c, (int *) f->re[p[i]].lval.rp) && c != 0 && c != HAT)) {
				q = f->re[p[i]].lfollow;
				for (j = 1; j <= *q; j++) {
					if (q[j] >= maxsetvec) {
						resizesetvec(__func__);
					}
					if (setvec[q[j]] == 0) {
						setcnt++;
						setvec[q[j]] = 1;
					}
				}
			}
		}
	}
	/* determine if setvec is a previous state */
	tmpset[0] = setcnt;
	j = 1;
	for (i = f->accept; i >= 0; i--)
		if (setvec[i]) {
			tmpset[j++] = i;
		}
	resize_state(f, f->curstat > s ? f->curstat : s);
	/* tmpset == previous state? */
	for (i = 1; i <= f->curstat; i++) {
		p = f->posns[i];
		if ((k = tmpset[0]) != p[0])
			goto different;
		for (j = 1; j <= k; j++)
			if (tmpset[j] != p[j])
				goto different;
		/* setvec is state i */
		if (c != HAT)
			set_gototab(f, s, c, i);
		return i;
	  different:;
	}

	/* add tmpset to current set of states */
	++(f->curstat);
	resize_state(f, f->curstat);
	clear_gototab(f, f->curstat);
	xfree(f->posns[f->curstat]);
	p = intalloc(setcnt + 1, __func__);

	f->posns[f->curstat] = p;
	if (c != HAT)
		set_gototab(f, s, c, f->curstat);
	for (i = 0; i <= setcnt; i++)
		p[i] = tmpset[i];
	if (setvec[f->accept])
		f->out[f->curstat] = 1;
	else
		f->out[f->curstat] = 0;
	return f->curstat;
}


void freefa(fa *f)	/* free a finite automaton */
{
	int i;

	if (f == NULL)
		return;
	for (i = 0; i < f->state_count; i++)
		xfree(f->gototab[i].entries);
	xfree(f->gototab);
	for (i = 0; i <= f->curstat; i++)
		xfree(f->posns[i]);
	for (i = 0; i <= f->accept; i++) {
		xfree(f->re[i].lfollow);
		if (f->re[i].ltype == CCL || f->re[i].ltype == NCCL)
			xfree(f->re[i].lval.np);
	}
	xfree(f->restr);
	xfree(f->out);
	xfree(f->posns);
	xfree(f->gototab);
	xfree(f);
}
