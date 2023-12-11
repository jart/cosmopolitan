/* pathphys.c -- return pathname with all symlinks expanded. */

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

#if !defined (MAXSYMLINKS)
#  define MAXSYMLINKS 32
#endif

#if !defined (errno)
extern int errno;
#endif /* !errno */

extern char *get_working_directory PARAMS((char *));

static int
_path_readlink (path, buf, bufsiz)
     char *path;
     char *buf;
     int bufsiz;
{
#ifdef HAVE_READLINK
  return readlink (path, buf, bufsiz);
#else
  errno = EINVAL;
  return -1;
#endif
}

/* Look for ROOTEDPATH, PATHSEP, DIRSEP, and ISDIRSEP in ../../general.h */

#define DOUBLE_SLASH(p)	((p[0] == '/') && (p[1] == '/') && p[2] != '/')

/*
 * Return PATH with all symlinks expanded in newly-allocated memory.
 * This always gets an absolute pathname.
 */

char *
sh_physpath (path, flags)
     char *path;
     int flags;
{
  char tbuf[PATH_MAX+1], linkbuf[PATH_MAX+1];
  char *result, *p, *q, *qsave, *qbase, *workpath;
  int double_slash_path, linklen, nlink;

  linklen = strlen (path);

#if 0
  /* First sanity check -- punt immediately if the name is too long. */
  if (linklen >= PATH_MAX)
    return (savestring (path));
#endif

  nlink = 0;
  q = result = (char *)xmalloc (PATH_MAX + 1);

  /* Even if we get something longer than PATH_MAX, we might be able to
     shorten it, so we try. */
  if (linklen >= PATH_MAX)
    workpath = savestring (path);
  else
    {
      workpath = (char *)xmalloc (PATH_MAX + 1);
      strcpy (workpath, path);
    }

  /* This always gets an absolute pathname. */

  /* POSIX.2 says to leave a leading `//' alone.  On cygwin, we skip over any
     leading `x:' (dos drive name). */
#if defined (__CYGWIN__)
  qbase = (ISALPHA((unsigned char)workpath[0]) && workpath[1] == ':') ? workpath + 3 : workpath + 1;
#else
  qbase = workpath + 1;
#endif
  double_slash_path = DOUBLE_SLASH (workpath);
  qbase += double_slash_path;

  for (p = workpath; p < qbase; )
    *q++ = *p++;
  qbase = q;

  /*
   * invariants:
   *	  qbase points to the portion of the result path we want to modify
   *      p points at beginning of path element we're considering.
   *      q points just past the last path element we wrote (no slash).
   *
   * XXX -- need to fix error checking for too-long pathnames
   */

  while (*p)
    {
      if (ISDIRSEP(p[0])) /* null element */
	p++;
      else if(p[0] == '.' && PATHSEP(p[1]))	/* . and ./ */
	p += 1; 	/* don't count the separator in case it is nul */
      else if (p[0] == '.' && p[1] == '.' && PATHSEP(p[2])) /* .. and ../ */
	{
	  p += 2; /* skip `..' */
	  if (q > qbase)
	    {
	      while (--q > qbase && ISDIRSEP(*q) == 0)
		;
	    }
	}
      else	/* real path element */
	{
	  /* add separator if not at start of work portion of result */
	  qsave = q;
	  if (q != qbase)
	    *q++ = DIRSEP;
	  while (*p && (ISDIRSEP(*p) == 0))
	    {
	      if (q - result >= PATH_MAX)
		{
#ifdef ENAMETOOLONG
		  errno = ENAMETOOLONG;
#else
		  errno = EINVAL;
#endif
		  goto error;
		}
		
	      *q++ = *p++;
	    }

	  *q = '\0';

	  linklen = _path_readlink (result, linkbuf, PATH_MAX);
	  if (linklen < 0)	/* if errno == EINVAL, it's not a symlink */
	    {
	      if (errno != EINVAL)
		goto error;
	      continue;
	    }

	  /* It's a symlink, and the value is in LINKBUF. */
	  nlink++;
	  if (nlink > MAXSYMLINKS)
	    {
#ifdef ELOOP
	      errno = ELOOP;
#else
	      errno = EINVAL;
#endif
error:
	      free (result);
	      free (workpath);
	      return ((char *)NULL);
	    }

	  linkbuf[linklen] = '\0';

	  /* If the new path length would overrun PATH_MAX, punt now. */
	  if ((strlen (p) + linklen + 2) >= PATH_MAX)
	    {
#ifdef ENAMETOOLONG
	      errno = ENAMETOOLONG;
#else
	      errno = EINVAL;
#endif
	      goto error;
	    }

	  /* Form the new pathname by copying the link value to a temporary
	     buffer and appending the rest of `workpath'.  Reset p to point
	     to the start of the rest of the path.  If the link value is an
	     absolute pathname, reset p, q, and qbase.  If not, reset p
	     and q. */
	  strcpy (tbuf, linkbuf);
	  tbuf[linklen] = '/';
	  strcpy (tbuf + linklen, p);
	  strcpy (workpath, tbuf);

	  if (ABSPATH(linkbuf))
	    {
	      q = result;
	      /* Duplicating some code here... */
#if defined (__CYGWIN__)
	      qbase = (ISALPHA((unsigned char)workpath[0]) && workpath[1] == ':') ? workpath + 3 : workpath + 1;
#else
	      qbase = workpath + 1;
#endif
	      double_slash_path = DOUBLE_SLASH (workpath);
	      qbase += double_slash_path;
    
	      for (p = workpath; p < qbase; )
		*q++ = *p++;
	      qbase = q;
	    }
	  else
	    {
	      p = workpath;
	      q = qsave;
	    }
	}
    }

  *q = '\0';
  free (workpath);

  /* If the result starts with `//', but the original path does not, we
     can turn the // into /.  Because of how we set `qbase', this should never
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

char *
sh_realpath (pathname, resolved)
     const char *pathname;
     char *resolved;
{
  char *tdir, *wd;

  if (pathname == 0 || *pathname == '\0')
    {
      errno = (pathname == 0) ? EINVAL : ENOENT;
      return ((char *)NULL);
    }

  if (ABSPATH (pathname) == 0)
    {
      wd = get_working_directory ("sh_realpath");
      if (wd == 0)
	return ((char *)NULL);
      tdir = sh_makepath (wd, (char *)pathname, 0);
      free (wd);
    }
  else
    tdir = savestring (pathname);

  wd = sh_physpath (tdir, 0);
  free (tdir);

  if (resolved == 0)
    return (wd);

  if (wd)
    {
      strncpy (resolved, wd, PATH_MAX - 1);
      resolved[PATH_MAX - 1] = '\0';
      free (wd);
      return resolved;
    }
  else
    {
      resolved[0] = '\0';
      return wd;
    }
}
