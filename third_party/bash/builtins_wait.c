/* wait.c, created from wait.def. */
#line 51 "./wait.def"

#line 66 "./wait.def"

#include "config.h"

#include "bashtypes.h"
#include <signal.h>

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#include "chartypes.h"

#include "bashansi.h"

#include "shell.h"
#include "execute_cmd.h"
#include "jobs.h"
#include "trap.h"
#include "sig.h"
#include "common.h"
#include "bashgetopt.h"

extern int wait_signal_received;

procenv_t wait_intr_buf;
int wait_intr_flag;

static int set_waitlist PARAMS((WORD_LIST *));
static void unset_waitlist PARAMS((void));

/* Wait for the pid in LIST to stop or die.  If no arguments are given, then
   wait for all of the active background processes of the shell and return
   0.  If a list of pids or job specs are given, return the exit status of
   the last one waited for. */

#define WAIT_RETURN(s) \
  do \
    { \
      wait_signal_received = 0; \
      wait_intr_flag = 0; \
      return (s);\
    } \
  while (0)

int
wait_builtin (list)
     WORD_LIST *list;
{
  int status, code, opt, nflag, vflags, bindflags;
  volatile int wflags;
  char *vname;
  SHELL_VAR *pidvar;
  struct procstat pstat;

  USE_VAR(list);

  nflag = wflags = vflags = 0;
  vname = NULL;
  pidvar = (SHELL_VAR *)NULL;
  reset_internal_getopt ();
  while ((opt = internal_getopt (list, "fnp:")) != -1)
    {
      switch (opt)
	{
#if defined (JOB_CONTROL)
	case 'n':
	  nflag = 1;
	  break;
	case 'f':
	  wflags |= JWAIT_FORCE;
	  break;
	case 'p':
	  vname = list_optarg;
	  vflags = list_optflags;	  
	  break;	
#endif
	CASE_HELPOPT;
	default:
	  builtin_usage ();
	  return (EX_USAGE);
	}
    }
  list = loptend;

  /* Sanity-check variable name if -p supplied. */
  if (vname)
    {
#if defined (ARRAY_VARS)
      int arrayflags;

      SET_VFLAGS (vflags, arrayflags, bindflags);
      if (legal_identifier (vname) == 0 && valid_array_reference (vname, arrayflags) == 0)
#else
      bindflags = 0;
      if (legal_identifier (vname) == 0)
#endif
	{
	  sh_invalidid (vname);
	  WAIT_RETURN (EXECUTION_FAILURE);
	}
      if (builtin_unbind_variable (vname) == -2)
	WAIT_RETURN (EXECUTION_FAILURE);
    }

  /* POSIX.2 says:  When the shell is waiting (by means of the wait utility)
     for asynchronous commands to complete, the reception of a signal for
     which a trap has been set shall cause the wait utility to return
     immediately with an exit status greater than 128, after which the trap
     associated with the signal shall be taken.

     We handle SIGINT here; it's the only one that needs to be treated
     specially (I think), since it's handled specially in {no,}jobs.c. */
  wait_intr_flag = 1;
  code = setjmp_sigs (wait_intr_buf);

  if (code)
    {
      last_command_exit_signal = wait_signal_received;
      status = 128 + wait_signal_received;
      wait_sigint_cleanup ();
#if defined (JOB_CONTROL)
      if (wflags & JWAIT_WAITING)
	unset_waitlist ();
#endif
      WAIT_RETURN (status);
    }

  opt = first_pending_trap ();
#if defined (SIGCHLD)
  /* We special case SIGCHLD when not in posix mode because we don't break
     out of the wait even when the signal is trapped; we run the trap after
     the wait completes. See how it's handled in jobs.c:waitchld(). */
  if (opt == SIGCHLD && posixly_correct == 0)
    opt = next_pending_trap (opt+1);
#endif
  if (opt != -1)
    {
      last_command_exit_signal = wait_signal_received = opt;
      status = opt + 128;
      WAIT_RETURN (status);
    }

  /* We support jobs or pids.
     wait <pid-or-job> [pid-or-job ...] */

#if defined (JOB_CONTROL)
  if (nflag)
    {
      if (list)
	{
	  opt = set_waitlist (list);
	  if (opt == 0)
	    WAIT_RETURN (127);
	  wflags |= JWAIT_WAITING;
	}

      status = wait_for_any_job (wflags, &pstat);
      if (vname && status >= 0)
	builtin_bind_var_to_int (vname, pstat.pid, bindflags);

      if (status < 0)
	status = 127;
      if (list)
	unset_waitlist ();
      WAIT_RETURN (status);
    }
#endif
      
  /* But wait without any arguments means to wait for all of the shell's
     currently active background processes. */
  if (list == 0)
    {
      opt = wait_for_background_pids (&pstat);
#if 0
      /* Compatibility with NetBSD sh: don't set VNAME since it doesn't
	 correspond to the return status. */
      if (vname && opt)
	builtin_bind_var_to_int (vname, pstat.pid, bindflags);
#endif
      WAIT_RETURN (EXECUTION_SUCCESS);
    }

  status = EXECUTION_SUCCESS;
  while (list)
    {
      pid_t pid;
      char *w;
      intmax_t pid_value;

      w = list->word->word;
      if (DIGIT (*w))
	{
	  if (legal_number (w, &pid_value) && pid_value == (pid_t)pid_value)
	    {
	      pid = (pid_t)pid_value;
	      status = wait_for_single_pid (pid, wflags|JWAIT_PERROR);
	      /* status > 256 means pid error */
	      pstat.pid = (status > 256) ? NO_PID : pid;
	      pstat.status = (status > 256) ? 127 : status;
	      if (status > 256)
		status = 127;
	    }
	  else
	    {
	      sh_badpid (w);
	      pstat.pid = NO_PID;
	      pstat.status = 127;
	      WAIT_RETURN (EXECUTION_FAILURE);
	    }
	}
#if defined (JOB_CONTROL)
      else if (*w && *w == '%')
	/* Must be a job spec.  Check it out. */
	{
	  int job;
	  sigset_t set, oset;

	  BLOCK_CHILD (set, oset);
	  job = get_job_spec (list);

	  if (INVALID_JOB (job))
	    {
	      if (job != DUP_JOB)
		sh_badjob (list->word->word);
	      UNBLOCK_CHILD (oset);
	      status = 127;	/* As per Posix.2, section 4.70.2 */
	      pstat.pid = NO_PID;
	      pstat.status = status;
	      list = list->next;
	      continue;
	    }

	  /* Job spec used.  Wait for the last pid in the pipeline. */
	  UNBLOCK_CHILD (oset);
	  status = wait_for_job (job, wflags, &pstat);
	}
#endif /* JOB_CONTROL */
      else
	{
	  sh_badpid (w);
	  pstat.pid = NO_PID;
	  pstat.status = 127;
	  status = EXECUTION_FAILURE;
	}

      /* Don't waste time with a longjmp. */
      if (wait_signal_received)
	{
	  last_command_exit_signal = wait_signal_received;
	  status = 128 + wait_signal_received;
	  wait_sigint_cleanup ();
	  WAIT_RETURN (status);
	}

      list = list->next;
    }

  if (vname && pstat.pid != NO_PID)
    builtin_bind_var_to_int (vname, pstat.pid, bindflags);

  WAIT_RETURN (status);
}

#if defined (JOB_CONTROL)
/* Take each valid pid or jobspec in LIST and mark the corresponding job as
   J_WAITING, so wait -n knows which jobs to wait for. Return the number of
   jobs we found. */
static int
set_waitlist (list)
     WORD_LIST *list;
{
  sigset_t set, oset;
  int job, r, njob;
  intmax_t pid;
  WORD_LIST *l;

  BLOCK_CHILD (set, oset);
  njob = 0;
  for (l = list; l; l = l->next)
    {
      job = NO_JOB;
      job = (l && legal_number (l->word->word, &pid) && pid == (pid_t) pid)
		? get_job_by_pid ((pid_t) pid, 0, 0)
		: get_job_spec (l);
      if (job == NO_JOB || jobs == 0 || INVALID_JOB (job))
	{
	  sh_badjob (l->word->word);
	  continue;
	}
      /* We don't check yet to see if one of the desired jobs has already
	 terminated, but we could. We wait until wait_for_any_job(). This
	 has the advantage of validating all the arguments. */
      if ((jobs[job]->flags & J_WAITING) == 0)
	{
	  njob++;
	  jobs[job]->flags |= J_WAITING;
	}
    }
  UNBLOCK_CHILD (oset);
  return (njob);
}

/* Clean up after a call to wait -n jobs */
static void
unset_waitlist ()
{
  int i;
  sigset_t set, oset;

  BLOCK_CHILD (set, oset);
  for (i = 0; i < js.j_jobslots; i++)
    if (jobs[i] && (jobs[i]->flags & J_WAITING))
      jobs[i]->flags &= ~J_WAITING;
  UNBLOCK_CHILD (oset);
}
#endif
