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

#define DEBUG
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <wctype.h>
#include <fcntl.h>
#include <setjmp.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "awk.h"
#include "awkgram.tab.h"


static void stdinit(void);
static void flush_all(void);
static char *wide_char_to_byte_str(int rune, size_t *outlen);

#if 1
#define tempfree(x)	do { if (istemp(x)) tfree(x); } while (/*CONSTCOND*/0)
#else
void tempfree(Cell *p) {
	if (p->ctype == OCELL && (p->csub < CUNK || p->csub > CFREE)) {
		WARNING("bad csub %d in Cell %d %s",
			p->csub, p->ctype, p->sval);
	}
	if (istemp(p))
		tfree(p);
}
#endif

/* do we really need these? */
/* #ifdef _NFILE */
/* #ifndef FOPEN_MAX */
/* #define FOPEN_MAX _NFILE */
/* #endif */
/* #endif */
/*  */
/* #ifndef	FOPEN_MAX */
/* #define	FOPEN_MAX	40 */	/* max number of open files */
/* #endif */
/*  */
/* #ifndef RAND_MAX */
/* #define RAND_MAX	32767 */	/* all that ansi guarantees */
/* #endif */

jmp_buf env;
extern	int	pairstack[];
extern	Awkfloat	srand_seed;

Node	*winner = NULL;	/* root of parse tree */
Cell	*tmps;		/* free temporary cells for execution */

static Cell	truecell	={ OBOOL, BTRUE, 0, 0, 1.0, NUM, NULL, NULL };
Cell	*True	= &truecell;
static Cell	falsecell	={ OBOOL, BFALSE, 0, 0, 0.0, NUM, NULL, NULL };
Cell	*False	= &falsecell;
static Cell	breakcell	={ OJUMP, JBREAK, 0, 0, 0.0, NUM, NULL, NULL };
Cell	*jbreak	= &breakcell;
static Cell	contcell	={ OJUMP, JCONT, 0, 0, 0.0, NUM, NULL, NULL };
Cell	*jcont	= &contcell;
static Cell	nextcell	={ OJUMP, JNEXT, 0, 0, 0.0, NUM, NULL, NULL };
Cell	*jnext	= &nextcell;
static Cell	nextfilecell	={ OJUMP, JNEXTFILE, 0, 0, 0.0, NUM, NULL, NULL };
Cell	*jnextfile	= &nextfilecell;
static Cell	exitcell	={ OJUMP, JEXIT, 0, 0, 0.0, NUM, NULL, NULL };
Cell	*jexit	= &exitcell;
static Cell	retcell		={ OJUMP, JRET, 0, 0, 0.0, NUM, NULL, NULL };
Cell	*jret	= &retcell;
static Cell	tempcell	={ OCELL, CTEMP, 0, EMPTY, 0.0, NUM|STR|DONTFREE, NULL, NULL };

Node	*curnode = NULL;	/* the node being executed, for debugging */

/* buffer memory management */
int adjbuf(char **pbuf, int *psiz, int minlen, int quantum, char **pbptr,
	const char *whatrtn)
/* pbuf:    address of pointer to buffer being managed
 * psiz:    address of buffer size variable
 * minlen:  minimum length of buffer needed
 * quantum: buffer size quantum
 * pbptr:   address of movable pointer into buffer, or 0 if none
 * whatrtn: name of the calling routine if failure should cause fatal error
 *
 * return   0 for realloc failure, !=0 for success
 */
{
	if (minlen > *psiz) {
		char *tbuf;
		int rminlen = quantum ? minlen % quantum : 0;
		int boff = pbptr ? *pbptr - *pbuf : 0;
		/* round up to next multiple of quantum */
		if (rminlen)
			minlen += quantum - rminlen;
		tbuf = (char *) realloc(*pbuf, minlen);
		DPRINTF("adjbuf %s: %d %d (pbuf=%p, tbuf=%p)\n", whatrtn, *psiz, minlen, (void*)*pbuf, (void*)tbuf);
		if (tbuf == NULL) {
			if (whatrtn)
				FATAL("out of memory in %s", whatrtn);
			return 0;
		}
		*pbuf = tbuf;
		*psiz = minlen;
		if (pbptr)
			*pbptr = tbuf + boff;
	}
	return 1;
}

void run(Node *a)	/* execution of parse tree starts here */
{

	stdinit();
	execute(a);
	closeall();
}

Cell *execute(Node *u)	/* execute a node of the parse tree */
{
	Cell *(*proc)(Node **, int);
	Cell *x;
	Node *a;

	if (u == NULL)
		return(True);
	for (a = u; ; a = a->nnext) {
		curnode = a;
		if (isvalue(a)) {
			x = (Cell *) (a->narg[0]);
			if (isfld(x) && !donefld)
				fldbld();
			else if (isrec(x) && !donerec)
				recbld();
			return(x);
		}
		if (notlegal(a->nobj))	/* probably a Cell* but too risky to print */
			FATAL("illegal statement");
		proc = proctab[a->nobj-FIRSTTOKEN];
		x = (*proc)(a->narg, a->nobj);
		if (isfld(x) && !donefld)
			fldbld();
		else if (isrec(x) && !donerec)
			recbld();
		if (isexpr(a))
			return(x);
		if (isjump(x))
			return(x);
		if (a->nnext == NULL)
			return(x);
		tempfree(x);
	}
}


Cell *program(Node **a, int n)	/* execute an awk program */
{				/* a[0] = BEGIN, a[1] = body, a[2] = END */
	Cell *x;

	if (setjmp(env) != 0)
		goto ex;
	if (a[0]) {		/* BEGIN */
		x = execute(a[0]);
		if (isexit(x))
			return(True);
		if (isjump(x))
			FATAL("illegal break, continue, next or nextfile from BEGIN");
		tempfree(x);
	}
	if (a[1] || a[2])
		while (getrec(&record, &recsize, true) > 0) {
			x = execute(a[1]);
			if (isexit(x))
				break;
			tempfree(x);
		}
  ex:
	if (setjmp(env) != 0)	/* handles exit within END */
		goto ex1;
	if (a[2]) {		/* END */
		x = execute(a[2]);
		if (isbreak(x) || isnext(x) || iscont(x))
			FATAL("illegal break, continue, next or nextfile from END");
		tempfree(x);
	}
  ex1:
	return(True);
}

struct Frame {	/* stack frame for awk function calls */
	int nargs;	/* number of arguments in this call */
	Cell *fcncell;	/* pointer to Cell for function */
	Cell **args;	/* pointer to array of arguments after execute */
	Cell *retval;	/* return value */
};

#define	NARGS	50	/* max args in a call */

struct Frame *frame = NULL;	/* base of stack frames; dynamically allocated */
int	nframe = 0;		/* number of frames allocated */
struct Frame *frp = NULL;	/* frame pointer. bottom level unused */

Cell *call(Node **a, int n)	/* function call.  very kludgy and fragile */
{
	static const Cell newcopycell = { OCELL, CCOPY, 0, EMPTY, 0.0, NUM|STR|DONTFREE, NULL, NULL };
	int i, ncall, ndef;
	int freed = 0; /* handles potential double freeing when fcn & param share a tempcell */
	Node *x;
	Cell *args[NARGS], *oargs[NARGS];	/* BUG: fixed size arrays */
	Cell *y, *z, *fcn;
	char *s;

	fcn = execute(a[0]);	/* the function itself */
	s = fcn->nval;
	if (!isfcn(fcn))
		FATAL("calling undefined function %s", s);
	if (frame == NULL) {
		frp = frame = (struct Frame *) calloc(nframe += 100, sizeof(*frame));
		if (frame == NULL)
			FATAL("out of space for stack frames calling %s", s);
	}
	for (ncall = 0, x = a[1]; x != NULL; x = x->nnext)	/* args in call */
		ncall++;
	ndef = (int) fcn->fval;			/* args in defn */
	DPRINTF("calling %s, %d args (%d in defn), frp=%d\n", s, ncall, ndef, (int) (frp-frame));
	if (ncall > ndef)
		WARNING("function %s called with %d args, uses only %d",
			s, ncall, ndef);
	if (ncall + ndef > NARGS)
		FATAL("function %s has %d arguments, limit %d", s, ncall+ndef, NARGS);
	for (i = 0, x = a[1]; x != NULL; i++, x = x->nnext) {	/* get call args */
		DPRINTF("evaluate args[%d], frp=%d:\n", i, (int) (frp-frame));
		y = execute(x);
		oargs[i] = y;
		DPRINTF("args[%d]: %s %f <%s>, t=%o\n",
			i, NN(y->nval), y->fval, isarr(y) ? "(array)" : NN(y->sval), y->tval);
		if (isfcn(y))
			FATAL("can't use function %s as argument in %s", y->nval, s);
		if (isarr(y))
			args[i] = y;	/* arrays by ref */
		else
			args[i] = copycell(y);
		tempfree(y);
	}
	for ( ; i < ndef; i++) {	/* add null args for ones not provided */
		args[i] = gettemp();
		*args[i] = newcopycell;
	}
	frp++;	/* now ok to up frame */
	if (frp >= frame + nframe) {
		int dfp = frp - frame;	/* old index */
		frame = (struct Frame *) realloc(frame, (nframe += 100) * sizeof(*frame));
		if (frame == NULL)
			FATAL("out of space for stack frames in %s", s);
		frp = frame + dfp;
	}
	frp->fcncell = fcn;
	frp->args = args;
	frp->nargs = ndef;	/* number defined with (excess are locals) */
	frp->retval = gettemp();

	DPRINTF("start exec of %s, frp=%d\n", s, (int) (frp-frame));
	y = execute((Node *)(fcn->sval));	/* execute body */
	DPRINTF("finished exec of %s, frp=%d\n", s, (int) (frp-frame));

	for (i = 0; i < ndef; i++) {
		Cell *t = frp->args[i];
		if (isarr(t)) {
			if (t->csub == CCOPY) {
				if (i >= ncall) {
					freesymtab(t);
					t->csub = CTEMP;
					tempfree(t);
				} else {
					oargs[i]->tval = t->tval;
					oargs[i]->tval &= ~(STR|NUM|DONTFREE);
					oargs[i]->sval = t->sval;
					tempfree(t);
				}
			}
		} else if (t != y) {	/* kludge to prevent freeing twice */
			t->csub = CTEMP;
			tempfree(t);
		} else if (t == y && t->csub == CCOPY) {
			t->csub = CTEMP;
			tempfree(t);
			freed = 1;
		}
	}
	tempfree(fcn);
	if (isexit(y) || isnext(y))
		return y;
	if (freed == 0) {
		tempfree(y);	/* don't free twice! */
	}
	z = frp->retval;			/* return value */
	DPRINTF("%s returns %g |%s| %o\n", s, getfval(z), getsval(z), z->tval);
	frp--;
	return(z);
}

Cell *copycell(Cell *x)	/* make a copy of a cell in a temp */
{
	Cell *y;

	/* copy is not constant or field */

	y = gettemp();
	y->tval = x->tval & ~(CON|FLD|REC);
	y->csub = CCOPY;	/* prevents freeing until call is over */
	y->nval = x->nval;	/* BUG? */
	if (isstr(x) /* || x->ctype == OCELL */) {
		y->sval = tostring(x->sval);
		y->tval &= ~DONTFREE;
	} else
		y->tval |= DONTFREE;
	y->fval = x->fval;
	return y;
}

Cell *arg(Node **a, int n)	/* nth argument of a function */
{

	n = ptoi(a[0]);	/* argument number, counting from 0 */
	DPRINTF("arg(%d), frp->nargs=%d\n", n, frp->nargs);
	if (n+1 > frp->nargs)
		FATAL("argument #%d of function %s was not supplied",
			n+1, frp->fcncell->nval);
	return frp->args[n];
}

Cell *jump(Node **a, int n)	/* break, continue, next, nextfile, return */
{
	Cell *y;

	switch (n) {
	case EXIT:
		if (a[0] != NULL) {
			y = execute(a[0]);
			errorflag = (int) getfval(y);
			tempfree(y);
		}
		longjmp(env, 1);
	case RETURN:
		if (a[0] != NULL) {
			y = execute(a[0]);
			if ((y->tval & (STR|NUM)) == (STR|NUM)) {
				setsval(frp->retval, getsval(y));
				frp->retval->fval = getfval(y);
				frp->retval->tval |= NUM;
			}
			else if (y->tval & STR)
				setsval(frp->retval, getsval(y));
			else if (y->tval & NUM)
				setfval(frp->retval, getfval(y));
			else		/* can't happen */
				FATAL("bad type variable %d", y->tval);
			tempfree(y);
		}
		return(jret);
	case NEXT:
		return(jnext);
	case NEXTFILE:
		nextfile();
		return(jnextfile);
	case BREAK:
		return(jbreak);
	case CONTINUE:
		return(jcont);
	default:	/* can't happen */
		FATAL("illegal jump type %d", n);
	}
	return 0;	/* not reached */
}

Cell *awkgetline(Node **a, int n)	/* get next line from specific input */
{		/* a[0] is variable, a[1] is operator, a[2] is filename */
	Cell *r, *x;
	extern Cell **fldtab;
	FILE *fp;
	char *buf;
	int bufsize = recsize;
	int mode;
	bool newflag;
	double result;

	if ((buf = (char *) malloc(bufsize)) == NULL)
		FATAL("out of memory in getline");

	fflush(stdout);	/* in case someone is waiting for a prompt */
	r = gettemp();
	if (a[1] != NULL) {		/* getline < file */
		x = execute(a[2]);		/* filename */
		mode = ptoi(a[1]);
		if (mode == '|')		/* input pipe */
			mode = LE;	/* arbitrary flag */
		fp = openfile(mode, getsval(x), &newflag);
		tempfree(x);
		if (fp == NULL)
			n = -1;
		else
			n = readrec(&buf, &bufsize, fp, newflag);
		if (n <= 0) {
			;
		} else if (a[0] != NULL) {	/* getline var <file */
			x = execute(a[0]);
			setsval(x, buf);
			if (is_number(x->sval, & result)) {
				x->fval = result;
				x->tval |= NUM;
			}
			tempfree(x);
		} else {			/* getline <file */
			setsval(fldtab[0], buf);
			if (is_number(fldtab[0]->sval, & result)) {
				fldtab[0]->fval = result;
				fldtab[0]->tval |= NUM;
			}
		}
	} else {			/* bare getline; use current input */
		if (a[0] == NULL)	/* getline */
			n = getrec(&record, &recsize, true);
		else {			/* getline var */
			n = getrec(&buf, &bufsize, false);
			if (n > 0) {
				x = execute(a[0]);
				setsval(x, buf);
				if (is_number(x->sval, & result)) {
					x->fval = result;
					x->tval |= NUM;
				}
				tempfree(x);
			}
		}
	}
	setfval(r, (Awkfloat) n);
	free(buf);
	return r;
}

Cell *getnf(Node **a, int n)	/* get NF */
{
	if (!donefld)
		fldbld();
	return (Cell *) a[0];
}

static char *
makearraystring(Node *p, const char *func)
{
	char *buf;
	int bufsz = recsize;
	size_t blen;

	if ((buf = (char *) malloc(bufsz)) == NULL) {
		FATAL("%s: out of memory", func);
	}

	blen = 0;
	buf[blen] = '\0';

	for (; p; p = p->nnext) {
		Cell *x = execute(p);	/* expr */
		char *s = getsval(x);
		size_t seplen = strlen(getsval(subseploc));
		size_t nsub = p->nnext ? seplen : 0;
		size_t slen = strlen(s);
		size_t tlen = blen + slen + nsub;

		if (!adjbuf(&buf, &bufsz, tlen + 1, recsize, 0, func)) {
			FATAL("%s: out of memory %s[%s...]",
			      func ? func : "NULL", x->nval, buf);
		}
		memcpy(buf + blen, s, slen);
		if (nsub) {
			memcpy(buf + blen + slen, *SUBSEP, nsub);
		}
		buf[tlen] = '\0';
		blen = tlen;
		tempfree(x);
	}
	return buf;
}

Cell *array(Node **a, int n)	/* a[0] is symtab, a[1] is list of subscripts */
{
	Cell *x, *z;
	char *buf;

	x = execute(a[0]);	/* Cell* for symbol table */
	buf = makearraystring(a[1], __func__);
	if (!isarr(x)) {
		DPRINTF("making %s into an array\n", NN(x->nval));
		if (freeable(x))
			xfree(x->sval);
		x->tval &= ~(STR|NUM|DONTFREE);
		x->tval |= ARR;
		x->sval = (char *) makesymtab(NSYMTAB);
	}
	z = setsymtab(buf, "", 0.0, STR|NUM, (Array *) x->sval);
	z->ctype = OCELL;
	z->csub = CVAR;
	tempfree(x);
	free(buf);
	return(z);
}

Cell *awkdelete(Node **a, int n)	/* a[0] is symtab, a[1] is list of subscripts */
{
	Cell *x;

	x = execute(a[0]);	/* Cell* for symbol table */
	if (x == symtabloc) {
		FATAL("cannot delete SYMTAB or its elements");
	}
	if (!isarr(x))
		return True;
	if (a[1] == NULL) {	/* delete the elements, not the table */
		freesymtab(x);
		x->tval &= ~STR;
		x->tval |= ARR;
		x->sval = (char *) makesymtab(NSYMTAB);
	} else {
		char *buf = makearraystring(a[1], __func__);
		freeelem(x, buf);
		free(buf);
	}
	tempfree(x);
	return True;
}

Cell *intest(Node **a, int n)	/* a[0] is index (list), a[1] is symtab */
{
	Cell *ap, *k;
	char *buf;

	ap = execute(a[1]);	/* array name */
	if (!isarr(ap)) {
		DPRINTF("making %s into an array\n", ap->nval);
		if (freeable(ap))
			xfree(ap->sval);
		ap->tval &= ~(STR|NUM|DONTFREE);
		ap->tval |= ARR;
		ap->sval = (char *) makesymtab(NSYMTAB);
	}
	buf = makearraystring(a[0], __func__);
	k = lookup(buf, (Array *) ap->sval);
	tempfree(ap);
	free(buf);
	if (k == NULL)
		return(False);
	else
		return(True);
}


/* ======== utf-8 code ========== */

/*
 * Awk strings can contain ascii, random 8-bit items (eg Latin-1),
 * or utf-8.  u8_isutf tests whether a string starts with a valid
 * utf-8 sequence, and returns 0 if not (e.g., high bit set).
 * u8_nextlen returns length of next valid sequence, which is
 * 1 for ascii, 2..4 for utf-8, or 1 for high bit non-utf.
 * u8_strlen returns length of string in valid utf-8 sequences
 * and/or high-bit bytes.  Conversion functions go between byte
 * number and character number.
 *
 * In theory, this behaves the same as before for non-utf8 bytes.
 *
 * Limited checking! This is a potential security hole.
 */

/* is s the beginning of a valid utf-8 string? */
/* return length 1..4 if yes, 0 if no */
int u8_isutf(const char *s)
{
	int n, ret;
	unsigned char c;

	c = s[0];
	if (c < 128 || awk_mb_cur_max == 1)
		return 1; /* what if it's 0? */

	n = strlen(s);
	if (n >= 2 && ((c>>5) & 0x7) == 0x6 && (s[1] & 0xC0) == 0x80) {
		ret = 2; /* 110xxxxx 10xxxxxx */
	} else if (n >= 3 && ((c>>4) & 0xF) == 0xE && (s[1] & 0xC0) == 0x80
			 && (s[2] & 0xC0) == 0x80) {
		ret = 3; /* 1110xxxx 10xxxxxx 10xxxxxx */
	} else if (n >= 4 && ((c>>3) & 0x1F) == 0x1E && (s[1] & 0xC0) == 0x80
			 && (s[2] & 0xC0) == 0x80 && (s[3] & 0xC0) == 0x80) {
		ret = 4; /* 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
	} else {
		ret = 0;
	}
	return ret;
}

/* Convert (prefix of) utf8 string to utf-32 rune. */
/* Sets *rune to the value, returns the length. */
/* No error checking: watch out. */
int u8_rune(int *rune, const char *s)
{
	int n, ret;
	unsigned char c;

	c = s[0];
	if (c < 128 || awk_mb_cur_max == 1) {
		*rune = c;
		return 1;
	}

	n = strlen(s);
	if (n >= 2 && ((c>>5) & 0x7) == 0x6 && (s[1] & 0xC0) == 0x80) {
		*rune = ((c & 0x1F) << 6) | (s[1] & 0x3F); /* 110xxxxx 10xxxxxx */
		ret = 2;
	} else if (n >= 3 && ((c>>4) & 0xF) == 0xE && (s[1] & 0xC0) == 0x80
			  && (s[2] & 0xC0) == 0x80) {
		*rune = ((c & 0xF) << 12) | ((s[1] & 0x3F) << 6) | (s[2] & 0x3F);
			/* 1110xxxx 10xxxxxx 10xxxxxx */
		ret = 3;
	} else if (n >= 4 && ((c>>3) & 0x1F) == 0x1E && (s[1] & 0xC0) == 0x80
			  && (s[2] & 0xC0) == 0x80 && (s[3] & 0xC0) == 0x80) {
		*rune = ((c & 0x7) << 18) | ((s[1] & 0x3F) << 12) | ((s[2] & 0x3F) << 6) | (s[3] & 0x3F);
			/* 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
		ret = 4;
	} else {
		*rune = c;
		ret = 1;
	}
	return ret; /* returns one byte if sequence doesn't look like utf */
}

/* return length of next sequence: 1 for ascii or random, 2..4 for valid utf8 */
int u8_nextlen(const char *s)
{
	int len;

	len = u8_isutf(s);
	if (len == 0)
		len = 1;
	return len;
}

/* return number of utf characters or single non-utf bytes */
int u8_strlen(const char *s)
{
	int i, len, n, totlen;
	unsigned char c;

	n = strlen(s);
	totlen = 0;
	for (i = 0; i < n; i += len) {
		c = s[i];
		if (c < 128 || awk_mb_cur_max == 1) {
			len = 1;
		} else {
			len = u8_nextlen(&s[i]);
		}
		totlen++;
		if (i > n)
			FATAL("bad utf count [%s] n=%d i=%d\n", s, n, i);
	}
	return totlen;
}

/* convert utf-8 char number in a string to its byte offset */
int u8_char2byte(const char *s, int charnum)
{
	int n;
	int bytenum = 0;

	while (charnum > 0) {
		n = u8_nextlen(s);
		s += n;
		bytenum += n;
		charnum--;
	}
	return bytenum;
}

/* convert byte offset in s to utf-8 char number that starts there */
int u8_byte2char(const char *s, int bytenum)
{
	int i, len, b;
	int charnum = 0; /* BUG: what origin? */
	/* should be 0 to match start==0 which means no match */	

	b = strlen(s);
	if (bytenum > b) {
		return -1; /* ??? */
	}
	for (i = 0; i <= bytenum; i += len) {
		len = u8_nextlen(s+i);
		charnum++;
	}
	return charnum;
}

/* runetochar() adapted from rune.c in the Plan 9 distributione */

enum
{
	Runeerror = 128, /* from somewhere else */
	Runemax = 0x10FFFF,

	Bit1    = 7,
	Bitx    = 6,
	Bit2    = 5,
	Bit3    = 4,
	Bit4    = 3,
	Bit5    = 2,

	T1      = ((1<<(Bit1+1))-1) ^ 0xFF,     /* 0000 0000 */
	Tx      = ((1<<(Bitx+1))-1) ^ 0xFF,     /* 1000 0000 */
	T2      = ((1<<(Bit2+1))-1) ^ 0xFF,     /* 1100 0000 */
	T3      = ((1<<(Bit3+1))-1) ^ 0xFF,     /* 1110 0000 */
	T4      = ((1<<(Bit4+1))-1) ^ 0xFF,     /* 1111 0000 */
	T5      = ((1<<(Bit5+1))-1) ^ 0xFF,     /* 1111 1000 */

	Rune1   = (1<<(Bit1+0*Bitx))-1,	 	/* 0000 0000 0000 0000 0111 1111 */
	Rune2   = (1<<(Bit2+1*Bitx))-1,	 	/* 0000 0000 0000 0111 1111 1111 */
	Rune3   = (1<<(Bit3+2*Bitx))-1,	 	/* 0000 0000 1111 1111 1111 1111 */
	Rune4   = (1<<(Bit4+3*Bitx))-1,	 	/* 0011 1111 1111 1111 1111 1111 */

	Maskx   = (1<<Bitx)-1,		  	/* 0011 1111 */
	Testx   = Maskx ^ 0xFF,		 	/* 1100 0000 */

};

int runetochar(char *str, int c)
{	
	/* one character sequence 00000-0007F => 00-7F */     
	if (c <= Rune1) {
		str[0] = c;
		return 1;
	}
	
	/* two character sequence 00080-007FF => T2 Tx */
	if (c <= Rune2) {
		str[0] = T2 | (c >> 1*Bitx);
		str[1] = Tx | (c & Maskx);
		return 2;
	}

	/* three character sequence 00800-0FFFF => T3 Tx Tx */
	if (c > Runemax)
		c = Runeerror;
	if (c <= Rune3) {
		str[0] = T3 |  (c >> 2*Bitx);
		str[1] = Tx | ((c >> 1*Bitx) & Maskx);
		str[2] = Tx |  (c & Maskx);
		return 3;
	}
	
	/* four character sequence 010000-1FFFFF => T4 Tx Tx Tx */
	str[0] = T4 |  (c >> 3*Bitx);
	str[1] = Tx | ((c >> 2*Bitx) & Maskx);
	str[2] = Tx | ((c >> 1*Bitx) & Maskx);
	str[3] = Tx |  (c & Maskx);
	return 4;
}               


/* ========== end of utf8 code =========== */



Cell *matchop(Node **a, int n)	/* ~ and match() */
{
	Cell *x, *y, *z;
	char *s, *t;
	int i;
	int cstart, cpatlen, len;
	fa *pfa;
	int (*mf)(fa *, const char *) = match, mode = 0;

	if (n == MATCHFCN) {
		mf = pmatch;
		mode = 1;
	}
	x = execute(a[1]);	/* a[1] = target text */
	s = getsval(x);
	if (a[0] == NULL)	/* a[1] == 0: already-compiled reg expr */
		i = (*mf)((fa *) a[2], s);
	else {
		y = execute(a[2]);	/* a[2] = regular expr */
		t = getsval(y);
		pfa = makedfa(t, mode);
		i = (*mf)(pfa, s);
		tempfree(y);
	}
	z = x;
	if (n == MATCHFCN) {
		int start = patbeg - s + 1; /* origin 1 */
		if (patlen < 0) {
			start = 0; /* not found */
		} else {
			cstart = u8_byte2char(s, start-1);
			cpatlen = 0;
			for (i = 0; i < patlen; i += len) {
				len = u8_nextlen(patbeg+i);
				cpatlen++;
			}

			start = cstart;
			patlen = cpatlen;
		}

		setfval(rstartloc, (Awkfloat) start);
		setfval(rlengthloc, (Awkfloat) patlen);
		x = gettemp();
		x->tval = NUM;
		x->fval = start;
	} else if ((n == MATCH && i == 1) || (n == NOTMATCH && i == 0))
		x = True;
	else
		x = False;

	tempfree(z);
	return x;
}


Cell *boolop(Node **a, int n)	/* a[0] || a[1], a[0] && a[1], !a[0] */
{
	Cell *x, *y;
	int i;

	x = execute(a[0]);
	i = istrue(x);
	tempfree(x);
	switch (n) {
	case BOR:
		if (i) return(True);
		y = execute(a[1]);
		i = istrue(y);
		tempfree(y);
		if (i) return(True);
		else return(False);
	case AND:
		if ( !i ) return(False);
		y = execute(a[1]);
		i = istrue(y);
		tempfree(y);
		if (i) return(True);
		else return(False);
	case NOT:
		if (i) return(False);
		else return(True);
	default:	/* can't happen */
		FATAL("unknown boolean operator %d", n);
	}
	return 0;	/*NOTREACHED*/
}

Cell *relop(Node **a, int n)	/* a[0 < a[1], etc. */
{
	int i;
	Cell *x, *y;
	Awkfloat j;
	bool x_is_nan, y_is_nan;

	x = execute(a[0]);
	y = execute(a[1]);
	x_is_nan = isnan(x->fval);
	y_is_nan = isnan(y->fval);
	if (x->tval&NUM && y->tval&NUM) {
		if ((x_is_nan || y_is_nan) && n != NE)
			return(False);
		j = x->fval - y->fval;
		i = j<0? -1: (j>0? 1: 0);
	} else {
		i = strcmp(getsval(x), getsval(y));
	}
	tempfree(x);
	tempfree(y);
	switch (n) {
	case LT:	if (i<0) return(True);
			else return(False);
	case LE:	if (i<=0) return(True);
			else return(False);
	case NE:	if (x_is_nan && y_is_nan) return(True);
			else if (i!=0) return(True);
			else return(False);
	case EQ:	if (i == 0) return(True);
			else return(False);
	case GE:	if (i>=0) return(True);
			else return(False);
	case GT:	if (i>0) return(True);
			else return(False);
	default:	/* can't happen */
		FATAL("unknown relational operator %d", n);
	}
	return 0;	/*NOTREACHED*/
}

void tfree(Cell *a)	/* free a tempcell */
{
	if (freeable(a)) {
		DPRINTF("freeing %s %s %o\n", NN(a->nval), NN(a->sval), a->tval);
		xfree(a->sval);
	}
	if (a == tmps)
		FATAL("tempcell list is curdled");
	a->cnext = tmps;
	tmps = a;
}

Cell *gettemp(void)	/* get a tempcell */
{	int i;
	Cell *x;

	if (!tmps) {
		tmps = (Cell *) calloc(100, sizeof(*tmps));
		if (!tmps)
			FATAL("out of space for temporaries");
		for (i = 1; i < 100; i++)
			tmps[i-1].cnext = &tmps[i];
		tmps[i-1].cnext = NULL;
	}
	x = tmps;
	tmps = x->cnext;
	*x = tempcell;
	return(x);
}

Cell *indirect(Node **a, int n)	/* $( a[0] ) */
{
	Awkfloat val;
	Cell *x;
	int m;
	char *s;

	x = execute(a[0]);
	val = getfval(x);	/* freebsd: defend against super large field numbers */
	if ((Awkfloat)INT_MAX < val)
		FATAL("trying to access out of range field %s", x->nval);
	m = (int) val;
	if (m == 0 && !is_number(s = getsval(x), NULL))	/* suspicion! */
		FATAL("illegal field $(%s), name \"%s\"", s, x->nval);
		/* BUG: can x->nval ever be null??? */
	tempfree(x);
	x = fieldadr(m);
	x->ctype = OCELL;	/* BUG?  why are these needed? */
	x->csub = CFLD;
	return(x);
}

Cell *substr(Node **a, int nnn)		/* substr(a[0], a[1], a[2]) */
{
	int k, m, n;
	int mb, nb;
	char *s;
	int temp;
	Cell *x, *y, *z = NULL;

	x = execute(a[0]);
	y = execute(a[1]);
	if (a[2] != NULL)
		z = execute(a[2]);
	s = getsval(x);
	k = u8_strlen(s) + 1;
	if (k <= 1) {
		tempfree(x);
		tempfree(y);
		if (a[2] != NULL) {
			tempfree(z);
		}
		x = gettemp();
		setsval(x, "");
		return(x);
	}
	m = (int) getfval(y);
	if (m <= 0)
		m = 1;
	else if (m > k)
		m = k;
	tempfree(y);
	if (a[2] != NULL) {
		n = (int) getfval(z);
		tempfree(z);
	} else
		n = k - 1;
	if (n < 0)
		n = 0;
	else if (n > k - m)
		n = k - m;
	/* m is start, n is length from there */
	DPRINTF("substr: m=%d, n=%d, s=%s\n", m, n, s);
	y = gettemp();
	mb = u8_char2byte(s, m-1); /* byte offset of start char in s */
	nb = u8_char2byte(s, m-1+n);  /* byte offset of end+1 char in s */

	temp = s[nb];	/* with thanks to John Linderman */
	s[nb] = '\0';
	setsval(y, s + mb);
	s[nb] = temp;
	tempfree(x);
	return(y);
}

Cell *sindex(Node **a, int nnn)		/* index(a[0], a[1]) */
{
	Cell *x, *y, *z;
	char *s1, *s2, *p1, *p2, *q;
	Awkfloat v = 0.0;

	x = execute(a[0]);
	s1 = getsval(x);
	y = execute(a[1]);
	s2 = getsval(y);

	z = gettemp();
	for (p1 = s1; *p1 != '\0'; p1++) {
		for (q = p1, p2 = s2; *p2 != '\0' && *q == *p2; q++, p2++)
			continue;
		if (*p2 == '\0') {
			/* v = (Awkfloat) (p1 - s1 + 1);	 origin 1 */

		   /* should be a function: used in match() as well */
			int i, len;
			v = 0;
			for (i = 0; i < p1-s1+1; i += len) {
				len = u8_nextlen(s1+i);
				v++;
			}
			break;
		}
	}
	tempfree(x);
	tempfree(y);
	setfval(z, v);
	return(z);
}

int has_utf8(char *s)	/* return 1 if s contains any utf-8 (2 bytes or more) character */
{
	int n;

	for (n = 0; *s != 0; s += n) {
		n = u8_nextlen(s);
		if (n > 1)
			return 1;
	}
	return 0;
}

#define	MAXNUMSIZE	50

int format(char **pbuf, int *pbufsize, const char *s, Node *a)	/* printf-like conversions */
{
	char *fmt;
	char *p, *t;
	const char *os;
	Cell *x;
	int flag = 0, n;
	int fmtwd; /* format width */
	int fmtsz = recsize;
	char *buf = *pbuf;
	int bufsize = *pbufsize;
#define FMTSZ(a)   (fmtsz - ((a) - fmt))
#define BUFSZ(a)   (bufsize - ((a) - buf))

	static bool first = true;
	static bool have_a_format = false;

	if (first) {
		char xbuf[100];

		snprintf(xbuf, sizeof(xbuf), "%a", 42.0);
		have_a_format = (strcmp(xbuf, "0x1.5p+5") == 0);
		first = false;
	}

	os = s;
	p = buf;
	if ((fmt = (char *) malloc(fmtsz)) == NULL)
		FATAL("out of memory in format()");
	while (*s) {
		adjbuf(&buf, &bufsize, MAXNUMSIZE+1+p-buf, recsize, &p, "format1");
		if (*s != '%') {
			*p++ = *s++;
			continue;
		}
		if (*(s+1) == '%') {
			*p++ = '%';
			s += 2;
			continue;
		}
		fmtwd = atoi(s+1);
		if (fmtwd < 0)
			fmtwd = -fmtwd;
		adjbuf(&buf, &bufsize, fmtwd+1+p-buf, recsize, &p, "format2");
		for (t = fmt; (*t++ = *s) != '\0'; s++) {
			if (!adjbuf(&fmt, &fmtsz, MAXNUMSIZE+1+t-fmt, recsize, &t, "format3"))
				FATAL("format item %.30s... ran format() out of memory", os);
			/* Ignore size specifiers */
			if (strchr("hjLlqtz", *s) != NULL) {	/* the ansi panoply */
				t--;
				continue;
			}
			if (isalpha((uschar)*s))
				break;
			if (*s == '$') {
				FATAL("'$' not permitted in awk formats");
			}
			if (*s == '*') {
				if (a == NULL) {
					FATAL("not enough args in printf(%s)", os);
				}
				x = execute(a);
				a = a->nnext;
				snprintf(t - 1, FMTSZ(t - 1),
				    "%d", fmtwd=(int) getfval(x));
				if (fmtwd < 0)
					fmtwd = -fmtwd;
				adjbuf(&buf, &bufsize, fmtwd+1+p-buf, recsize, &p, "format");
				t = fmt + strlen(fmt);
				tempfree(x);
			}
		}
		*t = '\0';
		if (fmtwd < 0)
			fmtwd = -fmtwd;
		adjbuf(&buf, &bufsize, fmtwd+1+p-buf, recsize, &p, "format4");
		switch (*s) {
		case 'a': case 'A':
			if (have_a_format)
				flag = *s;
			else
				flag = 'f';
			break;
		case 'f': case 'e': case 'g': case 'E': case 'G':
			flag = 'f';
			break;
		case 'd': case 'i': case 'o': case 'x': case 'X': case 'u':
			flag = (*s == 'd' || *s == 'i') ? 'd' : 'u';
			*(t-1) = 'j';
			*t = *s;
			*++t = '\0';
			break;
		case 's':
			flag = 's';
			break;
		case 'c':
			flag = 'c';
			break;
		default:
			WARNING("weird printf conversion %s", fmt);
			flag = '?';
			break;
		}
		if (a == NULL)
			FATAL("not enough args in printf(%s)", os);
		x = execute(a);
		a = a->nnext;
		n = MAXNUMSIZE;
		if (fmtwd > n)
			n = fmtwd;
		adjbuf(&buf, &bufsize, 1+n+p-buf, recsize, &p, "format5");
		switch (flag) {
		case '?':
			snprintf(p, BUFSZ(p), "%s", fmt);	/* unknown, so dump it too */
			t = getsval(x);
			n = strlen(t);
			if (fmtwd > n)
				n = fmtwd;
			adjbuf(&buf, &bufsize, 1+strlen(p)+n+p-buf, recsize, &p, "format6");
			p += strlen(p);
			snprintf(p, BUFSZ(p), "%s", t);
			break;
		case 'a':
		case 'A':
		case 'f':	snprintf(p, BUFSZ(p), fmt, getfval(x)); break;
		case 'd':	snprintf(p, BUFSZ(p), fmt, (intmax_t) getfval(x)); break;
		case 'u':	snprintf(p, BUFSZ(p), fmt, (uintmax_t) getfval(x)); break;

		case 's': {
			t = getsval(x);
			n = strlen(t);
			/* if simple format or no utf-8 in the string, sprintf works */
			if (!has_utf8(t) || strcmp(fmt,"%s") == 0) {
				if (fmtwd > n)
					n = fmtwd;
				if (!adjbuf(&buf, &bufsize, 1+n+p-buf, recsize, &p, "format7"))
					FATAL("huge string/format (%d chars) in printf %.30s..." \
						" ran format() out of memory", n, t);
				snprintf(p, BUFSZ(p), fmt, t);
				break;
			}

			/* get here if string has utf-8 chars and fmt is not plain %s */
			/* "%-w.ps", where -, w and .p are all optional */
			/* '0' before the w is a flag character */
			/* fmt points at % */
			int ljust = 0, wid = 0, prec = n, pad = 0;
			char *f = fmt+1;
			if (f[0] == '-') {
				ljust = 1;
				f++;
			}
			// flags '0' and '+' are recognized but skipped
			if (f[0] == '0') {
				f++;
				if (f[0] == '+')
					f++;
			}
			if (f[0] == '+') {
				f++;
				if (f[0] == '0')
					f++;
			}
			if (isdigit(f[0])) { /* there is a wid */
				wid = strtol(f, &f, 10);
			}
			if (f[0] == '.') { /* there is a .prec */
				prec = strtol(++f, &f, 10);
			}
			if (prec > u8_strlen(t))
				prec = u8_strlen(t);
			pad = wid>prec ? wid - prec : 0;  // has to be >= 0
			int i, k, n;
			
			if (ljust) { // print prec chars from t, then pad blanks
				n = u8_char2byte(t, prec);
				for (k = 0; k < n; k++) {
					//putchar(t[k]);
					*p++ = t[k];
				}
				for (i = 0; i < pad; i++) {
					//printf(" ");
					*p++ = ' ';
				}
			} else { // print pad blanks, then prec chars from t
				for (i = 0; i < pad; i++) {
					//printf(" ");
					*p++ = ' ';
				}
				n = u8_char2byte(t, prec);
				for (k = 0; k < n; k++) {
					//putchar(t[k]);
					*p++ = t[k];
				}
			}
			*p = 0;
			break;
		}

               case 'c': {
			/*
			 * If a numeric value is given, awk should just turn
			 * it into a character and print it:
			 *      BEGIN { printf("%c\n", 65) }
			 * prints "A".
			 *
			 * But what if the numeric value is > 128 and
			 * represents a valid Unicode code point?!? We do
			 * our best to convert it back into UTF-8. If we
			 * can't, we output the encoding of the Unicode
			 * "invalid character", 0xFFFD.
			 */
			if (isnum(x)) {
				int charval = (int) getfval(x);

				if (charval != 0) {
					if (charval < 128 || awk_mb_cur_max == 1)
						snprintf(p, BUFSZ(p), fmt, charval);
					else {
						// possible unicode character
						size_t count;
						char *bs = wide_char_to_byte_str(charval, &count);

						if (bs == NULL)	{ // invalid character
							// use unicode invalid character, 0xFFFD
							static char invalid_char[] = "\357\277\275";
							bs = invalid_char;
							count = 3;
						}
						t = bs;
						n = count;
						goto format_percent_c;
					}
				} else {
					*p++ = '\0'; /* explicit null byte */
					*p = '\0';   /* next output will start here */
				}
				break;
			}
			t = getsval(x);
			n = u8_nextlen(t);
		format_percent_c:
			if (n < 2) { /* not utf8 */
				snprintf(p, BUFSZ(p), fmt, getsval(x)[0]);
				break;
			}

			// utf8 character, almost same song and dance as for %s
			int ljust = 0, wid = 0, prec = n, pad = 0;
			char *f = fmt+1;
			if (f[0] == '-') {
				ljust = 1;
				f++;
			}
			// flags '0' and '+' are recognized but skipped
			if (f[0] == '0') {
				f++;
				if (f[0] == '+')
					f++;
			}
			if (f[0] == '+') {
				f++;
				if (f[0] == '0')
					f++;
			}
			if (isdigit(f[0])) { /* there is a wid */
				wid = strtol(f, &f, 10);
			}
			if (f[0] == '.') { /* there is a .prec */
				prec = strtol(++f, &f, 10);
			}
			if (prec > 1)           // %c --> only one character
				prec = 1;
			pad = wid>prec ? wid - prec : 0;  // has to be >= 0
			int i;

			if (ljust) { // print one char from t, then pad blanks
				for (i = 0; i < n; i++)
					*p++ = t[i];
				for (i = 0; i < pad; i++) {
					//printf(" ");
					*p++ = ' ';
				}
			} else { // print pad blanks, then prec chars from t
				for (i = 0; i < pad; i++) {
					//printf(" ");
					*p++ = ' ';
				}
				for (i = 0; i < n; i++)
					*p++ = t[i];
			}
			*p = 0;
			break;
		}
		default:
			FATAL("can't happen: bad conversion %c in format()", flag);
		}

		tempfree(x);
		p += strlen(p);
		s++;
	}
	*p = '\0';
	free(fmt);
	for ( ; a; a = a->nnext) {		/* evaluate any remaining args */
		x = execute(a);
		tempfree(x);
	}
	*pbuf = buf;
	*pbufsize = bufsize;
	return p - buf;
}

Cell *awksprintf(Node **a, int n)		/* sprintf(a[0]) */
{
	Cell *x;
	Node *y;
	char *buf;
	int bufsz=3*recsize;

	if ((buf = (char *) malloc(bufsz)) == NULL)
		FATAL("out of memory in awksprintf");
	y = a[0]->nnext;
	x = execute(a[0]);
	if (format(&buf, &bufsz, getsval(x), y) == -1)
		FATAL("sprintf string %.30s... too long.  can't happen.", buf);
	tempfree(x);
	x = gettemp();
	x->sval = buf;
	x->tval = STR;
	return(x);
}

Cell *awkprintf(Node **a, int n)		/* printf */
{	/* a[0] is list of args, starting with format string */
	/* a[1] is redirection operator, a[2] is redirection file */
	FILE *fp;
	Cell *x;
	Node *y;
	char *buf;
	int len;
	int bufsz=3*recsize;

	if ((buf = (char *) malloc(bufsz)) == NULL)
		FATAL("out of memory in awkprintf");
	y = a[0]->nnext;
	x = execute(a[0]);
	if ((len = format(&buf, &bufsz, getsval(x), y)) == -1)
		FATAL("printf string %.30s... too long.  can't happen.", buf);
	tempfree(x);
	if (a[1] == NULL) {
		/* fputs(buf, stdout); */
		fwrite(buf, len, 1, stdout);
		if (ferror(stdout))
			FATAL("write error on stdout");
	} else {
		fp = redirect(ptoi(a[1]), a[2]);
		/* fputs(buf, fp); */
		fwrite(buf, len, 1, fp);
		fflush(fp);
		if (ferror(fp))
			FATAL("write error on %s", filename(fp));
	}
	free(buf);
	return(True);
}

Cell *arith(Node **a, int n)	/* a[0] + a[1], etc.  also -a[0] */
{
	Awkfloat i, j = 0;
	double v;
	Cell *x, *y, *z;

	x = execute(a[0]);
	i = getfval(x);
	tempfree(x);
	if (n != UMINUS && n != UPLUS) {
		y = execute(a[1]);
		j = getfval(y);
		tempfree(y);
	}
	z = gettemp();
	switch (n) {
	case ADD:
		i += j;
		break;
	case MINUS:
		i -= j;
		break;
	case MULT:
		i *= j;
		break;
	case DIVIDE:
		if (j == 0)
			FATAL("division by zero");
		i /= j;
		break;
	case MOD:
		if (j == 0)
			FATAL("division by zero in mod");
		modf(i/j, &v);
		i = i - j * v;
		break;
	case UMINUS:
		i = -i;
		break;
	case UPLUS: /* handled by getfval(), above */
		break;
	case POWER:
		if (j >= 0 && modf(j, &v) == 0.0)	/* pos integer exponent */
			i = ipow(i, (int) j);
               else {
			errno = 0;
			i = errcheck(pow(i, j), "pow");
               }
		break;
	default:	/* can't happen */
		FATAL("illegal arithmetic operator %d", n);
	}
	setfval(z, i);
	return(z);
}

double ipow(double x, int n)	/* x**n.  ought to be done by pow, but isn't always */
{
	double v;

	if (n <= 0)
		return 1;
	v = ipow(x, n/2);
	if (n % 2 == 0)
		return v * v;
	else
		return x * v * v;
}

Cell *incrdecr(Node **a, int n)		/* a[0]++, etc. */
{
	Cell *x, *z;
	int k;
	Awkfloat xf;

	x = execute(a[0]);
	xf = getfval(x);
	k = (n == PREINCR || n == POSTINCR) ? 1 : -1;
	if (n == PREINCR || n == PREDECR) {
		setfval(x, xf + k);
		return(x);
	}
	z = gettemp();
	setfval(z, xf);
	setfval(x, xf + k);
	tempfree(x);
	return(z);
}

Cell *assign(Node **a, int n)	/* a[0] = a[1], a[0] += a[1], etc. */
{		/* this is subtle; don't muck with it. */
	Cell *x, *y;
	Awkfloat xf, yf;
	double v;

	y = execute(a[1]);
	x = execute(a[0]);
	if (n == ASSIGN) {	/* ordinary assignment */
		if (x == y && !(x->tval & (FLD|REC)) && x != nfloc)
			;	/* self-assignment: leave alone unless it's a field or NF */
		else if ((y->tval & (STR|NUM)) == (STR|NUM)) {
			yf = getfval(y);
			setsval(x, getsval(y));
			x->fval = yf;
			x->tval |= NUM;
		}
		else if (isstr(y))
			setsval(x, getsval(y));
		else if (isnum(y))
			setfval(x, getfval(y));
		else
			funnyvar(y, "read value of");
		tempfree(y);
		return(x);
	}
	xf = getfval(x);
	yf = getfval(y);
	switch (n) {
	case ADDEQ:
		xf += yf;
		break;
	case SUBEQ:
		xf -= yf;
		break;
	case MULTEQ:
		xf *= yf;
		break;
	case DIVEQ:
		if (yf == 0)
			FATAL("division by zero in /=");
		xf /= yf;
		break;
	case MODEQ:
		if (yf == 0)
			FATAL("division by zero in %%=");
		modf(xf/yf, &v);
		xf = xf - yf * v;
		break;
	case POWEQ:
		if (yf >= 0 && modf(yf, &v) == 0.0)	/* pos integer exponent */
			xf = ipow(xf, (int) yf);
               else {
			errno = 0;
			xf = errcheck(pow(xf, yf), "pow");
               }
		break;
	default:
		FATAL("illegal assignment operator %d", n);
		break;
	}
	tempfree(y);
	setfval(x, xf);
	return(x);
}

Cell *cat(Node **a, int q)	/* a[0] cat a[1] */
{
	Cell *x, *y, *z;
	int n1, n2;
	char *s = NULL;
	int ssz = 0;

	x = execute(a[0]);
	n1 = strlen(getsval(x));
	adjbuf(&s, &ssz, n1 + 1, recsize, 0, "cat1");
	memcpy(s, x->sval, n1);

	tempfree(x);

	y = execute(a[1]);
	n2 = strlen(getsval(y));
	adjbuf(&s, &ssz, n1 + n2 + 1, recsize, 0, "cat2");
	memcpy(s + n1, y->sval, n2);
	s[n1 + n2] = '\0';

	tempfree(y);

	z = gettemp();
	z->sval = s;
	z->tval = STR;

	return(z);
}

Cell *pastat(Node **a, int n)	/* a[0] { a[1] } */
{
	Cell *x;

	if (a[0] == NULL)
		x = execute(a[1]);
	else {
		x = execute(a[0]);
		if (istrue(x)) {
			tempfree(x);
			x = execute(a[1]);
		}
	}
	return x;
}

Cell *dopa2(Node **a, int n)	/* a[0], a[1] { a[2] } */
{
	Cell *x;
	int pair;

	pair = ptoi(a[3]);
	if (pairstack[pair] == 0) {
		x = execute(a[0]);
		if (istrue(x))
			pairstack[pair] = 1;
		tempfree(x);
	}
	if (pairstack[pair] == 1) {
		x = execute(a[1]);
		if (istrue(x))
			pairstack[pair] = 0;
		tempfree(x);
		x = execute(a[2]);
		return(x);
	}
	return(False);
}

Cell *split(Node **a, int nnn)	/* split(a[0], a[1], a[2]); a[3] is type */
{
	Cell *x = NULL, *y, *ap;
	const char *s, *origs, *t;
	const char *fs = NULL;
	char *origfs = NULL;
	int sep;
	char temp, num[50];
	int n, tempstat, arg3type;
	int j;
	double result;

	y = execute(a[0]);	/* source string */
	origs = s = strdup(getsval(y));
	tempfree(y);
	arg3type = ptoi(a[3]);
	if (a[2] == NULL) {		/* BUG: CSV should override implicit fs but not explicit */
		fs = getsval(fsloc);
	} else if (arg3type == STRING) {	/* split(str,arr,"string") */
		x = execute(a[2]);
		fs = origfs = strdup(getsval(x));
		tempfree(x);
	} else if (arg3type == REGEXPR) {
		fs = "(regexpr)";	/* split(str,arr,/regexpr/) */
	} else {
		FATAL("illegal type of split");
	}
	sep = *fs;
	ap = execute(a[1]);	/* array name */
/* BUG 7/26/22: this appears not to reset array: see C1/asplit */
	freesymtab(ap);
	DPRINTF("split: s=|%s|, a=%s, sep=|%s|\n", s, NN(ap->nval), fs);
	ap->tval &= ~STR;
	ap->tval |= ARR;
	ap->sval = (char *) makesymtab(NSYMTAB);

	n = 0;
        if (arg3type == REGEXPR && strlen((char*)((fa*)a[2])->restr) == 0) {
		/* split(s, a, //); have to arrange that it looks like empty sep */
		arg3type = 0;
		fs = "";
		sep = 0;
	}
	if (*s != '\0' && (strlen(fs) > 1 || arg3type == REGEXPR)) {	/* reg expr */
		fa *pfa;
		if (arg3type == REGEXPR) {	/* it's ready already */
			pfa = (fa *) a[2];
		} else {
			pfa = makedfa(fs, 1);
		}
		if (nematch(pfa,s)) {
			tempstat = pfa->initstat;
			pfa->initstat = 2;
			do {
				n++;
				snprintf(num, sizeof(num), "%d", n);
				temp = *patbeg;
				setptr(patbeg, '\0');
				if (is_number(s, & result))
					setsymtab(num, s, result, STR|NUM, (Array *) ap->sval);
				else
					setsymtab(num, s, 0.0, STR, (Array *) ap->sval);
				setptr(patbeg, temp);
				s = patbeg + patlen;
				if (*(patbeg+patlen-1) == '\0' || *s == '\0') {
					n++;
					snprintf(num, sizeof(num), "%d", n);
					setsymtab(num, "", 0.0, STR, (Array *) ap->sval);
					pfa->initstat = tempstat;
					goto spdone;
				}
			} while (nematch(pfa,s));
			pfa->initstat = tempstat; 	/* bwk: has to be here to reset */
							/* cf gsub and refldbld */
		}
		n++;
		snprintf(num, sizeof(num), "%d", n);
		if (is_number(s, & result))
			setsymtab(num, s, result, STR|NUM, (Array *) ap->sval);
		else
			setsymtab(num, s, 0.0, STR, (Array *) ap->sval);
  spdone:
		pfa = NULL;

	} else if (a[2] == NULL && CSV) {	/* CSV only if no explicit separator */
		char *newt = (char *) malloc(strlen(s)); /* for building new string; reuse for each field */
		for (;;) {
			char *fr = newt;
			n++;
			if (*s == '"' ) { /* start of "..." */
				for (s++ ; *s != '\0'; ) {
					if (*s == '"' && s[1] != '\0' && s[1] == '"') {
						s += 2; /* doubled quote */
						*fr++ = '"';
					} else if (*s == '"' && (s[1] == '\0' || s[1] == ',')) {
						s++; /* skip over closing quote */
						break;
					} else {
						*fr++ = *s++;
					}
				}
				*fr++ = 0;
			} else {	/* unquoted field */
				while (*s != ',' && *s != '\0')
					*fr++ = *s++;
				*fr++ = 0;
			}
			snprintf(num, sizeof(num), "%d", n);
			if (is_number(newt, &result))
				setsymtab(num, newt, result, STR|NUM, (Array *) ap->sval);
			else
				setsymtab(num, newt, 0.0, STR, (Array *) ap->sval);
			if (*s++ == '\0')
				break;
		}
		free(newt);

	} else if (!CSV && sep == ' ') { /* usual case: split on white space */
		for (n = 0; ; ) {
#define ISWS(c)	((c) == ' ' || (c) == '\t' || (c) == '\n')
			while (ISWS(*s))
				s++;
			if (*s == '\0')
				break;
			n++;
			t = s;
			do
				s++;
			while (*s != '\0' && !ISWS(*s));
			temp = *s;
			setptr(s, '\0');
			snprintf(num, sizeof(num), "%d", n);
			if (is_number(t, & result))
				setsymtab(num, t, result, STR|NUM, (Array *) ap->sval);
			else
				setsymtab(num, t, 0.0, STR, (Array *) ap->sval);
			setptr(s, temp);
			if (*s != '\0')
				s++;
		}

	} else if (sep == 0) {	/* new: split(s, a, "") => 1 char/elem */
		for (n = 0; *s != '\0'; s += u8_nextlen(s)) {
			char buf[10];
			n++;
			snprintf(num, sizeof(num), "%d", n);

			for (j = 0; j < u8_nextlen(s); j++) {
				buf[j] = s[j];
			}
			buf[j] = '\0';

			if (isdigit((uschar)buf[0]))
				setsymtab(num, buf, atof(buf), STR|NUM, (Array *) ap->sval);
			else
				setsymtab(num, buf, 0.0, STR, (Array *) ap->sval);
		}

	} else if (*s != '\0') {  /* some random single character */
		for (;;) {
			n++;
			t = s;
			while (*s != sep && *s != '\n' && *s != '\0')
				s++;
			temp = *s;
			setptr(s, '\0');
			snprintf(num, sizeof(num), "%d", n);
			if (is_number(t, & result))
				setsymtab(num, t, result, STR|NUM, (Array *) ap->sval);
			else
				setsymtab(num, t, 0.0, STR, (Array *) ap->sval);
			setptr(s, temp);
			if (*s++ == '\0')
				break;
		}
	}
	tempfree(ap);
	xfree(origs);
	xfree(origfs);
	x = gettemp();
	x->tval = NUM;
	x->fval = n;
	return(x);
}

Cell *condexpr(Node **a, int n)	/* a[0] ? a[1] : a[2] */
{
	Cell *x;

	x = execute(a[0]);
	if (istrue(x)) {
		tempfree(x);
		x = execute(a[1]);
	} else {
		tempfree(x);
		x = execute(a[2]);
	}
	return(x);
}

Cell *ifstat(Node **a, int n)	/* if (a[0]) a[1]; else a[2] */
{
	Cell *x;

	x = execute(a[0]);
	if (istrue(x)) {
		tempfree(x);
		x = execute(a[1]);
	} else if (a[2] != NULL) {
		tempfree(x);
		x = execute(a[2]);
	}
	return(x);
}

Cell *whilestat(Node **a, int n)	/* while (a[0]) a[1] */
{
	Cell *x;

	for (;;) {
		x = execute(a[0]);
		if (!istrue(x))
			return(x);
		tempfree(x);
		x = execute(a[1]);
		if (isbreak(x)) {
			x = True;
			return(x);
		}
		if (isnext(x) || isexit(x) || isret(x))
			return(x);
		tempfree(x);
	}
}

Cell *dostat(Node **a, int n)	/* do a[0]; while(a[1]) */
{
	Cell *x;

	for (;;) {
		x = execute(a[0]);
		if (isbreak(x))
			return True;
		if (isnext(x) || isexit(x) || isret(x))
			return(x);
		tempfree(x);
		x = execute(a[1]);
		if (!istrue(x))
			return(x);
		tempfree(x);
	}
}

Cell *forstat(Node **a, int n)	/* for (a[0]; a[1]; a[2]) a[3] */
{
	Cell *x;

	x = execute(a[0]);
	tempfree(x);
	for (;;) {
		if (a[1]!=NULL) {
			x = execute(a[1]);
			if (!istrue(x)) return(x);
			else tempfree(x);
		}
		x = execute(a[3]);
		if (isbreak(x))		/* turn off break */
			return True;
		if (isnext(x) || isexit(x) || isret(x))
			return(x);
		tempfree(x);
		x = execute(a[2]);
		tempfree(x);
	}
}

Cell *instat(Node **a, int n)	/* for (a[0] in a[1]) a[2] */
{
	Cell *x, *vp, *arrayp, *cp, *ncp;
	Array *tp;
	int i;

	vp = execute(a[0]);
	arrayp = execute(a[1]);
	if (!isarr(arrayp)) {
		return True;
	}
	tp = (Array *) arrayp->sval;
	tempfree(arrayp);
	for (i = 0; i < tp->size; i++) {	/* this routine knows too much */
		for (cp = tp->tab[i]; cp != NULL; cp = ncp) {
			setsval(vp, cp->nval);
			ncp = cp->cnext;
			x = execute(a[2]);
			if (isbreak(x)) {
				tempfree(vp);
				return True;
			}
			if (isnext(x) || isexit(x) || isret(x)) {
				tempfree(vp);
				return(x);
			}
			tempfree(x);
		}
	}
	return True;
}

static char *nawk_convert(const char *s, int (*fun_c)(int),
    wint_t (*fun_wc)(wint_t))
{
	char *buf      = NULL;
	char *pbuf     = NULL;
	const char *ps = NULL;
	size_t n       = 0;
	wchar_t wc;
	const size_t sz = awk_mb_cur_max;
	int unused;

	if (sz == 1) {
		buf = tostring(s);

		for (pbuf = buf; *pbuf; pbuf++)
			*pbuf = fun_c((uschar)*pbuf);

		return buf;
	} else {
		/* upper/lower character may be shorter/longer */
		buf = tostringN(s, strlen(s) * sz + 1);

		(void) mbtowc(NULL, NULL, 0);	/* reset internal state */
		/*
		 * Reset internal state here too.
		 * Assign result to avoid a compiler warning. (Casting to void
		 * doesn't work.)
		 * Increment said variable to avoid a different warning.
		 */
		unused = wctomb(NULL, L'\0');
		unused++;

		ps   = s;
		pbuf = buf;
		while (n = mbtowc(&wc, ps, sz),
		       n > 0 && n != (size_t)-1 && n != (size_t)-2)
		{
			ps += n;

			n = wctomb(pbuf, fun_wc(wc));
			if (n == (size_t)-1)
				FATAL("illegal wide character %s", s);

			pbuf += n;
		}

		*pbuf = '\0';

		if (n)
			FATAL("illegal byte sequence %s", s);

		return buf;
	}
}

#ifdef __DJGPP__
static wint_t towupper(wint_t wc)
{
	if (wc >= 0 && wc < 256)
		return toupper(wc & 0xFF);

	return wc;
}

static wint_t towlower(wint_t wc)
{
	if (wc >= 0 && wc < 256)
		return tolower(wc & 0xFF);

	return wc;
}
#endif

static char *nawk_toupper(const char *s)
{
	return nawk_convert(s, toupper, towupper);
}

static char *nawk_tolower(const char *s)
{
	return nawk_convert(s, tolower, towlower);
}



Cell *bltin(Node **a, int n)	/* builtin functions. a[0] is type, a[1] is arg list */
{
	Cell *x, *y;
	Awkfloat u;
	int t;
	Awkfloat tmp;
	char *buf;
	Node *nextarg;
	FILE *fp;
	int status = 0;
	int estatus = 0;

	t = ptoi(a[0]);
	x = execute(a[1]);
	nextarg = a[1]->nnext;
	switch (t) {
	case FLENGTH:
		if (isarr(x))
			u = ((Array *) x->sval)->nelem;	/* GROT.  should be function*/
		else
			u = u8_strlen(getsval(x));
		break;
	case FLOG:
		errno = 0;
		u = errcheck(log(getfval(x)), "log");
		break;
	case FINT:
		modf(getfval(x), &u); break;
	case FEXP:
		errno = 0;
		u = errcheck(exp(getfval(x)), "exp");
		break;
	case FSQRT:
		errno = 0;
		u = errcheck(sqrt(getfval(x)), "sqrt");
		break;
	case FSIN:
		u = sin(getfval(x)); break;
	case FCOS:
		u = cos(getfval(x)); break;
	case FATAN:
		if (nextarg == NULL) {
			WARNING("atan2 requires two arguments; returning 1.0");
			u = 1.0;
		} else {
			y = execute(a[1]->nnext);
			u = atan2(getfval(x), getfval(y));
			tempfree(y);
			nextarg = nextarg->nnext;
		}
		break;
	case FSYSTEM:
		fflush(stdout);		/* in case something is buffered already */
		estatus = status = system(getsval(x));
		if (status != -1) {
			if (WIFEXITED(status)) {
				estatus = WEXITSTATUS(status);
			} else if (WIFSIGNALED(status)) {
				estatus = WTERMSIG(status) + 256;
#ifdef WCOREDUMP
				if (WCOREDUMP(status))
					estatus += 256;
#endif
			} else	/* something else?!? */
				estatus = 0;
		}
		/* else estatus was set to -1 */
		u = estatus;
		break;
	case FRAND:
		/* random() returns numbers in [0..2^31-1]
		 * in order to get a number in [0, 1), divide it by 2^31
		 */
		u = (Awkfloat) random() / (0x7fffffffL + 0x1UL);
		break;
	case FSRAND:
		if (isrec(x))	/* no argument provided */
			u = time((time_t *)0);
		else
			u = getfval(x);
		tmp = u;
		srandom((unsigned long) u);
		u = srand_seed;
		srand_seed = tmp;
		break;
	case FTOUPPER:
	case FTOLOWER:
		if (t == FTOUPPER)
			buf = nawk_toupper(getsval(x));
		else
			buf = nawk_tolower(getsval(x));
		tempfree(x);
		x = gettemp();
		setsval(x, buf);
		free(buf);
		return x;
	case FFLUSH:
		if (isrec(x) || strlen(getsval(x)) == 0) {
			flush_all();	/* fflush() or fflush("") -> all */
			u = 0;
		} else if ((fp = openfile(FFLUSH, getsval(x), NULL)) == NULL)
			u = EOF;
		else
			u = fflush(fp);
		break;
	default:	/* can't happen */
		FATAL("illegal function type %d", t);
		break;
	}
	tempfree(x);
	x = gettemp();
	setfval(x, u);
	if (nextarg != NULL) {
		WARNING("warning: function has too many arguments");
		for ( ; nextarg; nextarg = nextarg->nnext) {
			y = execute(nextarg);
			tempfree(y);
		}
	}
	return(x);
}

Cell *printstat(Node **a, int n)	/* print a[0] */
{
	Node *x;
	Cell *y;
	FILE *fp;

	if (a[1] == NULL)	/* a[1] is redirection operator, a[2] is file */
		fp = stdout;
	else
		fp = redirect(ptoi(a[1]), a[2]);
	for (x = a[0]; x != NULL; x = x->nnext) {
		y = execute(x);
		fputs(getpssval(y), fp);
		tempfree(y);
		if (x->nnext == NULL)
			fputs(getsval(orsloc), fp);
		else
			fputs(getsval(ofsloc), fp);
	}
	if (a[1] != NULL)
		fflush(fp);
	if (ferror(fp))
		FATAL("write error on %s", filename(fp));
	return(True);
}

Cell *nullproc(Node **a, int n)
{
	return 0;
}


FILE *redirect(int a, Node *b)	/* set up all i/o redirections */
{
	FILE *fp;
	Cell *x;
	char *fname;

	x = execute(b);
	fname = getsval(x);
	fp = openfile(a, fname, NULL);
	if (fp == NULL)
		FATAL("can't open file %s", fname);
	tempfree(x);
	return fp;
}

struct files {
	FILE	*fp;
	const char	*fname;
	int	mode;	/* '|', 'a', 'w' => LE/LT, GT */
} *files;

size_t nfiles;

static void stdinit(void)	/* in case stdin, etc., are not constants */
{
	nfiles = FOPEN_MAX;
	files = (struct files *) calloc(nfiles, sizeof(*files));
	if (files == NULL)
		FATAL("can't allocate file memory for %zu files", nfiles);
        files[0].fp = stdin;
	files[0].fname = tostring("/dev/stdin");
	files[0].mode = LT;
        files[1].fp = stdout;
	files[1].fname = tostring("/dev/stdout");
	files[1].mode = GT;
        files[2].fp = stderr;
	files[2].fname = tostring("/dev/stderr");
	files[2].mode = GT;
}

FILE *openfile(int a, const char *us, bool *pnewflag)
{
	const char *s = us;
	size_t i;
	int m;
	FILE *fp = NULL;

	if (*s == '\0')
		FATAL("null file name in print or getline");
	for (i = 0; i < nfiles; i++)
		if (files[i].fname && strcmp(s, files[i].fname) == 0 &&
		    (a == files[i].mode || (a==APPEND && files[i].mode==GT) ||
		     a == FFLUSH)) {
			if (pnewflag)
				*pnewflag = false;
			return files[i].fp;
		}
	if (a == FFLUSH)	/* didn't find it, so don't create it! */
		return NULL;

	for (i = 0; i < nfiles; i++)
		if (files[i].fp == NULL)
			break;
	if (i >= nfiles) {
		struct files *nf;
		size_t nnf = nfiles + FOPEN_MAX;
		nf = (struct files *) realloc(files, nnf * sizeof(*nf));
		if (nf == NULL)
			FATAL("cannot grow files for %s and %zu files", s, nnf);
		memset(&nf[nfiles], 0, FOPEN_MAX * sizeof(*nf));
		nfiles = nnf;
		files = nf;
	}
	fflush(stdout);	/* force a semblance of order */
	m = a;
	if (a == GT) {
		fp = fopen(s, "w");
	} else if (a == APPEND) {
		fp = fopen(s, "a");
		m = GT;	/* so can mix > and >> */
	} else if (a == '|') {	/* output pipe */
		fp = popen(s, "w");
	} else if (a == LE) {	/* input pipe */
		fp = popen(s, "r");
	} else if (a == LT) {	/* getline <file */
		fp = strcmp(s, "-") == 0 ? stdin : fopen(s, "r");	/* "-" is stdin */
	} else	/* can't happen */
		FATAL("illegal redirection %d", a);
	if (fp != NULL) {
		files[i].fname = tostring(s);
		files[i].fp = fp;
		files[i].mode = m;
		if (pnewflag)
			*pnewflag = true;
		if (fp != stdin && fp != stdout && fp != stderr)
			(void) fcntl(fileno(fp), F_SETFD, FD_CLOEXEC);
	}
	return fp;
}

const char *filename(FILE *fp)
{
	size_t i;

	for (i = 0; i < nfiles; i++)
		if (fp == files[i].fp)
			return files[i].fname;
	return "???";
}

Cell *closefile(Node **a, int n)
{
 	Cell *x;
	size_t i;
	bool stat;

 	x = execute(a[0]);
 	getsval(x);
	stat = true;
 	for (i = 0; i < nfiles; i++) {
		if (!files[i].fname || strcmp(x->sval, files[i].fname) != 0)
			continue;
		if (files[i].mode == GT || files[i].mode == '|')
			fflush(files[i].fp);
		if (ferror(files[i].fp)) {
			if ((files[i].mode == GT && files[i].fp != stderr)
			  || files[i].mode == '|')
				FATAL("write error on %s", files[i].fname);
			else
				WARNING("i/o error occurred on %s", files[i].fname);
		}
		if (files[i].fp == stdin || files[i].fp == stdout ||
		    files[i].fp == stderr)
			stat = freopen("/dev/null", "r+", files[i].fp) == NULL;
		else if (files[i].mode == '|' || files[i].mode == LE)
			stat = pclose(files[i].fp) == -1;
		else
			stat = fclose(files[i].fp) == EOF;
		if (stat)
			WARNING("i/o error occurred closing %s", files[i].fname);
		xfree(files[i].fname);
		files[i].fname = NULL;	/* watch out for ref thru this */
		files[i].fp = NULL;
		break;
 	}
 	tempfree(x);
 	x = gettemp();
	setfval(x, (Awkfloat) (stat ? -1 : 0));
 	return(x);
}

void closeall(void)
{
	size_t i;
	bool stat = false;

	for (i = 0; i < nfiles; i++) {
		if (! files[i].fp)
			continue;
		if (files[i].mode == GT || files[i].mode == '|')
			fflush(files[i].fp);
		if (ferror(files[i].fp)) {
			if ((files[i].mode == GT && files[i].fp != stderr)
			  || files[i].mode == '|')
				FATAL("write error on %s", files[i].fname);
			else
				WARNING("i/o error occurred on %s", files[i].fname);
		}
		if (files[i].fp == stdin || files[i].fp == stdout ||
		    files[i].fp == stderr)
			continue;
		if (files[i].mode == '|' || files[i].mode == LE)
			stat = pclose(files[i].fp) == -1;
		else
			stat = fclose(files[i].fp) == EOF;
		if (stat)
			WARNING("i/o error occurred while closing %s", files[i].fname);
	}
}

static void flush_all(void)
{
	size_t i;

	for (i = 0; i < nfiles; i++)
		if (files[i].fp)
			fflush(files[i].fp);
}

void backsub(char **pb_ptr, const char **sptr_ptr);

Cell *dosub(Node **a, int subop)        /* sub and gsub */
{
	fa *pfa;
	int tempstat = 0;
	char *repl;
	Cell *x;

	char *buf = NULL;
	char *pb = NULL;
	int bufsz = recsize;

	const char *r, *s;
	const char *start;
	const char *noempty = NULL;      /* empty match disallowed here */
	size_t m = 0;                    /* match count */
	size_t whichm;                   /* which match to select, 0 = global */
	int mtype;                       /* match type */

	if (a[0] == NULL) {	/* 0 => a[1] is already-compiled regexpr */
		pfa = (fa *) a[1];
	} else {
		x = execute(a[1]);
		pfa = makedfa(getsval(x), 1);
		tempfree(x);
	}

	x = execute(a[2]);	/* replacement string */
	repl = tostring(getsval(x));
	tempfree(x);

	switch (subop) {
	case SUB:
		whichm = 1;
		x = execute(a[3]);    /* source string */
		break;
	case GSUB:
		whichm = 0;
		x = execute(a[3]);    /* source string */
		break;
	default:
		FATAL("dosub: unrecognized subop: %d", subop);
	}

	start = getsval(x);
	while (pmatch(pfa, start)) {
		if (buf == NULL) {
			if ((pb = buf = (char *) malloc(bufsz)) == NULL)
				FATAL("out of memory in dosub");
			tempstat = pfa->initstat;
			pfa->initstat = 2;
		}

		/* match types */
		#define	MT_IGNORE  0  /* unselected or invalid */
		#define MT_INSERT  1  /* selected, empty */
		#define MT_REPLACE 2  /* selected, not empty */

		/* an empty match just after replacement is invalid */

		if (patbeg == noempty && patlen == 0) {
			mtype = MT_IGNORE;    /* invalid, not counted */
		} else if (whichm == ++m || whichm == 0) {
			mtype = patlen ? MT_REPLACE : MT_INSERT;
		} else {
			mtype = MT_IGNORE;    /* unselected, but counted */
		}

		/* leading text: */
		if (patbeg > start) {
			adjbuf(&buf, &bufsz, (pb - buf) + (patbeg - start),
				recsize, &pb, "dosub");
			s = start;
			while (s < patbeg)
				*pb++ = *s++;
		}

		if (mtype == MT_IGNORE)
			goto matching_text;  /* skip replacement text */

		r = repl;
		while (*r != 0) {
			adjbuf(&buf, &bufsz, 5+pb-buf, recsize, &pb, "dosub");
			if (*r == '\\') {
				backsub(&pb, &r);
			} else if (*r == '&') {
				r++;
				adjbuf(&buf, &bufsz, 1+patlen+pb-buf, recsize,
					&pb, "dosub");
				for (s = patbeg; s < patbeg+patlen; )
					*pb++ = *s++;
			} else {
				*pb++ = *r++;
			}
		}

matching_text:
		if (mtype == MT_REPLACE || *patbeg == '\0')
			goto next_search;  /* skip matching text */
		
		if (patlen == 0)
			patlen = u8_nextlen(patbeg);
		adjbuf(&buf, &bufsz, (pb-buf) + patlen, recsize, &pb, "dosub");
		s = patbeg;
		while (s < patbeg + patlen)
			*pb++ = *s++;

next_search:
		start = patbeg + patlen;
		if (m == whichm || *patbeg == '\0')
			break;
		if (mtype == MT_REPLACE)
			noempty = start;

		#undef MT_IGNORE
		#undef MT_INSERT
		#undef MT_REPLACE
	}

	xfree(repl);

	if (buf != NULL) {
		pfa->initstat = tempstat;

		/* trailing text */
		adjbuf(&buf, &bufsz, 1+strlen(start)+pb-buf, 0, &pb, "dosub");
		while ((*pb++ = *start++) != '\0')
			;

		setsval(x, buf);
		free(buf);
	}

	tempfree(x);
	x = gettemp();
	x->tval = NUM;
	x->fval = m;
	return x;
}

void backsub(char **pb_ptr, const char **sptr_ptr)	/* handle \\& variations */
{						/* sptr[0] == '\\' */
	char *pb = *pb_ptr;
	const char *sptr = *sptr_ptr;
	static bool first = true;
	static bool do_posix = false;

	if (first) {
		first = false;
		do_posix = (getenv("POSIXLY_CORRECT") != NULL);
	}

	if (sptr[1] == '\\') {
		if (sptr[2] == '\\' && sptr[3] == '&') { /* \\\& -> \& */
			*pb++ = '\\';
			*pb++ = '&';
			sptr += 4;
		} else if (sptr[2] == '&') {	/* \\& -> \ + matched */
			*pb++ = '\\';
			sptr += 2;
		} else if (do_posix) {		/* \\x -> \x */
			sptr++;
			*pb++ = *sptr++;
		} else {			/* \\x -> \\x */
			*pb++ = *sptr++;
			*pb++ = *sptr++;
		}
	} else if (sptr[1] == '&') {	/* literal & */
		sptr++;
		*pb++ = *sptr++;
	} else				/* literal \ */
		*pb++ = *sptr++;

	*pb_ptr = pb;
	*sptr_ptr = sptr;
}

static char *wide_char_to_byte_str(int rune, size_t *outlen)
{
	static char buf[5];
	int len;

	if (rune < 0 || rune > 0x10FFFF)
		return NULL;

	memset(buf, 0, sizeof(buf));

	len = 0;
	if (rune <= 0x0000007F) {
		buf[len++] = rune;
	} else if (rune <= 0x000007FF) {
		// 110xxxxx 10xxxxxx
		buf[len++] = 0xC0 | (rune >> 6);
		buf[len++] = 0x80 | (rune & 0x3F);
	} else if (rune <= 0x0000FFFF) {
		// 1110xxxx 10xxxxxx 10xxxxxx
		buf[len++] = 0xE0 | (rune >> 12);
		buf[len++] = 0x80 | ((rune >> 6) & 0x3F);
		buf[len++] = 0x80 | (rune & 0x3F);

	} else {
		// 0x00010000 - 0x10FFFF
		// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		buf[len++] = 0xF0 | (rune >> 18);
		buf[len++] = 0x80 | ((rune >> 12) & 0x3F);
		buf[len++] = 0x80 | ((rune >> 6) & 0x3F);
		buf[len++] = 0x80 | (rune & 0x3F);
	}

	*outlen = len;
	buf[len++] = '\0';

	return buf;
}
