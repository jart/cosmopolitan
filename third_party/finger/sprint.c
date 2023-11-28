/*
 * Copyright (c) 1989 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Tony Nardo of the Johns Hopkins University/Applied Physics Lab.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
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
#include "libc/mem/alg.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/time/struct/tm.h"
#include "libc/time/time.h"
#include "third_party/finger/finger.h"

#ifndef lint
/*static char sccsid[] = "from: @(#)sprint.c	5.8 (Berkeley) 12/4/90";*/
char sprint_rcsid[] = "$Id: sprint.c,v 1.10 1999/12/12 18:59:33 dholland Exp $";
#endif /* not lint */

static void stimeprint(WHERE *w);
static int psort(const void *a, const void *b);
static PERSON **sort(void);

void sflag_print(void) {
	register PERSON *pn;
	register WHERE *w;
	register char *p;
	PERSON **list;
	int maxlname, maxrname, space, cnt;

	list = sort();
	/*
	 * short format --
	 *	login name
	 *	real name
	 *	terminal name
	 *	if terminal writeable (add an '*' to the terminal name
	 *		if not)
	 *	if logged in show idle time and day logged in, else
	 *		show last login date and time.  If > 6 moths,
	 *		show year instead of time.
	 *	office location
	 *	office phone
	 */

	maxlname = maxrname = sizeof("Login  ");
	for (cnt = 0; cnt < entries; ++cnt) {
		int l;
		pn = list[cnt];
		l = pn->name ? strlen(pn->name) : 1;
		if (l > maxlname) maxlname = l;
		l = pn->realname ? strlen(pn->realname) : 1;
		if (l > maxrname) maxrname = l;
	}
	/* prevent screen overflow */
	space = getscreenwidth() - 50;
	if (maxlname + maxrname > space) maxrname = space - maxlname;

	/* add a space if there's room */
	if (maxlname + maxrname < space-2) { maxlname++; maxrname++; }

	(void)xprintf("%-*s %-*s %s\n", maxlname, "Login", maxrname,
	    "Name", " Tty      Idle  Login Time   Office     Office Phone");
	for (cnt = 0; cnt < entries; ++cnt) {
		pn = list[cnt];
		for (w = pn->whead; w != NULL; w = w->next) {
			(void)xprintf("%-*.*s %-*.*s ", maxlname, maxlname,
			    pn->name, maxrname, maxrname,
			    pn->realname ? pn->realname : "");
			if (!w->loginat) {
				(void)xprintf("  *     *  No logins   ");
				goto office;
			}
			(void)xputc(w->info == LOGGEDIN && !w->writable ?
				    '*' : ' ');
			if (*w->tty)
				(void)xprintf("%-7.7s ", w->tty);
			else
				(void)xprintf("        ");
			if (w->info == LOGGEDIN) {
				stimeprint(w);
				(void)xprintf("  ");
			} else
				(void)xprintf("    *  ");
			p = ctime(&w->loginat);
			(void)xprintf("%.6s", p + 4);
			if (now - w->loginat >= SECSPERDAY * DAYSPERNYEAR / 2)
				(void)xprintf("  %.4s", p + 20);
			else
				(void)xprintf(" %.5s", p + 11);
office:
			if (w->host[0] != '\0') {
				xprintf(" (%s)", w->host);
			} else {
			if (pn->office)
				(void)xprintf(" %-10.10s", pn->office);
			else if (pn->officephone)
				(void)xprintf(" %-10.10s", " ");
			if (pn->officephone)
				(void)xprintf(" %-.14s",
				    prphone(pn->officephone));
			}
			xputc('\n');
		}
	}
}

static PERSON **sort(void) {
	register PERSON *pn, **lp;
	PERSON **list;

	if (!(list = (PERSON **)malloc((unsigned)(entries * sizeof(PERSON *))))) {
		eprintf("finger: Out of space.\n");
		exit(1);
	}
	for (lp = list, pn = phead; pn != NULL; pn = pn->next)
		*lp++ = pn;
	(void)qsort(list, entries, sizeof(PERSON *), psort);
	return(list);
}

static int psort(const void *a, const void *b) {
	const PERSON *const *p = (const PERSON *const *)a;
	const PERSON *const *t = (const PERSON *const *)b;
	return(strcmp((*p)->name, (*t)->name));
}

static void stimeprint(WHERE *w) {
	register struct tm *delta;

	delta = gmtime(&w->idletime);
	if (!delta->tm_yday)
		if (!delta->tm_hour)
			if (!delta->tm_min)
				(void)xprintf("     ");
			else
				(void)xprintf("%5d", delta->tm_min);
		else
			(void)xprintf("%2d:%02d",
			    delta->tm_hour, delta->tm_min);
	else
		(void)xprintf("%4dd", delta->tm_yday);
}
