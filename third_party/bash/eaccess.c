#include "libc/dce.h"
/* eaccess.c - eaccess replacement for the shell, plus other access functions. */

/* Copyright (C) 2006-2020 Free Software Foundation, Inc.

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

#if defined (HAVE_CONFIG_H)
#  include "config.h"
#endif

#include <stdio.h>

#include "bashtypes.h"

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#include "bashansi.h"

#include <errno.h>
#if !defined (errno)
extern int errno;
#endif /* !errno */

#if !defined (_POSIX_VERSION) && defined (HAVE_SYS_FILE_H)
#  include <sys/file.h>
#endif /* !_POSIX_VERSION */
#include "posixstat.h"
#include "filecntl.h"

#include "shell.h"

#if !defined (R_OK)
#define R_OK 4
#define W_OK 2
#define X_OK 1
#define F_OK 0
#endif /* R_OK */

static int path_is_devfd PARAMS((const char *));
static int sh_stataccess PARAMS((const char *, int));
#if HAVE_DECL_SETREGID
static int sh_euidaccess PARAMS((const char *, int));
#endif

static int
path_is_devfd (path)
     const char *path;
{
  if (path[0] == '/' && path[1] == 'd' && strncmp (path, "/dev/fd/", 8) == 0)
    return 1;
  else if (STREQN (path, "/dev/std", 8))
    {
      if (STREQ (path+8, "in") || STREQ (path+8, "out") || STREQ (path+8, "err"))
	return 1;
      else
	return 0;
    }
  else
    return 0;
}

/* A wrapper for stat () which disallows pathnames that are empty strings
   and handles /dev/fd emulation on systems that don't have it. */
int
sh_stat (path, finfo)
     const char *path;
     struct stat *finfo;
{
  static char *pbuf = 0;

  if (*path == '\0')
    {
      errno = ENOENT;
      return (-1);
    }
  if (path[0] == '/' && path[1] == 'd' && strncmp (path, "/dev/fd/", 8) == 0)
    {
      /* If stating /dev/fd/n doesn't produce the same results as fstat of
	 FD N, then define DEV_FD_STAT_BROKEN */
//#if !defined (HAVE_DEV_FD) || defined (DEV_FD_STAT_BROKEN)
if (IsBsd()) {//[jart]
      intmax_t fd;
      int r;

      if (legal_number (path + 8, &fd) && fd == (int)fd)
        {
          r = fstat ((int)fd, finfo);
          if (r == 0 || errno != EBADF)
            return (r);
        }
      errno = ENOENT;
      return (-1);
//#else
} else {//[jart]
  /* If HAVE_DEV_FD is defined, DEV_FD_PREFIX is defined also, and has a
     trailing slash.  Make sure /dev/fd/xx really uses DEV_FD_PREFIX/xx.
     On most systems, with the notable exception of linux, this is
     effectively a no-op. */
      pbuf = xrealloc (pbuf, sizeof (DEV_FD_PREFIX) + strlen (path + 8));
      strcpy (pbuf, DEV_FD_PREFIX);
      strcat (pbuf, path + 8);
      return (stat (pbuf, finfo));
//#endif /* !HAVE_DEV_FD */
}//[jart]
    }
#if !defined (HAVE_DEV_STDIN)
  else if (STREQN (path, "/dev/std", 8))
    {
      if (STREQ (path+8, "in"))
	return (fstat (0, finfo));
      else if (STREQ (path+8, "out"))
	return (fstat (1, finfo));
      else if (STREQ (path+8, "err"))
	return (fstat (2, finfo));
      else
	return (stat (path, finfo));
    }
#endif /* !HAVE_DEV_STDIN */
  return (stat (path, finfo));
}

/* Do the same thing access(2) does, but use the effective uid and gid,
   and don't make the mistake of telling root that any file is
   executable.  This version uses stat(2). */
static int
sh_stataccess (path, mode)
     const char *path;
     int mode;
{
  struct stat st;

  if (sh_stat (path, &st) < 0)
    return (-1);

  if (current_user.euid == 0)
    {
      /* Root can read or write any file. */
      if ((mode & X_OK) == 0)
	return (0);

      /* Root can execute any file that has any one of the execute
	 bits set. */
      if (st.st_mode & S_IXUGO)
	return (0);
    }

  if (st.st_uid == current_user.euid)	/* owner */
    mode <<= 6;
  else if (group_member (st.st_gid))
    mode <<= 3;

  if (st.st_mode & mode)
    return (0);

  errno = EACCES;
  return (-1);
}

#if HAVE_DECL_SETREGID
/* Version to call when uid != euid or gid != egid.  We temporarily swap
   the effective and real uid and gid as appropriate. */
static int
sh_euidaccess (path, mode)
     const char *path;
     int mode;
{
  int r, e;

  if (current_user.uid != current_user.euid)
    setreuid (current_user.euid, current_user.uid);
  if (current_user.gid != current_user.egid)
    setregid (current_user.egid, current_user.gid);

  r = access (path, mode);
  e = errno;

  if (current_user.uid != current_user.euid)
    setreuid (current_user.uid, current_user.euid);
  if (current_user.gid != current_user.egid)
    setregid (current_user.gid, current_user.egid);

  errno = e;
  return r;  
}
#endif

int
sh_eaccess (path, mode)
     const char *path;
     int mode;
{
  int ret;

  if (path_is_devfd (path))
    return (sh_stataccess (path, mode));

#if (defined (HAVE_FACCESSAT) && defined (AT_EACCESS)) || defined (HAVE_EACCESS)
#  if defined (HAVE_FACCESSAT) && defined (AT_EACCESS)
  ret = faccessat (AT_FDCWD, path, mode, AT_EACCESS);
#  else		/* HAVE_EACCESS */	/* FreeBSD */
  ret = eaccess (path, mode);	/* XXX -- not always correct for X_OK */
#  endif	/* HAVE_EACCESS */
#  if defined (__FreeBSD__) || defined (SOLARIS) || defined (_AIX)
  if (ret == 0 && current_user.euid == 0 && mode == X_OK)
    return (sh_stataccess (path, mode));
#  endif	/* __FreeBSD__ || SOLARIS || _AIX */
  return ret;
#elif defined (EFF_ONLY_OK)		/* SVR4(?), SVR4.2 */
  return access (path, mode|EFF_ONLY_OK);
#else
  if (mode == F_OK)
    return (sh_stataccess (path, mode));
    
#  if HAVE_DECL_SETREGID
  if (current_user.uid != current_user.euid || current_user.gid != current_user.egid)
    return (sh_euidaccess (path, mode));
#  endif

  if (current_user.uid == current_user.euid && current_user.gid == current_user.egid)
    {
      ret = access (path, mode);
#if defined (__FreeBSD__) || defined (SOLARIS)
      if (ret == 0 && current_user.euid == 0 && mode == X_OK)
	return (sh_stataccess (path, mode));
#endif
      return ret;
    }

  return (sh_stataccess (path, mode));
#endif
}
