/* strmatch.c -- ksh-like extended pattern matching for the shell and filename
		globbing. */

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

#include "config.h"

#include "stdc.h"
#include "strmatch.h"

extern int xstrmatch PARAMS((char *, char *, int));
#if defined (HANDLE_MULTIBYTE)
extern int internal_wstrmatch PARAMS((wchar_t *, wchar_t *, int));
#endif

int
strmatch (pattern, string, flags)
     char *pattern;
     char *string;
     int flags;
{
  if (string == 0 || pattern == 0)
    return FNM_NOMATCH;

  return (xstrmatch (pattern, string, flags));
}

#if defined (HANDLE_MULTIBYTE)
int
wcsmatch (wpattern, wstring, flags)
     wchar_t *wpattern;
     wchar_t *wstring;
     int flags;
{
  if (wstring == 0 || wpattern == 0)
    return (FNM_NOMATCH);

  return (internal_wstrmatch (wpattern, wstring, flags));
}
#endif

#ifdef TEST
main (c, v)
     int c;
     char **v;
{
  char *string, *pat;

  string = v[1];
  pat = v[2];

  if (strmatch (pat, string, 0) == 0)
    {
      printf ("%s matches %s\n", string, pat);
      exit (0);
    }
  else
    {
      printf ("%s does not match %s\n", string, pat);
      exit (1);
    }
}
#endif
