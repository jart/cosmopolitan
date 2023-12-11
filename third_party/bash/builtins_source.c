/* source.c, created from source.def. */
#line 22 "./source.def"

#line 37 "./source.def"

#line 53 "./source.def"

#include "config.h"

#include "bashtypes.h"
#include "posixstat.h"
#include "filecntl.h"
#if ! defined(_MINIX) && defined (HAVE_SYS_FILE_H)
#  include <sys/file.h>
#endif
#include <errno.h>

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#include "bashansi.h"
#include "bashintl.h"

#include "shell.h"
#include "execute_cmd.h"
#include "flags.h"
#include "findcmd.h"
#include "common.h"
#include "bashgetopt.h"
#include "trap.h"

#if !defined (errno)
extern int errno;
#endif /* !errno */

static void maybe_pop_dollar_vars PARAMS((void));

/* If non-zero, `.' uses $PATH to look up the script to be sourced. */
int source_uses_path = 1;

/* If non-zero, `.' looks in the current directory if the filename argument
   is not found in the $PATH. */
int source_searches_cwd = 1;

/* If this . script is supplied arguments, we save the dollar vars and
   replace them with the script arguments for the duration of the script's
   execution.  If the script does not change the dollar vars, we restore
   what we saved.  If the dollar vars are changed in the script, and we are
   not executing a shell function, we leave the new values alone and free
   the saved values. */
static void
maybe_pop_dollar_vars ()
{
  if (variable_context == 0 && (dollar_vars_changed () & ARGS_SETBLTIN))
    dispose_saved_dollar_vars ();
  else
    pop_dollar_vars ();
  if (debugging_mode)
    pop_args ();	/* restore BASH_ARGC and BASH_ARGV */
  set_dollar_vars_unchanged ();
  invalidate_cached_quoted_dollar_at ();	/* just invalidate to be safe */
}

/* Read and execute commands from the file passed as argument.  Guess what.
   This cannot be done in a subshell, since things like variable assignments
   take place in there.  So, I open the file, place it into a large string,
   close the file, and then execute the string. */
int
source_builtin (list)
     WORD_LIST *list;
{
  int result;
  char *filename, *debug_trap, *x;

  if (no_options (list))
    return (EX_USAGE);
  list = loptend;

  if (list == 0)
    {
      builtin_error (_("filename argument required"));
      builtin_usage ();
      return (EX_USAGE);
    }

#if defined (RESTRICTED_SHELL)
  if (restricted && strchr (list->word->word, '/'))
    {
      sh_restricted (list->word->word);
      return (EXECUTION_FAILURE);
    }
#endif

  filename = (char *)NULL;
  /* XXX -- should this be absolute_pathname? */
  if (posixly_correct && strchr (list->word->word, '/'))
    filename = savestring (list->word->word);
  else if (absolute_pathname (list->word->word))
    filename = savestring (list->word->word);
  else if (source_uses_path)
    filename = find_path_file (list->word->word);
  if (filename == 0)
    {
      if (source_searches_cwd == 0)
	{
	  x = printable_filename (list->word->word, 0);
	  builtin_error (_("%s: file not found"), x);
	  if (x != list->word->word)
	    free (x);
	  if (posixly_correct && interactive_shell == 0 && executing_command_builtin == 0)
	    {
	      last_command_exit_value = EXECUTION_FAILURE;
	      jump_to_top_level (EXITPROG);
	    }
	  return (EXECUTION_FAILURE);
	}
      else
	filename = savestring (list->word->word);
    }

  begin_unwind_frame ("source");
  add_unwind_protect (xfree, filename);

  if (list->next)
    {
      push_dollar_vars ();
      add_unwind_protect ((Function *)maybe_pop_dollar_vars, (char *)NULL);
      if (debugging_mode || shell_compatibility_level <= 44)
	init_bash_argv ();	/* Initialize BASH_ARGV and BASH_ARGC */
      remember_args (list->next, 1);
      if (debugging_mode)
	push_args (list->next);	/* Update BASH_ARGV and BASH_ARGC */
    }
  set_dollar_vars_unchanged ();

  /* Don't inherit the DEBUG trap unless function_trace_mode (overloaded)
     is set.  XXX - should sourced files inherit the RETURN trap?  Functions
     don't. */
  debug_trap = TRAP_STRING (DEBUG_TRAP);
  if (debug_trap && function_trace_mode == 0)
    {
      debug_trap = savestring (debug_trap);
      add_unwind_protect (xfree, debug_trap);
      add_unwind_protect (maybe_set_debug_trap, debug_trap);
      restore_default_signal (DEBUG_TRAP);
    }

  result = source_file (filename, (list && list->next));

  run_unwind_frame ("source");

  return (result);
}
