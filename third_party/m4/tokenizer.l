%{
/* $OpenBSD: tokenizer.l,v 1.10 2017/06/17 01:55:16 bcallah Exp $ */
/*
 * Copyright (c) 2004 Marc Espie <espie@cvs.openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include "third_party/m4/parser.h"

extern void m4_warnx(const char *, ...);
extern int mimic_gnu;
extern int32_t yylval;

int32_t number(void);
int32_t parse_radix(void);
%}

delim 	[ \t\n]
ws	{delim}+
hex	0[xX][0-9a-fA-F]+
oct	0[0-7]*
dec	[1-9][0-9]*
radix	0[rR][0-9]+:[0-9a-zA-Z]+

%option noyywrap

%%
{ws}			{/* just skip it */}
{hex}|{oct}|{dec}	{ yylval = number(); return(NUMBER); }
{radix}			{ if (mimic_gnu) {
				yylval = parse_radix(); return(NUMBER);
			  } else {
			  	return(ERROR);
			  }
			}
"<="			{ return(LE); }
">="			{ return(GE); }
"<<"			{ return(LSHIFT); }
">>"			{ return(RSHIFT); }
"=="			{ return(EQ); }
"!="			{ return(NE); }
"&&"			{ return(LAND); }
"||"			{ return(LOR); }
"**"			{ if (mimic_gnu) { return (EXPONENT); } }
.			{ return yytext[0]; }
%%

int32_t
number()
{
	long l;

	errno = 0;
	l = strtol(yytext, NULL, 0);
	if (((l == LONG_MAX || l == LONG_MIN) && errno == ERANGE) ||
	    l > INT32_MAX || l < INT32_MIN)
		m4_warnx("numeric overflow in expr: %s", yytext);
	return l;
}

int32_t
parse_radix()
{
	long base;
	char *next = NULL;
	long l;
	int d;

	l = 0;
	base = strtol(yytext+2, &next, 0);
	if (base > 36 || next == NULL) {
		m4_warnx("error in number %s", yytext);
	} else {
		next++;
		while (*next != 0) {
			if (*next >= '0' && *next <= '9')
				d = *next - '0';
			else if (*next >= 'a' && *next <= 'z')
				d = *next - 'a' + 10;
			else {
				assert(*next >= 'A' && *next <= 'Z');
				d = *next - 'A' + 10;
			}
			if (d >= base) {
				m4_warnx("error in number %s", yytext);
				return 0;
			}
			l = base * l + d;
			next++;
		}
	}
	return l;
}

