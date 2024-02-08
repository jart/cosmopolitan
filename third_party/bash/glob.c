/* glob.c -- file-name wildcard pattern matching for Bash.

   Copyright (C) 1985-2021 Free Software Foundation, Inc.

   This file is part of GNU Bash, the Bourne-Again SHell.
   
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

/* To whomever it may concern: I have never seen the code which most
   Unix programs use to perform this function.  I wrote this from scratch
   based on specifications for the pattern matching.  --RMS.  */

#include "config.h"

#if !defined (__GNUC__) && !defined (HAVE_ALLOCA_H) && defined (_AIX)
  #pragma alloca
#endif /* _AIX && RISC6000 && !__GNUC__ */

#include "bashtypes.h"

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#include "bashansi.h"
#include "posixdir.h"
#include "posixstat.h"
#include "shmbutil.h"
#include "xmalloc.h"

#include "filecntl.h"
#if !defined (F_OK)
#  define F_OK 0
#endif

#include "stdc.h"
#include "memalloc.h"

#include <signal.h>

#include "shell.h"
#include "general.h"

#include "glob.h"
#include "strmatch.h"

#if !defined (HAVE_BCOPY) && !defined (bcopy)
#  define bcopy(s, d, n) ((void) memcpy ((d), (s), (n)))
#endif /* !HAVE_BCOPY && !bcopy */

#if !defined (NULL)
#  if defined (__STDC__)
#    define NULL ((void *) 0)
#  else
#    define NULL 0x0
#  endif /* __STDC__ */
#endif /* !NULL */

#if !defined (FREE)
#  define FREE(x)	if (x) free (x)
#endif

/* Don't try to alloca() more than this much memory for `struct globval'
   in glob_vector() */
#ifndef ALLOCA_MAX
#  define ALLOCA_MAX	100000
#endif

struct globval
  {
    struct globval *next;
    char *name;
  };

extern void throw_to_top_level PARAMS((void));
extern int sh_eaccess PARAMS((const char *, int));
extern char *sh_makepath PARAMS((const char *, const char *, int));
extern int signal_is_pending PARAMS((int));
extern void run_pending_traps PARAMS((void));

extern int extended_glob;

/* Global variable which controls whether or not * matches .*.
   Non-zero means don't match .*.  */
int noglob_dot_filenames = 1;

/* Global variable which controls whether or not filename globbing
   is done without regard to case. */
int glob_ignore_case = 0;

/* Global variable controlling whether globbing ever returns . or ..
   regardless of the pattern. If set to 1, no glob pattern will ever
   match `.' or `..'. Disabled by default. */
int glob_always_skip_dot_and_dotdot = 1;

/* Global variable to return to signify an error in globbing. */
char *glob_error_return;

static struct globval finddirs_error_return;

/* Some forward declarations. */
static int skipname PARAMS((char *, char *, int));
#if HANDLE_MULTIBYTE
static int mbskipname PARAMS((char *, char *, int));
#endif
void udequote_pathname PARAMS((char *));
#if HANDLE_MULTIBYTE
void wcdequote_pathname PARAMS((wchar_t *));
static void wdequote_pathname PARAMS((char *));
static void dequote_pathname PARAMS((char *));
#else
#  define dequote_pathname(p) udequote_pathname(p)
#endif
static int glob_testdir PARAMS((char *, int));
static char **glob_dir_to_array PARAMS((char *, char **, int));

/* Make sure these names continue to agree with what's in smatch.c */
extern char *glob_patscan PARAMS((char *, char *, int));
extern wchar_t *glob_patscan_wc PARAMS((wchar_t *, wchar_t *, int));

/* And this from gmisc.c/gm_loop.c */
extern int wextglob_pattern_p PARAMS((wchar_t *));

extern char *glob_dirscan PARAMS((char *, int));

/* Compile `glob_loop.inc' for single-byte characters. */
#define GCHAR	unsigned char
#define CHAR	char
#define INT	int
#define L(CS)	CS
#define INTERNAL_GLOB_PATTERN_P internal_glob_pattern_p
#include "glob_loop.inc"

/* Compile `glob_loop.inc' again for multibyte characters. */
#if HANDLE_MULTIBYTE

#define GCHAR	wchar_t
#define CHAR	wchar_t
#define INT	wint_t
#define L(CS)	L##CS
#define INTERNAL_GLOB_PATTERN_P internal_glob_wpattern_p
#include "glob_loop.inc"

#endif /* HANDLE_MULTIBYTE */

/* And now a function that calls either the single-byte or multibyte version
   of internal_glob_pattern_p. */
int
glob_pattern_p (pattern)
     const char *pattern;
{
#if HANDLE_MULTIBYTE
  size_t n;
  wchar_t *wpattern;
  int r;

  if (MB_CUR_MAX == 1 || mbsmbchar (pattern) == 0)
    return (internal_glob_pattern_p ((unsigned char *)pattern));

  /* Convert strings to wide chars, and call the multibyte version. */
  n = xdupmbstowcs (&wpattern, NULL, pattern);
  if (n == (size_t)-1)
    /* Oops.  Invalid multibyte sequence.  Try it as single-byte sequence. */
    return (internal_glob_pattern_p ((unsigned char *)pattern));

  r = internal_glob_wpattern_p (wpattern);
  free (wpattern);

  return r;
#else
  return (internal_glob_pattern_p ((unsigned char *)pattern));
#endif
}

#if EXTENDED_GLOB

#if defined (HANDLE_MULTIBYTE)
#  define XSKIPNAME(p, d, f)	mbskipname(p, d, f)
#else
#  define XSKIPNAME(p, d, f)	skipname(p, d, f)
#endif

/* Return 1 if all subpatterns in the extended globbing pattern PAT indicate
   that the name should be skipped.  XXX - doesn't handle pattern negation,
   not sure if it should */
static int
extglob_skipname (pat, dname, flags)
     char *pat, *dname;
     int flags;
{
  char *pp, *pe, *t, *se;
  int n, r, negate, wild, nullpat, xflags;

  negate = *pat == '!';
  wild = *pat == '*' || *pat == '?';
  pp = pat + 2;
  se = pp + strlen (pp);		/* end of pattern string */
  pe = glob_patscan (pp, se, 0);	/* end of extglob pattern */

  /* if pe == 0, this is an invalid extglob pattern */
  if (pe == 0)
    return 0;

  xflags = flags | ( negate ? GX_NEGATE : 0);

  /* if pe != se we have more of the pattern at the end of the extglob
     pattern. Check the easy case first ( */
  if (pe == se && *pe == 0 && pe[-1] == ')' && (t = strchr (pp, '|')) == 0)
    {
      pe[-1] = '\0';
      /* This is where we check whether the pattern is being negated and
	 match all files beginning with `.' if the pattern begins with a
	 literal `.'. */
      r = XSKIPNAME (pp, dname, xflags); /*(*/
      pe[-1] = ')';
      return r;
    }

  /* Is the extglob pattern between the parens the null pattern?  The null
     pattern can match nothing, so should we check any remaining portion of
     the pattern? */
  nullpat = pe >= (pat + 2) && pe[-2] == '(' && pe[-1] == ')';

  /* check every subpattern */
  while (t = glob_patscan (pp, pe, '|'))
    {
      /* If T == PE and *T == 0 (&& PE[-1] == RPAREN), we have hit the end
	 of a pattern with no trailing characters. */
      n = t[-1];	/* ( */
      if (extglob_pattern_p (pp) && n == ')')		/* nested extglob? */
	t[-1] = n;	/* no-op for now */
      else
	t[-1] = '\0';
      r = XSKIPNAME (pp, dname, xflags);
      t[-1] = n;
      if (r == 0)	/* if any pattern says not skip, we don't skip */
        return r;
      pp = t;
      if (pp == pe)
	break;
    }

  /* glob_patscan might find end of string */
  if (pp == se)
    return r;

  /* but if it doesn't then we didn't match a leading dot */
  if (wild && *pe)	/* if we can match zero instances, check further */
    return (XSKIPNAME (pe, dname, flags));

  return 1;
}
#endif

/* Return 1 if DNAME should be skipped according to PAT.  Mostly concerned
   with matching leading `.'. */
static int
skipname (pat, dname, flags)
     char *pat;
     char *dname;
     int flags;
{
  int i;

#if EXTENDED_GLOB
  if (extglob_pattern_p (pat))		/* XXX */
    return (extglob_skipname (pat, dname, flags));
#endif

  if (glob_always_skip_dot_and_dotdot && DOT_OR_DOTDOT (dname))
    return 1;

  /* If a leading dot need not be explicitly matched, and the pattern
     doesn't start with a `.', don't match `.' or `..' */
  if (noglob_dot_filenames == 0 && pat[0] != '.' &&
	(pat[0] != '\\' || pat[1] != '.') &&
	DOT_OR_DOTDOT (dname))
    return 1;

#if 0
  /* This is where we check whether the pattern is being negated and
     match all files beginning with `.' if the pattern begins with a
     literal `.'. This is the negation of the next clause. */
  else if ((flags & GX_NEGATE) && noglob_dot_filenames == 0 &&
	dname[0] == '.' &&
	(pat[0] == '.' || (pat[0] == '\\' && pat[1] == '.')))
    return 0;
#endif

  /* If a dot must be explicitly matched, check to see if they do. */
  else if (noglob_dot_filenames && dname[0] == '.' &&
 	   pat[0] != '.' && (pat[0] != '\\' || pat[1] != '.'))
    return 1;

  return 0;
}

#if HANDLE_MULTIBYTE

static int
wskipname (pat, dname, flags)
     wchar_t *pat, *dname;
     int flags;
{
  int i;

  if (glob_always_skip_dot_and_dotdot && WDOT_OR_DOTDOT (dname))
    return 1;

  /* If a leading dot need not be explicitly matched, and the
     pattern doesn't start with a `.', don't match `.' or `..' */
  if (noglob_dot_filenames == 0 && pat[0] != L'.' &&
	(pat[0] != L'\\' || pat[1] != L'.') &&
	WDOT_OR_DOTDOT (dname))
    return 1;

#if 0
  /* This is where we check whether the pattern is being negated and
     match all files beginning with `.' if the pattern begins with a
     literal `.'. This is the negation of the next clause. */
  else if ((flags & GX_NEGATE) && noglob_dot_filenames == 0 &&
	dname[0] == L'.' &&
	(pat[0] == L'.' || (pat[0] == L'\\' && pat[1] == L'.')))
    return 0;
#endif

  /* If a leading dot must be explicitly matched, check to see if the
     pattern and dirname both have one. */
  else if (noglob_dot_filenames && dname[0] == L'.' &&
	pat[0] != L'.' && (pat[0] != L'\\' || pat[1] != L'.'))
    return 1;

  return 0;
}

static int
wextglob_skipname (pat, dname, flags)
     wchar_t *pat, *dname;
     int flags;
{
#if EXTENDED_GLOB
  wchar_t *pp, *pe, *t, *se, n;
  int r, negate, wild, nullpat, xflags;

  negate = *pat == L'!';
  wild = *pat == L'*' || *pat == L'?';
  pp = pat + 2;
  se = pp + wcslen (pp);
  pe = glob_patscan_wc (pp, se, 0);

  /* if pe == 0, this is an invalid extglob pattern */
  if (pe == 0)
    return 0;

  xflags = flags | ( negate ? GX_NEGATE : 0);

  /* if pe != se we have more of the pattern at the end of the extglob
     pattern. Check the easy case first ( */
  if (pe == se && *pe == L'\0' && pe[-1] == L')' && (t = wcschr (pp, L'|')) == 0)
    {
      pe[-1] = L'\0';
      r = wskipname (pp, dname, xflags); /*(*/
      pe[-1] = L')';
      return r;
    }

  /* Is the extglob pattern between the parens the null pattern?  The null
     pattern can match nothing, so should we check any remaining portion of
     the pattern? */
  nullpat = pe >= (pat + 2) && pe[-2] == L'(' && pe[-1] == L')';

  /* check every subpattern */
  while (t = glob_patscan_wc (pp, pe, '|'))
    {
      n = t[-1];	/* ( */
      if (wextglob_pattern_p (pp) && n == L')')		/* nested extglob? */
	t[-1] = n;	/* no-op for now */
      else
	t[-1] = L'\0';
      r = wskipname (pp, dname, xflags);
      t[-1] = n;
      if (r == 0)
	return 0;
      pp = t;
      if (pp == pe)
	break;
    }

  /* glob_patscan_wc might find end of string */
  if (pp == se)
    return r;

  /* but if it doesn't then we didn't match a leading dot */
  if (wild && *pe != L'\0')
    return (wskipname (pe, dname, flags));

  return 1;
#else
  return (wskipname (pat, dname, flags));
#endif
}

/* Return 1 if DNAME should be skipped according to PAT.  Handles multibyte
   characters in PAT and DNAME.  Mostly concerned with matching leading `.'. */
static int
mbskipname (pat, dname, flags)
     char *pat, *dname;
     int flags;
{
  int ret, ext;
  wchar_t *pat_wc, *dn_wc;
  size_t pat_n, dn_n;

  if (mbsmbchar (dname) == 0 && mbsmbchar (pat) == 0)
    return (skipname (pat, dname, flags));

  ext = 0;
#if EXTENDED_GLOB
  ext = extglob_pattern_p (pat);
#endif

  pat_wc = dn_wc = (wchar_t *)NULL;

  pat_n = xdupmbstowcs (&pat_wc, NULL, pat);
  if (pat_n != (size_t)-1)
    dn_n = xdupmbstowcs (&dn_wc, NULL, dname);

  ret = 0;
  if (pat_n != (size_t)-1 && dn_n !=(size_t)-1)
    ret = ext ? wextglob_skipname (pat_wc, dn_wc, flags) : wskipname (pat_wc, dn_wc, flags);
  else
    ret = skipname (pat, dname, flags);

  FREE (pat_wc);
  FREE (dn_wc);

  return ret;
}
#endif /* HANDLE_MULTIBYTE */

/* Remove backslashes quoting characters in PATHNAME by modifying PATHNAME. */
void
udequote_pathname (pathname)
     char *pathname;
{
  register int i, j;

  for (i = j = 0; pathname && pathname[i]; )
    {
      if (pathname[i] == '\\')
	i++;

      pathname[j++] = pathname[i++];

      if (pathname[i - 1] == 0)
	break;
    }
  if (pathname)
    pathname[j] = '\0';
}

#if HANDLE_MULTIBYTE
/* Remove backslashes quoting characters in PATHNAME by modifying PATHNAME. */
void
wcdequote_pathname (wpathname)
     wchar_t *wpathname;
{
  int i, j;

  for (i = j = 0; wpathname && wpathname[i]; )
    {
      if (wpathname[i] == L'\\')
	i++;

      wpathname[j++] = wpathname[i++];

      if (wpathname[i - 1] == L'\0')
	break;
    }
  if (wpathname)
    wpathname[j] = L'\0';
}

static void
wdequote_pathname (pathname)
     char *pathname;
{
  mbstate_t ps;
  size_t len, n;
  wchar_t *wpathname;
  int i, j;
  wchar_t *orig_wpathname;

  if (mbsmbchar (pathname) == 0)
    {
      udequote_pathname (pathname);
      return;
    }

  len = strlen (pathname);
  /* Convert the strings into wide characters.  */
  n = xdupmbstowcs (&wpathname, NULL, pathname);
  if (n == (size_t) -1)
    {
      /* Something wrong.  Fall back to single-byte */
      udequote_pathname (pathname);
      return;
    }
  orig_wpathname = wpathname;

  wcdequote_pathname (wpathname);

  /* Convert the wide character string into unibyte character set. */
  memset (&ps, '\0', sizeof(mbstate_t));
  n = wcsrtombs(pathname, (const wchar_t **)&wpathname, len, &ps);
  if (n == (size_t)-1 || (wpathname && *wpathname != 0))	/* what? now you tell me? */
    {
      wpathname = orig_wpathname;
      memset (&ps, '\0', sizeof(mbstate_t));
      n = xwcsrtombs (pathname, (const wchar_t **)&wpathname, len, &ps);
    }
  pathname[len] = '\0';

  /* Can't just free wpathname here; wcsrtombs changes it in many cases. */
  free (orig_wpathname);
}

static void
dequote_pathname (pathname)
     char *pathname;
{
  if (MB_CUR_MAX > 1)
    wdequote_pathname (pathname);
  else
    udequote_pathname (pathname);
}
#endif /* HANDLE_MULTIBYTE */

/* Test whether NAME exists. */

#if defined (HAVE_LSTAT)
#  define GLOB_TESTNAME(name)  (lstat (name, &finfo))
#else /* !HAVE_LSTAT */
#  if !defined (AFS)
#    define GLOB_TESTNAME(name)  (sh_eaccess (name, F_OK))
#  else /* AFS */
#    define GLOB_TESTNAME(name)  (access (name, F_OK))
#  endif /* AFS */
#endif /* !HAVE_LSTAT */

/* Return 0 if DIR is a directory, -2 if DIR is a symlink,  -1 otherwise. */
static int
glob_testdir (dir, flags)
     char *dir;
     int flags;
{
  struct stat finfo;
  int r;

/*itrace("glob_testdir: testing %s" flags = %d, dir, flags);*/
#if defined (HAVE_LSTAT)
  r = (flags & GX_ALLDIRS) ? lstat (dir, &finfo) : stat (dir, &finfo);
#else
  r = stat (dir, &finfo);
#endif
  if (r < 0)
    return (-1);

#if defined (S_ISLNK)
  if (S_ISLNK (finfo.st_mode))
    return (-2);
#endif

  if (S_ISDIR (finfo.st_mode) == 0)
    return (-1);

  return (0);
}

/* Recursively scan SDIR for directories matching PAT (PAT is always `**').
   FLAGS is simply passed down to the recursive call to glob_vector.  Returns
   a list of matching directory names.  EP, if non-null, is set to the last
   element of the returned list.  NP, if non-null, is set to the number of
   directories in the returned list.  These two variables exist for the
   convenience of the caller (always glob_vector). */
static struct globval *
finddirs (pat, sdir, flags, ep, np)
     char *pat;
     char *sdir;
     int flags;
     struct globval **ep;
     int *np;
{
  char **r, *n;
  int ndirs;
  struct globval *ret, *e, *g;

/*itrace("finddirs: pat = `%s' sdir = `%s' flags = 0x%x", pat, sdir, flags);*/
  e = ret = 0;
  r = glob_vector (pat, sdir, flags);
  if (r == 0 || r[0] == 0)
    {
      if (np)
	*np = 0;
      if (ep)
        *ep = 0;
      if (r && r != &glob_error_return)
	free (r);
      return (struct globval *)0;
    }
  for (ndirs = 0; r[ndirs] != 0; ndirs++)
    {
      g = (struct globval *) malloc (sizeof (struct globval));
      if (g == 0)
	{
	  while (ret)		/* free list built so far */
	    {
	      g = ret->next;
	      free (ret);
	      ret = g;
	    }

	  free (r);
	  if (np)
	    *np = 0;
	  if (ep)
	    *ep = 0;
	  return (&finddirs_error_return);
	}
      if (e == 0)
	e = g;

      g->next = ret;
      ret = g;

      g->name = r[ndirs];
    }

  free (r);
  if (ep)
    *ep = e;
  if (np)
    *np = ndirs;

  return ret;
}
     	
/* Return a vector of names of files in directory DIR
   whose names match glob pattern PAT.
   The names are not in any particular order.
   Wildcards at the beginning of PAT do not match an initial period.

   The vector is terminated by an element that is a null pointer.

   To free the space allocated, first free the vector's elements,
   then free the vector.

   Return 0 if cannot get enough memory to hold the pointer
   and the names.

   Return -1 if cannot access directory DIR.
   Look in errno for more information.  */

char **
glob_vector (pat, dir, flags)
     char *pat;
     char *dir;
     int flags;
{
  DIR *d;
  register struct dirent *dp;
  struct globval *lastlink, *e, *dirlist;
  register struct globval *nextlink;
  register char *nextname, *npat, *subdir;
  unsigned int count;
  int lose, skip, ndirs, isdir, sdlen, add_current, patlen;
  register char **name_vector;
  register unsigned int i;
  int mflags;		/* Flags passed to strmatch (). */
  int pflags;		/* flags passed to sh_makepath () */
  int hasglob;		/* return value from glob_pattern_p */
  int nalloca;
  struct globval *firstmalloc, *tmplink;
  char *convfn;

  lastlink = 0;
  count = lose = skip = add_current = 0;

  firstmalloc = 0;
  nalloca = 0;

  name_vector = NULL;

/*itrace("glob_vector: pat = `%s' dir = `%s' flags = 0x%x", pat, dir, flags);*/
  /* If PAT is empty, skip the loop, but return one (empty) filename. */
  if (pat == 0 || *pat == '\0')
    {
      if (glob_testdir (dir, 0) < 0)
	return ((char **) &glob_error_return);

      nextlink = (struct globval *)alloca (sizeof (struct globval));
      if (nextlink == NULL)
	return ((char **) NULL);

      nextlink->next = (struct globval *)0;
      nextname = (char *) malloc (1);
      if (nextname == 0)
	lose = 1;
      else
	{
	  lastlink = nextlink;
	  nextlink->name = nextname;
	  nextname[0] = '\0';
	  count = 1;
	}

      skip = 1;
    }

  patlen = (pat && *pat) ? strlen (pat) : 0;

  /* If the filename pattern (PAT) does not contain any globbing characters,
     or contains a pattern with only backslash escapes (hasglob == 2),
     we can dispense with reading the directory, and just see if there is
     a filename `DIR/PAT'.  If there is, and we can access it, just make the
     vector to return and bail immediately. */
  hasglob = 0;
  if (skip == 0 && ((hasglob = glob_pattern_p (pat)) == 0 || hasglob == 2))
    {
      int dirlen;
      struct stat finfo;

      if (glob_testdir (dir, 0) < 0)
	return ((char **) &glob_error_return);

      dirlen = strlen (dir);
      nextname = (char *)malloc (dirlen + patlen + 2);
      npat = (char *)malloc (patlen + 1);
      if (nextname == 0 || npat == 0)
	{
	  FREE (nextname);
	  FREE (npat);
	  lose = 1;
	}
      else
	{
	  strcpy (npat, pat);
	  dequote_pathname (npat);

	  strcpy (nextname, dir);
	  nextname[dirlen++] = '/';
	  strcpy (nextname + dirlen, npat);

	  if (GLOB_TESTNAME (nextname) >= 0)
	    {
	      free (nextname);
	      nextlink = (struct globval *)alloca (sizeof (struct globval));
	      if (nextlink)
		{
		  nextlink->next = (struct globval *)0;
		  lastlink = nextlink;
		  nextlink->name = npat;
		  count = 1;
		}
	      else
		{
		  free (npat);
		  lose = 1;
		}
	    }
	  else
	    {
	      free (nextname);
	      free (npat);
	    }
	}

      skip = 1;
    }

  if (skip == 0)
    {
      /* Open the directory, punting immediately if we cannot.  If opendir
	 is not robust (i.e., it opens non-directories successfully), test
	 that DIR is a directory and punt if it's not. */
#if defined (OPENDIR_NOT_ROBUST)
      if (glob_testdir (dir, 0) < 0)
	return ((char **) &glob_error_return);
#endif

      d = opendir (dir);
      if (d == NULL)
	return ((char **) &glob_error_return);

      /* Compute the flags that will be passed to strmatch().  We don't
	 need to do this every time through the loop. */
      mflags = (noglob_dot_filenames ? FNM_PERIOD : FNM_DOTDOT) | FNM_PATHNAME;

#ifdef FNM_CASEFOLD
      if (glob_ignore_case)
	mflags |= FNM_CASEFOLD;
#endif

      if (extended_glob)
	mflags |= FNM_EXTMATCH;

      add_current = ((flags & (GX_ALLDIRS|GX_ADDCURDIR)) == (GX_ALLDIRS|GX_ADDCURDIR));

      /* Scan the directory, finding all names that match	 For each name that matches, allocate a struct globval
	 on the stack and store the name in it.
	 Chain those structs together; lastlink is the front of the chain.  */
      while (1)
	{
	  /* Make globbing interruptible in the shell. */
	  if (interrupt_state || terminating_signal)
	    {
	      lose = 1;
	      break;
	    }
	  else if (signal_is_pending (SIGINT))	/* XXX - make SIGINT traps responsive */
	    {
	      lose = 1;
	      break;
	    }

	  dp = readdir (d);
	  if (dp == NULL)
	    break;

	  /* If this directory entry is not to be used, try again. */
	  if (REAL_DIR_ENTRY (dp) == 0)
	    continue;

#if 0
	  if (dp->d_name == 0 || *dp->d_name == 0)
	    continue;
#endif

#if HANDLE_MULTIBYTE
	  if (MB_CUR_MAX > 1 && mbskipname (pat, dp->d_name, flags))
	    continue;
	  else
#endif
	  if (skipname (pat, dp->d_name, flags))
	    continue;

	  /* If we're only interested in directories, don't bother with files */
	  if (flags & (GX_MATCHDIRS|GX_ALLDIRS))
	    {
	      pflags = (flags & GX_ALLDIRS) ? MP_RMDOT : 0;
	      if (flags & GX_NULLDIR)
		pflags |= MP_IGNDOT;
	      subdir = sh_makepath (dir, dp->d_name, pflags);
	      isdir = glob_testdir (subdir, flags);
	      if (isdir < 0 && (flags & GX_MATCHDIRS))
		{
		  free (subdir);
		  continue;
		}
	    }

	  if (flags & GX_ALLDIRS)
	    {
	      if (isdir == 0)
		{
		  dirlist = finddirs (pat, subdir, (flags & ~GX_ADDCURDIR), &e, &ndirs);
		  if (dirlist == &finddirs_error_return)
		    {
		      free (subdir);
		      lose = 1;
		      break;
		    }
		  if (ndirs)		/* add recursive directories to list */
		    {
		      if (firstmalloc == 0)
		        firstmalloc = e;
		      e->next = lastlink;
		      lastlink = dirlist;
		      count += ndirs;
		    }
		}

	      /* XXX - should we even add this if it's not a directory? */
	      nextlink = (struct globval *) malloc (sizeof (struct globval));
	      if (firstmalloc == 0)
		firstmalloc = nextlink;
	      sdlen = strlen (subdir);
	      nextname = (char *) malloc (sdlen + 1);
	      if (nextlink == 0 || nextname == 0)
		{
		  if (firstmalloc && firstmalloc == nextlink)
		    firstmalloc = 0;
		  /* If we reset FIRSTMALLOC we can free this here. */
		  FREE (nextlink);
		  FREE (nextname);
		  free (subdir);
		  lose = 1;
		  break;
		}
	      nextlink->next = lastlink;
	      lastlink = nextlink;
	      nextlink->name = nextname;
	      bcopy (subdir, nextname, sdlen + 1);
	      free (subdir);
	      ++count;
	      continue;
	    }
	  else if (flags & GX_MATCHDIRS)
	    free (subdir);

	  convfn = fnx_fromfs (dp->d_name, D_NAMLEN (dp));
	  if (strmatch (pat, convfn, mflags) != FNM_NOMATCH)
	    {
	      if (nalloca < ALLOCA_MAX)
		{
		  nextlink = (struct globval *) alloca (sizeof (struct globval));
		  nalloca += sizeof (struct globval);
		}
	      else
		{
		  nextlink = (struct globval *) malloc (sizeof (struct globval));
		  if (firstmalloc == 0)
		    firstmalloc = nextlink;
		}

	      nextname = (char *) malloc (D_NAMLEN (dp) + 1);
	      if (nextlink == 0 || nextname == 0)
		{
		  /* We free NEXTLINK here, since it won't be added to the
		     LASTLINK chain. If we used malloc, and it returned non-
		     NULL, firstmalloc will be set to something valid. If it's
		     NEXTLINK, reset it before we free NEXTLINK to avoid
		     duplicate frees. If not, it will be taken care of by the
		     loop below with TMPLINK. */
		  if (firstmalloc)
		    {
		      if (firstmalloc == nextlink)
			firstmalloc = 0;
		      FREE (nextlink);
		    }
		  FREE (nextname);
		  lose = 1;
		  break;
		}
	      nextlink->next = lastlink;
	      lastlink = nextlink;
	      nextlink->name = nextname;
	      bcopy (dp->d_name, nextname, D_NAMLEN (dp) + 1);
	      ++count;
	    }
	}

      (void) closedir (d);
    }

  /* compat: if GX_ADDCURDIR, add the passed directory also.  Add an empty
     directory name as a placeholder if GX_NULLDIR (in which case the passed
     directory name is "."). */
  if (add_current && lose == 0)
    {
      sdlen = strlen (dir);
      nextname = (char *)malloc (sdlen + 1);
      nextlink = (struct globval *) malloc (sizeof (struct globval));
      if (nextlink == 0 || nextname == 0)
	{
	  FREE (nextlink);
	  FREE (nextname);
	  lose = 1;
	}
      else
	{
	  nextlink->name = nextname;
	  nextlink->next = lastlink;
	  lastlink = nextlink;
	  if (flags & GX_NULLDIR)
	    nextname[0] = '\0';
	  else
	    bcopy (dir, nextname, sdlen + 1);
	  ++count;
	}
    }

  if (lose == 0)
    {
      name_vector = (char **) malloc ((count + 1) * sizeof (char *));
      lose |= name_vector == NULL;
    }

  /* Have we run out of memory or been interrupted? */
  if (lose)
    {
      tmplink = 0;

      /* Here free the strings we have got.  */
      while (lastlink)
	{
	  /* Since we build the list in reverse order, the first N entries
	     will be allocated with malloc, if firstmalloc is set, from
	     lastlink to firstmalloc. */
	  if (firstmalloc)
	    {
	      if (lastlink == firstmalloc)
		firstmalloc = 0;
	      tmplink = lastlink;
	    }
	  else
	    tmplink = 0;
	  free (lastlink->name);
	  lastlink = lastlink->next;
	  FREE (tmplink);
	}

      /* Don't call QUIT; here; let higher layers deal with it. */

      return ((char **)NULL);
    }

  /* Copy the name pointers from the linked list into the vector.  */
  for (tmplink = lastlink, i = 0; i < count; ++i)
    {
      name_vector[i] = tmplink->name;
      tmplink = tmplink->next;
    }

  name_vector[count] = NULL;

  /* If we allocated some of the struct globvals, free them now. */
  if (firstmalloc)
    {
      tmplink = 0;
      while (lastlink)
	{
	  tmplink = lastlink;
	  if (lastlink == firstmalloc)
	    lastlink = firstmalloc = 0;
	  else
	    lastlink = lastlink->next;
	  free (tmplink);
	}
    }

  return (name_vector);
}

/* Return a new array which is the concatenation of each string in ARRAY
   to DIR.  This function expects you to pass in an allocated ARRAY, and
   it takes care of free()ing that array.  Thus, you might think of this
   function as side-effecting ARRAY.  This should handle GX_MARKDIRS. */
static char **
glob_dir_to_array (dir, array, flags)
     char *dir, **array;
     int flags;
{
  register unsigned int i, l;
  int add_slash;
  char **result, *new;
  struct stat sb;

  l = strlen (dir);
  if (l == 0)
    {
      if (flags & GX_MARKDIRS)
	for (i = 0; array[i]; i++)
	  {
	    if ((stat (array[i], &sb) == 0) && S_ISDIR (sb.st_mode))
	      {
		l = strlen (array[i]);
		new = (char *)realloc (array[i], l + 2);
		if (new == 0)
		  return NULL;
		new[l] = '/';
		new[l+1] = '\0';
		array[i] = new;
	      }
	  }
      return (array);
    }

  add_slash = dir[l - 1] != '/';

  i = 0;
  while (array[i] != NULL)
    ++i;

  result = (char **) malloc ((i + 1) * sizeof (char *));
  if (result == NULL)
    return (NULL);

  for (i = 0; array[i] != NULL; i++)
    {
      /* 3 == 1 for NUL, 1 for slash at end of DIR, 1 for GX_MARKDIRS */
      result[i] = (char *) malloc (l + strlen (array[i]) + 3);

      if (result[i] == NULL)
	{
	  int ind;
	  for (ind = 0; ind < i; ind++)
	    free (result[ind]);
	  free (result);
	  return (NULL);
	}

      strcpy (result[i], dir);
      if (add_slash)
	result[i][l] = '/';
      if (array[i][0])
	{
	  strcpy (result[i] + l + add_slash, array[i]);
	  if (flags & GX_MARKDIRS)
	    {
	      if ((stat (result[i], &sb) == 0) && S_ISDIR (sb.st_mode))
		{
		  size_t rlen;
		  rlen = strlen (result[i]);
		  result[i][rlen] = '/';
		  result[i][rlen+1] = '\0';
		}
	    }
	}
      else
        result[i][l+add_slash] = '\0';
    }
  result[i] = NULL;

  /* Free the input array.  */
  for (i = 0; array[i] != NULL; i++)
    free (array[i]);
  free ((char *) array);

  return (result);
}

/* Do globbing on PATHNAME.  Return an array of pathnames that match,
   marking the end of the array with a null-pointer as an element.
   If no pathnames match, then the array is empty (first element is null).
   If there isn't enough memory, then return NULL.
   If a file system error occurs, return -1; `errno' has the error code.  */
char **
glob_filename (pathname, flags)
     char *pathname;
     int flags;
{
  char **result, **new_result;
  unsigned int result_size;
  char *directory_name, *filename, *dname, *fn;
  unsigned int directory_len;
  int free_dirname;			/* flag */
  int dflags, hasglob;

  result = (char **) malloc (sizeof (char *));
  result_size = 1;
  if (result == NULL)
    return (NULL);

  result[0] = NULL;

  directory_name = NULL;

  /* Find the filename.  */
  filename = strrchr (pathname, '/');
#if defined (EXTENDED_GLOB)
  if (filename && extended_glob)
    {
      fn = glob_dirscan (pathname, '/');
#if DEBUG_MATCHING
      if (fn != filename)
	fprintf (stderr, "glob_filename: glob_dirscan: fn (%s) != filename (%s)\n", fn ? fn : "(null)", filename);
#endif
      filename = fn;
    }
#endif

  if (filename == NULL)
    {
      filename = pathname;
      directory_name = "";
      directory_len = 0;
      free_dirname = 0;
    }
  else
    {
      directory_len = (filename - pathname) + 1;
      directory_name = (char *) malloc (directory_len + 1);

      if (directory_name == 0)		/* allocation failed? */
	{
	  free (result);
	  return (NULL);
	}

      bcopy (pathname, directory_name, directory_len);
      directory_name[directory_len] = '\0';
      ++filename;
      free_dirname = 1;
    }

  hasglob = 0;
  /* If directory_name contains globbing characters, then we
     have to expand the previous levels.  Just recurse.
     If glob_pattern_p returns != [0,1] we have a pattern that has backslash
     quotes but no unquoted glob pattern characters. We dequote it below. */
  if (directory_len > 0 && (hasglob = glob_pattern_p (directory_name)) == 1)
    {
      char **directories, *d, *p;
      register unsigned int i;
      int all_starstar, last_starstar;

      all_starstar = last_starstar = 0;
      d = directory_name;
      dflags = flags & ~GX_MARKDIRS;
      /* Collapse a sequence of ** patterns separated by one or more slashes
	 to a single ** terminated by a slash or NUL */
      if ((flags & GX_GLOBSTAR) && d[0] == '*' && d[1] == '*' && (d[2] == '/' || d[2] == '\0'))
	{
	  p = d;
	  while (d[0] == '*' && d[1] == '*' && (d[2] == '/' || d[2] == '\0'))
	    {
	      p = d;
	      if (d[2])
		{
		  d += 3;
		  while (*d == '/')
		    d++;
		  if (*d == 0)
		    break;
		}
	    }
	  if (*d == 0)
	    all_starstar = 1;
	  d = p;
	  dflags |= GX_ALLDIRS|GX_ADDCURDIR;
	  directory_len = strlen (d);
	}

      /* If there is a non [star][star]/ component in directory_name, we
	 still need to collapse trailing sequences of [star][star]/ into
	 a single one and note that the directory name ends with [star][star],
	 so we can compensate if filename is [star][star] */
      if ((flags & GX_GLOBSTAR) && all_starstar == 0)
	{
	  int dl, prev;
	  prev = dl = directory_len;
	  while (dl >= 4 && d[dl - 1] == '/' &&
			   d[dl - 2] == '*' &&
			   d[dl - 3] == '*' &&
			   d[dl - 4] == '/')
	    prev = dl, dl -= 3;
	  if (dl != directory_len)
	    last_starstar = 1;
	  directory_len = prev;
	}

      /* If the directory name ends in [star][star]/ but the filename is
	 [star][star], just remove the final [star][star] from the directory
	 so we don't have to scan everything twice. */
      if (last_starstar && directory_len > 4 &&
	    filename[0] == '*' && filename[1] == '*' && filename[2] == 0)
	{
	  directory_len -= 3;
	}

      if (d[directory_len - 1] == '/')
	d[directory_len - 1] = '\0';

      directories = glob_filename (d, dflags|GX_RECURSE);

      if (free_dirname)
	{
	  free (directory_name);
	  directory_name = NULL;
	}

      if (directories == NULL)
	goto memory_error;
      else if (directories == (char **)&glob_error_return)
	{
	  free ((char *) result);
	  return ((char **) &glob_error_return);
	}
      else if (*directories == NULL)
	{
	  free ((char *) directories);
	  free ((char *) result);
	  return ((char **) &glob_error_return);
	}

      /* If we have something like [star][star]/[star][star], it's no use to
         glob **, then do it again, and throw half the results away.  */
      if (all_starstar && filename[0] == '*' && filename[1] == '*' && filename[2] == 0)
	{
	  free ((char *) directories);
	  free (directory_name);
	  directory_name = NULL;
	  directory_len = 0;
	  goto only_filename;
	}

      /* We have successfully globbed the preceding directory name.
	 For each name in DIRECTORIES, call glob_vector on it and
	 FILENAME.  Concatenate the results together.  */
      for (i = 0; directories[i] != NULL; ++i)
	{
	  char **temp_results;
	  int shouldbreak;

	  shouldbreak = 0;
	  /* XXX -- we've recursively scanned any directories resulting from
	     a `**', so turn off the flag.  We turn it on again below if
	     filename is `**' */
	  /* Scan directory even on a NULL filename.  That way, `*h/'
	     returns only directories ending in `h', instead of all
	     files ending in `h' with a `/' appended. */
	  dname = directories[i];
	  dflags = flags & ~(GX_MARKDIRS|GX_ALLDIRS|GX_ADDCURDIR);
	  /* last_starstar? */
	  if ((flags & GX_GLOBSTAR) && filename[0] == '*' && filename[1] == '*' && filename[2] == '\0')
	    dflags |= GX_ALLDIRS|GX_ADDCURDIR;
	  if (dname[0] == '\0' && filename[0])
	    {
	      dflags |= GX_NULLDIR;
	      dname = ".";	/* treat null directory name and non-null filename as current directory */
	    }

	  /* Special handling for symlinks to directories with globstar on */
	  if (all_starstar && (dflags & GX_NULLDIR) == 0)
	    {
	      int dlen;

	      /* If we have a directory name that is not null (GX_NULLDIR above)
		 and is a symlink to a directory, we return the symlink if
		 we're not `descending' into it (filename[0] == 0) and return
		 glob_error_return (which causes the code below to skip the
		 name) otherwise. I should fold this into a test that does both
		 checks instead of calling stat twice. */
	      if (glob_testdir (dname, flags|GX_ALLDIRS) == -2 && glob_testdir (dname, 0) == 0)
		{
		  if (filename[0] != 0)
		    temp_results = (char **)&glob_error_return;		/* skip */
		  else
		    {
		      /* Construct array to pass to glob_dir_to_array */
		      temp_results = (char **)malloc (2 * sizeof (char *));
		      if (temp_results == NULL)
			goto memory_error;
		      temp_results[0] = (char *)malloc (1);
		      if (temp_results[0] == 0)
			{
			  free (temp_results);
			  goto memory_error;
			}
		      **temp_results = '\0';
		      temp_results[1] = NULL;
		      dflags |= GX_SYMLINK;	/* mostly for debugging */
		    }
		}
	      else
		temp_results = glob_vector (filename, dname, dflags);
	    }
	  else
	    temp_results = glob_vector (filename, dname, dflags);

	  /* Handle error cases. */
	  if (temp_results == NULL)
	    goto memory_error;
	  else if (temp_results == (char **)&glob_error_return)
	    /* This filename is probably not a directory.  Ignore it.  */
	    ;
	  else
	    {
	      char **array;
	      register unsigned int l;

	      /* If we're expanding **, we don't need to glue the directory
		 name to the results; we've already done it in glob_vector */
	      if ((dflags & GX_ALLDIRS) && filename[0] == '*' && filename[1] == '*' && (filename[2] == '\0' || filename[2] == '/'))
		{
		  /* When do we remove null elements from temp_results?  And
		     how to avoid duplicate elements in the final result? */
		  /* If (dflags & GX_NULLDIR) glob_filename potentially left a
		     NULL placeholder in the temp results just in case
		     glob_vector/glob_dir_to_array did something with it, but
		     if it didn't, and we're not supposed to be passing them
		     through for some reason ((flags & GX_NULLDIR) == 0) we
		     need to remove all the NULL elements from the beginning
		     of TEMP_RESULTS. */
		  /* If we have a null directory name and ** as the filename,
		     we have just searched for everything from the current
		     directory on down. Break now (shouldbreak = 1) to avoid
		     duplicate entries in the final result. */
#define NULL_PLACEHOLDER(x)	((x) && *(x) && **(x) == 0)
		  if ((dflags & GX_NULLDIR) && (flags & GX_NULLDIR) == 0 &&
			NULL_PLACEHOLDER (temp_results))
#undef NULL_PLACEHOLDER
		    {
		      register int i, n;
		      for (n = 0; temp_results[n] && *temp_results[n] == 0; n++)
			;
		      i = n;
		      do
			temp_results[i - n] = temp_results[i];
		      while (temp_results[i++] != 0);
		      array = temp_results;
		      shouldbreak = 1;
		    }
	          else
		    array = temp_results;
		}
	      else if (dflags & GX_SYMLINK)
		array = glob_dir_to_array (directories[i], temp_results, flags);
	      else
		array = glob_dir_to_array (directories[i], temp_results, flags);
	      l = 0;
	      while (array[l] != NULL)
		++l;

	      new_result = (char **)realloc (result, (result_size + l) * sizeof (char *));

	      if (new_result == NULL)
		{
		  for (l = 0; array[l]; ++l)
		    free (array[l]);
		  free ((char *)array);
		  goto memory_error;
		}
	      result = new_result;

	      for (l = 0; array[l] != NULL; ++l)
		result[result_size++ - 1] = array[l];

	      result[result_size - 1] = NULL;

	      /* Note that the elements of ARRAY are not freed.  */
	      if (array != temp_results)
		free ((char *) array);
	      else if ((dflags & GX_ALLDIRS) && filename[0] == '*' && filename[1] == '*' && filename[2] == '\0')
		free (temp_results);	/* expanding ** case above */

	      if (shouldbreak)
		break;
	    }
	}
      /* Free the directories.  */
      for (i = 0; directories[i]; i++)
	free (directories[i]);

      free ((char *) directories);

      return (result);
    }

only_filename:
  /* If there is only a directory name, return it. */
  if (*filename == '\0')
    {
      result = (char **) realloc ((char *) result, 2 * sizeof (char *));
      if (result == NULL)
	{
	  if (free_dirname)
	    free (directory_name);
	  return (NULL);
	}
      /* If we have a directory name with quoted characters, and we are
	 being called recursively to glob the directory portion of a pathname,
	 we need to dequote the directory name before returning it so the
	 caller can read the directory */
      if (directory_len > 0 && hasglob == 2 && (flags & GX_RECURSE) != 0)
	{
	  dequote_pathname (directory_name);
	  directory_len = strlen (directory_name);
	}

      /* We could check whether or not the dequoted directory_name is a
	 directory and return it here, returning the original directory_name
	 if not, but we don't do that. We do return the dequoted directory
	 name if we're not being called recursively and the dequoted name
	 corresponds to an actual directory. For better backwards compatibility,
	 we can return &glob_error_return unconditionally in this case. */

      if (directory_len > 0 && hasglob == 2 && (flags & GX_RECURSE) == 0)
	{
	  dequote_pathname (directory_name);
	  if (glob_testdir (directory_name, 0) < 0)
	    {
	      if (free_dirname)
		free (directory_name);
	      free ((char *) result);
	      return ((char **)&glob_error_return);
	    }
	}

      /* Handle GX_MARKDIRS here. */
      result[0] = (char *) malloc (directory_len + 1);
      if (result[0] == NULL)
	goto memory_error;
      bcopy (directory_name, result[0], directory_len + 1);
      if (free_dirname)
	free (directory_name);
      result[1] = NULL;
      return (result);
    }
  else
    {
      char **temp_results;

      /* There are no unquoted globbing characters in DIRECTORY_NAME.
	 Dequote it before we try to open the directory since there may
	 be quoted globbing characters which should be treated verbatim. */
      if (directory_len > 0)
	dequote_pathname (directory_name);

      /* We allocated a small array called RESULT, which we won't be using.
	 Free that memory now. */
      free (result);

      /* Just return what glob_vector () returns appended to the
	 directory name. */
      /* If flags & GX_ALLDIRS, we're called recursively */
      dflags = flags & ~GX_MARKDIRS;
      if (directory_len == 0)
	dflags |= GX_NULLDIR;
      if ((flags & GX_GLOBSTAR) && filename[0] == '*' && filename[1] == '*' && filename[2] == '\0')
	{
	  dflags |= GX_ALLDIRS|GX_ADDCURDIR;
#if 0
	  /* If we want all directories (dflags & GX_ALLDIRS) and we're not
	     being called recursively as something like `echo [star][star]/[star].o'
	     ((flags & GX_ALLDIRS) == 0), we want to prevent glob_vector from
	     adding a null directory name to the front of the temp_results
	     array.  We turn off ADDCURDIR if not called recursively and
	     dlen == 0 */
#endif
	  if (directory_len == 0 && (flags & GX_ALLDIRS) == 0)
	    dflags &= ~GX_ADDCURDIR;
	}
      temp_results = glob_vector (filename,
				  (directory_len == 0 ? "." : directory_name),
				  dflags);

      if (temp_results == NULL || temp_results == (char **)&glob_error_return)
	{
	  if (free_dirname)
	    free (directory_name);
	  QUIT;			/* XXX - shell */
	  run_pending_traps ();
	  return (temp_results);
	}

      result = glob_dir_to_array ((dflags & GX_ALLDIRS) ? "" : directory_name, temp_results, flags);

      if (free_dirname)
	free (directory_name);
      return (result);
    }

  /* We get to memory_error if the program has run out of memory, or
     if this is the shell, and we have been interrupted. */
 memory_error:
  if (result != NULL)
    {
      register unsigned int i;
      for (i = 0; result[i] != NULL; ++i)
	free (result[i]);
      free ((char *) result);
    }

  if (free_dirname && directory_name)
    free (directory_name);

  QUIT;
  run_pending_traps ();

  return (NULL);
}

#if defined (TEST)

main (argc, argv)
     int argc;
     char **argv;
{
  unsigned int i;

  for (i = 1; i < argc; ++i)
    {
      char **value = glob_filename (argv[i], 0);
      if (value == NULL)
	puts ("Out of memory.");
      else if (value == &glob_error_return)
	perror (argv[i]);
      else
	for (i = 0; value[i] != NULL; i++)
	  puts (value[i]);
    }

  exit (0);
}
#endif	/* TEST.  */
