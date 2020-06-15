asm(".ident\t\"\\n\\n\
getopt (BSD-3)\\n\
Copyright 1987, 1993, 1994 The Regents of the University of California\"");
asm(".include \"libc/disclaimer.inc\"");

/* clang-format off */
/*	$NetBSD: getopt.c,v 1.26 2003/08/07 16:43:40 agc Exp $	*/

/*
 * Copyright (c) 1987, 1993, 1994
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
 *
 * @(#)getopt.c	8.3 (Berkeley) 4/27/95
 * $FreeBSD: src/lib/libc/stdlib/getopt.c,v 1.8 2007/01/09 00:28:10 imp Exp $
 * $DragonFly: src/lib/libc/stdlib/getopt.c,v 1.7 2005/11/20 12:37:48 swildner Exp $
 */

#include "libc/str/str.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"

int	opterr,			/* if error message should be printed */
	optind,			/* index into parent argv vector */
	optopt,			/* character checked for validity */
	optreset;		/* reset getopt */
char	*optarg;		/* argument associated with option */
static char *place;		/* option letter processing */

#define	BADCH	(int)'?'
#define	BADARG	(int)':'

static char EMSG[] = { '\0' };

INITIALIZER(300, _init_getopt, {
  opterr = 1;
  optind = 1;
  place = EMSG;
});

/*
 * getopt --
 *	Parse argc/argv argument vector.
 */
int
getopt(int nargc, char * const nargv[], const char *ostr)
{
	char *oli;				/* option letter list index */

	/*
	 * Some programs like cvs expect optind = 0 to trigger
	 * a reset of getopt.
	 */
	if (optind == 0)
		optind = 1;

	if (optreset || *place == 0) {		/* update scanning pointer */
		optreset = 0;
		place = nargv[optind];
		if (optind >= nargc || *place++ != '-') {
			/* Argument is absent or is not an option */
			place = EMSG;
			return (-1);
		}
		optopt = *place++;
		if (optopt == '-' && *place == 0) {
			/* "--" => end of options */
			++optind;
			place = EMSG;
			return (-1);
		}
		if (optopt == 0) {
			/* Solitary '-', treat as a '-' option
			   if the program (eg su) is looking for it. */
			place = EMSG;
			if (strchr(ostr, '-') == NULL)
				return (-1);
			optopt = '-';
		}
	} else
		optopt = *place++;

	/* See if option letter is one the caller wanted... */
	if (optopt == ':' || (oli = strchr(ostr, optopt)) == NULL) {
		if (*place == 0)
			++optind;
		if (opterr && *ostr != ':')
			fprintf(stderr,
			    "%s: illegal option -- %c\n", program_invocation_name,
			    optopt);
		return (BADCH);
	}

	/* Does this option need an argument? */
	if (oli[1] != ':') {
		/* don't need argument */
		optarg = NULL;
		if (*place == 0)
			++optind;
	} else {
		/* Option-argument is either the rest of this argument or the
		   entire next argument. */
		if (*place)
			optarg = place;
		else if (nargc > ++optind)
			optarg = nargv[optind];
		else {
			/* option-argument absent */
			place = EMSG;
			if (*ostr == ':')
				return (BADARG);
			if (opterr)
				fprintf(stderr,
				    "%s: option requires an argument -- %c\n",
				    program_invocation_name, optopt);
			return (BADCH);
		}
		place = EMSG;
		++optind;
	}
	return (optopt);			/* return option letter */
}
