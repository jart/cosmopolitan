/* history.c, created from history.def. */
#line 22 "./history.def"

#line 58 "./history.def"

#include "config.h"

#if defined (HISTORY)
#include "bashtypes.h"
#if ! defined(_MINIX) && defined (HAVE_SYS_FILE_H)
#  include <sys/file.h>
#endif
#include "posixstat.h"
#include "filecntl.h"
#include <errno.h>
#include <stdio.h>
#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#include "bashansi.h"
#include "bashintl.h"

#include "shell.h"
#include "flags.h"
#include "parser.h"
#include "bashhist.h"
#include "third_party/readline/history.h"
#include "bashgetopt.h"
#include "common.h"

#if !defined (errno)
extern int errno;
#endif

static char *histtime PARAMS((HIST_ENTRY *, const char *));
static int display_history PARAMS((WORD_LIST *));
static void push_history PARAMS((WORD_LIST *));
static int expand_and_print_history PARAMS((WORD_LIST *));

#define AFLAG	0x01
#define RFLAG	0x02
#define WFLAG	0x04
#define NFLAG	0x08
#define SFLAG	0x10
#define PFLAG	0x20
#define CFLAG	0x40
#define DFLAG	0x80

#ifndef TIMELEN_MAX
#  define TIMELEN_MAX 128
#endif

int
history_builtin (list)
     WORD_LIST *list;
{
  int flags, opt, result, old_history_lines, obase, ind;
  char *filename, *delete_arg, *range;
  intmax_t delete_offset;

  flags = 0;
  reset_internal_getopt ();
  while ((opt = internal_getopt (list, "acd:npsrw")) != -1)
    {
      switch (opt)
	{
	case 'a':
	  flags |= AFLAG;
	  break;
	case 'c':
	  flags |= CFLAG;
	  break;
	case 'n':
	  flags |= NFLAG;
	  break;
	case 'r':
	  flags |= RFLAG;
	  break;
	case 'w':
	  flags |= WFLAG;
	  break;
	case 's':
	  flags |= SFLAG;
	  break;
	case 'd':
	  flags |= DFLAG;
	  delete_arg = list_optarg;
	  break;
	case 'p':
#if defined (BANG_HISTORY)
	  flags |= PFLAG;
#endif
	  break;
	CASE_HELPOPT;
	default:
	  builtin_usage ();
	  return (EX_USAGE);
	}
    }
  list = loptend;

  opt = flags & (AFLAG|RFLAG|WFLAG|NFLAG);
  if (opt && opt != AFLAG && opt != RFLAG && opt != WFLAG && opt != NFLAG)
    {
      builtin_error (_("cannot use more than one of -anrw"));
      return (EXECUTION_FAILURE);
    }

  /* clear the history, but allow other arguments to add to it again. */
  if (flags & CFLAG)
    {
      bash_clear_history ();
      if (list == 0)
	return (EXECUTION_SUCCESS);
    }

  if (flags & SFLAG)
    {
      if (list)
	push_history (list);
      return (EXECUTION_SUCCESS);
    }
#if defined (BANG_HISTORY)
  else if (flags & PFLAG)
    {
      if (list)
	return (expand_and_print_history (list));
      return (sh_chkwrite (EXECUTION_SUCCESS));
    }
#endif
  else if ((flags & DFLAG) && (range = strchr ((delete_arg[0] == '-') ? delete_arg + 1 : delete_arg, '-')))
    {
      intmax_t delete_start, delete_end;
      *range++ = '\0';
      if (legal_number (delete_arg, &delete_start) == 0 || legal_number (range, &delete_end) == 0)
	{
	  range[-1] = '-';
	  sh_erange (delete_arg, _("history position"));
	  return (EXECUTION_FAILURE);
	}
      if (delete_arg[0] == '-' && delete_start < 0)
	/* the_history[history_length] == 0x0, so this is correct */
        delete_start += history_length;
      /* numbers as displayed by display_history are offset by history_base */
      else if (delete_start > 0)
	delete_start -= history_base;

      if (delete_start < 0 || delete_start >= history_length)
	{
	  sh_erange (delete_arg, _("history position"));
	  return (EXECUTION_FAILURE);
	}

      if (range[0] == '-' && delete_end < 0)
        delete_end += history_length;
      else if (delete_end > 0)
	delete_end -= history_base;

      if (delete_end < 0 || delete_end >= history_length)
	{
	  sh_erange (range, _("history position"));
	  return (EXECUTION_FAILURE);
	}
      /* XXX - print error if end < start? */
      result = bash_delete_history_range (delete_start, delete_end);
      if (where_history () > history_length)
	history_set_pos (history_length);
      return (result ? EXECUTION_SUCCESS : EXECUTION_FAILURE);
    }
  else if (flags & DFLAG)
    {
      if (legal_number (delete_arg, &delete_offset) == 0)
	{
	  sh_erange (delete_arg, _("history position"));
	  return (EXECUTION_FAILURE);
	}
      /* check for negative offsets, count back from end of list */
      if (delete_arg[0] == '-' && delete_offset < 0)
	{
	  /* since the_history[history_length] == 0x0, this calculation means
	     that history -d -1 will delete the last history entry, which at
	     this point is the history -d -1 we just added. */
	  ind = history_length + delete_offset;
	  if (ind < 0)		/* offset by history_base below */
	    {
	      sh_erange (delete_arg, _("history position"));
	      return (EXECUTION_FAILURE);
	    }
	  opt = ind + history_base;	/* compensate for opt - history_base below */
	}
      else if ((delete_offset < history_base) || (delete_offset >= (history_base + history_length)))
	{
	  sh_erange (delete_arg, _("history position"));
	  return (EXECUTION_FAILURE);
	}
      else
	opt = delete_offset;

      /* Positive arguments from numbers as displayed by display_history need
	 to be offset by history_base */
      result = bash_delete_histent (opt - history_base);
      /* Since remove_history changes history_length, this can happen if
	 we delete the last history entry. */
      if (where_history () > history_length)
	history_set_pos (history_length);
      return (result ? EXECUTION_SUCCESS : EXECUTION_FAILURE);
    }
  else if ((flags & (AFLAG|RFLAG|NFLAG|WFLAG|CFLAG)) == 0)
    {
      result = display_history (list);
      return (sh_chkwrite (result));
    }

  filename = list ? list->word->word : get_string_value ("HISTFILE");
  result = EXECUTION_SUCCESS;

#if defined (RESTRICTED_SHELL)
  if (restricted && strchr (filename, '/'))
    {
      sh_restricted (filename);
      return (EXECUTION_FAILURE);
    }
#endif

  if (flags & AFLAG)		/* Append session's history to file. */
    result = maybe_append_history (filename);
  else if (flags & WFLAG)	/* Write entire history. */
    result = write_history (filename);
  else if (flags & RFLAG)	/* Read entire file. */
    {
      result = read_history (filename);
      history_lines_in_file = history_lines_read_from_file;
      /* history_lines_in_file = where_history () + history_base - 1; */
    }
  else if (flags & NFLAG)	/* Read `new' history from file. */
    {
      /* Read all of the lines in the file that we haven't already read. */
      old_history_lines = history_lines_in_file;
      obase = history_base;

      using_history ();
      result = read_history_range (filename, history_lines_in_file, -1);
      using_history ();

      history_lines_in_file = history_lines_read_from_file;
      /* history_lines_in_file = where_history () + history_base - 1; */

      /* If we're rewriting the history file at shell exit rather than just
	 appending the lines from this session to it, the question is whether
	 we reset history_lines_this_session to 0, losing any history entries
	 we had before we read the new entries from the history file, or
	 whether we count the new entries we just read from the file as
	 history lines added during this session.
	 Right now, we do the latter.  This will cause these history entries
	 to be written to the history file along with any intermediate entries
	 we add when we do a `history -a', but the alternative is losing
	 them altogether. */
      if (force_append_history == 0)
	history_lines_this_session += history_lines_in_file - old_history_lines +
				    history_base - obase;
    }

  return (result ? EXECUTION_FAILURE : EXECUTION_SUCCESS);
}

/* Accessors for HIST_ENTRY lists that are called HLIST. */
#define histline(i) (hlist[(i)]->line)
#define histdata(i) (hlist[(i)]->data)

static char *
histtime (hlist, histtimefmt)
     HIST_ENTRY *hlist;
     const char *histtimefmt;
{
  static char timestr[TIMELEN_MAX];
  time_t t;
  struct tm *tm;

  t = history_get_time (hlist);
  tm = t ? localtime (&t) : 0;
  if (t && tm)
    strftime (timestr, sizeof (timestr), histtimefmt, tm);
  else if (hlist->timestamp && hlist->timestamp[0])
    snprintf (timestr, sizeof (timestr), _("%s: invalid timestamp"),
	(hlist->timestamp[0] == '#') ? hlist->timestamp + 1: hlist->timestamp);
  else
    strcpy (timestr, "??");
  return timestr;
}

static int
display_history (list)
     WORD_LIST *list;
{
  register int i;
  intmax_t limit;
  HIST_ENTRY **hlist;
  char *histtimefmt, *timestr;

  if (list)
    {
      if (get_numeric_arg (list, 0, &limit) == 0)
	return (EXECUTION_FAILURE);

      if (limit < 0)
	limit = -limit;
    }
  else
    limit = -1;

  hlist = history_list ();

  if (hlist)
    {
      for (i = 0;  hlist[i]; i++)
	;

      if (0 <= limit && limit < i)
	i -= limit;
      else
	i = 0;

      histtimefmt = get_string_value ("HISTTIMEFORMAT");

      while (hlist[i])
	{
	  QUIT;

	  timestr = (histtimefmt && *histtimefmt) ? histtime (hlist[i], histtimefmt) : (char *)NULL;
	  printf ("%5d%c %s%s\n", i + history_base,
		  histdata(i) ? '*' : ' ',
		  ((timestr && *timestr) ? timestr : ""),
		  histline(i));
	  i++;
	}
    }

  return (EXECUTION_SUCCESS);
}

/* Remove the last entry in the history list and add each argument in
   LIST to the history. */
static void
push_history (list)
     WORD_LIST *list;
{
  char *s;

  /* Delete the last history entry if it was a single entry added to the
     history list (generally the `history -s' itself), or if `history -s'
     is being used in a compound command and the compound command was
     added to the history as a single element (command-oriented history).
     If you don't want history -s to remove the compound command from the
     history, change #if 0 to #if 1 below. */
#if 0
  if (remember_on_history && hist_last_line_pushed == 0 &&
	hist_last_line_added && bash_delete_last_history () == 0)
#else
  if (remember_on_history && hist_last_line_pushed == 0 &&
	(hist_last_line_added ||
	  (current_command_line_count > 0 && current_command_first_line_saved && command_oriented_history))
      && bash_delete_last_history () == 0)
#endif
      return;

  s = string_list (list);
  /* Call check_add_history with FORCE set to 1 to skip the check against
     current_command_line_count.  If history -s is used in a compound
     command, the above code will delete the compound command's history
     entry and this call will add the line to the history as a separate
     entry.  Without FORCE=1, if current_command_line_count were > 1, the
     line would be appended to the entry before the just-deleted entry. */
  check_add_history (s, 1);	/* obeys HISTCONTROL, HISTIGNORE */

  hist_last_line_pushed = 1;	/* XXX */
  free (s);
}

#if defined (BANG_HISTORY)
static int
expand_and_print_history (list)
     WORD_LIST *list;
{
  char *s;
  int r, result;

  if (hist_last_line_pushed == 0 && hist_last_line_added && bash_delete_last_history () == 0)
    return EXECUTION_FAILURE;
  result = EXECUTION_SUCCESS;
  while (list)
    {
      r = history_expand (list->word->word, &s);
      if (r < 0)
	{
	  builtin_error (_("%s: history expansion failed"), list->word->word);
	  result = EXECUTION_FAILURE;
	}
      else
	{
	  fputs (s, stdout);
	  putchar ('\n');
	}
      FREE (s);
      list = list->next;
    }
  fflush (stdout);
  return result;
}
#endif /* BANG_HISTORY */
#endif /* HISTORY */
