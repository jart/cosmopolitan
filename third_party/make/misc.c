/* Miscellaneous generic support functions for GNU Make.
Copyright (C) 1988-2023 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the License, or (at your option) any later
version.

GNU Make is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include "makeint.h"
#include "filedef.h"
#include "dep.h"
#include "os.h"
#include "debug.h"

#include <assert.h>
#include <stdarg.h>

#ifdef WINDOWS32
# include <windows.h>
# include <io.h>
#endif

#ifdef __EMX__
# define INCL_DOS
# include <os2.h>
#endif

#ifdef HAVE_FCNTL_H
# include <fcntl.h>
#else
# include <sys/file.h>
#endif

unsigned int
make_toui (const char *str, const char **error)
{
  char *end;
  unsigned long val = strtoul (str, &end, 10);

  if (error)
    {
      if (str[0] == '\0')
        *error = "Missing value";
      else if (*end != '\0')
        *error = "Invalid value";
      else
        *error = NULL;
    }

  return val;
}

/* Convert val into a string, written to buf.  buf must be large enough
   to hold the largest possible value, plus a nul byte.  Returns buf.
   We can't use standard PRI* here: those are based on intNN_t types.  */

char *
make_lltoa (long long val, char *buf)
{
  sprintf (buf, "%" MK_PRI64_PREFIX "d", val);
  return buf;
}

char *
make_ulltoa (unsigned long long val, char *buf)
{
  sprintf (buf, "%" MK_PRI64_PREFIX "u", val);
  return buf;
}

/* Simple random number generator, for use with shuffle.
   This doesn't need to be truly random, just pretty random.  Use our own
   implementation rather than relying on the C runtime's rand() so we always
   get the same results for a given seed, regardless of C runtime.  */

static unsigned int mk_state = 0;

void
make_seed (unsigned int seed)
{
  mk_state = seed;
}

unsigned int
make_rand ()
{
  /* mk_state must never be 0.  */
  if (mk_state == 0)
    mk_state = (unsigned int)(time (NULL) ^ make_pid ()) + 1;

  /* A simple xorshift RNG.  */
  mk_state ^= mk_state << 13;
  mk_state ^= mk_state >> 17;
  mk_state ^= mk_state << 5;

  return mk_state;
}

/* Compare strings *S1 and *S2.
   Return negative if the first is less, positive if it is greater,
   zero if they are equal.  */

int
alpha_compare (const void *v1, const void *v2)
{
  const char *s1 = *((char **)v1);
  const char *s2 = *((char **)v2);

  if (*s1 != *s2)
    return *s1 - *s2;
  return strcmp (s1, s2);
}

/* Discard each backslash-newline combination from LINE.
   Backslash-backslash-newline combinations become backslash-newlines.
   This is done by copying the text at LINE into itself.  */

void
collapse_continuations (char *line)
{
  char *out = line;
  char *in = line;
  char *q;

  q = strchr(in, '\n');
  if (q == 0)
    return;

  do
    {
      char *p = q;
      int i;
      size_t out_line_length;

      if (q > line && q[-1] == '\\')
        {
          /* Search for more backslashes.  */
          i = -2;
          while (&p[i] >= line && p[i] == '\\')
            --i;
          ++i;
        }
      else
        i = 0;

      /* The number of backslashes is now -I, keep half of them.  */
      out_line_length = (p - in) + i - i/2;
      if (out != in)
        memmove (out, in, out_line_length);
      out += out_line_length;

      /* When advancing IN, skip the newline too.  */
      in = q + 1;

      if (i & 1)
        {
          /* Backslash/newline handling:
             In traditional GNU Make all trailing whitespace, consecutive
             backslash/newlines, and any leading non-newline whitespace on the
             next line is reduced to a single space.
             In POSIX, each backslash/newline and is replaced by a space.  */
          while (ISBLANK (*in))
            ++in;
          if (! posix_pedantic)
            while (out > line && ISBLANK (out[-1]))
              --out;
          *out++ = ' ';
        }
      else
        {
          /* If the newline isn't quoted, put it in the output.  */
          *out++ = '\n';
        }

      q = strchr(in, '\n');
    }
  while (q);

  memmove(out, in, strlen(in) + 1);
}

/* Print N spaces (used in debug for target-depth).  */

void
print_spaces (unsigned int n)
{
  while (n-- > 0)
    putchar (' ');
}


/* Return a string whose contents concatenate the NUM strings provided
   This string lives in static, re-used memory.  */

const char *
concat (unsigned int num, ...)
{
  static size_t rlen = 0;
  static char *result = NULL;
  size_t ri = 0;
  va_list args;

  va_start (args, num);

  while (num-- > 0)
    {
      const char *s = va_arg (args, const char *);
      size_t l = xstrlen (s);

      if (l == 0)
        continue;

      if (ri + l > rlen)
        {
          rlen = ((rlen ? rlen : 60) + l) * 2;
          result = xrealloc (result, rlen);
        }

      memcpy (result + ri, s, l);
      ri += l;
    }

  va_end (args);

  /* Get some more memory if we don't have enough space for the
     terminating '\0'.   */
  if (ri == rlen)
    {
      rlen = (rlen ? rlen : 60) * 2;
      result = xrealloc (result, rlen);
    }

  result[ri] = '\0';

  return result;
}


#ifndef HAVE_UNISTD_H
pid_t getpid ();
#endif

pid_t make_pid ()
{
  return getpid ();
}

/* Like malloc but get fatal error if memory is exhausted.  */
/* Don't bother if we're using dmalloc; it provides these for us.  */

#ifndef HAVE_DMALLOC_H

#undef xmalloc
#undef xcalloc
#undef xrealloc
#undef xstrdup

void *
xmalloc (size_t size)
{
  /* Make sure we don't allocate 0, for pre-ISO implementations.  */
  void *result = malloc (size ? size : 1);
  if (result == 0)
    out_of_memory ();
  return result;
}


void *
xcalloc (size_t size)
{
  /* Make sure we don't allocate 0, for pre-ISO implementations.  */
  void *result = calloc (size ? size : 1, 1);
  if (result == 0)
    out_of_memory ();
  return result;
}


void *
xrealloc (void *ptr, size_t size)
{
  void *result;

  /* Some older implementations of realloc() don't conform to ISO.  */
  if (! size)
    size = 1;
  result = ptr ? realloc (ptr, size) : malloc (size);
  if (result == 0)
    out_of_memory ();
  return result;
}


char *
xstrdup (const char *ptr)
{
  char *result;

#ifdef HAVE_STRDUP
  result = strdup (ptr);
#else
  result = malloc (strlen (ptr) + 1);
#endif

  if (result == 0)
    out_of_memory ();

#ifdef HAVE_STRDUP
  return result;
#else
  return strcpy (result, ptr);
#endif
}

#endif  /* HAVE_DMALLOC_H */

char *
xstrndup (const char *str, size_t length)
{
  char *result;

#ifdef HAVE_STRNDUP
  result = strndup (str, length);
  if (result == 0)
    out_of_memory ();
#else
  result = xmalloc (length + 1);
  if (length > 0)
    strncpy (result, str, length);
  result[length] = '\0';
#endif

  return result;
}

#ifndef HAVE_MEMRCHR
void *
memrchr(const void* str, int ch, size_t len)
{
  const char* sp = str;
  const char* cp = sp;

  if (len == 0)
    return NULL;

  cp += len - 1;

  while (cp[0] != ch)
    {
      if (cp == sp)
        return NULL;
      --cp;
    }

  return (void*)cp;
}
#endif



/* Limited INDEX:
   Search through the string STRING, which ends at LIMIT, for the character C.
   Returns a pointer to the first occurrence, or nil if none is found.
   Like INDEX except that the string searched ends where specified
   instead of at the first null.  */

char *
lindex (const char *s, const char *limit, int c)
{
  while (s < limit)
    if (*s++ == c)
      return (char *)(s - 1);

  return 0;
}

/* Return the address of the first whitespace or null in the string S.  */

char *
end_of_token (const char *s)
{
  while (! END_OF_TOKEN (*s))
    ++s;
  return (char *)s;
}

/* Return the address of the first nonwhitespace or null in the string S.  */

char *
next_token (const char *s)
{
  NEXT_TOKEN (s);
  return (char *)s;
}

/* Find the next token in PTR; return the address of it, and store the length
   of the token into *LENGTHPTR if LENGTHPTR is not nil.  Set *PTR to the end
   of the token, so this function can be called repeatedly in a loop.  */

char *
find_next_token (const char **ptr, size_t *lengthptr)
{
  const char *p = next_token (*ptr);

  if (*p == '\0')
    return 0;

  *ptr = end_of_token (p);
  if (lengthptr != 0)
    *lengthptr = *ptr - p;

  return (char *)p;
}

/* Write a BUFFER of size LEN to file descriptor FD.
   Retry short writes from EINTR.  Return LEN, or -1 on error.  */
ssize_t
writebuf (int fd, const void *buffer, size_t len)
{
  const char *msg = buffer;
  size_t l = len;
  while (l)
    {
      ssize_t r;

      EINTRLOOP (r, write (fd, msg, l));
      if (r < 0)
        return r;

      l -= r;
      msg += r;
    }

  return (ssize_t)len;
}

/* Read until we get LEN bytes from file descriptor FD, into BUFFER.
   Retry short reads on EINTR.  If we get an error, return it.
   Return 0 at EOF.  */
ssize_t
readbuf (int fd, void *buffer, size_t len)
{
  char *msg = buffer;
  while (len)
    {
      ssize_t r;

      EINTRLOOP (r, read (fd, msg, len));
      if (r < 0)
        return r;
      if (r == 0)
        break;

      len -= r;
      msg += r;
    }

  return (ssize_t)(msg - (char*)buffer);
}


/* Copy a chain of 'struct dep'.  For 2nd expansion deps, dup the name.  */

struct dep *
copy_dep_chain (const struct dep *d)
{
  struct dep *firstnew = 0;
  struct dep *lastnew = 0;

  while (d != 0)
    {
      struct dep *c = xmalloc (sizeof (struct dep));
      memcpy (c, d, sizeof (struct dep));

      if (c->need_2nd_expansion)
        c->name = xstrdup (c->name);

      c->next = 0;
      if (firstnew == 0)
        firstnew = lastnew = c;
      else
        lastnew = lastnew->next = c;

      d = d->next;
    }

  return firstnew;
}

/* Free a chain of struct nameseq.
   For struct dep chains use free_dep_chain.  */

void
free_ns_chain (struct nameseq *ns)
{
  while (ns != 0)
    {
      struct nameseq *t = ns;
      ns = ns->next;
      free_ns (t);
    }
}


#ifdef MAKE_MAINTAINER_MODE

void
spin (const char* type)
{
  char filenm[256];
  struct stat dummy;

  sprintf (filenm, ".make-spin-%s", type);

  if (stat (filenm, &dummy) == 0)
    {
      fprintf (stderr, "SPIN on %s\n", filenm);
      do
#ifdef WINDOWS32
        Sleep (1000);
#else
        sleep (1);
#endif
      while (stat (filenm, &dummy) == 0);
    }
}

void
dbg (const char *fmt, ...)
{
  FILE *fp = fopen ("/tmp/gmkdebug.log", "a+");
  va_list args;
  char buf[4096];

  va_start (args, fmt);
  vsprintf (buf, fmt, args);
  va_end (args);

  fprintf(fp, "%u: %s\n", (unsigned) make_pid (), buf);
  fflush (fp);
  fclose (fp);
}

#endif



/* Provide support for temporary files.  */

#ifndef HAVE_STDLIB_H
# ifdef HAVE_MKSTEMP
int mkstemp (char *template);
# else
char *mktemp (char *template);
# endif
#endif

#ifndef HAVE_UMASK
mode_t
umask (mode_t mask)
{
  return 0;
}
#endif

#ifdef VMS
# define DEFAULT_TMPFILE    "sys$scratch:gnv$make_cmdXXXXXX.com"
#else
# define DEFAULT_TMPFILE    "GmXXXXXX"
#endif

const char *
get_tmpdir ()
{
  static const char *tmpdir = NULL;

  if (!tmpdir)
    {
#if defined (__MSDOS__) || defined (WINDOWS32) || defined (__EMX__)
# define TMP_EXTRAS   "TMP", "TEMP",
#else
# define TMP_EXTRAS
#endif
      const char *tlist[] = { "MAKE_TMPDIR", "TMPDIR", TMP_EXTRAS NULL };
      const char **tp;
      unsigned int found = 0;

      for (tp = tlist; *tp; ++tp)
        if ((tmpdir = getenv (*tp)) && *tmpdir != '\0')
          {
            struct stat st;
            int r;
            found = 1;
            EINTRLOOP(r, stat (tmpdir, &st));
            if (r < 0)
              OSSS (error, NILF,
                    _("%s value %s: %s"), *tp, tmpdir, strerror (errno));
            else if (! S_ISDIR (st.st_mode))
              OSS (error, NILF,
                   _("%s value %s: not a directory"), *tp, tmpdir);
            else
              return tmpdir;
          }

      tmpdir = DEFAULT_TMPDIR;

      if (found)
        OS (error, NILF, _("using default temporary directory '%s'"), tmpdir);
    }

  return tmpdir;
}

static char *
get_tmptemplate ()
{
  const char *tmpdir = get_tmpdir ();
  char *template;
  char *cp;

  template = xmalloc (strlen (tmpdir) + CSTRLEN (DEFAULT_TMPFILE) + 2);
  cp = stpcpy (template, tmpdir);

#if !defined VMS
  /* It's not possible for tmpdir to be empty.  */
  if (! ISDIRSEP (cp[-1]))
    *(cp++) = '/';
#endif

  strcpy (cp, DEFAULT_TMPFILE);

  return template;
}

#if !HAVE_MKSTEMP || !HAVE_FDOPEN
/* Generate a temporary filename.  This is not safe as another program could
   snipe our filename after we've generated it: use this only on systems
   without more secure alternatives.  */

static char *
get_tmppath ()
{
  char *path;

# ifdef HAVE_MKTEMP
  path = get_tmptemplate ();
  if (*mktemp (path) == '\0')
    {
      OSS (error, NILF,
           _("cannot generate temp path from %s: %s"), path, strerror (errno));
      return NULL;
    }
# else
  path = xmalloc (L_tmpnam + 1);
  if (tmpnam (path) == NULL)
    {
      OS (error, NILF,
        _("cannot generate temp name: %s"), strerror (errno));
      return NULL;
    }
# endif

  return path;
}
#endif

/* Generate a temporary file and return an fd for it.  If name is NULL then
   the temp file is anonymous and will be deleted when the process exits.  If
   name is not null then *name will point to an allocated buffer, or set to
   NULL on failure.  */
int
get_tmpfd (char **name)
{
  int fd = -1;
  char *tmpnm;
  mode_t mask;

  if (name)
    *name = NULL;
  else
    {
      /* If there's an os-specific way to get an anonymous temp file use it.  */
      fd = os_anontmp ();
      if (fd >= 0)
        return fd;
    }

  /* Preserve the current umask, and set a restrictive one for temp files.
     Only really needed for mkstemp() but won't hurt for the open method.  */
  mask = umask (0077);

#if defined(HAVE_MKSTEMP)
  tmpnm = get_tmptemplate ();

  /* It's safest to use mkstemp(), if we can.  */
  EINTRLOOP (fd, mkstemp (tmpnm));
#else
  tmpnm = get_tmppath ();
  if (!tmpnm)
    return -1;

  /* Can't use mkstemp(), but try to guard against a race condition.  */
  EINTRLOOP (fd, open (tmpnm, O_CREAT|O_EXCL|O_RDWR, 0600));
#endif
  if (fd < 0)
    {
      OSS (error, NILF,
           _("cannot create temporary file %s: %s"), tmpnm, strerror (errno));
      free (tmpnm);
      return -1;
    }

  if (name)
    *name = tmpnm;
  else
    {
      int r;
      EINTRLOOP (r, unlink (tmpnm));
      if (r < 0)
        OSS (error, NILF,
             _("cannot unlink temporary file %s: %s"), tmpnm, strerror (errno));
      free (tmpnm);
    }

  umask (mask);

  return fd;
}

/* Return a FILE* for a temporary file, opened in the safest way possible.
   Set name to point to an allocated buffer containing the name of the file,
   or NULL on failure.  Note, name cannot be NULL!  */
FILE *
get_tmpfile (char **name)
{
  /* Be consistent with tmpfile, which opens as if by "wb+".  */
  const char *tmpfile_mode = "wb+";
  FILE *file;

#if defined(HAVE_FDOPEN)
  int fd;
  assert (name);
  fd = get_tmpfd (name);
  if (fd < 0)
    return NULL;
  assert (*name);

  ENULLLOOP (file, fdopen (fd, tmpfile_mode));
  if (file == NULL)
    OSS (error, NILF,
         _("fdopen: temporary file %s: %s"), *name, strerror (errno));
#else
  /* Preserve the current umask, and set a restrictive one for temp files.  */
  mode_t mask = umask (0077);

  assert (name);
  *name = get_tmppath ();
  if (!*name)
    return NULL;

  /* Although this fopen is insecure, it is executed only on non-fdopen
     platforms, which should be a rarity nowadays.  */

  ENULLLOOP (file, fopen (*name, tmpfile_mode));
  if (file == NULL)
    {
      OSS (error, NILF,
           _("fopen: temporary file %s: %s"), *name, strerror (errno));
      free (*name);
      *name = NULL;
    }

  umask (mask);
#endif

  return file;
}


#if HAVE_TTYNAME && defined(__EMX__)
/* OS/2 kLIBC has a declaration for ttyname(), so configure finds it.
   But, it is not implemented!  Roll our own.  */
char *ttyname (int fd)
{
  ULONG type;
  ULONG attr;
  ULONG rc;

  rc = DosQueryHType (fd, &type, &attr);
  if (rc)
    {
      errno = EBADF;
      return NULL;
    }

  if (type == HANDTYPE_DEVICE)
    {
      if (attr & 3)     /* 1 = KBD$, 2 = SCREEN$ */
        return (char *) "/dev/con";

      if (attr & 4)     /* 4 = NUL */
        return (char *) "/dev/nul";

      if (attr & 8)     /* 8 = CLOCK$ */
        return (char *) "/dev/clock$";
    }

  errno = ENOTTY;
  return NULL;
}
#endif


#if !HAVE_STRCASECMP && !HAVE_STRICMP && !HAVE_STRCMPI
/* If we don't have strcasecmp() (from POSIX), or anything that can substitute
   for it, define our own version.  */

int
strcasecmp (const char *s1, const char *s2)
{
  while (1)
    {
      int c1 = (unsigned char) *(s1++);
      int c2 = (unsigned char) *(s2++);

      if (isalpha (c1))
        c1 = tolower (c1);
      if (isalpha (c2))
        c2 = tolower (c2);

      if (c1 != '\0' && c1 == c2)
        continue;

      return (c1 - c2);
    }
}
#endif

#if !HAVE_STRNCASECMP && !HAVE_STRNICMP && !HAVE_STRNCMPI
/* If we don't have strncasecmp() (from POSIX), or anything that can
   substitute for it, define our own version.  */

int
strncasecmp (const char *s1, const char *s2, size_t n)
{
  while (n-- > 0)
    {
      int c1 = (unsigned char) *(s1++);
      int c2 = (unsigned char) *(s2++);

      if (isalpha (c1))
        c1 = tolower (c1);
      if (isalpha (c2))
        c2 = tolower (c2);

      if (c1 != '\0' && c1 == c2)
        continue;

      return (c1 - c2);
    }

  return 0;
}
#endif


#ifdef NEED_GET_PATH_MAX
unsigned int
get_path_max (void)
{
  static unsigned int value;

  if (value == 0)
    {
      long x = pathconf ("/", _PC_PATH_MAX);
      if (x > 0)
        value = (unsigned int) x;
      else
        value = PATH_MAX;
    }

  return value;
}
#endif

#if !HAVE_MEMPCPY
void *
mempcpy (void *dest, const void *src, size_t n)
{
  return (char *) memcpy (dest, src, n) + n;
}
#endif

#if !HAVE_STPCPY
char *
stpcpy (char *dest, const char *src)
{
  char *d = dest;
  const char *s = src;

  do
    *d++ = *s;
  while (*s++ != '\0');

  return d - 1;
}
#endif

#if !HAVE_STRTOLL
# undef UNSIGNED
# undef USE_NUMBER_GROUPING
# undef USE_WIDE_CHAR
# define QUAD 1
# include <strtol.c>
#endif

#if !HAVE_STRERROR
char *
strerror (int errnum)
{
  static char msg[256];

#define SETMSG(_e, _m) case _e: strcpy(msg, _m); break

  switch (errnum)
    {
#ifdef EPERM
    SETMSG (EPERM  , "Operation not permitted");
#endif
#ifdef ENOENT
    SETMSG (ENOENT , "No such file or directory");
#endif
#ifdef ESRCH
    SETMSG (ESRCH  , "No such process");
#endif
#ifdef EINTR
    SETMSG (EINTR  , "Interrupted system call");
#endif
#ifdef EIO
    SETMSG (EIO    , "I/O error");
#endif
#ifdef ENXIO
    SETMSG (ENXIO  , "No such device or address");
#endif
#ifdef E2BIG
    SETMSG (E2BIG  , "Argument list too long");
#endif
#ifdef ENOEXEC
    SETMSG (ENOEXEC, "Exec format error");
#endif
#ifdef EBADF
    SETMSG (EBADF  , "Bad file number");
#endif
#ifdef ECHILD
    SETMSG (ECHILD , "No child processes");
#endif
#ifdef EAGAIN
    SETMSG (EAGAIN , "Try again");
#endif
#ifdef ENOMEM
    SETMSG (ENOMEM , "Out of memory");
#endif
#ifdef EACCES
    SETMSG (EACCES , "Permission denied");
#endif
#ifdef EFAULT
    SETMSG (EFAULT , "Bad address");
#endif
#ifdef ENOTBLK
    SETMSG (ENOTBLK, "Block device required");
#endif
#ifdef EBUSY
    SETMSG (EBUSY  , "Device or resource busy");
#endif
#ifdef EEXIST
    SETMSG (EEXIST , "File exists");
#endif
#ifdef EXDEV
    SETMSG (EXDEV  , "Cross-device link");
#endif
#ifdef ENODEV
    SETMSG (ENODEV , "No such device");
#endif
#ifdef ENOTDIR
    SETMSG (ENOTDIR, "Not a directory");
#endif
#ifdef EISDIR
    SETMSG (EISDIR , "Is a directory");
#endif
#ifdef EINVAL
    SETMSG (EINVAL , "Invalid argument");
#endif
#ifdef ENFILE
    SETMSG (ENFILE , "File table overflow");
#endif
#ifdef EMFILE
    SETMSG (EMFILE , "Too many open files");
#endif
#ifdef ENOTTY
    SETMSG (ENOTTY , "Not a typewriter");
#endif
#ifdef ETXTBSY
    SETMSG (ETXTBSY, "Text file busy");
#endif
#ifdef EFBIG
    SETMSG (EFBIG  , "File too large");
#endif
#ifdef ENOSPC
    SETMSG (ENOSPC , "No space left on device");
#endif
#ifdef ESPIPE
    SETMSG (ESPIPE , "Illegal seek");
#endif
#ifdef EROFS
    SETMSG (EROFS  , "Read-only file system");
#endif
#ifdef EMLINK
    SETMSG (EMLINK , "Too many links");
#endif
#ifdef EPIPE
    SETMSG (EPIPE  , "Broken pipe");
#endif
#ifdef EDOM
    SETMSG (EDOM   , "Math argument out of domain of func");
#endif
#ifdef ERANGE
    SETMSG (ERANGE , "Math result not representable");
#endif
    default: sprintf (msg, "Unknown error %d", errnum); break;
    }

  return msg;
}
#endif
