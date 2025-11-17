/*-
 * Copyright (c) 1993
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

#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

/*
 * Evaluate a command.
 */

#include "init.h"
#include "main.h"
#include "shell.h"
#include "nodes.h"
#include "syntax.h"
#include "expand.h"
#include "parser.h"
#include "jobs.h"
#include "eval.h"
#include "builtins.h"
#include "options.h"
#include "exec.h"
#include "redir.h"
#include "input.h"
#include "output.h"
#include "trap.h"
#include "var.h"
#include "memalloc.h"
#include "error.h"
#include "show.h"
#include "mystring.h"
#ifndef SMALL
#include "myhistedit.h"
#endif


int evalskip;			/* set if we are skipping commands */
STATIC int skipcount;		/* number of levels to skip */
MKINIT int loopnest;		/* current loop nesting level */
static int funcline;		/* starting line number of current function, or 0 if not in a function */


char *commandname;
int exitstatus;			/* exit status of last command */
int back_exitstatus;		/* exit status of backquoted command */
int savestatus = -1;		/* exit status of last command outside traps */

/* Prevent PS4 nesting. */
MKINIT int inps4;

MKINIT int tpip[2] = { -1 };

#if !defined(__alpha__) || (defined(__GNUC__) && __GNUC__ >= 3)
STATIC
#endif
void evaltreenr(union node *, int) __attribute__ ((__noreturn__));
STATIC int evalloop(union node *, int);
STATIC int evalfor(union node *, int);
STATIC int evalcase(union node *, int);
STATIC int evalsubshell(union node *, int);
STATIC void expredir(union node *);
STATIC int evalpipe(union node *, int);
#ifdef notyet
STATIC int evalcommand(union node *, int, struct backcmd *);
#else
STATIC int evalcommand(union node *, int);
#endif
STATIC int evalbltin(const struct builtincmd *, int, char **, int);
STATIC int evalfun(struct funcnode *, int, char **, int);
STATIC void prehash(union node *);
STATIC int eprintlist(struct output *, struct strlist *, int);
STATIC int bltincmd(int, char **);


STATIC const struct builtincmd bltin = {
	.name = nullstr,
	.builtin = bltincmd,
	.flags = BUILTIN_REGULAR,
};


/*
 * Called to reset things after an exception.
 */

#ifdef mkinit
INCLUDE "eval.h"

EXITRESET {
	if (savestatus >= 0) {
		if (exception == EXEXIT || evalskip == SKIPFUNCDEF)
			exitstatus = savestatus;
		savestatus = -1;
	}
	evalskip = 0;
	loopnest = 0;
	inps4 = 0;

	if (tpip[0] >= 0) {
		close(tpip[0]);
		close(tpip[1]);
	}
}
#endif



/*
 * The eval commmand.
 */

static int evalcmd(int argc, char **argv, int flags)
{
        char *p;
        char *concat;
        char **ap;

        if (argc > 1) {
                p = argv[1];
                if (argc > 2) {
                        STARTSTACKSTR(concat);
                        ap = argv + 2;
                        for (;;) {
                        	concat = stputs(p, concat);
                                if ((p = *ap++) == NULL)
                                        break;
                                STPUTC(' ', concat);
                        }
                        STPUTC('\0', concat);
                        p = grabstackstr(concat);
                }
                return evalstring(p, flags & EV_TESTED);
        }
        return 0;
}


/*
 * Execute a command or commands contained in a string.
 */

int
evalstring(char *s, int flags)
{
	union node *n;
	struct stackmark smark;
	int status;

	s = sstrdup(s);
	setinputstring(s);
	setstackmark(&smark);

	status = 0;
	for (; (n = parsecmd(0)) != NEOF; popstackmark(&smark)) {
		int i;

		i = evaltree(n, flags & ~(parser_eof() ? 0 : EV_EXIT));
		if (n)
			status = i;

		if (evalskip)
			break;
	}
	popstackmark(&smark);
	popfile();
	stunalloc(s);

	return status;
}



/*
 * Evaluate a parse tree.  The value is left in the global variable
 * exitstatus.
 */

int
evaltree(union node *n, int flags)
{
	int checkexit = 0;
	int (*evalfn)(union node *, int);
	struct stackmark smark;
	unsigned isor;
	int status = 0;

	setstackmark(&smark);

	if (nflag)
		goto out;

	if (n == NULL) {
		TRACE(("evaltree(NULL) called\n"));
		goto out;
	}

	dotrap();

#ifndef SMALL
	displayhist = 1;	/* show history substitutions done with fc */
#endif
	TRACE(("pid %d, evaltree(%p: %d, %d) called\n",
	    getpid(), n, n->type, flags));
	switch (n->type) {
	default:
#ifdef DEBUG
		out1fmt("Node type = %d\n", n->type);
#ifndef USE_GLIBC_STDIO
		flushout(out1);
#endif
		break;
#endif
	case NNOT:
		status = evaltree(n->nnot.com, EV_TESTED);
		if (!evalskip)
			status = !status;
		break;
	case NREDIR:
		errlinno = lineno = n->nredir.linno;
		if (funcline)
			lineno -= funcline - 1;
		expredir(n->nredir.redirect);
		pushredir(n->nredir.redirect);
		status = redirectsafe(n->nredir.redirect, REDIR_PUSH);
		if (status)
			checkexit = EV_TESTED;
		else
			status = evaltree(n->nredir.n, flags & EV_TESTED);
		if (n->nredir.redirect)
			popredir(0);
		break;
	case NCMD:
		evalfn = evalcommand;
checkexit:
		checkexit = EV_TESTED;
		goto calleval;
	case NFOR:
		evalfn = evalfor;
		goto calleval;
	case NWHILE:
	case NUNTIL:
		evalfn = evalloop;
		goto calleval;
	case NSUBSHELL:
	case NBACKGND:
		evalfn = evalsubshell;
		goto checkexit;
	case NPIPE:
		evalfn = evalpipe;
		goto checkexit;
	case NCASE:
		evalfn = evalcase;
		goto calleval;
	case NAND:
	case NOR:
	case NSEMI:
#if NAND + 1 != NOR
#error NAND + 1 != NOR
#endif
#if NOR + 1 != NSEMI
#error NOR + 1 != NSEMI
#endif
		isor = n->type - NAND;
		status = evaltree(n->nbinary.ch1,
				  (flags | ((isor >> 1) - 1)) & EV_TESTED);
		if ((!status) == isor || evalskip)
			break;
		n = n->nbinary.ch2;
evaln:
		evalfn = evaltree;
calleval:
		status = evalfn(n, flags);
		break;
	case NIF:
		status = evaltree(n->nif.test, EV_TESTED);
		if (evalskip)
			break;
		if (!status) {
			n = n->nif.ifpart;
			goto evaln;
		} else if (n->nif.elsepart) {
			n = n->nif.elsepart;
			goto evaln;
		}
		status = 0;
		break;
	case NDEFUN:
		defun(n);
		break;
	}

	exitstatus = status;

out:
	dotrap();

	if (eflag && (~flags & checkexit) && status)
		goto exexit;

	if (flags & EV_EXIT) {
exexit:
		exraise(EXEND);
	}

	popstackmark(&smark);

	return exitstatus;
}


#if !defined(__alpha__) || (defined(__GNUC__) && __GNUC__ >= 3)
STATIC
#endif
void evaltreenr(union node *n, int flags)
#ifdef HAVE_ATTRIBUTE_ALIAS
	__attribute__ ((alias("evaltree")));
#else
{
	evaltree(n, flags);
	abort();
}
#endif


static int skiploop(void)
{
	int skip = evalskip;

	switch (skip) {
	case 0:
		break;

	case SKIPBREAK:
	case SKIPCONT:
		if (likely(--skipcount <= 0)) {
			evalskip = 0;
			break;
		}

		skip = SKIPBREAK;
		break;
	}

	return skip;
}


STATIC int
evalloop(union node *n, int flags)
{
	int skip;
	int status;

	loopnest++;
	status = 0;
	flags &= EV_TESTED;
	do {
		int i;

		i = evaltree(n->nbinary.ch1, EV_TESTED);
		skip = skiploop();
		if (skip == SKIPFUNC)
			status = i;
		if (skip)
			continue;
		if (n->type != NWHILE)
			i = !i;
		if (i != 0)
			break;
		status = evaltree(n->nbinary.ch2, flags);
		skip = skiploop();
	} while (!(skip & ~SKIPCONT));
	loopnest--;

	return status;
}



STATIC int
evalfor(union node *n, int flags)
{
	struct arglist arglist;
	union node *argp;
	struct strlist *sp;
	int status;

	errlinno = lineno = n->nfor.linno;
	if (funcline)
		lineno -= funcline - 1;

	arglist.lastp = &arglist.list;
	for (argp = n->nfor.args ; argp ; argp = argp->narg.next) {
		expandarg(argp, &arglist, EXP_FULL | EXP_TILDE);
	}
	*arglist.lastp = NULL;

	status = 0;
	loopnest++;
	flags &= EV_TESTED;
	for (sp = arglist.list ; sp ; sp = sp->next) {
		setvar(n->nfor.var, sp->text, 0);
		status = evaltree(n->nfor.body, flags);
		if (skiploop() & ~SKIPCONT)
			break;
	}
	loopnest--;

	return status;
}



STATIC int
evalcase(union node *n, int flags)
{
	union node *cp;
	union node *patp;
	struct arglist arglist;
	int status = 0;

	errlinno = lineno = n->ncase.linno;
	if (funcline)
		lineno -= funcline - 1;

	arglist.lastp = &arglist.list;
	expandarg(n->ncase.expr, &arglist, FNMATCH_IS_ENABLED ? EXP_TILDE :
					   EXP_TILDE | EXP_MBCHAR);
	for (cp = n->ncase.cases ; cp && evalskip == 0 ; cp = cp->nclist.next) {
		for (patp = cp->nclist.pattern ; patp ; patp = patp->narg.next) {
			if (casematch(patp, arglist.list->text)) {
				/* Ensure body is non-empty as otherwise
				 * EV_EXIT may prevent us from setting the
				 * exit status.
				 */
				if (evalskip == 0 && cp->nclist.body) {
					status = evaltree(cp->nclist.body,
							  flags);
				}
				goto out;
			}
		}
	}
out:
	return status;
}



/*
 * Kick off a subshell to evaluate a tree.
 */

STATIC int
evalsubshell(union node *n, int flags)
{
	struct job *jp;
	int backgnd = (n->type == NBACKGND);
	int status;

	errlinno = lineno = n->nredir.linno;
	if (funcline)
		lineno -= funcline - 1;

	expredir(n->nredir.redirect);
	INTOFF;
	if (!backgnd && flags & EV_EXIT && !have_traps()) {
		forkreset(NULL);
		goto nofork;
	}
	jp = makejob(1);
	if (forkshell(jp, n->nredir.n, backgnd) == 0) {
		flags |= EV_EXIT;
		if (backgnd)
			flags &=~ EV_TESTED;
nofork:
		INTON;
		redirect(n->nredir.redirect, 0);
		evaltreenr(n->nredir.n, flags);
		/* never returns */
	}
	status = 0;
	if (! backgnd)
		status = waitforjob(jp);
	INTON;
	return status;
}



/*
 * Compute the names of the files in a redirection list.
 */

STATIC void
expredir(union node *n)
{
	union node *redir;

	for (redir = n ; redir ; redir = redir->nfile.next) {
		struct arglist fn;
		fn.lastp = &fn.list;
		switch (redir->type) {
		case NFROMTO:
		case NFROM:
		case NTO:
		case NCLOBBER:
		case NAPPEND:
			expandarg(redir->nfile.fname, &fn, EXP_TILDE | EXP_REDIR);
			redir->nfile.expfname = fn.list->text;
			break;
		case NFROMFD:
		case NTOFD:
			if (redir->ndup.vname) {
				expandarg(redir->ndup.vname, &fn, EXP_TILDE | EXP_REDIR);
				fixredir(redir, fn.list->text, 1);
			}
			break;
		}
	}
}



/*
 * Evaluate a pipeline.  All the processes in the pipeline are children
 * of the process creating the pipeline.  (This differs from some versions
 * of the shell, which make the last process in a pipeline the parent
 * of all the rest.)
 */

STATIC int
evalpipe(union node *n, int flags)
{
	struct job *jp;
	struct nodelist *lp;
	int pipelen;
	int prevfd;
	int pip[2];
	int status = 0;

	TRACE(("evalpipe(0x%lx) called\n", (long)n));
	pipelen = 0;
	for (lp = n->npipe.cmdlist ; lp ; lp = lp->next)
		pipelen++;
	flags |= EV_EXIT;
	INTOFF;
	jp = makejob(pipelen);
	prevfd = -1;
	for (lp = n->npipe.cmdlist ; lp ; lp = lp->next) {
		prehash(lp->n);
		pip[1] = -1;
		if (lp->next) {
			if (pipe(pip) < 0) {
				close(prevfd);
				sh_error("Pipe call failed");
			}
		}
		if (forkshell(jp, lp->n, n->npipe.backgnd) == 0) {
			INTON;
			if (pip[1] >= 0) {
				close(pip[0]);
			}
			if (prevfd > 0) {
				reset_input();
				dup2(prevfd, 0);
				close(prevfd);
			}
			if (pip[1] > 1) {
				dup2(pip[1], 1);
				close(pip[1]);
			}
			evaltreenr(lp->n, flags);
			/* never returns */
		}
		if (prevfd >= 0)
			close(prevfd);
		prevfd = pip[0];
		close(pip[1]);
	}
	if (n->npipe.backgnd == 0) {
		status = waitforjob(jp);
		TRACE(("evalpipe:  job done exit status %d\n", status));
	}
	INTON;

	return status;
}



/*
 * Execute a command inside back quotes.  If it's a builtin command, we
 * want to save its output in a block obtained from malloc.  Otherwise
 * we fork off a subprocess and get the output of the command via a pipe.
 * Should be called with interrupts off.
 */

void
evalbackcmd(union node *n, struct backcmd *result)
{
	struct job *jp;
	int pip[2];
	int pid;

	result->fd = -1;
	result->buf = NULL;
	result->nleft = 0;
	result->jp = NULL;
	if (n == NULL) {
		goto out;
	}

	sh_pipe(pip, 0);
	tpip[0] = pip[0];
	tpip[1] = pip[1];
	jp = makejob(1);
	pid = forkshell(jp, n, FORK_NOJOB);
	tpip[0] = -1;
	if (pid == 0) {
		FORCEINTON;
		close(pip[0]);
		if (pip[1] != 1) {
			dup2(pip[1], 1);
			close(pip[1]);
		}
		ifsfree();
		evaltreenr(n, EV_EXIT);
		/* NOTREACHED */
	}
	close(pip[1]);
	result->fd = pip[0];
	result->jp = jp;

out:
	TRACE(("evalbackcmd done: fd=%d buf=0x%x nleft=%d jp=0x%x\n",
		result->fd, result->buf, result->nleft, result->jp));
}

static struct strlist *fill_arglist(struct arglist *arglist,
				    union node **argpp)
{
	struct strlist **lastp = arglist->lastp;
	union node *argp;

	while ((argp = *argpp)) {
		expandarg(argp, arglist, EXP_FULL | EXP_TILDE);
		*argpp = argp->narg.next;
		if (*lastp)
			break;
	}

	return *lastp;
}

static int parse_command_args(struct arglist *arglist, union node **argpp,
			      const char **path)
{
	struct strlist *sp = arglist->list;
	char *cp, c;

	for (;;) {
		sp = unlikely(sp->next) ? sp->next :
					  fill_arglist(arglist, argpp);
		if (!sp)
			return 0;
		cp = sp->text;
		if (*cp++ != '-')
			break;
		if (!(c = *cp++))
			break;
		if (c == '-' && !*cp) {
			if (likely(!sp->next) && !fill_arglist(arglist, argpp))
				return 0;
			sp = sp->next;
			break;
		}
		do {
			switch (c) {
			case 'p':
				*path = defpath;
				break;
			default:
				/* run 'typecmd' for other options */
				return 0;
			}
		} while ((c = *cp++));
	}

	arglist->list = sp;
	return DO_NOFUNC;
}

/*
 * Execute a simple command.
 */

STATIC int
#ifdef notyet
evalcommand(union node *cmd, int flags, struct backcmd *backcmd)
#else
evalcommand(union node *cmd, int flags)
#endif
{
	struct localvar_list *localvar_stop;
	struct parsefile *file_stop;
	struct redirtab *redir_stop;
	union node *argp;
	struct arglist arglist;
	struct arglist varlist;
	char **argv;
	int argc;
	struct strlist *osp;
	struct strlist *sp;
#ifdef notyet
	int pip[2];
#endif
	struct cmdentry cmdentry;
	struct job *jp;
	char *lastarg;
	const char *path;
	int spclbltin;
	int cmd_flag;
	int execcmd;
	int status;
	char **nargv;
	int vflags;
	int vlocal;

	errlinno = lineno = cmd->ncmd.linno;
	if (funcline)
		lineno -= funcline - 1;

	/* First expand the arguments. */
	TRACE(("evalcommand(0x%lx, %d) called\n", (long)cmd, flags));
	file_stop = parsefile;
	back_exitstatus = 0;

	cmdentry.cmdtype = CMDBUILTIN;
	cmdentry.u.cmd = &bltin;
	varlist.lastp = &varlist.list;
	*varlist.lastp = NULL;
	arglist.lastp = &arglist.list;
	*arglist.lastp = NULL;

	cmd_flag = 0;
	execcmd = 0;
	spclbltin = -1;
	vflags = 0;
	vlocal = 0;
	path = NULL;

	argc = 0;
	argp = cmd->ncmd.args;
	if ((osp = fill_arglist(&arglist, &argp))) {
		int pseudovarflag = 0;

		for (;;) {
			find_command(arglist.list->text, &cmdentry,
				     cmd_flag | DO_REGBLTIN, pathval());

			vlocal++;

			/* implement bltin and command here */
			if (cmdentry.cmdtype != CMDBUILTIN)
				break;

			pseudovarflag = cmdentry.u.cmd->flags & BUILTIN_ASSIGN;
			if (likely(spclbltin < 0)) {
				spclbltin =
					cmdentry.u.cmd->flags &
					BUILTIN_SPECIAL
				;
				vlocal = spclbltin ^ BUILTIN_SPECIAL;
			}
			execcmd = cmdentry.u.cmd == EXECCMD;
			if (likely(cmdentry.u.cmd != COMMANDCMD))
				break;

			cmd_flag = parse_command_args(&arglist, &argp, &path);
			if (!cmd_flag)
				break;
		}

		for (; argp; argp = argp->narg.next)
			expandarg(argp, &arglist,
				  pseudovarflag &&
				  isassignment(argp->narg.text) ?
				  EXP_VARTILDE : EXP_FULL | EXP_TILDE);

		for (sp = arglist.list; sp; sp = sp->next)
			argc++;

		if (execcmd && argc > 1)
			vflags = VEXPORT;
	}

	localvar_stop = pushlocalvars(vlocal);

	/* Reserve one extra spot at the front for shellexec. */
	nargv = stalloc(sizeof (char *) * (argc + 2));
	argv = ++nargv;
	for (sp = arglist.list ; sp ; sp = sp->next) {
		TRACE(("evalcommand arg: %s\n", sp->text));
		*nargv++ = sp->text;
	}
	*nargv = NULL;

	lastarg = NULL;
	if (iflag && funcline == 0 && argc > 0)
		lastarg = nargv[-1];

	preverrout.fd = 2;
	expredir(cmd->ncmd.redirect);
	redir_stop = pushredir(cmd->ncmd.redirect);
	status = redirectsafe(cmd->ncmd.redirect, REDIR_PUSH|REDIR_SAVEFD2);

	if (unlikely(status)) {
bail:
		exitstatus = status;

		/* We have a redirection error. */
		if (spclbltin > 0)
			exraise(EXERROR);

		goto out;
	}

	for (argp = cmd->ncmd.assign; argp; argp = argp->narg.next) {
		struct strlist **spp;

		spp = varlist.lastp;
		expandarg(argp, &varlist, EXP_VARTILDE);

		if (vlocal)
			mklocal((*spp)->text, VEXPORT);
		else
			setvareq((*spp)->text, vflags);
	}

	/* Print the command if xflag is set. */
	if (xflag && !inps4) {
		struct output *out;
		int sep;

		out = &preverrout;
		inps4 = 1;
		outstr(expandstr(ps4val()), out);
		inps4 = 0;
		sep = 0;
		sep = eprintlist(out, varlist.list, sep);
		eprintlist(out, osp, sep);
		outcslow('\n', out);
#ifdef FLUSHERR
		flushout(out);
#endif
	}

	/* Now locate the command. */
	if (cmdentry.cmdtype != CMDBUILTIN ||
	    !(cmdentry.u.cmd->flags & BUILTIN_REGULAR)) {
		path = unlikely(path) ? path : pathval();
		find_command(argv[0], &cmdentry, cmd_flag | DO_ERR, path);
	}

	jp = NULL;

	/* Execute the command. */
	switch (cmdentry.cmdtype) {
	case CMDUNKNOWN:
		status = 127;
#ifdef FLUSHERR
		flushout(&errout);
#endif
		goto bail;

	default:
		flush_input();

		/* Fork off a child process if necessary. */
		if (!(flags & EV_EXIT) || have_traps()) {
			INTOFF;
			jp = vforkexec(cmd, argv, path, cmdentry.u.index);
			break;
		}
		shellexec(argv, path, cmdentry.u.index);
		/* NOTREACHED */

	case CMDBUILTIN:
		if (evalbltin(cmdentry.u.cmd, argc, argv, flags) &&
		    !(exception == EXERROR && spclbltin <= 0)) {
raise:
			longjmp(handler->loc, 1);
		}
		break;

	case CMDFUNCTION:
		if (evalfun(cmdentry.u.func, argc, argv, flags))
			goto raise;
		break;
	}

	status = waitforjob(jp);
	FORCEINTON;

out:
	if (cmd->ncmd.redirect)
		popredir(execcmd);
	unwindredir(redir_stop);
	unwindfiles(file_stop);
	unwindlocalvars(localvar_stop);
	if (lastarg)
		/* dsl: I think this is intended to be used to support
		 * '_' in 'vi' command mode during line editing...
		 * However I implemented that within libedit itself.
		 */
		setvar("_", lastarg, 0);

	return status;
}

STATIC int
evalbltin(const struct builtincmd *cmd, int argc, char **argv, int flags)
{
	char *volatile savecmdname;
	struct jmploc *volatile savehandler;
	struct jmploc jmploc;
	int status;
	int i;

	savecmdname = commandname;
	savehandler = handler;
	if ((i = setjmp(jmploc.loc)))
		goto cmddone;
	handler = &jmploc;
	commandname = argv[0];
	argptr = argv + 1;
	optptr = NULL;			/* initialize nextopt */
	if (cmd == EVALCMD)
		status = evalcmd(argc, argv, flags);
	else
		status = (*cmd->builtin)(argc, argv);
	flushall();
	if (outerr(out1))
		sh_warnx("%s: I/O error", commandname);
	status |= outerr(out1);
	exitstatus = status;
cmddone:
	freestdout();
	commandname = savecmdname;
	handler = savehandler;

	return i;
}

STATIC int
evalfun(struct funcnode *func, int argc, char **argv, int flags)
{
	volatile struct shparam saveparam;
	struct jmploc *volatile savehandler;
	struct jmploc jmploc;
	int e;
	int savefuncline;
	int saveloopnest;

	saveparam = shellparam;
	savefuncline = funcline;
	saveloopnest = loopnest;
	savehandler = handler;
	if ((e = setjmp(jmploc.loc))) {
		goto funcdone;
	}
	INTOFF;
	handler = &jmploc;
	shellparam.malloc = 0;
	func->count++;
	funcline = func->n.ndefun.linno;
	loopnest = 0;
	INTON;
	shellparam.nparam = argc - 1;
	shellparam.p = argv + 1;
	shellparam.optind = 1;
	shellparam.optoff = -1;
	evaltree(func->n.ndefun.body, flags & EV_TESTED);
funcdone:
	INTOFF;
	loopnest = saveloopnest;
	funcline = savefuncline;
	freefunc(func);
	freeparam(&shellparam);
	shellparam = saveparam;
	handler = savehandler;
	INTON;
	evalskip &= ~(SKIPFUNC | SKIPFUNCDEF);
	return e;
}


/*
 * Search for a command.  This is called before we fork so that the
 * location of the command will be available in the parent as well as
 * the child.  The check for "goodname" is an overly conservative
 * check that the name will not be subject to expansion.
 */

STATIC void
prehash(union node *n)
{
	struct cmdentry entry;

	if (n->type == NCMD && n->ncmd.args)
		if (goodname(n->ncmd.args->narg.text))
			find_command(n->ncmd.args->narg.text, &entry, 0,
				     pathval());
}



/*
 * Builtin commands.  Builtin commands whose functions are closely
 * tied to evaluation are implemented here.
 */

/*
 * No command given.
 */

STATIC int
bltincmd(int argc, char **argv)
{
	/*
	 * Preserve exitstatus of a previous possible redirection
	 * as POSIX mandates
	 */
	return back_exitstatus;
}


/*
 * Handle break and continue commands.  Break, continue, and return are
 * all handled by setting the evalskip flag.  The evaluation routines
 * above all check this flag, and if it is set they start skipping
 * commands rather than executing them.  The variable skipcount is
 * the number of loops to break/continue, or the number of function
 * levels to return.  (The latter is always 1.)  It should probably
 * be an error to break out of more loops than exist, but it isn't
 * in the standard shell so we don't make it one here.
 */

int
breakcmd(int argc, char **argv)
{
	int n = argc > 1 ? number(argv[1]) : 1;

	if (n <= 0)
		badnum(argv[1]);
	if (n > loopnest)
		n = loopnest;
	if (n > 0) {
		evalskip = (**argv == 'c')? SKIPCONT : SKIPBREAK;
		skipcount = n;
	}
	return 0;
}


/*
 * The return command.
 */

int
returncmd(int argc, char **argv)
{
	int skip;
	int status;

	/*
	 * If called outside a function, do what ksh does;
	 * skip the rest of the file.
	 */
	if (argv[1]) {
		skip = SKIPFUNC;
		status = number(argv[1]);
	} else {
		skip = SKIPFUNCDEF;
		status = exitstatus;
	}
	evalskip = skip;

	return status;
}


int
falsecmd(int argc, char **argv)
{
	return 1;
}


int
truecmd(int argc, char **argv)
{
	return 0;
}


int
execcmd(int argc, char **argv)
{
	if (argc > 1) {
		iflag = 0;		/* exit on error */
		mflag = 0;
		optschanged();
		flush_input();
		shellexec(argv + 1, pathval(), 0);
	}
	return 0;
}


STATIC int
eprintlist(struct output *out, struct strlist *sp, int sep)
{
	while (sp) {
		const char *p;

		p = " %s";
		p += (1 - sep);
		sep |= 1;
		outfmt(out, p, sp->text);
		sp = sp->next;
	}

	return sep;
}
