/* Miscellaneous generic support functions for GNU Make.
Copyright (C) 1988-2020 Free Software Foundation, Inc.
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

#include "makeint.h"
#include "filedef.h"
#include "dep.h"
#include "debug.h"

/* GNU make no longer supports pre-ANSI89 environments.  */

#include <stdarg.h>

#ifdef WINDOWS32
# include <windows.h>
# include <io.h>
#endif

#ifdef HAVE_FCNTL_H
# include <fcntl.h>
#else
# include <sys/file.h>
#endif

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
             In traditional GNU make all trailing whitespace, consecutive
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
  END_OF_TOKEN (s);
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

FILE *
get_tmpfile (char **name, const char *template)
{
  FILE *file;
#ifdef HAVE_FDOPEN
  int fd;
#endif

  /* Preserve the current umask, and set a restrictive one for temp files.  */
  mode_t mask = umask (0077);

#if defined(HAVE_MKSTEMP) || defined(HAVE_MKTEMP)
# define TEMPLATE_LEN   strlen (template)
#else
# define TEMPLATE_LEN   L_tmpnam
#endif
  *name = xmalloc (TEMPLATE_LEN + 1);
  strcpy (*name, template);

#if defined(HAVE_MKSTEMP) && defined(HAVE_FDOPEN)
  /* It's safest to use mkstemp(), if we can.  */
  EINTRLOOP (fd, mkstemp (*name));
  if (fd == -1)
    file = NULL;
  else
    file = fdopen (fd, "w");
#else
# ifdef HAVE_MKTEMP
  (void) mktemp (*name);
# else
  (void) tmpnam (*name);
# endif

# ifdef HAVE_FDOPEN
  /* Can't use mkstemp(), but guard against a race condition.  */
  EINTRLOOP (fd, open (*name, O_CREAT|O_EXCL|O_WRONLY, 0600));
  if (fd == -1)
    return 0;
  file = fdopen (fd, "w");
# else
  /* Not secure, but what can we do?  */
  file = fopen (*name, "w");
# endif
#endif

  umask (mask);

  return file;
}


#if !HAVE_STRCASECMP && !HAVE_STRICMP && !HAVE_STRCMPI
/* If we don't have strcasecmp() (from POSIX), or anything that can substitute
   for it, define our own version.  */

int
strcasecmp (const char *s1, const char *s2)
{
  while (1)
    {
      int c1 = (int) *(s1++);
      int c2 = (int) *(s2++);

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
strncasecmp (const char *s1, const char *s2, int n)
{
  while (n-- > 0)
    {
      int c1 = (int) *(s1++);
      int c2 = (int) *(s2++);

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

#ifdef  GETLOADAVG_PRIVILEGED

#ifdef POSIX

/* Hopefully if a system says it's POSIX.1 and has the setuid and setgid
   functions, they work as POSIX.1 says.  Some systems (Alpha OSF/1 1.2,
   for example) which claim to be POSIX.1 also have the BSD setreuid and
   setregid functions, but they don't work as in BSD and only the POSIX.1
   way works.  */

#undef HAVE_SETREUID
#undef HAVE_SETREGID

#else   /* Not POSIX.  */

/* Some POSIX.1 systems have the seteuid and setegid functions.  In a
   POSIX-like system, they are the best thing to use.  However, some
   non-POSIX systems have them too but they do not work in the POSIX style
   and we must use setreuid and setregid instead.  */

#undef HAVE_SETEUID
#undef HAVE_SETEGID

#endif  /* POSIX.  */

#ifndef HAVE_UNISTD_H
extern int getuid (), getgid (), geteuid (), getegid ();
extern int setuid (), setgid ();
#ifdef HAVE_SETEUID
extern int seteuid ();
#else
#ifdef  HAVE_SETREUID
extern int setreuid ();
#endif  /* Have setreuid.  */
#endif  /* Have seteuid.  */
#ifdef HAVE_SETEGID
extern int setegid ();
#else
#ifdef  HAVE_SETREGID
extern int setregid ();
#endif  /* Have setregid.  */
#endif  /* Have setegid.  */
#endif  /* No <unistd.h>.  */

/* Keep track of the user and group IDs for user- and make- access.  */
static int user_uid = -1, user_gid = -1, make_uid = -1, make_gid = -1;
#define access_inited   (user_uid != -1)
static enum { make, user } current_access;


/* Under -d, write a message describing the current IDs.  */

static void
log_access (const char *flavor)
{
  if (! ISDB (DB_JOBS))
    return;

  /* All the other debugging messages go to stdout,
     but we write this one to stderr because it might be
     run in a child fork whose stdout is piped.  */

  fprintf (stderr, _("%s: user %lu (real %lu), group %lu (real %lu)\n"),
           flavor, (unsigned long) geteuid (), (unsigned long) getuid (),
           (unsigned long) getegid (), (unsigned long) getgid ());
  fflush (stderr);
}


static void
init_access (void)
{
#ifndef VMS
  user_uid = getuid ();
  user_gid = getgid ();

  make_uid = geteuid ();
  make_gid = getegid ();

  /* Do these ever fail?  */
  if (user_uid == -1 || user_gid == -1 || make_uid == -1 || make_gid == -1)
    pfatal_with_name ("get{e}[gu]id");

  log_access (_("Initialized access"));

  current_access = make;
#endif
}

#endif  /* GETLOADAVG_PRIVILEGED */

/* Give the process appropriate permissions for access to
   user data (i.e., to stat files, or to spawn a child process).  */
void
user_access (void)
{
#ifdef  GETLOADAVG_PRIVILEGED

  if (!access_inited)
    init_access ();

  if (current_access == user)
    return;

  /* We are in "make access" mode.  This means that the effective user and
     group IDs are those of make (if it was installed setuid or setgid).
     We now want to set the effective user and group IDs to the real IDs,
     which are the IDs of the process that exec'd make.  */

#ifdef  HAVE_SETEUID

  /* Modern systems have the seteuid/setegid calls which set only the
     effective IDs, which is ideal.  */

  if (seteuid (user_uid) < 0)
    pfatal_with_name ("user_access: seteuid");

#else   /* Not HAVE_SETEUID.  */

#ifndef HAVE_SETREUID

  /* System V has only the setuid/setgid calls to set user/group IDs.
     There is an effective ID, which can be set by setuid/setgid.
     It can be set (unless you are root) only to either what it already is
     (returned by geteuid/getegid, now in make_uid/make_gid),
     the real ID (return by getuid/getgid, now in user_uid/user_gid),
     or the saved set ID (what the effective ID was before this set-ID
     executable (make) was exec'd).  */

  if (setuid (user_uid) < 0)
    pfatal_with_name ("user_access: setuid");

#else   /* HAVE_SETREUID.  */

  /* In 4BSD, the setreuid/setregid calls set both the real and effective IDs.
     They may be set to themselves or each other.  So you have two alternatives
     at any one time.  If you use setuid/setgid, the effective will be set to
     the real, leaving only one alternative.  Using setreuid/setregid, however,
     you can toggle between your two alternatives by swapping the values in a
     single setreuid or setregid call.  */

  if (setreuid (make_uid, user_uid) < 0)
    pfatal_with_name ("user_access: setreuid");

#endif  /* Not HAVE_SETREUID.  */
#endif  /* HAVE_SETEUID.  */

#ifdef  HAVE_SETEGID
  if (setegid (user_gid) < 0)
    pfatal_with_name ("user_access: setegid");
#else
#ifndef HAVE_SETREGID
  if (setgid (user_gid) < 0)
    pfatal_with_name ("user_access: setgid");
#else
  if (setregid (make_gid, user_gid) < 0)
    pfatal_with_name ("user_access: setregid");
#endif
#endif

  current_access = user;

  log_access (_("User access"));

#endif  /* GETLOADAVG_PRIVILEGED */
}

/* Give the process appropriate permissions for access to
   make data (i.e., the load average).  */
void
make_access (void)
{
#ifdef  GETLOADAVG_PRIVILEGED

  if (!access_inited)
    init_access ();

  if (current_access == make)
    return;

  /* See comments in user_access, above.  */

#ifdef  HAVE_SETEUID
  if (seteuid (make_uid) < 0)
    pfatal_with_name ("make_access: seteuid");
#else
#ifndef HAVE_SETREUID
  if (setuid (make_uid) < 0)
    pfatal_with_name ("make_access: setuid");
#else
  if (setreuid (user_uid, make_uid) < 0)
    pfatal_with_name ("make_access: setreuid");
#endif
#endif

#ifdef  HAVE_SETEGID
  if (setegid (make_gid) < 0)
    pfatal_with_name ("make_access: setegid");
#else
#ifndef HAVE_SETREGID
  if (setgid (make_gid) < 0)
    pfatal_with_name ("make_access: setgid");
#else
  if (setregid (user_gid, make_gid) < 0)
    pfatal_with_name ("make_access: setregid");
#endif
#endif

  current_access = make;

  log_access (_("Make access"));

#endif  /* GETLOADAVG_PRIVILEGED */
}

/* Give the process appropriate permissions for a child process.
   This is like user_access, but you can't get back to make_access.  */
void
child_access (void)
{
#ifdef  GETLOADAVG_PRIVILEGED

  if (!access_inited)
    abort ();

  /* Set both the real and effective UID and GID to the user's.
     They cannot be changed back to make's.  */

#ifndef HAVE_SETREUID
  if (setuid (user_uid) < 0)
    pfatal_with_name ("child_access: setuid");
#else
  if (setreuid (user_uid, user_uid) < 0)
    pfatal_with_name ("child_access: setreuid");
#endif

#ifndef HAVE_SETREGID
  if (setgid (user_gid) < 0)
    pfatal_with_name ("child_access: setgid");
#else
  if (setregid (user_gid, user_gid) < 0)
    pfatal_with_name ("child_access: setregid");
#endif

  log_access (_("Child access"));

#endif  /* GETLOADAVG_PRIVILEGED */
}

#ifdef NEED_GET_PATH_MAX
unsigned int
get_path_max (void)
{
  static unsigned int value;

  if (value == 0)
    {
      long int x = pathconf ("/", _PC_PATH_MAX);
      if (x > 0)
        value = x;
      else
        return MAXPATHLEN;
    }

  return value;
}
#endif
