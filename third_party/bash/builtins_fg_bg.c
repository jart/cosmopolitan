/* fg_bg.c, created from fg_bg.def. */
#line 22 "./fg_bg.def"

#line 36 "./fg_bg.def"

#include "config.h"

#include "bashtypes.h"
#include <signal.h>

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#include "bashintl.h"

#include "shell.h"
#include "execute_cmd.h"
#include "jobs.h"
#include "common.h"
#include "bashgetopt.h"

#if defined (JOB_CONTROL)
static int fg_bg PARAMS((WORD_LIST *, int));

/* How to bring a job into the foreground. */
int
fg_builtin (list)
     WORD_LIST *list;
{
  int fg_bit;
  register WORD_LIST *t;

  CHECK_HELPOPT (list);

  if (job_control == 0)
    {
      sh_nojobs ((char *)NULL);
      return (EXECUTION_FAILURE);
    }

  if (no_options (list))
    return (EX_USAGE);
  list = loptend;

  /* If the last arg on the line is '&', then start this job in the
     background.  Else, fg the job. */
  for (t = list; t && t->next; t = t->next)
    ;
  fg_bit = (t && t->word->word[0] == '&' && t->word->word[1] == '\0') == 0;

  return (fg_bg (list, fg_bit));
}
#endif /* JOB_CONTROL */

#line 100 "./fg_bg.def"

#if defined (JOB_CONTROL)
/* How to put a job into the background. */
int
bg_builtin (list)
     WORD_LIST *list;
{
  int r;

  CHECK_HELPOPT (list);

  if (job_control == 0)
    {
      sh_nojobs ((char *)NULL);
      return (EXECUTION_FAILURE);
    }

  if (no_options (list))
    return (EX_USAGE);
  list = loptend;

  /* This relies on the fact that fg_bg() takes a WORD_LIST *, but only acts
     on the first member (if any) of that list. */
  r = EXECUTION_SUCCESS;
  do
    {
      if (fg_bg (list, 0) == EXECUTION_FAILURE)
	r = EXECUTION_FAILURE;
      if (list)
	list = list->next;
    }
  while (list);

  return r;
}

/* How to put a job into the foreground/background. */
static int
fg_bg (list, foreground)
     WORD_LIST *list;
     int foreground;
{
  sigset_t set, oset;
  int job, status, old_async_pid;
  JOB *j;

  BLOCK_CHILD (set, oset);
  job = get_job_spec (list);

  if (INVALID_JOB (job))
    {
      if (job != DUP_JOB)
	sh_badjob (list ? list->word->word : _("current"));

      goto failure;
    }

  j = get_job_by_jid (job);
  /* Or if j->pgrp == shell_pgrp. */
  if (IS_JOBCONTROL (job) == 0)
    {
      builtin_error (_("job %d started without job control"), job + 1);
      goto failure;
    }

  if (foreground == 0)
    {
      old_async_pid = last_asynchronous_pid;
      last_asynchronous_pid = j->pgrp;	/* As per Posix.2 5.4.2 */
    }

  status = start_job (job, foreground);

  if (status >= 0)
    {
    /* win: */
      UNBLOCK_CHILD (oset);
      return (foreground ? status : EXECUTION_SUCCESS);
    }
  else
    {
      if (foreground == 0)
	last_asynchronous_pid = old_async_pid;

    failure:
      UNBLOCK_CHILD (oset);
      return (EXECUTION_FAILURE);
    }
}
#endif /* JOB_CONTROL */
