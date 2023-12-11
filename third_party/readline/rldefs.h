/* rldefs.h -- an attempt to isolate some of the system-specific defines
   for readline.  This should be included after any files that define
   system-specific constants like _POSIX_VERSION or USG. */

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

#if !defined (_RLDEFS_H_)
#define _RLDEFS_H_

#if defined (HAVE_CONFIG_H)
#  include "config.h"
#endif

#include "rlstdc.h"

#if defined (STRCOLL_BROKEN)
#  undef HAVE_STRCOLL
#endif

#if defined (_POSIX_VERSION) && !defined (TERMIOS_MISSING)
#  define TERMIOS_TTY_DRIVER
#else
#  if defined (HAVE_TERMIO_H)
#    define TERMIO_TTY_DRIVER
#  else
#    if !defined (__MINGW32__)
#      define NEW_TTY_DRIVER
#    else
#      define NO_TTY_DRIVER
#    endif
#  endif
#endif

/* Posix macro to check file in statbuf for directory-ness.
   This requires that <sys/stat.h> be included before this test. */
#if defined (S_IFDIR) && !defined (S_ISDIR)
#  define S_ISDIR(m) (((m)&S_IFMT) == S_IFDIR)
#endif

/* Decide which flavor of the header file describing the C library
   string functions to include and include it. */

#if defined (HAVE_STRING_H)
#  include <string.h>
#else /* !HAVE_STRING_H */
#  include <strings.h>
#endif /* !HAVE_STRING_H */

#if !defined (strchr) && !defined (__STDC__)
extern char *strchr (), *strrchr ();
#endif /* !strchr && !__STDC__ */

#if defined (PREFER_STDARG)
#  include <stdarg.h>
#else
#  if defined (PREFER_VARARGS)
#    include <varargs.h>
#  endif
#endif

#if defined (HAVE_STRCASECMP)
#define _rl_stricmp strcasecmp
#define _rl_strnicmp strncasecmp
#else
extern int _rl_stricmp (const char *, const char *);
extern int _rl_strnicmp (const char *, const char *, int);
#endif

#if defined (HAVE_STRPBRK) && !defined (HAVE_MULTIBYTE)
#  define _rl_strpbrk(a,b)	strpbrk((a),(b))
#else
extern char *_rl_strpbrk (const char *, const char *);
#endif

#if !defined (emacs_mode)
#  define no_mode -1
#  define vi_mode 0
#  define emacs_mode 1
#endif

#if !defined (RL_IM_INSERT)
#  define RL_IM_INSERT		1
#  define RL_IM_OVERWRITE	0
#
#  define RL_IM_DEFAULT		RL_IM_INSERT
#endif

/* If you cast map[key].function to type (Keymap) on a Cray,
   the compiler takes the value of map[key].function and
   divides it by 4 to convert between pointer types (pointers
   to functions and pointers to structs are different sizes).
   This is not what is wanted. */
#if defined (CRAY)
#  define FUNCTION_TO_KEYMAP(map, key)	(Keymap)((int)map[key].function)
#  define KEYMAP_TO_FUNCTION(data)	(rl_command_func_t *)((int)(data))
#else
#  define FUNCTION_TO_KEYMAP(map, key)	(Keymap)(map[key].function)
#  define KEYMAP_TO_FUNCTION(data)	(rl_command_func_t *)(data)
#endif

#ifndef savestring
#define savestring(x) strcpy ((char *)xmalloc (1 + strlen (x)), (x))
#endif

/* Possible values for _rl_bell_preference. */
#define NO_BELL 0
#define AUDIBLE_BELL 1
#define VISIBLE_BELL 2

/* Definitions used when searching the line for characters. */
/* NOTE: it is necessary that opposite directions are inverses */
#define	FTO	 1		/* forward to */
#define BTO	-1		/* backward to */
#define FFIND	 2		/* forward find */
#define BFIND	-2		/* backward find */

/* Possible values for the found_quote flags word used by the completion
   functions.  It says what kind of (shell-like) quoting we found anywhere
   in the line. */
#define RL_QF_SINGLE_QUOTE	0x01
#define RL_QF_DOUBLE_QUOTE	0x02
#define RL_QF_BACKSLASH		0x04
#define RL_QF_OTHER_QUOTE	0x08

/* Default readline line buffer length. */
#define DEFAULT_BUFFER_SIZE 256

#if !defined (STREQ)
#define STREQ(a, b)	(((a)[0] == (b)[0]) && (strcmp ((a), (b)) == 0))
#define STREQN(a, b, n)	(((n) == 0) ? (1) \
				    : ((a)[0] == (b)[0]) && (strncmp ((a), (b), (n)) == 0))
#endif

#if !defined (RL_STRLEN)
#  define RL_STRLEN(s) (((s) && (s)[0]) ? ((s)[1] ? ((s)[2] ? strlen(s) : 2) : 1) : 0)
#endif

#if !defined (FREE)
#  define FREE(x)	if (x) free (x)
#endif

#if !defined (SWAP)
#  define SWAP(s, e)  do { int t; t = s; s = e; e = t; } while (0)
#endif

/* CONFIGURATION SECTION */
#include "rlconf.h"

#endif /* !_RLDEFS_H_ */
