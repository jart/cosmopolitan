/* eval.c -- reading and evaluating commands. */

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

#include "bashansi.h"
#include <stdio.h>

#include <signal.h>

#include "bashintl.h"

#include "shell.h"
#include "parser.h"
#include "flags.h"
#include "trap.h"

#include "common.h"

#include "input.h"
#include "execute_cmd.h"

#if defined (HISTORY)
#  include "bashhist.h"
#endif

static void send_pwd_to_eterm PARAMS((void));
static sighandler alrm_catcher PARAMS((int));

/* Read and execute commands until EOF is reached.  This assumes that
   the input source has already been initialized. */
int
reader_loop ()
{
  int our_indirection_level;
  COMMAND * volatile current_command;

  USE_VAR(current_command);

  current_command = (COMMAND *)NULL;

  our_indirection_level = ++indirection_level;

  if (just_one_command)
    reset_readahead_token ();

  while (EOF_Reached == 0)
    {
      int code;

      code = setjmp_nosigs (top_level);

#if defined (PROCESS_SUBSTITUTION)
      unlink_fifo_list ();
#endif /* PROCESS_SUBSTITUTION */

      /* XXX - why do we set this every time through the loop?  And why do
	 it if SIGINT is trapped in an interactive shell? */
      if (interactive_shell && signal_is_ignored (SIGINT) == 0 && signal_is_trapped (SIGINT) == 0)
	set_signal_handler (SIGINT, sigint_sighandler);

      if (code != NOT_JUMPED)
	{
	  indirection_level = our_indirection_level;

	  switch (code)
	    {
	      /* Some kind of throw to top_level has occurred. */
	    case ERREXIT:
	      if (exit_immediately_on_error)
		reset_local_contexts ();	/* not in a function */
	    case FORCE_EOF:
	    case EXITPROG:
	    case EXITBLTIN:
	      current_command = (COMMAND *)NULL;
	      EOF_Reached = EOF;
	      goto exec_done;

	    case DISCARD:
	      /* Make sure the exit status is reset to a non-zero value, but
		 leave existing non-zero values (e.g., > 128 on signal)
		 alone. */
	      if (last_command_exit_value == 0)
		set_exit_status (EXECUTION_FAILURE);
	      if (subshell_environment)
		{
		  current_command = (COMMAND *)NULL;
		  EOF_Reached = EOF;
		  goto exec_done;
		}
	      /* Obstack free command elements, etc. */
	      if (current_command)
		{
		  dispose_command (current_command);
		  current_command = (COMMAND *)NULL;
		}

	      restore_sigmask ();
	      break;

	    default:
	      command_error ("reader_loop", CMDERR_BADJUMP, code, 0);
	    }
	}

      executing = 0;
      if (temporary_env)
	dispose_used_env_vars ();

#if (defined (ultrix) && defined (mips)) || defined (C_ALLOCA)
      /* Attempt to reclaim memory allocated with alloca (). */
      (void) alloca (0);
#endif

      if (read_command () == 0)
	{
	  if (interactive_shell == 0 && read_but_dont_execute)
	    {
	      set_exit_status (last_command_exit_value);
	      dispose_command (global_command);
	      global_command = (COMMAND *)NULL;
	    }
	  else if (current_command = global_command)
	    {
	      global_command = (COMMAND *)NULL;

	      /* If the shell is interactive, expand and display $PS0 after reading a
		 command (possibly a list or pipeline) and before executing it. */
	      if (interactive && ps0_prompt)
		{
		  char *ps0_string;

		  ps0_string = decode_prompt_string (ps0_prompt);
		  if (ps0_string && *ps0_string)
		    {
		      fprintf (stderr, "%s", ps0_string);
		      fflush (stderr);
		    }
		  free (ps0_string);
		}

	      current_command_number++;

	      executing = 1;
	      stdin_redir = 0;

	      execute_command (current_command);

	    exec_done:
	      QUIT;

	      if (current_command)
		{
		  dispose_command (current_command);
		  current_command = (COMMAND *)NULL;
		}
	    }
	}
      else
	{
	  /* Parse error, maybe discard rest of stream if not interactive. */
	  if (interactive == 0)
	    EOF_Reached = EOF;
	}
      if (just_one_command)
	EOF_Reached = EOF;
    }
  indirection_level--;
  return (last_command_exit_value);
}

/* Pretty print shell scripts */
int
pretty_print_loop ()
{
  COMMAND *current_command;
  char *command_to_print;
  int code;
  int global_posix_mode, last_was_newline;

  global_posix_mode = posixly_correct;
  last_was_newline = 0;
  while (EOF_Reached == 0)
    {
      code = setjmp_nosigs (top_level);
      if (code)
        return (EXECUTION_FAILURE);
      if (read_command() == 0)
	{
	  current_command = global_command;
	  global_command = 0;
	  posixly_correct = 1;			/* print posix-conformant */
	  if (current_command && (command_to_print = make_command_string (current_command)))
	    {
	      printf ("%s\n", command_to_print);	/* for now */
	      last_was_newline = 0;
	    }
	  else if (last_was_newline == 0)
	    {
	       printf ("\n");
	       last_was_newline = 1;
	    }
	  posixly_correct = global_posix_mode;
	  dispose_command (current_command);
	}
      else
	return (EXECUTION_FAILURE);
    }
    
  return (EXECUTION_SUCCESS);
}

static sighandler
alrm_catcher(i)
     int i;
{
  char *msg;

  msg = _("\007timed out waiting for input: auto-logout\n");
  write (1, msg, strlen (msg));

  bash_logout ();	/* run ~/.bash_logout if this is a login shell */
  jump_to_top_level (EXITPROG);
  SIGRETURN (0);
}

/* Send an escape sequence to emacs term mode to tell it the
   current working directory. */
static void
send_pwd_to_eterm ()
{
  char *pwd, *f;

  f = 0;
  pwd = get_string_value ("PWD");
  if (pwd == 0)
    f = pwd = get_working_directory ("eterm");
  fprintf (stderr, "\032/%s\n", pwd);
  free (f);
}

#if defined (ARRAY_VARS)
/* Caller ensures that A has a non-zero number of elements */
int
execute_array_command (a, v)
     ARRAY *a;
     void *v;
{
  char *tag;
  char **argv;
  int argc, i;

  tag = (char *)v;
  argc = 0;
  argv = array_to_argv (a, &argc);
  for (i = 0; i < argc; i++)
    {
      if (argv[i] && argv[i][0])
	execute_variable_command (argv[i], tag);
    }
  strvec_dispose (argv);
  return 0;
}
#endif
  
static void
execute_prompt_command ()
{
  char *command_to_execute;
  SHELL_VAR *pcv;
#if defined (ARRAY_VARS)
  ARRAY *pcmds;
#endif

  pcv = find_variable ("PROMPT_COMMAND");
  if (pcv  == 0 || var_isset (pcv) == 0 || invisible_p (pcv))
    return;
#if defined (ARRAY_VARS)
  if (array_p (pcv))
    {
      if ((pcmds = array_cell (pcv)) && array_num_elements (pcmds) > 0)
	execute_array_command (pcmds, "PROMPT_COMMAND");
      return;
    }
  else if (assoc_p (pcv))
    return;	/* currently don't allow associative arrays here */
#endif

  command_to_execute = value_cell (pcv);
  if (command_to_execute && *command_to_execute)
    execute_variable_command (command_to_execute, "PROMPT_COMMAND");
}

/* Call the YACC-generated parser and return the status of the parse.
   Input is read from the current input stream (bash_input).  yyparse
   leaves the parsed command in the global variable GLOBAL_COMMAND.
   This is where PROMPT_COMMAND is executed. */
int
parse_command ()
{
  int r;

  need_here_doc = 0;
  run_pending_traps ();

  /* Allow the execution of a random command just before the printing
     of each primary prompt.  If the shell variable PROMPT_COMMAND
     is set then its value (array or string) is the command(s) to execute. */
  /* The tests are a combination of SHOULD_PROMPT() and prompt_again() 
     from parse.y, which are the conditions under which the prompt is
     actually printed. */
  if (interactive && bash_input.type != st_string && parser_expanding_alias() == 0)
    {
#if defined (READLINE)
      if (no_line_editing || (bash_input.type == st_stdin && parser_will_prompt ()))
#endif
        execute_prompt_command ();

      if (running_under_emacs == 2)
	send_pwd_to_eterm ();	/* Yuck */
    }

  current_command_line_count = 0;
  r = yyparse ();

  if (need_here_doc)
    gather_here_documents ();

  return (r);
}

/* Read and parse a command, returning the status of the parse.  The command
   is left in the globval variable GLOBAL_COMMAND for use by reader_loop.
   This is where the shell timeout code is executed. */
int
read_command ()
{
  SHELL_VAR *tmout_var;
  int tmout_len, result;
  SigHandler *old_alrm;

  set_current_prompt_level (1);
  global_command = (COMMAND *)NULL;

  /* Only do timeouts if interactive. */
  tmout_var = (SHELL_VAR *)NULL;
  tmout_len = 0;
  old_alrm = (SigHandler *)NULL;

  if (interactive)
    {
      tmout_var = find_variable ("TMOUT");

      if (tmout_var && var_isset (tmout_var))
	{
	  tmout_len = atoi (value_cell (tmout_var));
	  if (tmout_len > 0)
	    {
	      old_alrm = set_signal_handler (SIGALRM, alrm_catcher);
	      alarm (tmout_len);
	    }
	}
    }

  QUIT;

  current_command_line_count = 0;
  result = parse_command ();

  if (interactive && tmout_var && (tmout_len > 0))
    {
      alarm(0);
      set_signal_handler (SIGALRM, old_alrm);
    }

  return (result);
}
