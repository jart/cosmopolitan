/****************************************************************
Copyright (C) Lucent Technologies 1997
All Rights Reserved

Permission to use, copy, modify, and distribute this software and
its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appear in all
copies and that both that the copyright notice and this
permission notice and warranty disclaimer appear in supporting
documentation, and that the name Lucent Technologies or any of
its entities not be used in advertising or publicity pertaining
to distribution of the software without specific, written prior
permission.

LUCENT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
IN NO EVENT SHALL LUCENT OR ANY OF ITS ENTITIES BE LIABLE FOR ANY
SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
THIS SOFTWARE.
****************************************************************/

#define DEBUG
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>
#include <math.h>
#include "awk.h"

extern int u8_nextlen(const char *s);

char	EMPTY[] = { '\0' };
FILE	*infile	= NULL;
bool	innew;		/* true = infile has not been read by readrec */
char	*file	= EMPTY;
char	*record;
int	recsize	= RECSIZE;
char	*fields;
int	fieldssize = RECSIZE;

Cell	**fldtab;	/* pointers to Cells */
static size_t	len_inputFS = 0;
static char	*inputFS = NULL; /* FS at time of input, for field splitting */

#define	MAXFLD	2
int	nfields	= MAXFLD;	/* last allocated slot for $i */

bool	donefld;	/* true = implies rec broken into fields */
bool	donerec;	/* true = record is valid (no flds have changed) */

int	lastfld	= 0;	/* last used field */
int	argno	= 1;	/* current input argument number */
extern	Awkfloat *ARGC;

static Cell dollar0 = { OCELL, CFLD, NULL, EMPTY, 0.0, REC|STR|DONTFREE, NULL, NULL };
static Cell dollar1 = { OCELL, CFLD, NULL, EMPTY, 0.0, FLD|STR|DONTFREE, NULL, NULL };

void recinit(unsigned int n)
{
	if ( (record = (char *) malloc(n)) == NULL
	  || (fields = (char *) malloc(n+1)) == NULL
	  || (fldtab = (Cell **) calloc(nfields+2, sizeof(*fldtab))) == NULL
	  || (fldtab[0] = (Cell *) malloc(sizeof(**fldtab))) == NULL)
		FATAL("out of space for $0 and fields");
	*record = '\0';
	*fldtab[0] = dollar0;
	fldtab[0]->sval = record;
	fldtab[0]->nval = tostring("0");
	makefields(1, nfields);
}

void makefields(int n1, int n2)		/* create $n1..$n2 inclusive */
{
	char temp[50];
	int i;

	for (i = n1; i <= n2; i++) {
		fldtab[i] = (Cell *) malloc(sizeof(**fldtab));
		if (fldtab[i] == NULL)
			FATAL("out of space in makefields %d", i);
		*fldtab[i] = dollar1;
		snprintf(temp, sizeof(temp), "%d", i);
		fldtab[i]->nval = tostring(temp);
	}
}

void initgetrec(void)
{
	int i;
	char *p;

	for (i = 1; i < *ARGC; i++) {
		p = getargv(i); /* find 1st real filename */
		if (p == NULL || *p == '\0') {  /* deleted or zapped */
			argno++;
			continue;
		}
		if (!isclvar(p)) {
			setsval(lookup("FILENAME", symtab), p);
			return;
		}
		setclvar(p);	/* a commandline assignment before filename */
		argno++;
	}
	infile = stdin;		/* no filenames, so use stdin */
	innew = true;
}

/*
 * POSIX specifies that fields are supposed to be evaluated as if they were
 * split using the value of FS at the time that the record's value ($0) was
 * read.
 *
 * Since field-splitting is done lazily, we save the current value of FS
 * whenever a new record is read in (implicitly or via getline), or when
 * a new value is assigned to $0.
 */
void savefs(void)
{
	size_t len;
	if ((len = strlen(getsval(fsloc))) < len_inputFS) {
		strcpy(inputFS, *FS);	/* for subsequent field splitting */
		return;
	}

	len_inputFS = len + 1;
	inputFS = (char *) realloc(inputFS, len_inputFS);
	if (inputFS == NULL)
		FATAL("field separator %.10s... is too long", *FS);
	memcpy(inputFS, *FS, len_inputFS);
}

static bool firsttime = true;

int getrec(char **pbuf, int *pbufsize, bool isrecord)	/* get next input record */
{			/* note: cares whether buf == record */
	int c;
	char *buf = *pbuf;
	uschar saveb0;
	int bufsize = *pbufsize, savebufsize = bufsize;

	if (firsttime) {
		firsttime = false;
		initgetrec();
	}
	DPRINTF("RS=<%s>, FS=<%s>, ARGC=%g, FILENAME=%s\n",
		*RS, *FS, *ARGC, *FILENAME);
	saveb0 = buf[0];
	buf[0] = 0;
	while (argno < *ARGC || infile == stdin) {
		DPRINTF("argno=%d, file=|%s|\n", argno, file);
		if (infile == NULL) {	/* have to open a new file */
			file = getargv(argno);
			if (file == NULL || *file == '\0') {	/* deleted or zapped */
				argno++;
				continue;
			}
			if (isclvar(file)) {	/* a var=value arg */
				setclvar(file);
				argno++;
				continue;
			}
			*FILENAME = file;
			DPRINTF("opening file %s\n", file);
			if (*file == '-' && *(file+1) == '\0')
				infile = stdin;
			else if ((infile = fopen(file, "r")) == NULL)
				FATAL("can't open file %s", file);
			innew = true;
			setfval(fnrloc, 0.0);
		}
		c = readrec(&buf, &bufsize, infile, innew);
		if (innew)
			innew = false;
		if (c != 0 || buf[0] != '\0') {	/* normal record */
			if (isrecord) {
				double result;

				if (freeable(fldtab[0]))
					xfree(fldtab[0]->sval);
				fldtab[0]->sval = buf;	/* buf == record */
				fldtab[0]->tval = REC | STR | DONTFREE;
				if (is_number(fldtab[0]->sval, & result)) {
					fldtab[0]->fval = result;
					fldtab[0]->tval |= NUM;
				}
				donefld = false;
				donerec = true;
				savefs();
			}
			setfval(nrloc, nrloc->fval+1);
			setfval(fnrloc, fnrloc->fval+1);
			*pbuf = buf;
			*pbufsize = bufsize;
			return 1;
		}
		/* EOF arrived on this file; set up next */
		if (infile != stdin)
			fclose(infile);
		infile = NULL;
		argno++;
	}
	buf[0] = saveb0;
	*pbuf = buf;
	*pbufsize = savebufsize;
	return 0;	/* true end of file */
}

void nextfile(void)
{
	if (infile != NULL && infile != stdin)
		fclose(infile);
	infile = NULL;
	argno++;
}

extern int readcsvrec(char **pbuf, int *pbufsize, FILE *inf, bool newflag);

int readrec(char **pbuf, int *pbufsize, FILE *inf, bool newflag)	/* read one record into buf */
{
	int sep, c, isrec; // POTENTIAL BUG? isrec is a macro in awk.h
	char *rr = *pbuf, *buf = *pbuf;
	int bufsize = *pbufsize;
	char *rs = getsval(rsloc);

	if (CSV) {
		c = readcsvrec(pbuf, pbufsize, inf, newflag);
		isrec = (c == EOF && rr == buf) ? false : true;
	} else if (*rs && rs[1]) {
		bool found;

		memset(buf, 0, bufsize);
		fa *pfa = makedfa(rs, 1);
		if (newflag)
			found = fnematch(pfa, inf, &buf, &bufsize, recsize);
		else {
			int tempstat = pfa->initstat;
			pfa->initstat = 2;
			found = fnematch(pfa, inf, &buf, &bufsize, recsize);
			pfa->initstat = tempstat;
		}
		if (found)
			setptr(patbeg, '\0');
		isrec = (found == 0 && *buf == '\0') ? false : true;

	} else {
		if ((sep = *rs) == 0) {
			sep = '\n';
			while ((c=getc(inf)) == '\n' && c != EOF)	/* skip leading \n's */
				;
			if (c != EOF)
				ungetc(c, inf);
		}
		for (rr = buf; ; ) {
			for (; (c=getc(inf)) != sep && c != EOF; ) {
				if (rr-buf+1 > bufsize)
					if (!adjbuf(&buf, &bufsize, 1+rr-buf,
					    recsize, &rr, "readrec 1"))
						FATAL("input record `%.30s...' too long", buf);
				*rr++ = c;
			}
			if (*rs == sep || c == EOF)
				break;
			if ((c = getc(inf)) == '\n' || c == EOF)	/* 2 in a row */
				break;
			if (!adjbuf(&buf, &bufsize, 2+rr-buf, recsize, &rr,
			    "readrec 2"))
				FATAL("input record `%.30s...' too long", buf);
			*rr++ = '\n';
			*rr++ = c;
		}
		if (!adjbuf(&buf, &bufsize, 1+rr-buf, recsize, &rr, "readrec 3"))
			FATAL("input record `%.30s...' too long", buf);
		*rr = 0;
		isrec = (c == EOF && rr == buf) ? false : true;
	}
	*pbuf = buf;
	*pbufsize = bufsize;
	DPRINTF("readrec saw <%s>, returns %d\n", buf, isrec);
	return isrec;
}


/*******************
 * loose ends here:
 *   \r\n should become \n
 *   what about bare \r?  Excel uses that for embedded newlines
 *   can't have "" in unquoted fields, according to RFC 4180
*/


int readcsvrec(char **pbuf, int *pbufsize, FILE *inf, bool newflag) /* csv can have \n's */
{			/* so read a complete record that might be multiple lines */
	int sep, c;
	char *rr = *pbuf, *buf = *pbuf;
	int bufsize = *pbufsize;
	bool in_quote = false;

	sep = '\n'; /* the only separator; have to skip over \n embedded in "..." */
	rr = buf;
	while ((c = getc(inf)) != EOF) {
		if (c == sep) {
			if (! in_quote)
				break;
			if (rr > buf && rr[-1] == '\r')	// remove \r if was \r\n
				rr--;
		}

		if (rr-buf+1 > bufsize)
			if (!adjbuf(&buf, &bufsize, 1+rr-buf,
			    recsize, &rr, "readcsvrec 1"))
				FATAL("input record `%.30s...' too long", buf);
		*rr++ = c;
		if (c == '"')
			in_quote = ! in_quote;
 	}
	if (c == '\n' && rr > buf && rr[-1] == '\r') 	// remove \r if was \r\n
		rr--;

	if (!adjbuf(&buf, &bufsize, 1+rr-buf, recsize, &rr, "readcsvrec 4"))
		FATAL("input record `%.30s...' too long", buf);
	*rr = 0;
	*pbuf = buf;
	*pbufsize = bufsize;
	DPRINTF("readcsvrec saw <%s>, returns %d\n", buf, c);
	return c;
}

char *getargv(int n)	/* get ARGV[n] */
{
	Cell *x;
	char *s, temp[50];
	extern Array *ARGVtab;

	snprintf(temp, sizeof(temp), "%d", n);
	if (lookup(temp, ARGVtab) == NULL)
		return NULL;
	x = setsymtab(temp, "", 0.0, STR, ARGVtab);
	s = getsval(x);
	DPRINTF("getargv(%d) returns |%s|\n", n, s);
	return s;
}

void setclvar(char *s)	/* set var=value from s */
{
	char *e, *p;
	Cell *q;
	double result;

/* commit f3d9187d4e0f02294fb1b0e31152070506314e67 broke T.argv test */
/* I don't understand why it was changed. */

	for (p=s; *p != '='; p++)
		;
	e = p;
	*p++ = 0;
	p = qstring(p, '\0');
	q = setsymtab(s, p, 0.0, STR, symtab);
	setsval(q, p);
	if (is_number(q->sval, & result)) {
		q->fval = result;
		q->tval |= NUM;
	}
	DPRINTF("command line set %s to |%s|\n", s, p);
	free(p);
	*e = '=';
}


void fldbld(void)	/* create fields from current record */
{
	/* this relies on having fields[] the same length as $0 */
	/* the fields are all stored in this one array with \0's */
	/* possibly with a final trailing \0 not associated with any field */
	char *r, *fr, sep;
	Cell *p;
	int i, j, n;

	if (donefld)
		return;
	if (!isstr(fldtab[0]))
		getsval(fldtab[0]);
	r = fldtab[0]->sval;
	n = strlen(r);
	if (n > fieldssize) {
		xfree(fields);
		if ((fields = (char *) malloc(n+2)) == NULL) /* possibly 2 final \0s */
			FATAL("out of space for fields in fldbld %d", n);
		fieldssize = n;
	}
	fr = fields;
	i = 0;	/* number of fields accumulated here */
	if (inputFS == NULL)	/* make sure we have a copy of FS */
		savefs();
	if (!CSV && strlen(inputFS) > 1) {	/* it's a regular expression */
		i = refldbld(r, inputFS);
	} else if (!CSV && (sep = *inputFS) == ' ') {	/* default whitespace */
		for (i = 0; ; ) {
			while (*r == ' ' || *r == '\t' || *r == '\n')
				r++;
			if (*r == 0)
				break;
			i++;
			if (i > nfields)
				growfldtab(i);
			if (freeable(fldtab[i]))
				xfree(fldtab[i]->sval);
			fldtab[i]->sval = fr;
			fldtab[i]->tval = FLD | STR | DONTFREE;
			do
				*fr++ = *r++;
			while (*r != ' ' && *r != '\t' && *r != '\n' && *r != '\0');
			*fr++ = 0;
		}
		*fr = 0;
	} else if (CSV) {	/* CSV processing.  no error handling */
		if (*r != 0) {
			for (;;) {
				i++;
				if (i > nfields)
					growfldtab(i);
				if (freeable(fldtab[i]))
					xfree(fldtab[i]->sval);
				fldtab[i]->sval = fr;
				fldtab[i]->tval = FLD | STR | DONTFREE;
				if (*r == '"' ) { /* start of "..." */
					for (r++ ; *r != '\0'; ) {
						if (*r == '"' && r[1] != '\0' && r[1] == '"') {
							r += 2; /* doubled quote */
							*fr++ = '"';
						} else if (*r == '"' && (r[1] == '\0' || r[1] == ',')) {
							r++; /* skip over closing quote */
							break;
						} else {
							*fr++ = *r++;
						}
					}
					*fr++ = 0;
				} else {	/* unquoted field */
					while (*r != ',' && *r != '\0')
						*fr++ = *r++;
					*fr++ = 0;
				}
				if (*r++ == 0)
					break;
	
			}
		}
		*fr = 0;
	} else if ((sep = *inputFS) == 0) {	/* new: FS="" => 1 char/field */
		for (i = 0; *r != '\0'; ) {
			char buf[10];
			i++;
			if (i > nfields)
				growfldtab(i);
			if (freeable(fldtab[i]))
				xfree(fldtab[i]->sval);
			n = u8_nextlen(r);
			for (j = 0; j < n; j++)
				buf[j] = *r++;
			buf[j] = '\0';
			fldtab[i]->sval = tostring(buf);
			fldtab[i]->tval = FLD | STR;
		}
		*fr = 0;
	} else if (*r != 0) {	/* if 0, it's a null field */
		/* subtle case: if length(FS) == 1 && length(RS > 0)
		 * \n is NOT a field separator (cf awk book 61,84).
		 * this variable is tested in the inner while loop.
		 */
		int rtest = '\n';  /* normal case */
		if (strlen(*RS) > 0)
			rtest = '\0';
		for (;;) {
			i++;
			if (i > nfields)
				growfldtab(i);
			if (freeable(fldtab[i]))
				xfree(fldtab[i]->sval);
			fldtab[i]->sval = fr;
			fldtab[i]->tval = FLD | STR | DONTFREE;
			while (*r != sep && *r != rtest && *r != '\0')	/* \n is always a separator */
				*fr++ = *r++;
			*fr++ = 0;
			if (*r++ == 0)
				break;
		}
		*fr = 0;
	}
	if (i > nfields)
		FATAL("record `%.30s...' has too many fields; can't happen", r);
	cleanfld(i+1, lastfld);	/* clean out junk from previous record */
	lastfld = i;
	donefld = true;
	for (j = 1; j <= lastfld; j++) {
		double result;

		p = fldtab[j];
		if(is_number(p->sval, & result)) {
			p->fval = result;
			p->tval |= NUM;
		}
	}
	setfval(nfloc, (Awkfloat) lastfld);
	donerec = true; /* restore */
	if (dbg) {
		for (j = 0; j <= lastfld; j++) {
			p = fldtab[j];
			printf("field %d (%s): |%s|\n", j, p->nval, p->sval);
		}
	}
}

void cleanfld(int n1, int n2)	/* clean out fields n1 .. n2 inclusive */
{				/* nvals remain intact */
	Cell *p;
	int i;

	for (i = n1; i <= n2; i++) {
		p = fldtab[i];
		if (freeable(p))
			xfree(p->sval);
		p->sval = EMPTY,
		p->tval = FLD | STR | DONTFREE;
	}
}

void newfld(int n)	/* add field n after end of existing lastfld */
{
	if (n > nfields)
		growfldtab(n);
	cleanfld(lastfld+1, n);
	lastfld = n;
	setfval(nfloc, (Awkfloat) n);
}

void setlastfld(int n)	/* set lastfld cleaning fldtab cells if necessary */
{
	if (n < 0)
		FATAL("cannot set NF to a negative value");
	if (n > nfields)
		growfldtab(n);

	if (lastfld < n)
	    cleanfld(lastfld+1, n);
	else
	    cleanfld(n+1, lastfld);

	lastfld = n;
}

Cell *fieldadr(int n)	/* get nth field */
{
	if (n < 0)
		FATAL("trying to access out of range field %d", n);
	if (n > nfields)	/* fields after NF are empty */
		growfldtab(n);	/* but does not increase NF */
	return(fldtab[n]);
}

void growfldtab(int n)	/* make new fields up to at least $n */
{
	int nf = 2 * nfields;
	size_t s;

	if (n > nf)
		nf = n;
	s = (nf+1) * (sizeof (struct Cell *));  /* freebsd: how much do we need? */
	if (s / sizeof(struct Cell *) - 1 == (size_t)nf) /* didn't overflow */
		fldtab = (Cell **) realloc(fldtab, s);
	else					/* overflow sizeof int */
		xfree(fldtab);	/* make it null */
	if (fldtab == NULL)
		FATAL("out of space creating %d fields", nf);
	makefields(nfields+1, nf);
	nfields = nf;
}

int refldbld(const char *rec, const char *fs)	/* build fields from reg expr in FS */
{
	/* this relies on having fields[] the same length as $0 */
	/* the fields are all stored in this one array with \0's */
	char *fr;
	int i, tempstat, n;
	fa *pfa;

	n = strlen(rec);
	if (n > fieldssize) {
		xfree(fields);
		if ((fields = (char *) malloc(n+1)) == NULL)
			FATAL("out of space for fields in refldbld %d", n);
		fieldssize = n;
	}
	fr = fields;
	*fr = '\0';
	if (*rec == '\0')
		return 0;
	pfa = makedfa(fs, 1);
	DPRINTF("into refldbld, rec = <%s>, pat = <%s>\n", rec, fs);
	tempstat = pfa->initstat;
	for (i = 1; ; i++) {
		if (i > nfields)
			growfldtab(i);
		if (freeable(fldtab[i]))
			xfree(fldtab[i]->sval);
		fldtab[i]->tval = FLD | STR | DONTFREE;
		fldtab[i]->sval = fr;
		DPRINTF("refldbld: i=%d\n", i);
		if (nematch(pfa, rec)) {
			pfa->initstat = 2;	/* horrible coupling to b.c */
			DPRINTF("match %s (%d chars)\n", patbeg, patlen);
			strncpy(fr, rec, patbeg-rec);
			fr += patbeg - rec + 1;
			*(fr-1) = '\0';
			rec = patbeg + patlen;
		} else {
			DPRINTF("no match %s\n", rec);
			strcpy(fr, rec);
			pfa->initstat = tempstat;
			break;
		}
	}
	return i;
}

void recbld(void)	/* create $0 from $1..$NF if necessary */
{
	int i;
	char *r, *p;
	char *sep = getsval(ofsloc);

	if (donerec)
		return;
	r = record;
	for (i = 1; i <= *NF; i++) {
		p = getsval(fldtab[i]);
		if (!adjbuf(&record, &recsize, 1+strlen(p)+r-record, recsize, &r, "recbld 1"))
			FATAL("created $0 `%.30s...' too long", record);
		while ((*r = *p++) != 0)
			r++;
		if (i < *NF) {
			if (!adjbuf(&record, &recsize, 2+strlen(sep)+r-record, recsize, &r, "recbld 2"))
				FATAL("created $0 `%.30s...' too long", record);
			for (p = sep; (*r = *p++) != 0; )
				r++;
		}
	}
	if (!adjbuf(&record, &recsize, 2+r-record, recsize, &r, "recbld 3"))
		FATAL("built giant record `%.30s...'", record);
	*r = '\0';
	DPRINTF("in recbld inputFS=%s, fldtab[0]=%p\n", inputFS, (void*)fldtab[0]);

	if (freeable(fldtab[0]))
		xfree(fldtab[0]->sval);
	fldtab[0]->tval = REC | STR | DONTFREE;
	fldtab[0]->sval = record;

	DPRINTF("in recbld inputFS=%s, fldtab[0]=%p\n", inputFS, (void*)fldtab[0]);
	DPRINTF("recbld = |%s|\n", record);
	donerec = true;
}

int	errorflag	= 0;

void yyerror(const char *s)
{
	SYNTAX("%s", s);
}

void SYNTAX(const char *fmt, ...)
{
	extern char *cmdname, *curfname;
	static int been_here = 0;
	va_list varg;

	if (been_here++ > 2)
		return;
	fprintf(stderr, "%s: ", cmdname);
	va_start(varg, fmt);
	vfprintf(stderr, fmt, varg);
	va_end(varg);
	fprintf(stderr, " at source line %d", lineno);
	if (curfname != NULL)
		fprintf(stderr, " in function %s", curfname);
	if (compile_time == COMPILING && cursource() != NULL)
		fprintf(stderr, " source file %s", cursource());
	fprintf(stderr, "\n");
	errorflag = 2;
	eprint();
}

extern int bracecnt, brackcnt, parencnt;

void bracecheck(void)
{
	int c;
	static int beenhere = 0;

	if (beenhere++)
		return;
	while ((c = input()) != EOF && c != '\0')
		bclass(c);
	bcheck2(bracecnt, '{', '}');
	bcheck2(brackcnt, '[', ']');
	bcheck2(parencnt, '(', ')');
}

void bcheck2(int n, int c1, int c2)
{
	if (n == 1)
		fprintf(stderr, "\tmissing %c\n", c2);
	else if (n > 1)
		fprintf(stderr, "\t%d missing %c's\n", n, c2);
	else if (n == -1)
		fprintf(stderr, "\textra %c\n", c2);
	else if (n < -1)
		fprintf(stderr, "\t%d extra %c's\n", -n, c2);
}

void FATAL(const char *fmt, ...)
{
	extern char *cmdname;
	va_list varg;

	fflush(stdout);
	fprintf(stderr, "%s: ", cmdname);
	va_start(varg, fmt);
	vfprintf(stderr, fmt, varg);
	va_end(varg);
	error();
	if (dbg > 1)		/* core dump if serious debugging on */
		abort();
	exit(2);
}

void WARNING(const char *fmt, ...)
{
	extern char *cmdname;
	va_list varg;

	fflush(stdout);
	fprintf(stderr, "%s: ", cmdname);
	va_start(varg, fmt);
	vfprintf(stderr, fmt, varg);
	va_end(varg);
	error();
}

void error()
{
	extern Node *curnode;

	fprintf(stderr, "\n");
	if (compile_time != ERROR_PRINTING) {
		if (NR && *NR > 0) {
			fprintf(stderr, " input record number %d", (int) (*FNR));
			if (strcmp(*FILENAME, "-") != 0)
				fprintf(stderr, ", file %s", *FILENAME);
			fprintf(stderr, "\n");
		}
		if (curnode)
			fprintf(stderr, " source line number %d", curnode->lineno);
		else if (lineno)
			fprintf(stderr, " source line number %d", lineno);
		if (compile_time == COMPILING && cursource() != NULL)
			fprintf(stderr, " source file %s", cursource());
		fprintf(stderr, "\n");
		eprint();
	}
}

void eprint(void)	/* try to print context around error */
{
	char *p, *q;
	int c;
	static int been_here = 0;
	extern char ebuf[], *ep;

	if (compile_time != COMPILING || been_here++ > 0 || ebuf == ep)
		return;
	if (ebuf == ep)
		return;
	p = ep - 1;
	if (p > ebuf && *p == '\n')
		p--;
	for ( ; p > ebuf && *p != '\n' && *p != '\0'; p--)
		;
	while (*p == '\n')
		p++;
	fprintf(stderr, " context is\n\t");
	for (q=ep-1; q>=p && *q!=' ' && *q!='\t' && *q!='\n'; q--)
		;
	for ( ; p < q; p++)
		if (*p)
			putc(*p, stderr);
	fprintf(stderr, " >>> ");
	for ( ; p < ep; p++)
		if (*p)
			putc(*p, stderr);
	fprintf(stderr, " <<< ");
	if (*ep)
		while ((c = input()) != '\n' && c != '\0' && c != EOF) {
			putc(c, stderr);
			bclass(c);
		}
	putc('\n', stderr);
	ep = ebuf;
}

void bclass(int c)
{
	switch (c) {
	case '{': bracecnt++; break;
	case '}': bracecnt--; break;
	case '[': brackcnt++; break;
	case ']': brackcnt--; break;
	case '(': parencnt++; break;
	case ')': parencnt--; break;
	}
}

double errcheck(double x, const char *s)
{

	if (errno == EDOM) {
		errno = 0;
		WARNING("%s argument out of domain", s);
		x = 1;
	} else if (errno == ERANGE) {
		errno = 0;
		WARNING("%s result out of range", s);
		x = 1;
	}
	return x;
}

int isclvar(const char *s)	/* is s of form var=something ? */
{
	const char *os = s;

	if (!isalpha((int) *s) && *s != '_')
		return 0;
	for ( ; *s; s++)
		if (!(isalnum((int) *s) || *s == '_'))
			break;
	return *s == '=' && s > os;
}

/* strtod is supposed to be a proper test of what's a valid number */
/* appears to be broken in gcc on linux: thinks 0x123 is a valid FP number */
/* wrong: violates 4.10.1.4 of ansi C standard */

/* well, not quite. As of C99, hex floating point is allowed. so this is
 * a bit of a mess. We work around the mess by checking for a hexadecimal
 * value and disallowing it. Similarly, we now follow gawk and allow only
 * +nan, -nan, +inf, and -inf for NaN and infinity values.
 */

/*
 * This routine now has a more complicated interface, the main point
 * being to avoid the double conversion of a string to double, and
 * also to convey out, if requested, the information that the numeric
 * value was a leading string or is all of the string. The latter bit
 * is used in getfval().
 */

bool is_valid_number(const char *s, bool trailing_stuff_ok,
			bool *no_trailing, double *result)
{
	double r;
	char *ep;
	bool retval = false;
	bool is_nan = false;
	bool is_inf = false;

	if (no_trailing)
		*no_trailing = false;

	while (isspace((int) *s))
		s++;

	/* no hex floating point, sorry */
	if (s[0] == '0' && tolower(s[1]) == 'x')
		return false;

	/* allow +nan, -nan, +inf, -inf, any other letter, no */
	if (s[0] == '+' || s[0] == '-') {
		is_nan = (strncasecmp(s+1, "nan", 3) == 0);
		is_inf = (strncasecmp(s+1, "inf", 3) == 0);
		if ((is_nan || is_inf)
		    && (isspace((int) s[4]) || s[4] == '\0'))
			goto convert;
		else if (! isdigit(s[1]) && s[1] != '.')
			return false;
	}
	else if (! isdigit(s[0]) && s[0] != '.')
		return false;

convert:
	errno = 0;
	r = strtod(s, &ep);
	if (ep == s || errno == ERANGE)
		return false;

	if (isnan(r) && s[0] == '-' && signbit(r) == 0)
		r = -r;

	if (result != NULL)
		*result = r;

	/*
	 * check for trailing stuff
	 */
	while (isspace((int) *ep))
		ep++;

	if (no_trailing != NULL)
		*no_trailing = (*ep == '\0');

        /* return true if found the end, or trailing stuff is allowed */
	retval = *ep == '\0' || trailing_stuff_ok;

	return retval;
}
