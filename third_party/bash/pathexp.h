/* pathexp.h -- The shell interface to the globbing library. */

/* Copyright (C) 1987-2020 Free Software Foundation, Inc.

   This file is part of GNU Bash, the Bourne Again SHell.

   Bash is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Bash is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Bash.  If not, see <http://www.gnu.org/licenses/>.
*/

#if !defined (_PATHEXP_H_)
#define _PATHEXP_H_

#define GLOB_FAILED(glist)	(glist) == (char **)&glob_error_return

extern int noglob_dot_filenames;
extern char *glob_error_return;

/* Flag values for quote_string_for_globbing */
#define QGLOB_CVTNULL	0x01	/* convert QUOTED_NULL strings to '\0' */
#define QGLOB_FILENAME	0x02	/* do correct quoting for matching filenames */
#define QGLOB_REGEXP	0x04	/* quote an ERE for regcomp/regexec */
#define QGLOB_CTLESC	0x08	/* turn CTLESC CTLESC into CTLESC for BREs */
#define QGLOB_DEQUOTE	0x10	/* like dequote_string but quote glob chars */

#if defined (EXTENDED_GLOB)
/* Flags to OR with other flag args to strmatch() to enabled the extended
   pattern matching. */
#  define FNMATCH_EXTFLAG	(extended_glob ? FNM_EXTMATCH : 0)
#else
#  define FNMATCH_EXTFLAG	0
#endif /* !EXTENDED_GLOB */

#define FNMATCH_IGNCASE		(match_ignore_case ? FNM_CASEFOLD : 0)
#define FNMATCH_NOCASEGLOB	(glob_ignore_case ? FNM_CASEFOLD : 0)

extern int glob_dot_filenames;
extern int extended_glob;
extern int glob_star;
extern int match_ignore_case;	/* doesn't really belong here */

extern int unquoted_glob_pattern_p PARAMS((char *));

/* PATHNAME can contain characters prefixed by CTLESC; this indicates
   that the character is to be quoted.  We quote it here in the style
   that the glob library recognizes.  If flags includes QGLOB_CVTNULL,
   we change quoted null strings (pathname[0] == CTLNUL) into empty
   strings (pathname[0] == 0).  If this is called after quote removal
   is performed, (flags & QGLOB_CVTNULL) should be 0; if called when quote
   removal has not been done (for example, before attempting to match a
   pattern while executing a case statement), flags should include
   QGLOB_CVTNULL.  If flags includes QGLOB_FILENAME, appropriate quoting
   to match a filename should be performed. */
extern char *quote_string_for_globbing PARAMS((const char *, int));

extern int glob_char_p PARAMS((const char *));
extern char *quote_globbing_chars PARAMS((const char *));

/* Call the glob library to do globbing on PATHNAME. FLAGS is additional
   flags to pass to QUOTE_STRING_FOR_GLOBBING, mostly having to do with
   whether or not we've already performed quote removal. */
extern char **shell_glob_filename PARAMS((const char *, int));

/* Filename completion ignore.  Used to implement the "fignore" facility of
   tcsh, GLOBIGNORE (like ksh-93 FIGNORE), and EXECIGNORE.

   It is passed a NULL-terminated array of (char *)'s that must be
   free()'d if they are deleted.  The first element (names[0]) is the
   least-common-denominator string of the matching patterns (i.e.
   u<TAB> produces names[0] = "und", names[1] = "under.c", names[2] =
   "undun.c", name[3] = NULL).  */

struct ign {
  char *val;
  int len, flags;
};

typedef int sh_iv_item_func_t PARAMS((struct ign *));

struct ignorevar {
  char *varname;	/* FIGNORE, GLOBIGNORE, or EXECIGNORE */
  struct ign *ignores;	/* Store the ignore strings here */
  int num_ignores;	/* How many are there? */
  char *last_ignoreval;	/* Last value of variable - cached for speed */
  sh_iv_item_func_t *item_func; /* Called when each item is parsed from $`varname' */
};

extern void setup_ignore_patterns PARAMS((struct ignorevar *));

extern void setup_glob_ignore PARAMS((char *));
extern int should_ignore_glob_matches PARAMS((void));
extern void ignore_glob_matches PARAMS((char **));

#endif
