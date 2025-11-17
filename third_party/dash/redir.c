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

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/param.h>	/* PIPE_BUF */
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

/*
 * Code for dealing with input/output redirection.
 */

#include "error.h"
#include "expand.h"
#include "input.h"
#include "jobs.h"
#include "main.h"
#include "memalloc.h"
#include "nodes.h"
#include "options.h"
#include "output.h"
#include "redir.h"
#include "shell.h"
#include "system.h"
#include "trap.h"


#define EMPTY -2		/* marks an unused slot in redirtab */
#define CLOSED -1		/* fd opened for redir needs to be closed */

#ifndef PIPE_BUF
# define PIPESIZE 4096		/* amount of buffering in a pipe */
#else
# define PIPESIZE PIPE_BUF
#endif


MKINIT
struct redirtab {
	struct redirtab *next;
	int renamed[10];
};


MKINIT struct redirtab *redirlist;

/* Bit map of currently closed file descriptors. */
static unsigned closed_redirs;

STATIC int openredirect(union node *);
#ifdef notyet
STATIC void dupredirect(union node *, int, char[10]);
#else
STATIC void dupredirect(union node *, int);
#endif
STATIC int openhere(union node *);


static unsigned update_closed_redirs(int fd, int nfd)
{
	unsigned val = closed_redirs;
	unsigned bit = 1 << fd;

	if (nfd >= 0)
		closed_redirs &= ~bit;
	else
		closed_redirs |= bit;

	return val & bit;
}


/*
 * Process a list of redirection commands.  If the REDIR_PUSH flag is set,
 * old file descriptors are stashed away so that the redirection can be
 * undone by calling popredir.  If the REDIR_BACKQ flag is set, then the
 * standard output, and the standard error if it becomes a duplicate of
 * stdout, is saved in memory.
 */

void
redirect(union node *redir, int flags)
{
	union node *n;
	struct redirtab *sv;
	int i;
	int fd;
	int newfd;
	int *p;
#if notyet
	char memory[10];	/* file descriptors to write to memory */

	for (i = 10 ; --i >= 0 ; )
		memory[i] = 0;
	memory[1] = flags & REDIR_BACKQ;
#endif
	if (!redir)
		return;
	sv = NULL;
	INTOFF;
	if (likely(flags & REDIR_PUSH))
		sv = redirlist;
	n = redir;
	do {
		newfd = openredirect(n);
		if (newfd < -1)
			continue;

		fd = n->nfile.fd;
		if (fd == 0)
			reset_input();

		if (sv) {
			int closed;

			p = &sv->renamed[fd];
			i = *p;

			closed = update_closed_redirs(fd, newfd);

			if (likely(i == EMPTY)) {
				i = CLOSED;
				if (fd != newfd && !closed) {
					i = savefd(fd, fd);
					fd = -1;
				}
			}

			*p = i;
		}

		if (fd == newfd)
			continue;

#ifdef notyet
		dupredirect(n, newfd, memory);
#else
		dupredirect(n, newfd);
#endif
	} while ((n = n->nfile.next));
	INTON;
#ifdef notyet
	if (memory[1])
		out1 = &memout;
	if (memory[2])
		out2 = &memout;
#endif
	if (flags & REDIR_SAVEFD2 && sv->renamed[2] >= 0)
		preverrout.fd = sv->renamed[2];
}


static int sh_open_fail(const char *, int, int) __attribute__((__noreturn__));
static int sh_open_fail(const char *pathname, int flags, int e)
{
	const char *word;
	int action;

	word = "open";
	action = E_OPEN;
	if (flags & O_CREAT) {
		word = "create";
		action = E_CREAT;
	}

	sh_error("cannot %s %s: %s", word, pathname, errmsg(e, action));
}


int sh_open(const char *pathname, int flags, int mayfail)
{
	int fd;
	int e;

	do {
		fd = open64(pathname, flags, 0666);
		e = errno;
	} while (fd < 0 && e == EINTR && !pending_sig);

	if (mayfail || fd >= 0)
		return fd;

	sh_open_fail(pathname, flags, e);
}


STATIC int
openredirect(union node *redir)
{
	struct stat64 sb;
	char *fname;
	int flags;
	int f;

	switch (redir->nfile.type) {
	case NFROM:
		flags = O_RDONLY;
do_open:
		f = sh_open(redir->nfile.expfname, flags, 0);
		break;
	case NFROMTO:
		flags = O_RDWR|O_CREAT;
		goto do_open;
	case NTO:
		/* Take care of noclobber mode. */
		if (Cflag) {
			fname = redir->nfile.expfname;
			if (stat64(fname, &sb) < 0) {
				flags = O_WRONLY|O_CREAT|O_EXCL;
				goto do_open;
			}

			if (S_ISREG(sb.st_mode))
				goto ecreate;

			f = sh_open(fname, O_WRONLY, 0);
			if (!fstat64(f, &sb) && S_ISREG(sb.st_mode)) {
				close(f);
				goto ecreate;
			}
			break;
		}
		/* FALLTHROUGH */
	case NCLOBBER:
		flags = O_WRONLY|O_CREAT|O_TRUNC;
		goto do_open;
	case NAPPEND:
		flags = O_WRONLY|O_CREAT|O_APPEND;
		goto do_open;
	case NTOFD:
	case NFROMFD:
		f = redir->ndup.dupfd;
		if (f == redir->nfile.fd)
			f = -2;
		break;
	default:
#ifdef DEBUG
		abort();
#endif
		/* Fall through to eliminate warning. */
	case NHERE:
	case NXHERE:
		f = openhere(redir);
		break;
	}

	return f;
ecreate:
	sh_open_fail(fname, O_CREAT, EEXIST);
}

static int sh_dup2(int ofd, int nfd, int cfd)
{
	if (nfd < 0) {
		nfd = dup(ofd);
		if (nfd >= 0)
			cfd = -1;
	} else
		nfd = dup2(ofd, nfd);
	if (likely(cfd >= 0))
		close(cfd);
	if (nfd < 0)
		sh_error("%d: %s", ofd, strerror(errno));

	return nfd;
}

#ifdef notyet
static void dupredirect(union node *redir, int f, char memory[10])
#else
static void dupredirect(union node *redir, int f)
#endif
{
	int fd = redir->nfile.fd;

#ifdef notyet
	memory[fd] = 0;
#endif
	if (redir->nfile.type == NTOFD || redir->nfile.type == NFROMFD) {
		/* if not ">&-" */
		if (f >= 0) {
#ifdef notyet
			if (memory[f])
				memory[fd] = 1;
			else
#endif
				sh_dup2(f, fd, -1);
			return;
		}
		close(fd);
	} else
		sh_dup2(f, fd, f);
}

int sh_pipe(int pip[2], int memfd)
{
	if (memfd) {
		pip[0] = memfd_create("dash", 0);
		if (pip[0] >= 0) {
			pip[1] = sh_dup2(pip[0], -1, pip[0]);
			return 1;
		}
	}

	if (pipe(pip) < 0)
		sh_error("Pipe call failed");

	return 0;
}

/*
 * Handle here documents.  Normally we fork off a process to write the
 * data to a pipe.  If the document is short, we can stuff the data in
 * the pipe without forking.
 */

STATIC int
openhere(union node *redir)
{
	size_t len = 0;
	int pip[2];
	int memfd;
	char *p;

	p = redir->nhere.doc->narg.text;
	if (redir->type == NXHERE) {
		expandarg(redir->nhere.doc, NULL, EXP_QUOTED);
		p = stackblock();
	}

	len = strlen(p);
	memfd = sh_pipe(pip, len > PIPESIZE);

	if (memfd || len <= PIPESIZE) {
		xwrite(pip[1], p, len);
		lseek(pip[1], 0, SEEK_SET);
		goto out;
	}

	if (forkshell((struct job *)NULL, (union node *)NULL, FORK_NOJOB) == 0) {
		close(pip[0]);
		signal(SIGINT, SIG_IGN);
		signal(SIGQUIT, SIG_IGN);
		signal(SIGHUP, SIG_IGN);
#ifdef SIGTSTP
		signal(SIGTSTP, SIG_IGN);
#endif
		signal(SIGPIPE, SIG_DFL);
		xwrite(pip[1], p, len);
		_exit(0);
	}
out:
	close(pip[1]);
	return pip[0];
}



/*
 * Undo the effects of the last redirection.
 */

void
popredir(int drop)
{
	struct redirtab *rp;
	int i;

	INTOFF;
	rp = redirlist;
	for (i = 0 ; i < 10 ; i++) {
		int closed;

		if (rp->renamed[i] == EMPTY)
			continue;

		closed = drop ? 1 : update_closed_redirs(i, rp->renamed[i]);

		switch (rp->renamed[i]) {
		case CLOSED:
			if (!closed)
				close(i);
			break;
		default:
			if (!drop) {
				if (i == 0)
					reset_input();
				dup2(rp->renamed[i], i);
			}
			close(rp->renamed[i]);
			break;
		}
	}
	redirlist = rp->next;
	ckfree(rp);
	INTON;
}

/*
 * Undo all redirections.  Called on error or interrupt.
 */

#ifdef mkinit

INCLUDE "redir.h"

EXITRESET {
	/*
	 * Discard all saved file descriptors.
	 */
	unwindredir(0);
}

FORKRESET {
	redirlist = NULL;
}

#endif



/*
 * Move a file descriptor to > 10.  Invokes sh_error on error unless
 * the original file dscriptor is not open.
 */

int
savefd(int from, int ofd)
{
	int newfd;
	int err;

#if HAVE_F_DUPFD_CLOEXEC
	newfd = fcntl(from, F_DUPFD_CLOEXEC, 10);
#else
	newfd = fcntl(from, F_DUPFD, 10);
#endif

	err = newfd < 0 ? errno : 0;
	if (err != EBADF) {
		close(ofd);
		if (err)
			sh_error("%d: %s", from, strerror(err));
		else if(!HAVE_F_DUPFD_CLOEXEC)
			fcntl(newfd, F_SETFD, FD_CLOEXEC);
	}

	return newfd;
}


int
redirectsafe(union node *redir, int flags)
{
	int err;
	volatile int saveint;
	struct jmploc *volatile savehandler = handler;
	struct jmploc jmploc;

	SAVEINT(saveint);
	if (!(err = setjmp(jmploc.loc) * 2)) {
		handler = &jmploc;
		redirect(redir, flags);
	}
	restore_handler_expandarg(savehandler, err);
	RESTOREINT(saveint);
	return err;
}


void unwindredir(struct redirtab *stop)
{
	while (redirlist != stop)
		popredir(0);
}


struct redirtab *pushredir(union node *redir)
{
	struct redirtab *sv;
	struct redirtab *q;
	int i;

	q = redirlist;
	if (!redir)
		goto out;

	sv = ckmalloc(sizeof (struct redirtab));
	sv->next = q;
	redirlist = sv;
	for (i = 0; i < 10; i++)
		sv->renamed[i] = EMPTY;

out:
	return q;
}
