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
#include "third_party/finger/finger.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/weirdtypes.h"
#include "libc/limits.h"
#include "libc/mem/mem.h"
#include "libc/nt/struct/msg.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/utmp.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/time/time.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/musl/passwd.h"

/*
 * Mail status reporting added 931007 by Luke Mewburn, <zak@rmit.edu.au>.
 */

char copyright[] =
  "@(#) Copyright (c) 1989 The Regents of the University of California.\n"
  "All rights reserved.\n";

/*
 * from: @(#)finger.c	5.22 (Berkeley) 6/29/90
 */
char finger_rcsid[] = \
  "$Id: finger.c,v 1.15 1999/12/18 16:41:51 dholland Exp $";

/*
 * Finger prints out information about users.  It is not portable since
 * certain fields (e.g. the full user name, office, and phone numbers) are
 * extracted from the gecos field of the passwd file which other UNIXes
 * may not have or may use for other things. (This is not really true any
 * more, btw.)
 *
 * There are currently two output formats; the short format is one line
 * per user and displays login name, tty, login time, real name, idle time,
 * and office location/phone number.  The long format gives the same
 * information (in a more legible format) as well as home directory, shell,
 * mail info, and .plan/.project files.
 */

static void loginlist(void);
static void userlist(int argc, char *argv[]);

int lflag, pplan;
static int sflag, mflag;
static int enable_nofinger;

time_t now;
char tbuf[TBUFLEN];

PERSON *phead, *ptail;		/* the linked list of all people */
int entries;			/* number of people */


int main(int argc, char *argv[]) {
	int ch;
	struct sockaddr_in sin;
	socklen_t slen = sizeof(sin);

	while ((ch = getopt(argc, argv, "lmps")) != EOF) {
		switch(ch) {
		  case 'l':
			lflag = 1;		/* long format */
			break;
		  case 'm':
			mflag = 1;		/* do exact match of names */
			break;
		  case 'p':
			pplan = 1;		/* don't show .plan/.project */
			break;
		  case 's':
			sflag = 1;		/* short format */
			break;
		  case '?':
		  case 'h':
		  default:
			eprintf("usage: finger [-lmps] [login ...]\n");
			return 1;
		}
	}
	argc -= optind;
	argv += optind;

	if (getsockname(STDOUT_FILENO, (struct sockaddr *)&sin, &slen)==0) {
		/*
		 * stdout is a socket. must be a network finger request,
		 * so emit CRs with our LFs at the ends of lines.
		 */
		set_crmode();

		/*
		 * Also, enable .nofinger processing.
		 */
		enable_nofinger = 1;
	}

	/*
	 * Also check stdin for nofinger processing, because of older
	 * fingerds that make stdout a pipe for CRLF handling.
	 */
	if (getsockname(STDIN_FILENO, (struct sockaddr *)&sin, &slen)==0) {
		enable_nofinger = 1;
	}	

	time(&now);

	setpwent();

	if (!*argv) {
		/*
		 * Assign explicit "small" format if no names given and -l
		 * not selected.  Force the -s BEFORE we get names so proper
		 * screening will be done.
		 */
	        if (!lflag) {
			sflag = 1;	/* if -l not explicit, force -s */
		}
		loginlist();
		if (entries == 0) {
			xprintf("No one logged on.\n");
		}
	}
	else {
		userlist(argc, argv);
		/*
		 * Assign explicit "large" format if names given and -s not
		 * explicitly stated.  Force the -l AFTER we get names so any
		 * remote finger attempts specified won't be mishandled.
		 */
		if (!sflag)
			lflag = 1;	/* if -s not explicit, force -l */
	}
	if (entries != 0) {
		if (lflag) lflag_print();
		else sflag_print();
	}
	return 0;
}

/* Returns 1 if .nofinger is found and enable_nofinger is set. */
static
int
check_nofinger(struct passwd *pw)
{
	if (enable_nofinger) {
		char path[PATH_MAX];
		struct stat tripe;
		snprintf(path, sizeof(path), "%s/.nofinger", pw->pw_dir);
		if (stat(path, &tripe)==0) {
			return 1;
		}
	}
	return 0;
}

static void
loginlist(void)
{
	PERSON *pn;
	struct passwd *pw;
	struct utmp *uptr;
	char name[UT_NAMESIZE + 1];

	name[UT_NAMESIZE] = '\0';

	/*
	 * if (!freopen(_PATH_UTMP, "r", stdin)) {
	 *	fprintf(stderr, "finger: can't read %s.\n", _PATH_UTMP);
	 *	exit(2);
	 * }
	 */
	setutent();
	while ((uptr = getutent())!=NULL) {
		if (!uptr->ut_name[0])
			continue;
#ifdef USER_PROCESS
		if (uptr->ut_type != USER_PROCESS) continue;
#endif
		if ((pn = find_person(uptr->ut_name)) == NULL) {
			memcpy(name, uptr->ut_name, UT_NAMESIZE);
			if ((pw = getpwnam(name)) == NULL)
				continue;
			if (check_nofinger(pw))
				continue;
			pn = enter_person(pw);
		}
		enter_where(uptr, pn);
	}
	for (pn = phead; lflag && pn != NULL; pn = pn->next)
		enter_lastlog(pn);
	endutent();
}


static void do_local(int argc, char *argv[], int *used) {
	int i;
	struct passwd *pw;

	/*
	 * traverse the list of possible login names and check the login name
	 * and real name against the name specified by the user.
	 */
	if (mflag) {
		for (i = 0; i < argc; i++)
			if (used[i] >= 0 && (pw = getpwnam(argv[i]))) {
				if (!check_nofinger(pw)) {
					enter_person(pw);
					used[i] = 1;
				}
			}
	} else for (pw = getpwent(); pw; pw = getpwent())
		for (i = 0; i < argc; i++)
			if (used[i] >= 0 &&
			    (!strcasecmp(pw->pw_name, argv[i]) ||
			    match(pw, argv[i]))) {
				if (!check_nofinger(pw)) {
					enter_person(pw);
					used[i] = 1;
				}
			}

	/* list errors */
	for (i = 0; i < argc; i++)
		if (!used[i])
			(void)eprintf("finger: %s: no such user.\n", argv[i]);

}

static void
userlist(int argc, char *argv[])
{
	int i;
	PERSON *pn;
	PERSON *nethead, **nettail;
	struct utmp *uptr;
	int dolocal, *used;

	used = calloc(argc, sizeof(int));
	if (!used) {
		eprintf("finger: out of space.\n");
		exit(1);
	}

	/* pull out all network requests */
	for (i = 0, dolocal = 0, nettail = &nethead; i < argc; i++) {
		if (!strchr(argv[i], '@')) {
			dolocal = 1;
			continue;
		}
		pn = palloc();
		*nettail = pn;
		nettail = &pn->next;
		pn->name = argv[i];
		used[i] = -1;
	}
	*nettail = NULL;

	if (dolocal) do_local(argc, argv, used);

	/* handle network requests */
	for (pn = nethead; pn; pn = pn->next) {
		netfinger(pn->name);
		if (pn->next || entries)
			xputc('\n');
	}

	if (entries == 0)
		return;

	/*
	 * Scan thru the list of users currently logged in, saving
	 * appropriate data whenever a match occurs.
	 */
	/*
	 * if (!freopen(_PATH_UTMP, "r", stdin)) {
	 *	(void)fprintf( stderr, "finger: can't read %s.\n", _PATH_UTMP);
	 *	exit(1);
	 * }
	 */
	setutent();
	while ((uptr = getutent())!=NULL) {
		if (!uptr->ut_name[0])
			continue;
#ifdef USER_PROCESS
		if (uptr->ut_type != USER_PROCESS) continue;
#endif
		if ((pn = find_person(uptr->ut_name)) == NULL) {
			continue;
		}
		enter_where(uptr, pn);
	}
	for (pn = phead; pn != NULL; pn = pn->next) {
		enter_lastlog(pn);
	}
	endutent();
}
