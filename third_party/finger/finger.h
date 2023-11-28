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
 *
 *	from: @(#)finger.h	5.5 (Berkeley) 6/1/90
 *	$Id: finger.h,v 1.7 1999/09/14 10:51:11 dholland Exp $
 */
#include "third_party/musl/passwd.h"
#include "libc/runtime/utmp.h"
#include "libc/calls/weirdtypes.h"

/*
 * All unique persons are linked in a list headed by "head" and linkd
 * by the "next" field, as well as kept in a hash table.
 */

typedef struct person {
	struct person *next;		/* link to next person */
	struct person *hlink;		/* link to next person in hash bucket */
	uid_t uid;			/* user id */
	char *dir;			/* user's home directory */
	char *homephone;		/* pointer to home phone no. */
	char *name;			/* login name */
	char *office;			/* pointer to office name */
	char *officephone;		/* pointer to office phone no. */
	char *realname;			/* pointer to full name */
	char *shell;			/* user's shell */
	time_t mailread;		/* last time mail was read */
	time_t mailrecv;		/* last time mail was read */
	struct where *whead, *wtail;	/* list of where he is or has been */
} PERSON;

enum status { LASTLOG, LOGGEDIN };

typedef struct where {
	struct where *next;		/* next place he is or has been */
	enum status info;		/* type/status of request */
	short writable;			/* tty is writable */
	time_t loginat;			/* time of (last) login */
	time_t idletime;		/* how long idle (if logged in) */
	char tty[UT_LINESIZE+1];	/* null terminated tty line */
	char host[UT_HOSTSIZE+1];	/* null terminated remote host name */
} WHERE;

extern PERSON *phead, *ptail;		/* the linked list of all people */

extern int entries;			/* number of people */

#define TBUFLEN 1024
extern char tbuf[TBUFLEN];		/* temp buffer for anybody */

extern time_t now;
extern int lflag, pplan;

struct utmp;
PERSON *enter_person(struct passwd *);
PERSON *find_person(const char *name);
PERSON *palloc(void);
WHERE *walloc(PERSON *);
void lflag_print(void);
void sflag_print(void);
void enter_where(struct utmp *ut, PERSON *pn);
void enter_lastlog(PERSON *pn);
int match(struct passwd *pw, const char *user);
void netfinger(const char *name);
const char *prphone(const char *num);

#ifndef DAYSPERNYEAR
#define DAYSPERNYEAR  365
#endif

#ifndef SECSPERDAY
#define SECSPERDAY  (60 * 60 * 24)
#endif

/* turn on crnl translation on output */
void set_crmode(void);

/* Display, masking control characters and possibly doing crnl translation */
void xputc(int ch);
void xputs(const char *buf);
int xprintf(const char *fmt, ...);

/* Send to stderr, possibly doing crnl translation */
int eprintf(const char *fmt, ...);

/* terminal inquiries */
int is8bit(void);
int getscreenwidth(void);
