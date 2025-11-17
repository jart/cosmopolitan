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
 *	@(#)input.h	8.2 (Berkeley) 5/4/95
 */

#include <limits.h>
#include <stdbool.h>

#ifdef SMALL
#define IS_DEFINED_SMALL 1
#else
#define IS_DEFINED_SMALL 0
#endif

#define PUNGETC_MAX (MB_LEN_MAX > 16 ? MB_LEN_MAX : 16)

/* PEOF (the end of file marker) is defined in syntax.h */
#define PEOA ((PEOF) - 1)

enum {
	INPUT_PUSH_FILE = 1,
	INPUT_NOFILE_OK = 2,
};

struct alias;

struct strpush {
	struct strpush *prev;	/* preceding string on stack */
	char *prevstring;
	int prevnleft;
	struct alias *ap;	/* if push was associated with an alias */
	char *string;		/* remember the string since it may change */

	/* Delay freeing so we can stop nested aliases. */
	struct strpush *spfree;

	/* Number of outstanding calls to pungetc. */
	int unget;
};

/*
 * The parsefile structure pointed to by the global variable parsefile
 * contains information about the current file being read.
 */

struct parsefile {
	struct parsefile *prev;	/* preceding file on stack */
	int linno;		/* current line */
	int fd;			/* file descriptor (or -1 if string) */
	int nleft;		/* number of chars left in this line */
	int eof;		/* do not read again once we hit EOF */
	char *nextc;		/* next char in buffer */
	char *buf;		/* input buffer */
	struct strpush *strpush; /* for pushing strings at this level */
	struct strpush basestrpush; /* so pushing one is fast */

	/* Delay freeing so we can stop nested aliases. */
	struct strpush *spfree;

#ifndef SMALL
	int lleft;		/* number of chars left in this buffer */
#endif

	/* Number of outstanding calls to pungetc. */
	int unget;
};

extern struct parsefile *parsefile;
extern int stdin_istty;

/*
 * The input line number.  Input.c just defines this variable, and saves
 * and restores it when files are pushed and popped.  The user of this
 * package must set its value.
 */
#define plinno (parsefile->linno)

int pgetc(void);
int pgetc_eoa(void);
void pungetc(void);
void pungetn(int);
void pushstring(char *, void *);
int setinputfile(const char *, int);
void setinputstring(char *);
void pushstdin(void);
void popfile(void);
void unwindfiles(struct parsefile *);
void popallfiles(void);
void flush_input(void);
void reset_input(void);

static inline int input_get_lleft(struct parsefile *pf)
{
#ifdef SMALL
	return 0;
#else
	return pf->lleft;
#endif
}

static inline void input_set_lleft(struct parsefile *pf, int len)
{
#ifndef SMALL
	pf->lleft = len;
#endif
}
