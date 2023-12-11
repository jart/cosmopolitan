/* syntax.h -- Syntax definitions for the shell */

/* Copyright (C) 2000, 2001, 2005, 2008, 2009-2020 Free Software Foundation, Inc.

   This file is part of GNU Bash, the Bourne Again SHell.

   Bash is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Bash is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Bash.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _SYNTAX_H_
#define _SYNTAX_H_

/* Defines for use by mksyntax.c */

#define slashify_in_quotes "\\`$\"\n"
#define slashify_in_here_document "\\`$"

#define shell_meta_chars   "()<>;&|"
#define shell_break_chars  "()<>;&| \t\n"

#define shell_quote_chars	"\"`'"

#if defined (PROCESS_SUBSTITUTION)
#  define shell_exp_chars		"$<>"
#else
#  define shell_exp_chars		"$"
#endif

#if defined (EXTENDED_GLOB)
#  define ext_glob_chars	"@*+?!"
#else
#  define ext_glob_chars	""
#endif
#define shell_glob_chars	"*?[]^"

/* Defines shared by mksyntax.c and the rest of the shell code. */

/* Values for character flags in syntax tables */

#define CWORD		0x0000	/* nothing special; an ordinary character */
#define CSHMETA		0x0001	/* shell meta character */
#define CSHBRK		0x0002	/* shell break character */
#define CBACKQ		0x0004	/* back quote */
#define CQUOTE		0x0008	/* shell quote character */
#define CSPECL		0x0010	/* special character that needs quoting */
#define CEXP		0x0020	/* shell expansion character */
#define CBSDQUOTE	0x0040	/* characters escaped by backslash in double quotes */
#define CBSHDOC		0x0080	/* characters escaped by backslash in here doc */
#define CGLOB		0x0100	/* globbing characters */
#define CXGLOB		0x0200	/* extended globbing characters */
#define CXQUOTE		0x0400	/* cquote + backslash */
#define CSPECVAR	0x0800	/* single-character shell variable name */
#define CSUBSTOP	0x1000	/* values of OP for ${word[:]OPstuff} */
#define CBLANK		0x2000	/* whitespace (blank) character */

/* Defines for use by the rest of the shell. */
extern int sh_syntaxtab[];
extern int sh_syntabsiz;

#define shellmeta(c)	(sh_syntaxtab[(unsigned char)(c)] & CSHMETA)
#define shellbreak(c)	(sh_syntaxtab[(unsigned char)(c)] & CSHBRK)
#define shellquote(c)	(sh_syntaxtab[(unsigned char)(c)] & CQUOTE)
#define shellxquote(c)	(sh_syntaxtab[(unsigned char)(c)] & CXQUOTE)

#define shellblank(c)	(sh_syntaxtab[(unsigned char)(c)] & CBLANK)

#define parserblank(c)	((c) == ' ' || (c) == '\t')

#define issyntype(c, t)	((sh_syntaxtab[(unsigned char)(c)] & (t)) != 0)
#define notsyntype(c,t) ((sh_syntaxtab[(unsigned char)(c)] & (t)) == 0)

#if defined (PROCESS_SUBSTITUTION)
#  define shellexp(c)	((c) == '$' || (c) == '<' || (c) == '>')
#else
#  define shellexp(c)	((c) == '$')
#endif

#if defined (EXTENDED_GLOB)
#  define PATTERN_CHAR(c) \
	((c) == '@' || (c) == '*' || (c) == '+' || (c) == '?' || (c) == '!')
#else
#  define PATTERN_CHAR(c) 0
#endif

#define GLOB_CHAR(c) \
	((c) == '*' || (c) == '?' || (c) == '[' || (c) == ']' || (c) == '^')

#define CTLESC '\001'
#define CTLNUL '\177'

#if !defined (HAVE_ISBLANK) && !defined (isblank)
#  define isblank(x)	((x) == ' ' || (x) == '\t')
#endif

#endif /* _SYNTAX_H_ */
