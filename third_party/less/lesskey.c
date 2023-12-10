/*
 * Copyright (C) 1984-2023  Mark Nudelman
 *
 * You may distribute under the terms of either the GNU General Public
 * License or the Less License, as specified in the README file.
 *
 * For more information, see the README file.
 */


/*
 *  lesskey [-o output] [input]
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 *  Make a .less file.
 *  If no input file is specified, standard input is used.
 *  If no output file is specified, $HOME/.less is used.
 *
 *  The .less file is used to specify (to "less") user-defined
 *  key bindings.  Basically any sequence of 1 to MAX_CMDLEN
 *  keystrokes may be bound to an existing less function.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 *  The input file is an ascii file consisting of a 
 *  sequence of lines of the form:
 *          string <whitespace> action [chars] <newline>
 *
 *      "string" is a sequence of command characters which form
 *              the new user-defined command.  The command
 *              characters may be:
 *              1. The actual character itself.
 *              2. A character preceded by ^ to specify a
 *                 control character (e.g. ^X means control-X).
 *              3. A backslash followed by one to three octal digits
 *                 to specify a character by its octal value.
 *              4. A backslash followed by b, e, n, r or t
 *                 to specify \b, ESC, \n, \r or \t, respectively.
 *              5. Any character (other than those mentioned above) preceded 
 *                 by a \ to specify the character itself (characters which
 *                 must be preceded by \ include ^, \, and whitespace.
 *      "action" is the name of a "less" action, from the table below.
 *      "chars" is an optional sequence of characters which is treated
 *              as keyboard input after the command is executed.
 *
 *      Blank lines and lines which start with # are ignored, 
 *      except for the special control lines:
 *              #command        Signals the beginning of the command
 *                              keys section.
 *              #line-edit      Signals the beginning of the line-editing
 *                              keys section.
 *              #env            Signals the beginning of the environment
 *                              variable section.
 *              #stop           Stops command parsing in less;
 *                              causes all default keys to be disabled.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 *      The output file is a non-ascii file, consisting of a header,
 *      one or more sections, and a trailer.
 *      Each section begins with a section header, a section length word
 *      and the section data.  Normally there are three sections:
 *              CMD_SECTION     Definition of command keys.
 *              EDIT_SECTION    Definition of editing keys.
 *              END_SECTION     A special section header, with no 
 *                              length word or section data.
 *
 *      Section data consists of zero or more byte sequences of the form:
 *              string <0> <action>
 *      or
 *              string <0> <action|A_EXTRA> chars <0>
 *
 *      "string" is the command string.
 *      "<0>" is one null byte.
 *      "<action>" is one byte containing the action code (the A_xxx value).
 *      If action is ORed with A_EXTRA, the action byte is followed
 *              by the null-terminated "chars" string.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 */

#include "defines.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lesskey.h"
#include "cmd.h"

char fileheader[] = {
	C0_LESSKEY_MAGIC, 
	C1_LESSKEY_MAGIC, 
	C2_LESSKEY_MAGIC, 
	C3_LESSKEY_MAGIC
};
char filetrailer[] = {
	C0_END_LESSKEY_MAGIC, 
	C1_END_LESSKEY_MAGIC, 
	C2_END_LESSKEY_MAGIC
};
char cmdsection[1] =    { CMD_SECTION };
char editsection[1] =   { EDIT_SECTION };
char varsection[1] =    { VAR_SECTION };
char endsection[1] =    { END_SECTION };

char *infile = NULL;
char *outfile = NULL ;

extern char version[];

static void usage(void)
{
	fprintf(stderr, "usage: lesskey [-o output] [input]\n");
	exit(1);
}

void lesskey_parse_error(char *s)
{
	fprintf(stderr, "%s\n", s);
}

int lstrtoi(char *buf, char **ebuf, int radix)
{
	return (int) strtol(buf, ebuf, radix);
}

void out_of_memory(void)
{
	fprintf(stderr, "lesskey: cannot allocate memory\n");
	exit(1);
}

void * ecalloc(int count, unsigned int size)
{
	void *p;

	p = calloc(count, size);
	if (p == NULL)
		out_of_memory();
	return (p);
}

static char * mkpathname(char *dirname, char *filename)
{
	char *pathname;

	pathname = ecalloc(strlen(dirname) + strlen(filename) + 2, sizeof(char));
	strcpy(pathname, dirname);
	strcat(pathname, PATHNAME_SEP);
	strcat(pathname, filename);
	return (pathname);
}

/*
 * Figure out the name of a default file (in the user's HOME directory).
 */
char * homefile(char *filename)
{
	char *p;
	char *pathname;

	if ((p = getenv("HOME")) != NULL && *p != '\0')
		pathname = mkpathname(p, filename);
#if OS2
	else if ((p = getenv("INIT")) != NULL && *p != '\0')
		pathname = mkpathname(p, filename);
#endif
	else
	{
		fprintf(stderr, "cannot find $HOME - using current directory\n");
		pathname = mkpathname(".", filename);
	}
	return (pathname);
}

/*
 * Parse command line arguments.
 */
static void parse_args(int argc, char **argv)
{
	char *arg;

	outfile = NULL;
	while (--argc > 0)
	{
		arg = *++argv;
		if (arg[0] != '-')
			/* Arg does not start with "-"; it's not an option. */
			break;
		if (arg[1] == '\0')
			/* "-" means standard input. */
			break;
		if (arg[1] == '-' && arg[2] == '\0')
		{
			/* "--" means end of options. */
			argc--;
			argv++;
			break;
		}
		switch (arg[1])
		{
		case '-':
			if (strncmp(arg, "--output", 8) == 0)
			{
				if (arg[8] == '\0')
					outfile = &arg[8];
				else if (arg[8] == '=')
					outfile = &arg[9];
				else
					usage();
				goto opt_o;
			}
			if (strcmp(arg, "--version") == 0)
			{
				goto opt_V;
			}
			usage();
			break;
		case 'o':
			outfile = &argv[0][2];
		opt_o:
			if (*outfile == '\0')
			{
				if (--argc <= 0)
					usage();
				outfile = *(++argv);
			}
			break;
		case 'V':
		opt_V:
			printf("lesskey  version %s\n", version);
			exit(0);
		default:
			usage();
		}
	}
	if (argc > 1)
		usage();
	/*
	 * Open the input file, or use DEF_LESSKEYINFILE if none specified.
	 */
	if (argc > 0)
		infile = *argv;
}

/*
 * Output some bytes.
 */
static void fputbytes(FILE *fd, char *buf, int len)
{
	while (len-- > 0)
	{
		fwrite(buf, sizeof(char), 1, fd);
		buf++;
	}
}

/*
 * Output an integer, in special KRADIX form.
 */
static void fputint(FILE *fd, unsigned int val)
{
	char c;

	if (val >= KRADIX*KRADIX)
	{
		fprintf(stderr, "error: cannot write %d, max %d\n", 
			val, KRADIX*KRADIX);
		exit(1);
	}
	c = val % KRADIX;
	fwrite(&c, sizeof(char), 1, fd);
	c = val / KRADIX;
	fwrite(&c, sizeof(char), 1, fd);
}

int main(int argc, char *argv[])
{
	struct lesskey_tables tables;
	FILE *out;
	int errors;

#ifdef WIN32
	if (getenv("HOME") == NULL)
	{
		/*
		 * If there is no HOME environment variable,
		 * try the concatenation of HOMEDRIVE + HOMEPATH.
		 */
		char *drive = getenv("HOMEDRIVE");
		char *path  = getenv("HOMEPATH");
		if (drive != NULL && path != NULL)
		{
			char *env = (char *) ecalloc(strlen(drive) + 
					strlen(path) + 6, sizeof(char));
			strcpy(env, "HOME=");
			strcat(env, drive);
			strcat(env, path);
			putenv(env);
		}
	}
#endif /* WIN32 */

	/*
	 * Process command line arguments.
	 */
	parse_args(argc, argv);
	errors = parse_lesskey(infile, &tables);
	if (errors)
	{
		fprintf(stderr, "%d errors; no output produced\n", errors);
		return (1);
	}

	fprintf(stderr, "NOTE: lesskey is deprecated.\n      It is no longer necessary to run lesskey,\n      when using less version 582 and later.\n");

	/*
	 * Write the output file.
	 * If no output file was specified, use "$HOME/.less"
	 */
	if (outfile == NULL)
		outfile = getenv("LESSKEY");
	if (outfile == NULL)
		outfile = homefile(LESSKEYFILE);
	if ((out = fopen(outfile, "wb")) == NULL)
	{
#if HAVE_PERROR
		perror(outfile);
#else
		fprintf(stderr, "Cannot open %s\n", outfile);
#endif
		return (1);
	}

	/* File header */
	fputbytes(out, fileheader, sizeof(fileheader));

	/* Command key section */
	fputbytes(out, cmdsection, sizeof(cmdsection));
	fputint(out, tables.cmdtable.buf.end);
	fputbytes(out, (char *)tables.cmdtable.buf.data, tables.cmdtable.buf.end);
	/* Edit key section */
	fputbytes(out, editsection, sizeof(editsection));
	fputint(out, tables.edittable.buf.end);
	fputbytes(out, (char *)tables.edittable.buf.data, tables.edittable.buf.end);

	/* Environment variable section */
	fputbytes(out, varsection, sizeof(varsection)); 
	fputint(out, tables.vartable.buf.end);
	fputbytes(out, (char *)tables.vartable.buf.data, tables.vartable.buf.end);

	/* File trailer */
	fputbytes(out, endsection, sizeof(endsection));
	fputbytes(out, filetrailer, sizeof(filetrailer));
	fclose(out);
	return (0);
}
