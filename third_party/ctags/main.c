/*
 *   $Id: main.c 536 2007-06-02 06:09:00Z elliotth $
 *
 *   Copyright (c) 1996-2003, Darren Hiebert
 *
 *   Author: Darren Hiebert <dhiebert@users.sourceforge.net>
 *           http://ctags.sourceforge.net
 *
 *   This source code is released for free distribution under the terms of the
 *   GNU General Public License. It is provided on an as-is basis and no
 *   responsibility is accepted for its failure to perform as expected.
 *
 *   This is a reimplementation of the ctags (1) program. It is an attempt to
 *   provide a fully featured ctags program which is free of the limitations
 *   which most (all?) others are subject to.
 *
 *   This module contains the start-up code and routines to determine the list
 *   of files to parsed for tags.
 */
#include "libc/runtime/runtime.h"
#include "third_party/ctags/general.h" /* must always come first */
/**/
#include "libc/calls/struct/dirent.h"
#include "libc/calls/weirdtypes.h"
#include "libc/mem/alg.h"
#include "libc/str/str.h"
#include "libc/time/time.h"
#include "third_party/ctags/debug.h"
#include "third_party/ctags/keyword.h"
#include "third_party/ctags/main.h"
#include "third_party/ctags/options.h"
#include "third_party/ctags/read.h"
#include "third_party/ctags/routines.h"

/*
*   MACROS
*/
#define plural(value)  (((unsigned long)(value) == 1L) ? "" : "s")

/*
*   DATA DEFINITIONS
*/
static struct { long files, lines, bytes; } Totals = { 0, 0, 0 };

/*
*   FUNCTION PROTOTYPES
*/
static boolean createTagsForEntry (const char *const entryName);

/*
*   FUNCTION DEFINITIONS
*/

extern void addTotals (
		const unsigned int files, const long unsigned int lines,
		const long unsigned int bytes)
{
	Totals.files += files;
	Totals.lines += lines;
	Totals.bytes += bytes;
}

extern boolean isDestinationStdout (void)
{
	boolean toStdout = FALSE;

	if (Option.xref  ||  Option.filter  ||
		(Option.tagFileName != NULL  &&  (strcmp (Option.tagFileName, "-") == 0
                                                  || strcmp (Option.tagFileName, "/dev/stdout") == 0
		)))
		toStdout = TRUE;
	return toStdout;
}

static boolean recurseUsingOpendir (const char *const dirName)
{
	boolean resize = FALSE;
	DIR *const dir = opendir (dirName);
	if (dir == NULL)
		error (WARNING | PERROR, "cannot recurse into directory \"%s\"", dirName);
	else
	{
		struct dirent *entry;
		while ((entry = readdir (dir)) != NULL)
		{
			if (strcmp (entry->d_name, ".") != 0  &&
				strcmp (entry->d_name, "..") != 0)
			{
				vString *filePath;
				if (strcmp (dirName, ".") == 0)
					filePath = vStringNewInit (entry->d_name);
				else
					filePath = combinePathAndFile (dirName, entry->d_name);
				resize |= createTagsForEntry (vStringValue (filePath));
				vStringDelete (filePath);
			}
		}
		closedir (dir);
	}
	return resize;
}


static boolean recurseIntoDirectory (const char *const dirName)
{
	boolean resize = FALSE;
	if (isRecursiveLink (dirName))
		verbose ("ignoring \"%s\" (recursive link)\n", dirName);
	else if (! Option.recurse)
		verbose ("ignoring \"%s\" (directory)\n", dirName);
	else
	{
		verbose ("RECURSING into directory \"%s\"\n", dirName);
		resize = recurseUsingOpendir (dirName);
	}
	return resize;
}

static boolean createTagsForEntry (const char *const entryName)
{
	boolean resize = FALSE;
	fileStatus *status = eStat (entryName);

	Assert (entryName != NULL);
	if (isExcludedFile (entryName))
		verbose ("excluding \"%s\"\n", entryName);
	else if (status->isSymbolicLink  &&  ! Option.followLinks)
		verbose ("ignoring \"%s\" (symbolic link)\n", entryName);
	else if (! status->exists)
		error (WARNING | PERROR, "cannot open source file \"%s\"", entryName);
	else if (status->isDirectory)
		resize = recurseIntoDirectory (entryName);
	else if (! status->isNormalFile)
		verbose ("ignoring \"%s\" (special file)\n", entryName);
	else
		resize = parseFile (entryName);

	eStatFree (status);
	return resize;
}

#ifdef MANUAL_GLOBBING

static boolean createTagsForWildcardArg (const char *const arg)
{
	boolean resize = FALSE;
	vString *const pattern = vStringNewInit (arg);
	char *patternS = vStringValue (pattern);

#if defined (HAVE_FINDFIRST) || defined (HAVE__FINDFIRST)
	/*  We must transform the "." and ".." forms into something that can
	 *  be expanded by the findfirst/_findfirst functions.
	 */
	if (Option.recurse  &&
		(strcmp (patternS, ".") == 0  ||  strcmp (patternS, "..") == 0))
	{
		vStringPut (pattern, OUTPUT_PATH_SEPARATOR);
		vStringCatS (pattern, "*.*");
	}
	resize |= createTagsForWildcardUsingFindfirst (patternS);
#endif
	vStringDelete (pattern);
	return resize;
}

#endif

static boolean createTagsForArgs (cookedArgs *const args)
{
	boolean resize = FALSE;

	/*  Generate tags for each argument on the command line.
	 */
	while (! cArgOff (args))
	{
		const char *const arg = cArgItem (args);

#ifdef MANUAL_GLOBBING
		resize |= createTagsForWildcardArg (arg);
#else
		resize |= createTagsForEntry (arg);
#endif
		cArgForth (args);
		parseOptions (args);
	}
	return resize;
}

/*  Read from an opened file a list of file names for which to generate tags.
 */
static boolean createTagsFromFileInput (FILE *const fp, const boolean filter)
{
	boolean resize = FALSE;
	if (fp != NULL)
	{
		cookedArgs *args = cArgNewFromLineFile (fp);
		parseOptions (args);
		while (! cArgOff (args))
		{
			resize |= createTagsForEntry (cArgItem (args));
			if (filter)
			{
				if (Option.filterTerminator != NULL)
					fputs (Option.filterTerminator, stdout);
				fflush (stdout);
			}
			cArgForth (args);
			parseOptions (args);
		}
		cArgDelete (args);
	}
	return resize;
}

/*  Read from a named file a list of file names for which to generate tags.
 */
static boolean createTagsFromListFile (const char *const fileName)
{
	boolean resize;
	Assert (fileName != NULL);
	if (strcmp (fileName, "-") == 0)
		resize = createTagsFromFileInput (stdin, FALSE);
	else
	{
		FILE *const fp = fopen (fileName, "r");
		if (fp == NULL)
			error (FATAL | PERROR, "cannot open list file \"%s\"", fileName);
		resize = createTagsFromFileInput (fp, FALSE);
		fclose (fp);
	}
	return resize;
}

#if defined (HAVE_CLOCK)
# define CLOCK_AVAILABLE
#else
#error wut
#endif

static void printTotals (const clock_t *const timeStamps)
{
	const unsigned long totalTags = TagFile.numTags.added +
									TagFile.numTags.prev;

	fprintf (errout, "%ld file%s, %ld line%s (%ld kB) scanned",
			Totals.files, plural (Totals.files),
			Totals.lines, plural (Totals.lines),
			Totals.bytes/1024L);
#ifdef CLOCK_AVAILABLE
	{
		const double interval = ((double) (timeStamps [1] - timeStamps [0])) /
								CLOCKS_PER_SEC;

		fprintf (errout, " in %.01f seconds", interval);
		if (interval != (double) 0.0)
			fprintf (errout, " (%lu kB/s)",
					(unsigned long) (Totals.bytes / interval) / 1024L);
	}
#endif
	fputc ('\n', errout);

	fprintf (errout, "%lu tag%s added to tag file",
			TagFile.numTags.added, plural (TagFile.numTags.added));
	if (Option.append)
		fprintf (errout, " (now %lu tags)", totalTags);
	fputc ('\n', errout);

	if (totalTags > 0  &&  Option.sorted != SO_UNSORTED)
	{
		fprintf (errout, "%lu tag%s sorted", totalTags, plural (totalTags));
#ifdef CLOCK_AVAILABLE
		fprintf (errout, " in %.02f seconds",
				((double) (timeStamps [2] - timeStamps [1])) / CLOCKS_PER_SEC);
#endif
		fputc ('\n', errout);
	}

#ifdef DEBUG
	fprintf (errout, "longest tag line = %lu\n",
			(unsigned long) TagFile.max.line);
#endif
}

static boolean etagsInclude (void)
{
	return (boolean)(Option.etags && Option.etagsInclude != NULL);
}

static void makeTags (cookedArgs *args)
{
	clock_t timeStamps [3];
	boolean resize = FALSE;
	boolean files = (boolean)(! cArgOff (args) || Option.fileList != NULL
							  || Option.filter);

	if (! files)
	{
		if (filesRequired ())
			error (FATAL, "No files specified. Try \"%s --help\".",
				getExecutableName ());
		else if (! Option.recurse && ! etagsInclude ())
			return;
	}

#define timeStamp(n) timeStamps[(n)]=(Option.printTotals ? clock():(clock_t)0)
	if (! Option.filter)
		openTagFile ();

	timeStamp (0);

	if (! cArgOff (args))
	{
		verbose ("Reading command line arguments\n");
		resize = createTagsForArgs (args);
	}
	if (Option.fileList != NULL)
	{
		verbose ("Reading list file\n");
		resize = (boolean) (createTagsFromListFile (Option.fileList) || resize);
	}
	if (Option.filter)
	{
		verbose ("Reading filter input\n");
		resize = (boolean) (createTagsFromFileInput (stdin, TRUE) || resize);
	}
	if (! files  &&  Option.recurse)
		resize = recurseIntoDirectory (".");

	timeStamp (1);

	if (! Option.filter)
		closeTagFile (resize);

	timeStamp (2);

	if (Option.printTotals)
		printTotals (timeStamps);
#undef timeStamp
}

/*
 *		Start up code
 */

extern int main (int __unused argc, char **argv)
{
	cookedArgs *args;

	setCurrentDirectory ();
	setExecutableName (*argv++);
	checkRegex ();

	args = cArgNewFromArgv (argv);
	previewFirstOption (args);
	testEtagsInvocation ();
	initializeParsing ();
	initOptions ();
	readOptionConfiguration ();
	verbose ("Reading initial options from command line\n");
	parseOptions (args);
	checkOptions ();
	makeTags (args);

	/*  Clean up.
	 */
	cArgDelete (args);
	freeKeywordTable ();
	freeRoutineResources ();
	freeSourceFileResources ();
	freeTagFileResources ();
	freeOptionResources ();
	freeParserResources ();
	freeRegexResources ();

	exit (0);
	return 0;
}

/* vi:set tabstop=4 shiftwidth=4: */
