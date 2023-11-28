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
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/paths.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/s.h"
#include "libc/time/struct/tm.h"
#include "libc/time/time.h"
#include "third_party/finger/finger.h"

/*
 * from: @(#)lprint.c	5.13 (Berkeley) 10/31/90
 */
char lprint_rcsid[] =
  "$Id: lprint.c,v 1.11 1999/09/14 10:51:11 dholland Exp $";

static void lprint(PERSON *pn);
static int demi_print(char *str, int oddfield);
static int show_text(const char *directory, const char *file_name,
		     const char *header);

#define	LINE_LEN	80
#define	TAB_LEN		8		/* 8 spaces between tabs */
#define	_PATH_FORWARD	".forward"
#define	_PATH_PLAN	".plan"
#define	_PATH_PROJECT	".project"
#define _PATH_PGPKEY	".pgpkey"

void
lflag_print(void)
{
	register PERSON *pn = phead;
	while (1) {
		lprint(pn);
		if (!pplan) {
			show_text(pn->dir, _PATH_PGPKEY, "PGP key:\n");
			show_text(pn->dir, _PATH_PROJECT, "Project:\n");
			if (!show_text(pn->dir, _PATH_PLAN, "Plan:\n")) {
				xprintf("No Plan.\n");
			}
		}
		if (!(pn = pn->next))
			break;
		xputc('\n');
	}
}

static void
lprint(PERSON *pn)
{
	struct tm *delta, *tp;
	WHERE *w;
	int cpr, len, maxlen;
	int oddfield;
	char timebuf[128];

	/*
	 * long format --
	 *	login name
	 *	real name
	 *	home directory
	 *	shell
	 *	office, office phone, home phone if available
	 */
	xprintf("Login: %-15s\t\t\tName: %s\nDirectory: %-25s",
		pn->name, pn->realname, pn->dir);
	xprintf("\tShell: %-s\n", *pn->shell ? pn->shell : _PATH_BSHELL);

	/*
	 * try and print office, office phone, and home phone on one line;
	 * if that fails, do line filling so it looks nice.
	 */
#define	OFFICE_TAG		"Office"
#define	OFFICE_PHONE_TAG	"Office Phone"
	oddfield = 0;
	if (pn->office && pn->officephone &&
	    strlen(pn->office) + strlen(pn->officephone) +
	    sizeof(OFFICE_TAG) + 2 <= 5 * TAB_LEN)
	{
		snprintf(tbuf, TBUFLEN, "%s: %s, %s", OFFICE_TAG, pn->office,
			 prphone(pn->officephone));
		oddfield = demi_print(tbuf, oddfield);
	}
	else {
		if (pn->office) {
			snprintf(tbuf, TBUFLEN, "%s: %s", OFFICE_TAG,
				 pn->office);
			oddfield = demi_print(tbuf, oddfield);
		}
		if (pn->officephone) {
			snprintf(tbuf, TBUFLEN, "%s: %s", OFFICE_PHONE_TAG,
				 prphone(pn->officephone));
			oddfield = demi_print(tbuf, oddfield);
		}
	}
	if (pn->homephone) {
		snprintf(tbuf, TBUFLEN, "%s: %s", "Home Phone",
			 prphone(pn->homephone));
		oddfield = demi_print(tbuf, oddfield);
	}
	if (oddfield) xputc('\n');

	/*
	 * long format con't: * if logged in
	 *	terminal
	 *	idle time
	 *	if messages allowed
	 *	where logged in from
	 * if not logged in
	 *	when last logged in
	 */
	/* find out longest device name for this user for formatting */
	for (w = pn->whead, maxlen = -1; w != NULL; w = w->next)
		if ((len = strlen(w->tty)) > maxlen)
			maxlen = len;
	/* find rest of entries for user */
	for (w = pn->whead; w != NULL; w = w->next) {
		switch (w->info) {
		  case LOGGEDIN:
			tp = localtime(&w->loginat);
			/*
			 * t = asctime(tp);
			 * tzset();
			 * tzn = tzname[daylight];
			 * cpr = printf("On since %.16s (%s) on %s",
			 *   t, tzn, w->tty);
			 */
			strftime(timebuf, sizeof(timebuf),
				 "%a %b %e %R (%Z)", tp);
			cpr = xprintf("On since %s on %s", timebuf, w->tty);
			if (*w->host) {
				cpr += xprintf(" from %s", w->host);
			}
			/*
			 * idle time is tough; if have one, print a comma,
			 * then spaces to pad out the device name, then the
			 * idle time.  Follow with a comma if a remote login.
			 */
			delta = gmtime(&w->idletime);
			if (delta->tm_yday || delta->tm_hour
			    || delta->tm_min || delta->tm_sec) {
				if (*w->host)
				    xputc('\n');
				cpr += xprintf("%-*s",
				    (int) (maxlen - strlen(w->tty) + 3), "");
				if (delta->tm_yday > 0) {
					cpr += xprintf("%d day%s ",
					   delta->tm_yday,
					   delta->tm_yday == 1 ? "" : "s");
				}
				if (delta->tm_hour > 0) {
					cpr += xprintf("%d hour%s ",
					   delta->tm_hour,
					   delta->tm_hour == 1 ? "" : "s");
				}
				if ((delta->tm_min > 0) && !delta->tm_yday) {
					cpr += xprintf("%d minute%s ",
					   delta->tm_min,
					   delta->tm_min == 1 ? "" : "s");
				}
				if ((delta->tm_sec > 0) && !delta->tm_yday
				     && !delta->tm_hour) {
					cpr += xprintf("%d second%s ",
					   delta->tm_sec,
					   delta->tm_sec == 1 ? "" : "s");
				}
				cpr += xprintf("idle");
			}
			if (!w->writable) {
				if (delta->tm_yday || delta->tm_hour
				    || delta->tm_min || delta->tm_sec)
					cpr += xprintf("\n    ");
				cpr += xprintf(" (messages off)");
			}
			break;
		case LASTLOG:
			if (w->loginat == 0) {
				(void)xprintf("Never logged in.");
				break;
			}
			tp = localtime(&w->loginat);
			/*
			 * t = asctime(tp);
			 * tzset();
			 * tzn = tzname[daylight];
			 * if(now - w->loginat > SECSPERDAY * DAYSPERNYEAR / 2)
			 *	cpr =
			 *	    printf("Last login %.16s %.4s (%s) on %s",
			 *	    t, t + 20, tzn, w->tty);
			 * else
			 *	cpr = printf("Last login %.16s (%s) on %s",
			 *	    t, tzn, w->tty);
			 */
			if (now - w->loginat < SECSPERDAY * DAYSPERNYEAR / 2) {
				strftime(timebuf, sizeof(timebuf),
					 "%a %b %e %R (%Z)", tp);
			}
			else {
				strftime(timebuf, sizeof(timebuf),
					 "%a %b %e %R %Y (%Z)", tp);
			}
			cpr = xprintf("Last login %s on %s", timebuf, w->tty);
			if (*w->host) {
				cpr += xprintf(" from %s", w->host);
			}
			break;
		}
		xputc('\n');
	}

	/* If the user forwards mail elsewhere, tell us about it */
	show_text(pn->dir, _PATH_FORWARD, "Mail forwarded to ");

	/* Print the standard mailbox information. */
		if (pn->mailrecv == -1)
			xprintf("No mail.\n");
		else if (pn->mailrecv > pn->mailread) {
			tp = localtime(&pn->mailrecv);
			/*
			 * t = asctime(tp);
			 * tzset();
			 * tzn = tzname[daylight];
			 * printf("New mail received %.16s %.4s (%s)\n", t,
			 *      t + 20, tzn);
			 */
			strftime(timebuf, sizeof(timebuf),
				 "%a %b %e %R %Y (%Z)", tp);
			xprintf("New mail received %s\n", timebuf);
			tp = localtime(&pn->mailread);
			/*
			 * t = asctime(tp);
			 * tzset();
			 * tzn = tzname[daylight];
			 * printf("     Unread since %.16s %.4s (%s)\n", t,
			 *      t + 20, tzn);
			 */
			strftime(timebuf, sizeof(timebuf),
				 "%a %b %e %R %Y (%Z)", tp);
			xprintf("     Unread since %s\n", timebuf);
		} else {
			tp = localtime(&pn->mailread);
			/*
			 * t = asctime(tp);
			 * tzset();
			 * tzn = tzname[daylight];
			 * printf("Mail last read %.16s %.4s (%s)\n", t,
			 *      t + 20, tzn);
			 */
			strftime(timebuf, sizeof(timebuf),
				 "%a %b %e %R %Y (%Z)", tp);
			xprintf("Mail last read %s\n", timebuf);
		}
}

static int
demi_print(char *str, int oddfield)
{
	static int lenlast;
	int lenthis, maxlen;

	lenthis = strlen(str);
	if (oddfield) {
		/*
		 * We left off on an odd number of fields.  If we haven't
		 * crossed the midpoint of the screen, and we have room for
		 * the next field, print it on the same line; otherwise,
		 * print it on a new line.
		 *
		 * Note: we insist on having the right hand fields start
		 * no less than 5 tabs out.
		 */
		maxlen = 5 * TAB_LEN;
		if (maxlen < lenlast)
			maxlen = lenlast;
		if (((((maxlen / TAB_LEN) + 1) * TAB_LEN) +
		    lenthis) <= LINE_LEN) {
			while(lenlast < (4 * TAB_LEN)) {
				xputc('\t');
				lenlast += TAB_LEN;
			}
			(void)xprintf("\t%s\n", str);	/* force one tab */
		} else {
			(void)xprintf("\n%s", str);	/* go to next line */
			oddfield = !oddfield;	/* this'll be undone below */
		}
	} else
		(void)xprintf("%s", str);
	oddfield = !oddfield;			/* toggle odd/even marker */
	lenlast = lenthis;
	return(oddfield);
}

static int
show_text(const char *directory, const char *file_name, const char *header)
{
	int ch, lastc = 0, fd;
	FILE *fp;
	struct stat sbuf1, sbuf2;

	snprintf(tbuf, TBUFLEN, "%s/%s", directory, file_name);

	if (lstat(tbuf, &sbuf1) || !S_ISREG(sbuf1.st_mode)) return 0;
	fd = open(tbuf, O_RDONLY);
	if (fd<0) return 0;
	if (fstat(fd, &sbuf2)) { close(fd); return 0; }
	/* if we didn't get the same file both times, bail */
	if (sbuf1.st_dev!=sbuf2.st_dev || sbuf1.st_ino!=sbuf2.st_ino) {
		close(fd);
		return 0;
	}
	fp = fdopen(fd, "r");
	if (fp == NULL) { close(fd); return 0; }

	xprintf("%s", header);
	while ((ch = getc(fp)) != EOF) {
		xputc(ch);
		lastc = ch;
	}
	if (lastc != '\n') xputc('\n');

	fclose(fp);
	return 1;
}

