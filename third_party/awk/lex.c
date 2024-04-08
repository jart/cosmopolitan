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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "awk.h"
#include "awkgram.tab.h"

extern YYSTYPE	yylval;
extern bool	infunc;

int	lineno	= 1;
int	bracecnt = 0;
int	brackcnt  = 0;
int	parencnt = 0;

typedef struct Keyword {
	const char *word;
	int	sub;
	int	type;
} Keyword;

const Keyword keywords[] = {	/* keep sorted: binary searched */
	{ "BEGIN",	XBEGIN,		XBEGIN },
	{ "END",	XEND,		XEND },
	{ "NF",		VARNF,		VARNF },
	{ "atan2",	FATAN,		BLTIN },
	{ "break",	BREAK,		BREAK },
	{ "close",	CLOSE,		CLOSE },
	{ "continue",	CONTINUE,	CONTINUE },
	{ "cos",	FCOS,		BLTIN },
	{ "delete",	DELETE,		DELETE },
	{ "do",		DO,		DO },
	{ "else",	ELSE,		ELSE },
	{ "exit",	EXIT,		EXIT },
	{ "exp",	FEXP,		BLTIN },
	{ "fflush",	FFLUSH,		BLTIN },
	{ "for",	FOR,		FOR },
	{ "func",	FUNC,		FUNC },
	{ "function",	FUNC,		FUNC },
	{ "getline",	GETLINE,	GETLINE },
	{ "gsub",	GSUB,		GSUB },
	{ "if",		IF,		IF },
	{ "in",		IN,		IN },
	{ "index",	INDEX,		INDEX },
	{ "int",	FINT,		BLTIN },
	{ "length",	FLENGTH,	BLTIN },
	{ "log",	FLOG,		BLTIN },
	{ "match",	MATCHFCN,	MATCHFCN },
	{ "next",	NEXT,		NEXT },
	{ "nextfile",	NEXTFILE,	NEXTFILE },
	{ "print",	PRINT,		PRINT },
	{ "printf",	PRINTF,		PRINTF },
	{ "rand",	FRAND,		BLTIN },
	{ "return",	RETURN,		RETURN },
	{ "sin",	FSIN,		BLTIN },
	{ "split",	SPLIT,		SPLIT },
	{ "sprintf",	SPRINTF,	SPRINTF },
	{ "sqrt",	FSQRT,		BLTIN },
	{ "srand",	FSRAND,		BLTIN },
	{ "sub",	SUB,		SUB },
	{ "substr",	SUBSTR,		SUBSTR },
	{ "system",	FSYSTEM,	BLTIN },
	{ "tolower",	FTOLOWER,	BLTIN },
	{ "toupper",	FTOUPPER,	BLTIN },
	{ "while",	WHILE,		WHILE },
};

#define	RET(x)	{ if(dbg)printf("lex %s\n", tokname(x)); return(x); }

static int peek(void)
{
	int c = input();
	unput(c);
	return c;
}

static int gettok(char **pbuf, int *psz)	/* get next input token */
{
	int c, retc;
	char *buf = *pbuf;
	int sz = *psz;
	char *bp = buf;

	c = input();
	if (c == 0)
		return 0;
	buf[0] = c;
	buf[1] = 0;
	if (!isalnum(c) && c != '.' && c != '_')
		return c;

	*bp++ = c;
	if (isalpha(c) || c == '_') {	/* it's a varname */
		for ( ; (c = input()) != 0; ) {
			if (bp-buf >= sz)
				if (!adjbuf(&buf, &sz, bp-buf+2, 100, &bp, "gettok"))
					FATAL( "out of space for name %.10s...", buf );
			if (isalnum(c) || c == '_')
				*bp++ = c;
			else {
				*bp = 0;
				unput(c);
				break;
			}
		}
		*bp = 0;
		retc = 'a';	/* alphanumeric */
	} else {	/* maybe it's a number, but could be . */
		char *rem;
		/* read input until can't be a number */
		for ( ; (c = input()) != 0; ) {
			if (bp-buf >= sz)
				if (!adjbuf(&buf, &sz, bp-buf+2, 100, &bp, "gettok"))
					FATAL( "out of space for number %.10s...", buf );
			if (isdigit(c) || c == 'e' || c == 'E'
			  || c == '.' || c == '+' || c == '-')
				*bp++ = c;
			else {
				unput(c);
				break;
			}
		}
		*bp = 0;
		strtod(buf, &rem);	/* parse the number */
		if (rem == buf) {	/* it wasn't a valid number at all */
			buf[1] = 0;	/* return one character as token */
			retc = (uschar)buf[0];	/* character is its own type */
			unputstr(rem+1); /* put rest back for later */
		} else {	/* some prefix was a number */
			unputstr(rem);	/* put rest back for later */
			rem[0] = 0;	/* truncate buf after number part */
			retc = '0';	/* type is number */
		}
	}
	*pbuf = buf;
	*psz = sz;
	return retc;
}

int	word(char *);
int	string(void);
int	regexpr(void);
bool	sc	= false;	/* true => return a } right now */
bool	reg	= false;	/* true => return a REGEXPR now */

int yylex(void)
{
	int c;
	static char *buf = NULL;
	static int bufsize = 5; /* BUG: setting this small causes core dump! */

	if (buf == NULL && (buf = (char *) malloc(bufsize)) == NULL)
		FATAL( "out of space in yylex" );
	if (sc) {
		sc = false;
		RET('}');
	}
	if (reg) {
		reg = false;
		return regexpr();
	}
	for (;;) {
		c = gettok(&buf, &bufsize);
		if (c == 0)
			return 0;
		if (isalpha(c) || c == '_')
			return word(buf);
		if (isdigit(c)) {
			char *cp = tostring(buf);
			double result;

			if (is_number(cp, & result))
				yylval.cp = setsymtab(buf, cp, result, CON|NUM, symtab);
			else
				yylval.cp = setsymtab(buf, cp, 0.0, STR, symtab);
			free(cp);
			/* should this also have STR set? */
			RET(NUMBER);
		}

		yylval.i = c;
		switch (c) {
		case '\n':	/* {EOL} */
			lineno++;
			RET(NL);
		case '\r':	/* assume \n is coming */
		case ' ':	/* {WS}+ */
		case '\t':
			break;
		case '#':	/* #.* strip comments */
			while ((c = input()) != '\n' && c != 0)
				;
			unput(c);
			/*
			 * Next line is a hack, itcompensates for
			 * unput's treatment of \n.
			 */
			lineno++;
			break;
		case ';':
			RET(';');
		case '\\':
			if (peek() == '\n') {
				input();
				lineno++;
			} else if (peek() == '\r') {
				input(); input();	/* \n */
				lineno++;
			} else {
				RET(c);
			}
			break;
		case '&':
			if (peek() == '&') {
				input(); RET(AND);
			} else
				RET('&');
		case '|':
			if (peek() == '|') {
				input(); RET(BOR);
			} else
				RET('|');
		case '!':
			if (peek() == '=') {
				input(); yylval.i = NE; RET(NE);
			} else if (peek() == '~') {
				input(); yylval.i = NOTMATCH; RET(MATCHOP);
			} else
				RET(NOT);
		case '~':
			yylval.i = MATCH;
			RET(MATCHOP);
		case '<':
			if (peek() == '=') {
				input(); yylval.i = LE; RET(LE);
			} else {
				yylval.i = LT; RET(LT);
			}
		case '=':
			if (peek() == '=') {
				input(); yylval.i = EQ; RET(EQ);
			} else {
				yylval.i = ASSIGN; RET(ASGNOP);
			}
		case '>':
			if (peek() == '=') {
				input(); yylval.i = GE; RET(GE);
			} else if (peek() == '>') {
				input(); yylval.i = APPEND; RET(APPEND);
			} else {
				yylval.i = GT; RET(GT);
			}
		case '+':
			if (peek() == '+') {
				input(); yylval.i = INCR; RET(INCR);
			} else if (peek() == '=') {
				input(); yylval.i = ADDEQ; RET(ASGNOP);
			} else
				RET('+');
		case '-':
			if (peek() == '-') {
				input(); yylval.i = DECR; RET(DECR);
			} else if (peek() == '=') {
				input(); yylval.i = SUBEQ; RET(ASGNOP);
			} else
				RET('-');
		case '*':
			if (peek() == '=') {	/* *= */
				input(); yylval.i = MULTEQ; RET(ASGNOP);
			} else if (peek() == '*') {	/* ** or **= */
				input();	/* eat 2nd * */
				if (peek() == '=') {
					input(); yylval.i = POWEQ; RET(ASGNOP);
				} else {
					RET(POWER);
				}
			} else
				RET('*');
		case '/':
			RET('/');
		case '%':
			if (peek() == '=') {
				input(); yylval.i = MODEQ; RET(ASGNOP);
			} else
				RET('%');
		case '^':
			if (peek() == '=') {
				input(); yylval.i = POWEQ; RET(ASGNOP);
			} else
				RET(POWER);

		case '$':
			/* BUG: awkward, if not wrong */
			c = gettok(&buf, &bufsize);
			if (isalpha(c)) {
				if (strcmp(buf, "NF") == 0) {	/* very special */
					unputstr("(NF)");
					RET(INDIRECT);
				}
				c = peek();
				if (c == '(' || c == '[' || (infunc && isarg(buf) >= 0)) {
					unputstr(buf);
					RET(INDIRECT);
				}
				yylval.cp = setsymtab(buf, "", 0.0, STR|NUM, symtab);
				RET(IVAR);
			} else if (c == 0) {	/*  */
				SYNTAX( "unexpected end of input after $" );
				RET(';');
			} else {
				unputstr(buf);
				RET(INDIRECT);
			}

		case '}':
			if (--bracecnt < 0)
				SYNTAX( "extra }" );
			sc = true;
			RET(';');
		case ']':
			if (--brackcnt < 0)
				SYNTAX( "extra ]" );
			RET(']');
		case ')':
			if (--parencnt < 0)
				SYNTAX( "extra )" );
			RET(')');
		case '{':
			bracecnt++;
			RET('{');
		case '[':
			brackcnt++;
			RET('[');
		case '(':
			parencnt++;
			RET('(');

		case '"':
			return string();	/* BUG: should be like tran.c ? */

		default:
			RET(c);
		}
	}
}

extern int runetochar(char *str, int c);

int string(void)
{
	int c, n;
	char *s, *bp;
	static char *buf = NULL;
	static int bufsz = 500;

	if (buf == NULL && (buf = (char *) malloc(bufsz)) == NULL)
		FATAL("out of space for strings");
	for (bp = buf; (c = input()) != '"'; ) {
		if (!adjbuf(&buf, &bufsz, bp-buf+2, 500, &bp, "string"))
			FATAL("out of space for string %.10s...", buf);
		switch (c) {
		case '\n':
		case '\r':
		case 0:
			*bp = '\0';
			SYNTAX( "non-terminated string %.10s...", buf );
			if (c == 0)	/* hopeless */
				FATAL( "giving up" );
			lineno++;
			break;
		case '\\':
			c = input();
			switch (c) {
			case '\n': break;
			case '"': *bp++ = '"'; break;
			case 'n': *bp++ = '\n'; break;
			case 't': *bp++ = '\t'; break;
			case 'f': *bp++ = '\f'; break;
			case 'r': *bp++ = '\r'; break;
			case 'b': *bp++ = '\b'; break;
			case 'v': *bp++ = '\v'; break;
			case 'a': *bp++ = '\a'; break;
			case '\\': *bp++ = '\\'; break;

			case '0': case '1': case '2': /* octal: \d \dd \ddd */
			case '3': case '4': case '5': case '6': case '7':
				n = c - '0';
				if ((c = peek()) >= '0' && c < '8') {
					n = 8 * n + input() - '0';
					if ((c = peek()) >= '0' && c < '8')
						n = 8 * n + input() - '0';
				}
				*bp++ = n;
				break;

			case 'x':	/* hex  \x0-9a-fA-F (exactly two) */
			    {
				int i;

				if (!isxdigit(peek())) {
					unput(c);
					break;
				}
				n = 0;
				for (i = 0; i < 2; i++) {
					c = input();
					if (c == 0)
						break;
					if (isxdigit(c)) {
						c = tolower(c);
						n *= 16;
						if (isdigit(c))
							n += (c - '0');
						else
							n += 10 + (c - 'a');
					} else {
						unput(c);
						break;
					}
				}
				if (i)
					*bp++ = n;
				break;
			    }

			case 'u':	/* utf  \u0-9a-fA-F (1..8) */
			    {
				int i;

				n = 0;
				for (i = 0; i < 8; i++) {
					c = input();
					if (!isxdigit(c) || c == 0)
						break;
					c = tolower(c);
					n *= 16;
					if (isdigit(c))
						n += (c - '0');
					else
						n += 10 + (c - 'a');
				}
				unput(c);
				bp += runetochar(bp, n);
				break;
			    }

			default:
				*bp++ = c;
				break;
			}
			break;
		default:
			*bp++ = c;
			break;
		}
	}
	*bp = 0;
	s = tostring(buf);
	*bp++ = ' '; *bp++ = '\0';
	yylval.cp = setsymtab(buf, s, 0.0, CON|STR|DONTFREE, symtab);
	free(s);
	RET(STRING);
}


static int binsearch(char *w, const Keyword *kp, int n)
{
	int cond, low, mid, high;

	low = 0;
	high = n - 1;
	while (low <= high) {
		mid = (low + high) / 2;
		if ((cond = strcmp(w, kp[mid].word)) < 0)
			high = mid - 1;
		else if (cond > 0)
			low = mid + 1;
		else
			return mid;
	}
	return -1;
}

int word(char *w)
{
	const Keyword *kp;
	int c, n;

	n = binsearch(w, keywords, sizeof(keywords)/sizeof(keywords[0]));
	if (n != -1) {	/* found in table */
		kp = keywords + n;
		yylval.i = kp->sub;
		switch (kp->type) {	/* special handling */
		case BLTIN:
			if (kp->sub == FSYSTEM && safe)
				SYNTAX( "system is unsafe" );
			RET(kp->type);
		case FUNC:
			if (infunc)
				SYNTAX( "illegal nested function" );
			RET(kp->type);
		case RETURN:
			if (!infunc)
				SYNTAX( "return not in function" );
			RET(kp->type);
		case VARNF:
			yylval.cp = setsymtab("NF", "", 0.0, NUM, symtab);
			RET(VARNF);
		default:
			RET(kp->type);
		}
	}
	c = peek();	/* look for '(' */
	if (c != '(' && infunc && (n=isarg(w)) >= 0) {
		yylval.i = n;
		RET(ARG);
	} else {
		yylval.cp = setsymtab(w, "", 0.0, STR|NUM|DONTFREE, symtab);
		if (c == '(') {
			RET(CALL);
		} else {
			RET(VAR);
		}
	}
}

void startreg(void)	/* next call to yylex will return a regular expression */
{
	reg = true;
}

int regexpr(void)
{
	int c;
	static char *buf = NULL;
	static int bufsz = 500;
	char *bp;

	if (buf == NULL && (buf = (char *) malloc(bufsz)) == NULL)
		FATAL("out of space for reg expr");
	bp = buf;
	for ( ; (c = input()) != '/' && c != 0; ) {
		if (!adjbuf(&buf, &bufsz, bp-buf+3, 500, &bp, "regexpr"))
			FATAL("out of space for reg expr %.10s...", buf);
		if (c == '\n') {
			*bp = '\0';
			SYNTAX( "newline in regular expression %.10s...", buf );
			unput('\n');
			break;
		} else if (c == '\\') {
			*bp++ = '\\';
			*bp++ = input();
		} else {
			*bp++ = c;
		}
	}
	*bp = 0;
	if (c == 0)
		SYNTAX("non-terminated regular expression %.10s...", buf);
	yylval.s = tostring(buf);
	unput('/');
	RET(REGEXPR);
}

/* low-level lexical stuff, sort of inherited from lex */

char	ebuf[300];
char	*ep = ebuf;
char	yysbuf[100];	/* pushback buffer */
char	*yysptr = yysbuf;
FILE	*yyin = NULL;

int input(void)	/* get next lexical input character */
{
	int c;
	extern char *lexprog;

	if (yysptr > yysbuf)
		c = (uschar)*--yysptr;
	else if (lexprog != NULL) {	/* awk '...' */
		if ((c = (uschar)*lexprog) != 0)
			lexprog++;
	} else				/* awk -f ... */
		c = pgetc();
	if (c == EOF)
		c = 0;
	if (ep >= ebuf + sizeof ebuf)
		ep = ebuf;
	*ep = c;
	if (c != 0) {
		ep++;
	}
	return (c);
}

void unput(int c)	/* put lexical character back on input */
{
	if (c == '\n')  
		lineno--;
	if (yysptr >= yysbuf + sizeof(yysbuf))
		FATAL("pushed back too much: %.20s...", yysbuf);
	*yysptr++ = c;
	if (--ep < ebuf)
		ep = ebuf + sizeof(ebuf) - 1;
}

void unputstr(const char *s)	/* put a string back on input */
{
	int i;

	for (i = strlen(s)-1; i >= 0; i--)
		unput(s[i]);
}
