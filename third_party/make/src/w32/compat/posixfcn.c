/* Replacements for Posix functions and Posix functionality for MS-Windows.

Copyright (C) 2013-2020 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the License, or (at your option) any later
version.

GNU Make is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <string.h>
#include <io.h>
#include <stdarg.h>
#include <errno.h>
#include <windows.h>

#include "dlfcn.h"

#include "makeint.h"
#include "job.h"

#ifndef NO_OUTPUT_SYNC
/* Support for OUTPUT_SYNC and related functionality.  */

/* Emulation of fcntl that supports only F_GETFD and F_SETLKW.  */
int
fcntl (intptr_t fd, int cmd, ...)
{
  va_list ap;

  va_start (ap, cmd);

  switch (cmd)
    {
      case F_GETFD:
        va_end (ap);
        /* Could have used GetHandleInformation, but that isn't
           supported on Windows 9X.  */
        if (_get_osfhandle (fd) == -1)
          return -1;
        return 0;
      case F_SETLKW:
        {
          void *buf = va_arg (ap, void *);
          struct flock *fl = (struct flock *)buf;
          HANDLE hmutex = (HANDLE)fd;
          static struct flock last_fl;
          short last_type = last_fl.l_type;

          va_end (ap);

          if (hmutex == INVALID_HANDLE_VALUE || !hmutex)
            return -1;

          last_fl = *fl;

          switch (fl->l_type)
            {

              case F_WRLCK:
                {
                  DWORD result;

                  if (last_type == F_WRLCK)
                    {
                      /* Don't call WaitForSingleObject if we already
                         own the mutex, because doing so will require
                         us to call ReleaseMutex an equal number of
                         times, before the mutex is actually
                         released.  */
                      return 0;
                    }

                  result = WaitForSingleObject (hmutex, INFINITE);
                  switch (result)
                    {
                      case WAIT_OBJECT_0:
                        /* We don't care if the mutex owner crashed or
                           exited.  */
                      case WAIT_ABANDONED:
                        return 0;
                      case WAIT_FAILED:
                      case WAIT_TIMEOUT: /* cannot happen, really */
                        {
                          DWORD err = GetLastError ();

                          /* Invalidate the last command.  */
                          memset (&last_fl, 0, sizeof (last_fl));

                          switch (err)
                            {
                              case ERROR_INVALID_HANDLE:
                              case ERROR_INVALID_FUNCTION:
                                errno = EINVAL;
                                return -1;
                              default:
                                errno = EDEADLOCK;
                                return -1;
                            }
                        }
                    }
                }
              case F_UNLCK:
                {
                  /* FIXME: Perhaps we should call ReleaseMutex
                     repatedly until it errors out, to make sure the
                     mutext is released even if we somehow managed to
                     to take ownership multiple times?  */
                  BOOL status = ReleaseMutex (hmutex);

                  if (status)
                    return 0;
                  else
                    {
                      DWORD err = GetLastError ();

                      if (err == ERROR_NOT_OWNER)
                        errno = EPERM;
                      else
                        {
                          memset (&last_fl, 0, sizeof (last_fl));
                          errno = EINVAL;
                        }
                      return -1;
                    }
                }
              default:
                errno = ENOSYS;
                return -1;
            }
        }
      default:
        errno = ENOSYS;
        va_end (ap);
        return -1;
    }
}

static intptr_t mutex_handle = -1;

/* Record in a static variable the mutex handle we were requested to
   use.  That nameless mutex was created by the top-level Make, and
   its handle was passed to us via inheritance.  The value of that
   handle is passed via the command-line arguments, so that we know
   which handle to use.  */
void
record_sync_mutex (const char *str)
{
  char *endp;
  intptr_t hmutex = strtol (str, &endp, 16);

  if (*endp == '\0')
    mutex_handle = hmutex;
  else
    {
      mutex_handle = -1;
      errno = EINVAL;
    }
}

/* Create a new mutex or reuse one created by our parent.  */
intptr_t
create_mutex (void)
{
  SECURITY_ATTRIBUTES secattr;
  intptr_t hmutex = -1;

  /* If we have a mutex handle passed from the parent Make, just use
     that.  */
  if (mutex_handle > 0)
    return mutex_handle;

  /* We are the top-level Make, and we want the handle to be inherited
     by our child processes.  */
  secattr.nLength = sizeof (secattr);
  secattr.lpSecurityDescriptor = NULL; /* use default security descriptor */
  secattr.bInheritHandle = TRUE;

  hmutex = (intptr_t)CreateMutex (&secattr, FALSE, NULL);
  if (!hmutex)
    {
      DWORD err = GetLastError ();

      fprintf (stderr, "CreateMutex: error %lu\n", err);
      errno = ENOLCK;
      hmutex = -1;
    }

  mutex_handle = hmutex;
  return hmutex;
}

/* Return non-zero if F1 and F2 are 2 streams representing the same
   file or pipe or device.  */
int
same_stream (FILE *f1, FILE *f2)
{
  HANDLE fh1 = (HANDLE)_get_osfhandle (fileno (f1));
  HANDLE fh2 = (HANDLE)_get_osfhandle (fileno (f2));

  /* Invalid file descriptors get treated as different streams.  */
  if (fh1 && fh1 != INVALID_HANDLE_VALUE
      && fh2 && fh2 != INVALID_HANDLE_VALUE)
    {
      if (fh1 == fh2)
        return 1;
      else
        {
          DWORD ftyp1 = GetFileType (fh1), ftyp2 = GetFileType (fh2);

          if (ftyp1 != ftyp2
              || ftyp1 == FILE_TYPE_UNKNOWN || ftyp2 == FILE_TYPE_UNKNOWN)
            return 0;
          else if (ftyp1 == FILE_TYPE_CHAR)
            {
              /* For character devices, check if they both refer to a
                 console.  This loses if both handles refer to the
                 null device (FIXME!), but in that case we don't care
                 in the context of Make.  */
              DWORD conmode1, conmode2;

              /* Each process on Windows can have at most 1 console,
                 so if both handles are for the console device, they
                 are the same.  We also compare the console mode to
                 distinguish between stdin and stdout/stderr.  */
              if (GetConsoleMode (fh1, &conmode1)
                  && GetConsoleMode (fh2, &conmode2)
                  && conmode1 == conmode2)
                return 1;
            }
          else
            {
              /* For disk files and pipes, compare their unique
                 attributes.  */
              BY_HANDLE_FILE_INFORMATION bhfi1, bhfi2;

              /* Pipes get zero in the volume serial number, but do
                 appear to have meaningful information in file index
                 attributes.  We test file attributes as well, for a
                 good measure.  */
              if (GetFileInformationByHandle (fh1, &bhfi1)
                  && GetFileInformationByHandle (fh2, &bhfi2))
                return (bhfi1.dwVolumeSerialNumber == bhfi2.dwVolumeSerialNumber
                        && bhfi1.nFileIndexLow == bhfi2.nFileIndexLow
                        && bhfi1.nFileIndexHigh == bhfi2.nFileIndexHigh
                        && bhfi1.dwFileAttributes == bhfi2.dwFileAttributes);
            }
        }
    }
  return 0;
}

/* A replacement for tmpfile, since the MSVCRT implementation creates
   the file in the root directory of the current drive, which might
   not be writable by our user.  Most of the code borrowed from
   create_batch_file, see job.c.  */
FILE *
tmpfile (void)
{
  char temp_path[MAXPATHLEN];
  unsigned path_size = GetTempPath (sizeof temp_path, temp_path);
  int path_is_dot = 0;
  /* The following variable is static so we won't try to reuse a name
     that was generated a little while ago, because that file might
     not be on disk yet, since we use FILE_ATTRIBUTE_TEMPORARY below,
     which tells the OS it doesn't need to flush the cache to disk.
     If the file is not yet on disk, we might think the name is
     available, while it really isn't.  This happens in parallel
     builds, where Make doesn't wait for one job to finish before it
     launches the next one.  */
  static unsigned uniq = 0;
  static int second_loop = 0;
  const char base[] = "gmake_tmpf";
  const unsigned sizemax = sizeof base - 1 + 4 + 10 + 10;
  unsigned pid = GetCurrentProcessId ();

  if (path_size == 0)
    {
      path_size = GetCurrentDirectory (sizeof temp_path, temp_path);
      path_is_dot = 1;
    }

  ++uniq;
  if (uniq >= 0x10000 && !second_loop)
    {
      /* If we already had 64K batch files in this
         process, make a second loop through the numbers,
         looking for free slots, i.e. files that were
         deleted in the meantime.  */
      second_loop = 1;
      uniq = 1;
    }
  while (path_size > 0 &&
         path_size + sizemax < sizeof temp_path &&
         !(uniq >= 0x10000 && second_loop))
    {
      HANDLE h;

      sprintf (temp_path + path_size,
               "%s%s%u-%x.tmp",
               temp_path[path_size - 1] == '\\' ? "" : "\\",
               base, pid, uniq);
      h = CreateFile (temp_path,  /* file name */
                      GENERIC_READ | GENERIC_WRITE | DELETE, /* desired access */
                      FILE_SHARE_READ | FILE_SHARE_WRITE,    /* share mode */
                      NULL,                                  /* default security attributes */
                      CREATE_NEW,                            /* creation disposition */
                      FILE_ATTRIBUTE_NORMAL |                /* flags and attributes */
                      FILE_ATTRIBUTE_TEMPORARY |
                      FILE_FLAG_DELETE_ON_CLOSE,
                      NULL);                                 /* no template file */

      if (h == INVALID_HANDLE_VALUE)
        {
          const DWORD er = GetLastError ();

          if (er == ERROR_FILE_EXISTS || er == ERROR_ALREADY_EXISTS)
            {
              ++uniq;
              if (uniq == 0x10000 && !second_loop)
                {
                  second_loop = 1;
                  uniq = 1;
                }
            }

          /* The temporary path is not guaranteed to exist, or might
             not be writable by user.  Use the current directory as
             fallback.  */
          else if (path_is_dot == 0)
            {
              path_size = GetCurrentDirectory (sizeof temp_path, temp_path);
              path_is_dot = 1;
            }

          else
            {
              errno = EACCES;
              break;
            }
        }
      else
        {
          int fd = _open_osfhandle ((intptr_t)h, 0);

          return _fdopen (fd, "w+b");
        }
    }

  if (uniq >= 0x10000)
    errno = EEXIST;
  return NULL;
}

#endif  /* !NO_OUTPUT_SYNC */

#if MAKE_LOAD

/* Support for dynamic loading of objects.  */


static DWORD last_err;

void *
dlopen (const char *file, int mode)
{
  char dllfn[MAX_PATH], *p;
  HANDLE dllhandle;

  if ((mode & ~(RTLD_LAZY | RTLD_NOW | RTLD_GLOBAL)) != 0)
    {
      errno = EINVAL;
      last_err = ERROR_INVALID_PARAMETER;
      return NULL;
    }

  if (!file)
    dllhandle = GetModuleHandle (NULL);
  else
    {
      /* MSDN says to be sure to use backslashes in the DLL file name.  */
      strcpy (dllfn, file);
      for (p = dllfn; *p; p++)
        if (*p == '/')
          *p = '\\';

      dllhandle = LoadLibrary (dllfn);
    }
  if (!dllhandle)
    last_err = GetLastError ();

  return dllhandle;
}

char *
dlerror (void)
{
  static char errbuf[1024];
  DWORD ret;

  if (!last_err)
    return NULL;

  ret = FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM
                       | FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL, last_err, 0, errbuf, sizeof (errbuf), NULL);
  while (ret > 0 && (errbuf[ret - 1] == '\n' || errbuf[ret - 1] == '\r'))
    --ret;

  errbuf[ret] = '\0';
  if (!ret)
    sprintf (errbuf, "Error code %lu", last_err);

  last_err = 0;
  return errbuf;
}

void *
dlsym (void *handle, const char *name)
{
  FARPROC addr = NULL;

  if (!handle || handle == INVALID_HANDLE_VALUE)
    {
      last_err = ERROR_INVALID_PARAMETER;
      return NULL;
    }

  addr = GetProcAddress (handle, name);
  if (!addr)
    last_err = GetLastError ();

  return (void *)addr;
}

int
dlclose (void *handle)
{
  if (!handle || handle == INVALID_HANDLE_VALUE)
    return -1;
  if (!FreeLibrary (handle))
    return -1;

  return 0;
}


#endif  /* MAKE_LOAD */


/* MS runtime's isatty returns non-zero for any character device,
   including the null device, which is not what we want.  */
int
isatty (int fd)
{
  HANDLE fh = (HANDLE) _get_osfhandle (fd);
  DWORD con_mode;

  if (fh == INVALID_HANDLE_VALUE)
    {
      errno = EBADF;
      return 0;
    }
  if (GetConsoleMode (fh, &con_mode))
    return 1;

  errno = ENOTTY;
  return 0;
}

char *
ttyname (int fd)
{
  /* This "knows" that Make only asks about stdout and stderr.  A more
     sophisticated implementation should test whether FD is open for
     input or output.  We can do that by looking at the mode returned
     by GetConsoleMode.  */
  return "CONOUT$";
}
