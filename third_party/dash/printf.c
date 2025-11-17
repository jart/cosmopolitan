/*
 * Copyright (c) 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 * Copyright (c) 1997-2005
 *	Herbert Xu <herbert@gondor.apana.org.au>.  All rights reserved.
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

#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

static int	 conv_escape_str(char *, char **);
static int	 getchr(void);
static double	 getdouble(void);
static uintmax_t getuintmax(int);
static char	*getstr(void);
static char	*mklong(const char *, const char *);
static void      check_conversion(const char *, const char *);

static int	rval;
static char  **gargv;

#define isodigit(c)	((c) >= '0' && (c) <= '7')
#define octtobin(c)	((c) - '0')

#include "bltin.h"
#include "parser.h"
#include "system.h"

#define PF(f, func) { \
	switch ((char *)param - (char *)array) { \
	default: \
		(void)printf(f, array[0], array[1], func); \
		break; \
	case sizeof(*param): \
		(void)printf(f, array[0], func); \
		break; \
	case 0: \
		(void)printf(f, func); \
		break; \
	} \
}

#define ASPF(sp, f, func) ({ \
	int ret; \
	switch ((char *)param - (char *)array) { \
	default: \
		ret = xasprintf(sp, f, array[0], array[1], func); \
		break; \
	case sizeof(*param): \
		ret = xasprintf(sp, f, array[0], func); \
		break; \
	case 0: \
		ret = xasprintf(sp, f, func); \
		break; \
	} \
	ret; \
})


static int print_escape_str(const char *f, int *param, int *array, char *s)
{
	struct stackmark smark;
	char *p, *q;
	int done;
	int len;
	int total;

	setstackmark(&smark);
	done = conv_escape_str(s, &q);
	p = stackblock();
	len = q - p;
	total = len - 1;

	q[-1] = (!!((f[1] - 's') | done) - 1) & f[2];
	total += !!q[-1];
	if (f[1] == 's')
		goto easy;

	p = makestrspace(len, q);
	memset(p, 'X', total);
	p[total] = 0;

	q = stackblock();
	total = ASPF(&p, f, p);

	len = strchrnul(p, 'X') - p;
	memcpy(p + len, q, strspn(p + len, "X"));

easy:
	out1mem(p, total);

	popstackmark(&smark);
	return done;
}


int printfcmd(int argc, char *argv[])
{
	char *fmt;
	char *format;
	int ch;

	rval = 0;

	nextopt(nullstr);

	argv = argptr;
	format = *argv;

	if (!format)
		error("usage: printf format [arg ...]");

	gargv = ++argv;

#define SKIP1	"#-+ 0"
#define SKIP2	"*0123456789"
	do {
		/*
		 * Basic algorithm is to scan the format string for conversion
		 * specifications -- once one is found, find out if the field
		 * width or precision is a '*'; if it is, gather up value. 
		 * Note, format strings are reused as necessary to use up the
		 * provided arguments, arguments of zero/null string are 
		 * provided to use up the format string.
		 */

		/* find next format specification */
		for (fmt = format; (ch = *fmt++) ;) {
			char *start;
			char nextch;
			int array[2];
			int *param;

			if (ch == '\\') {
				unsigned ret;
				char *cp;

				STARTSTACKSTR(cp);
				CHECKSTRSPACE(4, cp);
				ret = conv_escape(fmt, cp, false);
				fmt += ret >> 4;
				out1mem(cp, ret & 15);
				continue;
			}
			if (ch != '%' || (*fmt == '%' && (++fmt || 1))) {
				putchar(ch);
				continue;
			}

			/* Ok - we've found a format specification,
			   Save its address for a later printf(). */
			start = fmt - 1;
			param = array;

			/* skip to field width */
			fmt += strspn(fmt, SKIP1);
			if (*fmt == '*') {
				++fmt;
				*param++ = getuintmax(1);
			} else {
				/* skip to possible '.',
				 * get following precision
				 */
				fmt += strspn(fmt, SKIP2);
			}

			if (*fmt == '.') {
				++fmt;
				if (*fmt == '*') {
					++fmt;
					*param++ = getuintmax(1);
				} else
					fmt += strspn(fmt, SKIP2);
			}

			ch = *fmt;
			if (!ch)
				error("missing format character");
			/* null terminate format string to we can use it
			   as an argument to printf. */
			nextch = fmt[1];
			fmt[1] = 0;
			switch (ch) {

			case 'b':
				*fmt = 's';
				/* escape if a \c was encountered */
				if (print_escape_str(start, param, array,
						     getstr()))
					goto out;
				*fmt = 'b';
				break;
			case 'c': {
				int p = getchr();
				PF(start, p);
				break;
			}
			case 's': {
				char *p = getstr();
				PF(start, p);
				break;
			}
			case 'd':
			case 'i': {
				uintmax_t p = getuintmax(1);
				start = mklong(start, fmt);
				PF(start, p);
				break;
			}
			case 'o':
			case 'u':
			case 'x':
			case 'X': {
				uintmax_t p = getuintmax(0);
				start = mklong(start, fmt);
				PF(start, p);
				break;
			}
			case 'a':
			case 'A':
			case 'e':
			case 'E':
			case 'f':
			case 'F':
			case 'g':
			case 'G': {
				double p = getdouble();
				PF(start, p);
				break;
			}
			default:
				error("%s: invalid directive", start);
			}
			*++fmt = nextch;
		}
	} while (gargv != argv && *gargv);

out:
	return rval;
}


/*
 * Print SysV echo(1) style escape string 
 *	Halts processing string if a \c escape is encountered.
 */
static int
conv_escape_str(char *str, char **sp)
{
	char *cp;
	int c;

	/* convert string into a temporary buffer... */
	STARTSTACKSTR(cp);

	do {
		unsigned ret;
		int ch;

		CHECKSTRSPACE(4, cp);

		c = *str++;
		if (c != '\\') {
putchar:
			USTPUTC(c, cp);
			continue;
		}

		ch = *str;
		if (ch == 'c') {
			/* \c as in SYSV echo - abort all processing.... */
			c = 0x100;
			goto putchar;
		}

		/* 
		 * %b string octal constants are not like those in C.
		 * They start with a \0, and are followed by 0, 1, 2, 
		 * or 3 octal digits. 
		 */
		if (ch == '0' && isodigit(str[1]))
			str++;

		/* Finally test for sequences valid in the format string */
		ret = conv_escape(str, cp, false);
		str += ret >> 4;
		cp += ret & 15;
	} while (c & 0xff);

	*sp = cp;

	return c;
}

/*
 * Print "standard" escape characters 
 */
unsigned conv_escape(char *str0, char *out0, bool mbchar)
{
	char *out = out0;
	char *str = str0;
	unsigned value;
	int ch;

	ch = *str;
	value = ch;

	switch (ch) {
	default:
		if (ch == '"' || ch == '\'')
			break;

		if (ch == 'U') {
			ch = 8;
			goto hex;
		}

		value = '\\';

		if (isodigit(ch)) {
			ch = 3;
			value = 0;
			do {
				value <<= 3;
				value += octtobin(*str++);
			} while (--ch && isodigit(*str));
		}

		str--;
		break;

	case 'x':
		ch = 2;

hex:
		value = 0;
		do {
			int c = *++str;
			int d;

			if (c >= '0' && c <= '9')
				d = c - '0';
			else {
				int cl;

				cl = c & ~0x20;
				if (cl >= 'A' && cl <= 'F')
					d = cl - 'A' + 10;
				else {
					str--;
					break;
				}
			}

			value <<= 4;
			value += d;
		} while (--ch);

		if (value < 0x80)
			break;

		if (value < 0x110000) {
			int mboff = (mbchar - 1) * 2;
			unsigned uni = value;
			int len;

			value = 0x80 << 8 | (value & 0xfc0) << 2 |
				0x80 | (value & 0x3f);

			if (uni < 0x800) {
				value |= 0x40 << 8;
				len = 2;
			} else {
				value |= 0x80 << 16 | (uni & 0x3f000) << 4;
				if (uni < 0x10000) {
					value |= 0x60 << 16;
					len = 3;
				} else {
					value |= 0xf0 << 24 |
						 (uni & ~0x3ffff) << 6;
					len = 4;
				}
			}

			value = htonl(value << (4 - len) * 8);

			USTPUTC(CTLMBCHAR, out);
			USTPUTC(len, out);
			STADJUST(mboff, out);
			*(uint32_t *)out = value;
			STADJUST(len, out);
			USTPUTC(len, out);
			USTPUTC(CTLMBCHAR, out);
			STADJUST(mboff, out);
		}

		goto out_noput;

	case 'u':
		ch = 4;
		goto hex;

	case '\\':
		break;

	case 'a':				/* alert */
	case 'b':				/* backspace */
	case 'f':				/* form-feed */
		value -= 'a';
		value += '\a';
		break;

	case 'e':	value = '\033';	break;	/* <ESC> */
	case 'n':	value = '\n';	break;	/* newline */
	case 'r':	value = '\r';	break;	/* carriage-return */
	case 't':	value = '\t';	break;	/* tab */
	case 'v':	value = '\v';	break;	/* vertical-tab */
	}

	USTPUTC(value, out);

out_noput:
	str++;
	return (out - out0) | (str - str0) << 4;
}

static char *
mklong(const char *str, const char *ch)
{
	/*
	 * Replace a string like "%92.3u" with "%92.3"PRIuMAX.
	 *
	 * Although C99 does not guarantee it, we assume PRIiMAX,
	 * PRIoMAX, PRIuMAX, PRIxMAX, and PRIXMAX are all the same
	 * as PRIdMAX with the final 'd' replaced by the corresponding
	 * character.
	 */

	char *copy;
	size_t len;	

	len = ch - str + sizeof(PRIdMAX);
	STARTSTACKSTR(copy);
	copy = makestrspace(len, copy);
	memcpy(copy, str, len - sizeof(PRIdMAX));
	memcpy(copy + len - sizeof(PRIdMAX), PRIdMAX, sizeof(PRIdMAX));
	copy[len - 2] = *ch;
	return (copy);	
}

static int
getchr(void)
{
	int val = 0;

	if (*gargv)
		val = **gargv++;
	return val;
}

static char *
getstr(void)
{
	char *val = nullstr;

	if (*gargv)
		val = *gargv++;
	return val;
}

static uintmax_t
getuintmax(int sign)
{
	uintmax_t val = 0;
	char *cp, *ep;

	cp = *gargv;
	if (cp == NULL)
		goto out;
	gargv++;

	val = (unsigned char) cp[1];
	if (*cp == '\"' || *cp == '\'')
		goto out;

	errno = 0;
	val = sign ? strtoimax(cp, &ep, 0) : strtoumax(cp, &ep, 0);
	check_conversion(cp, ep);
out:
	return val;
}

static double
getdouble(void)
{
	double val;
	char *cp, *ep;

	cp = *gargv;
	if (cp == NULL)
		return 0;
	gargv++;

	if (*cp == '\"' || *cp == '\'')
		return (unsigned char) cp[1];

	errno = 0;
	val = strtod(cp, &ep);
	check_conversion(cp, ep);
	return val;
}

static void
check_conversion(const char *s, const char *ep)
{
	if (*ep) {
		if (ep == s)
			warnx("%s: expected numeric value", s);
		else
			warnx("%s: not completely converted", s);
		rval = 1;
	} else if (errno == ERANGE) {
		warnx("%s: %s", s, strerror(ERANGE));
		rval = 1;
	}
}

int
echocmd(int argc, char **argv)
{
	const char *lastfmt = snlfmt;
	int nonl;

	if (*++argv && equal(*argv, "-n")) {
		argv++;
		lastfmt = "%s";
	}

	do {
		const char *fmt = "%s ";
		char *s = *argv;

		if (!s || !*++argv)
			fmt = lastfmt;

		nonl = print_escape_str(fmt, NULL, NULL, s ?: nullstr);
	} while (!nonl && *argv);
	return 0;
}
