/* collsyms.h -- collating symbol names and their corresponding characters
		 (in ascii) as given by POSIX.2 in table 2.8. */

/* Copyright (C) 1997-2002 Free Software Foundation, Inc.

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

/* The upper-case letters, lower-case letters, and digits are omitted from
   this table.  The digits are not included in the table in the POSIX.2
   spec.  The upper and lower case letters are translated by the code
   in smatch.c:collsym(). */

typedef struct _COLLSYM {
  XCHAR *name;
  CHAR code;
} __COLLSYM;

static __COLLSYM POSIXCOLL [] =
{
	{  L("NUL"),			L('\0') },
	{  L("SOH"),			L('\001') },
	{  L("STX"),			L('\002') },
	{  L("ETX"),			L('\003') },
	{  L("EOT"),			L('\004') },
	{  L("ENQ"),			L('\005') },
	{  L("ACK"),			L('\006') },
#ifdef __STDC__
	{  L("alert"),			L('\a') },
#else
	{  L("alert"),			L('\007') },
#endif
	{  L("BS"),			L('\010') },
	{  L("backspace"),		L('\b') },
	{  L("HT"),			L('\011') },
	{  L("tab"),			L('\t') },
	{  L("LF"),			L('\012') },
	{  L("newline"),		L('\n') },
	{  L("VT"),			L('\013') },
	{  L("vertical-tab"),		L('\v') },
	{  L("FF"),			L('\014') },
	{  L("form-feed"),		L('\f') },
	{  L("CR"),			L('\015') },
	{  L("carriage-return"),	L('\r') },
	{  L("SO"),			L('\016') },
	{  L("SI"),			L('\017') },
	{  L("DLE"),			L('\020') },
	{  L("DC1"),			L('\021') },
	{  L("DC2"),			L('\022') },
	{  L("DC3"),			L('\023') },
	{  L("DC4"),			L('\024') },
	{  L("NAK"),			L('\025') },
	{  L("SYN"),			L('\026') },
	{  L("ETB"),			L('\027') },
	{  L("CAN"),			L('\030') },
	{  L("EM"),			L('\031') },
	{  L("SUB"),			L('\032') },
	{  L("ESC"),			L('\033') },
	{  L("IS4"),			L('\034') },
	{  L("FS"),			L('\034') },
	{  L("IS3"),			L('\035') },
	{  L("GS"),			L('\035') },
	{  L("IS2"),			L('\036') },
	{  L("RS"),			L('\036') },
	{  L("IS1"),			L('\037') },
	{  L("US"),			L('\037') },
	{  L("space"),			L(' ') },
	{  L("exclamation-mark"),	L('!') },
	{  L("quotation-mark"),		L('"') },
	{  L("number-sign"),		L('#') },
	{  L("dollar-sign"),		L('$') },
	{  L("percent-sign"),		L('%') },
	{  L("ampersand"),		L('&') },
	{  L("apostrophe"),		L('\'') },
	{  L("left-parenthesis"),	L('(') },
	{  L("right-parenthesis"),	L(')') },
	{  L("asterisk"),		L('*') },
	{  L("plus-sign"),		L('+') },
	{  L("comma"),			L(',') },
	{  L("hyphen"),			L('-') },
	{  L("hyphen-minus"),		L('-') },
	{  L("minus"),			L('-') },	/* extension from POSIX.2 */
	{  L("dash"),			L('-') },	/* extension from POSIX.2 */
	{  L("period"),			L('.') },
	{  L("full-stop"),		L('.') },
	{  L("slash"),			L('/') },
	{  L("solidus"),		L('/') },	/* extension from POSIX.2 */
	{  L("zero"),			L('0') },
	{  L("one"),			L('1') },
	{  L("two"),			L('2') },
	{  L("three"),			L('3') },
	{  L("four"),			L('4') },
	{  L("five"),			L('5') },
	{  L("six"),			L('6') },
	{  L("seven"),			L('7') },
	{  L("eight"),			L('8') },
	{  L("nine"),			L('9') },
	{  L("colon"),			L(':') },
	{  L("semicolon"),		L(';') },
	{  L("less-than-sign"),		L('<') },
	{  L("equals-sign"),		L('=') },
	{  L("greater-than-sign"),	L('>') },
	{  L("question-mark"),		L('?') },
	{  L("commercial-at"),		L('@') },
	/* upper-case letters omitted */
	{  L("left-square-bracket"),	L('[') },
	{  L("backslash"),		L('\\') },
	{  L("reverse-solidus"),	L('\\') },
	{  L("right-square-bracket"),	L(']') },
	{  L("circumflex"),		L('^') },
	{  L("circumflex-accent"),	L('^') },	/* extension from POSIX.2 */
	{  L("underscore"),		L('_') },
	{  L("grave-accent"),		L('`') },
	/* lower-case letters omitted */
	{  L("left-brace"),		L('{') },	/* extension from POSIX.2 */
	{  L("left-curly-bracket"),	L('{') },
	{  L("vertical-line"),		L('|') },
	{  L("right-brace"),		L('}') },	/* extension from POSIX.2 */
	{  L("right-curly-bracket"),	L('}') },
	{  L("tilde"),			L('~') },
	{  L("DEL"),			L('\177') },
	{  0,	0 },
};

#undef _COLLSYM
#undef __COLLSYM
#undef POSIXCOLL
