/* evalfile.c - read and evaluate commands from a file or file descriptor */

/* Copyright (C) 1996-2017 Free Software Foundation, Inc.

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
#  include <unistd.h>
#endif

#include "bashtypes.h"
#include "posixstat.h"
#include "filecntl.h"

#include <stdio.h>
#include <signal.h>
#include <errno.h>

#include "bashansi.h"
#include "bashintl.h"

#include "shell.h"
#include "parser.h"
#include "jobs.h"
#include "builtins.h"
#include "flags.h"
#include "input.h"
#include "execute_cmd.h"
#include "trap.h"

#include "y.tab.h"

#if defined (HISTORY)
#  include "bashhist.h"
#endif

#include "typemax.h"

#include "common.h"

#if !defined (errno)
extern int errno;
#endif

/* Flags for _evalfile() */
#define FEVAL_ENOENTOK		0x001
#define FEVAL_BUILTIN		0x002
#define FEVAL_UNWINDPROT	0x004
#define FEVAL_NONINT		0x008
#define FEVAL_LONGJMP		0x010
#define FEVAL_HISTORY		0x020
#define FEVAL_CHECKBINARY	0x040
#define FEVAL_REGFILE		0x080
#define FEVAL_NOPUSHARGS	0x100

/* How many `levels' of sourced files we have. */
int sourcelevel = 0;

static int
_evalfile (filename, flags)
     const char *filename;
     int flags;
{
  volatile int old_interactive;
  procenv_t old_return_catch;
  int return_val, fd, result, pflags, i, nnull;
  ssize_t nr;			/* return value from read(2) */
  char *string;
  struct stat finfo;
  size_t file_size;
  sh_vmsg_func_t *errfunc;
#if defined (ARRAY_VARS)
  SHELL_VAR *funcname_v, *bash_source_v, *bash_lineno_v;
  ARRAY *funcname_a, *bash_source_a, *bash_lineno_a;
  struct func_array_state *fa;
#  if defined (DEBUGGER)
  SHELL_VAR *bash_argv_v, *bash_argc_v;
  ARRAY *bash_argv_a, *bash_argc_a;
#  endif
  char *t, tt[2];
#endif

  USE_VAR(pflags);

#if defined (ARRAY_VARS)
  GET_ARRAY_FROM_VAR ("FUNCNAME", funcname_v, funcname_a);
  GET_ARRAY_FROM_VAR ("BASH_SOURCE", bash_source_v, bash_source_a);
  GET_ARRAY_FROM_VAR ("BASH_LINENO", bash_lineno_v, bash_lineno_a);
#  if defined (DEBUGGER)
  GET_ARRAY_FROM_VAR ("BASH_ARGV", bash_argv_v, bash_argv_a);
  GET_ARRAY_FROM_VAR ("BASH_ARGC", bash_argc_v, bash_argc_a);
#  endif
#endif

  fd = open (filename, O_RDONLY);

  if (fd < 0 || (fstat (fd, &finfo) == -1))
    {
      i = errno;
      if (fd >= 0)
	close (fd);
      errno = i;

file_error_and_exit:
      if (((flags & FEVAL_ENOENTOK) == 0) || errno != ENOENT)
	file_error (filename);

      if (flags & FEVAL_LONGJMP)
	{
	  last_command_exit_value = EXECUTION_FAILURE;
	  jump_to_top_level (EXITPROG);
	}

      return ((flags & FEVAL_BUILTIN) ? EXECUTION_FAILURE
      				      : ((errno == ENOENT && (flags & FEVAL_ENOENTOK) != 0) ? 0 : -1));
    }

  errfunc = ((flags & FEVAL_BUILTIN) ? builtin_error : internal_error);

  if (S_ISDIR (finfo.st_mode))
    {
      (*errfunc) (_("%s: is a directory"), filename);
      close (fd);
      return ((flags & FEVAL_BUILTIN) ? EXECUTION_FAILURE : -1);
    }
  else if ((flags & FEVAL_REGFILE) && S_ISREG (finfo.st_mode) == 0)
    {
      (*errfunc) (_("%s: not a regular file"), filename);
      close (fd);
      return ((flags & FEVAL_BUILTIN) ? EXECUTION_FAILURE : -1);
    }

  file_size = (size_t)finfo.st_size;
  /* Check for overflow with large files. */
  if (file_size != finfo.st_size || file_size + 1 < file_size)
    {
      (*errfunc) (_("%s: file is too large"), filename);
      close (fd);
      return ((flags & FEVAL_BUILTIN) ? EXECUTION_FAILURE : -1);
    }      

  if (S_ISREG (finfo.st_mode) && file_size <= SSIZE_MAX)
    {
      string = (char *)xmalloc (1 + file_size);
      nr = read (fd, string, file_size);
      if (nr >= 0)
	string[nr] = '\0';
    }
  else
    nr = zmapfd (fd, &string, 0);

  return_val = errno;
  close (fd);
  errno = return_val;

  if (nr < 0)		/* XXX was != file_size, not < 0 */
    {
      free (string);
      goto file_error_and_exit;
    }

  if (nr == 0)
    {
      free (string);
      return ((flags & FEVAL_BUILTIN) ? EXECUTION_SUCCESS : 1);
    }
      
  if ((flags & FEVAL_CHECKBINARY) && 
      check_binary_file (string, (nr > 80) ? 80 : nr))
    {
      free (string);
      (*errfunc) (_("%s: cannot execute binary file"), filename);
      return ((flags & FEVAL_BUILTIN) ? EX_BINARY_FILE : -1);
    }

  i = strlen (string);
  if (i < nr)
    {
      for (nnull = i = 0; i < nr; i++)
	if (string[i] == '\0')
          {
	    memmove (string+i, string+i+1, nr - i);
	    nr--;
	    /* Even if the `check binary' flag is not set, we want to avoid
	       sourcing files with more than 256 null characters -- that
	       probably indicates a binary file. */
	    if ((flags & FEVAL_BUILTIN) && ++nnull > 256)
	      {
		free (string);
		(*errfunc) (_("%s: cannot execute binary file"), filename);
		return ((flags & FEVAL_BUILTIN) ? EX_BINARY_FILE : -1);
	      }
          }
    }

  if (flags & FEVAL_UNWINDPROT)
    {
      begin_unwind_frame ("_evalfile");

      unwind_protect_int (return_catch_flag);
      unwind_protect_jmp_buf (return_catch);
      if (flags & FEVAL_NONINT)
	unwind_protect_int (interactive);
      unwind_protect_int (sourcelevel);
    }
  else
    {
      COPY_PROCENV (return_catch, old_return_catch);
      if (flags & FEVAL_NONINT)
	old_interactive = interactive;
    }

  if (flags & FEVAL_NONINT)
    interactive = 0;

  return_catch_flag++;
  sourcelevel++;

#if defined (ARRAY_VARS)
  array_push (bash_source_a, (char *)filename);
  t = itos (executing_line_number ());
  array_push (bash_lineno_a, t);
  free (t);
  array_push (funcname_a, "source");	/* not exactly right */

  fa = (struct func_array_state *)xmalloc (sizeof (struct func_array_state));
  fa->source_a = bash_source_a;
  fa->source_v = bash_source_v;
  fa->lineno_a = bash_lineno_a;
  fa->lineno_v = bash_lineno_v;
  fa->funcname_a = funcname_a;
  fa->funcname_v = funcname_v;
  if (flags & FEVAL_UNWINDPROT)
    add_unwind_protect (restore_funcarray_state, fa);

#  if defined (DEBUGGER)
  /* Have to figure out a better way to do this when `source' is supplied
     arguments */
  if ((flags & FEVAL_NOPUSHARGS) == 0)
    {
      if (shell_compatibility_level <= 44)
	init_bash_argv ();
      array_push (bash_argv_a, (char *)filename);	/* XXX - unconditionally? */
      tt[0] = '1'; tt[1] = '\0';
      array_push (bash_argc_a, tt);
      if (flags & FEVAL_UNWINDPROT)
	add_unwind_protect (pop_args, 0);
    }
#  endif
#endif

  /* set the flags to be passed to parse_and_execute */
  pflags = SEVAL_RESETLINE;
  pflags |= (flags & FEVAL_HISTORY) ? 0 : SEVAL_NOHIST;

  if (flags & FEVAL_BUILTIN)
    result = EXECUTION_SUCCESS;

  return_val = setjmp_nosigs (return_catch);

  /* If `return' was seen outside of a function, but in the script, then
     force parse_and_execute () to clean up. */
  if (return_val)
    {
      parse_and_execute_cleanup (-1);
      result = return_catch_value;
    }
  else
    result = parse_and_execute (string, filename, pflags);

  if (flags & FEVAL_UNWINDPROT)
    run_unwind_frame ("_evalfile");
  else
    {
      if (flags & FEVAL_NONINT)
	interactive = old_interactive;
#if defined (ARRAY_VARS)
      restore_funcarray_state (fa);
#  if defined (DEBUGGER)
      if ((flags & FEVAL_NOPUSHARGS) == 0)
	{
	  /* Don't need to call pop_args here until we do something better
	     when source is passed arguments (see above). */
	  array_pop (bash_argc_a);
	  array_pop (bash_argv_a);
	}
#  endif
#endif
      return_catch_flag--;
      sourcelevel--;
      COPY_PROCENV (old_return_catch, return_catch);
    }

  /* If we end up with EOF after sourcing a file, which can happen when the file
     doesn't end with a newline, pretend that it did. */
  if (current_token == yacc_EOF)
    push_token ('\n');		/* XXX */

  return ((flags & FEVAL_BUILTIN) ? result : 1);
}

int
maybe_execute_file (fname, force_noninteractive)
     const char *fname;
     int force_noninteractive;
{
  char *filename;
  int result, flags;

  filename = bash_tilde_expand (fname, 0);
  flags = FEVAL_ENOENTOK;
  if (force_noninteractive)
    flags |= FEVAL_NONINT;
  result = _evalfile (filename, flags);
  free (filename);
  return result;
}

int
force_execute_file (fname, force_noninteractive)
     const char *fname;
     int force_noninteractive;
{
  char *filename;
  int result, flags;

  filename = bash_tilde_expand (fname, 0);
  flags = 0;
  if (force_noninteractive)
    flags |= FEVAL_NONINT;
  result = _evalfile (filename, flags);
  free (filename);
  return result;
}

#if defined (HISTORY)
int
fc_execute_file (filename)
     const char *filename;
{
  int flags;

  /* We want these commands to show up in the history list if
     remember_on_history is set.  We use FEVAL_BUILTIN to return
     the result of parse_and_execute. */
  flags = FEVAL_ENOENTOK|FEVAL_HISTORY|FEVAL_REGFILE|FEVAL_BUILTIN;
  return (_evalfile (filename, flags));
}
#endif /* HISTORY */

int
source_file (filename, sflags)
     const char *filename;
     int sflags;
{
  int flags, rval;

  flags = FEVAL_BUILTIN|FEVAL_UNWINDPROT|FEVAL_NONINT;
  if (sflags)
    flags |= FEVAL_NOPUSHARGS;
  /* POSIX shells exit if non-interactive and file error. */
  if (posixly_correct && interactive_shell == 0 && executing_command_builtin == 0)
    flags |= FEVAL_LONGJMP;
  rval = _evalfile (filename, flags);

  run_return_trap ();
  return rval;
}
