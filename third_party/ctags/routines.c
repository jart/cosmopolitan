/*
 *   $Id: routines.c 536 2007-06-02 06:09:00Z elliotth $
 *
 *   Copyright (c) 2002-2003, Darren Hiebert
 *
 *   This source code is released for free distribution under the terms of the
 *   GNU General Public License.
 *
 *   This module contains a lose assortment of shared functions.
 */
#include "third_party/ctags/general.h"
/**/
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/s.h"
#include "third_party/ctags/config.h"
#include "third_party/ctags/debug.h"
#include "third_party/ctags/routines.h"

/*
 *  Miscellaneous macros
 */
#define selected(var,feature)	(((int)(var) & (int)(feature)) == (int)feature)

char *CurrentDirectory;

static const char *ExecutableProgram;
static const char *ExecutableName;

/*
*   FUNCTION DEFINITIONS
*/

extern void freeRoutineResources (void)
{
	if (CurrentDirectory != NULL)
		eFree (CurrentDirectory);
}

extern void setExecutableName (const char *const path)
{
	ExecutableProgram = path;
	ExecutableName = baseFilename (path);
#ifdef VAXC
{
	/* remove filetype from executable name */
	char *p = strrchr (ExecutableName, '.');
	if (p != NULL)
		*p = '\0';
}
#endif
}

extern const char *getExecutableName (void)
{
	return ExecutableName;
}

extern const char *getExecutablePath (void)
{
	return ExecutableProgram;
}

extern void error (
		const errorSelection selection, const char *const format, ...)
{
	va_list ap;

	va_start (ap, format);
	fprintf (errout, "%s: %s", getExecutableName (),
			selected (selection, WARNING) ? "Warning: " : "");
	vfprintf (errout, format, ap);
	if (selected (selection, PERROR))
		fprintf (errout, " : %s", strerror (errno));
	fputs ("\n", errout);
	va_end (ap);
	if (selected (selection, FATAL))
		exit (1);
}

/*
 *  Memory allocation functions
 */

extern void *eMalloc (const size_t size)
{
	void *buffer = malloc (size);

	if (buffer == NULL)
		error (FATAL, "out of memory");

	return buffer;
}

extern void *eCalloc (const size_t count, const size_t size)
{
	void *buffer = calloc (count, size);

	if (buffer == NULL)
		error (FATAL, "out of memory");

	return buffer;
}

extern void *eRealloc (void *const ptr, const size_t size)
{
	void *buffer;
	if (ptr == NULL)
		buffer = eMalloc (size);
	else
	{
		buffer = realloc (ptr, size);
		if (buffer == NULL)
			error (FATAL, "out of memory");
	}
	return buffer;
}

extern void eFree (void *const ptr)
{
	Assert (ptr != NULL);
	free (ptr);
}

/*
 *  String manipulation functions
 */

/*
 * Compare two strings, ignoring case.
 * Return 0 for match, < 0 for smaller, > 0 for bigger
 * Make sure case is folded to uppercase in comparison (like for 'sort -f')
 * This makes a difference when one of the chars lies between upper and lower
 * ie. one of the chars [ \ ] ^ _ ` for ascii. (The '_' in particular !)
 */
extern int struppercmp (const char *s1, const char *s2)
{
	int result;
	do
	{
		result = toupper ((int) *s1) - toupper ((int) *s2);
	} while (result == 0  &&  *s1++ != '\0'  &&  *s2++ != '\0');
	return result;
}

extern int strnuppercmp (const char *s1, const char *s2, size_t n)
{
	int result;
	do
	{
		result = toupper ((int) *s1) - toupper ((int) *s2);
	} while (result == 0  &&  --n > 0  &&  *s1++ != '\0'  &&  *s2++ != '\0');
	return result;
}

extern char* eStrdup (const char* str)
{
	char* result = xMalloc (strlen (str) + 1, char);
	strcpy (result, str);
	return result;
}

extern void toLowerString (char* str)
{
	while (*str != '\0')
	{
		*str = tolower ((int) *str);
		++str;
	}
}

extern void toUpperString (char* str)
{
	while (*str != '\0')
	{
		*str = toupper ((int) *str);
		++str;
	}
}

/*  Newly allocated string containing lower case conversion of a string.
 */
extern char* newLowerString (const char* str)
{
	char* const result = xMalloc (strlen (str) + 1, char);
	int i = 0;
	do
		result [i] = tolower ((int) str [i]);
	while (str [i++] != '\0');
	return result;
}

/*  Newly allocated string containing upper case conversion of a string.
 */
extern char* newUpperString (const char* str)
{
	char* const result = xMalloc (strlen (str) + 1, char);
	int i = 0;
	do
		result [i] = toupper ((int) str [i]);
	while (str [i++] != '\0');
	return result;
}

/*
 * File system functions
 */

extern void setCurrentDirectory (void)
{
	char* buf;
	if (CurrentDirectory == NULL)
		CurrentDirectory = xMalloc ((size_t) (PATH_MAX + 1), char);
	buf = getcwd (CurrentDirectory, PATH_MAX);
	if (buf == NULL)
		perror ("");
	if (CurrentDirectory [strlen (CurrentDirectory) - (size_t) 1] !=
			PATH_SEPARATOR)
	{
		sprintf (CurrentDirectory + strlen (CurrentDirectory), "%c",
				OUTPUT_PATH_SEPARATOR);
	}
}

/* For caching of stat() calls */
extern fileStatus *eStat (const char *const fileName)
{
	struct stat status;
	static fileStatus file;
	if (file.name == NULL  ||  strcmp (fileName, file.name) != 0)
	{
		eStatFree (&file);
		file.name = eStrdup (fileName);
		if (lstat (file.name, &status) != 0)
			file.exists = FALSE;
		else
		{
			file.isSymbolicLink = (boolean) S_ISLNK (status.st_mode);
			if (file.isSymbolicLink  &&  stat (file.name, &status) != 0)
				file.exists = FALSE;
			else
			{
				file.exists = TRUE;
				file.isDirectory = (boolean) S_ISDIR (status.st_mode);
				file.isNormalFile = (boolean) (S_ISREG (status.st_mode));
				file.isExecutable = (boolean) ((status.st_mode &
					(S_IXUSR | S_IXGRP | S_IXOTH)) != 0);
				file.isSetuid = (boolean) ((status.st_mode & S_ISUID) != 0);
				file.size = status.st_size;
			}
		}
	}
	return &file;
}

extern void eStatFree (fileStatus *status)
{
	if (status->name != NULL)
	{
		eFree (status->name);
		status->name = NULL;
	}
}

extern boolean doesFileExist (const char *const fileName)
{
	fileStatus *status = eStat (fileName);
	return status->exists;
}

extern boolean isRecursiveLink (const char* const dirName)
{
	boolean result = FALSE;
	fileStatus *status = eStat (dirName);
	if (status->isSymbolicLink)
	{
		char* const path = absoluteFilename (dirName);
		while (path [strlen (path) - 1] == PATH_SEPARATOR)
			path [strlen (path) - 1] = '\0';
		while (! result  &&  strlen (path) > (size_t) 1)
		{
			char *const separator = strrchr (path, PATH_SEPARATOR);
			if (separator == NULL)
				break;
			else if (separator == path)  /* backed up to root directory */
				*(separator + 1) = '\0';
			else
				*separator = '\0';
			result = isSameFile (path, dirName);
		}
		eFree (path);
	}
	return result;
}

/*
 *  Pathname manipulation (O/S dependent!!!)
 */

static boolean isPathSeparator (const int c)
{
	boolean result;
	result = (boolean) (c == PATH_SEPARATOR);
	return result;
}

extern boolean isSameFile (const char *const name1, const char *const name2)
{
	boolean result = FALSE;
	struct stat stat1, stat2;
	if (stat (name1, &stat1) == 0  &&  stat (name2, &stat2) == 0)
		result = (boolean) (stat1.st_ino == stat2.st_ino);
	return result;
}

extern const char *baseFilename (const char *const filePath)
{
	const char *tail = strrchr (filePath, PATH_SEPARATOR);
	if (tail == NULL)
		tail = filePath;
	else
		++tail;  /* step past last delimiter */

	return tail;
}

extern const char *fileExtension (const char *const fileName)
{
	const char *extension;
	const char *pDelimiter = NULL;
	const char *const base = baseFilename (fileName);
	if (pDelimiter == NULL)
	    pDelimiter = strrchr (base, '.');

	if (pDelimiter == NULL)
		extension = "";
	else
		extension = pDelimiter + 1;  /* skip to first char of extension */

	return extension;
}

extern boolean isAbsolutePath (const char *const path)
{
	boolean result = FALSE;
	result = isPathSeparator (path [0]);
	return result;
}

extern vString *combinePathAndFile (
	const char *const path, const char *const file)
{
	vString *const filePath = vStringNew ();
	const int lastChar = path [strlen (path) - 1];
	boolean terminated = isPathSeparator (lastChar);

	vStringCopyS (filePath, path);
	if (! terminated)
	{
		vStringPut (filePath, OUTPUT_PATH_SEPARATOR);
		vStringTerminate (filePath);
	}
	vStringCatS (filePath, file);

	return filePath;
}

/* Return a newly-allocated string whose contents concatenate those of
 * s1, s2, s3.
 * Routine adapted from Gnu etags.
 */
static char* concat (const char *s1, const char *s2, const char *s3)
{
  int len1 = strlen (s1), len2 = strlen (s2), len3 = strlen (s3);
  char *result = xMalloc (len1 + len2 + len3 + 1, char);

  strcpy (result, s1);
  strcpy (result + len1, s2);
  strcpy (result + len1 + len2, s3);
  result [len1 + len2 + len3] = '\0';

  return result;
}

/* Return a newly allocated string containing the absolute file name of FILE
 * given CWD (which should end with a slash).
 * Routine adapted from Gnu etags.
 */
extern char* absoluteFilename (const char *file)
{
	char *slashp, *cp;
	char *res = NULL;
	if (isAbsolutePath (file))
	{
		res = eStrdup (file);
	}
	else
		res = concat (CurrentDirectory, file, "");

	/* Delete the "/dirname/.." and "/." substrings. */
	slashp = strchr (res, PATH_SEPARATOR);
	while (slashp != NULL  &&  slashp [0] != '\0')
	{
		if (slashp[1] == '.')
		{
			if (slashp [2] == '.' &&
				(slashp [3] == PATH_SEPARATOR || slashp [3] == '\0'))
			{
				cp = slashp;
				do
					cp--;
				while (cp >= res  &&  ! isAbsolutePath (cp));
				if (cp < res)
					cp = slashp;/* the absolute name begins with "/.." */
				memmove (cp, slashp + 3, strlen(slashp + 3) + 1);
				slashp = cp;
				continue;
			}
			else if (slashp [2] == PATH_SEPARATOR  ||  slashp [2] == '\0')
			{
				memmove (slashp, slashp + 2, strlen(slashp + 2) + 1);
				continue;
			}
		}
		slashp = strchr (slashp + 1, PATH_SEPARATOR);
	}

	if (res [0] == '\0')
		return eStrdup ("/");
	else
	{
		return res;
	}
}

/* Return a newly allocated string containing the absolute file name of dir
 * where `file' resides given `CurrentDirectory'.
 * Routine adapted from Gnu etags.
 */
extern char* absoluteDirname (char *file)
{
	char *slashp, *res;
	char save;
	slashp = strrchr (file, PATH_SEPARATOR);
	if (slashp == NULL)
		res = eStrdup (CurrentDirectory);
	else
	{
		save = slashp [1];
		slashp [1] = '\0';
		res = absoluteFilename (file);
		slashp [1] = save;
	}
	return res;
}

/* Return a newly allocated string containing the file name of FILE relative
 * to the absolute directory DIR (which should end with a slash).
 * Routine adapted from Gnu etags.
 */
extern char* relativeFilename (const char *file, const char *dir)
{
	const char *fp, *dp;
	char *absdir, *res;
	int i;

	/* Find the common root of file and dir (with a trailing slash). */
	absdir = absoluteFilename (file);
	fp = absdir;
	dp = dir;
	while (*fp++ == *dp++)
		continue;
	fp--;
	dp--;  /* back to the first differing char */
	do
	{  /* look at the equal chars until path sep */
		if (fp == absdir)
			return absdir;  /* first char differs, give up */
		fp--;
		dp--;
	} while (*fp != PATH_SEPARATOR);

	/* Build a sequence of "../" strings for the resulting relative file name.
	 */
	i = 0;
	while ((dp = strchr (dp + 1, PATH_SEPARATOR)) != NULL)
		i += 1;
	res = xMalloc (3 * i + strlen (fp + 1) + 1, char);
	res [0] = '\0';
	while (i-- > 0)
		strcat (res, "../");

	/* Add the file name relative to the common root of file and dir. */
	strcat (res, fp + 1);
	free (absdir);

	return res;
}

extern FILE *tempFile (const char *const mode, char **const pName)
{
	char *name;
	FILE *fp;
	int fd;
	const char *const pattern = "tags.XXXXXX";
	const char *tmpdir = NULL;
	fileStatus *file = eStat (ExecutableProgram);
	if (! file->isSetuid)
		tmpdir = getenv ("TMPDIR");
	if (tmpdir == NULL)
		tmpdir = TMPDIR;
	name = xMalloc (strlen (tmpdir) + 1 + strlen (pattern) + 1, char);
	sprintf (name, "%s%c%s", tmpdir, OUTPUT_PATH_SEPARATOR, pattern);
	fd = mkstemp (name);
	eStatFree (file);
	if (fd == -1)
		error (FATAL | PERROR, "cannot open temporary file");
	fp = fdopen (fd, mode);
	if (fp == NULL)
		error (FATAL | PERROR, "cannot open temporary file");
	DebugStatement (
		debugPrintf (DEBUG_STATUS, "opened temporary file %s\n", name); )
	Assert (*pName == NULL);
	*pName = name;
	return fp;
}

/* vi:set tabstop=4 shiftwidth=4: */
