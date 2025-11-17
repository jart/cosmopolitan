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
 */

/*
 * Routines to check for mail.  (Perhaps make part of main.c?)
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "shell.h"
#include "nodes.h"
#include "exec.h"	/* defines padvance() */
#include "var.h"
#include "output.h"
#include "memalloc.h"
#include "error.h"
#include "mail.h"
#include "mystring.h"


#define MAXMBOXES 10

/* times of mailboxes */
static time_t mailtime[MAXMBOXES];
/* Set if MAIL or MAILPATH is changed. */
static int changed;



/*
 * Print appropriate message(s) if mail has arrived.  If changed is set,
 * then the value of MAIL has changed, so we just update the values.
 */

void
chkmail(void)
{
	const char *mpath;
	char *p;
	char *q;
	time_t *mtp;
	struct stackmark smark;
	struct stat64 statb;

	setstackmark(&smark);
	mpath = mpathset() ? mpathval() : mailval();
	for (mtp = mailtime; mtp < mailtime + MAXMBOXES; mtp++) {
		int len;

		len = padvance_magic(&mpath, nullstr, 2);
		if (len < 0)
			break;
		p = stackblock();
		if (*p == '\0')
			continue;
		for (q = p ; *q ; q++);
#ifdef DEBUG
		if (q[-1] != '/')
			abort();
#endif
		q[-1] = '\0';			/* delete trailing '/' */
		if (stat64(p, &statb) < 0) {
			*mtp = 0;
			continue;
		}
		if (!changed && statb.st_mtime != *mtp) {
			outfmt(
				&errout, snlfmt,
				pathopt ? pathopt : "you have mail"
			);
		}
		*mtp = statb.st_mtime;
	}
	changed = 0;
	popstackmark(&smark);
}


void
changemail(const char *val)
{
	changed++;
}
