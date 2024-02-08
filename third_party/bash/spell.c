/* spell.c -- spelling correction for pathnames. */

/* Copyright (C) 2000-2020 Free Software Foundation, Inc.

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

#if defined (HAVE_UNISTD_H)
#  ifdef _MINIX
#    include <sys/types.h>
#  endif
#  include <unistd.h>
#endif

#include "bashtypes.h"
#include "posixdir.h"
#include "posixstat.h"
#if defined (HAVE_SYS_PARAM_H)
#include <sys/param.h>
#endif

#include <stdio.h>

#include "bashansi.h"
#include "maxpath.h"
#include "stdc.h"

static int mindist PARAMS((char *, char *, char *));
static int spdist PARAMS((char *, char *));

/*
 * `spname' and its helpers are inspired by the code in "The UNIX
 * Programming Environment", Kernighan & Pike, Prentice-Hall 1984,
 * pages 209 - 213.
 */

/*
 *	`spname' -- return a correctly spelled filename
 *
 *	int spname(char * oldname, char * newname)
 *	Returns:  -1 if no reasonable match found
 *		   0 if exact match found
 *		   1 if corrected
 *	Stores corrected name in `newname'.
 */
int
spname(oldname, newname)
     char *oldname;
     char *newname;
{
  char *op, *np, *p;
  char guess[PATH_MAX + 1], best[PATH_MAX + 1];

  op = oldname;
  np = newname;
  for (;;)
    {
      while (*op == '/')    /* Skip slashes */
	*np++ = *op++;
      *np = '\0';

      if (*op == '\0')    /* Exact or corrected */
	{
	  /* `.' is rarely the right thing. */
	  if (oldname[1] == '\0' && newname[1] == '\0' &&
		oldname[0] != '.' && newname[0] == '.')
	    return -1;
	  return strcmp(oldname, newname) != 0;
	}

      /* Copy next component into guess */
      for (p = guess; *op != '/' && *op != '\0'; op++)
	if (p < guess + PATH_MAX)
	  *p++ = *op;
      *p = '\0';

      if (mindist(newname, guess, best) >= 3)
	return -1;  /* Hopeless */

      /*
       *  Add to end of newname
       */
      for (p = best; *np = *p++; np++)
	;
    }
}

/*
 *  Search directory for a guess
 */
static int
mindist(dir, guess, best)
     char *dir;
     char *guess;
     char *best;
{
  DIR *fd;
  struct dirent *dp;
  int dist, x;

  dist = 3;    /* Worst distance */
  if (*dir == '\0')
    dir = ".";

  if ((fd = opendir(dir)) == NULL)
    return dist;

  while ((dp = readdir(fd)) != NULL)
    {
      /*
       *  Look for a better guess.  If the new guess is as
       *  good as the current one, we take it.  This way,
       *  any single character match will be a better match
       *  than ".".
       */
      x = spdist(dp->d_name, guess);
      if (x <= dist && x != 3)
	{
	  strcpy(best, dp->d_name);
	  dist = x;
	  if (dist == 0)    /* Exact match */
	    break;
	}
    }
  (void)closedir(fd);

  /* Don't return `.' */
  if (best[0] == '.' && best[1] == '\0')
    dist = 3;
  return dist;
}

/*
 *  `spdist' -- return the "distance" between two names.
 *
 *  int spname(char * oldname, char * newname)
 *  Returns:  0 if strings are identical
 *      1 if two characters are transposed
 *      2 if one character is wrong, added or deleted
 *      3 otherwise
 */
static int
spdist(cur, new)
     char *cur, *new;
{
  while (*cur == *new)
    {
      if (*cur == '\0')
	return 0;    /* Exact match */
      cur++;
      new++;
    }

  if (*cur)
    {
      if (*new)
	{
	  if (cur[1] && new[1] && cur[0] == new[1] && cur[1] == new[0] && strcmp (cur + 2, new + 2) == 0)
	    return 1;  /* Transposition */

	  if (strcmp (cur + 1, new + 1) == 0)
	    return 2;  /* One character mismatch */
	}

      if (strcmp(&cur[1], &new[0]) == 0)
	return 2;    /* Extra character */
    }

  if (*new && strcmp(cur, new + 1) == 0)
    return 2;      /* Missing character */

  return 3;
}

char *
dirspell (dirname)
     char *dirname;
{
  int n;
  char *guess;

  n = (strlen (dirname) * 3 + 1) / 2 + 1;
  guess = (char *)malloc (n);
  if (guess == 0)
    return 0;

  switch (spname (dirname, guess))
    {
    case -1:
    default:
      free (guess);
      return (char *)NULL;
    case 0:
    case 1:
      return guess;
    }
}
