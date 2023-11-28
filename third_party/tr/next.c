/*	$OpenBSD: str.c,v 1.14 2021/11/02 03:09:15 cheloha Exp $	*/
/*	$NetBSD: str.c,v 1.7 1995/08/31 22:13:47 jtc Exp $	*/
/*-
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
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
#include "libc/assert.h"
#include "libc/calls/typedef/u.h"
#include "libc/fmt/conv.h"
#include "libc/log/bsd.h"
#include "libc/mem/alg.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "third_party/tr/extern.h"

static int	backslash(STR *);
static int	bracket(STR *);
static int	c_class(const void *, const void *);
static void	genclass(STR *);
static void	genequiv(STR *);
static int	genrange(STR *);
static void	genseq(STR *);

int
next(s)
	STR *s;
{
	int ch;

	switch (s->state) {
	case EOS:
		return (0);
	case INFINITE:
		return (1);
	case NORMAL:
		switch (ch = *s->str) {
		case '\0':
			s->state = EOS;
			return (0);
		case '\\':
			s->lastch = backslash(s);
			break;
		case '[':
			if (bracket(s))
				return (next(s));
			/* FALLTHROUGH */
		default:
			++s->str;
			s->lastch = ch;
			break;
		}

		/* We can start a range at any time. */
		if (s->str[0] == '-' && genrange(s))
			return (next(s));
		return (1);
	case RANGE:
		if (s->cnt-- == 0) {
			s->state = NORMAL;
			return (next(s));
		}
		++s->lastch;
		return (1);
	case SEQUENCE:
		if (s->cnt-- == 0) {
			s->state = NORMAL;
			return (next(s));
		}
		return (1);
	case SET:
		if ((s->lastch = s->set[s->cnt++]) == OOBCH) {
			s->state = NORMAL;
			return (next(s));
		}
		return (1);
	default:
		return 0;
	}
	/* NOTREACHED */
}

static int
bracket(s)
	STR *s;
{
	char *p;

	switch (s->str[1]) {
	case ':':				/* "[:class:]" */
		if ((p = strstr((char *)s->str + 2, ":]")) == NULL)
			return (0);
		*p = '\0';
		s->str += 2;
		genclass(s);
		s->str = (unsigned char *)p + 2;
		return (1);
	case '=':				/* "[=equiv=]" */
		if ((p = strstr((char *)s->str + 2, "=]")) == NULL)
			return (0);
		s->str += 2;
		genequiv(s);
		return (1);
	default:				/* "[\###*n]" or "[#*n]" */
		if ((p = strpbrk((char *)s->str + 2, "*]")) == NULL)
			return (0);
		if (p[0] != '*' || strchr(p, ']') == NULL)
			return (0);
		s->str += 1;
		genseq(s);
		return (1);
	}
	/* NOTREACHED */
}

typedef struct {
	char *name;
	int (*func)(int);
	int *set;
} CLASS;

static CLASS classes[] = {
	{ "alnum",  isalnum,  },
	{ "alpha",  isalpha,  },
	{ "blank",  isblank,  },
	{ "cntrl",  iscntrl,  },
	{ "digit",  isdigit,  },
	{ "graph",  isgraph,  },
	{ "lower",  islower,  },
	{ "print",  isprint,  },
	{ "punct",  ispunct,  },
	{ "space",  isspace,  },
	{ "upper",  isupper,  },
	{ "xdigit", isxdigit, },
};

static void
genclass(STR *s)
{
	CLASS *cp, tmp;
	size_t len;
	int i;

	tmp.name = (char *)s->str;
	if ((cp = (CLASS *)bsearch(&tmp, classes, sizeof(classes) /
	    sizeof(CLASS), sizeof(CLASS), c_class)) == NULL)
		errx(1, "unknown class %s", s->str);

	/*
	 * Generate the set of characters in the class if we haven't
	 * already done so.
	 */
	if (cp->set == NULL) {
		cp->set = reallocarray(NULL, NCHARS + 1, sizeof(*cp->set));
		if (cp->set == NULL)
			err(1, NULL);
		len = 0;
		for (i = 0; i < NCHARS; i++) {
			if (cp->func(i)) {
				cp->set[len] = i;
				len++;
			}
		}
		cp->set[len] = OOBCH;
		len++;
		cp->set = reallocarray(cp->set, len, sizeof(*cp->set));
		if (cp->set == NULL)
			err(1, NULL);
	}

	s->cnt = 0;
	s->state = SET;
	s->set = cp->set;
}

static int
c_class(a, b)
	const void *a, *b;
{
	return (strcmp(((CLASS *)a)->name, ((CLASS *)b)->name));
}

/*
 * English doesn't have any equivalence classes, so for now
 * we just syntax check and grab the character.
 */
static void
genequiv(s)
	STR *s;
{
	if (*s->str == '\\') {
		s->equiv[0] = backslash(s);
		if (*s->str != '=')
			errx(1, "misplaced equivalence equals sign");
	} else {
		s->equiv[0] = s->str[0];
		if (s->str[1] != '=')
			errx(1, "misplaced equivalence equals sign");
	}
	s->str += 2;
	s->cnt = 0;
	s->state = SET;
	s->set = s->equiv;
}

static int
genrange(s)
	STR *s;
{
	int stopval;
	unsigned char *savestart;

	savestart = s->str;
	stopval = *++s->str == '\\' ? backslash(s) : *s->str++;
	if (stopval < (u_char)s->lastch) {
		s->str = savestart;
		return (0);
	}
	s->cnt = stopval - s->lastch + 1;
	s->state = RANGE;
	--s->lastch;
	return (1);
}

static void
genseq(s)
	STR *s;
{
	char *ep;

	if (s->which == STRING1)
		errx(1, "sequences only valid in string2");

	if (*s->str == '\\')
		s->lastch = backslash(s);
	else
		s->lastch = *s->str++;
	if (*s->str != '*')
		errx(1, "misplaced sequence asterisk");

	switch (*++s->str) {
	case '\\':
		s->cnt = backslash(s);
		break;
	case ']':
		s->cnt = 0;
		++s->str;
		break;
	default:
		if (isdigit(*s->str)) {
			s->cnt = strtol((char *)s->str, &ep, 0);
			if (*ep == ']') {
				s->str = (unsigned char *)ep + 1;
				break;
			}
		}
		errx(1, "illegal sequence count");
		/* NOTREACHED */
	}

	s->state = s->cnt ? SEQUENCE : INFINITE;
}

/*
 * Translate \??? into a character.  Up to 3 octal digits, if no digits either
 * an escape code or a literal character.
 */
static int
backslash(STR *s)
{
	size_t i;
	int ch, val;

	unassert(*s->str == '\\');
	s->str++;

	/* Empty escapes become plain backslashes. */
	if (*s->str == '\0') {
		s->state = EOS;
		return ('\\');
	}

	val = 0;
	for (i = 0; i < 3; i++) {
		if (s->str[i] < '0' || '7' < s->str[i])
			break;
		val = val * 8 + s->str[i] - '0';
	}
	if (i > 0) {
		if (val > UCHAR_MAX)
			errx(1, "octal value out of range: %d", val);
		s->str += i;
		return (val);
	}

	ch = *s->str++;
	switch (ch) {
		case 'a':			/* escape characters */
			return ('\7');
		case 'b':
			return ('\b');
		case 'f':
			return ('\f');
		case 'n':
			return ('\n');
		case 'r':
			return ('\r');
		case 't':
			return ('\t');
		case 'v':
			return ('\13');
		default:			/* \x" -> x */
			return (ch);
	}
}
