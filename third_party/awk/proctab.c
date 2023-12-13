/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│ vi: set noet ft=c ts=8 sw=8 fenc=utf-8                                   :vi │
╚──────────────────────────────────────────────────────────────────────────────╝
│                                                                              │
│ Copyright (C) Lucent Technologies 1997                                       │
│ All Rights Reserved                                                          │
│                                                                              │
│ Permission to use, copy, modify, and distribute this software and            │
│ its documentation for any purpose and without fee is hereby                  │
│ granted, provided that the above copyright notice appear in all              │
│ copies and that both that the copyright notice and this                      │
│ permission notice and warranty disclaimer appear in supporting               │
│ documentation, and that the name Lucent Technologies or any of               │
│ its entities not be used in advertising or publicity pertaining              │
│ to distribution of the software without specific, written prior              │
│ permission.                                                                  │
│                                                                              │
│ LUCENT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,                │
│ INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.             │
│ IN NO EVENT SHALL LUCENT OR ANY OF ITS ENTITIES BE LIABLE FOR ANY            │
│ SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES                    │
│ WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER              │
│ IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,               │
│ ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF               │
│ THIS SOFTWARE.                                                               │
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/awk/awk.h"
#include "third_party/awk/awkgram.tab.h"

static const char * const printname[95] = {
	"FIRSTTOKEN",	/* 258 */
	"PROGRAM",	/* 259 */
	"PASTAT",	/* 260 */
	"PASTAT2",	/* 261 */
	"XBEGIN",	/* 262 */
	"XEND",	/* 263 */
	"NL",	/* 264 */
	"ARRAY",	/* 265 */
	"MATCH",	/* 266 */
	"NOTMATCH",	/* 267 */
	"MATCHOP",	/* 268 */
	"FINAL",	/* 269 */
	"DOT",	/* 270 */
	"ALL",	/* 271 */
	"CCL",	/* 272 */
	"NCCL",	/* 273 */
	"CHAR",	/* 274 */
	"OR",	/* 275 */
	"STAR",	/* 276 */
	"QUEST",	/* 277 */
	"PLUS",	/* 278 */
	"EMPTYRE",	/* 279 */
	"ZERO",	/* 280 */
	"AND",	/* 281 */
	"BOR",	/* 282 */
	"APPEND",	/* 283 */
	"EQ",	/* 284 */
	"GE",	/* 285 */
	"GT",	/* 286 */
	"LE",	/* 287 */
	"LT",	/* 288 */
	"NE",	/* 289 */
	"IN",	/* 290 */
	"ARG",	/* 291 */
	"BLTIN",	/* 292 */
	"BREAK",	/* 293 */
	"CLOSE",	/* 294 */
	"CONTINUE",	/* 295 */
	"DELETE",	/* 296 */
	"DO",	/* 297 */
	"EXIT",	/* 298 */
	"FOR",	/* 299 */
	"FUNC",	/* 300 */
	"SUB",	/* 301 */
	"GSUB",	/* 302 */
	"IF",	/* 303 */
	"INDEX",	/* 304 */
	"LSUBSTR",	/* 305 */
	"MATCHFCN",	/* 306 */
	"NEXT",	/* 307 */
	"NEXTFILE",	/* 308 */
	"ADD",	/* 309 */
	"MINUS",	/* 310 */
	"MULT",	/* 311 */
	"DIVIDE",	/* 312 */
	"MOD",	/* 313 */
	"ASSIGN",	/* 314 */
	"ASGNOP",	/* 315 */
	"ADDEQ",	/* 316 */
	"SUBEQ",	/* 317 */
	"MULTEQ",	/* 318 */
	"DIVEQ",	/* 319 */
	"MODEQ",	/* 320 */
	"POWEQ",	/* 321 */
	"PRINT",	/* 322 */
	"PRINTF",	/* 323 */
	"SPRINTF",	/* 324 */
	"ELSE",	/* 325 */
	"INTEST",	/* 326 */
	"CONDEXPR",	/* 327 */
	"POSTINCR",	/* 328 */
	"PREINCR",	/* 329 */
	"POSTDECR",	/* 330 */
	"PREDECR",	/* 331 */
	"VAR",	/* 332 */
	"IVAR",	/* 333 */
	"VARNF",	/* 334 */
	"CALL",	/* 335 */
	"NUMBER",	/* 336 */
	"STRING",	/* 337 */
	"REGEXPR",	/* 338 */
	"GETLINE",	/* 339 */
	"RETURN",	/* 340 */
	"SPLIT",	/* 341 */
	"SUBSTR",	/* 342 */
	"WHILE",	/* 343 */
	"CAT",	/* 344 */
	"NOT",	/* 345 */
	"UMINUS",	/* 346 */
	"UPLUS",	/* 347 */
	"POWER",	/* 348 */
	"DECR",	/* 349 */
	"INCR",	/* 350 */
	"INDIRECT",	/* 351 */
	"LASTTOKEN",	/* 352 */
};


Cell *(*proctab[95])(Node **, int) = {
	nullproc,	/* FIRSTTOKEN */
	program,	/* PROGRAM */
	pastat,	/* PASTAT */
	dopa2,	/* PASTAT2 */
	nullproc,	/* XBEGIN */
	nullproc,	/* XEND */
	nullproc,	/* NL */
	array,	/* ARRAY */
	matchop,	/* MATCH */
	matchop,	/* NOTMATCH */
	nullproc,	/* MATCHOP */
	nullproc,	/* FINAL */
	nullproc,	/* DOT */
	nullproc,	/* ALL */
	nullproc,	/* CCL */
	nullproc,	/* NCCL */
	nullproc,	/* CHAR */
	nullproc,	/* OR */
	nullproc,	/* STAR */
	nullproc,	/* QUEST */
	nullproc,	/* PLUS */
	nullproc,	/* EMPTYRE */
	nullproc,	/* ZERO */
	boolop,	/* AND */
	boolop,	/* BOR */
	nullproc,	/* APPEND */
	relop,	/* EQ */
	relop,	/* GE */
	relop,	/* GT */
	relop,	/* LE */
	relop,	/* LT */
	relop,	/* NE */
	instat,	/* IN */
	arg,	/* ARG */
	bltin,	/* BLTIN */
	jump,	/* BREAK */
	closefile,	/* CLOSE */
	jump,	/* CONTINUE */
	awkdelete,	/* DELETE */
	dostat,	/* DO */
	jump,	/* EXIT */
	forstat,	/* FOR */
	nullproc,	/* FUNC */
	sub,	/* SUB */
	gsub,	/* GSUB */
	ifstat,	/* IF */
	sindex,	/* INDEX */
	nullproc,	/* LSUBSTR */
	matchop,	/* MATCHFCN */
	jump,	/* NEXT */
	jump,	/* NEXTFILE */
	arith,	/* ADD */
	arith,	/* MINUS */
	arith,	/* MULT */
	arith,	/* DIVIDE */
	arith,	/* MOD */
	assign,	/* ASSIGN */
	nullproc,	/* ASGNOP */
	assign,	/* ADDEQ */
	assign,	/* SUBEQ */
	assign,	/* MULTEQ */
	assign,	/* DIVEQ */
	assign,	/* MODEQ */
	assign,	/* POWEQ */
	printstat,	/* PRINT */
	awkprintf,	/* PRINTF */
	awksprintf,	/* SPRINTF */
	nullproc,	/* ELSE */
	intest,	/* INTEST */
	condexpr,	/* CONDEXPR */
	incrdecr,	/* POSTINCR */
	incrdecr,	/* PREINCR */
	incrdecr,	/* POSTDECR */
	incrdecr,	/* PREDECR */
	nullproc,	/* VAR */
	nullproc,	/* IVAR */
	getnf,	/* VARNF */
	call,	/* CALL */
	nullproc,	/* NUMBER */
	nullproc,	/* STRING */
	nullproc,	/* REGEXPR */
	awkgetline,	/* GETLINE */
	jump,	/* RETURN */
	split,	/* SPLIT */
	substr,	/* SUBSTR */
	whilestat,	/* WHILE */
	cat,	/* CAT */
	boolop,	/* NOT */
	arith,	/* UMINUS */
	arith,	/* UPLUS */
	arith,	/* POWER */
	nullproc,	/* DECR */
	nullproc,	/* INCR */
	indirect,	/* INDIRECT */
	nullproc,	/* LASTTOKEN */
};

const char *tokname(int n)
{
	static char buf[100];

	if (n < FIRSTTOKEN || n > LASTTOKEN) {
		snprintf(buf, sizeof(buf), "token %d", n);
		return buf;
	}
	return printname[n-FIRSTTOKEN];
}
