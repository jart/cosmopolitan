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

/*
 * this program makes the table to link function names
 * and type indices that is used by execute() in run.c.
 * it finds the indices in awkgram.tab.h, produced by bison.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "awk.h"
#include "awkgram.tab.h"

struct xx
{	int token;
	const char *name;
	const char *pname;
} proc[] = {
	{ PROGRAM, "program", NULL },
	{ BOR, "boolop", " || " },
	{ AND, "boolop", " && " },
	{ NOT, "boolop", " !" },
	{ NE, "relop", " != " },
	{ EQ, "relop", " == " },
	{ LE, "relop", " <= " },
	{ LT, "relop", " < " },
	{ GE, "relop", " >= " },
	{ GT, "relop", " > " },
	{ ARRAY, "array", NULL },
	{ INDIRECT, "indirect", "$(" },
	{ SUBSTR, "substr", "substr" },
	{ SUB, "sub", "sub" },
	{ GSUB, "gsub", "gsub" },
	{ INDEX, "sindex", "sindex" },
	{ SPRINTF, "awksprintf", "sprintf " },
	{ ADD, "arith", " + " },
	{ MINUS, "arith", " - " },
	{ MULT, "arith", " * " },
	{ DIVIDE, "arith", " / " },
	{ MOD, "arith", " % " },
	{ UMINUS, "arith", " -" },
	{ UPLUS, "arith", " +" },
	{ POWER, "arith", " **" },
	{ PREINCR, "incrdecr", "++" },
	{ POSTINCR, "incrdecr", "++" },
	{ PREDECR, "incrdecr", "--" },
	{ POSTDECR, "incrdecr", "--" },
	{ CAT, "cat", " " },
	{ PASTAT, "pastat", NULL },
	{ PASTAT2, "dopa2", NULL },
	{ MATCH, "matchop", " ~ " },
	{ NOTMATCH, "matchop", " !~ " },
	{ MATCHFCN, "matchop", "matchop" },
	{ INTEST, "intest", "intest" },
	{ PRINTF, "awkprintf", "printf" },
	{ PRINT, "printstat", "print" },
	{ CLOSE, "closefile", "closefile" },
	{ DELETE, "awkdelete", "awkdelete" },
	{ SPLIT, "split", "split" },
	{ ASSIGN, "assign", " = " },
	{ ADDEQ, "assign", " += " },
	{ SUBEQ, "assign", " -= " },
	{ MULTEQ, "assign", " *= " },
	{ DIVEQ, "assign", " /= " },
	{ MODEQ, "assign", " %= " },
	{ POWEQ, "assign", " ^= " },
	{ CONDEXPR, "condexpr", " ?: " },
	{ IF, "ifstat", "if(" },
	{ WHILE, "whilestat", "while(" },
	{ FOR, "forstat", "for(" },
	{ DO, "dostat", "do" },
	{ IN, "instat", "instat" },
	{ NEXT, "jump", "next" },
	{ NEXTFILE, "jump", "nextfile" },
	{ EXIT, "jump", "exit" },
	{ BREAK, "jump", "break" },
	{ CONTINUE, "jump", "continue" },
	{ RETURN, "jump", "ret" },
	{ BLTIN, "bltin", "bltin" },
	{ CALL, "call", "call" },
	{ ARG, "arg", "arg" },
	{ VARNF, "getnf", "NF" },
	{ GETLINE, "awkgetline", "getline" },
	{ 0, "", "" },
};

#define SIZE	(LASTTOKEN - FIRSTTOKEN + 1)
const char *table[SIZE];
char *names[SIZE];

int main(int argc, char *argv[])
{
	const struct xx *p;
	int i, n, tok;
	char c;
	FILE *fp;
	char buf[200], name[200], def[200];
	enum { TOK_UNKNOWN, TOK_ENUM, TOK_DEFINE } tokentype = TOK_UNKNOWN;

	printf("#include <stdio.h>\n");
	printf("#include \"awk.h\"\n");
	printf("#include \"awkgram.tab.h\"\n\n");

	if (argc != 2) {
		fprintf(stderr, "usage: maketab YTAB_H\n");
		exit(1);
	}
	if ((fp = fopen(argv[1], "r")) == NULL) {
		fprintf(stderr, "maketab can't open %s!\n", argv[1]);
		exit(1);
	}
	printf("static const char * const printname[%d] = {\n", SIZE);
	i = 0;
	while (fgets(buf, sizeof buf, fp) != NULL) {
		// 199 is sizeof(def) - 1
		if (tokentype != TOK_ENUM) {
			n = sscanf(buf, "%1c %199s %199s %d", &c, def, name,
			    &tok);
			if (n == 4 && c == '#' && strcmp(def, "define") == 0) {
				tokentype = TOK_DEFINE;
			} else if (tokentype != TOK_UNKNOWN) {
				continue;
			}
		}
		if (tokentype != TOK_DEFINE) {
			/* not a valid #define, bison uses enums now */
			n = sscanf(buf, "%199s = %d,\n", name, &tok);
			if (n != 2)
				continue;
			tokentype = TOK_ENUM;
		}
		if (strcmp(name, "YYSTYPE_IS_DECLARED") == 0) {
			tokentype = TOK_UNKNOWN;
			continue;
		}
		if (tok < FIRSTTOKEN || tok > LASTTOKEN) {
			tokentype = TOK_UNKNOWN;
			/* fprintf(stderr, "maketab funny token %d %s ignored\n", tok, buf); */
			continue;
		}
		names[tok-FIRSTTOKEN] = strdup(name);
		if (names[tok-FIRSTTOKEN] == NULL) {
			fprintf(stderr, "maketab out of space copying %s", name);
			continue;
		}
		printf("\t\"%s\",\t/* %d */\n", name, tok);
		i++;
	}
	printf("};\n\n");

	for (p=proc; p->token!=0; p++)
		table[p->token-FIRSTTOKEN] = p->name;
	printf("\nCell *(*proctab[%d])(Node **, int) = {\n", SIZE);
	for (i=0; i<SIZE; i++)
		printf("\t%s,\t/* %s */\n",
		    table[i] ? table[i] : "nullproc", names[i] ? names[i] : "");
	printf("};\n\n");

	printf("const char *tokname(int n)\n");	/* print a tokname() function */
	printf("{\n");
	printf("\tstatic char buf[100];\n\n");
	printf("\tif (n < FIRSTTOKEN || n > LASTTOKEN) {\n");
	printf("\t\tsnprintf(buf, sizeof(buf), \"token %%d\", n);\n");
	printf("\t\treturn buf;\n");
	printf("\t}\n");
	printf("\treturn printname[n-FIRSTTOKEN];\n");
	printf("}\n");
	return 0;
}
