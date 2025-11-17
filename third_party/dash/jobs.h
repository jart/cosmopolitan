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
 *	@(#)jobs.h	8.2 (Berkeley) 5/4/95
 */

#include <inttypes.h>
#include <sys/types.h>

/* Mode argument to forkshell.  Don't change FORK_FG or FORK_BG. */
#define FORK_FG 0
#define FORK_BG 1
#define FORK_NOJOB 2

/* mode flags for showjob(s) */
#define	SHOW_PGID	0x01	/* only show pgid - for jobs -p */
#define	SHOW_PID	0x04	/* include process pid */
#define	SHOW_CHANGED	0x08	/* only jobs whose state has changed */


/*
 * A job structure contains information about a job.  A job is either a
 * single process or a set of processes contained in a pipeline.  In the
 * latter case, pidlist will be non-NULL, and will point to a -1 terminated
 * array of pids.
 */

struct procstat {
	pid_t	pid;		/* process id */
 	int	status;		/* last process status from wait() */
 	char	*cmd;		/* text of command being run */
};

struct job {
	struct procstat ps0;	/* status of process */
	struct procstat *ps;	/* status or processes when more than one */
#if JOBS
	int stopstatus;		/* status of a stopped job */
#endif
	uint32_t
		nprocs: 16,	/* number of processes */
		state: 8,
#define	JOBRUNNING	0	/* at least one proc running */
#define	JOBSTOPPED	1	/* all procs are stopped */
#define	JOBDONE		2	/* all procs are completed */
#if JOBS
		sigint: 1,	/* job was killed by SIGINT */
		jobctl: 1,	/* job running under job control */
#endif
		waited: 1,	/* true if this entry has been waited for */
		used: 1,	/* true if this entry is in used */
		changed: 1;	/* true if status has changed */
	struct job *prev_job;	/* previous job */
};

union node;

extern pid_t backgndpid;	/* pid of last background process */
extern int job_warning;		/* user was warned about stopped jobs */
#if JOBS
extern int jobctl;		/* true if doing job control */
#else
#define jobctl 0
#endif
extern int vforked;		/* Set if we are in the vforked child */

void setjobctl(int);
int killcmd(int, char **);
int fgcmd(int, char **);
int bgcmd(int, char **);
int jobscmd(int, char **);
struct output;
void showjobs(struct output *, int);
int waitcmd(int, char **);
struct job *makejob(int);
int forkshell(struct job *, union node *, int);
struct job *vforkexec(union node *n, char **argv, const char *path, int idx);
int waitforjob(struct job *);
int stoppedjobs(void);
char *commandtextcont(union node *n, char *next);

#if ! JOBS
#define setjobctl(on) ((void)(on))	/* do nothing */
#endif
