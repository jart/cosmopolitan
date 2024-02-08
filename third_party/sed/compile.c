/*	$NetBSD: compile.c,v 1.48 2019/10/05 20:23:55 christos Exp $	*/

/*-
 * Copyright (c) 1992 Diomidis Spinellis.
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Diomidis Spinellis of Imperial College, University of London.
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

#include "libc/calls/makedev.h"
#include "libc/calls/weirdtypes.h"
#include "libc/intrin/newbie.h"
#include "libc/calls/typedef/u.h"
#include "libc/calls/weirdtypes.h"
#include "libc/sock/select.h"
#include "libc/sysv/consts/endian.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/stat.macros.h"
#include "libc/calls/weirdtypes.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/consts/utime.h"
#include "libc/mem/gc.h"
#include "libc/time/time.h"

#include "libc/str/str.h"
#include "libc/log/bsd.h"
#include "libc/errno.h"
#include "libc/calls/calls.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/o.h"
#include "libc/limits.h"
#include "libc/sysv/consts/_posix.h"
#include "third_party/regex/regex.h"
#include "libc/calls/calls.h"
#include "libc/stdio/stdio.h"
#include "libc/temp.h"
#include "libc/mem/alg.h"
#include "libc/fmt/conv.h"
#include "libc/mem/mem.h"
#include "libc/stdio/rand.h"
#include "libc/runtime/runtime.h"
#include "libc/temp.h"
#include "libc/sysv/consts/exit.h"
#include "third_party/gdtoa/gdtoa.h"
#include "libc/mem/alg.h"
#include "libc/str/str.h"
#include "libc/str/str.h"
#include "libc/time/time.h"

#include "third_party/sed/defs.h"
#include "third_party/sed/extern.h"

#define LHSZ	128
#define	LHMASK	(LHSZ - 1)
static struct labhash {
	struct	labhash *lh_next;
	u_int	lh_hash;
	struct	s_command *lh_cmd;
	int	lh_ref;
} *labels[LHSZ];

static char	 *compile_addr(char *, struct s_addr *);
static char	 *compile_ccl(char **, char *);
static char	 *compile_delimited(char *, char *, int);
static char	 *compile_flags(char *, struct s_subst *);
static regex_t	 *compile_re(char *, int);
static char	 *compile_subst(char *, struct s_subst *);
static char	 *compile_text(void);
static char	 *compile_tr(char *, struct s_tr **);
static struct s_command
		**compile_stream(struct s_command **);
static char	 *duptoeol(char *, const char *);
static void	  enterlabel(struct s_command *);
static struct s_command
		 *findlabel(char *);
static void	  fixuplabel(struct s_command *, struct s_command *);
static void	  uselabel(void);
static void	  parse_escapes(char *);

/*
 * Command specification.  This is used to drive the command parser.
 */
struct s_format {
	char code;				/* Command code */
	int naddr;				/* Number of address args */
	enum e_args args;			/* Argument type */
};

static struct s_format cmd_fmts[] = {
	{'{', 2, GROUP},
	{'}', 0, ENDGROUP},
	{'a', 1, TEXT},
	{'b', 2, BRANCH},
	{'c', 2, TEXT},
	{'d', 2, EMPTY},
	{'D', 2, EMPTY},
	{'g', 2, EMPTY},
	{'G', 2, EMPTY},
	{'h', 2, EMPTY},
	{'H', 2, EMPTY},
	{'i', 1, TEXT},
	{'l', 2, EMPTY},
	{'n', 2, EMPTY},
	{'N', 2, EMPTY},
	{'p', 2, EMPTY},
	{'P', 2, EMPTY},
	{'q', 1, EMPTY},
	{'r', 1, RFILE},
	{'s', 2, SUBST},
	{'t', 2, BRANCH},
	{'w', 2, WFILE},
	{'x', 2, EMPTY},
	{'y', 2, TR},
	{'!', 2, NONSEL},
	{':', 0, LABEL},
	{'#', 0, COMMENT},
	{'=', 1, EMPTY},
	{'\0', 0, COMMENT},
};

/* The compiled program. */
struct s_command *prog;

/*
 * Compile the program into prog.
 * Initialise appends.
 */
void
compile(void)
{
	*compile_stream(&prog) = NULL;
	fixuplabel(prog, NULL);
	uselabel();
	if (appendnum > 0)
		appends_ = xmalloc(sizeof(struct s_appends) * appendnum);
	g_match = xmalloc((maxnsub + 1) * sizeof(regmatch_t));
}

#define EATSPACE() do {							\
	if (p)								\
		while (*p && isspace((unsigned char)*p))                \
			p++;						\
	} while (0)

static struct s_command **
compile_stream(struct s_command **link)
{
	char *p;
	static char lbuf[_POSIX2_LINE_MAX + 1];	/* To save stack */
	struct s_command *cmd, *cmd2, *stack;
	struct s_format *fp;
	char *re = gc(malloc(_POSIX2_LINE_MAX + 1));
	int naddr;				/* Number of addresses */

	stack = 0;
	for (;;) {
		if ((p = cu_fgets(lbuf, sizeof(lbuf), NULL)) == NULL) {
			if (stack != 0)
				errx(1, "%lu: %s: unexpected EOF (pending }'s)",
							linenum, fname);
			return (link);
		}

semicolon:	EATSPACE();
		if (p) {
			if (*p == '#' || *p == '\0')
				continue;
			else if (*p == ';') {
				p++;
				goto semicolon;
			}
		}
		*link = cmd = xmalloc(sizeof(struct s_command));
		link = &cmd->next;
		cmd->startline = cmd->nonsel = 0;
		/* First parse the addresses */
		naddr = 0;

/* Valid characters to start an address */
#define	addrchar(c)	(strchr("0123456789/\\$", (c)))
		if (addrchar(*p)) {
			naddr++;
			cmd->a1 = xmalloc(sizeof(struct s_addr));
			p = compile_addr(p, cmd->a1);
			EATSPACE();				/* EXTENSION */
			if (*p == ',') {
				p++;
				EATSPACE();			/* EXTENSION */
				naddr++;
				cmd->a2 = xmalloc(sizeof(struct s_addr));
				p = compile_addr(p, cmd->a2);
				EATSPACE();
			} else
				cmd->a2 = 0;
		} else
			cmd->a1 = cmd->a2 = 0;

nonsel:		/* Now parse the command */
		if (!*p)
			errx(1, "%lu: %s: command expected", linenum, fname);
		cmd->code = *p;
		for (fp = cmd_fmts; fp->code; fp++)
			if (fp->code == *p)
				break;
		if (!fp->code)
			errx(1, "%lu: %s: invalid command code %c", linenum, fname, *p);
		if (naddr > fp->naddr)
			errx(1,
				"%lu: %s: command %c expects up to %d address(es), found %d",
				linenum, fname, *p, fp->naddr, naddr);
		switch (fp->args) {
		case NONSEL:			/* ! */
			p++;
			EATSPACE();
			cmd->nonsel = ! cmd->nonsel;
			goto nonsel;
		case GROUP:			/* { */
			p++;
			EATSPACE();
			cmd->next = stack;
			stack = cmd;
			link = &cmd->u.c;
			if (*p)
				goto semicolon;
			break;
		case ENDGROUP:
			/*
			 * Short-circuit command processing, since end of
			 * group is really just a noop.
			 */
			cmd->nonsel = 1;
			if (stack == 0)
				errx(1, "%lu: %s: unexpected }", linenum, fname);
			cmd2 = stack;
			stack = cmd2->next;
			cmd2->next = cmd;
			/*FALLTHROUGH*/
		case EMPTY:		/* d D g G h H l n N p P q x = \0 */
			p++;
			EATSPACE();
			switch (*p) {
			case ';':
				p++;
				link = &cmd->next;
				goto semicolon;
			case '}':
				goto semicolon;
			case '\0':
				break;
			default:
				errx(1, "%lu: %s: extra characters at the end of %c command",
						linenum, fname, cmd->code);
			}
			break;
		case TEXT:			/* a c i */
			p++;
			EATSPACE();
			if (*p != '\\')
				errx(1,
"%lu: %s: command %c expects \\ followed by text", linenum, fname, cmd->code);
			p++;
			EATSPACE();
			if (*p)
				errx(1,
				"%lu: %s: extra characters after \\ at the end of %c command",
				linenum, fname, cmd->code);
			cmd->t = compile_text();
			break;
		case COMMENT:			/* \0 # */
			break;
		case WFILE:			/* w */
			p++;
			EATSPACE();
			if (*p == '\0')
				errx(1, "%lu: %s: filename expected", linenum, fname);
			cmd->t = duptoeol(p, "w command");
			if (aflag)
				cmd->u.fd = -1;
			else if ((cmd->u.fd = open(p,
			    O_WRONLY|O_APPEND|O_CREAT|O_TRUNC,
			    DEFFILEMODE)) == -1)
				err(1, "%s", p);
			break;
		case RFILE:			/* r */
			p++;
			EATSPACE();
			if (*p == '\0')
				errx(1, "%lu: %s: filename expected", linenum, fname);
			else
				cmd->t = duptoeol(p, "read command");
			break;
		case BRANCH:			/* b t */
			p++;
			EATSPACE();
			if (*p == '\0')
				cmd->t = NULL;
			else
				cmd->t = duptoeol(p, "branch");
			break;
		case LABEL:			/* : */
			p++;
			EATSPACE();
			cmd->t = duptoeol(p, "label");
			if (strlen(p) == 0)
				errx(1, "%lu: %s: empty label", linenum, fname);
			enterlabel(cmd);
			break;
		case SUBST:			/* s */
			p++;
			if (*p == '\0' || *p == '\\')
				errx(1,
"%lu: %s: substitute pattern can not be delimited by newline or backslash",
					linenum, fname);
			cmd->u.s = xcalloc(1, sizeof(struct s_subst));
			p = compile_delimited(p, re, 0);
			if (p == NULL)
				errx(1,
				"%lu: %s: unterminated substitute pattern", linenum, fname);

			/* Compile RE with no case sensitivity temporarily */
			if (*re == '\0')
				cmd->u.s->re = NULL;
			else
				cmd->u.s->re = compile_re(re, 0);
			--p;
			p = compile_subst(p, cmd->u.s);
			p = compile_flags(p, cmd->u.s);

			/* Recompile RE with case sensitivity from "I" flag if any */
			if (*re == '\0')
				cmd->u.s->re = NULL;
			else
				cmd->u.s->re = compile_re(re, cmd->u.s->icase);
			EATSPACE();
			if (*p == ';') {
				p++;
				link = &cmd->next;
				goto semicolon;
			}
			break;
		case TR:			/* y */
			p++;
			p = compile_tr(p, &cmd->u.y);
			EATSPACE();
			switch (*p) {
			case ';':
				p++;
				link = &cmd->next;
				goto semicolon;
			case '}':
				goto semicolon;
			case '\0':
				break;
			default:
				errx(1,
"%lu: %s: extra text at the end of a transform command", linenum, fname);
			}
			if (*p)
			break;
		}
	}
}

/*
 * Get a delimited string.  P points to the delimeter of the string; d points
 * to a buffer area.  Newline and delimiter escapes are processed; other
 * escapes are ignored.
 *
 * Returns a pointer to the first character after the final delimiter or NULL
 * in the case of a non-terminated string.  The character array d is filled
 * with the processed string.
 */
static char *
compile_delimited(char *p, char *d, int is_tr)
{
	char c;

	c = *p++;
	if (c == '\0')
		return (NULL);
	else if (c == '\\')
		errx(1, "%lu: %s: \\ can not be used as a string delimiter",
				linenum, fname);
	else if (c == '\n')
		errx(1, "%lu: %s: newline can not be used as a string delimiter",
				linenum, fname);
	while (*p) {
		if (*p == '[' && *p != c) {
			if ((d = compile_ccl(&p, d)) == NULL)
				errx(1, "%lu: %s: unbalanced brackets ([])", linenum, fname);
			continue;
		} else if (*p == '\\' && p[1] == '[') {
			*d++ = *p++;
		} else if (*p == '\\' && p[1] == c)
			p++;
		else if (*p == '\\' && p[1] == 'n') {
			*d++ = '\n';
			p += 2;
			continue;
		} else if (*p == '\\' && p[1] == '\\') {
			if (is_tr)
				p++;
			else
				*d++ = *p++;
		} else if (*p == c) {
			*d = '\0';
			return (p + 1);
		}
		*d++ = *p++;
	}
	return (NULL);
}


/* compile_ccl: expand a POSIX character class */
static char *
compile_ccl(char **sp, char *t)
{
	int c, d;
	char *s = *sp;

	*t++ = *s++;
	if (*s == '^')
		*t++ = *s++;
	if (*s == ']')
		*t++ = *s++;
	for (; *s && (*t = *s) != ']'; s++, t++)
		if (*s == '[' && ((d = *(s+1)) == '.' || d == ':' || d == '=')) {
			*++t = *++s, t++, s++;
			for (c = *s; (*t = *s) != ']' || c != d; s++, t++)
				if ((c = *s) == '\0')
					return NULL;
		}
	return (*s == ']') ? *sp = ++s, ++t : NULL;
}

/*
 * Compiles the regular expression in RE and returns a pointer to the compiled
 * regular expression.
 * Cflags are passed to regcomp.
 */
static regex_t *
compile_re(char *re, int case_insensitive)
{
	regex_t *rep;
	int eval, flags;


	flags = rflags;
	if (case_insensitive)
		flags |= REG_ICASE;
	rep = xmalloc(sizeof(regex_t));
	parse_escapes(re);
	if ((eval = regcomp(rep, re, flags)) != 0)
		errx(1, "%lu: %s: RE error: %s",
				linenum, fname, strregerror(eval, rep));
	if (maxnsub < rep->re_nsub)
		maxnsub = rep->re_nsub;
	return (rep);
}

static char
cton(char c, int base)
{
	switch (c) {
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7':
		return (char)(c - '0');
	case '8': case '9':
		return base == 8 ? '?' : (char)(c - '0');
	case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
		return base == 16 ? (char)(c - 'a' + 10) : '?'; 
	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
		return base == 16 ? (char)(c - 'A' + 10) : '?'; 
	default:
		return '?';
	}
}

static int
ston(char **pp, char *sp, int base)
{
	char *p = *pp, n;
	int r = cton(p[1], base);

	if (r == '?')
		return 0;

	p++;
	while ((n = cton(p[1], base)) != '?' && r < 255) {
		r = r * base + n;
		p++;
	}
	*sp = (char)r;
	*pp = p;
	return 1;
}
		
static int
unescape(char **pp, char **spp)
{
	char *p = *pp;
	char *sp = *spp;

	switch (*p) {
	case 'o':
		if (!ston(&p, sp, 8))
			return 0;
		break;
	case 'd':
		if (!ston(&p, sp, 10))
			return 0;
		break;
	case 'x':
		if (!ston(&p, sp, 16))
			return 0;
		break;
	case 'a':
		*sp = '\a';
		p++;
		break;
#if 0
	// No, \b RE
	case 'b':
		*sp = '\b';
		break;
#endif
	case 'f':
		*sp = '\f';
		break;
	case 'n':
		*sp = '\n';
		break;
	case 'r':
		*sp = '\r';
		break;
	case 't':
		*sp = '\t';
		break;
	case 'v':
		*sp = '\v';
		break;
	case 'e':
		*sp = '\e';
		break;
	default:
		return 0;
	}
	*spp = sp + 1;
	*pp = p;
	return 1;
}

static void
parse_escapes(char *buf)
{
	char bracket = '\0';
	char *p, *q;

	p = q = buf;

	for (p = q = buf; *p; p++) {
		if (*p == '\\' && p[1] && !bracket) {
			p++;
			if (unescape(&p, &q))
				continue;
			*q++ = '\\';
		}
		switch (*p) {
		case '[':
			if (!bracket)
				bracket = *p;
			break;
		case '.':
		case ':':
		case '=':
			if (bracket == '[' && p[-1] == '[')
				bracket = *p;
			break;
		case ']':
			if (!bracket)
			    break;
			if (bracket == '[')
				bracket = '\0';
			else if (p[-2] != bracket && p[-1] == bracket)
				bracket = '[';
			break;
		default:
			break;
		}
		*q++ = *p;
	}
	*q = '\0';
}

/*
 * Compile the substitution string of a regular expression and set res to
 * point to a saved copy of it.  Nsub is the number of parenthesized regular
 * expressions.
 */
static char *
compile_subst(char *p, struct s_subst *s)
{
	static char lbuf[_POSIX2_LINE_MAX + 1];
	size_t asize, size;
	u_char ref;
	char c, *text, *op, *sp;
	int more = 1, sawesc = 0;

	c = *p++;			/* Terminator character */
	if (c == '\0')
		return (NULL);

	s->maxbref = 0;
	s->linenum = linenum;
	asize = 2 * _POSIX2_LINE_MAX + 1;
	text = xmalloc(asize);
	size = 0;
	do {
		op = sp = text + size;
		for (; *p; p++) {
			if (*p == '\\' || sawesc) {
				/*
				 * If this is a continuation from the last
				 * buffer, we won't have a character to
				 * skip over.
				 */
				if (sawesc)
					sawesc = 0;
				else
					p++;

				switch (*p) {
				case '\0':
					/*
					 * This escaped character is continued
					 * in the next part of the line.  Note
					 * this fact, then cause the loop to
					 * exit w/ normal EOL case and reenter
					 * above with the new buffer.
					 */
					sawesc = 1;
					p--;
					continue;
				case '0': case '1': case '2': case '3':
				case '4': case '5': case '6': case '7':
				case '8': case '9':
					*sp++ = '\\';
					ref = (u_char)(*p - '0');
					if (s->re != NULL &&
					    ref > s->re->re_nsub)
						errx(1, "%lu: %s: \\%c not defined in the RE",
								linenum, fname, *p);
					if (s->maxbref < ref)
						s->maxbref = ref;
					break;
				case '&':
				case '\\':
					*sp++ = '\\';
					break;
				default:
					if (unescape(&p, &sp))
						continue;
					break;
				}
			} else if (*p == c) {
				if (*++p == '\0' && more) {
					if (cu_fgets(lbuf, sizeof(lbuf), &more))
						p = lbuf;
				}
				*sp++ = '\0';
				size += (size_t)(sp - op);
				s->new = xrealloc(text, size);
				return (p);
			} else if (*p == '\n') {
				errx(1,
"%lu: %s: unescaped newline inside substitute pattern", linenum, fname);
				/* NOTREACHED */
			}
			*sp++ = *p;
		}
		size += (size_t)(sp - op);
		if (asize - size < _POSIX2_LINE_MAX + 1) {
			asize *= 2;
			text = xrealloc(text, asize);
		}
	} while (cu_fgets(p = lbuf, sizeof(lbuf), &more));
	errx(1, "%lu: %s: unterminated substitute in regular expression",
			linenum, fname);
	/* NOTREACHED */
}

/*
 * Compile the flags of the s command
 */
static char *
compile_flags(char *p, struct s_subst *s)
{
	int gn;			/* True if we have seen g or n */
	unsigned long nval;
	char wfile[_POSIX2_LINE_MAX + 1], *q;

	s->n = 1;				/* Default */
	s->p = 0;
	s->wfile = NULL;
	s->wfd = -1;
	s->icase = 0;
	for (gn = 0;;) {
		EATSPACE();			/* EXTENSION */
		switch (*p) {
		case 'g':
			if (gn)
				errx(1,
"%lu: %s: more than one number or 'g' in substitute flags", linenum, fname);
			gn = 1;
			s->n = 0;
			break;
		case '\0':
		case '\n':
		case ';':
			return (p);
		case 'p':
			s->p = 1;
			break;
		case 'i':
		case 'I':
			s->icase = 1;
			break;
		case '1': case '2': case '3':
		case '4': case '5': case '6':
		case '7': case '8': case '9':
			if (gn)
				errx(1,
"%lu: %s: more than one number or 'g' in substitute flags", linenum, fname);
			gn = 1;
			errno = 0;
			nval = strtoul(p, &p, 10);
			if (errno == ERANGE || nval > INT_MAX)
				errx(1,
"%lu: %s: overflow in the 'N' substitute flag", linenum, fname);
			s->n = (int)nval;
			p--;
			break;
		case 'w':
			p++;
#ifdef HISTORIC_PRACTICE
			if (*p != ' ') {
				warnx("%lu: %s: space missing before w wfile", linenum, fname);
				return (p);
			}
#endif
			EATSPACE();
			q = wfile;
			while (*p) {
				if (*p == '\n')
					break;
				*q++ = *p++;
			}
			*q = '\0';
			if (q == wfile)
				errx(1, "%lu: %s: no wfile specified", linenum, fname);
			s->wfile = strdup(wfile);
			if (!aflag && (s->wfd = open(wfile,
			    O_WRONLY|O_APPEND|O_CREAT|O_TRUNC,
			    DEFFILEMODE)) == -1)
				err(1, "%s", wfile);
			return (p);
		default:
			errx(1, "%lu: %s: bad flag in substitute command: '%c'",
					linenum, fname, *p);
			break;
		}
		p++;
	}
}

/*
 * Compile a translation set of strings into a lookup table.
 */
static char *
compile_tr(char *p, struct s_tr **py)
{
	struct s_tr *y;
	size_t i;
	const char *op, *np;
	char *old = gc(malloc(_POSIX2_LINE_MAX + 1));
	char *new = gc(malloc(_POSIX2_LINE_MAX + 1));
	size_t oclen, oldlen, nclen, newlen;
	mbstate_t mbs1, mbs2;

	*py = y = xmalloc(sizeof(*y));
	y->multis = NULL;
	y->nmultis = 0;

	if (*p == '\0' || *p == '\\')
		errx(1,
	"%lu: %s: transform pattern can not be delimited by newline or backslash",
			linenum, fname);
	p = compile_delimited(p, old, 1);
	if (p == NULL)
		errx(1, "%lu: %s: unterminated transform source string",
				linenum, fname);
	p = compile_delimited(p - 1, new, 1);
	if (p == NULL)
		errx(1, "%lu: %s: unterminated transform target string",
				linenum, fname);
	EATSPACE();
	op = old;
	oldlen = mbsrtowcs(NULL, &op, 0, NULL);
	if (oldlen == (size_t)-1)
		err(1, "");
	np = new;
	newlen = mbsrtowcs(NULL, &np, 0, NULL);
	if (newlen == (size_t)-1)
		err(1, "");
	if (newlen != oldlen)
		errx(1, "%lu: %s: transform strings are not the same length",
				linenum, fname);
	if (MB_CUR_MAX == 1) {
		/*
		 * The single-byte encoding case is easy: generate a
		 * lookup table.
		 */
		for (i = 0; i <= UCHAR_MAX; i++)
			y->bytetab[i] = (u_char)i;
		for (; *op; op++, np++)
			y->bytetab[(u_char)*op] = (u_char)*np;
	} else {
		/*
		 * Multi-byte encoding case: generate a lookup table as
		 * above, but only for single-byte characters. The first
		 * bytes of multi-byte characters have their lookup table
		 * entries set to 0, which causes do_tr() to search through
		 * an auxiliary vector of multi-byte mappings.
		 */
		memset(&mbs1, 0, sizeof(mbs1));
		memset(&mbs2, 0, sizeof(mbs2));
		for (i = 0; i <= UCHAR_MAX; i++)
			y->bytetab[i] = (u_char)((btowc((int)i) != WEOF) ? i : 0);
		while (*op != '\0') {
			oclen = mbrlen(op, MB_LEN_MAX, &mbs1);
			if (oclen == (size_t)-1 || oclen == (size_t)-2)
				err(1, "EILSEQ");
			nclen = mbrlen(np, MB_LEN_MAX, &mbs2);
			if (nclen == (size_t)-1 || nclen == (size_t)-2)
				err(1, "EILSEQ");
			if (oclen == 1 && nclen == 1)
				y->bytetab[(u_char)*op] = (u_char)*np;
			else {
				y->bytetab[(u_char)*op] = 0;
				y->multis = xrealloc(y->multis,
				    (y->nmultis + 1) * sizeof(*y->multis));
				i = y->nmultis++;
				y->multis[i].fromlen = oclen;
				memcpy(y->multis[i].from, op, oclen);
				y->multis[i].tolen = nclen;
				memcpy(y->multis[i].to, np, nclen);
			}
			op += oclen;
			np += nclen;
		}
	}
	return (p);
}

/*
 * Compile the text following an a or i command.
 */
static char *
compile_text(void)
{
	size_t asize, size;
	int esc_nl;
	char *text, *p, *op, *s;
	char lbuf[_POSIX2_LINE_MAX + 1];

	asize = 2 * _POSIX2_LINE_MAX + 1;
	text = xmalloc(asize);
	size = 0;
	while (cu_fgets(lbuf, sizeof(lbuf), NULL)) {
		op = s = text + size;
		p = lbuf;
		for (esc_nl = 0; *p != '\0'; p++) {
			if (*p == '\\' && p[1] != '\0' && *++p == '\n')
				esc_nl = 1;
			*s++ = *p;
		}
		size += (size_t)(s - op);
		if (!esc_nl) {
			*s = '\0';
			break;
		}
		if (asize - size < _POSIX2_LINE_MAX + 1) {
			asize *= 2;
			text = xrealloc(text, asize);
		}
	}
	text[size] = '\0';
	p = xrealloc(text, size + 1);
	return (p);
}

/*
 * Get an address and return a pointer to the first character after
 * it.  Fill the structure pointed to according to the address.
 */
static char *
compile_addr(char *p, struct s_addr *a)
{
	char *end, re[_POSIX2_LINE_MAX + 1];
	int icase;

	icase = 0;

	a->type = 0;
	switch (*p) {
	case '\\':				/* Context address */
		++p;
		/* FALLTHROUGH */
	case '/':				/* Context address */
		p = compile_delimited(p, re, 0);
		if (p == NULL)
			errx(1, "%lu: %s: unterminated regular expression", linenum, fname);
		/* Check for case insensitive regexp flag */
		if (*p == 'I') {
			icase = 1;
			p++;
		}
		if (*re == '\0')
			a->u.r = NULL;
		else
			a->u.r = compile_re(re, icase);
		a->type = AT_RE;
		return (p);

	case '$':				/* Last line */
		a->type = AT_LAST;
		return (p + 1);

	case '+':				/* Relative line number */
		a->type = AT_RELLINE;
		p++;
		/* FALLTHROUGH */
						/* Line number */
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		if (a->type == 0)
			a->type = AT_LINE;
		a->u.l = strtoul(p, &end, 10);
		return (end);
	default:
		errx(1, "%lu: %s: expected context address", linenum, fname);
		return (NULL);
	}
}

/*
 * duptoeol --
 *	Return a copy of all the characters up to \n or \0.
 */
static char *
duptoeol(char *s, const char *ctype)
{
	size_t len;
	int ws;
	char *p, *start;

	ws = 0;
	for (start = s; *s != '\0' && *s != '\n'; ++s)
		ws = isspace((unsigned char)*s);
	*s = '\0';
	if (ws)
		warnx("%lu: %s: whitespace after %s", linenum, fname, ctype);
	len = (size_t)(s - start + 1);
	p = xmalloc(len);
	return (memmove(p, start, len));
}

/*
 * Convert goto label names to addresses, and count a and r commands, in
 * the given subset of the script.  Free the memory used by labels in b
 * and t commands (but not by :).
 *
 * TODO: Remove } nodes
 */
static void
fixuplabel(struct s_command *cp, struct s_command *end)
{

	for (; cp != end; cp = cp->next)
		switch (cp->code) {
		case 'a':
		case 'r':
			appendnum++;
			break;
		case 'b':
		case 't':
			/* Resolve branch target. */
			if (cp->t == NULL) {
				cp->u.c = NULL;
				break;
			}
			if ((cp->u.c = findlabel(cp->t)) == NULL)
				errx(1, "%lu: %s: undefined label '%s'", linenum, fname, cp->t);
			free(cp->t);
			break;
		case '{':
			/* Do interior commands. */
			fixuplabel(cp->u.c, cp->next);
			break;
		}
}

/*
 * Associate the given command label for later lookup.
 */
static void
enterlabel(struct s_command *cp)
{
	struct labhash **lhp, *lh;
	u_char *p;
	u_int h, c;

	for (h = 0, p = (u_char *)cp->t; (c = *p) != 0; p++)
		h = (h << 5) + h + c;
	lhp = &labels[h & LHMASK];
	for (lh = *lhp; lh != NULL; lh = lh->lh_next)
		if (lh->lh_hash == h && strcmp(cp->t, lh->lh_cmd->t) == 0)
			errx(1, "%lu: %s: duplicate label '%s'", linenum, fname, cp->t);
	lh = xmalloc(sizeof *lh);
	lh->lh_next = *lhp;
	lh->lh_hash = h;
	lh->lh_cmd = cp;
	lh->lh_ref = 0;
	*lhp = lh;
}

/*
 * Find the label contained in the command l in the command linked
 * list cp.  L is excluded from the search.  Return NULL if not found.
 */
static struct s_command *
findlabel(char *name)
{
	struct labhash *lh;
	u_char *p;
	u_int h, c;

	for (h = 0, p = (u_char *)name; (c = *p) != 0; p++)
		h = (h << 5) + h + c;
	for (lh = labels[h & LHMASK]; lh != NULL; lh = lh->lh_next) {
		if (lh->lh_hash == h && strcmp(name, lh->lh_cmd->t) == 0) {
			lh->lh_ref = 1;
			return (lh->lh_cmd);
		}
	}
	return (NULL);
}

/*
 * Warn about any unused labels.  As a side effect, release the label hash
 * table space.
 */
static void
uselabel(void)
{
	struct labhash *lh, *next;
	int i;

	for (i = 0; i < LHSZ; i++) {
		for (lh = labels[i]; lh != NULL; lh = next) {
			next = lh->lh_next;
			if (!lh->lh_ref)
				warnx("%lu: %s: unused label '%s'",
				    linenum, fname, lh->lh_cmd->t);
			free(lh);
		}
	}
}
