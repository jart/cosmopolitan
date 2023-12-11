/* jobs.c, created from jobs.def. */
#line 22 "./jobs.def"

#line 48 "./jobs.def"

#include "config.h"

#if defined (JOB_CONTROL)
#include "bashtypes.h"
#include <signal.h>
#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#include "bashansi.h"
#include "bashintl.h"

#include "shell.h"
#include "jobs.h"
#include "execute_cmd.h"
#include "bashgetopt.h"
#include "common.h"

#define JSTATE_ANY	0x0
#define JSTATE_RUNNING	0x1
#define JSTATE_STOPPED	0x2

static int execute_list_with_replacements PARAMS((WORD_LIST *));

/* The `jobs' command.  Prints outs a list of active jobs.  If the
   argument `-l' is given, then the process id's are printed also.
   If the argument `-p' is given, print the process group leader's
   pid only.  If `-n' is given, only processes that have changed
   status since the last notification are printed.  If -x is given,
   replace all job specs with the pid of the appropriate process
   group leader and execute the command.  The -r and -s options mean
   to print info about running and stopped jobs only, respectively. */
int
jobs_builtin (list)
     WORD_LIST *list;
{
  int form, execute, state, opt, any_failed, job;
  sigset_t set, oset;

  execute = any_failed = 0;
  form = JLIST_STANDARD;
  state = JSTATE_ANY;

  reset_internal_getopt ();
  while ((opt = internal_getopt (list, "lpnxrs")) != -1)
    {
      switch (opt)
	{
	case 'l':
	  form = JLIST_LONG;
	  break;
	case 'p':
	  form = JLIST_PID_ONLY;
	  break;
	case 'n':
	  form = JLIST_CHANGED_ONLY;
	  break;
	case 'x':
	  if (form != JLIST_STANDARD)
	    {
	      builtin_error (_("no other options allowed with `-x'"));
	      return (EXECUTION_FAILURE);
	    }
	  execute++;
	  break;
	case 'r':
	  state = JSTATE_RUNNING;
	  break;
	case 's':
	  state = JSTATE_STOPPED;
	  break;

	CASE_HELPOPT;
	default:
	  builtin_usage ();
	  return (EX_USAGE);
	}
    }

  list = loptend;

  if (execute)
    return (execute_list_with_replacements (list));

  if (!list)
    {
      switch (state)
	{
	case JSTATE_ANY:
	  list_all_jobs (form);
	  break;
	case JSTATE_RUNNING:
	  list_running_jobs (form);
	  break;
	case JSTATE_STOPPED:
	  list_stopped_jobs (form);
	  break;
	}
      return (EXECUTION_SUCCESS);
    }

  while (list)
    {
      BLOCK_CHILD (set, oset);
      job = get_job_spec (list);

      if ((job == NO_JOB) || jobs == 0 || get_job_by_jid (job) == 0)
	{
	  sh_badjob (list->word->word);
	  any_failed++;
	}
      else if (job != DUP_JOB)
	list_one_job ((JOB *)NULL, form, 0, job);

      UNBLOCK_CHILD (oset);
      list = list->next;
    }
  return (any_failed ? EXECUTION_FAILURE : EXECUTION_SUCCESS);
}

static int
execute_list_with_replacements (list)
     WORD_LIST *list;
{
  register WORD_LIST *l;
  int job, result;
  COMMAND *command;
  JOB *j;

  /* First do the replacement of job specifications with pids. */
  for (l = list; l; l = l->next)
    {
      if (l->word->word[0] == '%')	/* we have a winner */
	{
	  job = get_job_spec (l);

	  /* A bad job spec is not really a job spec! Pass it through. */
	  if (INVALID_JOB (job))
	    continue;

	  j = get_job_by_jid (job);
	  free (l->word->word);
	  l->word->word = itos (j->pgrp);
	}
    }

  /* Next make a new simple command and execute it. */
  begin_unwind_frame ("jobs_builtin");

  command = make_bare_simple_command ();
  command->value.Simple->words = copy_word_list (list);
  command->value.Simple->redirects = (REDIRECT *)NULL;
  command->flags |= CMD_INHIBIT_EXPANSION;
  command->value.Simple->flags |= CMD_INHIBIT_EXPANSION;

  add_unwind_protect (dispose_command, command);
  result = execute_command (command);
  dispose_command (command);

  discard_unwind_frame ("jobs_builtin");
  return (result);
}
#endif /* JOB_CONTROL */

#line 231 "./jobs.def"

#if defined (JOB_CONTROL)
int
disown_builtin (list)
     WORD_LIST *list;
{
  int opt, job, retval, nohup_only, running_jobs, all_jobs;
  sigset_t set, oset;
  intmax_t pid_value;

  nohup_only = running_jobs = all_jobs = 0;
  reset_internal_getopt ();
  while ((opt = internal_getopt (list, "ahr")) != -1)
    {
      switch (opt)
	{
	case 'a':
	  all_jobs = 1;
	  break;
	case 'h':
	  nohup_only = 1;
	  break;
	case 'r':
	  running_jobs = 1;
	  break;
	CASE_HELPOPT;
	default:
	  builtin_usage ();
	  return (EX_USAGE);
	}
    }
  list = loptend;
  retval = EXECUTION_SUCCESS;

  /* `disown -a' or `disown -r' */
  if (list == 0 && (all_jobs || running_jobs))
    {
      if (nohup_only)
	nohup_all_jobs (running_jobs);
      else
	delete_all_jobs (running_jobs);
      return (EXECUTION_SUCCESS);
    }

  do
    {
      BLOCK_CHILD (set, oset);
      job = (list && legal_number (list->word->word, &pid_value) && pid_value == (pid_t) pid_value)
		? get_job_by_pid ((pid_t) pid_value, 0, 0)
		: get_job_spec (list);

      if (job == NO_JOB || jobs == 0 || INVALID_JOB (job))
	{
	  sh_badjob (list ? list->word->word : _("current"));
	  retval = EXECUTION_FAILURE;
	}
      else if (nohup_only)
	nohup_job (job);
      else
	delete_job (job, 1);
      UNBLOCK_CHILD (oset);

      if (list)
	list = list->next;
    }
  while (list);

  return (retval);
}
#endif /* JOB_CONTROL */
