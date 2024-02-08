/* suspend.c, created from suspend.def. */
#line 22 "./suspend.def"

#line 40 "./suspend.def"

#include "config.h"

#if defined (JOB_CONTROL)
#if defined (HAVE_UNISTD_H)
#  ifdef _MINIX
#    include <sys/types.h>
#  endif
#  include <unistd.h>
#endif

#include "bashtypes.h"
#include <signal.h>
#include "bashintl.h"
#include "shell.h"
#include "jobs.h"
#include "common.h"
#include "bashgetopt.h"

static sighandler suspend_continue PARAMS((int));

static SigHandler *old_cont;
#if 0
static SigHandler *old_stop;
#endif

/* Continue handler. */
static sighandler
suspend_continue (sig)
     int sig;
{
  set_signal_handler (SIGCONT, old_cont);
#if 0
  set_signal_handler (SIGSTOP, old_stop);
#endif
  SIGRETURN (0);
}

/* Suspending the shell.  If -f is the arg, then do the suspend
   no matter what.  Otherwise, complain if a login shell. */
int
suspend_builtin (list)
     WORD_LIST *list;
{
  int opt, force;

  reset_internal_getopt ();
  force = 0;
  while ((opt = internal_getopt (list, "f")) != -1)
    switch (opt)
      {
      case 'f':
	force++;
	break;
      CASE_HELPOPT;
      default:
	builtin_usage ();
	return (EX_USAGE);
      }
      
  list = loptend;
  no_args (list);

  if (force == 0)  
    {
      if (job_control == 0)
	{
	  sh_nojobs (_("cannot suspend"));
	  return (EXECUTION_FAILURE);
	}

      if (login_shell)
	{
	  builtin_error (_("cannot suspend a login shell"));
	  return (EXECUTION_FAILURE);
	}
    }

  /* XXX - should we put ourselves back into the original pgrp now?  If so,
     call end_job_control() here and do the right thing in suspend_continue
     (that is, call restart_job_control()). */
  old_cont = (SigHandler *)set_signal_handler (SIGCONT, suspend_continue);
#if 0
  old_stop = (SigHandler *)set_signal_handler (SIGSTOP, SIG_DFL);
#endif
  killpg (shell_pgrp, SIGSTOP);
  return (EXECUTION_SUCCESS);
}

#endif /* JOB_CONTROL */
