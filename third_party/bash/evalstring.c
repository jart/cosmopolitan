/* evalstring.c - evaluate a string as one or more shell commands. */

/* Copyright (C) 1996-2022 Free Software Foundation, Inc.

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

#if defined (HAVE_UNISTD_H)
#  ifdef _MINIX
#    include <sys/types.h>
#  endif
#  include <unistd.h>
#endif

#include <stdio.h>
#include <signal.h>

#include <errno.h>

#include "filecntl.h"
#include "bashansi.h"

#include "shell.h"
#include "jobs.h"
#include "builtins.h"
#include "flags.h"
#include "parser.h"
#include "input.h"
#include "execute_cmd.h"
#include "redir.h"
#include "trap.h"
#include "bashintl.h"

#include "y.tab.h"

#if defined (HISTORY)
#  include "bashhist.h"
#endif

#include "common.h"
#include "builtext.h"

#if !defined (errno)
extern int errno;
#endif

#define IS_BUILTIN(s)	(builtin_address_internal(s, 0) != (struct builtin *)NULL)

int parse_and_execute_level = 0;

static int cat_file PARAMS((REDIRECT *));

#define PE_TAG "parse_and_execute top"
#define PS_TAG "parse_string top"

#if defined (HISTORY)
static void
set_history_remembering ()
{
  remember_on_history = enable_history_list;
}
#endif

static void
restore_lastcom (x)
     char *x;
{
  FREE (the_printed_command_except_trap);
  the_printed_command_except_trap = x;
}

int
should_optimize_fork (command, subshell)
     COMMAND *command;
     int subshell;
{
  return (running_trap == 0 &&
      command->type == cm_simple &&
      signal_is_trapped (EXIT_TRAP) == 0 &&
      signal_is_trapped (ERROR_TRAP) == 0 &&
      any_signals_trapped () < 0 &&
      (subshell || (command->redirects == 0 && command->value.Simple->redirects == 0)) &&
      ((command->flags & CMD_TIME_PIPELINE) == 0) &&
      ((command->flags & CMD_INVERT_RETURN) == 0));
}

/* This has extra tests to account for STARTUP_STATE == 2, which is for
   -c command but has been extended to command and process substitution
   (basically any time you call parse_and_execute in a subshell). */
int
should_suppress_fork (command)
     COMMAND *command;
{
  int subshell;

  subshell = subshell_environment & SUBSHELL_PROCSUB;	/* salt to taste */
  return (startup_state == 2 && parse_and_execute_level == 1 &&
	  *bash_input.location.string == '\0' &&
	  parser_expanding_alias () == 0 &&
	  should_optimize_fork (command, subshell));
}

int
can_optimize_connection (command)
     COMMAND *command;
{
  return (*bash_input.location.string == '\0' &&
	  parser_expanding_alias () == 0 &&
	  (command->value.Connection->connector == AND_AND || command->value.Connection->connector == OR_OR || command->value.Connection->connector == ';') &&
	  command->value.Connection->second->type == cm_simple);
}

void
optimize_connection_fork (command)
     COMMAND *command;
{
  if (command->type == cm_connection &&
      (command->value.Connection->connector == AND_AND || command->value.Connection->connector == OR_OR || command->value.Connection->connector == ';') &&
      (command->value.Connection->second->flags & CMD_TRY_OPTIMIZING) &&
      ((startup_state == 2 && should_suppress_fork (command->value.Connection->second)) ||
       ((subshell_environment & SUBSHELL_PAREN) && should_optimize_fork (command->value.Connection->second, 0))))
    {
      command->value.Connection->second->flags |= CMD_NO_FORK;
      command->value.Connection->second->value.Simple->flags |= CMD_NO_FORK;
    }
}

void
optimize_subshell_command (command)
     COMMAND *command;
{
  if (should_optimize_fork (command, 0))
    {
      command->flags |= CMD_NO_FORK;
      command->value.Simple->flags |= CMD_NO_FORK;
    }
  else if (command->type == cm_connection &&
	   (command->value.Connection->connector == AND_AND || command->value.Connection->connector == OR_OR || command->value.Connection->connector == ';') &&
	   command->value.Connection->second->type == cm_simple &&
	   parser_expanding_alias () == 0)
    {	   
      command->value.Connection->second->flags |= CMD_TRY_OPTIMIZING;
      command->value.Connection->second->value.Simple->flags |= CMD_TRY_OPTIMIZING;
    }
}

void
optimize_shell_function (command)
     COMMAND *command;
{
  COMMAND *fc;

  fc = (command->type == cm_group) ? command->value.Group->command : command;

  if (fc->type == cm_simple && should_suppress_fork (fc))
    {
      fc->flags |= CMD_NO_FORK;
      fc->value.Simple->flags |= CMD_NO_FORK;
    }
  else if (fc->type == cm_connection && can_optimize_connection (fc) && should_suppress_fork (fc->value.Connection->second))
    {
      fc->value.Connection->second->flags |= CMD_NO_FORK;
      fc->value.Connection->second->value.Simple->flags |= CMD_NO_FORK;
    }  
}

int
can_optimize_cat_file (command)
     COMMAND *command;
{
  return (command->type == cm_simple && !command->redirects &&
	    (command->flags & CMD_TIME_PIPELINE) == 0 &&
	    command->value.Simple->words == 0 &&
	    command->value.Simple->redirects &&
	    command->value.Simple->redirects->next == 0 &&
	    command->value.Simple->redirects->instruction == r_input_direction &&
	    command->value.Simple->redirects->redirector.dest == 0);
}

/* How to force parse_and_execute () to clean up after itself. */
void
parse_and_execute_cleanup (old_running_trap)
     int old_running_trap;
{
  if (running_trap > 0)
    {
      /* We assume if we have a different value for running_trap than when
	 we started (the only caller that cares is evalstring()), the
	 original caller will perform the cleanup, and we should not step
	 on them. */
      if (running_trap != old_running_trap)
	run_trap_cleanup (running_trap - 1);
      unfreeze_jobs_list ();
    }

  if (have_unwind_protects ())
     run_unwind_frame (PE_TAG);
  else
    parse_and_execute_level = 0;			/* XXX */
}

static void
parse_prologue (string, flags, tag)
     char *string;
     int flags;
     char *tag;
{
  char *orig_string, *lastcom;
  int x;

  orig_string = string;
  /* Unwind protect this invocation of parse_and_execute (). */
  begin_unwind_frame (tag);
  unwind_protect_int (parse_and_execute_level);
  unwind_protect_jmp_buf (top_level);
  unwind_protect_int (indirection_level);
  unwind_protect_int (line_number);
  unwind_protect_int (line_number_for_err_trap);
  unwind_protect_int (loop_level);
  unwind_protect_int (executing_list);
  unwind_protect_int (comsub_ignore_return);
  if (flags & (SEVAL_NONINT|SEVAL_INTERACT))
    unwind_protect_int (interactive);

#if defined (HISTORY)
  if (parse_and_execute_level == 0)
    add_unwind_protect (set_history_remembering, (char *)NULL);
  else
    unwind_protect_int (remember_on_history);	/* can be used in scripts */
#  if defined (BANG_HISTORY)
  unwind_protect_int (history_expansion_inhibited);
#  endif /* BANG_HISTORY */
#endif /* HISTORY */

  if (interactive_shell)
    {
      x = get_current_prompt_level ();
      add_unwind_protect (set_current_prompt_level, x);
    }

  if (the_printed_command_except_trap)
    {
      lastcom = savestring (the_printed_command_except_trap);
      add_unwind_protect (restore_lastcom, lastcom);
    }

  add_unwind_protect (pop_stream, (char *)NULL);
  if (parser_expanding_alias ())
    add_unwind_protect (parser_restore_alias, (char *)NULL);

  if (orig_string && ((flags & SEVAL_NOFREE) == 0))
    add_unwind_protect (xfree, orig_string);
  end_unwind_frame ();

  if (flags & (SEVAL_NONINT|SEVAL_INTERACT))
    interactive = (flags & SEVAL_NONINT) ? 0 : 1;

#if defined (HISTORY)
  if (flags & SEVAL_NOHIST)
    bash_history_disable ();
#  if defined (BANG_HISTORY)
  if (flags & SEVAL_NOHISTEXP)
    history_expansion_inhibited = 1;
#  endif /* BANG_HISTORY */
#endif /* HISTORY */
}

/* Parse and execute the commands in STRING.  Returns whatever
   execute_command () returns.  This frees STRING.  FLAGS is a
   flags word; look in common.h for the possible values.  Actions
   are:
   	(flags & SEVAL_NONINT) -> interactive = 0;
   	(flags & SEVAL_INTERACT) -> interactive = 1;
   	(flags & SEVAL_NOHIST) -> call bash_history_disable ()
   	(flags & SEVAL_NOFREE) -> don't free STRING when finished
   	(flags & SEVAL_RESETLINE) -> reset line_number to 1
   	(flags & SEVAL_NOHISTEXP) -> history_expansion_inhibited -> 1
*/

int
parse_and_execute (string, from_file, flags)
     char *string;
     const char *from_file;
     int flags;
{
  int code, lreset;
  volatile int should_jump_to_top_level, last_result;
  COMMAND *volatile command;
  volatile sigset_t pe_sigmask;

  parse_prologue (string, flags, PE_TAG);

  parse_and_execute_level++;

  lreset = flags & SEVAL_RESETLINE;

#if defined (HAVE_POSIX_SIGNALS)
  /* If we longjmp and are going to go on, use this to restore signal mask */
  sigemptyset ((sigset_t *)&pe_sigmask);
  sigprocmask (SIG_BLOCK, (sigset_t *)NULL, (sigset_t *)&pe_sigmask);
#endif

  /* Reset the line number if the caller wants us to.  If we don't reset the
     line number, we have to subtract one, because we will add one just
     before executing the next command (resetting the line number sets it to
     0; the first line number is 1). */
  push_stream (lreset);
  if (parser_expanding_alias ())
    /* push current shell_input_line */
    parser_save_alias ();
  
  if (lreset == 0)
    line_number--;
    
  indirection_level++;

  code = should_jump_to_top_level = 0;
  last_result = EXECUTION_SUCCESS;

  /* We need to reset enough of the token state so we can start fresh. */
  if (current_token == yacc_EOF)
    current_token = '\n';		/* reset_parser() ? */

  with_input_from_string (string, from_file);
  clear_shell_input_line ();
  while (*(bash_input.location.string) || parser_expanding_alias ())
    {
      command = (COMMAND *)NULL;

      if (interrupt_state)
	{
	  last_result = EXECUTION_FAILURE;
	  break;
	}

      /* Provide a location for functions which `longjmp (top_level)' to
	 jump to.  This prevents errors in substitution from restarting
	 the reader loop directly, for example. */
      code = setjmp_nosigs (top_level);

      if (code)
	{
	  should_jump_to_top_level = 0;
	  switch (code)
	    {
	    case ERREXIT:
	      /* variable_context -> 0 is what eval.c:reader_loop() does in
		 these circumstances.  Don't bother with cleanup here because
		 we don't want to run the function execution cleanup stuff
		 that will cause pop_context and other functions to run.
		 We call reset_local_contexts() instead, which just frees
		 context memory.
		 XXX - change that if we want the function context to be
		 unwound. */
	      if (exit_immediately_on_error && variable_context)
	        {
	          discard_unwind_frame ("pe_dispose");
	          reset_local_contexts (); /* not in a function */
	        }
	      should_jump_to_top_level = 1;
	      goto out;
	    case FORCE_EOF:	      
	    case EXITPROG:
	      if (command)
		run_unwind_frame ("pe_dispose");
	      /* Remember to call longjmp (top_level) after the old
		 value for it is restored. */
	      should_jump_to_top_level = 1;
	      goto out;

	    case EXITBLTIN:
	      if (command)
		{
		  if (variable_context && signal_is_trapped (0))
		    {
		      /* Let's make sure we run the exit trap in the function
			 context, as we do when not running parse_and_execute.
			 The pe_dispose unwind frame comes before any unwind-
			 protects installed by the string we're evaluating, so
			 it will undo the current function scope. */
		      dispose_command (command);
		      discard_unwind_frame ("pe_dispose");
		    }
		  else
		    run_unwind_frame ("pe_dispose");
		}
	      should_jump_to_top_level = 1;
	      goto out;

	    case DISCARD:
	      if (command)
		run_unwind_frame ("pe_dispose");
	      last_result = last_command_exit_value = EXECUTION_FAILURE; /* XXX */
	      set_pipestatus_from_exit (last_command_exit_value);
	      if (subshell_environment)
		{
		  should_jump_to_top_level = 1;
		  goto out;
		}
	      else
		{
#if 0
		  dispose_command (command);	/* pe_dispose does this */
#endif
#if defined (HAVE_POSIX_SIGNALS)
		  sigprocmask (SIG_SETMASK, (sigset_t *)&pe_sigmask, (sigset_t *)NULL);
#endif
		  continue;
		}

	    default:
	      command_error ("parse_and_execute", CMDERR_BADJUMP, code, 0);
	      break;
	    }
	}

      if (parse_command () == 0)
	{
	  if ((flags & SEVAL_PARSEONLY) || (interactive_shell == 0 && read_but_dont_execute))
	    {
	      last_result = EXECUTION_SUCCESS;
	      dispose_command (global_command);
	      global_command = (COMMAND *)NULL;
	    }
	  else if (command = global_command)
	    {
	      struct fd_bitmap *bitmap;

	      if (flags & SEVAL_FUNCDEF)
		{
		  char *x;

		  /* If the command parses to something other than a straight
		     function definition, or if we have not consumed the entire
		     string, or if the parser has transformed the function
		     name (as parsing will if it begins or ends with shell
		     whitespace, for example), reject the attempt */
		  if (command->type != cm_function_def ||
		      ((x = parser_remaining_input ()) && *x) ||
		      (STREQ (from_file, command->value.Function_def->name->word) == 0))
		    {
		      internal_warning (_("%s: ignoring function definition attempt"), from_file);
		      should_jump_to_top_level = 0;
		      last_result = last_command_exit_value = EX_BADUSAGE;
		      set_pipestatus_from_exit (last_command_exit_value);
		      reset_parser ();
		      break;
		    }
		}

	      bitmap = new_fd_bitmap (FD_BITMAP_SIZE);
	      begin_unwind_frame ("pe_dispose");
	      add_unwind_protect (dispose_fd_bitmap, bitmap);
	      add_unwind_protect (dispose_command, command);	/* XXX */

	      global_command = (COMMAND *)NULL;

	      if ((subshell_environment & SUBSHELL_COMSUB) && comsub_ignore_return)
		command->flags |= CMD_IGNORE_RETURN;

#if defined (ONESHOT)
	      /*
	       * IF
	       *   we were invoked as `bash -c' (startup_state == 2) AND
	       *   parse_and_execute has not been called recursively AND
	       *   we're not running a trap AND
	       *   we have parsed the full command (string == '\0') AND
	       *   we're not going to run the exit trap AND
	       *   we have a simple command without redirections AND
	       *   the command is not being timed AND
	       *   the command's return status is not being inverted AND
	       *   there aren't any traps in effect
	       * THEN
	       *   tell the execution code that we don't need to fork
	       */
	      if (should_suppress_fork (command))
		{
		  command->flags |= CMD_NO_FORK;
		  command->value.Simple->flags |= CMD_NO_FORK;
		}

	      /* Can't optimize forks out here execept for simple commands.
		 This knows that the parser sets up commands as left-side heavy
		 (&& and || are left-associative) and after the single parse,
		 if we are at the end of the command string, the last in a
		 series of connection commands is
		 command->value.Connection->second. */
	      else if (command->type == cm_connection && can_optimize_connection (command))
		{
		  command->value.Connection->second->flags |= CMD_TRY_OPTIMIZING;
		  command->value.Connection->second->value.Simple->flags |= CMD_TRY_OPTIMIZING;
		}
#endif /* ONESHOT */

	      /* See if this is a candidate for $( <file ). */
	      if (startup_state == 2 &&
		  (subshell_environment & SUBSHELL_COMSUB) &&
		  *bash_input.location.string == '\0' &&
		  can_optimize_cat_file (command))
		{
		  int r;
		  r = cat_file (command->value.Simple->redirects);
		  last_result = (r < 0) ? EXECUTION_FAILURE : EXECUTION_SUCCESS;
		}
	      else
		last_result = execute_command_internal
				(command, 0, NO_PIPE, NO_PIPE, bitmap);
	      dispose_command (command);
	      dispose_fd_bitmap (bitmap);
	      discard_unwind_frame ("pe_dispose");

	      if (flags & SEVAL_ONECMD)
		{
		  reset_parser ();
		  break;
		}
	    }
	}
      else
	{
	  last_result = EX_BADUSAGE;	/* was EXECUTION_FAILURE */

	  if (interactive_shell == 0 && this_shell_builtin &&
	      (this_shell_builtin == source_builtin || this_shell_builtin == eval_builtin) &&
	      last_command_exit_value == EX_BADSYNTAX && posixly_correct && executing_command_builtin == 0)
	    {
	      should_jump_to_top_level = 1;
	      code = ERREXIT;
	      last_command_exit_value = EX_BADUSAGE;
	    }

	  /* Since we are shell compatible, syntax errors in a script
	     abort the execution of the script.  Right? */
	  break;
	}
    }

 out:

  run_unwind_frame (PE_TAG);

  if (interrupt_state && parse_and_execute_level == 0)
    {
      /* An interrupt during non-interactive execution in an
	 interactive shell (e.g. via $PROMPT_COMMAND) should
	 not cause the shell to exit. */
      interactive = interactive_shell;
      throw_to_top_level ();
    }

  CHECK_TERMSIG;

  if (should_jump_to_top_level)
    jump_to_top_level (code);

  return (last_result);
}

/* Parse a command contained in STRING according to FLAGS and return the
   number of characters consumed from the string.  If non-NULL, set *ENDP
   to the position in the string where the parse ended.  Used to validate
   command substitutions during parsing to obey Posix rules about finding
   the end of the command and balancing parens. */
int
parse_string (string, from_file, flags, cmdp, endp)
     char *string;
     const char *from_file;
     int flags;
     COMMAND **cmdp;
     char **endp;
{
  int code, nc;
  volatile int should_jump_to_top_level;
  COMMAND *volatile command, *oglobal;
  char *ostring;
  volatile sigset_t ps_sigmask;

  parse_prologue (string, flags, PS_TAG);

#if defined (HAVE_POSIX_SIGNALS)
  /* If we longjmp and are going to go on, use this to restore signal mask */
  sigemptyset ((sigset_t *)&ps_sigmask);
  sigprocmask (SIG_BLOCK, (sigset_t *)NULL, (sigset_t *)&ps_sigmask);
#endif

  /* Reset the line number if the caller wants us to.  If we don't reset the
     line number, we have to subtract one, because we will add one just
     before executing the next command (resetting the line number sets it to
     0; the first line number is 1). */
  push_stream (0);
  if (parser_expanding_alias ())
    /* push current shell_input_line */
    parser_save_alias ();

  code = should_jump_to_top_level = 0;
  oglobal = global_command;

  with_input_from_string (string, from_file);
  ostring = bash_input.location.string;
  while (*(bash_input.location.string))		/* XXX - parser_expanding_alias () ? */
    {
      command = (COMMAND *)NULL;

#if 0
      if (interrupt_state)
	break;
#endif

      /* Provide a location for functions which `longjmp (top_level)' to
	 jump to. */
      code = setjmp_nosigs (top_level);

      if (code)
	{
	  INTERNAL_DEBUG(("parse_string: longjmp executed: code = %d", code));

	  should_jump_to_top_level = 0;
	  switch (code)
	    {
	    case FORCE_EOF:
	    case ERREXIT:
	    case EXITPROG:
	    case EXITBLTIN:
	    case DISCARD:		/* XXX */
	      if (command)
		dispose_command (command);
	      /* Remember to call longjmp (top_level) after the old
		 value for it is restored. */
	      should_jump_to_top_level = 1;
	      goto out;

	    default:
#if defined (HAVE_POSIX_SIGNALS)
	      sigprocmask (SIG_SETMASK, (sigset_t *)&ps_sigmask, (sigset_t *)NULL);
#endif
	      command_error ("parse_string", CMDERR_BADJUMP, code, 0);
	      break;
	    }
	}
	  
      if (parse_command () == 0)
	{
	  if (cmdp)
	    *cmdp = global_command;
	  else
	    dispose_command (global_command);
	  global_command = (COMMAND *)NULL;
	}
      else
	{
	  if ((flags & SEVAL_NOLONGJMP) == 0)
	    {
	      should_jump_to_top_level = 1;
	      code = DISCARD;
	    }
	  else
	    reset_parser ();	/* XXX - sets token_to_read */
	  break;
	}

      if (current_token == yacc_EOF || current_token == shell_eof_token)
	{
	  if (current_token == shell_eof_token)
	    rewind_input_string ();
	  break;
	}
    }

out:

  global_command = oglobal;
  nc = bash_input.location.string - ostring;
  if (endp)
    *endp = bash_input.location.string;

  run_unwind_frame (PS_TAG);

  /* If we return < 0, the caller (xparse_dolparen) will jump_to_top_level for
     us, after doing cleanup */
  if (should_jump_to_top_level)
    {
      if (parse_and_execute_level == 0)
	top_level_cleanup ();
      if (code == DISCARD)
	return -DISCARD;
      jump_to_top_level (code);
    }

  return (nc);
}

int
open_redir_file (r, fnp)
     REDIRECT *r;
     char **fnp;
{
  char *fn;
  int fd, rval;

  if (r->instruction != r_input_direction)
    return -1;

  /* Get the filename. */
  if (posixly_correct && !interactive_shell)
    disallow_filename_globbing++;
  fn = redirection_expand (r->redirectee.filename);
  if (posixly_correct && !interactive_shell)
    disallow_filename_globbing--;

  if (fn == 0)
    {
      redirection_error (r, AMBIGUOUS_REDIRECT, fn);
      return -1;
    }

  fd = open(fn, O_RDONLY);
  if (fd < 0)
    {
      file_error (fn);
      free (fn);
      if (fnp)
	*fnp = 0;
      return -1;
    }

  if (fnp)
    *fnp = fn;
  return fd;
}

/* Handle a $( < file ) command substitution.  This expands the filename,
   returning errors as appropriate, then just cats the file to the standard
   output. */
static int
cat_file (r)
     REDIRECT *r;
{
  char *fn;
  int fd, rval;

  fd = open_redir_file (r, &fn);
  if (fd < 0)
    return -1;

  rval = zcatfd (fd, 1, fn);

  free (fn);
  close (fd);

  return (rval);
}

int
evalstring (string, from_file, flags)
     char *string;
     const char *from_file;
     int flags;
{
  volatile int r, rflag, rcatch;
  volatile int was_trap;

  /* Are we running a trap when we execute this function? */
  was_trap = running_trap;

  rcatch = 0;
  rflag = return_catch_flag;
  /* If we are in a place where `return' is valid, we have to catch
     `eval "... return"' and make sure parse_and_execute cleans up. Then
     we can trampoline to the previous saved return_catch location. */
  if (rflag)
    {
      begin_unwind_frame ("evalstring");

      unwind_protect_int (return_catch_flag);
      unwind_protect_jmp_buf (return_catch);

      return_catch_flag++;	/* increment so we have a counter */
      rcatch = setjmp_nosigs (return_catch);
    }

  if (rcatch)
    {
      /* We care about whether or not we are running the same trap we were
	 when we entered this function. */
      parse_and_execute_cleanup (was_trap);
      r = return_catch_value;
    }
  else
    /* Note that parse_and_execute () frees the string it is passed. */
    r = parse_and_execute (string, from_file, flags);

  if (rflag)
    {
      run_unwind_frame ("evalstring");
      if (rcatch && return_catch_flag)
	{
	  return_catch_value = r;
	  sh_longjmp (return_catch, 1);
	}
    }
    
  return (r);
}
