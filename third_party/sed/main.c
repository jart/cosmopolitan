/*	$NetBSD: main.c,v 1.36 2020/05/15 22:39:54 christos Exp $	*/

/*-
 * Copyright (c) 2013 Johann 'Myrkraverk' Oskarsson.
 * Copyright (c) 1992 Diomidis Spinellis.
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Diomidis Spinellis of Imperial College, University of London.
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

#if HAVE_NBTOOL_CONFIG_H
#include "nbtool_config.h"
#endif

#include <sys/cdefs.h>
__RCSID("$NetBSD: main.c,v 1.36 2020/05/15 22:39:54 christos Exp $");
#ifdef __FBSDID
__FBSDID("$FreeBSD: head/usr.bin/sed/main.c 252231 2013-06-26 04:14:19Z pfg $");
#endif

#ifndef lint
__COPYRIGHT("@(#) Copyright (c) 1992, 1993\
	The Regents of the University of California.  All rights reserved.");
#endif

#if 0
static const char sccsid[] = "@(#)main.c	8.2 (Berkeley) 1/3/94";
#endif

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <sys/stat.h>

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <locale.h>
#include <regex.h>
#include <stddef.h>
#define _WITH_GETLINE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "defs.h"
#include "extern.h"

/*
 * Linked list of units (strings and files) to be compiled
 */
struct s_compunit {
	struct s_compunit *next;
	enum e_cut {CU_FILE, CU_STRING} type;
	char *s;			/* Pointer to string or fname */
};

/*
 * Linked list pointer to compilation units and pointer to current
 * next pointer.
 */
static struct s_compunit *script, **cu_nextp = &script;

/*
 * Linked list of files to be processed
 */
struct s_flist {
	char *fname;
	struct s_flist *next;
};

/*
 * Linked list pointer to files and pointer to current
 * next pointer.
 */
static struct s_flist *files, **fl_nextp = &files;

FILE *infile;			/* Current input file */
FILE *outfile;			/* Current output file */

int aflag, eflag, nflag;
int rflags = 0;
static int rval;		/* Exit status */

static int ispan;		/* Whether inplace editing spans across files */

/*
 * Current file and line number; line numbers restart across compilation
 * units, but span across input files.  The latter is optional if editing
 * in place.
 */
const char *fname;		/* File name. */
const char *outfname;		/* Output file name */
static char oldfname[PATH_MAX];	/* Old file name (for in-place editing) */
static char tmpfname[PATH_MAX];	/* Temporary file name (for in-place editing) */
static const char *inplace;	/* Inplace edit file extension. */
u_long linenum;

static void add_compunit(enum e_cut, char *);
static void add_file(char *);
static void usage(void) __dead;

int
main(int argc, char *argv[])
{
	int c, fflag;
	char *temp_arg;

	setprogname(argv[0]);
	(void) setlocale(LC_ALL, "");

	fflag = 0;
	inplace = NULL;

	while ((c = getopt(argc, argv, "EI::ae:f:i::lnru")) != -1)
		switch (c) {
		case 'r':		/* Gnu sed compat */
		case 'E':
			rflags = REG_EXTENDED;
			break;
		case 'I':
			inplace = optarg ? optarg : __UNCONST("");
			ispan = 1;	/* span across input files */
			break;
		case 'a':
			aflag = 1;
			break;
		case 'e':
			eflag = 1;
			temp_arg = xmalloc(strlen(optarg) + 2);
			strcpy(temp_arg, optarg);
			strcat(temp_arg, "\n");
			add_compunit(CU_STRING, temp_arg);
			break;
		case 'f':
			fflag = 1;
			add_compunit(CU_FILE, optarg);
			break;
		case 'i':
			inplace = optarg ? optarg : __UNCONST("");
			ispan = 0;	/* don't span across input files */
			break;
		case 'l':
#ifdef _IOLBF
			c = setvbuf(stdout, NULL, _IOLBF, 0);
#else
			c = setlinebuf(stdout);
#endif
			if (c)
				warn("setting line buffered output failed");
			break;
		case 'n':
			nflag = 1;
			break;
		case 'u':
#ifdef _IONBF
			c = setvbuf(stdout, NULL, _IONBF, 0);
#else
			c = -1;
			errno = EOPNOTSUPP;
#endif
			if (c)
				warn("setting unbuffered output failed");
			break;
		default:
		case '?':
			usage();
		}
	argc -= optind;
	argv += optind;

	/* First usage case; script is the first arg */
	if (!eflag && !fflag && *argv) {
		add_compunit(CU_STRING, *argv);
		argv++;
	}

	compile();

	/* Continue with first and start second usage */
	if (*argv)
		for (; *argv; argv++)
			add_file(*argv);
	else
		add_file(NULL);
	process();
	cfclose(prog, NULL);
	if (fclose(stdout))
		err(1, "stdout");
	exit(rval);
}

static void
usage(void)
{
	(void)fprintf(stderr,
	    "Usage:  %s [-aElnru] command [file ...]\n"
	    "\t%s [-aElnru] [-e command] [-f command_file] [-I[extension]]\n"
	    "\t    [-i[extension]] [file ...]\n", getprogname(), getprogname());
	exit(1);
}

/*
 * Like fgets, but go through the chain of compilation units chaining them
 * together.  Empty strings and files are ignored.
 */
char *
cu_fgets(char *buf, int n, int *more)
{
	static enum {ST_EOF, ST_FILE, ST_STRING} state = ST_EOF;
	static FILE *f;		/* Current open file */
	static char *s;		/* Current pointer inside string */
	static char string_ident[30];
	char *p;

again:
	switch (state) {
	case ST_EOF:
		if (script == NULL) {
			if (more != NULL)
				*more = 0;
			return (NULL);
		}
		linenum = 0;
		switch (script->type) {
		case CU_FILE:
			if ((f = fopen(script->s, "r")) == NULL)
				err(1, "%s", script->s);
			fname = script->s;
			state = ST_FILE;
			goto again;
		case CU_STRING:
			if (((size_t)snprintf(string_ident,
			    sizeof(string_ident), "\"%s\"", script->s)) >=
			    sizeof(string_ident) - 1)
				(void)strcpy(string_ident +
				    sizeof(string_ident) - 6, " ...\"");
			fname = string_ident;
			s = script->s;
			state = ST_STRING;
			goto again;
		default:
			abort();
		}
	case ST_FILE:
		if ((p = fgets(buf, n, f)) != NULL) {
			linenum++;
			if (linenum == 1 && buf[0] == '#' && buf[1] == 'n')
				nflag = 1;
			if (more != NULL)
				*more = !feof(f);
			return (p);
		}
		script = script->next;
		(void)fclose(f);
		state = ST_EOF;
		goto again;
	case ST_STRING:
		if (linenum == 0 && s[0] == '#' && s[1] == 'n')
			nflag = 1;
		p = buf;
		for (;;) {
			if (n-- <= 1) {
				*p = '\0';
				linenum++;
				if (more != NULL)
					*more = 1;
				return (buf);
			}
			switch (*s) {
			case '\0':
				state = ST_EOF;
				if (s == script->s) {
					script = script->next;
					goto again;
				} else {
					script = script->next;
					*p = '\0';
					linenum++;
					if (more != NULL)
						*more = 0;
					return (buf);
				}
			case '\n':
				*p++ = '\n';
				*p = '\0';
				s++;
				linenum++;
				if (more != NULL)
					*more = 0;
				return (buf);
			default:
				*p++ = *s++;
			}
		}
	}
	/* NOTREACHED */
	return (NULL);
}

/*
 * Like fgets, but go through the list of files chaining them together.
 * Set len to the length of the line.
 */
int
mf_fgets(SPACE *sp, enum e_spflag spflag)
{
	struct stat sb;
	size_t len;
	static char *p = NULL;
	static size_t plen = 0;
	int c;
	static int firstfile;

	if (infile == NULL) {
		/* stdin? */
		if (files->fname == NULL) {
			if (inplace != NULL)
				errx(1, "-I or -i may not be used with stdin");
			infile = stdin;
			fname = "stdin";
			outfile = stdout;
			outfname = "stdout";
		}
		firstfile = 1;
	}

	for (;;) {
		if (infile != NULL && (c = getc(infile)) != EOF) {
			(void)ungetc(c, infile);
			break;
		}
		/* If we are here then either eof or no files are open yet */
		if (infile == stdin) {
			sp->len = 0;
			return (0);
		}
		if (infile != NULL) {
			fclose(infile);
			if (*oldfname != '\0') {
				/* if there was a backup file, remove it */
				unlink(oldfname);
				/*
				 * Backup the original.  Note that hard links
				 * are not supported on all filesystems.
				 */
				if ((link(fname, oldfname) != 0) &&
				   (rename(fname, oldfname) != 0)) {
					warn("rename()");
					if (*tmpfname)
						unlink(tmpfname);
					exit(1);
				}
				*oldfname = '\0';
			}
			if (*tmpfname != '\0') {
				if (outfile != NULL && outfile != stdout)
					if (fclose(outfile) != 0) {
						warn("fclose()");
						unlink(tmpfname);
						exit(1);
					}
				outfile = NULL;
				if (rename(tmpfname, fname) != 0) {
					/* this should not happen really! */
					warn("rename()");
					unlink(tmpfname);
					exit(1);
				}
				*tmpfname = '\0';
			}
			outfname = NULL;
		}
		if (firstfile == 0)
			files = files->next;
		else
			firstfile = 0;
		if (files == NULL) {
			sp->len = 0;
			return (0);
		}
		fname = files->fname;
		if (inplace != NULL) {
			if (lstat(fname, &sb) != 0)
				err(1, "%s", fname);
			if (!(sb.st_mode & S_IFREG))
				errx(1, "%s: %s %s", fname,
				    "in-place editing only",
				    "works for regular files");
			if (*inplace != '\0') {
				strlcpy(oldfname, fname,
				    sizeof(oldfname));
				len = strlcat(oldfname, inplace,
				    sizeof(oldfname));
				if (len > sizeof(oldfname))
					errx(1, "%s: name too long", fname);
			}
			char d_name[PATH_MAX], f_name[PATH_MAX];
			(void)strlcpy(d_name, fname, sizeof(d_name));
			(void)strlcpy(f_name, fname, sizeof(f_name));
			len = (size_t)snprintf(tmpfname, sizeof(tmpfname),
			    "%s/.!%ld!%s", dirname(d_name), (long)getpid(),
			    basename(f_name));
			if (len >= sizeof(tmpfname))
				errx(1, "%s: name too long", fname);
			unlink(tmpfname);
			if (outfile != NULL && outfile != stdout)
				fclose(outfile);
			if ((outfile = fopen(tmpfname, "w")) == NULL)
				err(1, "%s", fname);
			fchown(fileno(outfile), sb.st_uid, sb.st_gid);
			fchmod(fileno(outfile), sb.st_mode & ALLPERMS);
			outfname = tmpfname;
			if (!ispan) {
				linenum = 0;
				resetstate();
			}
		} else {
			outfile = stdout;
			outfname = "stdout";
		}
		if ((infile = fopen(fname, "r")) == NULL) {
			warn("%s", fname);
			rval = 1;
			continue;
		}
	}
	/*
	 * We are here only when infile is open and we still have something
	 * to read from it.
	 *
	 * Use getline() so that we can handle essentially infinite input
	 * data.  The p and plen are static so each invocation gives
	 * getline() the same buffer which is expanded as needed.
	 */
	ssize_t slen = getline(&p, &plen, infile);
	if (slen == -1)
		err(1, "%s", fname);
	if (slen != 0 && p[slen - 1] == '\n') {
		sp->append_newline = 1;
		slen--;
	} else if (!lastline()) {
		sp->append_newline = 1;
	} else {
		sp->append_newline = 0;
	}
	cspace(sp, p, (size_t)slen, spflag);

	linenum++;

	return (1);
}

/*
 * Add a compilation unit to the linked list
 */
static void
add_compunit(enum e_cut type, char *s)
{
	struct s_compunit *cu;

	cu = xmalloc(sizeof(struct s_compunit));
	cu->type = type;
	cu->s = s;
	cu->next = NULL;
	*cu_nextp = cu;
	cu_nextp = &cu->next;
}

/*
 * Add a file to the linked list
 */
static void
add_file(char *s)
{
	struct s_flist *fp;

	fp = xmalloc(sizeof(struct s_flist));
	fp->next = NULL;
	*fl_nextp = fp;
	fp->fname = s;
	fl_nextp = &fp->next;
}

static int
next_files_have_lines(void)
{
	struct s_flist *file;
	FILE *file_fd;
	int ch;

	file = files;
	while ((file = file->next) != NULL) {
		if ((file_fd = fopen(file->fname, "r")) == NULL)
			continue;

		if ((ch = getc(file_fd)) != EOF) {
			/*
			 * This next file has content, therefore current
			 * file doesn't contains the last line.
			 */
			ungetc(ch, file_fd);
			fclose(file_fd);
			return (1);
		}

		fclose(file_fd);
	}

	return (0);
}

int
lastline(void)
{
	int ch;

	if (feof(infile)) {
		return !(
		    (inplace == NULL || ispan) &&
		    next_files_have_lines());
	}
	if ((ch = getc(infile)) == EOF) {
		return !(
		    (inplace == NULL || ispan) &&
		    next_files_have_lines());
	}
	ungetc(ch, infile);
	return (0);
}
