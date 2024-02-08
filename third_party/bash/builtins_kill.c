/* kill.c, created from kill.def. */
#line 22 "./kill.def"

#line 46 "./kill.def"

#include "config.h"

#include <stdio.h>
#include <errno.h>
#if defined (HAVE_UNISTD_H)
#  ifdef _MINIX
#    include <sys/types.h>
#  endif
#  include <unistd.h>
#endif

#include "bashansi.h"
#include "bashintl.h"

#include <signal.h>

#include "shell.h"
#include "trap.h"
#include "jobs.h"
#include "common.h"

/* Not all systems declare ERRNO in errno.h... and some systems #define it! */
#if !defined (errno)
extern int errno;
#endif /* !errno */

static void kill_error PARAMS((pid_t, int));

#if !defined (CONTINUE_AFTER_KILL_ERROR)
#  define CONTINUE_OR_FAIL return (EXECUTION_FAILURE)
#else
#  define CONTINUE_OR_FAIL goto continue_killing
#endif /* CONTINUE_AFTER_KILL_ERROR */

/* Here is the kill builtin.  We only have it so that people can type
   kill -KILL %1?  No, if you fill up the process table this way you
   can still kill some. */
int
kill_builtin (list)
     WORD_LIST *list;
{
  int sig, any_succeeded, listing, saw_signal, dflags;
  char *sigspec, *word;
  pid_t pid;
  intmax_t pid_value;

  if (list == 0)
    {
      builtin_usage ();
      return (EX_USAGE);
    }
  CHECK_HELPOPT (list);

  any_succeeded = listing = saw_signal = 0;
  sig = SIGTERM;
  sigspec = "TERM";

  dflags = DSIG_NOCASE | ((posixly_correct == 0) ? DSIG_SIGPREFIX : 0);
  /* Process options. */
  while (list)
    {
      word = list->word->word;

      if (ISOPTION (word, 'l') || ISOPTION (word, 'L'))
	{
	  listing++;
	  list = list->next;
	}
      else if (ISOPTION (word, 's') || ISOPTION (word, 'n'))
	{
	  list = list->next;
	  if (list)
	    {
	      sigspec = list->word->word;
use_sigspec:
	      if (sigspec[0] == '0' && sigspec[1] == '\0')
		sig = 0;
	      else
		sig = decode_signal (sigspec, dflags);
	      list = list->next;
	      saw_signal++;
	    }
	  else
	    {
	      sh_needarg (word);
	      return (EXECUTION_FAILURE);
	    }
	}
      else if (word[0] == '-' && word[1] == 's' && ISALPHA (word[2]))
	{
	  sigspec = word + 2;
	  goto use_sigspec;
	}
      else if (word[0] == '-' && word[1] == 'n' && ISDIGIT (word[2]))
	{
	  sigspec = word + 2;
	  goto use_sigspec;
	}
      else if (ISOPTION (word, '-'))
	{
	  list = list->next;
	  break;
	}
      else if (ISOPTION (word, '?'))
	{
	  builtin_usage ();
	  return (EX_USAGE);
	}
      /* If this is a signal specification then process it.  We only process
	 the first one seen; other arguments may signify process groups (e.g,
	 -num == process group num). */
      else if (*word == '-' && saw_signal == 0)
	{
	  sigspec = word + 1;
	  sig = decode_signal (sigspec, dflags);
	  saw_signal++;
	  list = list->next;
	}
      else
	break;
    }

  if (listing)
    return (display_signal_list (list, 0));

  /* OK, we are killing processes. */
  if (sig == NO_SIG)
    {
      sh_invalidsig (sigspec);
      return (EXECUTION_FAILURE);
    }

  if (list == 0)
    {
      builtin_usage ();
      return (EX_USAGE);
    }

  while (list)
    {
      word = list->word->word;

      if (*word == '-')
	word++;

      /* Use the entire argument in case of minus sign presence. */
      if (*word && legal_number (list->word->word, &pid_value) && (pid_value == (pid_t)pid_value))
	{
	  pid = (pid_t) pid_value;

	  if (kill_pid (pid, sig, pid < -1) < 0)
	    {
	      if (errno == EINVAL)
		sh_invalidsig (sigspec);
	      else
		kill_error (pid, errno);
	      CONTINUE_OR_FAIL;
	    }
	  else
	    any_succeeded++;
	}
#if defined (JOB_CONTROL)
      else if (*list->word->word && *list->word->word != '%')
	{
	  builtin_error (_("%s: arguments must be process or job IDs"), list->word->word);
	  CONTINUE_OR_FAIL;
	}
      else if (*word)
	/* Posix.2 says you can kill without job control active (4.32.4) */
	{			/* Must be a job spec.  Check it out. */
	  int job;
	  sigset_t set, oset;
	  JOB *j;

	  BLOCK_CHILD (set, oset);
	  job = get_job_spec (list);

	  if (INVALID_JOB (job))
	    {
	      if (job != DUP_JOB)
		sh_badjob (list->word->word);
	      UNBLOCK_CHILD (oset);
	      CONTINUE_OR_FAIL;
	    }

	  j = get_job_by_jid (job);
	  /* Job spec used.  Kill the process group. If the job was started
	     without job control, then its pgrp == shell_pgrp, so we have
	     to be careful.  We take the pid of the first job in the pipeline
	     in that case. */
	  pid = IS_JOBCONTROL (job) ? j->pgrp : j->pipe->pid;

	  UNBLOCK_CHILD (oset);

	  if (kill_pid (pid, sig, 1) < 0)
	    {
	      if (errno == EINVAL)
		sh_invalidsig (sigspec);
	      else
		kill_error (pid, errno);
	      CONTINUE_OR_FAIL;
	    }
	  else
	    any_succeeded++;
	}
#endif /* !JOB_CONTROL */
      else
	{
	  sh_badpid (list->word->word);
	  CONTINUE_OR_FAIL;
	}
    continue_killing:
      list = list->next;
    }

  return (any_succeeded ? EXECUTION_SUCCESS : EXECUTION_FAILURE);
}

static void
kill_error (pid, e)
     pid_t pid;
     int e;
{
  char *x;

  x = strerror (e);
  if (x == 0)
    x = _("Unknown error");
  builtin_error ("(%ld) - %s", (long)pid, x);
}
