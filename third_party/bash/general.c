/* general.c -- Stuff that is used by all files. */

/* Copyright (C) 1987-2021 Free Software Foundation, Inc.

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

#include "bashintl.h"

#include "shell.h"
#include "parser.h"
#include "flags.h"
#include "findcmd.h"
#include "test.h"
#include "trap.h"
#include "pathexp.h"

#include "common.h"

#if defined (HAVE_MBSTR_H) && defined (HAVE_MBSCHR)
#  include <mbstr.h>		/* mbschr */
#endif

#include "tilde.h"

#if !defined (errno)
extern int errno;
#endif /* !errno */

#ifdef __CYGWIN__
#  include <sys/cygwin.h>
#endif

static char *bash_special_tilde_expansions PARAMS((char *));
static int unquoted_tilde_word PARAMS((const char *));
static void initialize_group_array PARAMS((void));

/* A standard error message to use when getcwd() returns NULL. */
const char * const bash_getcwd_errstr = N_("getcwd: cannot access parent directories");

/* Do whatever is necessary to initialize `Posix mode'.  This currently
   modifies the following variables which are controlled via shopt:
      interactive_comments
      source_uses_path
      expand_aliases
      inherit_errexit
      print_shift_error
      posixglob

   and the following variables which cannot be user-modified:

      source_searches_cwd

  If we add to the first list, we need to change the table and functions
  below */

static struct {
  int *posix_mode_var;
} posix_vars[] = 
{
  &interactive_comments,
  &source_uses_path,
  &expand_aliases,
  &inherit_errexit,
  &print_shift_error,
  0
};

static char *saved_posix_vars = 0;

void
posix_initialize (on)
     int on;
{
  /* Things that should be turned on when posix mode is enabled. */
  if (on != 0)
    {
      interactive_comments = source_uses_path = expand_aliases = 1;
      inherit_errexit = 1;
      source_searches_cwd = 0;
      print_shift_error = 1;
    }

  /* Things that should be turned on when posix mode is disabled. */
  else if (saved_posix_vars)		/* on == 0, restore saved settings */
    {
      set_posix_options (saved_posix_vars);
      free (saved_posix_vars);
      saved_posix_vars = 0;
    }
  else	/* on == 0, restore a default set of settings */
    {
      source_searches_cwd = 1;
      expand_aliases = interactive_shell;
      print_shift_error = 0;
    }
}

int
num_posix_options ()
{
  return ((sizeof (posix_vars) / sizeof (posix_vars[0])) - 1);
}

char *
get_posix_options (bitmap)
     char *bitmap;
{
  register int i;

  if (bitmap == 0)
    bitmap = (char *)xmalloc (num_posix_options ());	/* no trailing NULL */
  for (i = 0; posix_vars[i].posix_mode_var; i++)
    bitmap[i] = *(posix_vars[i].posix_mode_var);
  return bitmap;
}

#undef save_posix_options
void
save_posix_options ()
{
  saved_posix_vars = get_posix_options (saved_posix_vars);
}

void
set_posix_options (bitmap)
     const char *bitmap;
{
  register int i;

  for (i = 0; posix_vars[i].posix_mode_var; i++)
    *(posix_vars[i].posix_mode_var) = bitmap[i];
}

/* **************************************************************** */
/*								    */
/*  Functions to convert to and from and display non-standard types */
/*								    */
/* **************************************************************** */

#if defined (RLIMTYPE)
RLIMTYPE
string_to_rlimtype (s)
     char *s;
{
  RLIMTYPE ret;
  int neg;

  ret = 0;
  neg = 0;
  while (s && *s && whitespace (*s))
    s++;
  if (s && (*s == '-' || *s == '+'))
    {
      neg = *s == '-';
      s++;
    }
  for ( ; s && *s && DIGIT (*s); s++)
    ret = (ret * 10) + TODIGIT (*s);
  return (neg ? -ret : ret);
}

void
print_rlimtype (n, addnl)
     RLIMTYPE n;
     int addnl;
{
  char s[INT_STRLEN_BOUND (RLIMTYPE) + 1], *p;

  p = s + sizeof(s);
  *--p = '\0';

  if (n < 0)
    {
      do
	*--p = '0' - n % 10;
      while ((n /= 10) != 0);

      *--p = '-';
    }
  else
    {
      do
	*--p = '0' + n % 10;
      while ((n /= 10) != 0);
    }

  printf ("%s%s", p, addnl ? "\n" : "");
}
#endif /* RLIMTYPE */

/* **************************************************************** */
/*								    */
/*		       Input Validation Functions		    */
/*								    */
/* **************************************************************** */

/* Return non-zero if all of the characters in STRING are digits. */
int
all_digits (string)
     const char *string;
{
  register const char *s;

  for (s = string; *s; s++)
    if (DIGIT (*s) == 0)
      return (0);

  return (1);
}

/* Return non-zero if the characters pointed to by STRING constitute a
   valid number.  Stuff the converted number into RESULT if RESULT is
   not null. */
int
legal_number (string, result)
     const char *string;
     intmax_t *result;
{
  intmax_t value;
  char *ep;

  if (result)
    *result = 0;

  if (string == 0)
    return 0;

  errno = 0;
  value = strtoimax (string, &ep, 10);
  if (errno || ep == string)
    return 0;	/* errno is set on overflow or underflow */

  /* Skip any trailing whitespace, since strtoimax does not. */
  while (whitespace (*ep))
    ep++;

  /* If *string is not '\0' but *ep is '\0' on return, the entire string
     is valid. */
  if (*string && *ep == '\0')
    {
      if (result)
	*result = value;
      /* The SunOS4 implementation of strtol() will happily ignore
	 overflow conditions, so this cannot do overflow correctly
	 on those systems. */
      return 1;
    }
    
  return (0);
}

/* Return 1 if this token is a legal shell `identifier'; that is, it consists
   solely of letters, digits, and underscores, and does not begin with a
   digit. */
int
legal_identifier (name)
     const char *name;
{
  register const char *s;
  unsigned char c;

  if (!name || !(c = *name) || (legal_variable_starter (c) == 0))
    return (0);

  for (s = name + 1; (c = *s) != 0; s++)
    {
      if (legal_variable_char (c) == 0)
	return (0);
    }
  return (1);
}

/* Return 1 if NAME is a valid value that can be assigned to a nameref
   variable.  FLAGS can be 2, in which case the name is going to be used
   to create a variable.  Other values are currently unused, but could
   be used to allow values to be stored and indirectly referenced, but
   not used in assignments. */
int
valid_nameref_value (name, flags)
     const char *name;
     int flags;
{
  if (name == 0 || *name == 0)
    return 0;

  /* valid identifier */
#if defined (ARRAY_VARS)  
  if (legal_identifier (name) || (flags != 2 && valid_array_reference (name, 0)))
#else
  if (legal_identifier (name))
#endif
    return 1;

  return 0;
}

int
check_selfref (name, value, flags)
     const char *name;
     char *value;
     int flags;
{
  char *t;

  if (STREQ (name, value))
    return 1;

#if defined (ARRAY_VARS)
  if (valid_array_reference (value, 0))
    {
      t = array_variable_name (value, 0, (char **)NULL, (int *)NULL);
      if (t && STREQ (name, t))
	{
	  free (t);
	  return 1;
	}
      free (t);
    }
#endif

  return 0;	/* not a self reference */
}

/* Make sure that WORD is a valid shell identifier, i.e.
   does not contain a dollar sign, nor is quoted in any way.
   If CHECK_WORD is non-zero,
   the word is checked to ensure that it consists of only letters,
   digits, and underscores, and does not consist of all digits. */
int
check_identifier (word, check_word)
     WORD_DESC *word;
     int check_word;
{
  if (word->flags & (W_HASDOLLAR|W_QUOTED))	/* XXX - HASDOLLAR? */
    {
      internal_error (_("`%s': not a valid identifier"), word->word);
      return (0);
    }
  else if (check_word && (all_digits (word->word) || legal_identifier (word->word) == 0))
    {
      internal_error (_("`%s': not a valid identifier"), word->word);
      return (0);
    }
  else
    return (1);
}

/* Return 1 if STRING is a function name that the shell will import from
   the environment.  Currently we reject attempts to import shell functions
   containing slashes, beginning with newlines or containing blanks.  In
   Posix mode, we require that STRING be a valid shell identifier.  Not
   used yet. */
int
importable_function_name (string, len)
     const char *string;
     size_t len;
{
  if (absolute_program (string))	/* don't allow slash */
    return 0;
  if (*string == '\n')			/* can't start with a newline */
    return 0;
  if (shellblank (*string) || shellblank(string[len-1]))
    return 0;
  return (posixly_correct ? legal_identifier (string) : 1);
}

int
exportable_function_name (string)
     const char *string;
{
  if (absolute_program (string))
    return 0;
  if (mbschr (string, '=') != 0)
    return 0;
  return 1;
}

/* Return 1 if STRING comprises a valid alias name.  The shell accepts
   essentially all characters except those which must be quoted to the
   parser (which disqualifies them from alias expansion anyway) and `/'. */
int
legal_alias_name (string, flags)
     const char *string;
     int flags;
{
  register const char *s;

  for (s = string; *s; s++)
    if (shellbreak (*s) || shellxquote (*s) || shellexp (*s) || (*s == '/'))
      return 0;
  return 1;
}

/* Returns non-zero if STRING is an assignment statement.  The returned value
   is the index of the `=' sign.  If FLAGS&1 we are expecting a compound assignment
   and require an array subscript before the `=' to denote an assignment
   statement. */
int
assignment (string, flags)
     const char *string;
     int flags;
{
  register unsigned char c;
  register int newi, indx;

  c = string[indx = 0];

#if defined (ARRAY_VARS)
  /* If parser_state includes PST_COMPASSIGN, FLAGS will include 1, so we are
     parsing the contents of a compound assignment. If parser_state includes
     PST_REPARSE, we are in the middle of an assignment statement and breaking
     the words between the parens into words and assignment statements, but
     we don't need to check for that right now. Within a compound assignment,
     the subscript is required to make the word an assignment statement. If
     we don't have a subscript, even if the word is a valid assignment
     statement otherwise, we don't want to treat it as one. */
  if ((flags & 1) && c != '[')		/* ] */
    return (0);
  else if ((flags & 1) == 0 && legal_variable_starter (c) == 0)
#else
  if (legal_variable_starter (c) == 0)
#endif
    return (0);

  while (c = string[indx])
    {
      /* The following is safe.  Note that '=' at the start of a word
	 is not an assignment statement. */
      if (c == '=')
	return (indx);

#if defined (ARRAY_VARS)
      if (c == '[')
	{
	  newi = skipsubscript (string, indx, (flags & 2) ? 1 : 0);
	  /* XXX - why not check for blank subscripts here, if we do in
	     valid_array_reference? */
	  if (string[newi++] != ']')
	    return (0);
	  if (string[newi] == '+' && string[newi+1] == '=')
	    return (newi + 1);
	  return ((string[newi] == '=') ? newi : 0);
	}
#endif /* ARRAY_VARS */

      /* Check for `+=' */
      if (c == '+' && string[indx+1] == '=')
	return (indx + 1);

      /* Variable names in assignment statements may contain only letters,
	 digits, and `_'. */
      if (legal_variable_char (c) == 0)
	return (0);

      indx++;
    }
  return (0);
}

int
line_isblank (line)
     const char *line;
{
  register int i;

  if (line == 0)
    return 0;		/* XXX */
  for (i = 0; line[i]; i++)
    if (isblank ((unsigned char)line[i]) == 0)
      break;
  return (line[i] == '\0');  
}

/* **************************************************************** */
/*								    */
/*	     Functions to manage files and file descriptors	    */
/*								    */
/* **************************************************************** */

/* A function to unset no-delay mode on a file descriptor.  Used in shell.c
   to unset it on the fd passed as stdin.  Should be called on stdin if
   readline gets an EAGAIN or EWOULDBLOCK when trying to read input. */

#if !defined (O_NDELAY)
#  if defined (FNDELAY)
#    define O_NDELAY FNDELAY
#  endif
#endif /* O_NDELAY */

/* Make sure no-delay mode is not set on file descriptor FD. */
int
sh_unset_nodelay_mode (fd)
     int fd;
{
  int flags, bflags;

  if ((flags = fcntl (fd, F_GETFL, 0)) < 0)
    return -1;

  bflags = 0;

  /* This is defined to O_NDELAY in filecntl.h if O_NONBLOCK is not present
     and O_NDELAY is defined. */
#ifdef O_NONBLOCK
  bflags |= O_NONBLOCK;
#endif

#ifdef O_NDELAY
  bflags |= O_NDELAY;
#endif

  if (flags & bflags)
    {
      flags &= ~bflags;
      return (fcntl (fd, F_SETFL, flags));
    }

  return 0;
}

/* Just a wrapper for the define in include/filecntl.h */
int
sh_setclexec (fd)
     int fd;
{
  return (SET_CLOSE_ON_EXEC (fd));
}

/* Return 1 if file descriptor FD is valid; 0 otherwise. */
int
sh_validfd (fd)
     int fd;
{
  return (fcntl (fd, F_GETFD, 0) >= 0);
}

int
fd_ispipe (fd)
     int fd;
{
  errno = 0;
  return ((lseek (fd, 0L, SEEK_CUR) < 0) && (errno == ESPIPE));
}

/* There is a bug in the NeXT 2.1 rlogind that causes opens
   of /dev/tty to fail. */

#if defined (__BEOS__)
/* On BeOS, opening in non-blocking mode exposes a bug in BeOS, so turn it
   into a no-op.  This should probably go away in the future. */
#  undef O_NONBLOCK
#  define O_NONBLOCK 0
#endif /* __BEOS__ */

void
check_dev_tty ()
{
  int tty_fd;
  char *tty;

  tty_fd = open ("/dev/tty", O_RDWR|O_NONBLOCK);

  if (tty_fd < 0)
    {
      tty = (char *)ttyname (fileno (stdin));
      if (tty == 0)
	return;
      tty_fd = open (tty, O_RDWR|O_NONBLOCK);
    }
  if (tty_fd >= 0)
    close (tty_fd);
}

/* Return 1 if PATH1 and PATH2 are the same file.  This is kind of
   expensive.  If non-NULL STP1 and STP2 point to stat structures
   corresponding to PATH1 and PATH2, respectively. */
int
same_file (path1, path2, stp1, stp2)
     const char *path1, *path2;
     struct stat *stp1, *stp2;
{
  struct stat st1, st2;

  if (stp1 == NULL)
    {
      if (stat (path1, &st1) != 0)
	return (0);
      stp1 = &st1;
    }

  if (stp2 == NULL)
    {
      if (stat (path2, &st2) != 0)
	return (0);
      stp2 = &st2;
    }

  return ((stp1->st_dev == stp2->st_dev) && (stp1->st_ino == stp2->st_ino));
}

/* Move FD to a number close to the maximum number of file descriptors
   allowed in the shell process, to avoid the user stepping on it with
   redirection and causing us extra work.  If CHECK_NEW is non-zero,
   we check whether or not the file descriptors are in use before
   duplicating FD onto them.  MAXFD says where to start checking the
   file descriptors.  If it's less than 20, we get the maximum value
   available from getdtablesize(2). */
int
move_to_high_fd (fd, check_new, maxfd)
     int fd, check_new, maxfd;
{
  int script_fd, nfds, ignore;

  if (maxfd < 20)
    {
      nfds = getdtablesize ();
      if (nfds <= 0)
	nfds = 20;
      if (nfds > HIGH_FD_MAX)
	nfds = HIGH_FD_MAX;		/* reasonable maximum */
    }
  else
    nfds = maxfd;

  for (nfds--; check_new && nfds > 3; nfds--)
    if (fcntl (nfds, F_GETFD, &ignore) == -1)
      break;

  if (nfds > 3 && fd != nfds && (script_fd = dup2 (fd, nfds)) != -1)
    {
      if (check_new == 0 || fd != fileno (stderr))	/* don't close stderr */
	close (fd);
      return (script_fd);
    }

  /* OK, we didn't find one less than our artificial maximum; return the
     original file descriptor. */
  return (fd);
}
 
/* Return non-zero if the characters from SAMPLE are not all valid
   characters to be found in the first line of a shell script.  We
   check up to the first newline, or SAMPLE_LEN, whichever comes first.
   All of the characters must be printable or whitespace. */

int
check_binary_file (sample, sample_len)
     const char *sample;
     int sample_len;
{
  register int i;
  unsigned char c;

  if (sample_len >= 4 && sample[0] == 0x7f && sample[1] == 'E' && sample[2] == 'L' && sample[3] == 'F')
    return 1;

  /* Generally we check the first line for NULs. If the first line looks like
     a `#!' interpreter specifier, we just look for NULs anywhere in the
     buffer. */
  if (sample[0] == '#' && sample[1] == '!')
    return (memchr (sample, '\0', sample_len) != NULL);

  for (i = 0; i < sample_len; i++)
    {
      c = sample[i];
      if (c == '\n')
	return (0);
      if (c == '\0')
	return (1);
    }

  return (0);
}

/* **************************************************************** */
/*								    */
/*		    Functions to manipulate pipes		    */
/*								    */
/* **************************************************************** */

int
sh_openpipe (pv)
     int *pv;
{
  int r;

  if ((r = pipe (pv)) < 0)
    return r;

  pv[0] = move_to_high_fd (pv[0], 1, 64);
  pv[1] = move_to_high_fd (pv[1], 1, 64);

  return 0;  
}

int
sh_closepipe (pv)
     int *pv;
{
  if (pv[0] >= 0)
    close (pv[0]);

  if (pv[1] >= 0)
    close (pv[1]);

  pv[0] = pv[1] = -1;
  return 0;
}

/* **************************************************************** */
/*								    */
/*		    Functions to inspect pathnames		    */
/*								    */
/* **************************************************************** */

int
file_exists (fn)
     const char *fn;
{
  struct stat sb;

  return (stat (fn, &sb) == 0);
}

int
file_isdir (fn)
     const char *fn;
{
  struct stat sb;

  return ((stat (fn, &sb) == 0) && S_ISDIR (sb.st_mode));
}

int
file_iswdir (fn)
     const char *fn;
{
  return (file_isdir (fn) && sh_eaccess (fn, W_OK) == 0);
}

/* Return 1 if STRING is "." or "..", optionally followed by a directory
   separator */
int
path_dot_or_dotdot (string)
     const char *string;
{
  if (string == 0 || *string == '\0' || *string != '.')
    return (0);

  /* string[0] == '.' */
  if (PATHSEP(string[1]) || (string[1] == '.' && PATHSEP(string[2])))
    return (1);

  return (0);
}

/* Return 1 if STRING contains an absolute pathname, else 0.  Used by `cd'
   to decide whether or not to look up a directory name in $CDPATH. */
int
absolute_pathname (string)
     const char *string;
{
  if (string == 0 || *string == '\0')
    return (0);

  if (ABSPATH(string))
    return (1);

  if (string[0] == '.' && PATHSEP(string[1]))	/* . and ./ */
    return (1);

  if (string[0] == '.' && string[1] == '.' && PATHSEP(string[2]))	/* .. and ../ */
    return (1);

  return (0);
}

/* Return 1 if STRING is an absolute program name; it is absolute if it
   contains any slashes.  This is used to decide whether or not to look
   up through $PATH. */
int
absolute_program (string)
     const char *string;
{
  return ((char *)mbschr (string, '/') != (char *)NULL);
}

/* **************************************************************** */
/*								    */
/*		    Functions to manipulate pathnames		    */
/*								    */
/* **************************************************************** */

/* Turn STRING (a pathname) into an absolute pathname, assuming that
   DOT_PATH contains the symbolic location of `.'.  This always
   returns a new string, even if STRING was an absolute pathname to
   begin with. */
char *
make_absolute (string, dot_path)
     const char *string, *dot_path;
{
  char *result;

  if (dot_path == 0 || ABSPATH(string))
#ifdef __CYGWIN__
    {
      char pathbuf[PATH_MAX + 1];

      /* WAS cygwin_conv_to_full_posix_path (string, pathbuf); */
      cygwin_conv_path (CCP_WIN_A_TO_POSIX, string, pathbuf, PATH_MAX);
      result = savestring (pathbuf);
    }
#else
    result = savestring (string);
#endif
  else
    result = sh_makepath (dot_path, string, 0);

  return (result);
}

/* Return the `basename' of the pathname in STRING (the stuff after the
   last '/').  If STRING is `/', just return it. */
char *
base_pathname (string)
     char *string;
{
  char *p;

#if 0
  if (absolute_pathname (string) == 0)
    return (string);
#endif

  if (string[0] == '/' && string[1] == 0)
    return (string);

  p = (char *)strrchr (string, '/');
  return (p ? ++p : string);
}

/* Return the full pathname of FILE.  Easy.  Filenames that begin
   with a '/' are returned as themselves.  Other filenames have
   the current working directory prepended.  A new string is
   returned in either case. */
char *
full_pathname (file)
     char *file;
{
  char *ret;

  file = (*file == '~') ? bash_tilde_expand (file, 0) : savestring (file);

  if (ABSPATH(file))
    return (file);

  ret = sh_makepath ((char *)NULL, file, (MP_DOCWD|MP_RMDOT));
  free (file);

  return (ret);
}

/* A slightly related function.  Get the prettiest name of this
   directory possible. */
static char tdir[PATH_MAX];

/* Return a pretty pathname.  If the first part of the pathname is
   the same as $HOME, then replace that with `~'.  */
char *
polite_directory_format (name)
     char *name;
{
  char *home;
  int l;

  home = get_string_value ("HOME");
  l = home ? strlen (home) : 0;
  if (l > 1 && strncmp (home, name, l) == 0 && (!name[l] || name[l] == '/'))
    {
      strncpy (tdir + 1, name + l, sizeof(tdir) - 2);
      tdir[0] = '~';
      tdir[sizeof(tdir) - 1] = '\0';
      return (tdir);
    }
  else
    return (name);
}

/* Trim NAME.  If NAME begins with `~/', skip over tilde prefix.  Trim to
   keep any tilde prefix and PROMPT_DIRTRIM trailing directory components
   and replace the intervening characters with `...' */
char *
trim_pathname (name, maxlen)
     char *name;
     int maxlen;
{
  int nlen, ndirs;
  intmax_t nskip;
  char *nbeg, *nend, *ntail, *v;

  if (name == 0 || (nlen = strlen (name)) == 0)
    return name;
  nend = name + nlen;

  v = get_string_value ("PROMPT_DIRTRIM");
  if (v == 0 || *v == 0)
    return name;
  if (legal_number (v, &nskip) == 0 || nskip <= 0)
    return name;

  /* Skip over tilde prefix */
  nbeg = name;
  if (name[0] == '~')
    for (nbeg = name; *nbeg; nbeg++)
      if (*nbeg == '/')
	{
	  nbeg++;
	  break;
	}
  if (*nbeg == 0)
    return name;

  for (ndirs = 0, ntail = nbeg; *ntail; ntail++)
    if (*ntail == '/')
      ndirs++;
  if (ndirs < nskip)
    return name;

  for (ntail = (*nend == '/') ? nend : nend - 1; ntail > nbeg; ntail--)
    {
      if (*ntail == '/')
	nskip--;
      if (nskip == 0)
	break;
    }
  if (ntail == nbeg)
    return name;

  /* Now we want to return name[0..nbeg]+"..."+ntail, modifying name in place */
  nlen = ntail - nbeg;
  if (nlen <= 3)
    return name;

  *nbeg++ = '.';
  *nbeg++ = '.';
  *nbeg++ = '.';

  nlen = nend - ntail;
  memmove (nbeg, ntail, nlen);
  nbeg[nlen] = '\0';

  return name;
}

/* Return a printable representation of FN without special characters.  The
   caller is responsible for freeing memory if this returns something other
   than its argument.  If FLAGS is non-zero, we are printing for portable
   re-input and should single-quote filenames appropriately. */
char *
printable_filename (fn, flags)
     char *fn;
     int flags;
{
  char *newf;

  if (ansic_shouldquote (fn))
    newf = ansic_quote (fn, 0, NULL);
  else if (flags && sh_contains_shell_metas (fn))
    newf = sh_single_quote (fn);
  else
    newf = fn;

  return newf;
}

/* Given a string containing units of information separated by colons,
   return the next one pointed to by (P_INDEX), or NULL if there are no more.
   Advance (P_INDEX) to the character after the colon. */
char *
extract_colon_unit (string, p_index)
     char *string;
     int *p_index;
{
  int i, start, len;
  char *value;

  if (string == 0)
    return (string);

  len = strlen (string);
  if (*p_index >= len)
    return ((char *)NULL);

  i = *p_index;

  /* Each call to this routine leaves the index pointing at a colon if
     there is more to the path.  If I is > 0, then increment past the
     `:'.  If I is 0, then the path has a leading colon.  Trailing colons
     are handled OK by the `else' part of the if statement; an empty
     string is returned in that case. */
  if (i && string[i] == ':')
    i++;

  for (start = i; string[i] && string[i] != ':'; i++)
    ;

  *p_index = i;

  if (i == start)
    {
      if (string[i])
	(*p_index)++;
      /* Return "" in the case of a trailing `:'. */
      value = (char *)xmalloc (1);
      value[0] = '\0';
    }
  else
    value = substring (string, start, i);

  return (value);
}

/* **************************************************************** */
/*								    */
/*		    Tilde Initialization and Expansion		    */
/*								    */
/* **************************************************************** */

#if defined (PUSHD_AND_POPD)
extern char *get_dirstack_from_string PARAMS((char *));
#endif

static char **bash_tilde_prefixes;
static char **bash_tilde_prefixes2;
static char **bash_tilde_suffixes;
static char **bash_tilde_suffixes2;

/* If tilde_expand hasn't been able to expand the text, perhaps it
   is a special shell expansion.  This function is installed as the
   tilde_expansion_preexpansion_hook.  It knows how to expand ~- and ~+.
   If PUSHD_AND_POPD is defined, ~[+-]N expands to directories from the
   directory stack. */
static char *
bash_special_tilde_expansions (text)
     char *text;
{
  char *result;

  result = (char *)NULL;

  if (text[0] == '+' && text[1] == '\0')
    result = get_string_value ("PWD");
  else if (text[0] == '-' && text[1] == '\0')
    result = get_string_value ("OLDPWD");
#if defined (PUSHD_AND_POPD)
  else if (DIGIT (*text) || ((*text == '+' || *text == '-') && DIGIT (text[1])))
    result = get_dirstack_from_string (text);
#endif

  return (result ? savestring (result) : (char *)NULL);
}

/* Initialize the tilde expander.  In Bash, we handle `~-' and `~+', as
   well as handling special tilde prefixes; `:~" and `=~' are indications
   that we should do tilde expansion. */
void
tilde_initialize ()
{
  static int times_called = 0;

  /* Tell the tilde expander that we want a crack first. */
  tilde_expansion_preexpansion_hook = bash_special_tilde_expansions;

  /* Tell the tilde expander about special strings which start a tilde
     expansion, and the special strings that end one.  Only do this once.
     tilde_initialize () is called from within bashline_reinitialize (). */
  if (times_called++ == 0)
    {
      bash_tilde_prefixes = strvec_create (3);
      bash_tilde_prefixes[0] = "=~";
      bash_tilde_prefixes[1] = ":~";
      bash_tilde_prefixes[2] = (char *)NULL;

      bash_tilde_prefixes2 = strvec_create (2);
      bash_tilde_prefixes2[0] = ":~";
      bash_tilde_prefixes2[1] = (char *)NULL;

      tilde_additional_prefixes = bash_tilde_prefixes;

      bash_tilde_suffixes = strvec_create (3);
      bash_tilde_suffixes[0] = ":";
      bash_tilde_suffixes[1] = "=~";	/* XXX - ?? */
      bash_tilde_suffixes[2] = (char *)NULL;

      tilde_additional_suffixes = bash_tilde_suffixes;

      bash_tilde_suffixes2 = strvec_create (2);
      bash_tilde_suffixes2[0] = ":";
      bash_tilde_suffixes2[1] = (char *)NULL;
    }
}

/* POSIX.2, 3.6.1:  A tilde-prefix consists of an unquoted tilde character
   at the beginning of the word, followed by all of the characters preceding
   the first unquoted slash in the word, or all the characters in the word
   if there is no slash...If none of the characters in the tilde-prefix are
   quoted, the characters in the tilde-prefix following the tilde shell be
   treated as a possible login name. */

#define TILDE_END(c)	((c) == '\0' || (c) == '/' || (c) == ':')

static int
unquoted_tilde_word (s)
     const char *s;
{
  const char *r;

  for (r = s; TILDE_END(*r) == 0; r++)
    {
      switch (*r)
	{
	case '\\':
	case '\'':
	case '"':
	  return 0;
	}
    }
  return 1;
}

/* Find the end of the tilde-prefix starting at S, and return the tilde
   prefix in newly-allocated memory.  Return the length of the string in
   *LENP.  FLAGS tells whether or not we're in an assignment context --
   if so, `:' delimits the end of the tilde prefix as well. */
char *
bash_tilde_find_word (s, flags, lenp)
     const char *s;
     int flags, *lenp;
{
  const char *r;
  char *ret;
  int l;

  for (r = s; *r && *r != '/'; r++)
    {
      /* Short-circuit immediately if we see a quote character.  Even though
	 POSIX says that `the first unquoted slash' (or `:') terminates the
	 tilde-prefix, in practice, any quoted portion of the tilde prefix
	 will cause it to not be expanded. */
      if (*r == '\\' || *r == '\'' || *r == '"')  
	{
	  ret = savestring (s);
	  if (lenp)
	    *lenp = 0;
	  return ret;
	}
      else if (flags && *r == ':')
	break;
    }
  l = r - s;
  ret = xmalloc (l + 1);
  strncpy (ret, s, l);
  ret[l] = '\0';
  if (lenp)
    *lenp = l;
  return ret;
}
    
/* Tilde-expand S by running it through the tilde expansion library.
   ASSIGN_P is 1 if this is a variable assignment, so the alternate
   tilde prefixes should be enabled (`=~' and `:~', see above).  If
   ASSIGN_P is 2, we are expanding the rhs of an assignment statement,
   so `=~' is not valid. */
char *
bash_tilde_expand (s, assign_p)
     const char *s;
     int assign_p;
{
  int r;
  char *ret;

  tilde_additional_prefixes = assign_p == 0 ? (char **)0
  					    : (assign_p == 2 ? bash_tilde_prefixes2 : bash_tilde_prefixes);
  if (assign_p == 2)
    tilde_additional_suffixes = bash_tilde_suffixes2;

  r = (*s == '~') ? unquoted_tilde_word (s) : 1;
  ret = r ? tilde_expand (s) : savestring (s);

  QUIT;

  return (ret);
}

/* **************************************************************** */
/*								    */
/*	  Functions to manipulate and search the group list	    */
/*								    */
/* **************************************************************** */

static int ngroups, maxgroups;

/* The set of groups that this user is a member of. */
static GETGROUPS_T *group_array = (GETGROUPS_T *)NULL;

#if !defined (NOGROUP)
#  define NOGROUP (gid_t) -1
#endif

static void
initialize_group_array ()
{
  register int i;

  if (maxgroups == 0)
    maxgroups = getmaxgroups ();

  ngroups = 0;
  group_array = (GETGROUPS_T *)xrealloc (group_array, maxgroups * sizeof (GETGROUPS_T));

#if defined (HAVE_GETGROUPS)
  ngroups = getgroups (maxgroups, group_array);
#endif

  /* If getgroups returns nothing, or the OS does not support getgroups(),
     make sure the groups array includes at least the current gid. */
  if (ngroups == 0)
    {
      group_array[0] = current_user.gid;
      ngroups = 1;
    }

  /* If the primary group is not in the groups array, add it as group_array[0]
     and shuffle everything else up 1, if there's room. */
  for (i = 0; i < ngroups; i++)
    if (current_user.gid == (gid_t)group_array[i])
      break;
  if (i == ngroups && ngroups < maxgroups)
    {
      for (i = ngroups; i > 0; i--)
	group_array[i] = group_array[i - 1];
      group_array[0] = current_user.gid;
      ngroups++;
    }

  /* If the primary group is not group_array[0], swap group_array[0] and
     whatever the current group is.  The vast majority of systems should
     not need this; a notable exception is Linux. */
  if (group_array[0] != current_user.gid)
    {
      for (i = 0; i < ngroups; i++)
	if (group_array[i] == current_user.gid)
	  break;
      if (i < ngroups)
	{
	  group_array[i] = group_array[0];
	  group_array[0] = current_user.gid;
	}
    }
}

/* Return non-zero if GID is one that we have in our groups list. */
int
#if defined (__STDC__) || defined ( _MINIX)
group_member (gid_t gid)
#else
group_member (gid)
     gid_t gid;
#endif /* !__STDC__ && !_MINIX */
{
#if defined (HAVE_GETGROUPS)
  register int i;
#endif

  /* Short-circuit if possible, maybe saving a call to getgroups(). */
  if (gid == current_user.gid || gid == current_user.egid)
    return (1);

#if defined (HAVE_GETGROUPS)
  if (ngroups == 0)
    initialize_group_array ();

  /* In case of error, the user loses. */
  if (ngroups <= 0)
    return (0);

  /* Search through the list looking for GID. */
  for (i = 0; i < ngroups; i++)
    if (gid == (gid_t)group_array[i])
      return (1);
#endif

  return (0);
}

char **
get_group_list (ngp)
     int *ngp;
{
  static char **group_vector = (char **)NULL;
  register int i;

  if (group_vector)
    {
      if (ngp)
	*ngp = ngroups;
      return group_vector;
    }

  if (ngroups == 0)
    initialize_group_array ();

  if (ngroups <= 0)
    {
      if (ngp)
	*ngp = 0;
      return (char **)NULL;
    }

  group_vector = strvec_create (ngroups);
  for (i = 0; i < ngroups; i++)
    group_vector[i] = itos (group_array[i]);

  if (ngp)
    *ngp = ngroups;
  return group_vector;
}

int *
get_group_array (ngp)
     int *ngp;
{
  int i;
  static int *group_iarray = (int *)NULL;

  if (group_iarray)
    {
      if (ngp)
	*ngp = ngroups;
      return (group_iarray);
    }

  if (ngroups == 0)
    initialize_group_array ();    

  if (ngroups <= 0)
    {
      if (ngp)
	*ngp = 0;
      return (int *)NULL;
    }

  group_iarray = (int *)xmalloc (ngroups * sizeof (int));
  for (i = 0; i < ngroups; i++)
    group_iarray[i] = (int)group_array[i];

  if (ngp)
    *ngp = ngroups;
  return group_iarray;
}

/* **************************************************************** */
/*								    */
/*	  Miscellaneous functions				    */
/*								    */
/* **************************************************************** */

/* Return a value for PATH that is guaranteed to find all of the standard
   utilities.  This uses Posix.2 configuration variables, if present.  It
   uses a value defined in config.h as a last resort. */
char *
conf_standard_path ()
{
#if defined (_CS_PATH) && defined (HAVE_CONFSTR)
  char *p;
  size_t len;

  len = (size_t)confstr (_CS_PATH, (char *)NULL, (size_t)0);
  if (len > 0)
    {
      p = (char *)xmalloc (len + 2);
      *p = '\0';
      confstr (_CS_PATH, p, len);
      return (p);
    }
  else
    return (savestring (STANDARD_UTILS_PATH));
#else /* !_CS_PATH || !HAVE_CONFSTR  */
#  if defined (CS_PATH)
  return (savestring (CS_PATH));
#  else
  return (savestring (STANDARD_UTILS_PATH));
#  endif /* !CS_PATH */
#endif /* !_CS_PATH || !HAVE_CONFSTR */
}

int
default_columns ()
{
  char *v;
  int c;

  c = -1;
  v = get_string_value ("COLUMNS");
  if (v && *v)
    {
      c = atoi (v);
      if (c > 0)
	return c;
    }

  if (check_window_size)
    get_new_window_size (0, (int *)0, &c);

  return (c > 0 ? c : 80);
}

  
