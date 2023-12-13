/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│ vi: set noet ft=c ts=8 sw=8 fenc=utf-8                                   :vi │
╚──────────────────────────────────────────────────────────────────────────────╝
│                                                                              │
│ Copyright (C) Lucent Technologies 1997                                       │
│ All Rights Reserved                                                          │
│                                                                              │
│ Permission to use, copy, modify, and distribute this software and            │
│ its documentation for any purpose and without fee is hereby                  │
│ granted, provided that the above copyright notice appear in all              │
│ copies and that both that the copyright notice and this                      │
│ permission notice and warranty disclaimer appear in supporting               │
│ documentation, and that the name Lucent Technologies or any of               │
│ its entities not be used in advertising or publicity pertaining              │
│ to distribution of the software without specific, written prior              │
│ permission.                                                                  │
│                                                                              │
│ LUCENT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,                │
│ INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.             │
│ IN NO EVENT SHALL LUCENT OR ANY OF ITS ENTITIES BE LIABLE FOR ANY            │
│ SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES                    │
│ WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER              │
│ IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,               │
│ ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF               │
│ THIS SOFTWARE.                                                               │
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#define DEBUG
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/str/locale.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "third_party/awk/awk.h"

asm(".ident\t\"\\n\\n\
Copyright (C) Lucent Technologies 1997\\n\
All Rights Reserved\\n\
\\n\
Permission to use, copy, modify, and distribute this software and\\n\
its documentation for any purpose and without fee is hereby\\n\
granted, provided that the above copyright notice appear in all\\n\
copies and that both that the copyright notice and this\\n\
permission notice and warranty disclaimer appear in supporting\\n\
documentation, and that the name Lucent Technologies or any of\\n\
its entities not be used in advertising or publicity pertaining\\n\
to distribution of the software without specific, written prior\\n\
permission.\\n\
\\n\
LUCENT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,\\n\
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.\\n\
IN NO EVENT SHALL LUCENT OR ANY OF ITS ENTITIES BE LIABLE FOR ANY\\n\
SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES\\n\
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER\\n\
IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,\\n\
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF\\n\
THIS SOFTWARE.\"");

const char	*version = "version 20220530";
extern	int	nfields;

int	dbg	= 0;
Awkfloat	srand_seed = 1;
char	*cmdname;	/* gets argv[0] for error messages */
extern	FILE	*yyin;	/* lex input file */
char	*lexprog;	/* points to program argument if it exists */
enum compile_states	compile_time = ERROR_PRINTING;

static char	**pfile;	/* program filenames from -f's */
static size_t	maxpfile;	/* max program filename */
static size_t	npfile;		/* number of filenames */
static size_t	curpfile;	/* current filename */

bool	safe = false;	/* true => "safe" mode */

static wontreturn void fpecatch(int n, siginfo_t *si, void *uc)
{
	const char *emsg[10];
        emsg[0] = "Unknown error";
        emsg[FPE_INTDIV] = "Integer divide by zero";
        emsg[FPE_INTOVF] = "Integer overflow";
        emsg[FPE_FLTDIV] = "Floating point divide by zero";
        emsg[FPE_FLTOVF] = "Floating point overflow";
        emsg[FPE_FLTUND] = "Floating point underflow";
        emsg[FPE_FLTRES] = "Floating point inexact result";
        emsg[FPE_FLTINV] = "Invalid Floating point operation";
        emsg[FPE_FLTSUB] = "Subscript out of range";
	FATAL("floating point exception: %s",
              (size_t)si->si_code < sizeof(emsg) / sizeof(emsg[0]) &&
              emsg[si->si_code] ? emsg[si->si_code] : emsg[0]);
}

/* Can this work with recursive calls?  I don't think so.
void segvcatch(int n)
{
	FATAL("segfault.  Do you have an unbounded recursive call?", n);
}
*/

static const char *
setfs(char *p)
{
	/* wart: t=>\t */
	if (p[0] == 't' && p[1] == '\0')
		return "\t";
	return p;
}

static char *
getarg(int *argc, char ***argv, const char *msg)
{
	if ((*argv)[1][2] != '\0') {	/* arg is -fsomething */
		return &(*argv)[1][2];
	} else {			/* arg is -f something */
		(*argc)--; (*argv)++;
		if (*argc <= 1)
			FATAL("%s", msg);
		return (*argv)[1];
	}
}

int _awk(int argc, char *argv[])
{
	const char *fs = NULL;
	struct sigaction sa;
	char *fn, *vn;

	setlocale(LC_CTYPE, "");
	setlocale(LC_NUMERIC, "C"); /* for parsing cmdline & prog */
	cmdname = argv[0];

	if (pledge("stdio rpath wpath cpath proc exec", NULL) == -1) {
		fprintf(stderr, "%s: pledge: incorrect arguments\n",
		    cmdname);
		exit(1);
	}

	if (argc == 1) {
		fprintf(stderr,
		  "usage: %s [-F fs] [-v var=value] [-f progfile | 'prog'] [file ...]\n",
		  cmdname);
		exit(1);
	}

	sa.sa_sigaction = fpecatch;
	sa.sa_flags = SA_SIGINFO;
	sigemptyset(&sa.sa_mask);
	(void)sigaction(SIGFPE, &sa, NULL);

	/* Set and keep track of the random seed */
	srand_seed = 1;
	srandom((unsigned long) srand_seed);

	yyin = NULL;
	symtab = makesymtab(NSYMTAB/NSYMTAB);
	while (argc > 1 && argv[1][0] == '-' && argv[1][1] != '\0') {
		if (strcmp(argv[1], "-version") == 0 || strcmp(argv[1], "--version") == 0) {
			printf("awk %s\n", version);
			return 0;
		}
		if (strcmp(argv[1], "--") == 0) {	/* explicit end of args */
			argc--;
			argv++;
			break;
		}
		switch (argv[1][1]) {
		case 's':
			if (strcmp(argv[1], "-safe") == 0)
				safe = true;
			break;
		case 'f':	/* next argument is program filename */
			fn = getarg(&argc, &argv, "no program filename");
			if (npfile >= maxpfile) {
				maxpfile += 20;
				pfile = (char **) realloc(pfile, maxpfile * sizeof(*pfile));
				if (pfile == NULL)
					FATAL("error allocating space for -f options");
 			}
			pfile[npfile++] = fn;
 			break;
		case 'F':	/* set field separator */
			fs = setfs(getarg(&argc, &argv, "no field separator"));
			break;
		case 'v':	/* -v a=1 to be done NOW.  one -v for each */
			vn = getarg(&argc, &argv, "no variable name");
			if (isclvar(vn))
				setclvar(vn);
			else
				FATAL("invalid -v option argument: %s", vn);
			break;
		case 'd':
			dbg = atoi(&argv[1][2]);
			if (dbg == 0)
				dbg = 1;
			printf("awk %s\n", version);
			break;
		default:
			WARNING("unknown option %s ignored", argv[1]);
			break;
		}
		argc--;
		argv++;
	}
	/* argv[1] is now the first argument */
	if (npfile == 0) {	/* no -f; first argument is program */
		if (argc <= 1) {
			if (dbg)
				exit(0);
			FATAL("no program given");
		}
		DPRINTF("program = |%s|\n", argv[1]);
		lexprog = argv[1];
		argc--;
		argv++;
	}
	recinit(recsize);
	syminit();
	compile_time = COMPILING;
	argv[0] = cmdname;	/* put prog name at front of arglist */
	DPRINTF("argc=%d, argv[0]=%s\n", argc, argv[0]);
	arginit(argc, argv);
	if (!safe)
		envinit(environ);
	yyparse();
	if (fs)
		*FS = qstring(fs, '\0');
	DPRINTF("errorflag=%d\n", errorflag);
	if (errorflag == 0) {
		compile_time = RUNNING;
		run(winner);
	} else
		bracecheck();
	return(errorflag);
}

int pgetc(void)		/* get 1 character from awk program */
{
	int c;

	for (;;) {
		if (yyin == NULL) {
			if (curpfile >= npfile)
				return EOF;
			if (strcmp(pfile[curpfile], "-") == 0)
				yyin = stdin;
			else if ((yyin = fopen(pfile[curpfile], "r")) == NULL)
				FATAL("can't open file %s", pfile[curpfile]);
			lineno = 1;
		}
		if ((c = getc(yyin)) != EOF)
			return c;
		if (yyin != stdin)
			fclose(yyin);
		yyin = NULL;
		curpfile++;
	}
}

char *cursource(void)	/* current source file name */
{
	if (npfile > 0)
		return pfile[curpfile < npfile ? curpfile : curpfile - 1];
	else
		return NULL;
}
