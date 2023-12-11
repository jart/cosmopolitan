/* histlib.h -- internal definitions for the history library. */

/* Copyright (C) 1989-2009,2021-2022 Free Software Foundation, Inc.

   This file contains the GNU History Library (History), a set of
   routines for managing the text of previously typed lines.

   History is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   History is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with History.  If not, see <http://www.gnu.org/licenses/>.
*/

#if !defined (_HISTLIB_H_)
#define _HISTLIB_H_

#if defined (HAVE_STRING_H)
#  include <string.h>
#else
#  include <strings.h>
#endif /* !HAVE_STRING_H */

#if !defined (STREQ)
#define STREQ(a, b)	(((a)[0] == (b)[0]) && (strcmp ((a), (b)) == 0))
#define STREQN(a, b, n) (((n) == 0) ? (1) \
				    : ((a)[0] == (b)[0]) && (strncmp ((a), (b), (n)) == 0))
#endif

#ifndef savestring
#define savestring(x) strcpy (xmalloc (1 + strlen (x)), (x))
#endif

#ifndef whitespace
#define whitespace(c) (((c) == ' ') || ((c) == '\t'))
#endif

#ifndef _rl_digit_p
#define _rl_digit_p(c)  ((c) >= '0' && (c) <= '9')
#endif

#ifndef _rl_digit_value
#define _rl_digit_value(c) ((c) - '0')
#endif

#ifndef member
#  if !defined (strchr) && !defined (__STDC__)
extern char *strchr ();
#  endif /* !strchr && !__STDC__ */
#define member(c, s) ((c) ? ((char *)strchr ((s), (c)) != (char *)NULL) : 0)
#endif

#ifndef FREE
#  define FREE(x)	if (x) free (x)
#endif

/* Possible history errors passed to hist_error. */
#define EVENT_NOT_FOUND 0
#define BAD_WORD_SPEC	1
#define SUBST_FAILED	2
#define BAD_MODIFIER	3
#define NO_PREV_SUBST	4

/* Possible definitions for history starting point specification. */
#define NON_ANCHORED_SEARCH	0
#define ANCHORED_SEARCH		0x01
#define PATTERN_SEARCH		0x02

/* Possible definitions for what style of writing the history file we want. */
#define HISTORY_APPEND 0
#define HISTORY_OVERWRITE 1

/* internal extern function declarations used by other parts of the library */

/* histsearch.c */
extern int _hs_history_patsearch (const char *, int, int);

/* history.c */
extern void _hs_replace_history_data (int, histdata_t *, histdata_t *);
extern int _hs_at_end_of_history (void);

/* histfile.c */
extern void _hs_append_history_line (int, const char *);

#endif /* !_HISTLIB_H_ */
