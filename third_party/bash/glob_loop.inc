/* Copyright (C) 1991-2020 Free Software Foundation, Inc.

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

static int INTERNAL_GLOB_PATTERN_P PARAMS((const GCHAR *));

/* Return nonzero if PATTERN has any special globbing chars in it.
   Compiled twice, once each for single-byte and multibyte characters. */
static int
INTERNAL_GLOB_PATTERN_P (pattern)
     const GCHAR *pattern;
{
  register const GCHAR *p;
  register GCHAR c;
  int bopen, bsquote;

  p = pattern;
  bopen = bsquote = 0;

  while ((c = *p++) != L('\0'))
    switch (c)
      {
      case L('?'):
      case L('*'):
	return 1;

      case L('['):      /* Only accept an open brace if there is a close */
	bopen++;        /* brace to match it.  Bracket expressions must be */
	continue;       /* complete, according to Posix.2 */
      case L(']'):
	if (bopen)
	  return 1;
	continue;

      case L('+'):         /* extended matching operators */
      case L('@'):
      case L('!'):
	if (*p == L('('))  /*) */
	  return 1;
	continue;

      case L('\\'):
	/* Don't let the pattern end in a backslash (GMATCH returns no match
	   if the pattern ends in a backslash anyway), but otherwise note that 
	   we have seen this, since the matching engine uses backslash as an
	   escape character and it can be removed. We return 2 later if we
	   have seen only backslash-escaped characters, so interested callers
	   know they can shortcut and just dequote the pathname. */
	if (*p != L('\0'))
	  {
	    p++;
	    bsquote = 1;
	    continue;
	  }
	else 	/* (*p == L('\0')) */
	  return 0;
      }

#if 0
  return bsquote ? 2 : 0;
#else
  return (0);
#endif
}

#undef INTERNAL_GLOB_PATTERN_P
#undef L
#undef INT
#undef CHAR
#undef GCHAR
