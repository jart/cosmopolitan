/* trap.c, created from trap.def. */
#line 22 "./trap.def"

#line 58 "./trap.def"

#include "config.h"

#if defined (HAVE_UNISTD_H)
#  ifdef _MINIX
#    include <sys/types.h>
#  endif
#  include <unistd.h>
#endif

#include "bashtypes.h"
#include <signal.h>
#include <stdio.h>
#include "bashansi.h"

#include "shell.h"
#include "trap.h"
#include "common.h"
#include "bashgetopt.h"

static void showtrap PARAMS((int, int));
static int display_traps PARAMS((WORD_LIST *, int));

/* The trap command:

   trap <arg> <signal ...>
   trap <signal ...>
   trap -l
   trap -p [sigspec ...]
   trap [--]

   Set things up so that ARG is executed when SIGNAL(s) N is received.
   If ARG is the empty string, then ignore the SIGNAL(s).  If there is
   no ARG, then set the trap for SIGNAL(s) to its original value.  Just
   plain "trap" means to print out the list of commands associated with
   each signal number.  Single arg of "-l" means list the signal names. */

/* Possible operations to perform on the list of signals.*/
#define SET 0			/* Set this signal to first_arg. */
#define REVERT 1		/* Revert to this signals original value. */
#define IGNORE 2		/* Ignore this signal. */

int
trap_builtin (list)
     WORD_LIST *list;
{
  int list_signal_names, display, result, opt;

  list_signal_names = display = 0;
  result = EXECUTION_SUCCESS;

  reset_internal_getopt ();
  while ((opt = internal_getopt (list, "lp")) != -1)
    {
      switch (opt)
	{
	case 'l':
	  list_signal_names++;
	  break;
	case 'p':
	  display++;
	  break;
	CASE_HELPOPT;
	default:
	  builtin_usage ();
	  return (EX_USAGE);
	}
    }
  list = loptend;

  opt = DSIG_NOCASE|DSIG_SIGPREFIX;	/* flags for decode_signal */

  if (list_signal_names)
    return (sh_chkwrite (display_signal_list ((WORD_LIST *)NULL, 1)));
  else if (display || list == 0)
    {
      initialize_terminating_signals ();
      get_all_original_signals ();
      return (sh_chkwrite (display_traps (list, display && posixly_correct)));
    }
  else
    {
      char *first_arg;
      int operation, sig, first_signal;

      operation = SET;
      first_arg = list->word->word;
      first_signal = first_arg && *first_arg && all_digits (first_arg) && signal_object_p (first_arg, opt);

      /* Backwards compatibility.  XXX - question about whether or not we
	 should throw an error if an all-digit argument doesn't correspond
	 to a valid signal number (e.g., if it's `50' on a system with only
	 32 signals).  */
      if (first_signal)
	operation = REVERT;
      /* When in posix mode, the historical behavior of looking for a
	 missing first argument is disabled.  To revert to the original
	 signal handling disposition, use `-' as the first argument. */
      else if (posixly_correct == 0 && first_arg && *first_arg &&
		(*first_arg != '-' || first_arg[1]) &&
		signal_object_p (first_arg, opt) && list->next == 0)
	operation = REVERT;
      else
	{
	  list = list->next;
	  if (list == 0)
	    {
	      builtin_usage ();
	      return (EX_USAGE);
	    }
	  else if (*first_arg == '\0')
	    operation = IGNORE;
	  else if (first_arg[0] == '-' && !first_arg[1])
	    operation = REVERT;
	}

      /* If we're in a command substitution, we haven't freed the trap strings
	 (though we reset the signal handlers).  If we're setting a trap to
	 handle a signal here, free the rest of the trap strings since they
	 don't apply any more. */
      if (subshell_environment & SUBSHELL_RESETTRAP)
	{
	  free_trap_strings ();
	  subshell_environment &= ~SUBSHELL_RESETTRAP;
	}

      while (list)
	{
	  sig = decode_signal (list->word->word, opt);

	  if (sig == NO_SIG)
	    {
	      sh_invalidsig (list->word->word);
	      result = EXECUTION_FAILURE;
	    }
	  else
	    {
	      switch (operation)
		{
		  case SET:
		    set_signal (sig, first_arg);
		    break;

		  case REVERT:
		    restore_default_signal (sig);

		    /* Signals that the shell treats specially need special
		       handling. */
		    switch (sig)
		      {
		      case SIGINT:
			/* XXX - should we do this if original disposition
			   was SIG_IGN? */
			if (interactive)
			  set_signal_handler (SIGINT, sigint_sighandler);
			/* special cases for interactive == 0 */
			else if (interactive_shell && (sourcelevel||running_trap||parse_and_execute_level))
			  set_signal_handler (SIGINT, sigint_sighandler);
			else
			  set_signal_handler (SIGINT, termsig_sighandler);
			break;

		      case SIGQUIT:
			/* Always ignore SIGQUIT. */
			set_signal_handler (SIGQUIT, SIG_IGN);
			break;
		      case SIGTERM:
#if defined (JOB_CONTROL)
		      case SIGTTIN:
		      case SIGTTOU:
		      case SIGTSTP:
#endif /* JOB_CONTROL */
			if (interactive)
			  set_signal_handler (sig, SIG_IGN);
			break;
		      }
		    break;

		  case IGNORE:
		    ignore_signal (sig);
		    break;
		}
	    }
	  list = list->next;
	}
    }

  return (result);
}

static void
showtrap (i, show_default)
     int i, show_default;
{
  char *t, *p, *sn;
  int free_t;

  free_t = 1;
  p = trap_list[i];
  if (p == (char *)DEFAULT_SIG && signal_is_hard_ignored (i) == 0)
    {
      if (show_default)
	t = "-";
      else
	return;
      free_t = 0;
    }
  else if (signal_is_hard_ignored (i))
    t = (char *)NULL;
  else
    t = (p == (char *)IGNORE_SIG) ? (char *)NULL : sh_single_quote (p);

  sn = signal_name (i);
  /* Make sure that signals whose names are unknown (for whatever reason)
     are printed as signal numbers. */
  if (STREQN (sn, "SIGJUNK", 7) || STREQN (sn, "unknown", 7))
    printf ("trap -- %s %d\n", t ? t : "''", i);
  else if (posixly_correct)
    {
      if (STREQN (sn, "SIG", 3))
	printf ("trap -- %s %s\n", t ? t : "''", sn+3);
      else
	printf ("trap -- %s %s\n", t ? t : "''", sn);
    }
  else
    printf ("trap -- %s %s\n", t ? t : "''", sn);

  if (free_t)
    FREE (t);
}

static int
display_traps (list, show_all)
     WORD_LIST *list;
     int show_all;
{
  int result, i;

  if (list == 0)
    {
      for (i = 0; i < BASH_NSIG; i++)
	showtrap (i, show_all);
      return (EXECUTION_SUCCESS);
    }

  for (result = EXECUTION_SUCCESS; list; list = list->next)
    {
      i = decode_signal (list->word->word, DSIG_NOCASE|DSIG_SIGPREFIX);
      if (i == NO_SIG)
	{
	  sh_invalidsig (list->word->word);
	  result = EXECUTION_FAILURE;
	}
      else
	showtrap (i, show_all);
    }

  return (result);
}
