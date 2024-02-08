/* mapfile.c, created from mapfile.def. */
#line 23 "./mapfile.def"

#line 59 "./mapfile.def"

#line 67 "./mapfile.def"

#include "config.h"

#include "builtins.h"
#include "bashtypes.h"
#include "posixstat.h"

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#include "bashansi.h"
#include "bashintl.h"

#include <stdio.h>
#include <errno.h>

#include "bashintl.h"
#include "shell.h"
#include "common.h"
#include "bashgetopt.h"

#if !defined (errno)
extern int errno;
#endif

#if defined (ARRAY_VARS)

static int run_callback PARAMS((const char *, unsigned int, const char *));

#define DEFAULT_ARRAY_NAME	"MAPFILE"
#define DEFAULT_VARIABLE_NAME	"MAPLINE"	/* not used right now */

/* The value specifying how frequently `mapfile'  calls the callback. */
#define DEFAULT_QUANTUM 5000

/* Values for FLAGS */
#define MAPF_CLEARARRAY	0x01
#define MAPF_CHOP	0x02

static int delim;

static int
run_callback (callback, curindex, curline)
     const char *callback;
     unsigned int curindex;
     const char *curline;
{
  unsigned int execlen;
  char  *execstr, *qline;
  int flags;

  qline = sh_single_quote (curline);
  execlen = strlen (callback) + strlen (qline) + 10;
  /* 1 for each space between %s and %d,
     another 1 for the last nul char for C string. */
  execlen += 3;
  execstr = xmalloc (execlen);

  flags = SEVAL_NOHIST;
#if 0
  if (interactive)
    flags |= SEVAL_INTERACT;
#endif
  snprintf (execstr, execlen, "%s %d %s", callback, curindex, qline);
  free (qline);
  return evalstring (execstr, NULL, flags);
}

static void
do_chop(line, delim)
     char *line;
     unsigned char delim;
{
  int length;

  length = strlen (line);
  if (length && (unsigned char)line[length-1] == delim) 
    line[length-1] = '\0';
}

static int
mapfile (fd, line_count_goal, origin, nskip, callback_quantum, callback, array_name, delim, flags)
     int fd;
     long line_count_goal, origin, nskip, callback_quantum;
     char *callback, *array_name;
     int delim;
     int flags;
{
  char *line;
  size_t line_length;
  unsigned int array_index, line_count;
  SHELL_VAR *entry;
  struct stat sb;
  int unbuffered_read;
  
  line = NULL;
  line_length = 0;
  unbuffered_read = 0;

  /* The following check should be done before reading any lines.  Doing it
     here allows us to call bind_array_element instead of bind_array_variable
     and skip the variable lookup on every call. */
  entry = builtin_find_indexed_array (array_name, flags & MAPF_CLEARARRAY);
  if (entry == 0)
    return EXECUTION_FAILURE;

#ifndef __CYGWIN__
  /* If the delimiter is a newline, turn on unbuffered reads for pipes
     (terminals are ok). If the delimiter is not a newline, unbuffered reads
     for every file descriptor that's not a regular file. */
  if (delim == '\n')
    unbuffered_read = (lseek (fd, 0L, SEEK_CUR) < 0) && (errno == ESPIPE);
  else
    unbuffered_read = (fstat (fd, &sb) != 0) || (S_ISREG (sb.st_mode) == 0);
#else
  unbuffered_read = 1;
#endif

  zreset ();

  /* Skip any lines at beginning of file? */
  for (line_count = 0; line_count < nskip; line_count++)
    if (zgetline (fd, &line, &line_length, delim, unbuffered_read) < 0)
      break;

  line = 0;
  line_length = 0;    

  /* Reset the buffer for bash own stream */
  for (array_index = origin, line_count = 1; 
 	zgetline (fd, &line, &line_length, delim, unbuffered_read) != -1;
	array_index++) 
    {
      /* Remove trailing newlines? */
      if (flags & MAPF_CHOP)
	do_chop (line, delim);
	  
      /* Has a callback been registered and if so is it time to call it? */
      if (callback && line_count && (line_count % callback_quantum) == 0) 
	{
	  /* Reset the buffer for bash own stream. */
	  if (unbuffered_read == 0)
	    zsyncfd (fd);

	  run_callback (callback, array_index, line);
	}

      /* XXX - bad things can happen if the callback modifies ENTRY, e.g.,
	 unsetting it or changing it to a non-indexed-array type. */
      bind_array_element (entry, array_index, line, 0);

      /* Have we exceeded # of lines to store? */
      line_count++;
      if (line_count_goal != 0 && line_count > line_count_goal) 
	break;
    }

  free (line);

  if (unbuffered_read == 0)
    zsyncfd (fd);

  return EXECUTION_SUCCESS;
}

int
mapfile_builtin (list)
     WORD_LIST *list;
{
  int opt, code, fd, flags;
  intmax_t intval;
  long lines, origin, nskip, callback_quantum;
  char *array_name, *callback;

  fd = 0;
  lines = origin = nskip = 0;
  flags = MAPF_CLEARARRAY;
  callback_quantum = DEFAULT_QUANTUM;
  callback = 0;
  delim = '\n';

  reset_internal_getopt ();
  while ((opt = internal_getopt (list, "d:u:n:O:tC:c:s:")) != -1)
    {
      switch (opt)
	{
	case 'd':
	  delim = *list_optarg;
	  break;
	case 'u':
	  code = legal_number (list_optarg, &intval);
	  if (code == 0 || intval < 0 || intval != (int)intval)
	    {
	      builtin_error (_("%s: invalid file descriptor specification"), list_optarg);
	      return (EXECUTION_FAILURE);
	    }
	  else
	    fd = intval;

	  if (sh_validfd (fd) == 0)
	    {
	      builtin_error (_("%d: invalid file descriptor: %s"), fd, strerror (errno));
	      return (EXECUTION_FAILURE);
	    }
	  break;	  

	case 'n':
	  code = legal_number (list_optarg, &intval);
	  if (code == 0 || intval < 0 || intval != (unsigned)intval)
	    {
	      builtin_error (_("%s: invalid line count"), list_optarg);
	      return (EXECUTION_FAILURE);
	    }
	  else
	    lines = intval;
	  break;

	case 'O':
	  code = legal_number (list_optarg, &intval);
	  if (code == 0 || intval < 0 || intval != (unsigned)intval)
	    {
	      builtin_error (_("%s: invalid array origin"), list_optarg);
	      return (EXECUTION_FAILURE);
	    }
	  else
	    origin = intval;
	  flags &= ~MAPF_CLEARARRAY;
	  break;
	case 't':
	  flags |= MAPF_CHOP;
	  break;
	case 'C':
	  callback = list_optarg;
	  break;
	case 'c':
	  code = legal_number (list_optarg, &intval);
	  if (code == 0 || intval <= 0 || intval != (unsigned)intval)
	    {
	      builtin_error (_("%s: invalid callback quantum"), list_optarg);
	      return (EXECUTION_FAILURE);
	    }
	  else
	    callback_quantum = intval;
	  break;
	case 's':
	  code = legal_number (list_optarg, &intval);
	  if (code == 0 || intval < 0 || intval != (unsigned)intval)
	    {
	      builtin_error (_("%s: invalid line count"), list_optarg);
	      return (EXECUTION_FAILURE);
	    }
	  else
	    nskip = intval;
	  break;
	CASE_HELPOPT;
	default:
	  builtin_usage ();
	  return (EX_USAGE);
	}
    }
  list = loptend;

  if (list == 0) 
    array_name = DEFAULT_ARRAY_NAME;
  else if (list->word == 0 || list->word->word == 0)
    {
      builtin_error ("internal error: getting variable name");
      return (EXECUTION_FAILURE);
    }
  else if (list->word->word[0] == '\0')
    {
      builtin_error (_("empty array variable name"));
      return (EX_USAGE);
    } 
  else
    array_name = list->word->word;
  
  if (legal_identifier (array_name) == 0)
    {
      sh_invalidid (array_name);
      return (EXECUTION_FAILURE);
    }

  return mapfile (fd, lines, origin, nskip, callback_quantum, callback, array_name, delim, flags);
}

#else

int
mapfile_builtin (list)
     WORD_LIST *list;
{
  builtin_error (_("array variable support required"));
  return (EXECUTION_FAILURE);
}

#endif  /* ARRAY_VARS */
