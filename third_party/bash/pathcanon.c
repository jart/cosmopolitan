/* pathcanon.c -- canonicalize and manipulate pathnames. */

/* Copyright (C) 2000 Free Software Foundation, Inc.

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

#include "bashtypes.h"
#if defined (HAVE_SYS_PARAM_H)
#  include <sys/param.h>
#endif
#include "posixstat.h"

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#include "filecntl.h"
#include "bashansi.h"
#include <stdio.h>
#include "chartypes.h"
#include <errno.h>

#include "shell.h"

#if !defined (errno)
extern int errno;
#endif

#if defined (__CYGWIN__)
#include <sys/cygwin.h>

static int
_is_cygdrive (path)
     char *path;
{
  static char user[MAXPATHLEN];
  static char system[MAXPATHLEN];
  static int first_time = 1;

  /* If the path is the first part of a network path, treat it as
     existing. */
  if (path[0] == '/' && path[1] == '/' && !strchr (path + 2, '/'))
    return 1; 
  /* Otherwise check for /cygdrive prefix. */
  if (first_time)
    {
      char user_flags[MAXPATHLEN];
      char system_flags[MAXPATHLEN];
      /* Get the cygdrive info */
      cygwin_internal (CW_GET_CYGDRIVE_INFO, user, system, user_flags, system_flags);
      first_time = 0;
    }
  return !strcasecmp (path, user) || !strcasecmp (path, system);
}
#endif /* __CYGWIN__ */	

/* Return 1 if PATH corresponds to a directory.  A function for debugging. */
static int
_path_isdir (path)
     char *path;
{
  int l;
  struct stat sb;

  /* This should leave errno set to the correct value. */
  errno = 0;
  l = stat (path, &sb) == 0 && S_ISDIR (sb.st_mode);
#if defined (__CYGWIN__)
  if (l == 0)
    l = _is_cygdrive (path);
#endif
  return l;
}

/* Canonicalize PATH, and return a new path.  The new path differs from PATH
   in that:
	Multiple `/'s are collapsed to a single `/'.
	Leading `./'s and trailing `/.'s are removed.
	Trailing `/'s are removed.
	Non-leading `../'s and trailing `..'s are handled by removing
	portions of the path. */

/* Look for ROOTEDPATH, PATHSEP, DIRSEP, and ISDIRSEP in ../../general.h */

#define DOUBLE_SLASH(p)	((p[0] == '/') && (p[1] == '/') && p[2] != '/')

char *
sh_canonpath (path, flags)
     char *path;
     int flags;
{
  char stub_char;
  char *result, *p, *q, *base, *dotdot;
  int rooted, double_slash_path;

  /* The result cannot be larger than the input PATH. */
  result = (flags & PATH_NOALLOC) ? path : savestring (path);

  /* POSIX.2 says to leave a leading `//' alone.  On cygwin, we skip over any
     leading `x:' (dos drive name). */
  if (rooted = ROOTEDPATH(path))
    {
      stub_char = DIRSEP;
#if defined (__CYGWIN__)
      base = (ISALPHA((unsigned char)result[0]) && result[1] == ':') ? result + 3 : result + 1;
#else
      base = result + 1;
#endif
      double_slash_path = DOUBLE_SLASH (path);
      base += double_slash_path;
    }
  else
    {
      stub_char = '.';
#if defined (__CYGWIN__)
      base = (ISALPHA((unsigned char)result[0]) && result[1] == ':') ? result + 2 : result;
#else
      base = result;
#endif
      double_slash_path = 0;
    }

  /*
   * invariants:
   *	  base points to the portion of the path we want to modify
   *      p points at beginning of path element we're considering.
   *      q points just past the last path element we wrote (no slash).
   *      dotdot points just past the point where .. cannot backtrack
   *	  any further (no slash).
   */
  p = q = dotdot = base;

  while (*p)
    {
      if (ISDIRSEP(p[0])) /* null element */
	p++;
      else if(p[0] == '.' && PATHSEP(p[1]))	/* . and ./ */
	p += 1; 	/* don't count the separator in case it is nul */
      else if (p[0] == '.' && p[1] == '.' && PATHSEP(p[2])) /* .. and ../ */
	{
	  p += 2; /* skip `..' */
	  if (q > dotdot)	/* can backtrack */
	    {
	      if (flags & PATH_CHECKDOTDOT)
		{
		  char c;

		  /* Make sure what we have so far corresponds to a valid
		     path before we chop some of it off. */
		  c = *q;
		  *q = '\0';
		  if (_path_isdir (result) == 0)
		    {
		      if ((flags & PATH_NOALLOC) == 0)
			free (result);
		      return ((char *)NULL);
		    }
		  *q = c;
		}

	      while (--q > dotdot && ISDIRSEP(*q) == 0)
		;
	    }
	  else if (rooted == 0)
	    {
	      /* /.. is / but ./../ is .. */
	      if (q != base)
		*q++ = DIRSEP;
	      *q++ = '.';
	      *q++ = '.';
	      dotdot = q;
	    }
	}
      else	/* real path element */
	{
	  /* add separator if not at start of work portion of result */
	  if (q != base)
	    *q++ = DIRSEP;
	  while (*p && (ISDIRSEP(*p) == 0))
	    *q++ = *p++;
	  /* Check here for a valid directory with _path_isdir. */
	  if (flags & PATH_CHECKEXISTS)
	    {
	      char c;

	      /* Make sure what we have so far corresponds to a valid
		 path before we chop some of it off. */
	      c = *q;
	      *q = '\0';
	      if (_path_isdir (result) == 0)
		{
		  if ((flags & PATH_NOALLOC) == 0)
		    free (result);
		  return ((char *)NULL);
		}
	      *q = c;
	    }
	}
    }

  /* Empty string is really ``.'' or `/', depending on what we started with. */
  if (q == result)
    *q++ = stub_char;
  *q = '\0';

  /* If the result starts with `//', but the original path does not, we
     can turn the // into /.  Because of how we set `base', this should never
     be true, but it's a sanity check. */
  if (DOUBLE_SLASH(result) && double_slash_path == 0)
    {
      if (result[2] == '\0')	/* short-circuit for bare `//' */
	result[1] = '\0';
      else
	memmove (result, result + 1, strlen (result + 1) + 1);
    }

  return (result);
}
