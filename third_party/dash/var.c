/*-
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 * Copyright (c) 1997-2005
 *	Herbert Xu <herbert@gondor.apana.org.au>.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Kenneth Almquist.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_PATHS_H
#include <paths.h>
#endif

/*
 * Shell variables.
 */

#include "shell.h"
#include "output.h"
#include "expand.h"
#include "nodes.h"	/* for other headers */
#include "exec.h"
#include "syntax.h"
#include "options.h"
#include "mail.h"
#include "var.h"
#include "memalloc.h"
#include "error.h"
#include "mystring.h"
#include "parser.h"
#include "show.h"
#ifndef SMALL
#include "myhistedit.h"
#endif
#include "system.h"


#define VTABSIZE 39


struct localvar_list {
	struct localvar_list *next;
	struct localvar *lv;
};

MKINIT struct localvar_list *localvar_stack;

const char defpathvar[] =
	"PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin";
char defifsvar[] = "IFS= \t\n";
MKINIT char defoptindvar[] = "OPTIND=1";

int lineno;
char linenovar[sizeof("LINENO=")+sizeof(int)*CHAR_BIT/3+1] = "LINENO=";

/* Some macros in var.h depend on the order, add new variables to the end. */
struct var varinit[] = {
#if ATTY
	{ 0,	VSTRFIXED|VTEXTFIXED|VUNSET,	"ATTY\0",	0 },
#endif
	{ 0,	VSTRFIXED|VTEXTFIXED,		defifsvar,	changeifs },
	{ 0,	VSTRFIXED|VTEXTFIXED|VUNSET,	"MAIL\0",	changemail },
	{ 0,	VSTRFIXED|VTEXTFIXED|VUNSET,	"MAILPATH\0",	changemail },
	{ 0,	VSTRFIXED|VTEXTFIXED,		defpathvar,	changepath },
	{ 0,	VSTRFIXED|VTEXTFIXED,		"PS1=$ ",	0 },
	{ 0,	VSTRFIXED|VTEXTFIXED,		"PS2=> ",	0 },
	{ 0,	VSTRFIXED|VTEXTFIXED,		"PS4=+ ",	0 },
	{ 0,	VSTRFIXED|VTEXTFIXED|VNOFUNC,	defoptindvar,	getoptsreset },
#ifdef WITH_LINENO
	{ 0,	VSTRFIXED|VTEXTFIXED,		linenovar,	0 },
#endif
#ifndef SMALL
	{ 0,	VSTRFIXED|VTEXTFIXED|VUNSET,	"TERM\0",	0 },
	{ 0,	VSTRFIXED|VTEXTFIXED|VUNSET,	"HISTSIZE\0",	sethistsize },
#endif
};

STATIC struct var *vartab[VTABSIZE];

STATIC struct var **hashvar(const char *);
STATIC int vpcmp(const void *, const void *);
STATIC struct var **findvar(const char *);

/*
 * Initialize the varable symbol tables and import the environment
 */

#ifdef mkinit
INCLUDE <unistd.h>
INCLUDE <sys/types.h>
INCLUDE <sys/stat.h>
INCLUDE "cd.h"
INCLUDE "output.h"
INCLUDE "var.h"
MKINIT char **environ;
INIT {
	char **envp;
	static char ppid[32] = "PPID=";
	const char *p;
	struct stat64 st1, st2;

	initvar();
	for (envp = environ ; *envp ; envp++) {
		p = endofname(*envp);
		if (p != *envp && *p == '=') {
			setvareq(*envp, VEXPORT|VTEXTFIXED);
		}
	}

	setvareq(defifsvar, VTEXTFIXED);
	setvareq(defoptindvar, VTEXTFIXED);

	fmtstr(ppid + 5, sizeof(ppid) - 5, "%ld", (long) getppid());
	setvareq(ppid, VTEXTFIXED);

	p = lookupvar("PWD");
	if (p)
		if (*p != '/' || stat64(p, &st1) || stat64(dotdir, &st2) ||
		    st1.st_dev != st2.st_dev || st1.st_ino != st2.st_ino)
			p = 0;
	setpwd(p, 0);
}

RESET {
	unwindlocalvars(0);
}
#endif

static char *varnull(const char *s)
{
	return (strchr(s, '=') ?: nullstr - 1) + 1;
}

/*
 * This routine initializes the builtin variables.  It is called when the
 * shell is initialized.
 */

void
initvar(void)
{
	struct var *vp;
	struct var *end;
	struct var **vpp;

	vp = varinit;
	end = vp + sizeof(varinit) / sizeof(varinit[0]);
	do {
		vpp = hashvar(vp->text);
		vp->next = *vpp;
		*vpp = vp;
	} while (++vp < end);
	/*
	 * PS1 depends on uid
	 */
	if (!geteuid())
		vps1.text = "PS1=# ";
}

/*
 * Set the value of a variable.  The flags argument is ored with the
 * flags of the variable.  If val is NULL, the variable is unset.
 */

struct var *setvar(const char *name, const char *val, int flags)
{
	char *p, *q;
	size_t namelen;
	char *nameeq;
	size_t vallen;
	struct var *vp;

	q = endofname(name);
	p = strchrnul(q, '=');
	namelen = p - name;
	if (!namelen || p != q)
		sh_error("%.*s: bad variable name", namelen, name);
	vallen = 0;
	if (val == NULL) {
		flags |= VUNSET;
	} else {
		vallen = strlen(val);
	}
	INTOFF;
	p = mempcpy(nameeq = ckmalloc(namelen + vallen + 2), name, namelen);
	if (val) {
		*p++ = '=';
		p = mempcpy(p, val, vallen);
	}
	*p = '\0';
	vp = setvareq(nameeq, flags | VNOSAVE);
	INTON;

	return vp;
}

/*
 * Set the given integer as the value of a variable.  The flags argument is
 * ored with the flags of the variable.
 */

intmax_t setvarint(const char *name, intmax_t val, int flags)
{
	int len = max_int_length(sizeof(val));
	char buf[len];

	fmtstr(buf, len, "%" PRIdMAX, val);
	setvar(name, buf, flags);
	return val;
}



/*
 * Same as setvar except that the variable and value are passed in
 * the first argument as name=value.  Since the first argument will
 * be actually stored in the table, it should not be a string that
 * will go away.
 * Called with interrupts off.
 */

struct var *setvareq(char *s, int flags)
{
	struct var *vp, **vpp;

	flags |= (VEXPORT & (((unsigned) (1 - aflag)) - 1));
	vpp = findvar(s);
	vp = *vpp;
	if (vp) {
		unsigned bits;

		if (vp->flags & VREADONLY) {
			const char *n;

			if (flags & VNOSAVE)
				free(s);
			n = vp->text;
			sh_error("%.*s: is read only", strchrnul(n, '=') - n,
				 n);
		}

		if ((vp->flags & (VTEXTFIXED|VSTACK)) == 0)
			ckfree(vp->text);

		if ((flags & (VEXPORT|VREADONLY|VSTRFIXED|VUNSET)) != VUNSET)
			bits = ~(VTEXTFIXED|VSTACK|VNOSAVE|VUNSET);
		else if ((vp->flags & VSTRFIXED))
			bits = VSTRFIXED;
		else {
			*vpp = vp->next;
			ckfree(vp);
out_free:
			if ((flags & (VTEXTFIXED|VSTACK|VNOSAVE)) == VNOSAVE)
				ckfree(s);
			goto out;
		}

		flags |= vp->flags & bits;
	} else {
		if ((flags & (VEXPORT|VREADONLY|VSTRFIXED|VUNSET)) == VUNSET)
			goto out_free;
		/* not found */
		vp = ckmalloc(sizeof (*vp));
		vp->next = *vpp;
		vp->func = NULL;
		*vpp = vp;
	}
	if (!(flags & (VTEXTFIXED|VSTACK|VNOSAVE)))
		s = savestr(s);
	vp->text = s;
	vp->flags = flags;

	if (vp->func && (flags & VNOFUNC) == 0)
		(*vp->func)(varnull(s));

out:
	return vp;
}

/*
 * Find the value of a variable.  Returns NULL if not set.
 */

char *
lookupvar(const char *name)
{
	struct var *v;

	if ((v = *findvar(name)) && !(v->flags & VUNSET)) {
#ifdef WITH_LINENO
		if (v == &vlineno && v->text == linenovar) {
			fmtstr(linenovar+7, sizeof(linenovar)-7, "%d", lineno);
		}
#endif
		return strchrnul(v->text, '=') + 1;
	}
	return NULL;
}

intmax_t lookupvarint(const char *name)
{
	return atomax(lookupvar(name) ?: nullstr, 0);
}



/*
 * Generate a list of variables satisfying the given conditions.
 */

char **
listvars(int on, int off, char ***end)
{
	struct var **vpp;
	struct var *vp;
	char **ep;
	int mask;

	STARTSTACKSTR(ep);
	vpp = vartab;
	mask = on | off;
	do {
		for (vp = *vpp ; vp ; vp = vp->next)
			if ((vp->flags & mask) == on) {
				if (ep == stackstrend())
					ep = growstackstr();
				*ep++ = (char *) vp->text;
			}
	} while (++vpp < vartab + VTABSIZE);
	if (ep == stackstrend())
		ep = growstackstr();
	if (end)
		*end = ep;
	*ep++ = NULL;
	return grabstackstr(ep);
}



/*
 * POSIX requires that 'set' (but not export or readonly) output the
 * variables in lexicographic order - by the locale's collating order (sigh).
 * Maybe we could keep them in an ordered balanced binary tree
 * instead of hashed lists.
 * For now just roll 'em through qsort for printing...
 */

int
showvars(const char *prefix, int on, int off)
{
	const char *sep;
	char **ep, **epend;

	ep = listvars(on, off, &epend);
	qsort(ep, epend - ep, sizeof(char *), vpcmp);

	sep = *prefix ? spcstr : prefix;

	for (; ep < epend; ep++) {
		const char *p;
		const char *q;

		p = strchrnul(*ep, '=');
		q = nullstr;
		if (*p)
			q = single_quote(++p);

		out1fmt("%s%s%.*s%s\n", prefix, sep, (int)(p - *ep), *ep, q);
	}

	return 0;
}



/*
 * The export and readonly commands.
 */

int
exportcmd(int argc, char **argv)
{
	struct var *vp;
	char *name;
	const char *p;
	char **aptr;
	int flag = argv[0][0] == 'r'? VREADONLY : VEXPORT;
	int notp;

	notp = nextopt("p") - 'p';
	if (notp && ((name = *(aptr = argptr)))) {
		do {
			if ((p = strchr(name, '=')) != NULL) {
				p++;
			} else {
				if ((vp = *findvar(name))) {
					vp->flags |= flag;
					continue;
				}
			}
			setvar(name, p, flag);
		} while ((name = *++aptr) != NULL);
	} else {
		showvars(argv[0], flag, 0);
	}
	return 0;
}


/*
 * The "local" command.
 */

int
localcmd(int argc, char **argv)
{
	char *name;

	if (!localvar_stack)
		sh_error("not in a function");

	argv = argptr;
	while ((name = *argv++) != NULL) {
		mklocal(name, 0);
	}
	return 0;
}


/*
 * Make a variable a local variable.  When a variable is made local, it's
 * value and flags are saved in a localvar structure.  The saved values
 * will be restored when the shell function returns.  We handle the name
 * "-" as a special case.
 */

void mklocal(char *name, int flags)
{
	struct localvar *lvp;
	struct var *vp;

	INTOFF;
	lvp = ckmalloc(sizeof (struct localvar));
	if (name[0] == '-' && name[1] == '\0') {
		char *p;
		p = ckmalloc(sizeof(optlist));
		lvp->text = memcpy(p, optlist, sizeof(optlist));
		vp = NULL;
	} else {
		char *eq;

		vp = *findvar(name);
		eq = strchr(name, '=');
		if (vp == NULL) {
			if (eq)
				vp = setvareq(name, VSTRFIXED | flags);
			else
				vp = setvar(name, NULL, VSTRFIXED | flags);
			lvp->flags = VUNSET;
		} else {
			lvp->text = vp->text;
			lvp->flags = vp->flags;
			vp->flags |= VSTRFIXED|VTEXTFIXED;
			if (eq)
				setvareq(name, flags);
		}
	}
	lvp->vp = vp;
	lvp->next = localvar_stack->lv;
	localvar_stack->lv = lvp;
	INTON;
}


/*
 * Called after a function returns.
 * Interrupts must be off.
 */

static void
poplocalvars(void)
{
	struct localvar_list *ll;
	struct localvar *lvp, *next;
	struct var *vp;

	INTOFF;
	ll = localvar_stack;
	localvar_stack = ll->next;

	next = ll->lv;
	ckfree(ll);

	while ((lvp = next) != NULL) {
		next = lvp->next;
		vp = lvp->vp;
		TRACE(("poplocalvar %s\n", vp ? vp->text : "-"));
		if (vp == NULL) {	/* $- saved */
			memcpy(optlist, lvp->text, sizeof(optlist));
			ckfree(lvp->text);
			optschanged();
		} else if (lvp->flags == VUNSET) {
			vp->flags &= ~(VSTRFIXED|VREADONLY);
			unsetvar(vp->text);
		} else {
			if ((vp->flags & (VTEXTFIXED|VSTACK)) == 0)
				ckfree(vp->text);
			vp->flags = lvp->flags;
			vp->text = lvp->text;
			if (vp->func && !(vp->flags & VNOFUNC))
				(*vp->func)(varnull(vp->text));
		}
		ckfree(lvp);
	}
	INTON;
}


/*
 * Create a new localvar environment.
 */
struct localvar_list *pushlocalvars(int push)
{
	struct localvar_list *ll;
	struct localvar_list *top;

	top = localvar_stack;
	if (!push)
		goto out;

	INTOFF;
	ll = ckmalloc(sizeof(*ll));
	ll->lv = NULL;
	ll->next = top;
	localvar_stack = ll;
	INTON;

out:
	return top;
}


void unwindlocalvars(struct localvar_list *stop)
{
	while (localvar_stack != stop)
		poplocalvars();
}


/*
 * The unset builtin command.  We unset the function before we unset the
 * variable to allow a function to be unset when there is a readonly variable
 * with the same name.
 */

int
unsetcmd(int argc, char **argv)
{
	char **ap;
	int i;
	int flag = 0;

	while ((i = nextopt("vf")) != '\0') {
		flag = i;
	}

	for (ap = argptr; *ap ; ap++) {
		if (flag != 'f') {
			unsetvar(*ap);
			continue;
		}
		if (flag != 'v')
			unsetfunc(*ap);
	}
	return 0;
}


/*
 * Unset the specified variable.
 */

void unsetvar(const char *s)
{
	setvar(s, 0, 0);
}



/*
 * Find the appropriate entry in the hash table from the name.
 */

STATIC struct var **
hashvar(const char *p)
{
	return &vartab[hashval(p) % VTABSIZE];
}



/*
 * Compares two strings up to the first = or '\0'.  The first
 * string must be terminated by '='; the second may be terminated by
 * either '=' or '\0'.
 */

int
varcmp(const char *p, const char *q)
{
	int c = *p, d = *q;
	while (c == d) {
		if (!c)
			break;
		p++;
		q++;
		c = *p;
		d = *q;
		if (c == '=')
			c = '\0';
		if (d == '=')
			d = '\0';
	}
	return c - d;
}

STATIC int
vpcmp(const void *a, const void *b)
{
	return varcmp(*(const char **)a, *(const char **)b);
}

STATIC struct var **
findvar(const char *name)
{
	struct var **vpp;

	for (vpp = hashvar(name); *vpp; vpp = &(*vpp)->next) {
		if (varequal((*vpp)->text, name)) {
			break;
		}
	}
	return vpp;
}
