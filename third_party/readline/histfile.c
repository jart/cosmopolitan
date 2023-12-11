/* histfile.c - functions to manipulate the history file. */

/* Copyright (C) 1989-2019 Free Software Foundation, Inc.

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

/* The goal is to make the implementation transparent, so that you
   don't have to know what data types are used, just what functions
   you can call.  I think I have done that. */

#define READLINE_LIBRARY

#if defined (__TANDEM)
#  define _XOPEN_SOURCE_EXTENDED 1
#  include <unistd.h>
#  include <floss.h>
#endif

#if defined (HAVE_CONFIG_H)
#  include "config.h"
#endif

#include <stdio.h>

#if defined (HAVE_LIMITS_H)
#  include <limits.h>
#endif

#include <sys/types.h>
#if ! defined (_MINIX) && defined (HAVE_SYS_FILE_H)
#  include <sys/file.h>
#endif
#include "posixstat.h"
#include <fcntl.h>

#if defined (HAVE_STDLIB_H)
#  include <stdlib.h>
#else
#  include "ansi_stdlib.h"
#endif /* HAVE_STDLIB_H */

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#include <ctype.h>

#if defined (__EMX__)
#  undef HAVE_MMAP
#endif

#ifdef HISTORY_USE_MMAP
#  include <sys/mman.h>

#  ifdef MAP_FILE
#    define MAP_RFLAGS	(MAP_FILE|MAP_PRIVATE)
#    define MAP_WFLAGS	(MAP_FILE|MAP_SHARED)
#  else
#    define MAP_RFLAGS	MAP_PRIVATE
#    define MAP_WFLAGS	MAP_SHARED
#  endif

#  ifndef MAP_FAILED
#    define MAP_FAILED	((void *)-1)
#  endif

#endif /* HISTORY_USE_MMAP */

#if defined(_WIN32)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#endif

/* If we're compiling for __EMX__ (OS/2) or __CYGWIN__ (cygwin32 environment
   on win 95/98/nt), we want to open files with O_BINARY mode so that there
   is no \n -> \r\n conversion performed.  On other systems, we don't want to
   mess around with O_BINARY at all, so we ensure that it's defined to 0. */
#if defined (__EMX__) || defined (__CYGWIN__)
#  ifndef O_BINARY
#    define O_BINARY 0
#  endif
#else /* !__EMX__ && !__CYGWIN__ */
#  undef O_BINARY
#  define O_BINARY 0
#endif /* !__EMX__ && !__CYGWIN__ */

#include <errno.h>
#if !defined (errno)
extern int errno;
#endif /* !errno */

#include "history.h"
#include "histlib.h"

#include "rlshell.h"
#include "xmalloc.h"

#if !defined (PATH_MAX)
#  define PATH_MAX	1024	/* default */
#endif

/* history file version; currently unused */
int history_file_version = 1;

/* If non-zero, we write timestamps to the history file in history_do_write() */
int history_write_timestamps = 0;

/* If non-zero, we assume that a history file that starts with a timestamp
   uses timestamp-delimited entries and can include multi-line history
   entries. Used by read_history_range */
int history_multiline_entries = 0;

/* Immediately after a call to read_history() or read_history_range(), this
   will return the number of lines just read from the history file in that
   call. */
int history_lines_read_from_file = 0;

/* Immediately after a call to write_history() or history_do_write(), this
   will return the number of lines just written to the history file in that
   call.  This also works with history_truncate_file. */
int history_lines_written_to_file = 0;

/* Does S look like the beginning of a history timestamp entry?  Placeholder
   for more extensive tests. */
#define HIST_TIMESTAMP_START(s)		(*(s) == history_comment_char && isdigit ((unsigned char)(s)[1]) )

static char *history_backupfile (const char *);
static char *history_tempfile (const char *);
static int histfile_backup (const char *, const char *);
static int histfile_restore (const char *, const char *);
static int history_rename (const char *, const char *);

/* Return the string that should be used in the place of this
   filename.  This only matters when you don't specify the
   filename to read_history (), or write_history (). */
static char *
history_filename (const char *filename)
{
  char *return_val;
  const char *home;
  int home_len;

  return_val = filename ? savestring (filename) : (char *)NULL;

  if (return_val)
    return (return_val);
  
  home = sh_get_env_value ("HOME");
#if defined (_WIN32)
  if (home == 0)
    home = sh_get_env_value ("APPDATA");
#endif

  if (home == 0)
    return (NULL);
  else
    home_len = strlen (home);

  return_val = (char *)xmalloc (2 + home_len + 8); /* strlen(".history") == 8 */
  strcpy (return_val, home);
  return_val[home_len] = '/';
#if defined (__MSDOS__)
  strcpy (return_val + home_len + 1, "_history");
#else
  strcpy (return_val + home_len + 1, ".history");
#endif

  return (return_val);
}

static char *
history_backupfile (const char *filename)
{
  const char *fn;
  char *ret, linkbuf[PATH_MAX+1];
  size_t len;
  ssize_t n;
  struct stat fs;

  fn = filename;  
#if defined (HAVE_READLINK)
  /* Follow symlink to avoid backing up symlink itself; call will fail if
     not a symlink */
  if ((n = readlink (filename, linkbuf, sizeof (linkbuf) - 1)) > 0)
    {
      linkbuf[n] = '\0';
      fn = linkbuf;
    }
#endif
      
  len = strlen (fn);
  ret = xmalloc (len + 2);
  strcpy (ret, fn);
  ret[len] = '-';
  ret[len+1] = '\0';
  return ret;
}
  
static char *
history_tempfile (const char *filename)
{
  const char *fn;
  char *ret, linkbuf[PATH_MAX+1];
  size_t len;
  ssize_t n;
  struct stat fs;
  int pid;

  fn = filename;  
#if defined (HAVE_READLINK)
  /* Follow symlink so tempfile created in the same directory as any symlinked
     history file; call will fail if not a symlink */
  if ((n = readlink (filename, linkbuf, sizeof (linkbuf) - 1)) > 0)
    {
      linkbuf[n] = '\0';
      fn = linkbuf;
    }
#endif
      
  len = strlen (fn);
  ret = xmalloc (len + 11);
  strcpy (ret, fn);

  pid = (int)getpid ();

  /* filename-PID.tmp */
  ret[len] = '-';
  ret[len+1] = (pid / 10000 % 10) + '0';
  ret[len+2] = (pid / 1000 % 10) + '0';
  ret[len+3] = (pid / 100 % 10) + '0';
  ret[len+4] = (pid / 10 % 10) + '0';
  ret[len+5] = (pid % 10) + '0';
  strcpy (ret + len + 6, ".tmp");

  return ret;
}
  
/* Add the contents of FILENAME to the history list, a line at a time.
   If FILENAME is NULL, then read from ~/.history.  Returns 0 if
   successful, or errno if not. */
int
read_history (const char *filename)
{
  return (read_history_range (filename, 0, -1));
}

/* Read a range of lines from FILENAME, adding them to the history list.
   Start reading at the FROM'th line and end at the TO'th.  If FROM
   is zero, start at the beginning.  If TO is less than FROM, read
   until the end of the file.  If FILENAME is NULL, then read from
   ~/.history.  Returns 0 if successful, or errno if not. */
int
read_history_range (const char *filename, int from, int to)
{
  register char *line_start, *line_end, *p;
  char *input, *buffer, *bufend, *last_ts;
  int file, current_line, chars_read, has_timestamps, reset_comment_char;
  struct stat finfo;
  size_t file_size;
#if defined (EFBIG)
  int overflow_errno = EFBIG;
#elif defined (EOVERFLOW)
  int overflow_errno = EOVERFLOW;
#else
  int overflow_errno = EIO;
#endif

  history_lines_read_from_file = 0;

  buffer = last_ts = (char *)NULL;
  input = history_filename (filename);
  file = input ? open (input, O_RDONLY|O_BINARY, 0666) : -1;

  if ((file < 0) || (fstat (file, &finfo) == -1))
    goto error_and_exit;

  if (S_ISREG (finfo.st_mode) == 0)
    {
#ifdef EFTYPE
      errno = EFTYPE;
#else
      errno = EINVAL;
#endif
      goto error_and_exit;
    }

  file_size = (size_t)finfo.st_size;

  /* check for overflow on very large files */
  if (file_size != finfo.st_size || file_size + 1 < file_size)
    {
      errno = overflow_errno;
      goto error_and_exit;
    }

  if (file_size == 0)
    {
      xfree (input);
      close (file);
      return 0;	/* don't waste time if we don't have to */
    }

#ifdef HISTORY_USE_MMAP
  /* We map read/write and private so we can change newlines to NULs without
     affecting the underlying object. */
  buffer = (char *)mmap (0, file_size, PROT_READ|PROT_WRITE, MAP_RFLAGS, file, 0);
  if ((void *)buffer == MAP_FAILED)
    {
      errno = overflow_errno;
      goto error_and_exit;
    }
  chars_read = file_size;
#else
  buffer = (char *)malloc (file_size + 1);
  if (buffer == 0)
    {
      errno = overflow_errno;
      goto error_and_exit;
    }

  chars_read = read (file, buffer, file_size);
#endif
  if (chars_read < 0)
    {
  error_and_exit:
      if (errno != 0)
	chars_read = errno;
      else
	chars_read = EIO;
      if (file >= 0)
	close (file);

      FREE (input);
#ifndef HISTORY_USE_MMAP
      FREE (buffer);
#endif

      return (chars_read);
    }

  close (file);

  /* Set TO to larger than end of file if negative. */
  if (to < 0)
    to = chars_read;

  /* Start at beginning of file, work to end. */
  bufend = buffer + chars_read;
  *bufend = '\0';		/* null-terminate buffer for timestamp checks */
  current_line = 0;

  /* Heuristic: the history comment character rarely changes, so assume we
     have timestamps if the buffer starts with `#[:digit:]' and temporarily
     set history_comment_char so timestamp parsing works right */
  reset_comment_char = 0;
  if (history_comment_char == '\0' && buffer[0] == '#' && isdigit ((unsigned char)buffer[1]))
    {
      history_comment_char = '#';
      reset_comment_char = 1;
    }

  has_timestamps = HIST_TIMESTAMP_START (buffer);
  history_multiline_entries += has_timestamps && history_write_timestamps;

  /* Skip lines until we are at FROM. */
  if (has_timestamps)
    last_ts = buffer;
  for (line_start = line_end = buffer; line_end < bufend && current_line < from; line_end++)
    if (*line_end == '\n')
      {
      	p = line_end + 1;
      	/* If we see something we think is a timestamp, continue with this
	   line.  We should check more extensively here... */
	if (HIST_TIMESTAMP_START(p) == 0)
	  current_line++;
	else
	  last_ts = p;
	line_start = p;
	/* If we are at the last line (current_line == from) but we have
	   timestamps (has_timestamps), then line_start points to the
	   text of the last command, and we need to skip to its end. */
	if (current_line >= from && has_timestamps)
	  {
	    for (line_end = p; line_end < bufend && *line_end != '\n'; line_end++)
	      ;
	    line_start = (*line_end == '\n') ? line_end + 1 : line_end;
	  }
      }

  /* If there are lines left to gobble, then gobble them now. */
  for (line_end = line_start; line_end < bufend; line_end++)
    if (*line_end == '\n')
      {
	/* Change to allow Windows-like \r\n end of line delimiter. */
	if (line_end > line_start && line_end[-1] == '\r')
	  line_end[-1] = '\0';
	else
	  *line_end = '\0';

	if (*line_start)
	  {
	    if (HIST_TIMESTAMP_START(line_start) == 0)
	      {
	      	if (last_ts == NULL && history_length > 0 && history_multiline_entries)
		  _hs_append_history_line (history_length - 1, line_start);
		else
		  add_history (line_start);
		if (last_ts)
		  {
		    add_history_time (last_ts);
		    last_ts = NULL;
		  }
	      }
	    else
	      {
		last_ts = line_start;
		current_line--;
	      }
	  }

	current_line++;

	if (current_line >= to)
	  break;

	line_start = line_end + 1;
      }

  history_lines_read_from_file = current_line;
  if (reset_comment_char)
    history_comment_char = '\0';

  FREE (input);
#ifndef HISTORY_USE_MMAP
  FREE (buffer);
#else
  munmap (buffer, file_size);
#endif

  return (0);
}

/* We need a special version for WIN32 because Windows rename() refuses to
   overwrite an existing file. */
static int
history_rename (const char *old, const char *new)
{
#if defined (_WIN32)
  return (MoveFileEx (old, new, MOVEFILE_REPLACE_EXISTING) == 0 ? -1 : 0);
#else
  return (rename (old, new));
#endif
}

/* Save FILENAME to BACK, handling case where FILENAME is a symlink
   (e.g., ~/.bash_history -> .histfiles/.bash_history.$HOSTNAME) */
static int
histfile_backup (const char *filename, const char *back)
{
#if defined (HAVE_READLINK)
  char linkbuf[PATH_MAX+1];
  ssize_t n;

  /* Follow to target of symlink to avoid renaming symlink itself */
  if ((n = readlink (filename, linkbuf, sizeof (linkbuf) - 1)) > 0)
    {
      linkbuf[n] = '\0';
      return (history_rename (linkbuf, back));
    }
#endif
  return (history_rename (filename, back));
}

/* Restore ORIG from BACKUP handling case where ORIG is a symlink
   (e.g., ~/.bash_history -> .histfiles/.bash_history.$HOSTNAME) */
static int
histfile_restore (const char *backup, const char *orig)
{
#if defined (HAVE_READLINK)
  char linkbuf[PATH_MAX+1];
  ssize_t n;

  /* Follow to target of symlink to avoid renaming symlink itself */
  if ((n = readlink (orig, linkbuf, sizeof (linkbuf) - 1)) > 0)
    {
      linkbuf[n] = '\0';
      return (history_rename (backup, linkbuf));
    }
#endif
  return (history_rename (backup, orig));
}

/* Should we call chown, based on whether finfo and nfinfo describe different
   files with different owners? */

#define SHOULD_CHOWN(finfo, nfinfo) \
  (finfo.st_uid != nfinfo.st_uid || finfo.st_gid != nfinfo.st_gid)
  
/* Truncate the history file FNAME, leaving only LINES trailing lines.
   If FNAME is NULL, then use ~/.history.  Writes a new file and renames
   it to the original name.  Returns 0 on success, errno on failure. */
int
history_truncate_file (const char *fname, int lines)
{
  char *buffer, *filename, *tempname, *bp, *bp1;		/* bp1 == bp+1 */
  int file, chars_read, rv, orig_lines, exists, r;
  struct stat finfo, nfinfo;
  size_t file_size;

  history_lines_written_to_file = 0;

  buffer = (char *)NULL;
  filename = history_filename (fname);
  tempname = 0;
  file = filename ? open (filename, O_RDONLY|O_BINARY, 0666) : -1;
  rv = exists = 0;

  /* Don't try to truncate non-regular files. */
  if (file == -1 || fstat (file, &finfo) == -1)
    {
      rv = errno;
      if (file != -1)
	close (file);
      goto truncate_exit;
    }
  exists = 1;

  nfinfo.st_uid = finfo.st_uid;
  nfinfo.st_gid = finfo.st_gid;

  if (S_ISREG (finfo.st_mode) == 0)
    {
      close (file);
#ifdef EFTYPE
      rv = EFTYPE;
#else
      rv = EINVAL;
#endif
      goto truncate_exit;
    }

  file_size = (size_t)finfo.st_size;

  /* check for overflow on very large files */
  if (file_size != finfo.st_size || file_size + 1 < file_size)
    {
      close (file);
#if defined (EFBIG)
      rv = errno = EFBIG;
#elif defined (EOVERFLOW)
      rv = errno = EOVERFLOW;
#else
      rv = errno = EINVAL;
#endif
      goto truncate_exit;
    }

  buffer = (char *)malloc (file_size + 1);
  if (buffer == 0)
    {
      rv = errno;
      close (file);
      goto truncate_exit;
    }

  chars_read = read (file, buffer, file_size);
  close (file);

  if (chars_read <= 0)
    {
      rv = (chars_read < 0) ? errno : 0;
      goto truncate_exit;
    }

  orig_lines = lines;
  /* Count backwards from the end of buffer until we have passed
     LINES lines.  bp1 is set funny initially.  But since bp[1] can't
     be a comment character (since it's off the end) and *bp can't be
     both a newline and the history comment character, it should be OK. */
  for (bp1 = bp = buffer + chars_read - 1; lines && bp > buffer; bp--)
    {
      if (*bp == '\n' && HIST_TIMESTAMP_START(bp1) == 0)
	lines--;
      bp1 = bp;
    }

  /* If this is the first line, then the file contains exactly the
     number of lines we want to truncate to, so we don't need to do
     anything.  It's the first line if we don't find a newline between
     the current value of i and 0.  Otherwise, write from the start of
     this line until the end of the buffer. */
  for ( ; bp > buffer; bp--)
    {
      if (*bp == '\n' && HIST_TIMESTAMP_START(bp1) == 0)
        {
	  bp++;
	  break;
        }
      bp1 = bp;
    }

  /* Write only if there are more lines in the file than we want to
     truncate to. */
  if (bp <= buffer)
    {
      rv = 0;
      /* No-op if LINES == 0 at this point */
      history_lines_written_to_file = orig_lines - lines;
      goto truncate_exit;
    }

  tempname = history_tempfile (filename);

  if ((file = open (tempname, O_WRONLY|O_CREAT|O_TRUNC|O_BINARY, 0600)) != -1)
    {
      if (write (file, bp, chars_read - (bp - buffer)) < 0)
	rv = errno;

      if (fstat (file, &nfinfo) < 0 && rv == 0)
	rv = errno;

      if (close (file) < 0 && rv == 0)
	rv = errno;
    }
  else
    rv = errno;

 truncate_exit:
  FREE (buffer);

  history_lines_written_to_file = orig_lines - lines;

  if (rv == 0 && filename && tempname)
    rv = histfile_restore (tempname, filename);

  if (rv != 0)
    {
      rv = errno;
      if (tempname)
	unlink (tempname);
      history_lines_written_to_file = 0;
    }

#if defined (HAVE_CHOWN)
  /* Make sure the new filename is owned by the same user as the old.  If one
     user is running this, it's a no-op.  If the shell is running after sudo
     with a shared history file, we don't want to leave the history file
     owned by root. */
  if (rv == 0 && exists && SHOULD_CHOWN (finfo, nfinfo))
    r = chown (filename, finfo.st_uid, finfo.st_gid);
#endif

  xfree (filename);
  FREE (tempname);

  return rv;
}

/* Workhorse function for writing history.  Writes the last NELEMENT entries
   from the history list to FILENAME.  OVERWRITE is non-zero if you
   wish to replace FILENAME with the entries. */
static int
history_do_write (const char *filename, int nelements, int overwrite)
{
  register int i;
  char *output, *tempname, *histname;
  int file, mode, rv, exists;
  struct stat finfo, nfinfo;
#ifdef HISTORY_USE_MMAP
  size_t cursize;

  history_lines_written_to_file = 0;

  mode = overwrite ? O_RDWR|O_CREAT|O_TRUNC|O_BINARY : O_RDWR|O_APPEND|O_BINARY;
#else
  mode = overwrite ? O_WRONLY|O_CREAT|O_TRUNC|O_BINARY : O_WRONLY|O_APPEND|O_BINARY;
#endif
  histname = history_filename (filename);
  exists = histname ? (stat (histname, &finfo) == 0) : 0;

  tempname = (overwrite && exists && S_ISREG (finfo.st_mode)) ? history_tempfile (histname) : 0;
  output = tempname ? tempname : histname;

  file = output ? open (output, mode, 0600) : -1;
  rv = 0;

  if (file == -1)
    {
      rv = errno;
      FREE (histname);
      FREE (tempname);
      return (rv);
    }

#ifdef HISTORY_USE_MMAP
  cursize = overwrite ? 0 : lseek (file, 0, SEEK_END);
#endif

  if (nelements > history_length)
    nelements = history_length;

  /* Build a buffer of all the lines to write, and write them in one syscall.
     Suggested by Peter Ho (peter@robosts.oxford.ac.uk). */
  {
    HIST_ENTRY **the_history;	/* local */
    register int j;
    int buffer_size;
    char *buffer;

    the_history = history_list ();
    /* Calculate the total number of bytes to write. */
    for (buffer_size = 0, i = history_length - nelements; i < history_length; i++)
      {
	if (history_write_timestamps && the_history[i]->timestamp && the_history[i]->timestamp[0])
	  buffer_size += strlen (the_history[i]->timestamp) + 1;
	buffer_size += strlen (the_history[i]->line) + 1;
      }

    /* Allocate the buffer, and fill it. */
#ifdef HISTORY_USE_MMAP
    if (ftruncate (file, buffer_size+cursize) == -1)
      goto mmap_error;
    buffer = (char *)mmap (0, buffer_size, PROT_READ|PROT_WRITE, MAP_WFLAGS, file, cursize);
    if ((void *)buffer == MAP_FAILED)
      {
mmap_error:
	rv = errno;
	close (file);
	if (tempname)
	  unlink (tempname);
	FREE (histname);
	FREE (tempname);
	return rv;
      }
#else    
    buffer = (char *)malloc (buffer_size);
    if (buffer == 0)
      {
      	rv = errno;
	close (file);
	if (tempname)
	  unlink (tempname);
	FREE (histname);
	FREE (tempname);
	return rv;
      }
#endif

    for (j = 0, i = history_length - nelements; i < history_length; i++)
      {
	if (history_write_timestamps && the_history[i]->timestamp && the_history[i]->timestamp[0])
	  {
	    strcpy (buffer + j, the_history[i]->timestamp);
	    j += strlen (the_history[i]->timestamp);
	    buffer[j++] = '\n';
	  }
	strcpy (buffer + j, the_history[i]->line);
	j += strlen (the_history[i]->line);
	buffer[j++] = '\n';
      }

#ifdef HISTORY_USE_MMAP
    if (msync (buffer, buffer_size, MS_ASYNC) != 0 || munmap (buffer, buffer_size) != 0)
      rv = errno;
#else
    if (write (file, buffer, buffer_size) < 0)
      rv = errno;
    xfree (buffer);
#endif
  }

  history_lines_written_to_file = nelements;

  if (close (file) < 0 && rv == 0)
    rv = errno;

  if (rv == 0 && histname && tempname)
    rv = histfile_restore (tempname, histname);

  if (rv != 0)
    {
      rv = errno;
      if (tempname)
	unlink (tempname);
      history_lines_written_to_file = 0;
    }

#if defined (HAVE_CHOWN)
  /* Make sure the new filename is owned by the same user as the old.  If one
     user is running this, it's a no-op.  If the shell is running after sudo
     with a shared history file, we don't want to leave the history file
     owned by root. */
  if (rv == 0 && exists)
    mode = chown (histname, finfo.st_uid, finfo.st_gid);
#endif

  FREE (histname);
  FREE (tempname);

  return (rv);
}

/* Append NELEMENT entries to FILENAME.  The entries appended are from
   the end of the list minus NELEMENTs up to the end of the list. */
int
append_history (int nelements, const char *filename)
{
  return (history_do_write (filename, nelements, HISTORY_APPEND));
}

/* Overwrite FILENAME with the current history.  If FILENAME is NULL,
   then write the history list to ~/.history.  Values returned
   are as in read_history ().*/
int
write_history (const char *filename)
{
  return (history_do_write (filename, history_length, HISTORY_OVERWRITE));
}
