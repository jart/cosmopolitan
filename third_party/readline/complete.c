/* complete.c -- filename completion for readline. */

/* Copyright (C) 1987-2021 Free Software Foundation, Inc.

   This file is part of the GNU Readline Library (Readline), a library
   for reading lines of text with interactive input and history editing.

   Readline is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Readline is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Readline.  If not, see <http://www.gnu.org/licenses/>.
*/

#define READLINE_LIBRARY

#if defined (__TANDEM)
#  define _XOPEN_SOURCE_EXTENDED 1
#endif

#if defined (HAVE_CONFIG_H)
#  include "config.h"
#endif

#include <sys/types.h>
#if defined (__TANDEM)
#  include <sys/stat.h>
#endif
#include <fcntl.h>
#if defined (HAVE_SYS_FILE_H)
#  include <sys/file.h>
#endif

#include <signal.h>

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif /* HAVE_UNISTD_H */

#if defined (HAVE_STDLIB_H)
#  include <stdlib.h>
#else
#  include "ansi_stdlib.h"
#endif /* HAVE_STDLIB_H */

#include <stdio.h>

#include <errno.h>
#if !defined (errno)
extern int errno;
#endif /* !errno */

#if defined (HAVE_PWD_H)
#include <pwd.h>
#endif

#include "posixdir.h"
#include "posixstat.h"

/* System-specific feature definitions and include files. */
#include "rldefs.h"
#include "rlmbutil.h"

/* Some standard library routines. */
#include "readline.h"
#include "xmalloc.h"
#include "rlprivate.h"

#if defined (COLOR_SUPPORT)
#  include "colors.h"
#endif

#ifdef __STDC__
typedef int QSFUNC (const void *, const void *);
#else
typedef int QSFUNC ();
#endif

#ifdef HAVE_LSTAT
#  define LSTAT lstat
#else
#  define LSTAT stat
#endif

/* Unix version of a hidden file.  Could be different on other systems. */
#define HIDDEN_FILE(fname)	((fname)[0] == '.')

/* Most systems don't declare getpwent in <pwd.h> if _POSIX_SOURCE is
   defined. */
#if defined (HAVE_GETPWENT) && (!defined (HAVE_GETPW_DECLS) || defined (_POSIX_SOURCE))
extern struct passwd *getpwent (void);
#endif /* HAVE_GETPWENT && (!HAVE_GETPW_DECLS || _POSIX_SOURCE) */

/* If non-zero, then this is the address of a function to call when
   completing a word would normally display the list of possible matches.
   This function is called instead of actually doing the display.
   It takes three arguments: (char **matches, int num_matches, int max_length)
   where MATCHES is the array of strings that matched, NUM_MATCHES is the
   number of strings in that array, and MAX_LENGTH is the length of the
   longest string in that array. */
rl_compdisp_func_t *rl_completion_display_matches_hook = (rl_compdisp_func_t *)NULL;

#if defined (VISIBLE_STATS) || defined (COLOR_SUPPORT)
#  if !defined (X_OK)
#    define X_OK 1
#  endif
#endif

#if defined (VISIBLE_STATS)
static int stat_char (char *);
#endif

#if defined (COLOR_SUPPORT)
static int colored_stat_start (const char *);
static void colored_stat_end (void);
static int colored_prefix_start (void);
static void colored_prefix_end (void);
#endif

static int path_isdir (const char *);

static char *rl_quote_filename (char *, int, char *);

static void _rl_complete_sigcleanup (int, void *);

static void set_completion_defaults (int);
static int get_y_or_n (int);
static int _rl_internal_pager (int);
static char *printable_part (char *);
static int fnwidth (const char *);
static int fnprint (const char *, int, const char *);
static int print_filename (char *, char *, int);

static char **gen_completion_matches (char *, int, int, rl_compentry_func_t *, int, int);

static char **remove_duplicate_matches (char **);
static void insert_match (char *, int, int, char *);
static int append_to_match (char *, int, int, int);
static void insert_all_matches (char **, int, char *);
static int complete_fncmp (const char *, int, const char *, int);
static void display_matches (char **);
static int compute_lcd_of_matches (char **, int, const char *);
static int postprocess_matches (char ***, int);
static int compare_match (char *, const char *);
static int complete_get_screenwidth (void);

static char *make_quoted_replacement (char *, int, char *);

/* **************************************************************** */
/*								    */
/*	Completion matching, from readline's point of view.	    */
/*								    */
/* **************************************************************** */

/* Variables known only to the readline library. */

/* If non-zero, non-unique completions always show the list of matches. */
int _rl_complete_show_all = 0;

/* If non-zero, non-unique completions show the list of matches, unless it
   is not possible to do partial completion and modify the line. */
int _rl_complete_show_unmodified = 0;

/* If non-zero, completed directory names have a slash appended. */
int _rl_complete_mark_directories = 1;

/* If non-zero, the symlinked directory completion behavior introduced in
   readline-4.2a is disabled, and symlinks that point to directories have
   a slash appended (subject to the value of _rl_complete_mark_directories).
   This is user-settable via the mark-symlinked-directories variable. */
int _rl_complete_mark_symlink_dirs = 0;

/* If non-zero, completions are printed horizontally in alphabetical order,
   like `ls -x'. */
int _rl_print_completions_horizontally;

/* Non-zero means that case is not significant in filename completion. */
#if (defined (__MSDOS__) && !defined (__DJGPP__)) || (defined (_WIN32) && !defined (__CYGWIN__))
int _rl_completion_case_fold = 1;
#else
int _rl_completion_case_fold = 0;
#endif

/* Non-zero means that `-' and `_' are equivalent when comparing filenames
  for completion. */
int _rl_completion_case_map = 0;

/* If zero, don't match hidden files (filenames beginning with a `.' on
   Unix) when doing filename completion. */
int _rl_match_hidden_files = 1;

/* Length in characters of a common prefix replaced with an ellipsis (`...')
   when displaying completion matches.  Matches whose printable portion has
   more than this number of displaying characters in common will have the common
   display prefix replaced with an ellipsis. */
int _rl_completion_prefix_display_length = 0;

/* The readline-private number of screen columns to use when displaying
   matches.  If < 0 or > _rl_screenwidth, it is ignored. */
int _rl_completion_columns = -1;

#if defined (COLOR_SUPPORT)
/* Non-zero means to use colors to indicate file type when listing possible
   completions.  The colors used are taken from $LS_COLORS, if set. */
int _rl_colored_stats = 0;

/* Non-zero means to use a color (currently magenta) to indicate the common
   prefix of a set of possible word completions. */
int _rl_colored_completion_prefix = 0;
#endif

/* If non-zero, when completing in the middle of a word, don't insert
   characters from the match that match characters following point in
   the word.  This means, for instance, completing when the cursor is
   after the `e' in `Makefile' won't result in `Makefilefile'. */
int _rl_skip_completed_text = 0;

/* If non-zero, menu completion displays the common prefix first in the
   cycle of possible completions instead of the last. */
int _rl_menu_complete_prefix_first = 0;

/* Global variables available to applications using readline. */

#if defined (VISIBLE_STATS)
/* Non-zero means add an additional character to each filename displayed
   during listing completion iff rl_filename_completion_desired which helps
   to indicate the type of file being listed. */
int rl_visible_stats = 0;
#endif /* VISIBLE_STATS */

/* If non-zero, then this is the address of a function to call when
   completing on a directory name.  The function is called with
   the address of a string (the current directory name) as an arg. */
rl_icppfunc_t *rl_directory_completion_hook = (rl_icppfunc_t *)NULL;

rl_icppfunc_t *rl_directory_rewrite_hook = (rl_icppfunc_t *)NULL;

rl_icppfunc_t *rl_filename_stat_hook = (rl_icppfunc_t *)NULL;

/* If non-zero, this is the address of a function to call when reading
   directory entries from the filesystem for completion and comparing
   them to the partial word to be completed.  The function should
   either return its first argument (if no conversion takes place) or
   newly-allocated memory.  This can, for instance, convert filenames
   between character sets for comparison against what's typed at the
   keyboard.  The returned value is what is added to the list of
   matches.  The second argument is the length of the filename to be
   converted. */
rl_dequote_func_t *rl_filename_rewrite_hook = (rl_dequote_func_t *)NULL;

/* Non-zero means readline completion functions perform tilde expansion. */
int rl_complete_with_tilde_expansion = 0;

/* Pointer to the generator function for completion_matches ().
   NULL means to use rl_filename_completion_function (), the default filename
   completer. */
rl_compentry_func_t *rl_completion_entry_function = (rl_compentry_func_t *)NULL;

/* Pointer to generator function for rl_menu_complete ().  NULL means to use
   *rl_completion_entry_function (see above). */
rl_compentry_func_t *rl_menu_completion_entry_function = (rl_compentry_func_t *)NULL;

/* Pointer to alternative function to create matches.
   Function is called with TEXT, START, and END.
   START and END are indices in RL_LINE_BUFFER saying what the boundaries
   of TEXT are.
   If this function exists and returns NULL then call the value of
   rl_completion_entry_function to try to match, otherwise use the
   array of strings returned. */
rl_completion_func_t *rl_attempted_completion_function = (rl_completion_func_t *)NULL;

/* Non-zero means to suppress normal filename completion after the
   user-specified completion function has been called. */
int rl_attempted_completion_over = 0;

/* Set to a character indicating the type of completion being performed
   by rl_complete_internal, available for use by application completion
   functions. */
int rl_completion_type = 0;

/* Up to this many items will be displayed in response to a
   possible-completions call.  After that, we ask the user if
   she is sure she wants to see them all.  A negative value means
   don't ask. */
int rl_completion_query_items = 100;

int _rl_page_completions = 1;

/* The basic list of characters that signal a break between words for the
   completer routine.  The contents of this variable is what breaks words
   in the shell, i.e. " \t\n\"\\'`@$><=" */
const char *rl_basic_word_break_characters = " \t\n\"\\'`@$><=;|&{("; /* }) */

/* List of basic quoting characters. */
const char *rl_basic_quote_characters = "\"'";

/* The list of characters that signal a break between words for
   rl_complete_internal.  The default list is the contents of
   rl_basic_word_break_characters.  */
const char *rl_completer_word_break_characters = 0;

/* Hook function to allow an application to set the completion word
   break characters before readline breaks up the line.  Allows
   position-dependent word break characters. */
rl_cpvfunc_t *rl_completion_word_break_hook = (rl_cpvfunc_t *)NULL;

/* List of characters which can be used to quote a substring of the line.
   Completion occurs on the entire substring, and within the substring
   rl_completer_word_break_characters are treated as any other character,
   unless they also appear within this list. */
const char *rl_completer_quote_characters = (const char *)NULL;

/* List of characters that should be quoted in filenames by the completer. */
const char *rl_filename_quote_characters = (const char *)NULL;

/* List of characters that are word break characters, but should be left
   in TEXT when it is passed to the completion function.  The shell uses
   this to help determine what kind of completing to do. */
const char *rl_special_prefixes = (const char *)NULL;

/* If non-zero, then disallow duplicates in the matches. */
int rl_ignore_completion_duplicates = 1;

/* Non-zero means that the results of the matches are to be treated
   as filenames.  This is ALWAYS zero on entry, and can only be changed
   within a completion entry finder function. */
int rl_filename_completion_desired = 0;

/* Non-zero means that the results of the matches are to be quoted using
   double quotes (or an application-specific quoting mechanism) if the
   filename contains any characters in rl_filename_quote_chars.  This is
   ALWAYS non-zero on entry, and can only be changed within a completion
   entry finder function. */
int rl_filename_quoting_desired = 1;

/* This function, if defined, is called by the completer when real
   filename completion is done, after all the matching names have been
   generated. It is passed a (char**) known as matches in the code below.
   It consists of a NULL-terminated array of pointers to potential
   matching strings.  The 1st element (matches[0]) is the maximal
   substring that is common to all matches. This function can re-arrange
   the list of matches as required, but all elements of the array must be
   free()'d if they are deleted. The main intent of this function is
   to implement FIGNORE a la SunOS csh. */
rl_compignore_func_t *rl_ignore_some_completions_function = (rl_compignore_func_t *)NULL;

/* Set to a function to quote a filename in an application-specific fashion.
   Called with the text to quote, the type of match found (single or multiple)
   and a pointer to the quoting character to be used, which the function can
   reset if desired. */
rl_quote_func_t *rl_filename_quoting_function = rl_quote_filename;
         
/* Function to call to remove quoting characters from a filename.  Called
   before completion is attempted, so the embedded quotes do not interfere
   with matching names in the file system.  Readline doesn't do anything
   with this; it's set only by applications. */
rl_dequote_func_t *rl_filename_dequoting_function = (rl_dequote_func_t *)NULL;

/* Function to call to decide whether or not a word break character is
   quoted.  If a character is quoted, it does not break words for the
   completer. */
rl_linebuf_func_t *rl_char_is_quoted_p = (rl_linebuf_func_t *)NULL;

/* If non-zero, the completion functions don't append anything except a
   possible closing quote.  This is set to 0 by rl_complete_internal and
   may be changed by an application-specific completion function. */
int rl_completion_suppress_append = 0;

/* Character appended to completed words when at the end of the line.  The
   default is a space. */
int rl_completion_append_character = ' ';

/* If non-zero, the completion functions don't append any closing quote.
   This is set to 0 by rl_complete_internal and may be changed by an
   application-specific completion function. */
int rl_completion_suppress_quote = 0;

/* Set to any quote character readline thinks it finds before any application
   completion function is called. */
int rl_completion_quote_character;

/* Set to a non-zero value if readline found quoting anywhere in the word to
   be completed; set before any application completion function is called. */
int rl_completion_found_quote;

/* If non-zero, a slash will be appended to completed filenames that are
   symbolic links to directory names, subject to the value of the
   mark-directories variable (which is user-settable).  This exists so
   that application completion functions can override the user's preference
   (set via the mark-symlinked-directories variable) if appropriate.
   It's set to the value of _rl_complete_mark_symlink_dirs in
   rl_complete_internal before any application-specific completion
   function is called, so without that function doing anything, the user's
   preferences are honored. */
int rl_completion_mark_symlink_dirs;

/* If non-zero, inhibit completion (temporarily). */
int rl_inhibit_completion;

/* Set to the last key used to invoke one of the completion functions */
int rl_completion_invoking_key;

/* If non-zero, sort the completion matches.  On by default. */
int rl_sort_completion_matches = 1;

/* Variables local to this file. */

/* Local variable states what happened during the last completion attempt. */
static int completion_changed_buffer;
static int last_completion_failed = 0;

/* The result of the query to the user about displaying completion matches */
static int completion_y_or_n;

static int _rl_complete_display_matches_interrupt = 0;

/*************************************/
/*				     */
/*    Bindable completion functions  */
/*				     */
/*************************************/

/* Complete the word at or before point.  You have supplied the function
   that does the initial simple matching selection algorithm (see
   rl_completion_matches ()).  The default is to do filename completion. */
int
rl_complete (int ignore, int invoking_key)
{
  rl_completion_invoking_key = invoking_key;

  if (rl_inhibit_completion)
    return (_rl_insert_char (ignore, invoking_key));
#if 0
  else if (rl_last_func == rl_complete && completion_changed_buffer == 0 && last_completion_failed == 0)
#else
  else if (rl_last_func == rl_complete && completion_changed_buffer == 0)
#endif
    return (rl_complete_internal ('?'));
  else if (_rl_complete_show_all)
    return (rl_complete_internal ('!'));
  else if (_rl_complete_show_unmodified)
    return (rl_complete_internal ('@'));
  else
    return (rl_complete_internal (TAB));
}

/* List the possible completions.  See description of rl_complete (). */
int
rl_possible_completions (int ignore, int invoking_key)
{
  rl_completion_invoking_key = invoking_key;
  return (rl_complete_internal ('?'));
}

int
rl_insert_completions (int ignore, int invoking_key)
{
  rl_completion_invoking_key = invoking_key;
  return (rl_complete_internal ('*'));
}

/* Return the correct value to pass to rl_complete_internal performing
   the same tests as rl_complete.  This allows consecutive calls to an
   application's completion function to list possible completions and for
   an application-specific completion function to honor the
   show-all-if-ambiguous readline variable. */
int
rl_completion_mode (rl_command_func_t *cfunc)
{
  if (rl_last_func == cfunc && !completion_changed_buffer)
    return '?';
  else if (_rl_complete_show_all)
    return '!';
  else if (_rl_complete_show_unmodified)
    return '@';
  else
    return TAB;
}

/************************************/
/*				    */
/*    Completion utility functions  */
/*				    */
/************************************/

/* Reset public readline state on a signal or other event. */
void
_rl_reset_completion_state (void)
{
  rl_completion_found_quote = 0;
  rl_completion_quote_character = 0;
}

static void
_rl_complete_sigcleanup (int sig, void *ptr)
{
  if (sig == SIGINT)	/* XXX - for now */
    {
      _rl_free_match_list ((char **)ptr);
      _rl_complete_display_matches_interrupt = 1;
    }
}

/* Set default values for readline word completion.  These are the variables
   that application completion functions can change or inspect. */
static void
set_completion_defaults (int what_to_do)
{
  /* Only the completion entry function can change these. */
  rl_filename_completion_desired = 0;
  rl_filename_quoting_desired = 1;
  rl_completion_type = what_to_do;
  rl_completion_suppress_append = rl_completion_suppress_quote = 0;
  rl_completion_append_character = ' ';

  /* The completion entry function may optionally change this. */
  rl_completion_mark_symlink_dirs = _rl_complete_mark_symlink_dirs;

  /* Reset private state. */
  _rl_complete_display_matches_interrupt = 0;
}

/* The user must press "y" or "n". Non-zero return means "y" pressed. */
static int
get_y_or_n (int for_pager)
{
  int c;

  /* For now, disable pager in callback mode, until we later convert to state
     driven functions.  Have to wait until next major version to add new
     state definition, since it will change value of RL_STATE_DONE. */
#if defined (READLINE_CALLBACKS)
  if (RL_ISSTATE (RL_STATE_CALLBACK))
    return 1;
#endif

  for (;;)
    {
      RL_SETSTATE(RL_STATE_MOREINPUT);
      c = rl_read_key ();
      RL_UNSETSTATE(RL_STATE_MOREINPUT);

      if (c == 'y' || c == 'Y' || c == ' ')
	return (1);
      if (c == 'n' || c == 'N' || c == RUBOUT)
	return (0);
      if (c == ABORT_CHAR || c < 0)
	_rl_abort_internal ();
      if (for_pager && (c == NEWLINE || c == RETURN))
	return (2);
      if (for_pager && (c == 'q' || c == 'Q'))
	return (0);
      rl_ding ();
    }
}

static int
_rl_internal_pager (int lines)
{
  int i;

  fprintf (rl_outstream, "--More--");
  fflush (rl_outstream);
  i = get_y_or_n (1);
  _rl_erase_entire_line ();
  if (i == 0)
    return -1;
  else if (i == 2)
    return (lines - 1);
  else
    return 0;
}

static int
path_isdir (const char *filename)
{
  struct stat finfo;

  return (stat (filename, &finfo) == 0 && S_ISDIR (finfo.st_mode));
}

#if defined (VISIBLE_STATS)
/* Return the character which best describes FILENAME.
     `@' for symbolic links
     `/' for directories
     `*' for executables
     `=' for sockets
     `|' for FIFOs
     `%' for character special devices
     `#' for block special devices */
static int
stat_char (char *filename)
{
  struct stat finfo;
  int character, r;
  char *f;
  const char *fn;

  /* Short-circuit a //server on cygwin, since that will always behave as
     a directory. */
#if __CYGWIN__
  if (filename[0] == '/' && filename[1] == '/' && strchr (filename+2, '/') == 0)
    return '/';
#endif

  f = 0;
  if (rl_filename_stat_hook)
    {
      f = savestring (filename);
      (*rl_filename_stat_hook) (&f);
      fn = f;
    }
  else
    fn = filename;
    
#if defined (HAVE_LSTAT) && defined (S_ISLNK)
  r = lstat (fn, &finfo);
#else
  r = stat (fn, &finfo);
#endif

  if (r == -1)
    {
      xfree (f);
      return (0);
    }

  character = 0;
  if (S_ISDIR (finfo.st_mode))
    character = '/';
#if defined (S_ISCHR)
  else if (S_ISCHR (finfo.st_mode))
    character = '%';
#endif /* S_ISCHR */
#if defined (S_ISBLK)
  else if (S_ISBLK (finfo.st_mode))
    character = '#';
#endif /* S_ISBLK */
#if defined (S_ISLNK)
  else if (S_ISLNK (finfo.st_mode))
    character = '@';
#endif /* S_ISLNK */
#if defined (S_ISSOCK)
  else if (S_ISSOCK (finfo.st_mode))
    character = '=';
#endif /* S_ISSOCK */
#if defined (S_ISFIFO)
  else if (S_ISFIFO (finfo.st_mode))
    character = '|';
#endif
  else if (S_ISREG (finfo.st_mode))
    {
#if defined (_WIN32) && !defined (__CYGWIN__)
      char *ext;

      /* Windows doesn't do access and X_OK; check file extension instead */
      ext = strrchr (fn, '.');
      if (ext && (_rl_stricmp (ext, ".exe") == 0 ||
		  _rl_stricmp (ext, ".cmd") == 0 ||
		  _rl_stricmp (ext, ".bat") == 0 ||
		  _rl_stricmp (ext, ".com") == 0))
	character = '*';
#else
      if (access (filename, X_OK) == 0)
	character = '*';
#endif
    }

  xfree (f);
  return (character);
}
#endif /* VISIBLE_STATS */

#if defined (COLOR_SUPPORT)
static int
colored_stat_start (const char *filename)
{
  _rl_set_normal_color ();
  return (_rl_print_color_indicator (filename));
}

static void
colored_stat_end (void)
{
  _rl_prep_non_filename_text ();
  _rl_put_indicator (&_rl_color_indicator[C_CLR_TO_EOL]);
}

static int
colored_prefix_start (void)
{
  _rl_set_normal_color ();
  return (_rl_print_prefix_color ());
}

static void
colored_prefix_end (void)
{
  colored_stat_end ();		/* for now */
}
#endif

/* Return the portion of PATHNAME that should be output when listing
   possible completions.  If we are hacking filename completion, we
   are only interested in the basename, the portion following the
   final slash.  Otherwise, we return what we were passed.  Since
   printing empty strings is not very informative, if we're doing
   filename completion, and the basename is the empty string, we look
   for the previous slash and return the portion following that.  If
   there's no previous slash, we just return what we were passed. */
static char *
printable_part (char *pathname)
{
  char *temp, *x;

  if (rl_filename_completion_desired == 0)	/* don't need to do anything */
    return (pathname);

  temp = strrchr (pathname, '/');
#if defined (__MSDOS__) || defined (_WIN32)
  if (temp == 0 && ISALPHA ((unsigned char)pathname[0]) && pathname[1] == ':')
    temp = pathname + 1;
#endif

  if (temp == 0 || *temp == '\0')
    return (pathname);
  else if (temp[1] == 0 && temp == pathname)
    return (pathname);
  /* If the basename is NULL, we might have a pathname like '/usr/src/'.
     Look for a previous slash and, if one is found, return the portion
     following that slash.  If there's no previous slash, just return the
     pathname we were passed. */
  else if (temp[1] == '\0')
    {
      for (x = temp - 1; x > pathname; x--)
        if (*x == '/')
          break;
      return ((*x == '/') ? x + 1 : pathname);
    }
  else
    return ++temp;
}

/* Compute width of STRING when displayed on screen by print_filename */
static int
fnwidth (const char *string)
{
  int width, pos;
#if defined (HANDLE_MULTIBYTE)
  mbstate_t ps;
  int left, w;
  size_t clen;
  WCHAR_T wc;

  left = strlen (string) + 1;
  memset (&ps, 0, sizeof (mbstate_t));
#endif

  width = pos = 0;
  while (string[pos])
    {
      if (CTRL_CHAR (string[pos]) || string[pos] == RUBOUT)
	{
	  width += 2;
	  pos++;
	}
      else
	{
#if defined (HANDLE_MULTIBYTE)
	  clen = MBRTOWC (&wc, string + pos, left - pos, &ps);
	  if (MB_INVALIDCH (clen))
	    {
	      width++;
	      pos++;
	      memset (&ps, 0, sizeof (mbstate_t));
	    }
	  else if (MB_NULLWCH (clen))
	    break;
	  else
	    {
	      pos += clen;
	      w = WCWIDTH (wc);
	      width += (w >= 0) ? w : 1;
	    }
#else
	  width++;
	  pos++;
#endif
	}
    }

  return width;
}

#define ELLIPSIS_LEN	3

static int
fnprint (const char *to_print, int prefix_bytes, const char *real_pathname)
{
  int printed_len, w;
  const char *s;
  int common_prefix_len, print_len;
#if defined (HANDLE_MULTIBYTE)
  mbstate_t ps;
  const char *end;
  size_t tlen;
  int width;
  WCHAR_T wc;

  print_len = strlen (to_print);
  end = to_print + print_len + 1;
  memset (&ps, 0, sizeof (mbstate_t));
#else
  print_len = strlen (to_print);
#endif

  printed_len = common_prefix_len = 0;

  /* Don't print only the ellipsis if the common prefix is one of the
     possible completions.  Only cut off prefix_bytes if we're going to be
     printing the ellipsis, which takes precedence over coloring the
     completion prefix (see print_filename() below). */
  if (_rl_completion_prefix_display_length > 0 && prefix_bytes >= print_len)
    prefix_bytes = 0;

#if defined (COLOR_SUPPORT)
  if (_rl_colored_stats && (prefix_bytes == 0 || _rl_colored_completion_prefix <= 0))
    colored_stat_start (real_pathname);
#endif

  if (prefix_bytes && _rl_completion_prefix_display_length > 0 &&
      prefix_bytes > _rl_completion_prefix_display_length)
    {
      char ellipsis;

      ellipsis = (to_print[prefix_bytes] == '.') ? '_' : '.';
      for (w = 0; w < ELLIPSIS_LEN; w++)
	putc (ellipsis, rl_outstream);
      printed_len = ELLIPSIS_LEN;
    }
#if defined (COLOR_SUPPORT)
  else if (prefix_bytes && _rl_colored_completion_prefix > 0)
    {
      common_prefix_len = prefix_bytes;
      prefix_bytes = 0;
      /* XXX - print color indicator start here */
      colored_prefix_start ();
    }
#endif

  s = to_print + prefix_bytes;
  while (*s)
    {
      if (CTRL_CHAR (*s))
        {
          putc ('^', rl_outstream);
          putc (UNCTRL (*s), rl_outstream);
          printed_len += 2;
          s++;
#if defined (HANDLE_MULTIBYTE)
	  memset (&ps, 0, sizeof (mbstate_t));
#endif
        }
      else if (*s == RUBOUT)
	{
	  putc ('^', rl_outstream);
	  putc ('?', rl_outstream);
	  printed_len += 2;
	  s++;
#if defined (HANDLE_MULTIBYTE)
	  memset (&ps, 0, sizeof (mbstate_t));
#endif
	}
      else
	{
#if defined (HANDLE_MULTIBYTE)
	  tlen = MBRTOWC (&wc, s, end - s, &ps);
	  if (MB_INVALIDCH (tlen))
	    {
	      tlen = 1;
	      width = 1;
	      memset (&ps, 0, sizeof (mbstate_t));
	    }
	  else if (MB_NULLWCH (tlen))
	    break;
	  else
	    {
	      w = WCWIDTH (wc);
	      width = (w >= 0) ? w : 1;
	    }
	  fwrite (s, 1, tlen, rl_outstream);
	  s += tlen;
	  printed_len += width;
#else
	  putc (*s, rl_outstream);
	  s++;
	  printed_len++;
#endif
	}
      if (common_prefix_len > 0 && (s - to_print) >= common_prefix_len)
	{
#if defined (COLOR_SUPPORT)
	  /* printed bytes = s - to_print */
	  /* printed bytes should never be > but check for paranoia's sake */
	  colored_prefix_end ();
	  if (_rl_colored_stats)
	    colored_stat_start (real_pathname);		/* XXX - experiment */
#endif
	  common_prefix_len = 0;
	}
    }

#if defined (COLOR_SUPPORT)
  /* XXX - unconditional for now */
  if (_rl_colored_stats)
    colored_stat_end ();
#endif

  return printed_len;
}

/* Output TO_PRINT to rl_outstream.  If VISIBLE_STATS is defined and we
   are using it, check for and output a single character for `special'
   filenames.  Return the number of characters we output. */

static int
print_filename (char *to_print, char *full_pathname, int prefix_bytes)
{
  int printed_len, extension_char, slen, tlen;
  char *s, c, *new_full_pathname, *dn;

  extension_char = 0;
#if defined (COLOR_SUPPORT)
  /* Defer printing if we want to prefix with a color indicator */
  if (_rl_colored_stats == 0 || rl_filename_completion_desired == 0)
#endif
    printed_len = fnprint (to_print, prefix_bytes, to_print);

  if (rl_filename_completion_desired && (
#if defined (VISIBLE_STATS)
     rl_visible_stats ||
#endif
#if defined (COLOR_SUPPORT)
     _rl_colored_stats ||
#endif
     _rl_complete_mark_directories))
    {
      /* If to_print != full_pathname, to_print is the basename of the
	 path passed.  In this case, we try to expand the directory
	 name before checking for the stat character. */
      if (to_print != full_pathname)
	{
	  /* Terminate the directory name. */
	  c = to_print[-1];
	  to_print[-1] = '\0';

	  /* If setting the last slash in full_pathname to a NUL results in
	     full_pathname being the empty string, we are trying to complete
	     files in the root directory.  If we pass a null string to the
	     bash directory completion hook, for example, it will expand it
	     to the current directory.  We just want the `/'. */
	  if (full_pathname == 0 || *full_pathname == 0)
	    dn = "/";
	  else if (full_pathname[0] != '/')
	    dn = full_pathname;
	  else if (full_pathname[1] == 0)
	    dn = "//";		/* restore trailing slash to `//' */
	  else if (full_pathname[1] == '/' && full_pathname[2] == 0)
	    dn = "/";		/* don't turn /// into // */
	  else
	    dn = full_pathname;
	  s = tilde_expand (dn);
	  if (rl_directory_completion_hook)
	    (*rl_directory_completion_hook) (&s);

	  slen = strlen (s);
	  tlen = strlen (to_print);
	  new_full_pathname = (char *)xmalloc (slen + tlen + 2);
	  strcpy (new_full_pathname, s);
	  if (s[slen - 1] == '/')
	    slen--;
	  else
	    new_full_pathname[slen] = '/';
	  strcpy (new_full_pathname + slen + 1, to_print);

#if defined (VISIBLE_STATS)
	  if (rl_visible_stats)
	    extension_char = stat_char (new_full_pathname);
	  else
#endif
	  if (_rl_complete_mark_directories)
	    {
	      dn = 0;
	      if (rl_directory_completion_hook == 0 && rl_filename_stat_hook)
		{
		  dn = savestring (new_full_pathname);
		  (*rl_filename_stat_hook) (&dn);
		  xfree (new_full_pathname);
		  new_full_pathname = dn;
		}
	      if (path_isdir (new_full_pathname))
		extension_char = '/';
	    }

	  /* Move colored-stats code inside fnprint() */
#if defined (COLOR_SUPPORT)
	  if (_rl_colored_stats)
	    printed_len = fnprint (to_print, prefix_bytes, new_full_pathname);
#endif

	  xfree (new_full_pathname);
	  to_print[-1] = c;
	}
      else
	{
	  s = tilde_expand (full_pathname);
#if defined (VISIBLE_STATS)
	  if (rl_visible_stats)
	    extension_char = stat_char (s);
	  else
#endif
	    if (_rl_complete_mark_directories && path_isdir (s))
	      extension_char = '/';

	  /* Move colored-stats code inside fnprint() */
#if defined (COLOR_SUPPORT)
	  if (_rl_colored_stats)
	    printed_len = fnprint (to_print, prefix_bytes, s);
#endif
	}

      xfree (s);
      if (extension_char)
	{
	  putc (extension_char, rl_outstream);
	  printed_len++;
	}
    }

  return printed_len;
}

static char *
rl_quote_filename (char *s, int rtype, char *qcp)
{
  char *r;

  r = (char *)xmalloc (strlen (s) + 2);
  *r = *rl_completer_quote_characters;
  strcpy (r + 1, s);
  if (qcp)
    *qcp = *rl_completer_quote_characters;
  return r;
}

/* Find the bounds of the current word for completion purposes, and leave
   rl_point set to the end of the word.  This function skips quoted
   substrings (characters between matched pairs of characters in
   rl_completer_quote_characters).  First we try to find an unclosed
   quoted substring on which to do matching.  If one is not found, we use
   the word break characters to find the boundaries of the current word.
   We call an application-specific function to decide whether or not a
   particular word break character is quoted; if that function returns a
   non-zero result, the character does not break a word.  This function
   returns the opening quote character if we found an unclosed quoted
   substring, '\0' otherwise.  FP, if non-null, is set to a value saying
   which (shell-like) quote characters we found (single quote, double
   quote, or backslash) anywhere in the string.  DP, if non-null, is set to
   the value of the delimiter character that caused a word break. */

char
_rl_find_completion_word (int *fp, int *dp)
{
  int scan, end, found_quote, delimiter, pass_next, isbrk;
  char quote_char;
  const char *brkchars;

  end = rl_point;
  found_quote = delimiter = 0;
  quote_char = '\0';

  brkchars = 0;
  if (rl_completion_word_break_hook)
    brkchars = (*rl_completion_word_break_hook) ();
  if (brkchars == 0)
    brkchars = rl_completer_word_break_characters;

  if (rl_completer_quote_characters)
    {
      /* We have a list of characters which can be used in pairs to
	 quote substrings for the completer.  Try to find the start
	 of an unclosed quoted substring. */
      /* FOUND_QUOTE is set so we know what kind of quotes we found. */
      for (scan = pass_next = 0; scan < end; scan = MB_NEXTCHAR (rl_line_buffer, scan, 1, MB_FIND_ANY))
	{
	  if (pass_next)
	    {
	      pass_next = 0;
	      continue;
	    }

	  /* Shell-like semantics for single quotes -- don't allow backslash
	     to quote anything in single quotes, especially not the closing
	     quote.  If you don't like this, take out the check on the value
	     of quote_char. */
	  if (quote_char != '\'' && rl_line_buffer[scan] == '\\')
	    {
	      pass_next = 1;
	      found_quote |= RL_QF_BACKSLASH;
	      continue;
	    }

	  if (quote_char != '\0')
	    {
	      /* Ignore everything until the matching close quote char. */
	      if (rl_line_buffer[scan] == quote_char)
		{
		  /* Found matching close.  Abandon this substring. */
		  quote_char = '\0';
		  rl_point = end;
		}
	    }
	  else if (strchr (rl_completer_quote_characters, rl_line_buffer[scan]))
	    {
	      /* Found start of a quoted substring. */
	      quote_char = rl_line_buffer[scan];
	      rl_point = scan + 1;
	      /* Shell-like quoting conventions. */
	      if (quote_char == '\'')
		found_quote |= RL_QF_SINGLE_QUOTE;
	      else if (quote_char == '"')
		found_quote |= RL_QF_DOUBLE_QUOTE;
	      else
		found_quote |= RL_QF_OTHER_QUOTE;      
	    }
	}
    }

  if (rl_point == end && quote_char == '\0')
    {
      /* We didn't find an unclosed quoted substring upon which to do
         completion, so use the word break characters to find the
         substring on which to complete. */
      while (rl_point = MB_PREVCHAR (rl_line_buffer, rl_point, MB_FIND_ANY))
	{
	  scan = rl_line_buffer[rl_point];

	  if (strchr (brkchars, scan) == 0)
	    continue;

	  /* Call the application-specific function to tell us whether
	     this word break character is quoted and should be skipped. */
	  if (rl_char_is_quoted_p && found_quote &&
	      (*rl_char_is_quoted_p) (rl_line_buffer, rl_point))
	    continue;

	  /* Convoluted code, but it avoids an n^2 algorithm with calls
	     to char_is_quoted. */
	  break;
	}
    }

  /* If we are at an unquoted word break, then advance past it. */
  scan = rl_line_buffer[rl_point];

  /* If there is an application-specific function to say whether or not
     a character is quoted and we found a quote character, let that
     function decide whether or not a character is a word break, even
     if it is found in rl_completer_word_break_characters.  Don't bother
     if we're at the end of the line, though. */
  if (scan)
    {
      if (rl_char_is_quoted_p)
	isbrk = (found_quote == 0 ||
		(*rl_char_is_quoted_p) (rl_line_buffer, rl_point) == 0) &&
		strchr (brkchars, scan) != 0;
      else
	isbrk = strchr (brkchars, scan) != 0;

      if (isbrk)
	{
	  /* If the character that caused the word break was a quoting
	     character, then remember it as the delimiter. */
	  if (rl_basic_quote_characters &&
	      strchr (rl_basic_quote_characters, scan) &&
	      (end - rl_point) > 1)
	    delimiter = scan;

	  /* If the character isn't needed to determine something special
	     about what kind of completion to perform, then advance past it. */
	  if (rl_special_prefixes == 0 || strchr (rl_special_prefixes, scan) == 0)
	    rl_point++;
	}
    }

  if (fp)
    *fp = found_quote;
  if (dp)
    *dp = delimiter;

  return (quote_char);
}

static char **
gen_completion_matches (char *text, int start, int end, rl_compentry_func_t *our_func, int found_quote, int quote_char)
{
  char **matches;

  rl_completion_found_quote = found_quote;
  rl_completion_quote_character = quote_char;

  /* If the user wants to TRY to complete, but then wants to give
     up and use the default completion function, they set the
     variable rl_attempted_completion_function. */
  if (rl_attempted_completion_function)
    {
      matches = (*rl_attempted_completion_function) (text, start, end);
      if (RL_SIG_RECEIVED())
	{
	  _rl_free_match_list (matches);
	  matches = 0;
	  RL_CHECK_SIGNALS ();
	}

      if (matches || rl_attempted_completion_over)
	{
	  rl_attempted_completion_over = 0;
	  return (matches);
	}
    }

  /* XXX -- filename dequoting moved into rl_filename_completion_function */

  /* rl_completion_matches will check for signals as well to avoid a long
     delay while reading a directory. */
  matches = rl_completion_matches (text, our_func);
  if (RL_SIG_RECEIVED())
    {
      _rl_free_match_list (matches);
      matches = 0;
      RL_CHECK_SIGNALS ();
    }
  return matches;  
}

/* Filter out duplicates in MATCHES.  This frees up the strings in
   MATCHES. */
static char **
remove_duplicate_matches (char **matches)
{
  char *lowest_common;
  int i, j, newlen;
  char dead_slot;
  char **temp_array;

  /* Sort the items. */
  for (i = 0; matches[i]; i++)
    ;

  /* Sort the array without matches[0], since we need it to
     stay in place no matter what. */
  if (i && rl_sort_completion_matches)
    qsort (matches+1, i-1, sizeof (char *), (QSFUNC *)_rl_qsort_string_compare);

  /* Remember the lowest common denominator for it may be unique. */
  lowest_common = savestring (matches[0]);

  for (i = newlen = 0; matches[i + 1]; i++)
    {
      if (strcmp (matches[i], matches[i + 1]) == 0)
	{
	  xfree (matches[i]);
	  matches[i] = (char *)&dead_slot;
	}
      else
	newlen++;
    }

  /* We have marked all the dead slots with (char *)&dead_slot.
     Copy all the non-dead entries into a new array. */
  temp_array = (char **)xmalloc ((3 + newlen) * sizeof (char *));
  for (i = j = 1; matches[i]; i++)
    {
      if (matches[i] != (char *)&dead_slot)
	temp_array[j++] = matches[i];
    }
  temp_array[j] = (char *)NULL;

  if (matches[0] != (char *)&dead_slot)
    xfree (matches[0]);

  /* Place the lowest common denominator back in [0]. */
  temp_array[0] = lowest_common;

  /* If there is one string left, and it is identical to the
     lowest common denominator, then the LCD is the string to
     insert. */
  if (j == 2 && strcmp (temp_array[0], temp_array[1]) == 0)
    {
      xfree (temp_array[1]);
      temp_array[1] = (char *)NULL;
    }
  return (temp_array);
}

/* Find the common prefix of the list of matches, and put it into
   matches[0]. */
static int
compute_lcd_of_matches (char **match_list, int matches, const char *text)
{
  register int i, c1, c2, si;
  int low;		/* Count of max-matched characters. */
  int lx;
  char *dtext;		/* dequoted TEXT, if needed */
#if defined (HANDLE_MULTIBYTE)
  int v;
  size_t v1, v2;
  mbstate_t ps1, ps2;
  WCHAR_T wc1, wc2;
#endif

  /* If only one match, just use that.  Otherwise, compare each
     member of the list with the next, finding out where they
     stop matching. */
  if (matches == 1)
    {
      match_list[0] = match_list[1];
      match_list[1] = (char *)NULL;
      return 1;
    }

  for (i = 1, low = 100000; i < matches; i++)
    {
#if defined (HANDLE_MULTIBYTE)
      if (MB_CUR_MAX > 1 && rl_byte_oriented == 0)
	{
	  memset (&ps1, 0, sizeof (mbstate_t));
	  memset (&ps2, 0, sizeof (mbstate_t));
	}
#endif
      for (si = 0; (c1 = match_list[i][si]) && (c2 = match_list[i + 1][si]); si++)
	{
	    if (_rl_completion_case_fold)
	      {
	        c1 = _rl_to_lower (c1);
	        c2 = _rl_to_lower (c2);
	      }
#if defined (HANDLE_MULTIBYTE)
	    if (MB_CUR_MAX > 1 && rl_byte_oriented == 0)
	      {
		v1 = MBRTOWC (&wc1, match_list[i]+si, strlen (match_list[i]+si), &ps1);
		v2 = MBRTOWC (&wc2, match_list[i+1]+si, strlen (match_list[i+1]+si), &ps2);
		if (MB_INVALIDCH (v1) || MB_INVALIDCH (v2))
		  {
		    if (c1 != c2)	/* do byte comparison */
		      break;
		    continue;
		  }
		if (_rl_completion_case_fold)
		  {
		    wc1 = towlower (wc1);
		    wc2 = towlower (wc2);
		  }
		if (wc1 != wc2)
		  break;
		else if (v1 > 1)
		  si += v1 - 1;
	      }
	    else
#endif
	    if (c1 != c2)
	      break;
	}

      if (low > si)
	low = si;
    }

  /* If there were multiple matches, but none matched up to even the
     first character, and the user typed something, use that as the
     value of matches[0]. */
  if (low == 0 && text && *text)
    {
      match_list[0] = (char *)xmalloc (strlen (text) + 1);
      strcpy (match_list[0], text);
    }
  else
    {
      match_list[0] = (char *)xmalloc (low + 1);

      /* XXX - this might need changes in the presence of multibyte chars */

      /* If we are ignoring case, try to preserve the case of the string
	 the user typed in the face of multiple matches differing in case. */
      if (_rl_completion_case_fold)
	{
	  /* We're making an assumption here:
		IF we're completing filenames AND
		   the application has defined a filename dequoting function AND
		   we found a quote character AND
		   the application has requested filename quoting
		THEN
		   we assume that TEXT was dequoted before checking against
		   the file system and needs to be dequoted here before we
		   check against the list of matches
		FI */
	  dtext = (char *)NULL;
	  if (rl_filename_completion_desired &&
	      rl_filename_dequoting_function &&
	      rl_completion_found_quote &&
	      rl_filename_quoting_desired)
	    {
	      dtext = (*rl_filename_dequoting_function) ((char *)text, rl_completion_quote_character);
	      text = dtext;
	    }

	  /* sort the list to get consistent answers. */
	  if (rl_sort_completion_matches)
	    qsort (match_list+1, matches, sizeof(char *), (QSFUNC *)_rl_qsort_string_compare);

	  si = strlen (text);
	  lx = (si <= low) ? si : low;	/* check shorter of text and matches */
	  /* Try to preserve the case of what the user typed in the presence of
	     multiple matches: check each match for something that matches
	     what the user typed taking case into account; use it up to common
	     length of matches if one is found.  If not, just use first match. */
	  for (i = 1; i <= matches; i++)
	    if (strncmp (match_list[i], text, lx) == 0)
	      {
		strncpy (match_list[0], match_list[i], low);
		break;
	      }
	  /* no casematch, use first entry */
	  if (i > matches)
	    strncpy (match_list[0], match_list[1], low);

	  FREE (dtext);
	}
      else
        strncpy (match_list[0], match_list[1], low);

      match_list[0][low] = '\0';
    }

  return matches;
}

static int
postprocess_matches (char ***matchesp, int matching_filenames)
{
  char *t, **matches, **temp_matches;
  int nmatch, i;

  matches = *matchesp;

  if (matches == 0)
    return 0;

  /* It seems to me that in all the cases we handle we would like
     to ignore duplicate possibilities.  Scan for the text to
     insert being identical to the other completions. */
  if (rl_ignore_completion_duplicates)
    {
      temp_matches = remove_duplicate_matches (matches);
      xfree (matches);
      matches = temp_matches;
    }

  /* If we are matching filenames, then here is our chance to
     do clever processing by re-examining the list.  Call the
     ignore function with the array as a parameter.  It can
     munge the array, deleting matches as it desires. */
  if (rl_ignore_some_completions_function && matching_filenames)
    {
      for (nmatch = 1; matches[nmatch]; nmatch++)
	;
      (void)(*rl_ignore_some_completions_function) (matches);
      if (matches == 0 || matches[0] == 0)
	{
	  FREE (matches);
	  *matchesp = (char **)0;
	  return 0;
        }
      else
	{
	  /* If we removed some matches, recompute the common prefix. */
	  for (i = 1; matches[i]; i++)
	    ;
	  if (i > 1 && i < nmatch)
	    {
	      t = matches[0];
	      compute_lcd_of_matches (matches, i - 1, t);
	      FREE (t);
	    }
	}
    }

  *matchesp = matches;
  return (1);
}

static int
complete_get_screenwidth (void)
{
  int cols;
  char *envcols;

  cols = _rl_completion_columns;
  if (cols >= 0 && cols <= _rl_screenwidth)
    return cols;
  envcols = getenv ("COLUMNS");
  if (envcols && *envcols)
    cols = atoi (envcols);
  if (cols >= 0 && cols <= _rl_screenwidth)
    return cols;
  return _rl_screenwidth;
}

/* A convenience function for displaying a list of strings in
   columnar format on readline's output stream.  MATCHES is the list
   of strings, in argv format, LEN is the number of strings in MATCHES,
   and MAX is the length of the longest string in MATCHES. */
void
rl_display_match_list (char **matches, int len, int max)
{
  int count, limit, printed_len, lines, cols;
  int i, j, k, l, common_length, sind;
  char *temp, *t;

  /* Find the length of the prefix common to all items: length as displayed
     characters (common_length) and as a byte index into the matches (sind) */
  common_length = sind = 0;
  if (_rl_completion_prefix_display_length > 0)
    {
      t = printable_part (matches[0]);
      /* check again in case of /usr/src/ */
      temp = rl_filename_completion_desired ? strrchr (t, '/') : 0;
      common_length = temp ? fnwidth (temp) : fnwidth (t);
      sind = temp ? strlen (temp) : strlen (t);
      if (common_length > max || sind > max)
	common_length = sind = 0;

      if (common_length > _rl_completion_prefix_display_length && common_length > ELLIPSIS_LEN)
	max -= common_length - ELLIPSIS_LEN;
      else if (_rl_colored_completion_prefix <= 0)
	common_length = sind = 0;
    }
#if defined (COLOR_SUPPORT)
  else if (_rl_colored_completion_prefix > 0)
    {
      t = printable_part (matches[0]);
      temp = rl_filename_completion_desired ? strrchr (t, '/') : 0;
      common_length = temp ? fnwidth (temp) : fnwidth (t);
      sind = temp ? RL_STRLEN (temp+1) : RL_STRLEN (t);		/* want portion after final slash */
      if (common_length > max || sind > max)
	common_length = sind = 0;
    }
#endif

  /* How many items of MAX length can we fit in the screen window? */
  cols = complete_get_screenwidth ();
  max += 2;
  limit = cols / max;
  if (limit != 1 && (limit * max == cols))
    limit--;

  /* If cols == 0, limit will end up -1 */
  if (cols < _rl_screenwidth && limit < 0)
    limit = 1;

  /* Avoid a possible floating exception.  If max > cols,
     limit will be 0 and a divide-by-zero fault will result. */
  if (limit == 0)
    limit = 1;

  /* How many iterations of the printing loop? */
  count = (len + (limit - 1)) / limit;

  /* Watch out for special case.  If LEN is less than LIMIT, then
     just do the inner printing loop.
	   0 < len <= limit  implies  count = 1. */

  /* Sort the items if they are not already sorted. */
  if (rl_ignore_completion_duplicates == 0 && rl_sort_completion_matches)
    qsort (matches + 1, len, sizeof (char *), (QSFUNC *)_rl_qsort_string_compare);

  rl_crlf ();

  lines = 0;
  if (_rl_print_completions_horizontally == 0)
    {
      /* Print the sorted items, up-and-down alphabetically, like ls. */
      for (i = 1; i <= count; i++)
	{
	  for (j = 0, l = i; j < limit; j++)
	    {
	      if (l > len || matches[l] == 0)
		break;
	      else
		{
		  temp = printable_part (matches[l]);
		  printed_len = print_filename (temp, matches[l], sind);

		  if (j + 1 < limit)
		    {
		      if (max <= printed_len)
			putc (' ', rl_outstream);
		      else
			for (k = 0; k < max - printed_len; k++)
			  putc (' ', rl_outstream);
		    }
		}
	      l += count;
	    }
	  rl_crlf ();
#if defined (SIGWINCH)
	  if (RL_SIG_RECEIVED () && RL_SIGWINCH_RECEIVED() == 0)
#else
	  if (RL_SIG_RECEIVED ())
#endif
	    return;
	  lines++;
	  if (_rl_page_completions && lines >= (_rl_screenheight - 1) && i < count)
	    {
	      lines = _rl_internal_pager (lines);
	      if (lines < 0)
		return;
	    }
	}
    }
  else
    {
      /* Print the sorted items, across alphabetically, like ls -x. */
      for (i = 1; matches[i]; i++)
	{
	  temp = printable_part (matches[i]);
	  printed_len = print_filename (temp, matches[i], sind);
	  /* Have we reached the end of this line? */
#if defined (SIGWINCH)
	  if (RL_SIG_RECEIVED () && RL_SIGWINCH_RECEIVED() == 0)
#else
	  if (RL_SIG_RECEIVED ())
#endif
	    return;
	  if (matches[i+1])
	    {
	      if (limit == 1 || (i && (limit > 1) && (i % limit) == 0))
		{
		  rl_crlf ();
		  lines++;
		  if (_rl_page_completions && lines >= _rl_screenheight - 1)
		    {
		      lines = _rl_internal_pager (lines);
		      if (lines < 0)
			return;
		    }
		}
	      else if (max <= printed_len)
		putc (' ', rl_outstream);
	      else
		for (k = 0; k < max - printed_len; k++)
		  putc (' ', rl_outstream);
	    }
	}
      rl_crlf ();
    }
}

/* Display MATCHES, a list of matching filenames in argv format.  This
   handles the simple case -- a single match -- first.  If there is more
   than one match, we compute the number of strings in the list and the
   length of the longest string, which will be needed by the display
   function.  If the application wants to handle displaying the list of
   matches itself, it sets RL_COMPLETION_DISPLAY_MATCHES_HOOK to the
   address of a function, and we just call it.  If we're handling the
   display ourselves, we just call rl_display_match_list.  We also check
   that the list of matches doesn't exceed the user-settable threshold,
   and ask the user if he wants to see the list if there are more matches
   than RL_COMPLETION_QUERY_ITEMS. */
static void
display_matches (char **matches)
{
  int len, max, i;
  char *temp;

  /* Move to the last visible line of a possibly-multiple-line command. */
  _rl_move_vert (_rl_vis_botlin);

  /* Handle simple case first.  What if there is only one answer? */
  if (matches[1] == 0)
    {
      temp = printable_part (matches[0]);
      rl_crlf ();
      print_filename (temp, matches[0], 0);
      rl_crlf ();

      rl_forced_update_display ();
      rl_display_fixed = 1;

      return;
    }

  /* There is more than one answer.  Find out how many there are,
     and find the maximum printed length of a single entry. */
  for (max = 0, i = 1; matches[i]; i++)
    {
      temp = printable_part (matches[i]);
      len = fnwidth (temp);

      if (len > max)
	max = len;
    }

  len = i - 1;

  /* If the caller has defined a display hook, then call that now. */
  if (rl_completion_display_matches_hook)
    {
      (*rl_completion_display_matches_hook) (matches, len, max);
      return;
    }
	
  /* If there are many items, then ask the user if she really wants to
     see them all. */
  if (rl_completion_query_items > 0 && len >= rl_completion_query_items)
    {
      rl_crlf ();
      fprintf (rl_outstream, "Display all %d possibilities? (y or n)", len);
      fflush (rl_outstream);
      if ((completion_y_or_n = get_y_or_n (0)) == 0)
	{
	  rl_crlf ();

	  rl_forced_update_display ();
	  rl_display_fixed = 1;

	  return;
	}
    }

  rl_display_match_list (matches, len, max);

  rl_forced_update_display ();
  rl_display_fixed = 1;
}

/* qc == pointer to quoting character, if any */
static char *
make_quoted_replacement (char *match, int mtype, char *qc)
{
  int should_quote, do_replace;
  char *replacement;

  /* If we are doing completion on quoted substrings, and any matches
     contain any of the completer_word_break_characters, then auto-
     matically prepend the substring with a quote character (just pick
     the first one from the list of such) if it does not already begin
     with a quote string.  FIXME: Need to remove any such automatically
     inserted quote character when it no longer is necessary, such as
     if we change the string we are completing on and the new set of
     matches don't require a quoted substring. */
  replacement = match;

  should_quote = match && rl_completer_quote_characters &&
			rl_filename_completion_desired &&
			rl_filename_quoting_desired;

  if (should_quote)
    should_quote = should_quote && (!qc || !*qc ||
		     (rl_completer_quote_characters && strchr (rl_completer_quote_characters, *qc)));

  if (should_quote)
    {
      /* If there is a single match, see if we need to quote it.
         This also checks whether the common prefix of several
	 matches needs to be quoted. */
      should_quote = rl_filename_quote_characters
			? (_rl_strpbrk (match, rl_filename_quote_characters) != 0)
			: 0;

      do_replace = should_quote ? mtype : NO_MATCH;
      /* Quote the replacement, since we found an embedded
	 word break character in a potential match. */
      if (do_replace != NO_MATCH && rl_filename_quoting_function)
	replacement = (*rl_filename_quoting_function) (match, do_replace, qc);
    }
  return (replacement);
}

static void
insert_match (char *match, int start, int mtype, char *qc)
{
  char *replacement, *r;
  char oqc;
  int end, rlen;

  oqc = qc ? *qc : '\0';
  replacement = make_quoted_replacement (match, mtype, qc);

  /* Now insert the match. */
  if (replacement)
    {
      rlen = strlen (replacement);
      /* Don't double an opening quote character. */
      if (qc && *qc && start && rl_line_buffer[start - 1] == *qc &&
	    replacement[0] == *qc)
	start--;
      /* If make_quoted_replacement changed the quoting character, remove
	 the opening quote and insert the (fully-quoted) replacement. */
      else if (qc && (*qc != oqc) && start && rl_line_buffer[start - 1] == oqc &&
	    replacement[0] != oqc)
	start--;
      end = rl_point - 1;
      /* Don't double a closing quote character */
      if (qc && *qc && end && rl_line_buffer[rl_point] == *qc && replacement[rlen - 1] == *qc)
        end++;
      if (_rl_skip_completed_text)
	{
	  r = replacement;
	  while (start < rl_end && *r && rl_line_buffer[start] == *r)
	    {
	      start++;
	      r++;
	    }
	  if (start <= end || *r)
	    _rl_replace_text (r, start, end);
	  rl_point = start + strlen (r);
	}
      else
	_rl_replace_text (replacement, start, end);
      if (replacement != match)
        xfree (replacement);
    }
}

/* Append any necessary closing quote and a separator character to the
   just-inserted match.  If the user has specified that directories
   should be marked by a trailing `/', append one of those instead.  The
   default trailing character is a space.  Returns the number of characters
   appended.  If NONTRIVIAL_MATCH is set, we test for a symlink (if the OS
   has them) and don't add a suffix for a symlink to a directory.  A
   nontrivial match is one that actually adds to the word being completed.
   The variable rl_completion_mark_symlink_dirs controls this behavior
   (it's initially set to the what the user has chosen, indicated by the
   value of _rl_complete_mark_symlink_dirs, but may be modified by an
   application's completion function). */
static int
append_to_match (char *text, int delimiter, int quote_char, int nontrivial_match)
{
  char temp_string[4], *filename, *fn;
  int temp_string_index, s;
  struct stat finfo;

  temp_string_index = 0;
  if (quote_char && rl_point && rl_completion_suppress_quote == 0 &&
      rl_line_buffer[rl_point - 1] != quote_char)
    temp_string[temp_string_index++] = quote_char;

  if (delimiter)
    temp_string[temp_string_index++] = delimiter;
  else if (rl_completion_suppress_append == 0 && rl_completion_append_character)
    temp_string[temp_string_index++] = rl_completion_append_character;

  temp_string[temp_string_index++] = '\0';

  if (rl_filename_completion_desired)
    {
      filename = tilde_expand (text);
      if (rl_filename_stat_hook)
        {
          fn = savestring (filename);
	  (*rl_filename_stat_hook) (&fn);
	  xfree (filename);
	  filename = fn;
        }
      s = (nontrivial_match && rl_completion_mark_symlink_dirs == 0)
		? LSTAT (filename, &finfo)
		: stat (filename, &finfo);
      if (s == 0 && S_ISDIR (finfo.st_mode))
	{
	  if (_rl_complete_mark_directories /* && rl_completion_suppress_append == 0 */)
	    {
	      /* This is clumsy.  Avoid putting in a double slash if point
		 is at the end of the line and the previous character is a
		 slash. */
	      if (rl_point && rl_line_buffer[rl_point] == '\0' && rl_line_buffer[rl_point - 1] == '/')
		;
	      else if (rl_line_buffer[rl_point] != '/')
		rl_insert_text ("/");
	    }
	}
#ifdef S_ISLNK
      /* Don't add anything if the filename is a symlink and resolves to a
	 directory. */
      else if (s == 0 && S_ISLNK (finfo.st_mode) && path_isdir (filename))
	;
#endif
      else
	{
	  if (rl_point == rl_end && temp_string_index)
	    rl_insert_text (temp_string);
	}
      xfree (filename);
    }
  else
    {
      if (rl_point == rl_end && temp_string_index)
	rl_insert_text (temp_string);
    }

  return (temp_string_index);
}

static void
insert_all_matches (char **matches, int point, char *qc)
{
  int i;
  char *rp;

  rl_begin_undo_group ();
  /* remove any opening quote character; make_quoted_replacement will add
     it back. */
  if (qc && *qc && point && rl_line_buffer[point - 1] == *qc)
    point--;
  rl_delete_text (point, rl_point);
  rl_point = point;

  if (matches[1])
    {
      for (i = 1; matches[i]; i++)
	{
	  rp = make_quoted_replacement (matches[i], SINGLE_MATCH, qc);
	  rl_insert_text (rp);
	  rl_insert_text (" ");
	  if (rp != matches[i])
	    xfree (rp);
	}
    }
  else
    {
      rp = make_quoted_replacement (matches[0], SINGLE_MATCH, qc);
      rl_insert_text (rp);
      rl_insert_text (" ");
      if (rp != matches[0])
	xfree (rp);
    }
  rl_end_undo_group ();
}

void
_rl_free_match_list (char **matches)
{
  register int i;

  if (matches == 0)
    return;

  for (i = 0; matches[i]; i++)
    xfree (matches[i]);
  xfree (matches);
}

/* Compare a possibly-quoted filename TEXT from the line buffer and a possible
   MATCH that is the product of filename completion, which acts on the dequoted
   text. */
static int
compare_match (char *text, const char *match)
{
  char *temp;
  int r;

  if (rl_filename_completion_desired && rl_filename_quoting_desired && 
      rl_completion_found_quote && rl_filename_dequoting_function)
    {
      temp = (*rl_filename_dequoting_function) (text, rl_completion_quote_character);
      r = strcmp (temp, match);
      xfree (temp);
      return r;
    }      
  return (strcmp (text, match));
}

/* Complete the word at or before point.
   WHAT_TO_DO says what to do with the completion.
   `?' means list the possible completions.
   TAB means do standard completion.
   `*' means insert all of the possible completions.
   `!' means to do standard completion, and list all possible completions if
   there is more than one.
   `@' means to do standard completion, and list all possible completions if
   there is more than one and partial completion is not possible. */
int
rl_complete_internal (int what_to_do)
{
  char **matches;
  rl_compentry_func_t *our_func;
  int start, end, delimiter, found_quote, i, nontrivial_lcd;
  char *text, *saved_line_buffer;
  char quote_char;
  int tlen, mlen, saved_last_completion_failed;

  RL_SETSTATE(RL_STATE_COMPLETING);

  saved_last_completion_failed = last_completion_failed;

  set_completion_defaults (what_to_do);

  saved_line_buffer = rl_line_buffer ? savestring (rl_line_buffer) : (char *)NULL;
  our_func = rl_completion_entry_function
		? rl_completion_entry_function
		: rl_filename_completion_function;
  /* We now look backwards for the start of a filename/variable word. */
  end = rl_point;
  found_quote = delimiter = 0;
  quote_char = '\0';

  if (rl_point)
    /* This (possibly) changes rl_point.  If it returns a non-zero char,
       we know we have an open quote. */
    quote_char = _rl_find_completion_word (&found_quote, &delimiter);

  start = rl_point;
  rl_point = end;

  text = rl_copy_text (start, end);
  matches = gen_completion_matches (text, start, end, our_func, found_quote, quote_char);
  /* nontrivial_lcd is set if the common prefix adds something to the word
     being completed. */
  nontrivial_lcd = matches && compare_match (text, matches[0]) != 0;
  if (what_to_do == '!' || what_to_do == '@')
    tlen = strlen (text);
  xfree (text);

  if (matches == 0)
    {
      rl_ding ();
      FREE (saved_line_buffer);
      completion_changed_buffer = 0;
      last_completion_failed = 1;
      RL_UNSETSTATE(RL_STATE_COMPLETING);
      _rl_reset_completion_state ();
      return (0);
    }

  /* If we are matching filenames, the attempted completion function will
     have set rl_filename_completion_desired to a non-zero value.  The basic
     rl_filename_completion_function does this. */
  i = rl_filename_completion_desired;

  if (postprocess_matches (&matches, i) == 0)
    {
      rl_ding ();
      FREE (saved_line_buffer);
      completion_changed_buffer = 0;
      last_completion_failed = 1;
      RL_UNSETSTATE(RL_STATE_COMPLETING);
      _rl_reset_completion_state ();
      return (0);
    }

  if (matches && matches[0] && *matches[0])
    last_completion_failed = 0;

  switch (what_to_do)
    {
    case TAB:
    case '!':
    case '@':
      /* Insert the first match with proper quoting. */
      if (what_to_do == TAB)
        {
          if (*matches[0])
	    insert_match (matches[0], start, matches[1] ? MULT_MATCH : SINGLE_MATCH, &quote_char);
        }
      else if (*matches[0] && matches[1] == 0)
	/* should we perform the check only if there are multiple matches? */
	insert_match (matches[0], start, matches[1] ? MULT_MATCH : SINGLE_MATCH, &quote_char);
      else if (*matches[0])	/* what_to_do != TAB && multiple matches */
	{
	  mlen = *matches[0] ? strlen (matches[0]) : 0;
	  if (mlen >= tlen)
	    insert_match (matches[0], start, matches[1] ? MULT_MATCH : SINGLE_MATCH, &quote_char);
	}

      /* If there are more matches, ring the bell to indicate.
	 If we are in vi mode, Posix.2 says to not ring the bell.
	 If the `show-all-if-ambiguous' variable is set, display
	 all the matches immediately.  Otherwise, if this was the
	 only match, and we are hacking files, check the file to
	 see if it was a directory.  If so, and the `mark-directories'
	 variable is set, add a '/' to the name.  If not, and we
	 are at the end of the line, then add a space.  */
      if (matches[1])
	{
	  if (what_to_do == '!')
	    {
	      display_matches (matches);
	      break;
	    }
	  else if (what_to_do == '@')
	    {
	      if (nontrivial_lcd == 0)
		display_matches (matches);
	      break;
	    }
	  else if (rl_editing_mode != vi_mode)
	    rl_ding ();	/* There are other matches remaining. */
	}
      else
	append_to_match (matches[0], delimiter, quote_char, nontrivial_lcd);

      break;

    case '*':
      insert_all_matches (matches, start, &quote_char);
      break;

    case '?':
      /* Let's try to insert a single match here if the last completion failed
	 but this attempt returned a single match. */
      if (saved_last_completion_failed && matches[0] && *matches[0] && matches[1] == 0)
	{
	  insert_match (matches[0], start, matches[1] ? MULT_MATCH : SINGLE_MATCH, &quote_char);
	  append_to_match (matches[0], delimiter, quote_char, nontrivial_lcd);
	  break;
	}
      
      if (rl_completion_display_matches_hook == 0)
	{
	  _rl_sigcleanup = _rl_complete_sigcleanup;
	  _rl_sigcleanarg = matches;
	  _rl_complete_display_matches_interrupt = 0;
	}
      display_matches (matches);
      if (_rl_complete_display_matches_interrupt)
        {
          matches = 0;		/* already freed by rl_complete_sigcleanup */
          _rl_complete_display_matches_interrupt = 0;
	  if (rl_signal_event_hook)
	    (*rl_signal_event_hook) ();		/* XXX */
        }
      _rl_sigcleanup = 0;
      _rl_sigcleanarg = 0;
      break;

    default:
      _rl_ttymsg ("bad value %d for what_to_do in rl_complete", what_to_do);
      rl_ding ();
      FREE (saved_line_buffer);
      RL_UNSETSTATE(RL_STATE_COMPLETING);
      _rl_free_match_list (matches);
      _rl_reset_completion_state ();
      return 1;
    }

  _rl_free_match_list (matches);

  /* Check to see if the line has changed through all of this manipulation. */
  if (saved_line_buffer)
    {
      completion_changed_buffer = strcmp (rl_line_buffer, saved_line_buffer) != 0;
      xfree (saved_line_buffer);
    }

  RL_UNSETSTATE(RL_STATE_COMPLETING);
  _rl_reset_completion_state ();

  RL_CHECK_SIGNALS ();
  return 0;
}

/***************************************************************/
/*							       */
/*  Application-callable completion match generator functions  */
/*							       */
/***************************************************************/

/* Return an array of (char *) which is a list of completions for TEXT.
   If there are no completions, return a NULL pointer.
   The first entry in the returned array is the substitution for TEXT.
   The remaining entries are the possible completions.
   The array is terminated with a NULL pointer.

   ENTRY_FUNCTION is a function of two args, and returns a (char *).
     The first argument is TEXT.
     The second is a state argument; it should be zero on the first call, and
     non-zero on subsequent calls.  It returns a NULL pointer to the caller
     when there are no more matches.
 */
char **
rl_completion_matches (const char *text, rl_compentry_func_t *entry_function)
{
  register int i;

  /* Number of slots in match_list. */
  int match_list_size;

  /* The list of matches. */
  char **match_list;

  /* Number of matches actually found. */
  int matches;

  /* Temporary string binder. */
  char *string;

  matches = 0;
  match_list_size = 10;
  match_list = (char **)xmalloc ((match_list_size + 1) * sizeof (char *));
  match_list[1] = (char *)NULL;

  while (string = (*entry_function) (text, matches))
    {
      if (RL_SIG_RECEIVED ())
	{
	  /* Start at 1 because we don't set matches[0] in this function.
	     Only free the list members if we're building match list from
	     rl_filename_completion_function, since we know that doesn't
	     free the strings it returns. */
	  if (entry_function == rl_filename_completion_function)
	    {
	      for (i = 1; match_list[i]; i++)
		xfree (match_list[i]);
	    }
	  xfree (match_list);
	  match_list = 0;
	  match_list_size = 0;
	  matches = 0;
	  RL_CHECK_SIGNALS ();
	}

      if (matches + 1 >= match_list_size)
	match_list = (char **)xrealloc
	  (match_list, ((match_list_size += 10) + 1) * sizeof (char *));

      if (match_list == 0)
	return (match_list);

      match_list[++matches] = string;
      match_list[matches + 1] = (char *)NULL;
    }

  /* If there were any matches, then look through them finding out the
     lowest common denominator.  That then becomes match_list[0]. */
  if (matches)
    compute_lcd_of_matches (match_list, matches, text);
  else				/* There were no matches. */
    {
      xfree (match_list);
      match_list = (char **)NULL;
    }
  return (match_list);
}

/* A completion function for usernames.
   TEXT contains a partial username preceded by a random
   character (usually `~').  */
char *
rl_username_completion_function (const char *text, int state)
{
#if defined (__WIN32__) || defined (__OPENNT)
  return (char *)NULL;
#else /* !__WIN32__ && !__OPENNT) */
  static char *username = (char *)NULL;
  static struct passwd *entry;
  static int namelen, first_char, first_char_loc;
  char *value;

  if (state == 0)
    {
      FREE (username);

      first_char = *text;
      first_char_loc = first_char == '~';

      username = savestring (&text[first_char_loc]);
      namelen = strlen (username);
#if defined (HAVE_GETPWENT)
      setpwent ();
#endif
    }

#if defined (HAVE_GETPWENT)
  while (entry = getpwent ())
    {
      /* Null usernames should result in all users as possible completions. */
      if (namelen == 0 || (STREQN (username, entry->pw_name, namelen)))
	break;
    }
#endif

  if (entry == 0)
    {
#if defined (HAVE_GETPWENT)
      endpwent ();
#endif
      return ((char *)NULL);
    }
  else
    {
      value = (char *)xmalloc (2 + strlen (entry->pw_name));

      *value = *text;

      strcpy (value + first_char_loc, entry->pw_name);

      if (first_char == '~')
	rl_filename_completion_desired = 1;

      return (value);
    }
#endif /* !__WIN32__ && !__OPENNT */
}

/* Return non-zero if CONVFN matches FILENAME up to the length of FILENAME
   (FILENAME_LEN).  If _rl_completion_case_fold is set, compare without
   regard to the alphabetic case of characters.  If
   _rl_completion_case_map is set, make `-' and `_' equivalent.  CONVFN is
   the possibly-converted directory entry; FILENAME is what the user typed. */
static int
complete_fncmp (const char *convfn, int convlen, const char *filename, int filename_len)
{
  register char *s1, *s2;
  int d, len;
#if defined (HANDLE_MULTIBYTE)
  size_t v1, v2;
  mbstate_t ps1, ps2;
  WCHAR_T wc1, wc2;
#endif

#if defined (HANDLE_MULTIBYTE)
  memset (&ps1, 0, sizeof (mbstate_t));
  memset (&ps2, 0, sizeof (mbstate_t));
#endif

  if (filename_len == 0)
    return 1;
  if (convlen < filename_len)
    return 0;

  len = filename_len;
  s1 = (char *)convfn;
  s2 = (char *)filename;

  /* Otherwise, if these match up to the length of filename, then
     it is a match. */
  if (_rl_completion_case_fold && _rl_completion_case_map)
    {
      /* Case-insensitive comparison treating _ and - as equivalent */
#if defined (HANDLE_MULTIBYTE)
      if (MB_CUR_MAX > 1 && rl_byte_oriented == 0)
	{
	  do
	    {
	      v1 = MBRTOWC (&wc1, s1, convlen, &ps1);
	      v2 = MBRTOWC (&wc2, s2, filename_len, &ps2);
	      if (v1 == 0 && v2 == 0)
		return 1;
	      else if (MB_INVALIDCH (v1) || MB_INVALIDCH (v2))
		{
		  if (*s1 != *s2)		/* do byte comparison */
		    return 0;
		  else if ((*s1 == '-' || *s1 == '_') && (*s2 == '-' || *s2 == '_'))
		    return 0;
		  s1++; s2++; len--;
		  continue;
		}
	      wc1 = towlower (wc1);
	      wc2 = towlower (wc2);
	      s1 += v1;
	      s2 += v1;
	      len -= v1;
	      if ((wc1 == L'-' || wc1 == L'_') && (wc2 == L'-' || wc2 == L'_'))
	        continue;
	      if (wc1 != wc2)
		return 0;
	    }
	  while (len != 0);
	}
      else
#endif
	{
	do
	  {
	    d = _rl_to_lower (*s1) - _rl_to_lower (*s2);
	    /* *s1 == [-_] && *s2 == [-_] */
	    if ((*s1 == '-' || *s1 == '_') && (*s2 == '-' || *s2 == '_'))
	      d = 0;
	    if (d != 0)
	      return 0;
	    s1++; s2++;	/* already checked convlen >= filename_len */
	  }
	while (--len != 0);
	}

      return 1;
    }
  else if (_rl_completion_case_fold)
    {
#if defined (HANDLE_MULTIBYTE)
      if (MB_CUR_MAX > 1 && rl_byte_oriented == 0)
	{
	  do
	    {
	      v1 = MBRTOWC (&wc1, s1, convlen, &ps1);
	      v2 = MBRTOWC (&wc2, s2, filename_len, &ps2);
	      if (v1 == 0 && v2 == 0)
		return 1;
	      else if (MB_INVALIDCH (v1) || MB_INVALIDCH (v2))
		{
		  if (*s1 != *s2)		/* do byte comparison */
		    return 0;
		  s1++; s2++; len--;
		  continue;
		}
	      wc1 = towlower (wc1);
	      wc2 = towlower (wc2);
	      if (wc1 != wc2)
		return 0;
	      s1 += v1;
	      s2 += v1;
	      len -= v1;
	    }
	  while (len != 0);
	  return 1;
	}
      else
#endif
      if ((_rl_to_lower (convfn[0]) == _rl_to_lower (filename[0])) &&
	  (convlen >= filename_len) &&
	  (_rl_strnicmp (filename, convfn, filename_len) == 0))
	return 1;
    }
  else
    {
      if ((convfn[0] == filename[0]) &&
	  (convlen >= filename_len) &&
	  (strncmp (filename, convfn, filename_len) == 0))
	return 1;
    }
  return 0;
}

/* Okay, now we write the entry_function for filename completion.  In the
   general case.  Note that completion in the shell is a little different
   because of all the pathnames that must be followed when looking up the
   completion for a command. */
char *
rl_filename_completion_function (const char *text, int state)
{
  static DIR *directory = (DIR *)NULL;
  static char *filename = (char *)NULL;
  static char *dirname = (char *)NULL;
  static char *users_dirname = (char *)NULL;
  static int filename_len;
  char *temp, *dentry, *convfn;
  int dirlen, dentlen, convlen;
  int tilde_dirname;
  struct dirent *entry;

  /* If we don't have any state, then do some initialization. */
  if (state == 0)
    {
      /* If we were interrupted before closing the directory or reading
	 all of its contents, close it. */
      if (directory)
	{
	  closedir (directory);
	  directory = (DIR *)NULL;
	}
      FREE (dirname);
      FREE (filename);
      FREE (users_dirname);

      filename = savestring (text);
      if (*text == 0)
	text = ".";
      dirname = savestring (text);

      temp = strrchr (dirname, '/');

#if defined (__MSDOS__) || defined (_WIN32)
      /* special hack for //X/... */
      if (dirname[0] == '/' && dirname[1] == '/' && ISALPHA ((unsigned char)dirname[2]) && dirname[3] == '/')
        temp = strrchr (dirname + 3, '/');
#endif

      if (temp)
	{
	  strcpy (filename, ++temp);
	  *temp = '\0';
	}
#if defined (__MSDOS__) || (defined (_WIN32) && !defined (__CYGWIN__))
      /* searches from current directory on the drive */
      else if (ISALPHA ((unsigned char)dirname[0]) && dirname[1] == ':')
        {
          strcpy (filename, dirname + 2);
          dirname[2] = '\0';
        }
#endif
      else
	{
	  dirname[0] = '.';
	  dirname[1] = '\0';
	}

      /* We aren't done yet.  We also support the "~user" syntax. */

      /* Save the version of the directory that the user typed, dequoting
	 it if necessary. */
      if (rl_completion_found_quote && rl_filename_dequoting_function)
	users_dirname = (*rl_filename_dequoting_function) (dirname, rl_completion_quote_character);
      else
	users_dirname = savestring (dirname);

      tilde_dirname = 0;
      if (*dirname == '~')
	{
	  temp = tilde_expand (dirname);
	  xfree (dirname);
	  dirname = temp;
	  tilde_dirname = 1;
	}

      /* We have saved the possibly-dequoted version of the directory name
	 the user typed.  Now transform the directory name we're going to
	 pass to opendir(2).  The directory rewrite hook modifies only the
	 directory name; the directory completion hook modifies both the
	 directory name passed to opendir(2) and the version the user
	 typed.  Both the directory completion and rewrite hooks should perform
	 any necessary dequoting.  The hook functions return 1 if they modify
	 the directory name argument.  If either hook returns 0, it should
	 not modify the directory name pointer passed as an argument. */
      if (rl_directory_rewrite_hook)
	(*rl_directory_rewrite_hook) (&dirname);
      else if (rl_directory_completion_hook && (*rl_directory_completion_hook) (&dirname))
	{
	  xfree (users_dirname);
	  users_dirname = savestring (dirname);
	}
      else if (tilde_dirname == 0 && rl_completion_found_quote && rl_filename_dequoting_function)
	{
	  /* delete single and double quotes */
	  xfree (dirname);
	  dirname = savestring (users_dirname);
	}
      directory = opendir (dirname);

      /* Now dequote a non-null filename.  FILENAME will not be NULL, but may
	 be empty. */
      if (*filename && rl_completion_found_quote && rl_filename_dequoting_function)
	{
	  /* delete single and double quotes */
	  temp = (*rl_filename_dequoting_function) (filename, rl_completion_quote_character);
	  xfree (filename);
	  filename = temp;
	}
      filename_len = strlen (filename);

      rl_filename_completion_desired = 1;
    }

  /* At this point we should entertain the possibility of hacking wildcarded
     filenames, like /usr/man/man<WILD>/te<TAB>.  If the directory name
     contains globbing characters, then build an array of directories, and
     then map over that list while completing. */
  /* *** UNIMPLEMENTED *** */

  /* Now that we have some state, we can read the directory. */

  entry = (struct dirent *)NULL;
  while (directory && (entry = readdir (directory)))
    {
      convfn = dentry = entry->d_name;
      convlen = dentlen = D_NAMLEN (entry);

      if (rl_filename_rewrite_hook)
	{
	  convfn = (*rl_filename_rewrite_hook) (dentry, dentlen);
	  convlen = (convfn == dentry) ? dentlen : strlen (convfn);
	}

      /* Special case for no filename.  If the user has disabled the
         `match-hidden-files' variable, skip filenames beginning with `.'.
	 All other entries except "." and ".." match. */
      if (filename_len == 0)
	{
	  if (_rl_match_hidden_files == 0 && HIDDEN_FILE (convfn))
	    continue;

	  if (convfn[0] != '.' ||
	       (convfn[1] && (convfn[1] != '.' || convfn[2])))
	    break;
	}
      else
	{
	  if (complete_fncmp (convfn, convlen, filename, filename_len))
	    break;
	}
    }

  if (entry == 0)
    {
      if (directory)
	{
	  closedir (directory);
	  directory = (DIR *)NULL;
	}
      if (dirname)
	{
	  xfree (dirname);
	  dirname = (char *)NULL;
	}
      if (filename)
	{
	  xfree (filename);
	  filename = (char *)NULL;
	}
      if (users_dirname)
	{
	  xfree (users_dirname);
	  users_dirname = (char *)NULL;
	}

      return (char *)NULL;
    }
  else
    {
      /* dirname && (strcmp (dirname, ".") != 0) */
      if (dirname && (dirname[0] != '.' || dirname[1]))
	{
	  if (rl_complete_with_tilde_expansion && *users_dirname == '~')
	    {
	      dirlen = strlen (dirname);
	      temp = (char *)xmalloc (2 + dirlen + D_NAMLEN (entry));
	      strcpy (temp, dirname);
	      /* Canonicalization cuts off any final slash present.  We
		 may need to add it back. */
	      if (dirname[dirlen - 1] != '/')
	        {
	          temp[dirlen++] = '/';
	          temp[dirlen] = '\0';
	        }
	    }
	  else
	    {
	      dirlen = strlen (users_dirname);
	      temp = (char *)xmalloc (2 + dirlen + D_NAMLEN (entry));
	      strcpy (temp, users_dirname);
	      /* Make sure that temp has a trailing slash here. */
	      if (users_dirname[dirlen - 1] != '/')
		temp[dirlen++] = '/';
	    }

	  strcpy (temp + dirlen, convfn);
	}
      else
	temp = savestring (convfn);

      if (convfn != dentry)
	xfree (convfn);

      return (temp);
    }
}

/* An initial implementation of a menu completion function a la tcsh.  The
   first time (if the last readline command was not rl_old_menu_complete), we
   generate the list of matches.  This code is very similar to the code in
   rl_complete_internal -- there should be a way to combine the two.  Then,
   for each item in the list of matches, we insert the match in an undoable
   fashion, with the appropriate character appended (this happens on the
   second and subsequent consecutive calls to rl_old_menu_complete).  When we
   hit the end of the match list, we restore the original unmatched text,
   ring the bell, and reset the counter to zero. */
int
rl_old_menu_complete (int count, int invoking_key)
{
  rl_compentry_func_t *our_func;
  int matching_filenames, found_quote;

  static char *orig_text;
  static char **matches = (char **)0;
  static int match_list_index = 0;
  static int match_list_size = 0;
  static int orig_start, orig_end;
  static char quote_char;
  static int delimiter;

  /* The first time through, we generate the list of matches and set things
     up to insert them. */
  if (rl_last_func != rl_old_menu_complete)
    {
      /* Clean up from previous call, if any. */
      FREE (orig_text);
      if (matches)
	_rl_free_match_list (matches);

      match_list_index = match_list_size = 0;
      matches = (char **)NULL;

      rl_completion_invoking_key = invoking_key;

      RL_SETSTATE(RL_STATE_COMPLETING);

      /* Only the completion entry function can change these. */
      set_completion_defaults ('%');

      our_func = rl_menu_completion_entry_function;
      if (our_func == 0)
	our_func = rl_completion_entry_function
			? rl_completion_entry_function
			: rl_filename_completion_function;

      /* We now look backwards for the start of a filename/variable word. */
      orig_end = rl_point;
      found_quote = delimiter = 0;
      quote_char = '\0';

      if (rl_point)
	/* This (possibly) changes rl_point.  If it returns a non-zero char,
	   we know we have an open quote. */
	quote_char = _rl_find_completion_word (&found_quote, &delimiter);

      orig_start = rl_point;
      rl_point = orig_end;

      orig_text = rl_copy_text (orig_start, orig_end);
      matches = gen_completion_matches (orig_text, orig_start, orig_end,
					our_func, found_quote, quote_char);

      /* If we are matching filenames, the attempted completion function will
	 have set rl_filename_completion_desired to a non-zero value.  The basic
	 rl_filename_completion_function does this. */
      matching_filenames = rl_filename_completion_desired;

      if (matches == 0 || postprocess_matches (&matches, matching_filenames) == 0)
	{
	  rl_ding ();
	  FREE (matches);
	  matches = (char **)0;
	  FREE (orig_text);
	  orig_text = (char *)0;
	  completion_changed_buffer = 0;
	  RL_UNSETSTATE(RL_STATE_COMPLETING);
	  return (0);
	}

      RL_UNSETSTATE(RL_STATE_COMPLETING);

      for (match_list_size = 0; matches[match_list_size]; match_list_size++)
        ;
      /* matches[0] is lcd if match_list_size > 1, but the circular buffer
	 code below should take care of it. */

      if (match_list_size > 1 && _rl_complete_show_all)
	display_matches (matches);
    }

  /* Now we have the list of matches.  Replace the text between
     rl_line_buffer[orig_start] and rl_line_buffer[rl_point] with
     matches[match_list_index], and add any necessary closing char. */

  if (matches == 0 || match_list_size == 0) 
    {
      rl_ding ();
      FREE (matches);
      matches = (char **)0;
      completion_changed_buffer = 0;
      return (0);
    }

  match_list_index += count;
  if (match_list_index < 0)
    {
      while (match_list_index < 0)
	match_list_index += match_list_size;
    }
  else
    match_list_index %= match_list_size;

  if (match_list_index == 0 && match_list_size > 1)
    {
      rl_ding ();
      insert_match (orig_text, orig_start, MULT_MATCH, &quote_char);
    }
  else
    {
      insert_match (matches[match_list_index], orig_start, SINGLE_MATCH, &quote_char);
      append_to_match (matches[match_list_index], delimiter, quote_char,
		       compare_match (orig_text, matches[match_list_index]));
    }

  completion_changed_buffer = 1;
  return (0);
}

/* The current version of menu completion.
   The differences between this function and the original are:

1. It honors the maximum number of completions variable (completion-query-items)
2. It appends to the word as usual if there is only one match
3. It displays the common prefix if there is one, and makes it the first menu
   choice if the menu-complete-display-prefix option is enabled
*/
 
int
rl_menu_complete (int count, int ignore)
{
  rl_compentry_func_t *our_func;
  int matching_filenames, found_quote;

  static char *orig_text;
  static char **matches = (char **)0;
  static int match_list_index = 0;
  static int match_list_size = 0;
  static int nontrivial_lcd = 0;
  static int full_completion = 0;	/* set to 1 if menu completion should reinitialize on next call */
  static int orig_start, orig_end;
  static char quote_char;
  static int delimiter, cstate;

  /* The first time through, we generate the list of matches and set things
     up to insert them. */
  if ((rl_last_func != rl_menu_complete && rl_last_func != rl_backward_menu_complete) || full_completion)
    {
      /* Clean up from previous call, if any. */
      FREE (orig_text);
      if (matches)
	_rl_free_match_list (matches);

      match_list_index = match_list_size = 0;
      matches = (char **)NULL;

      full_completion = 0;

      RL_SETSTATE(RL_STATE_COMPLETING);

      /* Only the completion entry function can change these. */
      set_completion_defaults ('%');

      our_func = rl_menu_completion_entry_function;
      if (our_func == 0)
	our_func = rl_completion_entry_function
			? rl_completion_entry_function
			: rl_filename_completion_function;

      /* We now look backwards for the start of a filename/variable word. */
      orig_end = rl_point;
      found_quote = delimiter = 0;
      quote_char = '\0';

      if (rl_point)
	/* This (possibly) changes rl_point.  If it returns a non-zero char,
	   we know we have an open quote. */
	quote_char = _rl_find_completion_word (&found_quote, &delimiter);

      orig_start = rl_point;
      rl_point = orig_end;

      orig_text = rl_copy_text (orig_start, orig_end);
      matches = gen_completion_matches (orig_text, orig_start, orig_end,
					our_func, found_quote, quote_char);

      nontrivial_lcd = matches && compare_match (orig_text, matches[0]) != 0;

      /* If we are matching filenames, the attempted completion function will
	 have set rl_filename_completion_desired to a non-zero value.  The basic
	 rl_filename_completion_function does this. */
      matching_filenames = rl_filename_completion_desired;

      if (matches == 0 || postprocess_matches (&matches, matching_filenames) == 0)
	{
	  rl_ding ();
	  FREE (matches);
	  matches = (char **)0;
	  FREE (orig_text);
	  orig_text = (char *)0;
	  completion_changed_buffer = 0;
	  RL_UNSETSTATE(RL_STATE_COMPLETING);
	  return (0);
	}

      RL_UNSETSTATE(RL_STATE_COMPLETING);

      for (match_list_size = 0; matches[match_list_size]; match_list_size++)
        ;

      if (match_list_size == 0) 
	{
	  rl_ding ();
	  FREE (matches);
	  matches = (char **)0;
	  match_list_index = 0;
	  completion_changed_buffer = 0;
	  return (0);
        }

      /* matches[0] is lcd if match_list_size > 1, but the circular buffer
	 code below should take care of it. */
      if (*matches[0])
	{
	  insert_match (matches[0], orig_start, matches[1] ? MULT_MATCH : SINGLE_MATCH, &quote_char);
	  orig_end = orig_start + strlen (matches[0]);
	  completion_changed_buffer = STREQ (orig_text, matches[0]) == 0;
	}

      if (match_list_size > 1 && _rl_complete_show_all)
	{
	  display_matches (matches);
	  /* If there are so many matches that the user has to be asked
	     whether or not he wants to see the matches, menu completion
	     is unwieldy. */
	  if (rl_completion_query_items > 0 && match_list_size >= rl_completion_query_items)
	    {
	      rl_ding ();
	      FREE (matches);
	      matches = (char **)0;
	      full_completion = 1;
	      return (0);
	    }
	  else if (_rl_menu_complete_prefix_first)
	    {
	      rl_ding ();
	      return (0);
	    }
	}
      else if (match_list_size <= 1)
	{
	  append_to_match (matches[0], delimiter, quote_char, nontrivial_lcd);
	  full_completion = 1;
	  return (0);
	}
      else if (_rl_menu_complete_prefix_first && match_list_size > 1)
	{
	  rl_ding ();
	  return (0);
	}
    }

  /* Now we have the list of matches.  Replace the text between
     rl_line_buffer[orig_start] and rl_line_buffer[rl_point] with
     matches[match_list_index], and add any necessary closing char. */

  if (matches == 0 || match_list_size == 0) 
    {
      rl_ding ();
      FREE (matches);
      matches = (char **)0;
      completion_changed_buffer = 0;
      return (0);
    }

  match_list_index += count;
  if (match_list_index < 0)
    {
      while (match_list_index < 0)
	match_list_index += match_list_size;
    }
  else
    match_list_index %= match_list_size;

  if (match_list_index == 0 && match_list_size > 1)
    {
      rl_ding ();
      insert_match (matches[0], orig_start, MULT_MATCH, &quote_char);
    }
  else
    {
      insert_match (matches[match_list_index], orig_start, SINGLE_MATCH, &quote_char);
      append_to_match (matches[match_list_index], delimiter, quote_char,
		       compare_match (orig_text, matches[match_list_index]));
    }

  completion_changed_buffer = 1;
  return (0);
}

int
rl_backward_menu_complete (int count, int key)
{
  /* Positive arguments to backward-menu-complete translate into negative
     arguments for menu-complete, and vice versa. */
  return (rl_menu_complete (-count, key));
}
