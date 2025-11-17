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
 *
 *	@(#)options.h	8.2 (Berkeley) 5/4/95
 */

struct shparam {
	int nparam;		/* # of positional parameters (without $0) */
	unsigned char malloc;	/* if parameter list dynamically allocated */
	char **p;		/* parameter list */
	int optind;		/* next parameter to be processed by getopts */
	int optoff;		/* used by getopts */
};



#define eflag optlist[0]
#define fflag optlist[1]
#define Iflag optlist[2]
#define iflag optlist[3]
#define mflag optlist[4]
#define nflag optlist[5]
#define sflag optlist[6]
#define xflag optlist[7]
#define vflag optlist[8]
#define Vflag optlist[9]
#define	Eflag optlist[10]
#define	Cflag optlist[11]
#define	aflag optlist[12]
#define	bflag optlist[13]
#define	uflag optlist[14]
#define	nolog optlist[15]
#define	pipefail optlist[16]
#define	debug optlist[17]

#define NOPTS	18

extern const char optletters[NOPTS];
extern char optlist[NOPTS];


extern char *minusc;		/* argument to -c option */
extern char *arg0;		/* $0 */
extern struct shparam shellparam;  /* $@ */
extern char **argptr;		/* argument list for builtin commands */
extern char *optionarg;		/* set by nextopt */
extern char *optptr;		/* used by nextopt */

int procargs(int, char **);
void optschanged(void);
void setparam(char **);
void freeparam(volatile struct shparam *);
int shiftcmd(int, char **);
int setcmd(int, char **);
int getoptscmd(int, char **);
int nextopt(const char *);
void getoptsreset(const char *);
