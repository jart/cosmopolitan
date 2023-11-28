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
#include "libc/calls/struct/stat.macros.h"
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/paths.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "third_party/finger/finger.h"

#ifndef lint
/*static char sccsid[] = "from: @(#)util.c	5.14 (Berkeley) 1/17/91";*/
char util_rcsid[] = "$Id: util.c,v 1.18 1999/09/28 22:53:58 netbug Exp $";
#endif /* not lint */

#define	HBITS	8			/* number of bits in hash code */
#define	HSIZE	(1 << 8)		/* hash table size */
#define	HMASK	(HSIZE - 1)		/* hash code mask */
static PERSON *htab[HSIZE];		/* the buckets */

static int hash(const char *name);

static void find_idle_and_ttywrite(register WHERE *w) {
	struct stat sb;

	/* No device for X console. Utmp entry by XDM login (":0"). */
	if (w->tty[0] == ':') {
		w->idletime = 0;  /* would be nice to have it emit ??? */
		w->writable = 0;
		return;
	}
	snprintf(tbuf, TBUFLEN, "%s/%s", _PATH_DEV, w->tty);
	if (stat(tbuf, &sb) < 0) {
		eprintf("finger: %s: %s\n", tbuf, strerror(errno));
		return;
	}
	w->idletime = now < sb.st_atime ? 0 : now - sb.st_atime;

#define	TALKABLE	0220		/* tty is writable if 220 mode */
	w->writable = ((sb.st_mode & TALKABLE) == TALKABLE);
}

static void userinfo(PERSON *pn, struct passwd *pw) {
	char *p;
	struct stat sb;
	char *bp;
	char *rname;
	int i, j, ct;
	char *fields[4];
	int nfields;

	pn->uid = pw->pw_uid;
	pn->name = strdup(pw->pw_name);
	pn->dir = strdup(pw->pw_dir);
	pn->shell = strdup(pw->pw_shell);

	/* make a private copy of gecos to munge */
	strlcpy(tbuf, pw->pw_gecos, TBUFLEN);
	tbuf[TBUFLEN-1] = 0;  /* ensure null termination */
	bp = tbuf;

	/* why do we skip asterisks!?!? */
	if (*bp == '*') ++bp;

	/*
	 * fields[0] -> real name
	 * fields[1] -> office
	 * fields[2] -> officephone
	 * fields[3] -> homephone
	 */
	nfields = 0;
	for (p = strtok(bp, ","); p; p = strtok(NULL, ",")) {
		if (*p==0) p = NULL;  // skip empties
		if (nfields < 4) fields[nfields++] = p;
	}
	while (nfields<4) fields[nfields++] = NULL;

	if (fields[0]) {
		/*
		 * Ampersands in gecos get replaced by the capitalized login
		 * name. This is a major nuisance and whoever thought it up
		 * should be shot.
		 */
		p = fields[0];

		/* First, count the number of ampersands. */
		for (ct=i=0; p[i]; i++) if (p[i]=='&') ct++;
	
		/* This tells us how much space we need to copy the name. */
		rname = malloc(strlen(p) + ct*strlen(pw->pw_name) + 1);
		if (!rname) {
			eprintf("finger: Out of space.\n");
			exit(1);
		}

		/* Now, do it */
		for (i=j=0; p[i]; i++) {
			if (p[i]=='&') {
				strcpy(rname + j, pw->pw_name);
				if (islower(rname[j])) {
					rname[j] = toupper(rname[j]);
				}
				j += strlen(pw->pw_name);
			}
			else {
				rname[j++] = p[i];
			}
		}
		rname[j] = 0;

		pn->realname = rname;
	}

	pn->office =      fields[1] ? strdup(fields[1]) : NULL;
	pn->officephone = fields[2] ? strdup(fields[2]) : NULL;
	pn->homephone =   fields[3] ? strdup(fields[3]) : NULL;

	pn->mailrecv = -1;		/* -1 == not_valid */
	pn->mailread = -1;		/* -1 == not_valid */

	snprintf(tbuf, TBUFLEN, "%s/%s", _PATH_MAILDIR, pw->pw_name);
	if (stat(tbuf, &sb) < 0) {
		if (errno != ENOENT) {
			eprintf("finger: %s: %s\n", tbuf, strerror(errno));
			return;
		}
	}
	else if (sb.st_size != 0) {
		pn->mailrecv = sb.st_mtime;
		pn->mailread = sb.st_atime;
	}
}

int
match(struct passwd *pw, const char *user)
{
	char *p;
	int i, j, ct, rv=0;
	char *rname;

	strlcpy(tbuf, pw->pw_gecos, TBUFLEN);
	tbuf[TBUFLEN-1] = 0;  /* guarantee null termination */
	p = tbuf;

	/* why do we skip asterisks!?!? */
	if (*p == '*') ++p;

	/* truncate the uninteresting stuff off the end of gecos */
	p = strtok(p, ",");
	if (!p)	return 0;

	/*
	 * Ampersands get replaced by the login name.
	 */

	/* First, count the number of ampersands. */
	for (ct=i=0; p[i]; i++) if (p[i]=='&') ct++;

	/* This tells us how much space we need to copy the name. */
	rname = malloc(strlen(p) + ct*strlen(pw->pw_name) + 1);
	if (!rname) {
		eprintf("finger: Out of space.\n");
		exit(1);
	}

	/* Now, do it */
	for (i=j=0; p[i]; i++) {
	    if (p[i]=='&') {
		strcpy(rname + j, pw->pw_name);
		if (islower(rname[j])) rname[j] = toupper(rname[j]);
		j += strlen(pw->pw_name);
	    }
	    else {
		rname[j++] = p[i];
	    }
	}
	rname[j] = 0;

	for (p = strtok(rname, "\t "); p && !rv; p = strtok(NULL, "\t ")) {
	    if (!strcasecmp(p, user))
		rv = 1;
	}
	free(rname);

	return rv;
}

static int get_lastlog(int fd, uid_t uid, struct lastlog *ll) {
    loff_t pos;
    if (fd == -1) return -1;
    pos = (long)uid * sizeof(*ll);
    if (lseek(fd, pos, SEEK_SET) != pos) return -1;
    if (read(fd, ll, sizeof(*ll)) != sizeof(*ll)) return -1;
    return 0;
}

void enter_lastlog(PERSON *pn) {
	static int opened = 0, fd = -1;

	WHERE *w;
	struct lastlog ll;
	int doit = 0;

	/* some systems may not maintain lastlog, don't report errors. */
	if (!opened) {
		fd = open(_PATH_LASTLOG, O_RDONLY, 0);
		opened = 1;
	}
	if (get_lastlog(fd, pn->uid, &ll)) {
	    /* as if never logged in */
	    ll.ll_line[0] = ll.ll_host[0] = '\0';
	    ll.ll_time = 0;
	}

	if ((w = pn->whead) == NULL)
		doit = 1;
	else if (ll.ll_time != 0) {
		/* if last login is earlier than some current login */
		for (; !doit && w != NULL; w = w->next)
			if (w->info == LOGGEDIN && w->loginat < ll.ll_time)
				doit = 1;
		/*
		 * and if it's not any of the current logins
		 * can't use time comparison because there may be a small
		 * discrepency since login calls time() twice
		 */
		for (w = pn->whead; doit && w != NULL; w = w->next)
			if (w->info == LOGGEDIN &&
			    strncmp(w->tty, ll.ll_line, UT_LINESIZE) == 0)
				doit = 0;
	}
	if (doit) {
		w = walloc(pn);
		w->info = LASTLOG;
		bcopy(ll.ll_line, w->tty, UT_LINESIZE);
		w->tty[UT_LINESIZE] = 0;
		bcopy(ll.ll_host, w->host, UT_HOSTSIZE);
		w->host[UT_HOSTSIZE] = 0;
		w->loginat = ll.ll_time;
	}
}

void enter_where(struct utmp *ut, PERSON *pn) {
	register WHERE *w = walloc(pn);

	w->info = LOGGEDIN;
	bcopy(ut->ut_line, w->tty, UT_LINESIZE);
	w->tty[UT_LINESIZE] = 0;
	bcopy(ut->ut_host, w->host, UT_HOSTSIZE);
	w->host[UT_HOSTSIZE] = 0;
	w->loginat = ut->ut_time;
	find_idle_and_ttywrite(w);
}

PERSON * enter_person(struct passwd *pw) {
	register PERSON *pn, **pp;

	for (pp = htab + hash(pw->pw_name);
	     *pp != NULL && strcmp((*pp)->name, pw->pw_name) != 0;
	     pp = &(*pp)->hlink)
		;
	if ((pn = *pp) == NULL) {
		pn = palloc();
		entries++;
		if (phead == NULL)
			phead = ptail = pn;
		else {
			ptail->next = pn;
			ptail = pn;
		}
		pn->next = NULL;
		pn->hlink = NULL;
		*pp = pn;
		userinfo(pn, pw);
		pn->whead = NULL;
	}
	return(pn);
}

PERSON *find_person(const char *name) {
	register PERSON *pn;

	/* name may be only UT_NAMESIZE long and not terminated */
	for (pn = htab[hash(name)];
	     pn != NULL && strncmp(pn->name, name, UT_NAMESIZE) != 0;
	     pn = pn->hlink)
		;
	return(pn);
}

static int hash(const char *name) {
	register int h, i;

	h = 0;
	/* name may be only UT_NAMESIZE long and not terminated */
	for (i = UT_NAMESIZE; --i >= 0 && *name;)
		h = ((h << 2 | h >> (HBITS - 2)) ^ *name++) & HMASK;
	return(h);
}

PERSON *palloc(void) {
	PERSON *p;

	if ((p = (PERSON *)malloc((unsigned) sizeof(PERSON))) == NULL) {
		eprintf("finger: Out of space.\n");
		exit(1);
	}
	return(p);
}

WHERE *
walloc(PERSON *pn)
{
	register WHERE *w;

	if ((w = (WHERE *)malloc((unsigned) sizeof(WHERE))) == NULL) {
		eprintf("finger: Out of space.\n");
		exit(1);
	}
	if (pn->whead == NULL)
		pn->whead = pn->wtail = w;
	else {
		pn->wtail->next = w;
		pn->wtail = w;
	}
	w->next = NULL;
	return(w);
}

const char *
prphone(const char *num)
{
	char *p;
	const char *q;
	int len;
	static char pbuf[15];

	/* don't touch anything if the user has their own formatting */
	for (q = num; *q; ++q)
		if (!isdigit(*q))
			return(num);
	len = q - num;
	p = pbuf;
	switch(len) {
	case 11:			/* +0-123-456-7890 */
		*p++ = '+';
		*p++ = *num++;
		*p++ = '-';
		/* FALLTHROUGH */
	case 10:			/* 012-345-6789 */
		*p++ = *num++;
		*p++ = *num++;
		*p++ = *num++;
		*p++ = '-';
		/* FALLTHROUGH */
	case 7:				/* 012-3456 */
		*p++ = *num++;
		*p++ = *num++;
		*p++ = *num++;
		break;
	case 5:				/* x0-1234 */
	case 4:				/* x1234 */
		*p++ = 'x';
		*p++ = *num++;
		break;
	default:
		return num;
	}
	if (len != 4) {
		*p++ = '-';
		*p++ = *num++;
	}
	*p++ = *num++;
	*p++ = *num++;
	*p++ = *num++;
	*p = '\0';
	return(pbuf);
}
