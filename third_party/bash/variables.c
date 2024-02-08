/* variables.c -- Functions for hacking shell variables. */

/* Copyright (C) 1987-2022 Free Software Foundation, Inc.

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

#include "bashtypes.h"
#include "posixstat.h"
#include "posixtime.h"

#if defined (__QNX__)
#  if defined (__QNXNTO__)
#    include <sys/netmgr.h>
#  else
#    include <sys/vc.h>
#  endif /* !__QNXNTO__ */
#endif /* __QNX__ */

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#include <stdio.h>
#include "chartypes.h"
#if defined (HAVE_PWD_H)
#  include <pwd.h>
#endif
#include "bashansi.h"
#include "bashintl.h"
#include "filecntl.h"

#define NEED_XTRACE_SET_DECL

#include "shell.h"
#include "parser.h"
#include "flags.h"
#include "execute_cmd.h"
#include "findcmd.h"
#include "mailcheck.h"
#include "input.h"
#include "hashcmd.h"
#include "pathexp.h"
#include "alias.h"
#include "jobs.h"

#include "version.h"

#include "getopt.h"
#include "common.h"
#include "builtext.h"

#if defined (READLINE)
#  include "bashline.h"
#  include "third_party/readline/readline.h"
#else
#  include "tilde.h"
#endif

#if defined (HISTORY)
#  include "bashhist.h"
#  include "third_party/readline/history.h"
#endif /* HISTORY */

#if defined (PROGRAMMABLE_COMPLETION)
#  include "pcomplete.h"
#endif

#define VARIABLES_HASH_BUCKETS	1024	/* must be power of two */
#define FUNCTIONS_HASH_BUCKETS	512
#define TEMPENV_HASH_BUCKETS	4	/* must be power of two */

#define BASHFUNC_PREFIX		"BASH_FUNC_"
#define BASHFUNC_PREFLEN	10	/* == strlen(BASHFUNC_PREFIX */
#define BASHFUNC_SUFFIX		"%%"
#define BASHFUNC_SUFFLEN	2	/* == strlen(BASHFUNC_SUFFIX) */

#if ARRAY_EXPORT
#define BASHARRAY_PREFIX	"BASH_ARRAY_"
#define BASHARRAY_PREFLEN	11
#define BASHARRAY_SUFFIX	"%%"
#define BASHARRAY_SUFFLEN	2

#define BASHASSOC_PREFIX	"BASH_ASSOC_"
#define BASHASSOC_PREFLEN	11
#define BASHASSOC_SUFFIX	"%%"	/* needs to be the same as BASHARRAY_SUFFIX */
#define BASHASSOC_SUFFLEN	2
#endif

/* flags for find_variable_internal */

#define FV_FORCETEMPENV		0x01
#define FV_SKIPINVISIBLE	0x02
#define FV_NODYNAMIC		0x04

extern char **environ;

/* Variables used here and defined in other files. */
extern time_t shell_start_time;
extern struct timeval shellstart;

/* The list of shell variables that the user has created at the global
   scope, or that came from the environment. */
VAR_CONTEXT *global_variables = (VAR_CONTEXT *)NULL;

/* The current list of shell variables, including function scopes */
VAR_CONTEXT *shell_variables = (VAR_CONTEXT *)NULL;

/* The list of shell functions that the user has created, or that came from
   the environment. */
HASH_TABLE *shell_functions = (HASH_TABLE *)NULL;

HASH_TABLE *invalid_env = (HASH_TABLE *)NULL;

#if defined (DEBUGGER)
/* The table of shell function definitions that the user defined or that
   came from the environment. */
HASH_TABLE *shell_function_defs = (HASH_TABLE *)NULL;
#endif

/* The current variable context.  This is really a count of how deep into
   executing functions we are. */
int variable_context = 0;

/* If non-zero, local variables inherit values and attributes from a variable
   with the same name at a previous scope. */
int localvar_inherit = 0;

/* If non-zero, calling `unset' on local variables in previous scopes marks
   them as invisible so lookups find them unset. This is the same behavior
   as local variables in the current local scope. */
int localvar_unset = 0;

/* The set of shell assignments which are made only in the environment
   for a single command. */
HASH_TABLE *temporary_env = (HASH_TABLE *)NULL;

/* Set to non-zero if an assignment error occurs while putting variables
   into the temporary environment. */
int tempenv_assign_error;

/* Some funky variables which are known about specially.  Here is where
   "$*", "$1", and all the cruft is kept. */
char *dollar_vars[10];
WORD_LIST *rest_of_args = (WORD_LIST *)NULL;
int posparam_count = 0;

/* The value of $$. */
pid_t dollar_dollar_pid;

/* Non-zero means that we have to remake EXPORT_ENV. */
int array_needs_making = 1;

/* The number of times BASH has been executed.  This is set
   by initialize_variables (). */
int shell_level = 0;

/* An array which is passed to commands as their environment.  It is
   manufactured from the union of the initial environment and the
   shell variables that are marked for export. */
char **export_env = (char **)NULL;
static int export_env_index;
static int export_env_size;

#if defined (READLINE)
static int winsize_assignment;		/* currently assigning to LINES or COLUMNS */
#endif

SHELL_VAR nameref_invalid_value;
static SHELL_VAR nameref_maxloop_value;

static HASH_TABLE *last_table_searched;	/* hash_lookup sets this */
static VAR_CONTEXT *last_context_searched;

/* Some forward declarations. */
static void create_variable_tables PARAMS((void));

static void set_machine_vars PARAMS((void));
static void set_home_var PARAMS((void));
static void set_shell_var PARAMS((void));
static char *get_bash_name PARAMS((void));
static void initialize_shell_level PARAMS((void));
static void uidset PARAMS((void));
#if defined (ARRAY_VARS)
static void make_vers_array PARAMS((void));
#endif

static SHELL_VAR *null_assign PARAMS((SHELL_VAR *, char *, arrayind_t, char *));
#if defined (ARRAY_VARS)
static SHELL_VAR *null_array_assign PARAMS((SHELL_VAR *, char *, arrayind_t, char *));
#endif
static SHELL_VAR *get_self PARAMS((SHELL_VAR *));

#if defined (ARRAY_VARS)
static SHELL_VAR *init_dynamic_array_var PARAMS((char *, sh_var_value_func_t *, sh_var_assign_func_t *, int));
static SHELL_VAR *init_dynamic_assoc_var PARAMS((char *, sh_var_value_func_t *, sh_var_assign_func_t *, int));
#endif

static inline SHELL_VAR *set_int_value (SHELL_VAR *, intmax_t, int);
static inline SHELL_VAR *set_string_value (SHELL_VAR *, const char *, int);

static SHELL_VAR *assign_seconds PARAMS((SHELL_VAR *, char *, arrayind_t, char *));
static SHELL_VAR *get_seconds PARAMS((SHELL_VAR *));
static SHELL_VAR *init_seconds_var PARAMS((void));

static SHELL_VAR *assign_random PARAMS((SHELL_VAR *, char *, arrayind_t, char *));
static SHELL_VAR *get_random PARAMS((SHELL_VAR *));

static SHELL_VAR *get_urandom PARAMS((SHELL_VAR *));

static SHELL_VAR *assign_lineno PARAMS((SHELL_VAR *, char *, arrayind_t, char *));
static SHELL_VAR *get_lineno PARAMS((SHELL_VAR *));

static SHELL_VAR *assign_subshell PARAMS((SHELL_VAR *, char *, arrayind_t, char *));
static SHELL_VAR *get_subshell PARAMS((SHELL_VAR *));

static SHELL_VAR *get_epochseconds PARAMS((SHELL_VAR *));
static SHELL_VAR *get_epochrealtime PARAMS((SHELL_VAR *));

static SHELL_VAR *get_bashpid PARAMS((SHELL_VAR *));

static SHELL_VAR *get_bash_argv0 PARAMS((SHELL_VAR *));
static SHELL_VAR *assign_bash_argv0 PARAMS((SHELL_VAR *, char *, arrayind_t, char *));
static void set_argv0 PARAMS((void));

#if defined (HISTORY)
static SHELL_VAR *get_histcmd PARAMS((SHELL_VAR *));
#endif

#if defined (READLINE)
static SHELL_VAR *get_comp_wordbreaks PARAMS((SHELL_VAR *));
static SHELL_VAR *assign_comp_wordbreaks PARAMS((SHELL_VAR *, char *, arrayind_t, char *));
#endif

#if defined (PUSHD_AND_POPD) && defined (ARRAY_VARS)
static SHELL_VAR *assign_dirstack PARAMS((SHELL_VAR *, char *, arrayind_t, char *));
static SHELL_VAR *get_dirstack PARAMS((SHELL_VAR *));
#endif

#if defined (ARRAY_VARS)
static SHELL_VAR *get_groupset PARAMS((SHELL_VAR *));
#  if defined (DEBUGGER)
static SHELL_VAR *get_bashargcv PARAMS((SHELL_VAR *));
#  endif
static SHELL_VAR *build_hashcmd PARAMS((SHELL_VAR *));
static SHELL_VAR *get_hashcmd PARAMS((SHELL_VAR *));
static SHELL_VAR *assign_hashcmd PARAMS((SHELL_VAR *,  char *, arrayind_t, char *));
#  if defined (ALIAS)
static SHELL_VAR *build_aliasvar PARAMS((SHELL_VAR *));
static SHELL_VAR *get_aliasvar PARAMS((SHELL_VAR *));
static SHELL_VAR *assign_aliasvar PARAMS((SHELL_VAR *,  char *, arrayind_t, char *));
#  endif
#endif

static SHELL_VAR *get_funcname PARAMS((SHELL_VAR *));
static SHELL_VAR *init_funcname_var PARAMS((void));

static void initialize_dynamic_variables PARAMS((void));

static SHELL_VAR *bind_invalid_envvar PARAMS((const char *, char *, int));

static int var_sametype PARAMS((SHELL_VAR *, SHELL_VAR *));

static SHELL_VAR *hash_lookup PARAMS((const char *, HASH_TABLE *));
static SHELL_VAR *new_shell_variable PARAMS((const char *));
static SHELL_VAR *make_new_variable PARAMS((const char *, HASH_TABLE *));
static SHELL_VAR *bind_variable_internal PARAMS((const char *, char *, HASH_TABLE *, int, int));

static void dispose_variable_value PARAMS((SHELL_VAR *));
static void free_variable_hash_data PARAMS((PTR_T));

static VARLIST *vlist_alloc PARAMS((int));
static VARLIST *vlist_realloc PARAMS((VARLIST *, int));
static void vlist_add PARAMS((VARLIST *, SHELL_VAR *, int));

static void flatten PARAMS((HASH_TABLE *, sh_var_map_func_t *, VARLIST *, int));

static int qsort_var_comp PARAMS((SHELL_VAR **, SHELL_VAR **));

static SHELL_VAR **vapply PARAMS((sh_var_map_func_t *));
static SHELL_VAR **fapply PARAMS((sh_var_map_func_t *));

static int visible_var PARAMS((SHELL_VAR *));
static int visible_and_exported PARAMS((SHELL_VAR *));
static int export_environment_candidate PARAMS((SHELL_VAR *));
static int local_and_exported PARAMS((SHELL_VAR *));
static int visible_variable_in_context PARAMS((SHELL_VAR *));
static int variable_in_context PARAMS((SHELL_VAR *));
#if defined (ARRAY_VARS)
static int visible_array_vars PARAMS((SHELL_VAR *));
#endif

static SHELL_VAR *find_variable_internal PARAMS((const char *, int));

static SHELL_VAR *find_nameref_at_context PARAMS((SHELL_VAR *, VAR_CONTEXT *));
static SHELL_VAR *find_variable_nameref_context PARAMS((SHELL_VAR *, VAR_CONTEXT *, VAR_CONTEXT **));
static SHELL_VAR *find_variable_last_nameref_context PARAMS((SHELL_VAR *, VAR_CONTEXT *, VAR_CONTEXT **));

static SHELL_VAR *bind_tempenv_variable PARAMS((const char *, char *));
static void push_posix_temp_var PARAMS((PTR_T));
static void push_temp_var PARAMS((PTR_T));
static void propagate_temp_var PARAMS((PTR_T));
static void dispose_temporary_env PARAMS((sh_free_func_t *));     

static inline char *mk_env_string PARAMS((const char *, const char *, int));
static char **make_env_array_from_var_list PARAMS((SHELL_VAR **));
static char **make_var_export_array PARAMS((VAR_CONTEXT *));
static char **make_func_export_array PARAMS((void));
static void add_temp_array_to_env PARAMS((char **, int, int));

static int n_shell_variables PARAMS((void));
static int set_context PARAMS((SHELL_VAR *));

static void push_func_var PARAMS((PTR_T));
static void push_builtin_var PARAMS((PTR_T));
static void push_exported_var PARAMS((PTR_T));

static void delete_local_contexts PARAMS((VAR_CONTEXT *));

/* This needs to be looked at again. */
static inline void push_posix_tempvar_internal PARAMS((SHELL_VAR *, int));

static inline int find_special_var PARAMS((const char *));

static void
create_variable_tables ()
{
  if (shell_variables == 0)
    {
      shell_variables = global_variables = new_var_context ((char *)NULL, 0);
      shell_variables->scope = 0;
      shell_variables->table = hash_create (VARIABLES_HASH_BUCKETS);
    }

  if (shell_functions == 0)
    shell_functions = hash_create (FUNCTIONS_HASH_BUCKETS);

#if defined (DEBUGGER)
  if (shell_function_defs == 0)
    shell_function_defs = hash_create (FUNCTIONS_HASH_BUCKETS);
#endif
}

/* Initialize the shell variables from the current environment.
   If PRIVMODE is nonzero, don't import functions from ENV or
   parse $SHELLOPTS. */
void
initialize_shell_variables (env, privmode)
     char **env;
     int privmode;
{
  char *name, *string, *temp_string;
  int c, char_index, string_index, string_length, ro;
  SHELL_VAR *temp_var;

  create_variable_tables ();

  for (string_index = 0; env && (string = env[string_index++]); )
    {
      char_index = 0;
      name = string;
      while ((c = *string++) && c != '=')
	;
      if (string[-1] == '=')
	char_index = string - name - 1;

      /* If there are weird things in the environment, like `=xxx' or a
	 string without an `=', just skip them. */
      if (char_index == 0)
	continue;

      /* ASSERT(name[char_index] == '=') */
      name[char_index] = '\0';
      /* Now, name = env variable name, string = env variable value, and
	 char_index == strlen (name) */

      temp_var = (SHELL_VAR *)NULL;

#if defined (FUNCTION_IMPORT)
      /* If exported function, define it now.  Don't import functions from
	 the environment in privileged mode. */
      if (privmode == 0 && read_but_dont_execute == 0 && 
          STREQN (BASHFUNC_PREFIX, name, BASHFUNC_PREFLEN) &&
          STREQ (BASHFUNC_SUFFIX, name + char_index - BASHFUNC_SUFFLEN) &&
	  STREQN ("() {", string, 4))
	{
	  size_t namelen;
	  char *tname;		/* desired imported function name */

	  namelen = char_index - BASHFUNC_PREFLEN - BASHFUNC_SUFFLEN;

	  tname = name + BASHFUNC_PREFLEN;	/* start of func name */
	  tname[namelen] = '\0';		/* now tname == func name */

	  string_length = strlen (string);
	  temp_string = (char *)xmalloc (namelen + string_length + 2);

	  memcpy (temp_string, tname, namelen);
	  temp_string[namelen] = ' ';
	  memcpy (temp_string + namelen + 1, string, string_length + 1);

	  /* Don't import function names that are invalid identifiers from the
	     environment in posix mode, though we still allow them to be defined as
	     shell variables. */
	  if (absolute_program (tname) == 0 && (posixly_correct == 0 || legal_identifier (tname)))
	    parse_and_execute (temp_string, tname, SEVAL_NONINT|SEVAL_NOHIST|SEVAL_FUNCDEF|SEVAL_ONECMD);
	  else
	    free (temp_string);		/* parse_and_execute does this */

	  if (temp_var = find_function (tname))
	    {
	      VSETATTR (temp_var, (att_exported|att_imported));
	      array_needs_making = 1;
	    }
	  else
	    {
	      if (temp_var = bind_invalid_envvar (name, string, 0))
		{
		  VSETATTR (temp_var, (att_exported | att_imported | att_invisible));
		  array_needs_making = 1;
		}
	      last_command_exit_value = EXECUTION_FAILURE;
	      report_error (_("error importing function definition for `%s'"), tname);
	    }

	  /* Restore original suffix */
	  tname[namelen] = BASHFUNC_SUFFIX[0];
	}
      else
#endif /* FUNCTION_IMPORT */
#if defined (ARRAY_VARS)
#  if ARRAY_EXPORT
      /* Array variables may not yet be exported. */
      if (STREQN (BASHARRAY_PREFIX, name, BASHARRAY_PREFLEN) &&
	  STREQN (BASHARRAY_SUFFIX, name + char_index - BASHARRAY_SUFFLEN, BASHARRAY_SUFFLEN) &&
	  *string == '(' && string[1] == '[' && string[strlen (string) - 1] == ')')
	{
	  size_t namelen;
	  char *tname;		/* desired imported array variable name */

	  namelen = char_index - BASHARRAY_PREFLEN - BASHARRAY_SUFFLEN;

	  tname = name + BASHARRAY_PREFLEN;	/* start of variable name */
	  tname[namelen] = '\0';		/* now tname == varname */
	  
	  string_length = 1;
	  temp_string = extract_array_assignment_list (string, &string_length);
	  temp_var = assign_array_from_string (tname, temp_string, 0);
	  FREE (temp_string);
	  if (temp_var)
	    {
	      VSETATTR (temp_var, (att_exported | att_imported));
	      array_needs_making = 1;
	    }
	}
      else if (STREQN (BASHASSOC_PREFIX, name, BASHASSOC_PREFLEN) &&
	  STREQN (BASHASSOC_SUFFIX, name + char_index - BASHASSOC_SUFFLEN, BASHASSOC_SUFFLEN) &&
	  *string == '(' && string[1] == '[' && string[strlen (string) - 1] == ')')
	{
	  size_t namelen;
	  char *tname;		/* desired imported assoc variable name */

	  namelen = char_index - BASHASSOC_PREFLEN - BASHASSOC_SUFFLEN;

	  tname = name + BASHASSOC_PREFLEN;	/* start of variable name */
	  tname[namelen] = '\0';		/* now tname == varname */

	  /* need to make sure it exists as an associative array first */
	  temp_var = find_or_make_array_variable (tname, 2);
	  if (temp_var)
	    {
	      string_length = 1;
	      temp_string = extract_array_assignment_list (string, &string_length);
	      temp_var = assign_array_var_from_string (temp_var, temp_string, 0);
	    }
	  FREE (temp_string);
	  if (temp_var)
	    {
	      VSETATTR (temp_var, (att_exported | att_imported));
	      array_needs_making = 1;
	    }
	}
      else
#  endif /* ARRAY_EXPORT */
#endif
	{
	  ro = 0;
	  /* If we processed a command-line option that caused SHELLOPTS to be
	     set, it may already be set (and read-only) by the time we process
	     the shell's environment. */
	  if (/* posixly_correct &&*/ STREQ (name, "SHELLOPTS"))
	    {
	      temp_var = find_variable ("SHELLOPTS");
	      ro = temp_var && readonly_p (temp_var);
	      if (temp_var)
		VUNSETATTR (temp_var, att_readonly);
	    }
	  if (legal_identifier (name))
	    {
	      temp_var = bind_variable (name, string, 0);
	      if (temp_var)
		{
		  VSETATTR (temp_var, (att_exported | att_imported));
		  if (ro)
		    VSETATTR (temp_var, att_readonly);
		}
	    }
	  else
	    {
	      temp_var = bind_invalid_envvar (name, string, 0);
	      if (temp_var)
		VSETATTR (temp_var, (att_exported | att_imported | att_invisible));
	    }
	  if (temp_var)
	    array_needs_making = 1;
	}

      name[char_index] = '=';
      /* temp_var can be NULL if it was an exported function with a syntax
	 error (a different bug, but it still shouldn't dump core). */
      if (temp_var && function_p (temp_var) == 0)	/* XXX not yet */
	{
	  CACHE_IMPORTSTR (temp_var, name);
	}
    }

  set_pwd ();

  /* Set up initial value of $_ */
  temp_var = set_if_not ("_", dollar_vars[0]);

  /* Remember this pid. */
  dollar_dollar_pid = getpid ();

  /* Now make our own defaults in case the vars that we think are
     important are missing. */
  temp_var = set_if_not ("PATH", DEFAULT_PATH_VALUE);
  temp_var = set_if_not ("TERM", "dumb");

#if defined (__QNX__)
  /* set node id -- don't import it from the environment */
  {
    char node_name[22];
#  if defined (__QNXNTO__)
    netmgr_ndtostr(ND2S_LOCAL_STR, ND_LOCAL_NODE, node_name, sizeof(node_name));
#  else
    qnx_nidtostr (getnid (), node_name, sizeof (node_name));
#  endif
    temp_var = bind_variable ("NODE", node_name, 0);
    if (temp_var)
      set_auto_export (temp_var);
  }
#endif

  /* set up the prompts. */
  if (interactive_shell)
    {
#if defined (PROMPT_STRING_DECODE)
      set_if_not ("PS1", primary_prompt);
#else
      if (current_user.uid == -1)
	get_current_user_info ();
      set_if_not ("PS1", current_user.euid == 0 ? "# " : primary_prompt);
#endif
      set_if_not ("PS2", secondary_prompt);
    }

  if (current_user.euid == 0)
    bind_variable ("PS4", "+ ", 0);
  else
    set_if_not ("PS4", "+ ");

  /* Don't allow IFS to be imported from the environment. */
  temp_var = bind_variable ("IFS", " \t\n", 0);
  setifs (temp_var);

  /* Magic machine types.  Pretty convenient. */
  set_machine_vars ();

  /* Default MAILCHECK for interactive shells.  Defer the creation of a
     default MAILPATH until the startup files are read, because MAIL
     names a mail file if MAILPATH is not set, and we should provide a
     default only if neither is set. */
  if (interactive_shell)
    {
      temp_var = set_if_not ("MAILCHECK", posixly_correct ? "600" : "60");
      VSETATTR (temp_var, att_integer);
    }

  /* Do some things with shell level. */
  initialize_shell_level ();

  set_ppid ();

  set_argv0 ();

  /* Initialize the `getopts' stuff. */
  temp_var = bind_variable ("OPTIND", "1", 0);
  VSETATTR (temp_var, att_integer);
  getopts_reset (0);
  bind_variable ("OPTERR", "1", 0);
  sh_opterr = 1;

  if (login_shell == 1 && posixly_correct == 0)
    set_home_var ();

  /* Get the full pathname to THIS shell, and set the BASH variable
     to it. */
  name = get_bash_name ();
  temp_var = bind_variable ("BASH", name, 0);
  free (name);

  /* Make the exported environment variable SHELL be the user's login
     shell.  Note that the `tset' command looks at this variable
     to determine what style of commands to output; if it ends in "csh",
     then C-shell commands are output, else Bourne shell commands. */
  set_shell_var ();

  /* Make a variable called BASH_VERSION which contains the version info. */
  bind_variable ("BASH_VERSION", shell_version_string (), 0);
#if defined (ARRAY_VARS)
  make_vers_array ();
#endif

  if (command_execution_string)
    bind_variable ("BASH_EXECUTION_STRING", command_execution_string, 0);

  /* Find out if we're supposed to be in Posix.2 mode via an
     environment variable. */
  temp_var = find_variable ("POSIXLY_CORRECT");
  if (!temp_var)
    temp_var = find_variable ("POSIX_PEDANTIC");
  if (temp_var && imported_p (temp_var))
    sv_strict_posix (temp_var->name);

#if defined (HISTORY)
  /* Set history variables to defaults, and then do whatever we would
     do if the variable had just been set.  Do this only in the case
     that we are remembering commands on the history list. */
  if (remember_on_history)
    {
      name = bash_tilde_expand (posixly_correct ? "~/.sh_history" : "~/.bash_history", 0);

      set_if_not ("HISTFILE", name);
      free (name);
    }
#endif /* HISTORY */

  /* Seed the random number generators. */
  seedrand ();
  seedrand32 ();

  /* Handle some "special" variables that we may have inherited from a
     parent shell. */
  if (interactive_shell)
    {
      temp_var = find_variable ("IGNOREEOF");
      if (!temp_var)
	temp_var = find_variable ("ignoreeof");
      if (temp_var && imported_p (temp_var))
	sv_ignoreeof (temp_var->name);
    }

#if defined (HISTORY)
  if (interactive_shell && remember_on_history)
    {
      sv_history_control ("HISTCONTROL");
      sv_histignore ("HISTIGNORE");
      sv_histtimefmt ("HISTTIMEFORMAT");
    }
#endif /* HISTORY */

#if defined (READLINE) && defined (STRICT_POSIX)
  /* POSIXLY_CORRECT will be 1 here if the shell was compiled
     -DSTRICT_POSIX or if POSIXLY_CORRECT was supplied in the shell's
     environment */
  if (interactive_shell && posixly_correct && no_line_editing == 0)
    rl_prefer_env_winsize = 1;
#endif /* READLINE && STRICT_POSIX */

  /* Get the user's real and effective user ids. */
  uidset ();

  temp_var = set_if_not ("BASH_LOADABLES_PATH", DEFAULT_LOADABLE_BUILTINS_PATH);

  temp_var = find_variable ("BASH_XTRACEFD");
  if (temp_var && imported_p (temp_var))
    sv_xtracefd (temp_var->name);

  sv_shcompat ("BASH_COMPAT");

  /* Allow FUNCNEST to be inherited from the environment. */
  sv_funcnest ("FUNCNEST");

  /* Initialize the dynamic variables, and seed their values. */
  initialize_dynamic_variables ();
}

/* **************************************************************** */
/*								    */
/*	     Setting values for special shell variables		    */
/*								    */
/* **************************************************************** */

static void
set_machine_vars ()
{
  SHELL_VAR *temp_var;

  temp_var = set_if_not ("HOSTTYPE", HOSTTYPE);
  temp_var = set_if_not ("OSTYPE", OSTYPE);
  temp_var = set_if_not ("MACHTYPE", MACHTYPE);

  temp_var = set_if_not ("HOSTNAME", current_host_name);
}

/* Set $HOME to the information in the password file if we didn't get
   it from the environment. */

/* This function is not static so the tilde and readline libraries can
   use it. */
char *
sh_get_home_dir ()
{
  if (current_user.home_dir == 0)
    get_current_user_info ();
  return current_user.home_dir;
}

static void
set_home_var ()
{
  SHELL_VAR *temp_var;

  temp_var = find_variable ("HOME");
  if (temp_var == 0)
    temp_var = bind_variable ("HOME", sh_get_home_dir (), 0);
#if 0
  VSETATTR (temp_var, att_exported);
#endif
}

/* Set $SHELL to the user's login shell if it is not already set.  Call
   get_current_user_info if we haven't already fetched the shell. */
static void
set_shell_var ()
{
  SHELL_VAR *temp_var;

  temp_var = find_variable ("SHELL");
  if (temp_var == 0)
    {
      if (current_user.shell == 0)
	get_current_user_info ();
      temp_var = bind_variable ("SHELL", current_user.shell, 0);
    }
#if 0
  VSETATTR (temp_var, att_exported);
#endif
}

static char *
get_bash_name ()
{
  char *name;

  if ((login_shell == 1) && RELPATH(shell_name))
    {
      if (current_user.shell == 0)
	get_current_user_info ();
      name = savestring (current_user.shell);
    }
  else if (ABSPATH(shell_name))
    name = savestring (shell_name);
  else if (shell_name[0] == '.' && shell_name[1] == '/')
    {
      /* Fast path for common case. */
      char *cdir;
      int len;

      cdir = get_string_value ("PWD");
      if (cdir)
	{
	  len = strlen (cdir);
	  name = (char *)xmalloc (len + strlen (shell_name) + 1);
	  strcpy (name, cdir);
	  strcpy (name + len, shell_name + 1);
	}
      else
	name = savestring (shell_name);
    }
  else
    {
      char *tname;
      int s;

      tname = find_user_command (shell_name);

      if (tname == 0)
	{
	  /* Try the current directory.  If there is not an executable
	     there, just punt and use the login shell. */
	  s = file_status (shell_name);
	  if (s & FS_EXECABLE)
	    {
	      tname = make_absolute (shell_name, get_string_value ("PWD"));
	      if (*shell_name == '.')
		{
		  name = sh_canonpath (tname, PATH_CHECKDOTDOT|PATH_CHECKEXISTS);
		  if (name == 0)
		    name = tname;
		  else
		    free (tname);
		}
	     else
		name = tname;
	    }
	  else
	    {
	      if (current_user.shell == 0)
		get_current_user_info ();
	      name = savestring (current_user.shell);
	    }
	}
      else
	{
	  name = full_pathname (tname);
	  free (tname);
	}
    }

  return (name);
}

void
adjust_shell_level (change)
     int change;
{
  char new_level[5], *old_SHLVL;
  intmax_t old_level;
  SHELL_VAR *temp_var;

  old_SHLVL = get_string_value ("SHLVL");
  if (old_SHLVL == 0 || *old_SHLVL == '\0' || legal_number (old_SHLVL, &old_level) == 0)
    old_level = 0;

  shell_level = old_level + change;
  if (shell_level < 0)
    shell_level = 0;
  else if (shell_level >= 1000)
    {
      internal_warning (_("shell level (%d) too high, resetting to 1"), shell_level);
      shell_level = 1;
    }

  /* We don't need the full generality of itos here. */
  if (shell_level < 10)
    {
      new_level[0] = shell_level + '0';
      new_level[1] = '\0';
    }
  else if (shell_level < 100)
    {
      new_level[0] = (shell_level / 10) + '0';
      new_level[1] = (shell_level % 10) + '0';
      new_level[2] = '\0';
    }
  else if (shell_level < 1000)
    {
      new_level[0] = (shell_level / 100) + '0';
      old_level = shell_level % 100;
      new_level[1] = (old_level / 10) + '0';
      new_level[2] = (old_level % 10) + '0';
      new_level[3] = '\0';
    }

  temp_var = bind_variable ("SHLVL", new_level, 0);
  set_auto_export (temp_var);
}

static void
initialize_shell_level ()
{
  adjust_shell_level (1);
}

/* If we got PWD from the environment, update our idea of the current
   working directory.  In any case, make sure that PWD exists before
   checking it.  It is possible for getcwd () to fail on shell startup,
   and in that case, PWD would be undefined.  If this is an interactive
   login shell, see if $HOME is the current working directory, and if
   that's not the same string as $PWD, set PWD=$HOME. */

void
set_pwd ()
{
  SHELL_VAR *temp_var, *home_var;
  char *temp_string, *home_string, *current_dir;

  home_var = find_variable ("HOME");
  home_string = home_var ? value_cell (home_var) : (char *)NULL;

  temp_var = find_variable ("PWD");
  /* Follow posix rules for importing PWD */
  if (temp_var && imported_p (temp_var) &&
      (temp_string = value_cell (temp_var)) &&
      temp_string[0] == '/' &&
      same_file (temp_string, ".", (struct stat *)NULL, (struct stat *)NULL))
    {
      current_dir = sh_canonpath (temp_string, PATH_CHECKDOTDOT|PATH_CHECKEXISTS);
      if (current_dir == 0)
	current_dir = get_working_directory ("shell_init");
      else
	set_working_directory (current_dir);
      if (posixly_correct && current_dir)
	{
	  temp_var = bind_variable ("PWD", current_dir, 0);
	  set_auto_export (temp_var);
	}  
      free (current_dir);
    }
  else if (home_string && interactive_shell && login_shell &&
	   same_file (home_string, ".", (struct stat *)NULL, (struct stat *)NULL))
    {
      set_working_directory (home_string);
      temp_var = bind_variable ("PWD", home_string, 0);
      set_auto_export (temp_var);
    }
  else
    {
      temp_string = get_working_directory ("shell-init");
      if (temp_string)
	{
	  temp_var = bind_variable ("PWD", temp_string, 0);
	  set_auto_export (temp_var);
	  free (temp_string);
	}
    }

  /* According to the Single Unix Specification, v2, $OLDPWD is an
     `environment variable' and therefore should be auto-exported.  If we
     don't find OLDPWD in the environment, or it doesn't name a directory,
     make a dummy invisible variable for OLDPWD, and mark it as exported. */
  temp_var = find_variable ("OLDPWD");
#if defined (OLDPWD_CHECK_DIRECTORY)
  if (temp_var == 0 || value_cell (temp_var) == 0 || file_isdir (value_cell (temp_var)) == 0)
#else
  if (temp_var == 0 || value_cell (temp_var) == 0)
#endif
    {
      temp_var = bind_variable ("OLDPWD", (char *)NULL, 0);
      VSETATTR (temp_var, (att_exported | att_invisible));
    }
}

/* Make a variable $PPID, which holds the pid of the shell's parent.  */
void
set_ppid ()
{
  char namebuf[INT_STRLEN_BOUND(pid_t) + 1], *name;
  SHELL_VAR *temp_var;

  name = inttostr (getppid (), namebuf, sizeof(namebuf));
  temp_var = find_variable ("PPID");
  if (temp_var)
    VUNSETATTR (temp_var, (att_readonly | att_exported));
  temp_var = bind_variable ("PPID", name, 0);
  VSETATTR (temp_var, (att_readonly | att_integer));
}

static void
uidset ()
{
  char buff[INT_STRLEN_BOUND(uid_t) + 1], *b;
  register SHELL_VAR *v;

  b = inttostr (current_user.uid, buff, sizeof (buff));
  v = find_variable ("UID");
  if (v == 0)
    {
      v = bind_variable ("UID", b, 0);
      VSETATTR (v, (att_readonly | att_integer));
    }

  if (current_user.euid != current_user.uid)
    b = inttostr (current_user.euid, buff, sizeof (buff));

  v = find_variable ("EUID");
  if (v == 0)
    {
      v = bind_variable ("EUID", b, 0);
      VSETATTR (v, (att_readonly | att_integer));
    }
}

#if defined (ARRAY_VARS)
static void
make_vers_array ()
{
  SHELL_VAR *vv;
  ARRAY *av;
  char *s, d[32], b[INT_STRLEN_BOUND(int) + 1];

  unbind_variable_noref ("BASH_VERSINFO");

  vv = make_new_array_variable ("BASH_VERSINFO");
  av = array_cell (vv);
  strcpy (d, dist_version);
  s = strchr (d, '.');
  if (s)
    *s++ = '\0';
  array_insert (av, 0, d);
  array_insert (av, 1, s);
  s = inttostr (patch_level, b, sizeof (b));
  array_insert (av, 2, s);
  s = inttostr (build_version, b, sizeof (b));
  array_insert (av, 3, s);
  array_insert (av, 4, release_status);
  array_insert (av, 5, MACHTYPE);

  VSETATTR (vv, att_readonly);
}
#endif /* ARRAY_VARS */

/* Set the environment variables $LINES and $COLUMNS in response to
   a window size change. */
void
sh_set_lines_and_columns (lines, cols)
     int lines, cols;
{
  char val[INT_STRLEN_BOUND(int) + 1], *v;

#if defined (READLINE)
  /* If we are currently assigning to LINES or COLUMNS, don't do anything. */
  if (winsize_assignment)
    return;
#endif

  v = inttostr (lines, val, sizeof (val));
  bind_variable ("LINES", v, 0);

  v = inttostr (cols, val, sizeof (val));
  bind_variable ("COLUMNS", v, 0);
}

/* **************************************************************** */
/*								    */
/*		   Printing variables and values		    */
/*								    */
/* **************************************************************** */

/* Print LIST (a list of shell variables) to stdout in such a way that
   they can be read back in. */
void
print_var_list (list)
     register SHELL_VAR **list;
{
  register int i;
  register SHELL_VAR *var;

  for (i = 0; list && (var = list[i]); i++)
    if (invisible_p (var) == 0)
      print_assignment (var);
}

/* Print LIST (a list of shell functions) to stdout in such a way that
   they can be read back in. */
void
print_func_list (list)
     register SHELL_VAR **list;
{
  register int i;
  register SHELL_VAR *var;

  for (i = 0; list && (var = list[i]); i++)
    {
      printf ("%s ", var->name);
      print_var_function (var);
      printf ("\n");
    }
}
      
/* Print the value of a single SHELL_VAR.  No newline is
   output, but the variable is printed in such a way that
   it can be read back in. */
void
print_assignment (var)
     SHELL_VAR *var;
{
  if (var_isset (var) == 0)
    return;

  if (function_p (var))
    {
      printf ("%s", var->name);
      print_var_function (var);
      printf ("\n");
    }
#if defined (ARRAY_VARS)
  else if (array_p (var))
    print_array_assignment (var, 0);
  else if (assoc_p (var))
    print_assoc_assignment (var, 0);
#endif /* ARRAY_VARS */
  else
    {
      printf ("%s=", var->name);
      print_var_value (var, 1);
      printf ("\n");
    }
}

/* Print the value cell of VAR, a shell variable.  Do not print
   the name, nor leading/trailing newline.  If QUOTE is non-zero,
   and the value contains shell metacharacters, quote the value
   in such a way that it can be read back in. */
void
print_var_value (var, quote)
     SHELL_VAR *var;
     int quote;
{
  char *t;

  if (var_isset (var) == 0)
    return;

  if (quote && posixly_correct == 0 && ansic_shouldquote (value_cell (var)))
    {
      t = ansic_quote (value_cell (var), 0, (int *)0);
      printf ("%s", t);
      free (t);
    }
  else if (quote && sh_contains_shell_metas (value_cell (var)))
    {
      t = sh_single_quote (value_cell (var));
      printf ("%s", t);
      free (t);
    }
  else
    printf ("%s", value_cell (var));
}

/* Print the function cell of VAR, a shell variable.  Do not
   print the name, nor leading/trailing newline. */
void
print_var_function (var)
     SHELL_VAR *var;
{
  char *x;

  if (function_p (var) && var_isset (var))
    {
      x = named_function_string ((char *)NULL, function_cell(var), FUNC_MULTILINE|FUNC_EXTERNAL);
      printf ("%s", x);
    }
}

/* **************************************************************** */
/*								    */
/*		 	Dynamic Variables			    */
/*								    */
/* **************************************************************** */

/* DYNAMIC VARIABLES

   These are variables whose values are generated anew each time they are
   referenced.  These are implemented using a pair of function pointers
   in the struct variable: assign_func, which is called from bind_variable
   and, if arrays are compiled into the shell, some of the functions in
   arrayfunc.c, and dynamic_value, which is called from find_variable.

   assign_func is called from bind_variable_internal, if
   bind_variable_internal discovers that the variable being assigned to
   has such a function.  The function is called as
	SHELL_VAR *temp = (*(entry->assign_func)) (entry, value, ind)
   and the (SHELL_VAR *)temp is returned as the value of bind_variable.  It
   is usually ENTRY (self).  IND is an index for an array variable, and
   unused otherwise.

   dynamic_value is called from find_variable_internal to return a `new'
   value for the specified dynamic variable.  If this function is NULL,
   the variable is treated as a `normal' shell variable.  If it is not,
   however, then this function is called like this:
	tempvar = (*(var->dynamic_value)) (var);

   Sometimes `tempvar' will replace the value of `var'.  Other times, the
   shell will simply use the string value.  Pretty object-oriented, huh?

   Be warned, though: if you `unset' a special variable, it loses its
   special meaning, even if you subsequently set it.

   The special assignment code would probably have been better put in
   subst.c: do_assignment_internal, in the same style as
   stupidly_hack_special_variables, but I wanted the changes as
   localized as possible.  */

#define INIT_DYNAMIC_VAR(var, val, gfunc, afunc) \
  do \
    { \
      v = bind_variable (var, (val), 0); \
      v->dynamic_value = gfunc; \
      v->assign_func = afunc; \
    } \
  while (0)

#define INIT_DYNAMIC_ARRAY_VAR(var, gfunc, afunc) \
  do \
    { \
      v = make_new_array_variable (var); \
      v->dynamic_value = gfunc; \
      v->assign_func = afunc; \
    } \
  while (0)

#define INIT_DYNAMIC_ASSOC_VAR(var, gfunc, afunc) \
  do \
    { \
      v = make_new_assoc_variable (var); \
      v->dynamic_value = gfunc; \
      v->assign_func = afunc; \
    } \
  while (0)

static SHELL_VAR *
null_assign (self, value, unused, key)
     SHELL_VAR *self;
     char *value;
     arrayind_t unused;
     char *key;
{
  return (self);
}

#if defined (ARRAY_VARS)
static SHELL_VAR *
null_array_assign (self, value, ind, key)
     SHELL_VAR *self;
     char *value;
     arrayind_t ind;
     char *key;
{
  return (self);
}
#endif

/* Degenerate `dynamic_value' function; just returns what's passed without
   manipulation. */
static SHELL_VAR *
get_self (self)
     SHELL_VAR *self;
{
  return (self);
}

#if defined (ARRAY_VARS)
/* A generic dynamic array variable initializer.  Initialize array variable
   NAME with dynamic value function GETFUNC and assignment function SETFUNC. */
static SHELL_VAR *
init_dynamic_array_var (name, getfunc, setfunc, attrs)
     char *name;
     sh_var_value_func_t *getfunc;
     sh_var_assign_func_t *setfunc;
     int attrs;
{
  SHELL_VAR *v;

  v = find_variable (name);
  if (v)
    return (v);
  INIT_DYNAMIC_ARRAY_VAR (name, getfunc, setfunc);
  if (attrs)
    VSETATTR (v, attrs);
  return v;
}

static SHELL_VAR *
init_dynamic_assoc_var (name, getfunc, setfunc, attrs)
     char *name;
     sh_var_value_func_t *getfunc;
     sh_var_assign_func_t *setfunc;
     int attrs;
{
  SHELL_VAR *v;

  v = find_variable (name);
  if (v)
    return (v);
  INIT_DYNAMIC_ASSOC_VAR (name, getfunc, setfunc);
  if (attrs)
    VSETATTR (v, attrs);
  return v;
}
#endif

/* Set the string value of VAR to the string representation of VALUE.
   Right now this takes an INTMAX_T because that's what itos needs. If
   FLAGS&1, we force the integer attribute on. */
static inline SHELL_VAR *
set_int_value (SHELL_VAR *var, intmax_t value, int flags)
{
  char *p;

  p = itos (value);
  FREE (value_cell (var));
  var_setvalue (var, p);
  if (flags & 1)
    VSETATTR (var, att_integer);
  return (var);
}

static inline SHELL_VAR *
set_string_value (SHELL_VAR *var, const char *value, int flags)
{
  char *p;

  if (value && *value)
    p = savestring (value);
  else
    {
      p = (char *)xmalloc (1);
      p[0] = '\0';
    }
  FREE (value_cell (var));
  var_setvalue (var, p);
  return (var);
}

/* The value of $SECONDS.  This is the number of seconds since shell
   invocation, or, the number of seconds since the last assignment + the
   value of the last assignment. */
static intmax_t seconds_value_assigned;

static SHELL_VAR *
assign_seconds (self, value, unused, key)
     SHELL_VAR *self;
     char *value;
     arrayind_t unused;
     char *key;
{
  intmax_t nval;
  int expok;

  if (integer_p (self))
    nval = evalexp (value, 0, &expok);
  else
    expok = legal_number (value, &nval);
  seconds_value_assigned = expok ? nval : 0;
  gettimeofday (&shellstart, NULL);
  shell_start_time = shellstart.tv_sec;
  return (set_int_value (self, nval, integer_p (self) != 0));
}

static SHELL_VAR *
get_seconds (var)
     SHELL_VAR *var;
{
  time_t time_since_start;
  struct timeval tv;

  gettimeofday(&tv, NULL);
  time_since_start = tv.tv_sec - shell_start_time;
  return (set_int_value (var, seconds_value_assigned + time_since_start, 1));
}

static SHELL_VAR *
init_seconds_var ()
{
  SHELL_VAR *v;

  v = find_variable ("SECONDS");
  if (v)
    {
      if (legal_number (value_cell(v), &seconds_value_assigned) == 0)
	seconds_value_assigned = 0;
    }
  INIT_DYNAMIC_VAR ("SECONDS", (v ? value_cell (v) : (char *)NULL), get_seconds, assign_seconds);
  return v;      
}

/* Functions for $RANDOM and $SRANDOM */

int last_random_value;
static int seeded_subshell = 0;

static SHELL_VAR *
assign_random (self, value, unused, key)
     SHELL_VAR *self;
     char *value;
     arrayind_t unused;
     char *key;
{
  intmax_t seedval;
  int expok;

  if (integer_p (self))
    seedval = evalexp (value, 0, &expok);
  else
    expok = legal_number (value, &seedval);
  if (expok == 0)
    return (self);
  sbrand (seedval);
  if (subshell_environment)
    seeded_subshell = getpid ();
  return (set_int_value (self, seedval, integer_p (self) != 0));
}

int
get_random_number ()
{
  int rv, pid;

  /* Reset for command and process substitution. */
  pid = getpid ();
  if (subshell_environment && seeded_subshell != pid)
    {
      seedrand ();
      seeded_subshell = pid;
    }

  do
    rv = brand ();
  while (rv == last_random_value);

  return (last_random_value = rv);
}

static SHELL_VAR *
get_random (var)
     SHELL_VAR *var;
{
  int rv;

  rv = get_random_number ();
  return (set_int_value (var, rv, 1));
}

static SHELL_VAR *
get_urandom (var)
     SHELL_VAR *var;
{
  u_bits32_t rv;

  rv = get_urandom32 ();
  return (set_int_value (var, rv, 1));
}

static SHELL_VAR *
assign_lineno (var, value, unused, key)
     SHELL_VAR *var;
     char *value;
     arrayind_t unused;
     char *key;
{
  intmax_t new_value;

  if (value == 0 || *value == '\0' || legal_number (value, &new_value) == 0)
    new_value = 0;
  line_number = line_number_base = new_value;
  return (set_int_value (var, line_number, integer_p (var) != 0));
}

/* Function which returns the current line number. */
static SHELL_VAR *
get_lineno (var)
     SHELL_VAR *var;
{
  int ln;

  ln = executing_line_number ();
  return (set_int_value (var, ln, 0));
}

static SHELL_VAR *
assign_subshell (var, value, unused, key)
     SHELL_VAR *var;
     char *value;
     arrayind_t unused;
     char *key;
{
  intmax_t new_value;

  if (value == 0 || *value == '\0' || legal_number (value, &new_value) == 0)
    new_value = 0;
  subshell_level = new_value;
  return var;
}

static SHELL_VAR *
get_subshell (var)
     SHELL_VAR *var;
{
  return (set_int_value (var, subshell_level, 0));
}

static SHELL_VAR *
get_epochseconds (var)
     SHELL_VAR *var;
{
  intmax_t now;

  now = NOW;
  return (set_int_value (var, now, 0));
}

static SHELL_VAR *
get_epochrealtime (var)
     SHELL_VAR *var;
{
  char buf[32];
  struct timeval tv;

  gettimeofday (&tv, NULL);
  snprintf (buf, sizeof (buf), "%u%c%06u", (unsigned)tv.tv_sec,
					   locale_decpoint (),
					   (unsigned)tv.tv_usec);

  return (set_string_value (var, buf, 0));
}

static SHELL_VAR *
get_bashpid (var)
     SHELL_VAR *var;
{
  int pid;

  pid = getpid ();
  return (set_int_value (var, pid, 1));
}

static SHELL_VAR *
get_bash_argv0 (var)
     SHELL_VAR *var;
{
  return (set_string_value (var, dollar_vars[0], 0));
}

static char *static_shell_name = 0;

static SHELL_VAR *
assign_bash_argv0 (var, value, unused, key)
     SHELL_VAR *var;
     char *value;
     arrayind_t unused;
     char *key;
{
  size_t vlen;

  if (value == 0)
    return var;

  FREE (dollar_vars[0]);
  dollar_vars[0] = savestring (value);

  /* Need these gyrations because shell_name isn't dynamically allocated */
  vlen = STRLEN (value);
  static_shell_name = xrealloc (static_shell_name, vlen + 1);
  strcpy (static_shell_name, value);
  
  shell_name = static_shell_name;
  return var;
}

static void
set_argv0 ()
{
  SHELL_VAR *v;

  v = find_variable ("BASH_ARGV0");
  if (v && imported_p (v))
    assign_bash_argv0 (v, value_cell (v), 0, 0);
}
  
static SHELL_VAR *
get_bash_command (var)
     SHELL_VAR *var;
{
  char *p;

  p = the_printed_command_except_trap ? the_printed_command_except_trap : "";
  return (set_string_value (var, p, 0));
}

#if defined (HISTORY)
static SHELL_VAR *
get_histcmd (var)
     SHELL_VAR *var;
{
  int n;

  /* Do the same adjustment here we do in parse.y:prompt_history_number,
     assuming that we are in one of two states: decoding this as part of
     the prompt string, in which case we do not want to assume that the
     command has been saved to the history and the history number incremented,
     or the expansion is part of the current command being executed and has
     already been saved to history and the history number incremented.
     Right now we use EXECUTING as the determinant. */
  n = history_number () - executing;
  return (set_int_value (var, n, 0));
}
#endif

#if defined (READLINE)
/* When this function returns, VAR->value points to malloced memory. */
static SHELL_VAR *
get_comp_wordbreaks (var)
     SHELL_VAR *var;
{
  /* If we don't have anything yet, assign a default value. */
  if (rl_completer_word_break_characters == 0 && bash_readline_initialized == 0)
    enable_hostname_completion (perform_hostname_completion);

  return (set_string_value (var, rl_completer_word_break_characters, 0));
}

/* When this function returns, rl_completer_word_break_characters points to
   malloced memory. */
static SHELL_VAR *
assign_comp_wordbreaks (self, value, unused, key)
     SHELL_VAR *self;
     char *value;
     arrayind_t unused;
     char *key;
{
  if (rl_completer_word_break_characters &&
      rl_completer_word_break_characters != rl_basic_word_break_characters)
    free ((void *)rl_completer_word_break_characters);

  rl_completer_word_break_characters = savestring (value);
  return self;
}
#endif /* READLINE */

#if defined (PUSHD_AND_POPD) && defined (ARRAY_VARS)
static SHELL_VAR *
assign_dirstack (self, value, ind, key)
     SHELL_VAR *self;
     char *value;
     arrayind_t ind;
     char *key;
{
  set_dirstack_element (ind, 1, value);
  return self;
}

static SHELL_VAR *
get_dirstack (self)
     SHELL_VAR *self;
{
  ARRAY *a;
  WORD_LIST *l;

  l = get_directory_stack (0);
  a = array_from_word_list (l);
  array_dispose (array_cell (self));
  dispose_words (l);
  var_setarray (self, a);
  return self;
}
#endif /* PUSHD AND POPD && ARRAY_VARS */

#if defined (ARRAY_VARS)
/* We don't want to initialize the group set with a call to getgroups()
   unless we're asked to, but we only want to do it once. */
static SHELL_VAR *
get_groupset (self)
     SHELL_VAR *self;
{
  register int i;
  int ng;
  ARRAY *a;
  static char **group_set = (char **)NULL;

  if (group_set == 0)
    {
      group_set = get_group_list (&ng);
      a = array_cell (self);
      for (i = 0; i < ng; i++)
	array_insert (a, i, group_set[i]);
    }
  return (self);
}

#  if defined (DEBUGGER)
static SHELL_VAR *
get_bashargcv (self)
     SHELL_VAR *self;
{
  static int self_semaphore = 0;

  /* Backwards compatibility: if we refer to BASH_ARGV or BASH_ARGC at the
     top level without enabling debug mode, and we don't have an instance
     of the variable set, initialize the arg arrays.
     This will already have been done if debugging_mode != 0. */
  if (self_semaphore == 0 && variable_context == 0 && debugging_mode == 0)	/* don't do it for shell functions */
    {
      self_semaphore = 1;
      init_bash_argv ();
      self_semaphore = 0;
    }
  return self;
}
#  endif

static SHELL_VAR *
build_hashcmd (self)
     SHELL_VAR *self;
{
  HASH_TABLE *h;
  int i;
  char *k, *v;
  BUCKET_CONTENTS *item;

  h = assoc_cell (self);
  if (h)
    assoc_dispose (h);

  if (hashed_filenames == 0 || HASH_ENTRIES (hashed_filenames) == 0)
    {
      var_setvalue (self, (char *)NULL);
      return self;
    }

  h = assoc_create (hashed_filenames->nbuckets);
  for (i = 0; i < hashed_filenames->nbuckets; i++)
    {
      for (item = hash_items (i, hashed_filenames); item; item = item->next)
	{
	  k = savestring (item->key);
	  v = pathdata(item)->path;
	  assoc_insert (h, k, v);
	}
    }

  var_setvalue (self, (char *)h);
  return self;
}

static SHELL_VAR *
get_hashcmd (self)
     SHELL_VAR *self;
{
  build_hashcmd (self);
  return (self);
}

static SHELL_VAR *
assign_hashcmd (self, value, ind, key)
     SHELL_VAR *self;
     char *value;
     arrayind_t ind;
     char *key;
{
#if defined (RESTRICTED_SHELL)
  char *full_path;

  if (restricted)
    {
      if (strchr (value, '/'))
	{
	  sh_restricted (value);
	  return (SHELL_VAR *)NULL;
	}
      /* If we are changing the hash table in a restricted shell, make sure the
	 target pathname can be found using a $PATH search. */
      full_path = find_user_command (value);
      if (full_path == 0 || *full_path == 0 || executable_file (full_path) == 0)
	{
	  sh_notfound (value);
	  free (full_path);
	  return ((SHELL_VAR *)NULL);
	}
      free (full_path);
    }
#endif
  phash_insert (key, value, 0, 0);
  return (build_hashcmd (self));
}

#if defined (ALIAS)
static SHELL_VAR *
build_aliasvar (self)
     SHELL_VAR *self;
{
  HASH_TABLE *h;
  int i;
  char *k, *v;
  BUCKET_CONTENTS *item;

  h = assoc_cell (self);
  if (h)
    assoc_dispose (h);

  if (aliases == 0 || HASH_ENTRIES (aliases) == 0)
    {
      var_setvalue (self, (char *)NULL);
      return self;
    }

  h = assoc_create (aliases->nbuckets);
  for (i = 0; i < aliases->nbuckets; i++)
    {
      for (item = hash_items (i, aliases); item; item = item->next)
	{
	  k = savestring (item->key);
	  v = ((alias_t *)(item->data))->value;
	  assoc_insert (h, k, v);
	}
    }

  var_setvalue (self, (char *)h);
  return self;
}

static SHELL_VAR *
get_aliasvar (self)
     SHELL_VAR *self;
{
  build_aliasvar (self);
  return (self);
}

static SHELL_VAR *
assign_aliasvar (self, value, ind, key)
     SHELL_VAR *self;
     char *value;
     arrayind_t ind;
     char *key;
{
  if (legal_alias_name (key, 0) == 0)
    {
       report_error (_("`%s': invalid alias name"), key);
       return (self);
    }
  add_alias (key, value);
  return (build_aliasvar (self));
}
#endif /* ALIAS */

#endif /* ARRAY_VARS */

/* If ARRAY_VARS is not defined, this just returns the name of any
   currently-executing function.  If we have arrays, it's a call stack. */
static SHELL_VAR *
get_funcname (self)
     SHELL_VAR *self;
{
#if ! defined (ARRAY_VARS)
  if (variable_context && this_shell_function)
    return (set_string_value (self, this_shell_function->name, 0));
#endif
  return (self);
}

void
make_funcname_visible (on_or_off)
     int on_or_off;
{
  SHELL_VAR *v;

  v = find_variable ("FUNCNAME");
  if (v == 0 || v->dynamic_value == 0)
    return;

  if (on_or_off)
    VUNSETATTR (v, att_invisible);
  else
    VSETATTR (v, att_invisible);
}

static SHELL_VAR *
init_funcname_var ()
{
  SHELL_VAR *v;

  v = find_variable ("FUNCNAME");
  if (v)
    return v;
#if defined (ARRAY_VARS)
  INIT_DYNAMIC_ARRAY_VAR ("FUNCNAME", get_funcname, null_array_assign);
#else
  INIT_DYNAMIC_VAR ("FUNCNAME", (char *)NULL, get_funcname, null_assign);
#endif
  VSETATTR (v, att_invisible|att_noassign);
  return v;
}

static void
initialize_dynamic_variables ()
{
  SHELL_VAR *v;

  v = init_seconds_var ();

  INIT_DYNAMIC_VAR ("BASH_ARGV0", (char *)NULL, get_bash_argv0, assign_bash_argv0);

  INIT_DYNAMIC_VAR ("BASH_COMMAND", (char *)NULL, get_bash_command, (sh_var_assign_func_t *)NULL);
  INIT_DYNAMIC_VAR ("BASH_SUBSHELL", (char *)NULL, get_subshell, assign_subshell);

  INIT_DYNAMIC_VAR ("RANDOM", (char *)NULL, get_random, assign_random);
  VSETATTR (v, att_integer);
  INIT_DYNAMIC_VAR ("SRANDOM", (char *)NULL, get_urandom, (sh_var_assign_func_t *)NULL);
  VSETATTR (v, att_integer);  
  INIT_DYNAMIC_VAR ("LINENO", (char *)NULL, get_lineno, assign_lineno);
  VSETATTR (v, att_regenerate);

  INIT_DYNAMIC_VAR ("BASHPID", (char *)NULL, get_bashpid, null_assign);
  VSETATTR (v, att_integer);

  INIT_DYNAMIC_VAR ("EPOCHSECONDS", (char *)NULL, get_epochseconds, null_assign);
  VSETATTR (v, att_regenerate);
  INIT_DYNAMIC_VAR ("EPOCHREALTIME", (char *)NULL, get_epochrealtime, null_assign);
  VSETATTR (v, att_regenerate);

#if defined (HISTORY)
  INIT_DYNAMIC_VAR ("HISTCMD", (char *)NULL, get_histcmd, (sh_var_assign_func_t *)NULL);
  VSETATTR (v, att_integer);
#endif

#if defined (READLINE)
  INIT_DYNAMIC_VAR ("COMP_WORDBREAKS", (char *)NULL, get_comp_wordbreaks, assign_comp_wordbreaks);
#endif

#if defined (PUSHD_AND_POPD) && defined (ARRAY_VARS)
  v = init_dynamic_array_var ("DIRSTACK", get_dirstack, assign_dirstack, 0);
#endif /* PUSHD_AND_POPD && ARRAY_VARS */

#if defined (ARRAY_VARS)
  v = init_dynamic_array_var ("GROUPS", get_groupset, null_array_assign, att_noassign);

#  if defined (DEBUGGER)
  v = init_dynamic_array_var ("BASH_ARGC", get_bashargcv, null_array_assign, att_noassign|att_nounset);
  v = init_dynamic_array_var ("BASH_ARGV", get_bashargcv, null_array_assign, att_noassign|att_nounset);
#  endif /* DEBUGGER */
  v = init_dynamic_array_var ("BASH_SOURCE", get_self, null_array_assign, att_noassign|att_nounset);
  v = init_dynamic_array_var ("BASH_LINENO", get_self, null_array_assign, att_noassign|att_nounset);

  v = init_dynamic_assoc_var ("BASH_CMDS", get_hashcmd, assign_hashcmd, att_nofree);
#  if defined (ALIAS)
  v = init_dynamic_assoc_var ("BASH_ALIASES", get_aliasvar, assign_aliasvar, att_nofree);
#  endif
#endif

  v = init_funcname_var ();
}

/* **************************************************************** */
/*								    */
/*		Retrieving variables and values			    */
/*								    */
/* **************************************************************** */

#if 0	/* not yet */
int
var_isset (var)
     SHELL_VAR *var;
{
  return (var->value != 0);
}

int
var_isunset (var)
     SHELL_VAR *var;
{
  return (var->value == 0);
}
#endif

/* How to get a pointer to the shell variable or function named NAME.
   HASHED_VARS is a pointer to the hash table containing the list
   of interest (either variables or functions). */

static SHELL_VAR *
hash_lookup (name, hashed_vars)
     const char *name;
     HASH_TABLE *hashed_vars;
{
  BUCKET_CONTENTS *bucket;

  bucket = hash_search (name, hashed_vars, 0);
  /* If we find the name in HASHED_VARS, set LAST_TABLE_SEARCHED to that
     table. */
  if (bucket)
    last_table_searched = hashed_vars;
  return (bucket ? (SHELL_VAR *)bucket->data : (SHELL_VAR *)NULL);
}

SHELL_VAR *
var_lookup (name, vcontext)
     const char *name;
     VAR_CONTEXT *vcontext;
{
  VAR_CONTEXT *vc;
  SHELL_VAR *v;

  v = (SHELL_VAR *)NULL;
  for (vc = vcontext; vc; vc = vc->down)
    if (v = hash_lookup (name, vc->table))
      break;

  return v;
}

/* Look up the variable entry named NAME.  If SEARCH_TEMPENV is non-zero,
   then also search the temporarily built list of exported variables.
   The lookup order is:
	temporary_env
	shell_variables list
*/

SHELL_VAR *
find_variable_internal (name, flags)
     const char *name;
     int flags;
{
  SHELL_VAR *var;
  int search_tempenv, force_tempenv;
  VAR_CONTEXT *vc;

  var = (SHELL_VAR *)NULL;

  force_tempenv = (flags & FV_FORCETEMPENV);

  /* If explicitly requested, first look in the temporary environment for
     the variable.  This allows constructs such as "foo=x eval 'echo $foo'"
     to get the `exported' value of $foo.  This happens if we are executing
     a function or builtin, or if we are looking up a variable in a
     "subshell environment". */
  search_tempenv = force_tempenv || (expanding_redir == 0 && subshell_environment);

  if (search_tempenv && temporary_env)		
    var = hash_lookup (name, temporary_env);

  if (var == 0)
    {
      if ((flags & FV_SKIPINVISIBLE) == 0)
	var = var_lookup (name, shell_variables);
      else
	{
	  /* essentially var_lookup expanded inline so we can check for
	     att_invisible */
	  for (vc = shell_variables; vc; vc = vc->down)
	    {
	      var = hash_lookup (name, vc->table);
	      if (var && invisible_p (var))
		var = 0;
	      if (var)
		break;
	    }
	}
    }

  if (var == 0)
    return ((SHELL_VAR *)NULL);

  return (var->dynamic_value ? (*(var->dynamic_value)) (var) : var);
}

/* Look up and resolve the chain of nameref variables starting at V all the
   way to NULL or non-nameref. */
SHELL_VAR *
find_variable_nameref (v)
     SHELL_VAR *v;
{
  int level, flags;
  char *newname;
  SHELL_VAR *orig, *oldv;

  level = 0;
  orig = v;
  while (v && nameref_p (v))
    {
      level++;
      if (level > NAMEREF_MAX)
	return ((SHELL_VAR *)0);	/* error message here? */
      newname = nameref_cell (v);
      if (newname == 0 || *newname == '\0')
	return ((SHELL_VAR *)0);
      oldv = v;
      flags = 0;
      if (expanding_redir == 0 && (assigning_in_environment || executing_builtin))
	flags |= FV_FORCETEMPENV;
      /* We don't handle array subscripts here. */
      v = find_variable_internal (newname, flags);
      if (v == orig || v == oldv)
	{
	  internal_warning (_("%s: circular name reference"), orig->name);
#if 1
	  /* XXX - provisional change - circular refs go to
	     global scope for resolution, without namerefs. */
	  if (variable_context && v->context)
	    return (find_global_variable_noref (v->name));
	  else
#endif
	  return ((SHELL_VAR *)0);
	}
    }
  return v;
}

/* Resolve the chain of nameref variables for NAME.  XXX - could change later */
SHELL_VAR *
find_variable_last_nameref (name, vflags)
     const char *name;
     int vflags;
{
  SHELL_VAR *v, *nv;
  char *newname;
  int level, flags;

  nv = v = find_variable_noref (name);
  level = 0;
  while (v && nameref_p (v))
    {
      level++;
      if (level > NAMEREF_MAX)
        return ((SHELL_VAR *)0);	/* error message here? */
      newname = nameref_cell (v);
      if (newname == 0 || *newname == '\0')
	return ((vflags && invisible_p (v)) ? v : (SHELL_VAR *)0);
      nv = v;
      flags = 0;
      if (expanding_redir == 0 && (assigning_in_environment || executing_builtin))
	flags |= FV_FORCETEMPENV;
      /* We don't accommodate array subscripts here. */
      v = find_variable_internal (newname, flags);
    }
  return nv;
}

/* Resolve the chain of nameref variables for NAME.  XXX - could change later */
SHELL_VAR *
find_global_variable_last_nameref (name, vflags)
     const char *name;
     int vflags;
{
  SHELL_VAR *v, *nv;
  char *newname;
  int level;

  nv = v = find_global_variable_noref (name);
  level = 0;
  while (v && nameref_p (v))
    {
      level++;
      if (level > NAMEREF_MAX)
        return ((SHELL_VAR *)0);	/* error message here? */
      newname = nameref_cell (v);
      if (newname == 0 || *newname == '\0')
	return ((vflags && invisible_p (v)) ? v : (SHELL_VAR *)0);
      nv = v;
      /* We don't accommodate array subscripts here. */
      v = find_global_variable_noref (newname);
    }
  return nv;
}

static SHELL_VAR *
find_nameref_at_context (v, vc)
     SHELL_VAR *v;
     VAR_CONTEXT *vc;
{
  SHELL_VAR *nv, *nv2;
  char *newname;
  int level;

  nv = v;
  level = 1;
  while (nv && nameref_p (nv))
    {
      level++;
      if (level > NAMEREF_MAX)
        return (&nameref_maxloop_value);
      newname = nameref_cell (nv);
      if (newname == 0 || *newname == '\0')
        return ((SHELL_VAR *)NULL);      
      nv2 = hash_lookup (newname, vc->table);
      if (nv2 == 0)
        break;
      nv = nv2;
    }
  return nv;
}

/* Do nameref resolution from the VC, which is the local context for some
   function or builtin, `up' the chain to the global variables context.  If
   NVCP is not NULL, return the variable context where we finally ended the
   nameref resolution (so the bind_variable_internal can use the correct
   variable context and hash table). */
static SHELL_VAR *
find_variable_nameref_context (v, vc, nvcp)
     SHELL_VAR *v;
     VAR_CONTEXT *vc;
     VAR_CONTEXT **nvcp;
{
  SHELL_VAR *nv, *nv2;
  VAR_CONTEXT *nvc;

  /* Look starting at the current context all the way `up' */
  for (nv = v, nvc = vc; nvc; nvc = nvc->down)
    {
      nv2 = find_nameref_at_context (nv, nvc);
      if (nv2 == &nameref_maxloop_value)
	return (nv2);			/* XXX */
      if (nv2 == 0)
        continue;
      nv = nv2;
      if (*nvcp)
        *nvcp = nvc;
      if (nameref_p (nv) == 0)
        break;
    }
  return (nameref_p (nv) ? (SHELL_VAR *)NULL : nv);
}

/* Do nameref resolution from the VC, which is the local context for some
   function or builtin, `up' the chain to the global variables context.  If
   NVCP is not NULL, return the variable context where we finally ended the
   nameref resolution (so the bind_variable_internal can use the correct
   variable context and hash table). */
static SHELL_VAR *
find_variable_last_nameref_context (v, vc, nvcp)
     SHELL_VAR *v;
     VAR_CONTEXT *vc;
     VAR_CONTEXT **nvcp;
{
  SHELL_VAR *nv, *nv2;
  VAR_CONTEXT *nvc;

  /* Look starting at the current context all the way `up' */
  for (nv = v, nvc = vc; nvc; nvc = nvc->down)
    {
      nv2 = find_nameref_at_context (nv, nvc);
      if (nv2 == &nameref_maxloop_value)
	return (nv2);			/* XXX */
      if (nv2 == 0)
	continue;
      nv = nv2;
      if (*nvcp)
        *nvcp = nvc;
    }
  return (nameref_p (nv) ? nv : (SHELL_VAR *)NULL);
}

SHELL_VAR *
find_variable_nameref_for_create (name, flags)
     const char *name;
     int flags;
{
  SHELL_VAR *var;

  /* See if we have a nameref pointing to a variable that hasn't been
     created yet. */
  var = find_variable_last_nameref (name, 1);
  if ((flags&1) && var && nameref_p (var) && invisible_p (var))
    {
      internal_warning (_("%s: removing nameref attribute"), name);
      VUNSETATTR (var, att_nameref);
    }
  if (var && nameref_p (var))
    {
      if (legal_identifier (nameref_cell (var)) == 0)
	{
	  sh_invalidid (nameref_cell (var) ? nameref_cell (var) : "");
	  return ((SHELL_VAR *)INVALID_NAMEREF_VALUE);
	}
    }
  return (var);
}

SHELL_VAR *
find_variable_nameref_for_assignment (name, flags)
     const char *name;
     int flags;
{
  SHELL_VAR *var;

  /* See if we have a nameref pointing to a variable that hasn't been
     created yet. */
  var = find_variable_last_nameref (name, 1);
  if (var && nameref_p (var) && invisible_p (var))	/* XXX - flags */
    {
      internal_warning (_("%s: removing nameref attribute"), name);
      VUNSETATTR (var, att_nameref);
    }
  if (var && nameref_p (var))
    {
      if (valid_nameref_value (nameref_cell (var), 1) == 0)
	{
	  sh_invalidid (nameref_cell (var) ? nameref_cell (var) : "");
	  return ((SHELL_VAR *)INVALID_NAMEREF_VALUE);
	}
    }
  return (var);
}

/* If find_variable (name) returns NULL, check that it's not a nameref
   referencing a variable that doesn't exist. If it is, return the new
   name. If not, return the original name. Kind of like the previous
   function, but dealing strictly with names. This takes assignment flags
   so it can deal with the various assignment modes used by `declare'. */
char *
nameref_transform_name (name, flags)
     char *name;
     int flags;
{
  SHELL_VAR *v;
  char *newname;

  v = 0;
  if (flags & ASS_MKLOCAL)
    {
      v = find_variable_last_nameref (name, 1);
      /* If we're making local variables, only follow namerefs that point to
	 non-existent variables at the same variable context. */
      if (v && v->context != variable_context)
	v = 0;
    }
  else if (flags & ASS_MKGLOBAL)
    v = (flags & ASS_CHKLOCAL) ? find_variable_last_nameref (name, 1)
			       : find_global_variable_last_nameref (name, 1);
  if (v && nameref_p (v) && valid_nameref_value (nameref_cell (v), 1))
    return nameref_cell (v);
  return name;
}

/* Find a variable, forcing a search of the temporary environment first */
SHELL_VAR *
find_variable_tempenv (name)
     const char *name;
{
  SHELL_VAR *var;

  var = find_variable_internal (name, FV_FORCETEMPENV);
  if (var && nameref_p (var))
    var = find_variable_nameref (var);
  return (var);
}

/* Find a variable, not forcing a search of the temporary environment first */
SHELL_VAR *
find_variable_notempenv (name)
     const char *name;
{
  SHELL_VAR *var;

  var = find_variable_internal (name, 0);
  if (var && nameref_p (var))
    var = find_variable_nameref (var);
  return (var);
}

SHELL_VAR *
find_global_variable (name)
     const char *name;
{
  SHELL_VAR *var;

  var = var_lookup (name, global_variables);
  if (var && nameref_p (var))
    var = find_variable_nameref (var);	/* XXX - find_global_variable_noref? */

  if (var == 0)
    return ((SHELL_VAR *)NULL);

  return (var->dynamic_value ? (*(var->dynamic_value)) (var) : var);
}

SHELL_VAR *
find_global_variable_noref (name)
     const char *name;
{
  SHELL_VAR *var;

  var = var_lookup (name, global_variables);

  if (var == 0)
    return ((SHELL_VAR *)NULL);

  return (var->dynamic_value ? (*(var->dynamic_value)) (var) : var);
}

SHELL_VAR *
find_shell_variable (name)
     const char *name;
{
  SHELL_VAR *var;

  var = var_lookup (name, shell_variables);
  if (var && nameref_p (var))
    var = find_variable_nameref (var);

  if (var == 0)
    return ((SHELL_VAR *)NULL);

  return (var->dynamic_value ? (*(var->dynamic_value)) (var) : var);
}

/* Look up the variable entry named NAME.  Returns the entry or NULL. */
SHELL_VAR *
find_variable (name)
     const char *name;
{
  SHELL_VAR *v;
  int flags;

  last_table_searched = 0;
  flags = 0;
  if (expanding_redir == 0 && (assigning_in_environment || executing_builtin))
    flags |= FV_FORCETEMPENV;
  v = find_variable_internal (name, flags);
  if (v && nameref_p (v))
    v = find_variable_nameref (v);
  return v;
}

/* Find the first instance of NAME in the variable context chain; return first
   one found without att_invisible set; return 0 if no non-invisible instances
   found. */
SHELL_VAR *
find_variable_no_invisible (name)
     const char *name;
{
  SHELL_VAR *v;
  int flags;

  last_table_searched = 0;
  flags = FV_SKIPINVISIBLE;
  if (expanding_redir == 0 && (assigning_in_environment || executing_builtin))
    flags |= FV_FORCETEMPENV;
  v = find_variable_internal (name, flags);
  if (v && nameref_p (v))
    v = find_variable_nameref (v);
  return v;
}

/* Find the first instance of NAME in the variable context chain; return first
   one found even if att_invisible set. */
SHELL_VAR *
find_variable_for_assignment (name)
     const char *name;
{
  SHELL_VAR *v;
  int flags;

  last_table_searched = 0;
  flags = 0;
  if (expanding_redir == 0 && (assigning_in_environment || executing_builtin))
    flags |= FV_FORCETEMPENV;
  v = find_variable_internal (name, flags);
  if (v && nameref_p (v))
    v = find_variable_nameref (v);
  return v;
}

SHELL_VAR *
find_variable_noref (name)
     const char *name;
{
  SHELL_VAR *v;
  int flags;

  flags = 0;
  if (expanding_redir == 0 && (assigning_in_environment || executing_builtin))
    flags |= FV_FORCETEMPENV;
  v = find_variable_internal (name, flags);
  return v;
}

/* Look up the function entry whose name matches STRING.
   Returns the entry or NULL. */
SHELL_VAR *
find_function (name)
     const char *name;
{
  return (hash_lookup (name, shell_functions));
}

/* Find the function definition for the shell function named NAME.  Returns
   the entry or NULL. */
FUNCTION_DEF *
find_function_def (name)
     const char *name;
{
#if defined (DEBUGGER)
  return ((FUNCTION_DEF *)hash_lookup (name, shell_function_defs));
#else
  return ((FUNCTION_DEF *)0);
#endif
}

/* Return the value of VAR.  VAR is assumed to have been the result of a
   lookup without any subscript, if arrays are compiled into the shell. */
char *
get_variable_value (var)
     SHELL_VAR *var;
{
  if (var == 0)
    return ((char *)NULL);
#if defined (ARRAY_VARS)
  else if (array_p (var))
    return (array_reference (array_cell (var), 0));
  else if (assoc_p (var))
    return (assoc_reference (assoc_cell (var), "0"));
#endif
  else
    return (value_cell (var));
}

/* Return the string value of a variable.  Return NULL if the variable
   doesn't exist.  Don't cons a new string.  This is a potential memory
   leak if the variable is found in the temporary environment, but doesn't
   leak in practice.  Since functions and variables have separate name
   spaces, returns NULL if var_name is a shell function only. */
char *
get_string_value (var_name)
     const char *var_name;
{
  SHELL_VAR *var;

  var = find_variable (var_name);
  return ((var) ? get_variable_value (var) : (char *)NULL);
}

/* This is present for use by the tilde and readline libraries. */
char *
sh_get_env_value (v)
     const char *v;
{
  return get_string_value (v);
}

/* **************************************************************** */
/*								    */
/*		  Creating and setting variables		    */
/*								    */
/* **************************************************************** */

static int
var_sametype (v1, v2)
     SHELL_VAR *v1;
     SHELL_VAR *v2;
{
  if (v1 == 0 || v2 == 0)
    return 0;
#if defined (ARRAY_VARS)
  else if (assoc_p (v1) && assoc_p (v2))
    return 1;
  else if (array_p (v1) && array_p (v2))
    return 1;
  else if (array_p (v1) || array_p (v2))
    return 0;
  else if (assoc_p (v1) || assoc_p (v2))
    return 0;
#endif
  else
    return 1;
}

int
validate_inherited_value (var, type)
     SHELL_VAR *var;
     int type;
{
#if defined (ARRAY_VARS)
  if (type == att_array && assoc_p (var))
    return 0;
  else if (type == att_assoc && array_p (var))
    return 0;
  else
#endif
  return 1;	/* should we run convert_var_to_array here or let the caller? */
}

/* Set NAME to VALUE if NAME has no value. */
SHELL_VAR *
set_if_not (name, value)
     char *name, *value;
{
  SHELL_VAR *v;

  if (shell_variables == 0)
    create_variable_tables ();

  v = find_variable (name);
  if (v == 0)
    v = bind_variable_internal (name, value, global_variables->table, HASH_NOSRCH, 0);
  return (v);
}

/* Create a local variable referenced by NAME. */
SHELL_VAR *
make_local_variable (name, flags)
     const char *name;
     int flags;
{
  SHELL_VAR *new_var, *old_var, *old_ref;
  VAR_CONTEXT *vc;
  int was_tmpvar;
  char *old_value;

  /* We don't want to follow the nameref chain when making local variables; we
     just want to create them. */
  old_ref = find_variable_noref (name);
  if (old_ref && nameref_p (old_ref) == 0)
    old_ref = 0;
  /* local foo; local foo;  is a no-op. */
  old_var = find_variable (name);
  if (old_ref == 0 && old_var && local_p (old_var) && old_var->context == variable_context)
    return (old_var);

  /* local -n foo; local -n foo;  is a no-op. */
  if (old_ref && local_p (old_ref) && old_ref->context == variable_context)
    return (old_ref);

  /* From here on, we want to use the refvar, not the variable it references */
  if (old_ref)
    old_var = old_ref;

  was_tmpvar = old_var && tempvar_p (old_var);
  /* If we're making a local variable in a shell function, the temporary env
     has already been merged into the function's variable context stack.  We
     can assume that a temporary var in the same context appears in the same
     VAR_CONTEXT and can safely be returned without creating a new variable
     (which results in duplicate names in the same VAR_CONTEXT->table */
  /* We can't just test tmpvar_p because variables in the temporary env given
     to a shell function appear in the function's local variable VAR_CONTEXT
     but retain their tempvar attribute.  We want temporary variables that are
     found in temporary_env, hence the test for last_table_searched, which is
     set in hash_lookup and only (so far) checked here. */
  if (was_tmpvar && old_var->context == variable_context && last_table_searched != temporary_env)
    {
      VUNSETATTR (old_var, att_invisible);	/* XXX */
      /* We still want to flag this variable as local, though, and set things
         up so that it gets treated as a local variable. */
      new_var = old_var;
      /* Since we found the variable in a temporary environment, this will
	 succeed. */
      for (vc = shell_variables; vc; vc = vc->down)
	if (vc_isfuncenv (vc) && vc->scope == variable_context)
	  break;
      goto set_local_var_flags;

      return (old_var);
    }

  /* If we want to change to "inherit the old variable's value" semantics,
     here is where to save the old value. */
  old_value = was_tmpvar ? value_cell (old_var) : (char *)NULL;

  for (vc = shell_variables; vc; vc = vc->down)
    if (vc_isfuncenv (vc) && vc->scope == variable_context)
      break;

  if (vc == 0)
    {
      internal_error (_("make_local_variable: no function context at current scope"));
      return ((SHELL_VAR *)NULL);
    }
  else if (vc->table == 0)
    vc->table = hash_create (TEMPENV_HASH_BUCKETS);

  /* Since this is called only from the local/declare/typeset code, we can
     call builtin_error here without worry (of course, it will also work
     for anything that sets this_command_name).  Variables with the `noassign'
     attribute may not be made local.  The test against old_var's context
     level is to disallow local copies of readonly global variables (since I
     believe that this could be a security hole).  Readonly copies of calling
     function local variables are OK. */
  if (old_var && (noassign_p (old_var) ||
		 (readonly_p (old_var) && old_var->context == 0)))
    {
      if (readonly_p (old_var))
	sh_readonly (name);
      else if (noassign_p (old_var))
	builtin_error (_("%s: variable may not be assigned value"), name);
#if 0
      /* Let noassign variables through with a warning */
      if (readonly_p (old_var))
#endif
	return ((SHELL_VAR *)NULL);
    }

  if (old_var == 0)
    new_var = make_new_variable (name, vc->table);
  else
    {
      new_var = make_new_variable (name, vc->table);

      /* If we found this variable in one of the temporary environments,
	 inherit its value.  Watch to see if this causes problems with
	 things like `x=4 local x'. XXX - see above for temporary env
	 variables with the same context level as variable_context */
      /* XXX - we should only do this if the variable is not an array. */
      /* If we want to change the local variable semantics to "inherit
	 the old variable's value" here is where to set it.  And we would
	 need to use copy_variable (currently unused) to do it for all
	 possible variable values. */
      if (was_tmpvar)
	var_setvalue (new_var, savestring (old_value));
      else if (localvar_inherit || (flags & MKLOC_INHERIT))
	{
	  /* This may not make sense for nameref variables that are shadowing
	     variables with the same name, but we don't know that yet. */
#if defined (ARRAY_VARS)
	  if (assoc_p (old_var))
	    var_setassoc (new_var, assoc_copy (assoc_cell (old_var)));
	  else if (array_p (old_var))
	    var_setarray (new_var, array_copy (array_cell (old_var)));
	  else if (value_cell (old_var))
#else
	  if (value_cell (old_var))
#endif
	    var_setvalue (new_var, savestring (value_cell (old_var)));
	  else
	    var_setvalue (new_var, (char *)NULL);
	}

      if (localvar_inherit || (flags & MKLOC_INHERIT))
	{
	  /* It doesn't make sense to inherit the nameref attribute */
	  new_var->attributes = old_var->attributes & ~att_nameref;
	  new_var->dynamic_value = old_var->dynamic_value;
	  new_var->assign_func = old_var->assign_func;
	}
      else
	/* We inherit the export attribute, but no others. */
	new_var->attributes = exported_p (old_var) ? att_exported : 0;
    }

set_local_var_flags:
  vc->flags |= VC_HASLOCAL;

  new_var->context = variable_context;
  VSETATTR (new_var, att_local);

  if (ifsname (name))
    setifs (new_var);

  /* value_cell will be 0 if localvar_inherit == 0 or there was no old variable
     with the same name or the old variable was invisible */
  if (was_tmpvar == 0 && value_cell (new_var) == 0)
    VSETATTR (new_var, att_invisible);	/* XXX */
  return (new_var);
}

/* Create a new shell variable with name NAME. */
static SHELL_VAR *
new_shell_variable (name)
     const char *name;
{
  SHELL_VAR *entry;

  entry = (SHELL_VAR *)xmalloc (sizeof (SHELL_VAR));

  entry->name = savestring (name);
  var_setvalue (entry, (char *)NULL);
  CLEAR_EXPORTSTR (entry);

  entry->dynamic_value = (sh_var_value_func_t *)NULL;
  entry->assign_func = (sh_var_assign_func_t *)NULL;

  entry->attributes = 0;

  /* Always assume variables are to be made at toplevel!
     make_local_variable has the responsibility of changing the
     variable context. */
  entry->context = 0;

  return (entry);
}

/* Create a new shell variable with name NAME and add it to the hash table
   TABLE. */
static SHELL_VAR *
make_new_variable (name, table)
     const char *name;
     HASH_TABLE *table;
{
  SHELL_VAR *entry;
  BUCKET_CONTENTS *elt;

  entry = new_shell_variable (name);

  /* Make sure we have a shell_variables hash table to add to. */
  if (shell_variables == 0)
    create_variable_tables ();

  elt = hash_insert (savestring (name), table, HASH_NOSRCH);
  elt->data = (PTR_T)entry;

  return entry;
}

#if defined (ARRAY_VARS)
SHELL_VAR *
make_new_array_variable (name)
     char *name;
{
  SHELL_VAR *entry;
  ARRAY *array;

  entry = make_new_variable (name, global_variables->table);
  array = array_create ();

  var_setarray (entry, array);
  VSETATTR (entry, att_array);
  return entry;
}

SHELL_VAR *
make_local_array_variable (name, flags)
     char *name;
     int flags;
{
  SHELL_VAR *var;
  ARRAY *array;
  int assoc_ok;

  assoc_ok = flags & MKLOC_ASSOCOK;

  var = make_local_variable (name, flags & MKLOC_INHERIT);	/* XXX for now */
  /* If ASSOC_OK is non-zero, assume that we are ok with letting an assoc
     variable return to the caller without converting it. The caller will
     either flag an error or do the conversion itself. */
  if (var == 0 || array_p (var) || (assoc_ok && assoc_p (var)))
    return var;

  /* Validate any value we inherited from a variable instance at a previous
     scope and discard anything that's invalid. */
  if (localvar_inherit && assoc_p (var))
    {
      internal_warning (_("%s: cannot inherit value from incompatible type"), name);
      VUNSETATTR (var, att_assoc);
      dispose_variable_value (var);
      array = array_create ();
      var_setarray (var, array);
    }
  else if (localvar_inherit)
    var = convert_var_to_array (var);		/* XXX */
  else
    {
      dispose_variable_value (var);
      array = array_create ();
      var_setarray (var, array);
    }

  VSETATTR (var, att_array);
  return var;
}

SHELL_VAR *
make_new_assoc_variable (name)
     char *name;
{
  SHELL_VAR *entry;
  HASH_TABLE *hash;

  entry = make_new_variable (name, global_variables->table);
  hash = assoc_create (ASSOC_HASH_BUCKETS);

  var_setassoc (entry, hash);
  VSETATTR (entry, att_assoc);
  return entry;
}

SHELL_VAR *
make_local_assoc_variable (name, flags)
     char *name;
     int flags;
{
  SHELL_VAR *var;
  HASH_TABLE *hash;
  int array_ok;

  array_ok = flags & MKLOC_ARRAYOK;

  var = make_local_variable (name, flags & MKLOC_INHERIT);	/* XXX for now */
  /* If ARRAY_OK is non-zero, assume that we are ok with letting an array
     variable return to the caller without converting it. The caller will
     either flag an error or do the conversion itself. */
  if (var == 0 || assoc_p (var) || (array_ok && array_p (var)))
    return var;

  /* Validate any value we inherited from a variable instance at a previous
     scope and discard anything that's invalid. */
  if (localvar_inherit && array_p (var))
    {
      internal_warning (_("%s: cannot inherit value from incompatible type"), name);
      VUNSETATTR (var, att_array);
      dispose_variable_value (var);
      hash = assoc_create (ASSOC_HASH_BUCKETS);
      var_setassoc (var, hash);
    }
  else if (localvar_inherit)
    var = convert_var_to_assoc (var);		/* XXX */
  else
    {
      dispose_variable_value (var);
      hash = assoc_create (ASSOC_HASH_BUCKETS);
      var_setassoc (var, hash);
    }

  VSETATTR (var, att_assoc);
  return var;
}
#endif

char *
make_variable_value (var, value, flags)
     SHELL_VAR *var;
     char *value;
     int flags;
{
  char *retval, *oval;
  intmax_t lval, rval;
  int expok, olen, op;

  /* If this variable has had its type set to integer (via `declare -i'),
     then do expression evaluation on it and store the result.  The
     functions in expr.c (evalexp()) and bind_int_variable() are responsible
     for turning off the integer flag if they don't want further
     evaluation done.  Callers that find it inconvenient to do this can set
     the ASS_NOEVAL flag.  For the special case of arithmetic expression
     evaluation, the caller can set ASS_NOLONGJMP to avoid jumping out to
     top_level. */
  if ((flags & ASS_NOEVAL) == 0 && integer_p (var))
    {
      if (flags & ASS_APPEND)
	{
	  oval = value_cell (var);
	  lval = evalexp (oval, 0, &expok);	/* ksh93 seems to do this */
	  if (expok == 0)
	    {
	      if (flags & ASS_NOLONGJMP)
		goto make_value;
	      else
		{
		  top_level_cleanup ();
		  jump_to_top_level (DISCARD);
		}
	    }
	}
      rval = evalexp (value, 0, &expok);
      if (expok == 0)
	{
	  if (flags & ASS_NOLONGJMP)
	    goto make_value;
	  else
	    {
	      top_level_cleanup ();
	      jump_to_top_level (DISCARD);
	    }
	}
      /* This can be fooled if the variable's value changes while evaluating
	 `rval'.  We can change it if we move the evaluation of lval to here. */
      if (flags & ASS_APPEND)
	rval += lval;
      retval = itos (rval);
    }
#if defined (CASEMOD_ATTRS)
  else if ((flags & ASS_NOEVAL) == 0 && (capcase_p (var) || uppercase_p (var) || lowercase_p (var)))
    {
      if (flags & ASS_APPEND)
	{
	  oval = get_variable_value (var);
	  if (oval == 0)	/* paranoia */
	    oval = "";
	  olen = STRLEN (oval);
	  retval = (char *)xmalloc (olen + (value ? STRLEN (value) : 0) + 1);
	  strcpy (retval, oval);
	  if (value)
	    strcpy (retval+olen, value);
	}
      else if (*value)
	retval = savestring (value);
      else
	{
	  retval = (char *)xmalloc (1);
	  retval[0] = '\0';
	}
      op = capcase_p (var) ? CASE_CAPITALIZE
			 : (uppercase_p (var) ? CASE_UPPER : CASE_LOWER);
      oval = sh_modcase (retval, (char *)0, op);
      free (retval);
      retval = oval;
    }
#endif /* CASEMOD_ATTRS */
  else if (value)
    {
make_value:
      if (flags & ASS_APPEND)
	{
	  oval = get_variable_value (var);
	  if (oval == 0)	/* paranoia */
	    oval = "";
	  olen = STRLEN (oval);
	  retval = (char *)xmalloc (olen + (value ? STRLEN (value) : 0) + 1);
	  strcpy (retval, oval);
	  if (value)
	    strcpy (retval+olen, value);
	}
      else if (*value)
	retval = savestring (value);
      else
	{
	  retval = (char *)xmalloc (1);
	  retval[0] = '\0';
	}
    }
  else
    retval = (char *)NULL;

  return retval;
}

/* If we can optimize appending to string variables, say so */
static int
can_optimize_assignment (entry, value, aflags)
     SHELL_VAR *entry;
     char *value;
     int aflags;
{
  if ((aflags & ASS_APPEND) == 0)
    return 0;
#if defined (ARRAY_VARS)
  if (array_p (entry) || assoc_p (entry))
    return 0;
#endif
  if (integer_p (entry) || uppercase_p (entry) || lowercase_p (entry) || capcase_p (entry))
    return 0;
  if (readonly_p (entry) || noassign_p (entry))
    return 0;
  return 1;
}

/* right now we optimize appends to string variables */
static SHELL_VAR *
optimized_assignment (entry, value, aflags)
     SHELL_VAR *entry;
     char *value;
     int aflags;
{
  size_t len, vlen;
  char *v, *new;

  v = value_cell (entry);
  len = STRLEN (v);
  vlen = STRLEN (value);

  new = (char *)xrealloc (v, len + vlen + 8);	/* for now */
  if (vlen == 1)
    {
      new[len] = *value;
      new[len+1] = '\0';
    }
  else
    strcpy (new + len, value);
  var_setvalue (entry, new);
  return entry;
}

/* Bind a variable NAME to VALUE in the HASH_TABLE TABLE, which may be the
   temporary environment (but usually is not).  HFLAGS controls how NAME
   is looked up in TABLE; AFLAGS controls how VALUE is assigned */
static SHELL_VAR *
bind_variable_internal (name, value, table, hflags, aflags)
     const char *name;
     char *value;
     HASH_TABLE *table;
     int hflags, aflags;
{
  char *newval, *tname;
  SHELL_VAR *entry, *tentry;

  entry = (hflags & HASH_NOSRCH) ? (SHELL_VAR *)NULL : hash_lookup (name, table);
  /* Follow the nameref chain here if this is the global variables table */
  if (entry && nameref_p (entry) && (invisible_p (entry) == 0) && table == global_variables->table)
    {
      entry = find_global_variable (entry->name);
      /* Let's see if we have a nameref referencing a variable that hasn't yet
	 been created. */
      if (entry == 0)
	entry = find_variable_last_nameref (name, 0);	/* XXX */
      if (entry == 0)					/* just in case */
        return (entry);
    }

  /* The first clause handles `declare -n ref; ref=x;' or `declare -n ref;
     declare -n ref' */
  if (entry && invisible_p (entry) && nameref_p (entry))
    {
      if ((aflags & ASS_FORCE) == 0 && value && valid_nameref_value (value, 0) == 0)
	{
	  sh_invalidid (value);
	  return ((SHELL_VAR *)NULL);
	}
      goto assign_value;
    }
  else if (entry && nameref_p (entry))
    {
      newval = nameref_cell (entry);	/* XXX - newval can't be NULL here */
      if (valid_nameref_value (newval, 0) == 0)
	{
	  sh_invalidid (newval);
	  return ((SHELL_VAR *)NULL);
	}
#if defined (ARRAY_VARS)
      /* declare -n foo=x[2] ; foo=bar */
      if (valid_array_reference (newval, 0))
	{
	  tname = array_variable_name (newval, 0, (char **)0, (int *)0);
	  if (tname && (tentry = find_variable_noref (tname)) && nameref_p (tentry))
	    {
	      /* nameref variables can't be arrays */
	      internal_warning (_("%s: removing nameref attribute"), name_cell (tentry));
	      FREE (value_cell (tentry));		/* XXX - bash-4.3 compat */
	      var_setvalue (tentry, (char *)NULL);
	      VUNSETATTR (tentry, att_nameref);
	    }
	  free (tname);

	  /* entry == nameref variable; tentry == array variable;
	     newval == x[2]; value = bar
	     We don't need to call make_variable_value here, since
	     assign_array_element will eventually do it itself based on
	     newval and aflags. */

	  entry = assign_array_element (newval, value, aflags|ASS_NAMEREF, (array_eltstate_t *)0);
	  if (entry == 0)
	    return entry;
	}
      else
#endif
	{
	  entry = make_new_variable (newval, table);
	  var_setvalue (entry, make_variable_value (entry, value, aflags));
	}
    }
  else if (entry == 0)
    {
      entry = make_new_variable (name, table);
      var_setvalue (entry, make_variable_value (entry, value, aflags)); /* XXX */
    }
  else if (entry->assign_func)	/* array vars have assign functions now */
    {
      if ((readonly_p (entry) && (aflags & ASS_FORCE) == 0) || noassign_p (entry))
	{
	  if (readonly_p (entry))
	    err_readonly (name_cell (entry));
	  return (entry);
	}

      INVALIDATE_EXPORTSTR (entry);
      newval = (aflags & ASS_APPEND) ? make_variable_value (entry, value, aflags) : value;
      if (assoc_p (entry))
	entry = (*(entry->assign_func)) (entry, newval, -1, savestring ("0"));
      else if (array_p (entry))
	entry = (*(entry->assign_func)) (entry, newval, 0, 0);
      else
	entry = (*(entry->assign_func)) (entry, newval, -1, 0);
      if (newval != value)
	free (newval);
      return (entry);
    }
  else
    {
assign_value:
      if ((readonly_p (entry) && (aflags & ASS_FORCE) == 0) || noassign_p (entry))
	{
	  if (readonly_p (entry))
	    err_readonly (name_cell (entry));
	  return (entry);
	}

      /* Variables which are bound are visible. */
      VUNSETATTR (entry, att_invisible);

      /* If we can optimize the assignment, do so and return.  Right now, we
	 optimize appends to string variables. */
      if (can_optimize_assignment (entry, value, aflags))
	{
	  INVALIDATE_EXPORTSTR (entry);
	  optimized_assignment (entry, value, aflags);

	  if (mark_modified_vars)
	    VSETATTR (entry, att_exported);

	  if (exported_p (entry))
	    array_needs_making = 1;

	  return (entry);
	}

#if defined (ARRAY_VARS)
      if (assoc_p (entry) || array_p (entry))
        newval = make_array_variable_value (entry, 0, "0", value, aflags);
      else
#endif
      newval = make_variable_value (entry, value, aflags);	/* XXX */

      /* Invalidate any cached export string */
      INVALIDATE_EXPORTSTR (entry);

#if defined (ARRAY_VARS)
      /* XXX -- this bears looking at again -- XXX */
      /* If an existing array variable x is being assigned to with x=b or
	 `read x' or something of that nature, silently convert it to
	 x[0]=b or `read x[0]'. */
      if (assoc_p (entry))
	{
	  assoc_insert (assoc_cell (entry), savestring ("0"), newval);
	  free (newval);
	}
      else if (array_p (entry))
	{
	  array_insert (array_cell (entry), 0, newval);
	  free (newval);
	}
      else
#endif
	{
	  FREE (value_cell (entry));
	  var_setvalue (entry, newval);
	}
    }

  if (mark_modified_vars)
    VSETATTR (entry, att_exported);

  if (exported_p (entry))
    array_needs_making = 1;

  return (entry);
}
	
/* Bind a variable NAME to VALUE.  This conses up the name
   and value strings.  If we have a temporary environment, we bind there
   first, then we bind into shell_variables. */

SHELL_VAR *
bind_variable (name, value, flags)
     const char *name;
     char *value;
     int flags;
{
  SHELL_VAR *v, *nv;
  VAR_CONTEXT *vc, *nvc;

  if (shell_variables == 0)
    create_variable_tables ();

  /* If we have a temporary environment, look there first for the variable,
     and, if found, modify the value there before modifying it in the
     shell_variables table.  This allows sourced scripts to modify values
     given to them in a temporary environment while modifying the variable
     value that the caller sees. */
  if (temporary_env && value)		/* XXX - can value be null here? */
    bind_tempenv_variable (name, value);

  /* XXX -- handle local variables here. */
  for (vc = shell_variables; vc; vc = vc->down)
    {
      if (vc_isfuncenv (vc) || vc_isbltnenv (vc))
	{
	  v = hash_lookup (name, vc->table);
	  nvc = vc;
	  if (v && nameref_p (v))
	    {
	      /* This starts at the context where we found the nameref. If we
		 want to start the name resolution over again at the original
		 context, this is where we need to change it */
	      nv = find_variable_nameref_context (v, vc, &nvc);
	      if (nv == 0)
		{
		  nv = find_variable_last_nameref_context (v, vc, &nvc);
		  if (nv && nameref_p (nv))
		    {
		      /* If this nameref variable doesn't have a value yet,
			 set the value.  Otherwise, assign using the value as
			 normal. */
		      if (nameref_cell (nv) == 0)
			return (bind_variable_internal (nv->name, value, nvc->table, 0, flags));
#if defined (ARRAY_VARS)
		      else if (valid_array_reference (nameref_cell (nv), 0))
			return (assign_array_element (nameref_cell (nv), value, flags, (array_eltstate_t *)0));
		      else
#endif
		      return (bind_variable_internal (nameref_cell (nv), value, nvc->table, 0, flags));
		    }
		  else if (nv == &nameref_maxloop_value)
		    {
		      internal_warning (_("%s: circular name reference"), v->name);
		      return (bind_global_variable (v->name, value, flags));
		    }
		  else
		    v = nv;
		}
	      else if (nv == &nameref_maxloop_value)
		{
		  internal_warning (_("%s: circular name reference"), v->name);
		  return (bind_global_variable (v->name, value, flags));
		}
	      else
	        v = nv;
	    }
	  if (v)
	    return (bind_variable_internal (v->name, value, nvc->table, 0, flags));
	}
    }
  /* bind_variable_internal will handle nameref resolution in this case */
  return (bind_variable_internal (name, value, global_variables->table, 0, flags));
}

SHELL_VAR *
bind_global_variable (name, value, flags)
     const char *name;
     char *value;
     int flags;
{
  if (shell_variables == 0)
    create_variable_tables ();

  /* bind_variable_internal will handle nameref resolution in this case */
  return (bind_variable_internal (name, value, global_variables->table, 0, flags));
}

static SHELL_VAR *
bind_invalid_envvar (name, value, flags)
     const char *name;
     char *value;
     int flags;
{
  if (invalid_env == 0)
    invalid_env = hash_create (64);	/* XXX */
  return (bind_variable_internal (name, value, invalid_env, HASH_NOSRCH, flags));
}

/* Make VAR, a simple shell variable, have value VALUE.  Once assigned a
   value, variables are no longer invisible.  This is a duplicate of part
   of the internals of bind_variable.  If the variable is exported, or
   all modified variables should be exported, mark the variable for export
   and note that the export environment needs to be recreated. */
SHELL_VAR *
bind_variable_value (var, value, aflags)
     SHELL_VAR *var;
     char *value;
     int aflags;
{
  char *t;
  int invis;

  invis = invisible_p (var);
  VUNSETATTR (var, att_invisible);

  if (var->assign_func)
    {
      /* If we're appending, we need the old value, so use
	 make_variable_value */
      t = (aflags & ASS_APPEND) ? make_variable_value (var, value, aflags) : value;
      (*(var->assign_func)) (var, t, -1, 0);
      if (t != value && t)
	free (t);      
    }
  else
    {
      t = make_variable_value (var, value, aflags);
      if ((aflags & (ASS_NAMEREF|ASS_FORCE)) == ASS_NAMEREF && check_selfref (name_cell (var), t, 0))
	{
	  if (variable_context)
	    internal_warning (_("%s: circular name reference"), name_cell (var));
	  else
	    {
	      internal_error (_("%s: nameref variable self references not allowed"), name_cell (var));
	      free (t);
	      if (invis)
		VSETATTR (var, att_invisible);	/* XXX */
	      return ((SHELL_VAR *)NULL);
	    }
	}
      if ((aflags & ASS_NAMEREF) && (valid_nameref_value (t, 0) == 0))
	{
	  free (t);
	  if (invis)
	    VSETATTR (var, att_invisible);	/* XXX */
	  return ((SHELL_VAR *)NULL);
	}
      FREE (value_cell (var));
      var_setvalue (var, t);
    }

  INVALIDATE_EXPORTSTR (var);

  if (mark_modified_vars)
    VSETATTR (var, att_exported);

  if (exported_p (var))
    array_needs_making = 1;

  return (var);
}

/* Bind/create a shell variable with the name LHS to the RHS.
   This creates or modifies a variable such that it is an integer.

   This used to be in expr.c, but it is here so that all of the
   variable binding stuff is localized.  Since we don't want any
   recursive evaluation from bind_variable() (possible without this code,
   since bind_variable() calls the evaluator for variables with the integer
   attribute set), we temporarily turn off the integer attribute for each
   variable we set here, then turn it back on after binding as necessary. */

SHELL_VAR *
bind_int_variable (lhs, rhs, flags)
     char *lhs, *rhs;
     int flags;
{
  register SHELL_VAR *v;
  int isint, isarr, implicitarray, vflags, avflags;

  isint = isarr = implicitarray = 0;
#if defined (ARRAY_VARS)
  /* Don't rely on VA_NOEXPAND being 1, set it explicitly */
  vflags = (flags & ASS_NOEXPAND) ? VA_NOEXPAND : 0;
  if (flags & ASS_ONEWORD)
    vflags |= VA_ONEWORD;
  if (valid_array_reference (lhs, vflags))
    {
      isarr = 1;
      avflags = 0;
      /* Common code to translate between assignment and reference flags. */
      if (flags & ASS_NOEXPAND)
	avflags |= AV_NOEXPAND;
      if (flags & ASS_ONEWORD)
	avflags |= AV_ONEWORD;
      v = array_variable_part (lhs, avflags, (char **)0, (int *)0);
    }
  else if (legal_identifier (lhs) == 0)
    {
      sh_invalidid (lhs);
      return ((SHELL_VAR *)NULL);      
    }
  else
#endif
    v = find_variable (lhs);

  if (v)
    {
      isint = integer_p (v);
      VUNSETATTR (v, att_integer);
#if defined (ARRAY_VARS)
      if (array_p (v) && isarr == 0)
	implicitarray = 1;
#endif
    }

#if defined (ARRAY_VARS)
  if (isarr)
    v = assign_array_element (lhs, rhs, flags, (array_eltstate_t *)0);
  else if (implicitarray)
    v = bind_array_variable (lhs, 0, rhs, 0);	/* XXX - check on flags */
  else
#endif
    v = bind_variable (lhs, rhs, 0);	/* why not use bind_variable_value? */

  if (v)
    {
      if (isint)
	VSETATTR (v, att_integer);
      VUNSETATTR (v, att_invisible);
    }

  if (v && nameref_p (v))
    internal_warning (_("%s: assigning integer to name reference"), lhs);
     
  return (v);
}

SHELL_VAR *
bind_var_to_int (var, val, flags)
     char *var;
     intmax_t val;
     int flags;
{
  char ibuf[INT_STRLEN_BOUND (intmax_t) + 1], *p;

  p = fmtulong (val, 10, ibuf, sizeof (ibuf), 0);
  return (bind_int_variable (var, p, flags));
}

/* Do a function binding to a variable.  You pass the name and
   the command to bind to.  This conses the name and command. */
SHELL_VAR *
bind_function (name, value)
     const char *name;
     COMMAND *value;
{
  SHELL_VAR *entry;

  entry = find_function (name);
  if (entry == 0)
    {
      BUCKET_CONTENTS *elt;

      elt = hash_insert (savestring (name), shell_functions, HASH_NOSRCH);
      entry = new_shell_variable (name);
      elt->data = (PTR_T)entry;
    }
  else
    INVALIDATE_EXPORTSTR (entry);

  if (var_isset (entry))
    dispose_command (function_cell (entry));

  if (value)
    var_setfunc (entry, copy_command (value));
  else
    var_setfunc (entry, 0);

  VSETATTR (entry, att_function);

  if (mark_modified_vars)
    VSETATTR (entry, att_exported);

  VUNSETATTR (entry, att_invisible);		/* Just to be sure */

  if (exported_p (entry))
    array_needs_making = 1;

#if defined (PROGRAMMABLE_COMPLETION)
  set_itemlist_dirty (&it_functions);
#endif

  return (entry);
}

#if defined (DEBUGGER)
/* Bind a function definition, which includes source file and line number
   information in addition to the command, into the FUNCTION_DEF hash table.
   If (FLAGS & 1), overwrite any existing definition. If FLAGS == 0, leave
   any existing definition alone. */
void
bind_function_def (name, value, flags)
     const char *name;
     FUNCTION_DEF *value;
     int flags;
{
  FUNCTION_DEF *entry;
  BUCKET_CONTENTS *elt;
  COMMAND *cmd;

  entry = find_function_def (name);
  if (entry && (flags & 1))
    {
      dispose_function_def_contents (entry);
      entry = copy_function_def_contents (value, entry);
    }
  else if (entry)
    return;
  else
    {
      cmd = value->command;
      value->command = 0;
      entry = copy_function_def (value);
      value->command = cmd;

      elt = hash_insert (savestring (name), shell_function_defs, HASH_NOSRCH);
      elt->data = (PTR_T *)entry;
    }
}
#endif /* DEBUGGER */

/* Add STRING, which is of the form foo=bar, to the temporary environment
   HASH_TABLE (temporary_env).  The functions in execute_cmd.c are
   responsible for moving the main temporary env to one of the other
   temporary environments.  The expansion code in subst.c calls this. */
int
assign_in_env (word, flags)
     WORD_DESC *word;
     int flags;
{
  int offset, aflags;
  char *name, *temp, *value, *newname;
  SHELL_VAR *var;
  const char *string;

  string = word->word;

  aflags = 0;
  offset = assignment (string, 0);
  newname = name = savestring (string);
  value = (char *)NULL;

  if (name[offset] == '=')
    {
      name[offset] = 0;

      /* don't ignore the `+' when assigning temporary environment */
      if (name[offset - 1] == '+')
	{
	  name[offset - 1] = '\0';
	  aflags |= ASS_APPEND;
	}

      if (legal_identifier (name) == 0)
	{
	  sh_invalidid (name);
	  free (name);
	  return (0);
	}
  
      var = find_variable (name);
      if (var == 0)
	{
	  var = find_variable_last_nameref (name, 1);
	  /* If we're assigning a value to a nameref variable in the temp
	     environment, and the value of the nameref is valid for assignment,
	     but the variable does not already exist, assign to the nameref
	     target and add the target to the temporary environment.  This is
	     what ksh93 does */
	  /* We use 2 in the call to valid_nameref_value because we don't want
	     to allow array references here at all (newname will be used to
	     create a variable directly below) */
	  if (var && nameref_p (var) && valid_nameref_value (nameref_cell (var), 2))
	    {
	      newname = nameref_cell (var);
	      var = 0;		/* don't use it for append */
	    }
	}
      else
        newname = name_cell (var);	/* no-op if not nameref */
	  
      if (var && (readonly_p (var) || noassign_p (var)))
	{
	  if (readonly_p (var))
	    err_readonly (name);
	  free (name);
  	  return (0);
	}
      temp = name + offset + 1;

      value = expand_assignment_string_to_string (temp, 0);

      if (var && (aflags & ASS_APPEND))
	{
	  if (value == 0)
	    {
	      value = (char *)xmalloc (1);	/* like do_assignment_internal */
	      value[0] = '\0';
	    }
	  temp = make_variable_value (var, value, aflags);
	  FREE (value);
	  value = temp;
	}
    }

  if (temporary_env == 0)
    temporary_env = hash_create (TEMPENV_HASH_BUCKETS);

  var = hash_lookup (newname, temporary_env);
  if (var == 0)
    var = make_new_variable (newname, temporary_env);
  else
    FREE (value_cell (var));

  if (value == 0)
    {
      value = (char *)xmalloc (1);	/* see above */
      value[0] = '\0';
    }

  var_setvalue (var, value);
  var->attributes |= (att_exported|att_tempvar);
  var->context = variable_context;	/* XXX */

  INVALIDATE_EXPORTSTR (var);
  var->exportstr = mk_env_string (newname, value, 0);

  array_needs_making = 1;

  if (flags)
    {
      if (STREQ (newname, "POSIXLY_CORRECT") || STREQ (newname, "POSIX_PEDANDTIC"))
	save_posix_options ();		/* XXX one level of saving right now */
      stupidly_hack_special_variables (newname);
    }

  if (echo_command_at_execute)
    /* The Korn shell prints the `+ ' in front of assignment statements,
	so we do too. */
    xtrace_print_assignment (name, value, 0, 1);

  free (name);
  return 1;
}

/* **************************************************************** */
/*								    */
/*			Copying variables			    */
/*								    */
/* **************************************************************** */

#ifdef INCLUDE_UNUSED
/* Copy VAR to a new data structure and return that structure. */
SHELL_VAR *
copy_variable (var)
     SHELL_VAR *var;
{
  SHELL_VAR *copy = (SHELL_VAR *)NULL;

  if (var)
    {
      copy = (SHELL_VAR *)xmalloc (sizeof (SHELL_VAR));

      copy->attributes = var->attributes;
      copy->name = savestring (var->name);

      if (function_p (var))
	var_setfunc (copy, copy_command (function_cell (var)));
#if defined (ARRAY_VARS)
      else if (array_p (var))
	var_setarray (copy, array_copy (array_cell (var)));
      else if (assoc_p (var))
	var_setassoc (copy, assoc_copy (assoc_cell (var)));
#endif
      else if (nameref_cell (var))	/* XXX - nameref */
	var_setref (copy, savestring (nameref_cell (var)));
      else if (value_cell (var))	/* XXX - nameref */
	var_setvalue (copy, savestring (value_cell (var)));
      else
	var_setvalue (copy, (char *)NULL);

      copy->dynamic_value = var->dynamic_value;
      copy->assign_func = var->assign_func;

      copy->exportstr = COPY_EXPORTSTR (var);

      copy->context = var->context;
    }
  return (copy);
}
#endif

/* **************************************************************** */
/*								    */
/*		  Deleting and unsetting variables		    */
/*								    */
/* **************************************************************** */

/* Dispose of the information attached to VAR. */
static void
dispose_variable_value (var)
     SHELL_VAR *var;
{
  if (function_p (var))
    dispose_command (function_cell (var));
#if defined (ARRAY_VARS)
  else if (array_p (var))
    array_dispose (array_cell (var));
  else if (assoc_p (var))
    assoc_dispose (assoc_cell (var));
#endif
  else if (nameref_p (var))
    FREE (nameref_cell (var));
  else
    FREE (value_cell (var));
}

void
dispose_variable (var)
     SHELL_VAR *var;
{
  if (var == 0)
    return;

  if (nofree_p (var) == 0)
    dispose_variable_value (var);

  FREE_EXPORTSTR (var);

  free (var->name);

  if (exported_p (var))
    array_needs_making = 1;

  free (var);
}

/* Unset the shell variable referenced by NAME.  Unsetting a nameref variable
   unsets the variable it resolves to but leaves the nameref alone. */
int
unbind_variable (name)
     const char *name;
{
  SHELL_VAR *v, *nv;
  int r;

  v = var_lookup (name, shell_variables);
  nv = (v && nameref_p (v)) ? find_variable_nameref (v) : (SHELL_VAR *)NULL;

  r = nv ? makunbound (nv->name, shell_variables) : makunbound (name, shell_variables);
  return r;
}

/* Unbind NAME, where NAME is assumed to be a nameref variable */
int
unbind_nameref (name)
     const char *name;
{
  SHELL_VAR *v;

  v = var_lookup (name, shell_variables);
  if (v && nameref_p (v))
    return makunbound (name, shell_variables);
  return 0;
}

/* Unbind the first instance of NAME, whether it's a nameref or not */
int
unbind_variable_noref (name)
     const char *name;
{
  SHELL_VAR *v;

  v = var_lookup (name, shell_variables);
  if (v)
    return makunbound (name, shell_variables);
  return 0;
}

int
unbind_global_variable (name)
     const char *name;
{
  SHELL_VAR *v, *nv;
  int r;

  v = var_lookup (name, global_variables);
  /* This starts at the current scope, just like find_global_variable; should we
     use find_global_variable_nameref here? */
  nv = (v && nameref_p (v)) ? find_variable_nameref (v) : (SHELL_VAR *)NULL;

  r = nv ? makunbound (nv->name, shell_variables) : makunbound (name, global_variables);
  return r;
}

int
unbind_global_variable_noref (name)
     const char *name;
{
  SHELL_VAR *v;

  v = var_lookup (name, global_variables);
  if (v)
    return makunbound (name, global_variables);
  return 0;
}
 
int
check_unbind_variable (name)
     const char *name;
{
  SHELL_VAR *v;

  v = find_variable (name);
  if (v && readonly_p (v))
    {
      internal_error (_("%s: cannot unset: readonly %s"), name, "variable");
      return -2;
    }
  else if (v && non_unsettable_p (v))
    {
      internal_error (_("%s: cannot unset"), name);
      return -2;
    }
  return (unbind_variable (name));
}

/* Unset the shell function named NAME. */
int
unbind_func (name)
     const char *name;
{
  BUCKET_CONTENTS *elt;
  SHELL_VAR *func;

  elt = hash_remove (name, shell_functions, 0);

  if (elt == 0)
    return -1;

#if defined (PROGRAMMABLE_COMPLETION)
  set_itemlist_dirty (&it_functions);
#endif

  func = (SHELL_VAR *)elt->data;
  if (func)
    {
      if (exported_p (func))
	array_needs_making++;
      dispose_variable (func);
    }

  free (elt->key);
  free (elt);

  return 0;  
}

#if defined (DEBUGGER)
int
unbind_function_def (name)
     const char *name;
{
  BUCKET_CONTENTS *elt;
  FUNCTION_DEF *funcdef;

  elt = hash_remove (name, shell_function_defs, 0);

  if (elt == 0)
    return -1;

  funcdef = (FUNCTION_DEF *)elt->data;
  if (funcdef)
    dispose_function_def (funcdef);

  free (elt->key);
  free (elt);

  return 0;  
}
#endif /* DEBUGGER */

int
delete_var (name, vc)
     const char *name;
     VAR_CONTEXT *vc;
{
  BUCKET_CONTENTS *elt;
  SHELL_VAR *old_var;
  VAR_CONTEXT *v;

  for (elt = (BUCKET_CONTENTS *)NULL, v = vc; v; v = v->down)
    if (elt = hash_remove (name, v->table, 0))
      break;

  if (elt == 0)
    return (-1);

  old_var = (SHELL_VAR *)elt->data;
  free (elt->key);
  free (elt);

  dispose_variable (old_var);
  return (0);
}

/* Make the variable associated with NAME go away.  HASH_LIST is the
   hash table from which this variable should be deleted (either
   shell_variables or shell_functions).
   Returns non-zero if the variable couldn't be found. */
int
makunbound (name, vc)
     const char *name;
     VAR_CONTEXT *vc;
{
  BUCKET_CONTENTS *elt, *new_elt;
  SHELL_VAR *old_var;
  VAR_CONTEXT *v;
  char *t;

  for (elt = (BUCKET_CONTENTS *)NULL, v = vc; v; v = v->down)
    if (elt = hash_remove (name, v->table, 0))
      break;

  if (elt == 0)
    return (-1);

  old_var = (SHELL_VAR *)elt->data;

  if (old_var && exported_p (old_var))
    array_needs_making++;

  /* If we're unsetting a local variable and we're still executing inside
     the function, just mark the variable as invisible.  The function
     eventually called by pop_var_context() will clean it up later.  This
     must be done so that if the variable is subsequently assigned a new
     value inside the function, the `local' attribute is still present.
     We also need to add it back into the correct hash table. */
  if (old_var && local_p (old_var) &&
	(old_var->context == variable_context || (localvar_unset && old_var->context < variable_context)))
    {
      if (nofree_p (old_var))
	var_setvalue (old_var, (char *)NULL);
#if defined (ARRAY_VARS)
      else if (array_p (old_var))
	array_dispose (array_cell (old_var));
      else if (assoc_p (old_var))
	assoc_dispose (assoc_cell (old_var));
#endif
      else if (nameref_p (old_var))
	FREE (nameref_cell (old_var));
      else
	FREE (value_cell (old_var));
      /* Reset the attributes.  Preserve the export attribute if the variable
	 came from a temporary environment.  Make sure it stays local, and
	 make it invisible. */ 
      old_var->attributes = (exported_p (old_var) && tempvar_p (old_var)) ? att_exported : 0;
      VSETATTR (old_var, att_local);
      VSETATTR (old_var, att_invisible);
      var_setvalue (old_var, (char *)NULL);
      INVALIDATE_EXPORTSTR (old_var);

      new_elt = hash_insert (savestring (old_var->name), v->table, 0);
      new_elt->data = (PTR_T)old_var;
      stupidly_hack_special_variables (old_var->name);

      free (elt->key);
      free (elt);
      return (0);
    }

  /* Have to save a copy of name here, because it might refer to
     old_var->name.  If so, stupidly_hack_special_variables will
     reference freed memory. */
  t = savestring (name);

  free (elt->key);
  free (elt);

  dispose_variable (old_var);
  stupidly_hack_special_variables (t);
  free (t);

  return (0);
}

/* Get rid of all of the variables in the current context. */
void
kill_all_local_variables ()
{
  VAR_CONTEXT *vc;

  for (vc = shell_variables; vc; vc = vc->down)
    if (vc_isfuncenv (vc) && vc->scope == variable_context)
      break;
  if (vc == 0)
    return;		/* XXX */

  if (vc->table && vc_haslocals (vc))
    {
      delete_all_variables (vc->table);
      hash_dispose (vc->table);
    }
  vc->table = (HASH_TABLE *)NULL;
}

static void
free_variable_hash_data (data)
     PTR_T data;
{
  SHELL_VAR *var;

  var = (SHELL_VAR *)data;
  dispose_variable (var);
}

/* Delete the entire contents of the hash table. */
void
delete_all_variables (hashed_vars)
     HASH_TABLE *hashed_vars;
{
  hash_flush (hashed_vars, free_variable_hash_data);
}

/* **************************************************************** */
/*								    */
/*		     Setting variable attributes		    */
/*								    */
/* **************************************************************** */

#define FIND_OR_MAKE_VARIABLE(name, entry) \
  do \
    { \
      entry = find_variable (name); \
      if (!entry) \
	{ \
	  entry = bind_variable (name, "", 0); \
	  if (entry) entry->attributes |= att_invisible; \
	} \
    } \
  while (0)

/* Make the variable associated with NAME be readonly.
   If NAME does not exist yet, create it. */
void
set_var_read_only (name)
     char *name;
{
  SHELL_VAR *entry;

  FIND_OR_MAKE_VARIABLE (name, entry);
  VSETATTR (entry, att_readonly);
}

#ifdef INCLUDE_UNUSED
/* Make the function associated with NAME be readonly.
   If NAME does not exist, we just punt, like auto_export code below. */
void
set_func_read_only (name)
     const char *name;
{
  SHELL_VAR *entry;

  entry = find_function (name);
  if (entry)
    VSETATTR (entry, att_readonly);
}

/* Make the variable associated with NAME be auto-exported.
   If NAME does not exist yet, create it. */
void
set_var_auto_export (name)
     char *name;
{
  SHELL_VAR *entry;

  FIND_OR_MAKE_VARIABLE (name, entry);
  set_auto_export (entry);
}

/* Make the function associated with NAME be auto-exported. */
void
set_func_auto_export (name)
     const char *name;
{
  SHELL_VAR *entry;

  entry = find_function (name);
  if (entry)
    set_auto_export (entry);
}
#endif

/* **************************************************************** */
/*								    */
/*		     Creating lists of variables		    */
/*								    */
/* **************************************************************** */

static VARLIST *
vlist_alloc (nentries)
     int nentries;
{
  VARLIST  *vlist;

  vlist = (VARLIST *)xmalloc (sizeof (VARLIST));
  vlist->list = (SHELL_VAR **)xmalloc ((nentries + 1) * sizeof (SHELL_VAR *));
  vlist->list_size = nentries;
  vlist->list_len = 0;
  vlist->list[0] = (SHELL_VAR *)NULL;

  return vlist;
}

static VARLIST *
vlist_realloc (vlist, n)
     VARLIST *vlist;
     int n;
{
  if (vlist == 0)
    return (vlist = vlist_alloc (n));
  if (n > vlist->list_size)
    {
      vlist->list_size = n;
      vlist->list = (SHELL_VAR **)xrealloc (vlist->list, (vlist->list_size + 1) * sizeof (SHELL_VAR *));
    }
  return vlist;
}

static void
vlist_add (vlist, var, flags)
     VARLIST *vlist;
     SHELL_VAR *var;
     int flags;
{
  register int i;

  for (i = 0; i < vlist->list_len; i++)
    if (STREQ (var->name, vlist->list[i]->name))
      break;
  if (i < vlist->list_len)
    return;

  if (i >= vlist->list_size)
    vlist = vlist_realloc (vlist, vlist->list_size + 16);

  vlist->list[vlist->list_len++] = var;
  vlist->list[vlist->list_len] = (SHELL_VAR *)NULL;
}

/* Map FUNCTION over the variables in VAR_HASH_TABLE.  Return an array of the
   variables for which FUNCTION returns a non-zero value.  A NULL value
   for FUNCTION means to use all variables. */
SHELL_VAR **
map_over (function, vc)
     sh_var_map_func_t *function;
     VAR_CONTEXT *vc;
{
  VAR_CONTEXT *v;
  VARLIST *vlist;
  SHELL_VAR **ret;
  int nentries;

  for (nentries = 0, v = vc; v; v = v->down)
    nentries += HASH_ENTRIES (v->table);

  if (nentries == 0)
    return (SHELL_VAR **)NULL;

  vlist = vlist_alloc (nentries);

  for (v = vc; v; v = v->down)
    flatten (v->table, function, vlist, 0);

  ret = vlist->list;
  free (vlist);
  return ret;
}

SHELL_VAR **
map_over_funcs (function)
     sh_var_map_func_t *function;
{
  VARLIST *vlist;
  SHELL_VAR **ret;

  if (shell_functions == 0 || HASH_ENTRIES (shell_functions) == 0)
    return ((SHELL_VAR **)NULL);

  vlist = vlist_alloc (HASH_ENTRIES (shell_functions));

  flatten (shell_functions, function, vlist, 0);

  ret = vlist->list;
  free (vlist);
  return ret;
}

/* Flatten VAR_HASH_TABLE, applying FUNC to each member and adding those
   elements for which FUNC succeeds to VLIST->list.  FLAGS is reserved
   for future use.  Only unique names are added to VLIST.  If FUNC is
   NULL, each variable in VAR_HASH_TABLE is added to VLIST.  If VLIST is
   NULL, FUNC is applied to each SHELL_VAR in VAR_HASH_TABLE.  If VLIST
   and FUNC are both NULL, nothing happens. */
static void
flatten (var_hash_table, func, vlist, flags)
     HASH_TABLE *var_hash_table;
     sh_var_map_func_t *func;
     VARLIST *vlist;
     int flags;
{
  register int i;
  register BUCKET_CONTENTS *tlist;
  int r;
  SHELL_VAR *var;

  if (var_hash_table == 0 || (HASH_ENTRIES (var_hash_table) == 0) || (vlist == 0 && func == 0))
    return;

  for (i = 0; i < var_hash_table->nbuckets; i++)
    {
      for (tlist = hash_items (i, var_hash_table); tlist; tlist = tlist->next)
	{
	  var = (SHELL_VAR *)tlist->data;

	  r = func ? (*func) (var) : 1;
	  if (r && vlist)
	    vlist_add (vlist, var, flags);
	}
    }
}

void
sort_variables (array)
     SHELL_VAR **array;
{
  qsort (array, strvec_len ((char **)array), sizeof (SHELL_VAR *), (QSFUNC *)qsort_var_comp);
}

static int
qsort_var_comp (var1, var2)
     SHELL_VAR **var1, **var2;
{
  int result;

  if ((result = (*var1)->name[0] - (*var2)->name[0]) == 0)
    result = strcmp ((*var1)->name, (*var2)->name);

  return (result);
}

/* Apply FUNC to each variable in SHELL_VARIABLES, adding each one for
   which FUNC succeeds to an array of SHELL_VAR *s.  Returns the array. */
static SHELL_VAR **
vapply (func)
     sh_var_map_func_t *func;
{
  SHELL_VAR **list;

  list = map_over (func, shell_variables);
  if (list /* && posixly_correct */)
    sort_variables (list);
  return (list);
}

/* Apply FUNC to each variable in SHELL_FUNCTIONS, adding each one for
   which FUNC succeeds to an array of SHELL_VAR *s.  Returns the array. */
static SHELL_VAR **
fapply (func)
     sh_var_map_func_t *func;
{
  SHELL_VAR **list;

  list = map_over_funcs (func);
  if (list /* && posixly_correct */)
    sort_variables (list);
  return (list);
}

/* Create a NULL terminated array of all the shell variables. */
SHELL_VAR **
all_shell_variables ()
{
  return (vapply ((sh_var_map_func_t *)NULL));
}

/* Create a NULL terminated array of all the shell functions. */
SHELL_VAR **
all_shell_functions ()
{
  return (fapply ((sh_var_map_func_t *)NULL));
}

static int
visible_var (var)
     SHELL_VAR *var;
{
  return (invisible_p (var) == 0);
}

SHELL_VAR **
all_visible_functions ()
{
  return (fapply (visible_var));
}

SHELL_VAR **
all_visible_variables ()
{
  return (vapply (visible_var));
}

/* Return non-zero if the variable VAR is visible and exported.  Array
   variables cannot be exported. */
static int
visible_and_exported (var)
     SHELL_VAR *var;
{
  return (invisible_p (var) == 0 && exported_p (var));
}

/* Candidate variables for the export environment are either valid variables
   with the export attribute or invalid variables inherited from the initial
   environment and simply passed through. */
static int
export_environment_candidate (var)
     SHELL_VAR *var;
{
  return (exported_p (var) && (invisible_p (var) == 0 || imported_p (var)));
}

/* Return non-zero if VAR is a local variable in the current context and
   is exported. */
static int
local_and_exported (var)
     SHELL_VAR *var;
{
  return (invisible_p (var) == 0 && local_p (var) && var->context == variable_context && exported_p (var));
}

SHELL_VAR **
all_exported_variables ()
{
  return (vapply (visible_and_exported));
}

SHELL_VAR **
local_exported_variables ()
{
  return (vapply (local_and_exported));
}

static int
variable_in_context (var)
     SHELL_VAR *var;
{
  return (local_p (var) && var->context == variable_context);
}

static int
visible_variable_in_context (var)
     SHELL_VAR *var;
{
  return (invisible_p (var) == 0 && local_p (var) && var->context == variable_context);
}

SHELL_VAR **
all_local_variables (visible_only)
     int visible_only;
{
  VARLIST *vlist;
  SHELL_VAR **ret;
  VAR_CONTEXT *vc;

  vc = shell_variables;
  for (vc = shell_variables; vc; vc = vc->down)
    if (vc_isfuncenv (vc) && vc->scope == variable_context)
      break;

  if (vc == 0)
    {
      internal_error (_("all_local_variables: no function context at current scope"));
      return (SHELL_VAR **)NULL;
    }
  if (vc->table == 0 || HASH_ENTRIES (vc->table) == 0 || vc_haslocals (vc) == 0)
    return (SHELL_VAR **)NULL;
    
  vlist = vlist_alloc (HASH_ENTRIES (vc->table));

  if (visible_only)
    flatten (vc->table, visible_variable_in_context, vlist, 0);
  else
    flatten (vc->table, variable_in_context, vlist, 0);

  ret = vlist->list;
  free (vlist);
  if (ret)
    sort_variables (ret);
  return ret;
}

#if defined (ARRAY_VARS)
/* Return non-zero if the variable VAR is visible and an array. */
static int
visible_array_vars (var)
     SHELL_VAR *var;
{
  return (invisible_p (var) == 0 && (array_p (var) || assoc_p (var)));
}

SHELL_VAR **
all_array_variables ()
{
  return (vapply (visible_array_vars));
}
#endif /* ARRAY_VARS */

char **
all_variables_matching_prefix (prefix)
     const char *prefix;
{
  SHELL_VAR **varlist;
  char **rlist;
  int vind, rind, plen;

  plen = STRLEN (prefix);
  varlist = all_visible_variables ();
  for (vind = 0; varlist && varlist[vind]; vind++)
    ;
  if (varlist == 0 || vind == 0)
    return ((char **)NULL);
  rlist = strvec_create (vind + 1);
  for (vind = rind = 0; varlist[vind]; vind++)
    {
      if (plen == 0 || STREQN (prefix, varlist[vind]->name, plen))
	rlist[rind++] = savestring (varlist[vind]->name);
    }
  rlist[rind] = (char *)0;
  free (varlist);

  return rlist;
}

/* **************************************************************** */
/*								    */
/*		 Managing temporary variable scopes		    */
/*								    */
/* **************************************************************** */

/* Make variable NAME have VALUE in the temporary environment. */
static SHELL_VAR *
bind_tempenv_variable (name, value)
     const char *name;
     char *value;
{
  SHELL_VAR *var;

  var = temporary_env ? hash_lookup (name, temporary_env) : (SHELL_VAR *)NULL;

  if (var)
    {
      FREE (value_cell (var));
      var_setvalue (var, savestring (value));
      INVALIDATE_EXPORTSTR (var);
    }

  return (var);
}

/* Find a variable in the temporary environment that is named NAME.
   Return the SHELL_VAR *, or NULL if not found. */
SHELL_VAR *
find_tempenv_variable (name)
     const char *name;
{
  return (temporary_env ? hash_lookup (name, temporary_env) : (SHELL_VAR *)NULL);
}

char **tempvar_list;
int tvlist_ind;

/* Take a variable from an assignment statement preceding a posix special
   builtin (including `return') and create a global variable from it. This
   is called from merge_temporary_env, which is only called when in posix
   mode. */
static void
push_posix_temp_var (data)
     PTR_T data;
{
  SHELL_VAR *var, *v;
  HASH_TABLE *binding_table;

  var = (SHELL_VAR *)data;

  /* Just like do_assignment_internal(). This makes assignments preceding
     special builtins act like standalone assignment statements when in
     posix mode, satisfying the posix requirement that this affect the
     "current execution environment." */
  v = bind_variable (var->name, value_cell (var), ASS_FORCE|ASS_NOLONGJMP);

  /* XXX - do we need to worry about array variables here? */

  /* If this modifies an existing local variable, v->context will be non-zero.
     If it comes back with v->context == 0, we bound at the global context.
     Set binding_table appropriately. It doesn't matter whether it's correct
     if the variable is local, only that it's not global_variables->table */
  binding_table = v->context ? shell_variables->table : global_variables->table;

  /* global variables are no longer temporary and don't need propagating. */
  if (v->context == 0)
    var->attributes &= ~(att_tempvar|att_propagate);

  if (v)
    {
      v->attributes |= var->attributes;		/* preserve tempvar attribute if appropriate */
      /* If we don't bind a local variable, propagate the value. If we bind a
	 local variable (the "current execution environment"), keep it as local
	 and don't propagate it to the calling environment. */
      if (v->context > 0 && local_p (v) == 0)
	v->attributes |= att_propagate;
      else
	v->attributes &= ~att_propagate;
    }

  if (find_special_var (var->name) >= 0)
    tempvar_list[tvlist_ind++] = savestring (var->name);

  dispose_variable (var);
}

/* Push the variable described by (SHELL_VAR *)DATA down to the next
   variable context from the temporary environment. This can be called
   from one context:
      1. propagate_temp_var: which is called to propagate variables in
	 assignments like `var=value declare -x var' to the surrounding
	 scope.

  In this case, the variable should have the att_propagate flag set and
  we can create variables in the current scope.
*/
static void
push_temp_var (data)
     PTR_T data;
{
  SHELL_VAR *var, *v;
  HASH_TABLE *binding_table;

  var = (SHELL_VAR *)data;

  binding_table = shell_variables->table;
  if (binding_table == 0)
    {
      if (shell_variables == global_variables)
	/* shouldn't happen */
	binding_table = shell_variables->table = global_variables->table = hash_create (VARIABLES_HASH_BUCKETS);
      else
	binding_table = shell_variables->table = hash_create (TEMPENV_HASH_BUCKETS);
    }

  v = bind_variable_internal (var->name, value_cell (var), binding_table, 0, ASS_FORCE|ASS_NOLONGJMP);

  /* XXX - should we set the context here?  It shouldn't matter because of how
     assign_in_env works, but we do it anyway. */
  if (v)
    v->context = shell_variables->scope;

  if (binding_table == global_variables->table)		/* XXX */
    var->attributes &= ~(att_tempvar|att_propagate);
  else
    {
      var->attributes |= att_propagate;			/* XXX - propagate more than once? */
      if  (binding_table == shell_variables->table)
	shell_variables->flags |= VC_HASTMPVAR;
    }
  if (v)
    v->attributes |= var->attributes;

  if (find_special_var (var->name) >= 0)
    tempvar_list[tvlist_ind++] = savestring (var->name);

  dispose_variable (var);
}

/* Take a variable described by DATA and push it to the surrounding scope if
   the PROPAGATE attribute is set. That gets set by push_temp_var if we are
   taking a variable like `var=value declare -x var' and propagating it to
   the enclosing scope. */
static void
propagate_temp_var (data)
     PTR_T data;
{
  SHELL_VAR *var;

  var = (SHELL_VAR *)data;
  if (tempvar_p (var) && (var->attributes & att_propagate))
    push_temp_var (data);
  else
    {
      if (find_special_var (var->name) >= 0)
	tempvar_list[tvlist_ind++] = savestring (var->name);
      dispose_variable (var);
    }
}

/* Free the storage used in the hash table for temporary
   environment variables.  PUSHF is a function to be called
   to free each hash table entry.  It takes care of pushing variables
   to previous scopes if appropriate.  PUSHF stores names of variables
   that require special handling (e.g., IFS) on tempvar_list, so this
   function can call stupidly_hack_special_variables on all the
   variables in the list when the temporary hash table is destroyed. */
static void
dispose_temporary_env (pushf)
     sh_free_func_t *pushf;
{
  int i;
  HASH_TABLE *disposer;

  tempvar_list = strvec_create (HASH_ENTRIES (temporary_env) + 1);
  tempvar_list[tvlist_ind = 0] = 0;

  disposer = temporary_env;
  temporary_env = (HASH_TABLE *)NULL;

  hash_flush (disposer, pushf);
  hash_dispose (disposer);

  tempvar_list[tvlist_ind] = 0;

  array_needs_making = 1;

  for (i = 0; i < tvlist_ind; i++)
    stupidly_hack_special_variables (tempvar_list[i]);

  strvec_dispose (tempvar_list);
  tempvar_list = 0;
  tvlist_ind = 0;
}

void
dispose_used_env_vars ()
{
  if (temporary_env)
    {
      dispose_temporary_env (propagate_temp_var);
      maybe_make_export_env ();
    }
}

/* Take all of the shell variables in the temporary environment HASH_TABLE
   and make shell variables from them at the current variable context.
   Right now, this is only called in Posix mode to implement the historical
   accident of creating global variables from assignment statements preceding
   special builtins, but we check in case this acquires another caller later. */
void
merge_temporary_env ()
{
  if (temporary_env)
    dispose_temporary_env (posixly_correct ? push_posix_temp_var : push_temp_var);
}

/* Temporary function to use if we want to separate function and special
   builtin behavior. */
void
merge_function_temporary_env ()
{
  if (temporary_env)
    dispose_temporary_env (push_temp_var);
}

void
flush_temporary_env ()
{
  if (temporary_env)
    {
      hash_flush (temporary_env, free_variable_hash_data);
      hash_dispose (temporary_env);
      temporary_env = (HASH_TABLE *)NULL;
    }
}

/* **************************************************************** */
/*								    */
/*	     Creating and manipulating the environment		    */
/*								    */
/* **************************************************************** */

static inline char *
mk_env_string (name, value, attributes)
     const char *name, *value;
     int attributes;
{
  size_t name_len, value_len;
  char	*p, *q, *t;
  int isfunc, isarray;

  name_len = strlen (name);
  value_len = STRLEN (value);

  isfunc = attributes & att_function;
#if defined (ARRAY_VARS) && defined (ARRAY_EXPORT)
  isarray = attributes & (att_array|att_assoc);
#endif

  /* If we are exporting a shell function, construct the encoded function
     name. */
  if (isfunc && value)
    {
      p = (char *)xmalloc (BASHFUNC_PREFLEN + name_len + BASHFUNC_SUFFLEN + value_len + 2);
      q = p;
      memcpy (q, BASHFUNC_PREFIX, BASHFUNC_PREFLEN);
      q += BASHFUNC_PREFLEN;
      memcpy (q, name, name_len);
      q += name_len;
      memcpy (q, BASHFUNC_SUFFIX, BASHFUNC_SUFFLEN);
      q += BASHFUNC_SUFFLEN;
    }
#if defined (ARRAY_VARS) && defined (ARRAY_EXPORT)
  else if (isarray && value)
    {
      if (attributes & att_assoc)
	p = (char *)xmalloc (BASHASSOC_PREFLEN + name_len + BASHASSOC_SUFFLEN + value_len + 2);
      else
	p = (char *)xmalloc (BASHARRAY_PREFLEN + name_len + BASHARRAY_SUFFLEN + value_len + 2);
      q = p;
      if (attributes & att_assoc)
	{
	  memcpy (q, BASHASSOC_PREFIX, BASHASSOC_PREFLEN);
	  q += BASHASSOC_PREFLEN;
	}
      else
	{
	  memcpy (q, BASHARRAY_PREFIX, BASHARRAY_PREFLEN);
	  q += BASHARRAY_PREFLEN;
	}
      memcpy (q, name, name_len);
      q += name_len;
      /* These are actually the same currently */
      if (attributes & att_assoc)
        {
	  memcpy (q, BASHASSOC_SUFFIX, BASHASSOC_SUFFLEN);
	  q += BASHARRAY_SUFFLEN;
        }
      else
        {
	  memcpy (q, BASHARRAY_SUFFIX, BASHARRAY_SUFFLEN);
	  q += BASHARRAY_SUFFLEN;
        }
    }
#endif  
  else
    {
      p = (char *)xmalloc (2 + name_len + value_len);
      memcpy (p, name, name_len);
      q = p + name_len;
    }

  q[0] = '=';
  if (value && *value)
    {
      if (isfunc)
	{
	  t = dequote_escapes (value);
	  value_len = STRLEN (t);
	  memcpy (q + 1, t, value_len + 1);
	  free (t);
	}
      else
	memcpy (q + 1, value, value_len + 1);
    }
  else
    q[1] = '\0';

  return (p);
}

#ifdef DEBUG
/* Debugging */
static int
valid_exportstr (v)
     SHELL_VAR *v;
{
  char *s;

  s = v->exportstr;
  if (s == 0)
    {
      internal_error (_("%s has null exportstr"), v->name);
      return (0);
    }
  if (legal_variable_starter ((unsigned char)*s) == 0)
    {
      internal_error (_("invalid character %d in exportstr for %s"), *s, v->name);
      return (0);
    }
  for (s = v->exportstr + 1; s && *s; s++)
    {
      if (*s == '=')
	break;
      if (legal_variable_char ((unsigned char)*s) == 0)
	{
	  internal_error (_("invalid character %d in exportstr for %s"), *s, v->name);
	  return (0);
	}
    }
  if (*s != '=')
    {
      internal_error (_("no `=' in exportstr for %s"), v->name);
      return (0);
    }
  return (1);
}
#endif

#if defined (ARRAY_VARS)
#  define USE_EXPORTSTR (value == var->exportstr && array_p (var) == 0 && assoc_p (var) == 0)
#else
#  define USE_EXPORTSTR (value == var->exportstr)
#endif

static char **
make_env_array_from_var_list (vars)
     SHELL_VAR **vars;
{
  register int i, list_index;
  register SHELL_VAR *var;
  char **list, *value;

  list = strvec_create ((1 + strvec_len ((char **)vars)));

  for (i = 0, list_index = 0; var = vars[i]; i++)
    {
#if defined (__CYGWIN__)
      /* We don't use the exportstr stuff on Cygwin at all. */
      INVALIDATE_EXPORTSTR (var);
#endif

      /* If the value is generated dynamically, generate it here. */
      if (regen_p (var) && var->dynamic_value)
	{
	  var = (*(var->dynamic_value)) (var);
	  INVALIDATE_EXPORTSTR (var);
	}

      if (var->exportstr)
	value = var->exportstr;
      else if (function_p (var))
	value = named_function_string ((char *)NULL, function_cell (var), 0);
#if defined (ARRAY_VARS)
      else if (array_p (var))
#  if ARRAY_EXPORT
	value = array_to_assign (array_cell (var), 0);
#  else
	continue;	/* XXX array vars cannot yet be exported */
#  endif /* ARRAY_EXPORT */
      else if (assoc_p (var))
#  if ARRAY_EXPORT
	value = assoc_to_assign (assoc_cell (var), 0);
#  else
	continue;	/* XXX associative array vars cannot yet be exported */
#  endif /* ARRAY_EXPORT */
#endif
      else
	value = value_cell (var);

      if (value)
	{
	  /* Gee, I'd like to get away with not using savestring() if we're
	     using the cached exportstr... */
	  list[list_index] = USE_EXPORTSTR ? savestring (value)
					   : mk_env_string (var->name, value, var->attributes);

	  if (USE_EXPORTSTR == 0)
	    SAVE_EXPORTSTR (var, list[list_index]);

	  list_index++;
#undef USE_EXPORTSTR

#if defined (ARRAY_VARS) && defined (ARRAY_EXPORT)
	  if (array_p (var) || assoc_p (var))
	    free (value);
#endif
	}
    }

  list[list_index] = (char *)NULL;
  return (list);
}

/* Make an array of assignment statements from the hash table
   HASHED_VARS which contains SHELL_VARs.  Only visible, exported
   variables are eligible. */
static char **
make_var_export_array (vcxt)
     VAR_CONTEXT *vcxt;
{
  char **list;
  SHELL_VAR **vars;

#if 0
  vars = map_over (visible_and_exported, vcxt);
#else
  vars = map_over (export_environment_candidate, vcxt);
#endif

  if (vars == 0)
    return (char **)NULL;

  list = make_env_array_from_var_list (vars);

  free (vars);
  return (list);
}

static char **
make_func_export_array ()
{
  char **list;
  SHELL_VAR **vars;

  vars = map_over_funcs (visible_and_exported);
  if (vars == 0)
    return (char **)NULL;

  list = make_env_array_from_var_list (vars);

  free (vars);
  return (list);
}

/* Add ENVSTR to the end of the exported environment, EXPORT_ENV. */
#define add_to_export_env(envstr,do_alloc) \
do \
  { \
    if (export_env_index >= (export_env_size - 1)) \
      { \
	export_env_size += 16; \
	export_env = strvec_resize (export_env, export_env_size); \
	environ = export_env; \
      } \
    export_env[export_env_index++] = (do_alloc) ? savestring (envstr) : envstr; \
    export_env[export_env_index] = (char *)NULL; \
  } while (0)

/* Add ASSIGN to EXPORT_ENV, or supersede a previous assignment in the
   array with the same left-hand side.  Return the new EXPORT_ENV. */
char **
add_or_supercede_exported_var (assign, do_alloc)
     char *assign;
     int do_alloc;
{
  register int i;
  int equal_offset;

  equal_offset = assignment (assign, 0);
  if (equal_offset == 0)
    return (export_env);

  /* If this is a function, then only supersede the function definition.
     We do this by including the `=() {' in the comparison, like
     initialize_shell_variables does. */
  if (assign[equal_offset + 1] == '(' &&
     strncmp (assign + equal_offset + 2, ") {", 3) == 0)		/* } */
    equal_offset += 4;

  for (i = 0; i < export_env_index; i++)
    {
      if (STREQN (assign, export_env[i], equal_offset + 1))
	{
	  free (export_env[i]);
	  export_env[i] = do_alloc ? savestring (assign) : assign;
	  return (export_env);
	}
    }
  add_to_export_env (assign, do_alloc);
  return (export_env);
}

static void
add_temp_array_to_env (temp_array, do_alloc, do_supercede)
     char **temp_array;
     int do_alloc, do_supercede;
{
  register int i;

  if (temp_array == 0)
    return;

  for (i = 0; temp_array[i]; i++)
    {
      if (do_supercede)
	export_env = add_or_supercede_exported_var (temp_array[i], do_alloc);
      else
	add_to_export_env (temp_array[i], do_alloc);
    }

  free (temp_array);
}

/* Make the environment array for the command about to be executed, if the
   array needs making.  Otherwise, do nothing.  If a shell action could
   change the array that commands receive for their environment, then the
   code should `array_needs_making++'.

   The order to add to the array is:
   	temporary_env
   	list of var contexts whose head is shell_variables
  	shell_functions

  This is the shell variable lookup order.  We add only new variable
  names at each step, which allows local variables and variables in
  the temporary environments to shadow variables in the global (or
  any previous) scope.
*/

static int
n_shell_variables ()
{
  VAR_CONTEXT *vc;
  int n;

  for (n = 0, vc = shell_variables; vc; vc = vc->down)
    n += HASH_ENTRIES (vc->table);
  return n;
}

int
chkexport (name)
     char *name;
{
  SHELL_VAR *v;

  v = find_variable (name);
  if (v && exported_p (v))
    {
      array_needs_making = 1;
      maybe_make_export_env ();
      return 1;
    }
  return 0;
}

void
maybe_make_export_env ()
{
  register char **temp_array;
  int new_size;
  VAR_CONTEXT *tcxt, *icxt;

  if (array_needs_making)
    {
      if (export_env)
	strvec_flush (export_env);

      /* Make a guess based on how many shell variables and functions we
	 have.  Since there will always be array variables, and array
	 variables are not (yet) exported, this will always be big enough
	 for the exported variables and functions. */
      new_size = n_shell_variables () + HASH_ENTRIES (shell_functions) + 1 +
		 HASH_ENTRIES (temporary_env) + HASH_ENTRIES (invalid_env);
      if (new_size > export_env_size)
	{
	  export_env_size = new_size;
	  export_env = strvec_resize (export_env, export_env_size);
	  environ = export_env;
	}
      export_env[export_env_index = 0] = (char *)NULL;

      /* Make a dummy variable context from the temporary_env, stick it on
	 the front of shell_variables, call make_var_export_array on the
	 whole thing to flatten it, and convert the list of SHELL_VAR *s
	 to the form needed by the environment. */
      if (temporary_env)
	{
	  tcxt = new_var_context ((char *)NULL, 0);
	  tcxt->table = temporary_env;
	  tcxt->down = shell_variables;
	}
      else
	tcxt = shell_variables;

      if (invalid_env)
	{
	  icxt = new_var_context ((char *)NULL, 0);
	  icxt->table = invalid_env;
	  icxt->down = tcxt;
	}
      else
	icxt = tcxt;
      
      temp_array = make_var_export_array (icxt);
      if (temp_array)
	add_temp_array_to_env (temp_array, 0, 0);

      if (icxt != tcxt)
	free (icxt);

      if (tcxt != shell_variables)
	free (tcxt);

#if defined (RESTRICTED_SHELL)
      /* Restricted shells may not export shell functions. */
      temp_array = restricted ? (char **)0 : make_func_export_array ();
#else
      temp_array = make_func_export_array ();
#endif
      if (temp_array)
	add_temp_array_to_env (temp_array, 0, 0);

      array_needs_making = 0;
    }
}

/* This is an efficiency hack.  PWD and OLDPWD are auto-exported, so
   we will need to remake the exported environment every time we
   change directories.  `_' is always put into the environment for
   every external command, so without special treatment it will always
   cause the environment to be remade.

   If there is no other reason to make the exported environment, we can
   just update the variables in place and mark the exported environment
   as no longer needing a remake. */
void
update_export_env_inplace (env_prefix, preflen, value)
     char *env_prefix;
     int preflen;
     char *value;
{
  char *evar;

  evar = (char *)xmalloc (STRLEN (value) + preflen + 1);
  strcpy (evar, env_prefix);
  if (value)
    strcpy (evar + preflen, value);
  export_env = add_or_supercede_exported_var (evar, 0);
}

/* We always put _ in the environment as the name of this command. */
void
put_command_name_into_env (command_name)
     char *command_name;
{
  update_export_env_inplace ("_=", 2, command_name);
}

/* **************************************************************** */
/*								    */
/*		      Managing variable contexts		    */
/*								    */
/* **************************************************************** */

/* Allocate and return a new variable context with NAME and FLAGS.
   NAME can be NULL. */

VAR_CONTEXT *
new_var_context (name, flags)
     char *name;
     int flags;
{
  VAR_CONTEXT *vc;

  vc = (VAR_CONTEXT *)xmalloc (sizeof (VAR_CONTEXT));
  vc->name = name ? savestring (name) : (char *)NULL;
  vc->scope = variable_context;
  vc->flags = flags;

  vc->up = vc->down = (VAR_CONTEXT *)NULL;
  vc->table = (HASH_TABLE *)NULL;

  return vc;
}

/* Free a variable context and its data, including the hash table.  Dispose
   all of the variables. */
void
dispose_var_context (vc)
     VAR_CONTEXT *vc;
{
  FREE (vc->name);

  if (vc->table)
    {
      delete_all_variables (vc->table);
      hash_dispose (vc->table);
    }

  free (vc);
}

/* Set VAR's scope level to the current variable context. */
static int
set_context (var)
     SHELL_VAR *var;
{
  return (var->context = variable_context);
}

/* Make a new variable context with NAME and FLAGS and a HASH_TABLE of
   temporary variables, and push it onto shell_variables.  This is
   for shell functions. */
VAR_CONTEXT *
push_var_context (name, flags, tempvars)
     char *name;
     int flags;
     HASH_TABLE *tempvars;
{
  VAR_CONTEXT *vc;
  int posix_func_behavior;

  /* As of IEEE Std 1003.1-2017, assignment statements preceding shell
     functions no longer behave like assignment statements preceding
     special builtins, and do not persist in the current shell environment.
     This is austin group interp #654, though nobody implements it yet. */
  posix_func_behavior = 0;

  vc = new_var_context (name, flags);
  /* Posix interp 1009, temporary assignments preceding function calls modify
     the current environment *before* the command is executed. */
  if (posix_func_behavior && (flags & VC_FUNCENV) && tempvars == temporary_env)
    merge_temporary_env ();
  else if (tempvars)
    {
      vc->table = tempvars;
      /* Have to do this because the temp environment was created before
	 variable_context was incremented. */
      /* XXX - only need to do it if flags&VC_FUNCENV */
      flatten (tempvars, set_context, (VARLIST *)NULL, 0);
      vc->flags |= VC_HASTMPVAR;
    }
  vc->down = shell_variables;
  shell_variables->up = vc;

  return (shell_variables = vc);
}

/* This can be called from one of two code paths:
	1. pop_scope, which implements the posix rules for propagating variable
	   assignments preceding special builtins to the surrounding scope
	   (push_builtin_var -- isbltin == 1);
	2. pop_var_context, which is called from pop_context and implements the
	   posix rules for propagating variable assignments preceding function
	   calls to the surrounding scope (push_func_var -- isbltin == 0)

  It takes variables out of a temporary environment hash table. We take the
  variable in data.
*/

static inline void
push_posix_tempvar_internal (var, isbltin)
     SHELL_VAR *var;
     int isbltin;
{
  SHELL_VAR *v;
  int posix_var_behavior;

  /* As of IEEE Std 1003.1-2017, assignment statements preceding shell
     functions no longer behave like assignment statements preceding
     special builtins, and do not persist in the current shell environment.
     This is austin group interp #654, though nobody implements it yet. */
  posix_var_behavior = posixly_correct && isbltin;
  v = 0;

  if (local_p (var) && STREQ (var->name, "-"))
    {
      set_current_options (value_cell (var));
      set_shellopts ();
    }
  /* This takes variable assignments preceding special builtins that can execute
     multiple commands (source, eval, etc.) and performs the equivalent of
     an assignment statement to modify the closest enclosing variable (the
     posix "current execution environment"). This makes the behavior the same
     as push_posix_temp_var; but the circumstances of calling are slightly
     different. */
  else if (tempvar_p (var) && posix_var_behavior)
    {
      /* similar to push_posix_temp_var */
      v = bind_variable (var->name, value_cell (var), ASS_FORCE|ASS_NOLONGJMP);
      if (v)
	{
	  v->attributes |= var->attributes;
	  if (v->context == 0)
	    v->attributes &= ~(att_tempvar|att_propagate);
	  /* XXX - set att_propagate here if v->context > 0? */
	}
    }
  else if (tempvar_p (var) && propagate_p (var))
    {
      /* Make sure we have a hash table to store the variable in while it is
	 being propagated down to the global variables table.  Create one if
	 we have to */
      if ((vc_isfuncenv (shell_variables) || vc_istempenv (shell_variables)) && shell_variables->table == 0)
	shell_variables->table = hash_create (VARIABLES_HASH_BUCKETS);
      v = bind_variable_internal (var->name, value_cell (var), shell_variables->table, 0, 0);
      /* XXX - should we set v->context here? */
      if (v)
	v->context = shell_variables->scope;
      if (shell_variables == global_variables)
	var->attributes &= ~(att_tempvar|att_propagate);
      else
	shell_variables->flags |= VC_HASTMPVAR;
      if (v)
	v->attributes |= var->attributes;
    }
  else
    stupidly_hack_special_variables (var->name);	/* XXX */

#if defined (ARRAY_VARS)
  if (v && (array_p (var) || assoc_p (var)))
    {
      FREE (value_cell (v));
      if (array_p (var))
	var_setarray (v, array_copy (array_cell (var)));
      else
	var_setassoc (v, assoc_copy (assoc_cell (var)));
    }
#endif	  

  dispose_variable (var);
}

static void
push_func_var (data)
     PTR_T data;
{
  SHELL_VAR *var;

  var = (SHELL_VAR *)data;
  push_posix_tempvar_internal (var, 0);
}

static void
push_builtin_var (data)
     PTR_T data;
{
  SHELL_VAR *var;

  var = (SHELL_VAR *)data;
  push_posix_tempvar_internal (var, 1);
}

/* Pop the top context off of VCXT and dispose of it, returning the rest of
   the stack. */
void
pop_var_context ()
{
  VAR_CONTEXT *ret, *vcxt;

  vcxt = shell_variables;
  if (vc_isfuncenv (vcxt) == 0)
    {
      internal_error (_("pop_var_context: head of shell_variables not a function context"));
      return;
    }

  if (ret = vcxt->down)
    {
      ret->up = (VAR_CONTEXT *)NULL;
      shell_variables = ret;
      if (vcxt->table)
	hash_flush (vcxt->table, push_func_var);
      dispose_var_context (vcxt);
    }
  else
    internal_error (_("pop_var_context: no global_variables context"));
}

static void
delete_local_contexts (vcxt)
     VAR_CONTEXT *vcxt;
{
  VAR_CONTEXT *v, *t;

  for (v = vcxt; v != global_variables; v = t)
    {
      t = v->down;
      dispose_var_context (v);
    }
}

/* Delete the HASH_TABLEs for all variable contexts beginning at VCXT, and
   all of the VAR_CONTEXTs except GLOBAL_VARIABLES. */
void
delete_all_contexts (vcxt)
     VAR_CONTEXT *vcxt;
{
  delete_local_contexts (vcxt);
  delete_all_variables (global_variables->table);
  shell_variables = global_variables;
}

/* Reset the context so we are not executing in a shell function. Only call
   this if you are getting ready to exit the shell. */
void
reset_local_contexts ()
{
  delete_local_contexts (shell_variables);
  shell_variables = global_variables;
  variable_context = 0;
}

/* **************************************************************** */
/*								    */
/*	   Pushing and Popping temporary variable scopes	    */
/*								    */
/* **************************************************************** */

VAR_CONTEXT *
push_scope (flags, tmpvars)
     int flags;
     HASH_TABLE *tmpvars;
{
  return (push_var_context ((char *)NULL, flags, tmpvars));
}

static void
push_exported_var (data)
     PTR_T data;
{
  SHELL_VAR *var, *v;

  var = (SHELL_VAR *)data;

  /* If a temp var had its export attribute set, or it's marked to be
     propagated, bind it in the previous scope before disposing it. */
  /* XXX - This isn't exactly right, because all tempenv variables have the
    export attribute set. */
  if (tempvar_p (var) && exported_p (var) && (var->attributes & att_propagate))
    {
      var->attributes &= ~att_tempvar;		/* XXX */
      v = bind_variable_internal (var->name, value_cell (var), shell_variables->table, 0, 0);
      if (shell_variables == global_variables)
	var->attributes &= ~att_propagate;
      if (v)
	{
	  v->attributes |= var->attributes;
	  v->context = shell_variables->scope;
	}
    }
  else
    stupidly_hack_special_variables (var->name);	/* XXX */

  dispose_variable (var);
}

/* This is called to propagate variables in the temporary environment of a
   special builtin (if IS_SPECIAL != 0) or exported variables that are the
   result of a builtin like `source' or `command' that can operate on the
   variables in its temporary environment. In the first case, we call
   push_builtin_var, which does the right thing. */
void
pop_scope (is_special)
     int is_special;
{
  VAR_CONTEXT *vcxt, *ret;
  int is_bltinenv;

  vcxt = shell_variables;
  if (vc_istempscope (vcxt) == 0)
    {
      internal_error (_("pop_scope: head of shell_variables not a temporary environment scope"));
      return;
    }
  is_bltinenv = vc_isbltnenv (vcxt);	/* XXX - for later */

  ret = vcxt->down;
  if (ret)
    ret->up = (VAR_CONTEXT *)NULL;

  shell_variables = ret;

  /* Now we can take care of merging variables in VCXT into set of scopes
     whose head is RET (shell_variables). */
  FREE (vcxt->name);
  if (vcxt->table)
    {
      if (is_special)
	hash_flush (vcxt->table, push_builtin_var);
      else
	hash_flush (vcxt->table, push_exported_var);
      hash_dispose (vcxt->table);
    }
  free (vcxt);

  sv_ifs ("IFS");	/* XXX here for now */
}

/* **************************************************************** */
/*								    */
/*		 Pushing and Popping function contexts		    */
/*								    */
/* **************************************************************** */

struct saved_dollar_vars {
  char **first_ten;
  WORD_LIST *rest;
  int count;
};

static struct saved_dollar_vars *dollar_arg_stack = (struct saved_dollar_vars *)NULL;
static int dollar_arg_stack_slots;
static int dollar_arg_stack_index;

/* Functions to manipulate dollar_vars array. Need to keep these in sync with
   whatever remember_args() does. */
static char **
save_dollar_vars ()
{
  char **ret;
  int i;

  ret = strvec_create (10);
  for (i = 1; i < 10; i++)
    {
      ret[i] = dollar_vars[i];
      dollar_vars[i] = (char *)NULL;
    }
  return ret;
}

static void
restore_dollar_vars (args)
     char **args;
{
  int i;

  for (i = 1; i < 10; i++)
    dollar_vars[i] = args[i];
}

static void
free_dollar_vars ()
{
  int i;

  for (i = 1; i < 10; i++)
    {
      FREE (dollar_vars[i]);
      dollar_vars[i] = (char *)NULL;
    }
}

static void
free_saved_dollar_vars (args)
     char **args;
{
  int i;

  for (i = 1; i < 10; i++)
    FREE (args[i]);
}

/* Do what remember_args (xxx, 1) would have done. */
void
clear_dollar_vars ()
{
  free_dollar_vars ();
  dispose_words (rest_of_args);

  rest_of_args = (WORD_LIST *)NULL;
  posparam_count = 0;
}

/* XXX - should always be followed by remember_args () */
void
push_context (name, is_subshell, tempvars)
     char *name;	/* function name */
     int is_subshell;
     HASH_TABLE *tempvars;
{
  if (is_subshell == 0)
    push_dollar_vars ();
  variable_context++;
  push_var_context (name, VC_FUNCENV, tempvars);
}

/* Only called when subshell == 0, so we don't need to check, and can
   unconditionally pop the dollar vars off the stack. */
void
pop_context ()
{
  pop_dollar_vars ();
  variable_context--;
  pop_var_context ();

  sv_ifs ("IFS");		/* XXX here for now */
}

/* Save the existing positional parameters on a stack. */
void
push_dollar_vars ()
{
  if (dollar_arg_stack_index + 2 > dollar_arg_stack_slots)
    {
      dollar_arg_stack = (struct saved_dollar_vars *)
	xrealloc (dollar_arg_stack, (dollar_arg_stack_slots += 10)
		  * sizeof (struct saved_dollar_vars));
    }

  dollar_arg_stack[dollar_arg_stack_index].count = posparam_count;
  dollar_arg_stack[dollar_arg_stack_index].first_ten = save_dollar_vars ();
  dollar_arg_stack[dollar_arg_stack_index++].rest = rest_of_args;
  rest_of_args = (WORD_LIST *)NULL;
  posparam_count = 0;
  
  dollar_arg_stack[dollar_arg_stack_index].first_ten = (char **)NULL;
  dollar_arg_stack[dollar_arg_stack_index].rest = (WORD_LIST *)NULL;  
}

/* Restore the positional parameters from our stack. */
void
pop_dollar_vars ()
{
  if (dollar_arg_stack == 0 || dollar_arg_stack_index == 0)
    return;

  /* Wipe out current values */
  clear_dollar_vars ();

  rest_of_args = dollar_arg_stack[--dollar_arg_stack_index].rest;
  restore_dollar_vars (dollar_arg_stack[dollar_arg_stack_index].first_ten);
  free (dollar_arg_stack[dollar_arg_stack_index].first_ten);
  posparam_count = dollar_arg_stack[dollar_arg_stack_index].count;

  dollar_arg_stack[dollar_arg_stack_index].first_ten = (char **)NULL;
  dollar_arg_stack[dollar_arg_stack_index].rest = (WORD_LIST *)NULL;
  dollar_arg_stack[dollar_arg_stack_index].count = 0;

  set_dollar_vars_unchanged ();
  invalidate_cached_quoted_dollar_at ();
}

void
dispose_saved_dollar_vars ()
{
  if (dollar_arg_stack == 0 || dollar_arg_stack_index == 0)
    return;

  dispose_words (dollar_arg_stack[--dollar_arg_stack_index].rest);    
  free_saved_dollar_vars (dollar_arg_stack[dollar_arg_stack_index].first_ten);	
  free (dollar_arg_stack[dollar_arg_stack_index].first_ten);

  dollar_arg_stack[dollar_arg_stack_index].first_ten = (char **)NULL;  
  dollar_arg_stack[dollar_arg_stack_index].rest = (WORD_LIST *)NULL;
  dollar_arg_stack[dollar_arg_stack_index].count = 0;
}

/* Initialize BASH_ARGV and BASH_ARGC after turning on extdebug after the
   shell is initialized */
void
init_bash_argv ()
{
  if (bash_argv_initialized == 0)
    {
      save_bash_argv ();
      bash_argv_initialized = 1;
    }
}

void
save_bash_argv ()
{
  WORD_LIST *list;

  list = list_rest_of_args ();
  push_args (list);
  dispose_words (list);
}

/* Manipulate the special BASH_ARGV and BASH_ARGC variables. */

void
push_args (list)
     WORD_LIST *list;
{
#if defined (ARRAY_VARS) && defined (DEBUGGER)
  SHELL_VAR *bash_argv_v, *bash_argc_v;
  ARRAY *bash_argv_a, *bash_argc_a;
  WORD_LIST *l;
  arrayind_t i;
  char *t;

  GET_ARRAY_FROM_VAR ("BASH_ARGV", bash_argv_v, bash_argv_a);
  GET_ARRAY_FROM_VAR ("BASH_ARGC", bash_argc_v, bash_argc_a);

  for (l = list, i = 0; l; l = l->next, i++)
    array_push (bash_argv_a, l->word->word);

  t = itos (i);
  array_push (bash_argc_a, t);
  free (t);
#endif /* ARRAY_VARS && DEBUGGER */
}

/* Remove arguments from BASH_ARGV array.  Pop top element off BASH_ARGC
   array and use that value as the count of elements to remove from
   BASH_ARGV. */
void
pop_args ()
{
#if defined (ARRAY_VARS) && defined (DEBUGGER)
  SHELL_VAR *bash_argv_v, *bash_argc_v;
  ARRAY *bash_argv_a, *bash_argc_a;
  ARRAY_ELEMENT *ce;
  intmax_t i;

  GET_ARRAY_FROM_VAR ("BASH_ARGV", bash_argv_v, bash_argv_a);
  GET_ARRAY_FROM_VAR ("BASH_ARGC", bash_argc_v, bash_argc_a);

  ce = array_unshift_element (bash_argc_a);
  if (ce == 0 || legal_number (element_value (ce), &i) == 0)
    i = 0;

  for ( ; i > 0; i--)
    array_pop (bash_argv_a);
  array_dispose_element (ce);
#endif /* ARRAY_VARS && DEBUGGER */
}

/*************************************************
 *						 *
 *	Functions to manage special variables	 *
 *						 *
 *************************************************/

/* Extern declarations for variables this code has to manage. */

/* An alist of name.function for each special variable.  Most of the
   functions don't do much, and in fact, this would be faster with a
   switch statement, but by the end of this file, I am sick of switch
   statements. */

#define SET_INT_VAR(name, intvar)  intvar = find_variable (name) != 0

/* This table will be sorted with qsort() the first time it's accessed. */
struct name_and_function {
  char *name;
  sh_sv_func_t *function;
};

static struct name_and_function special_vars[] = {
  { "BASH_COMPAT", sv_shcompat },
  { "BASH_XTRACEFD", sv_xtracefd },

#if defined (JOB_CONTROL)
  { "CHILD_MAX", sv_childmax },
#endif

#if defined (READLINE)
#  if defined (STRICT_POSIX)
  { "COLUMNS", sv_winsize },
#  endif
  { "COMP_WORDBREAKS", sv_comp_wordbreaks },
#endif

  { "EXECIGNORE", sv_execignore },

  { "FUNCNEST", sv_funcnest },

  { "GLOBIGNORE", sv_globignore },

#if defined (HISTORY)
  { "HISTCONTROL", sv_history_control },
  { "HISTFILESIZE", sv_histsize },
  { "HISTIGNORE", sv_histignore },
  { "HISTSIZE", sv_histsize },
  { "HISTTIMEFORMAT", sv_histtimefmt },
#endif

#if defined (__CYGWIN__)
  { "HOME", sv_home },
#endif

#if defined (READLINE)
  { "HOSTFILE", sv_hostfile },
#endif

  { "IFS", sv_ifs },
  { "IGNOREEOF", sv_ignoreeof },

  { "LANG", sv_locale },
  { "LC_ALL", sv_locale },
  { "LC_COLLATE", sv_locale },
  { "LC_CTYPE", sv_locale },
  { "LC_MESSAGES", sv_locale },
  { "LC_NUMERIC", sv_locale },
  { "LC_TIME", sv_locale },

#if defined (READLINE) && defined (STRICT_POSIX)
  { "LINES", sv_winsize },
#endif

  { "MAIL", sv_mail },
  { "MAILCHECK", sv_mail },
  { "MAILPATH", sv_mail },

  { "OPTERR", sv_opterr },
  { "OPTIND", sv_optind },

  { "PATH", sv_path },
  { "POSIXLY_CORRECT", sv_strict_posix },

#if defined (READLINE)
  { "TERM", sv_terminal },
  { "TERMCAP", sv_terminal },
  { "TERMINFO", sv_terminal },
#endif /* READLINE */

  { "TEXTDOMAIN", sv_locale },
  { "TEXTDOMAINDIR", sv_locale },

#if defined (HAVE_TZSET)
  { "TZ", sv_tz },
#endif

#if defined (HISTORY) && defined (BANG_HISTORY)
  { "histchars", sv_histchars },
#endif /* HISTORY && BANG_HISTORY */

  { "ignoreeof", sv_ignoreeof },

  { (char *)0, (sh_sv_func_t *)0 }
};

#define N_SPECIAL_VARS	(sizeof (special_vars) / sizeof (special_vars[0]) - 1)

static int
sv_compare (sv1, sv2)
     struct name_and_function *sv1, *sv2;
{
  int r;

  if ((r = sv1->name[0] - sv2->name[0]) == 0)
    r = strcmp (sv1->name, sv2->name);
  return r;
}

static inline int
find_special_var (name)
     const char *name;
{
  register int i, r;

  for (i = 0; special_vars[i].name; i++)
    {
      r = special_vars[i].name[0] - name[0];
      if (r == 0)
	r = strcmp (special_vars[i].name, name);
      if (r == 0)
	return i;
      else if (r > 0)
	/* Can't match any of rest of elements in sorted list.  Take this out
	   if it causes problems in certain environments. */
	break;
    }
  return -1;
}

/* The variable in NAME has just had its state changed.  Check to see if it
   is one of the special ones where something special happens. */
void
stupidly_hack_special_variables (name)
     char *name;
{
  static int sv_sorted = 0;
  int i;

  if (sv_sorted == 0)	/* shouldn't need, but it's fairly cheap. */
    {
      qsort (special_vars, N_SPECIAL_VARS, sizeof (special_vars[0]),
		(QSFUNC *)sv_compare);
      sv_sorted = 1;
    }

  i = find_special_var (name);
  if (i != -1)
    (*(special_vars[i].function)) (name);
}

/* Special variables that need hooks to be run when they are unset as part
   of shell reinitialization should have their sv_ functions run here. */
void
reinit_special_variables ()
{
#if defined (READLINE)
  sv_comp_wordbreaks ("COMP_WORDBREAKS");
#endif
  sv_globignore ("GLOBIGNORE");
  sv_opterr ("OPTERR");
}

void
sv_ifs (name)
     char *name;
{
  SHELL_VAR *v;

  v = find_variable ("IFS");
  setifs (v);
}

/* What to do just after the PATH variable has changed. */
void
sv_path (name)
     char *name;
{
  /* hash -r */
  phash_flush ();
}

/* What to do just after one of the MAILxxxx variables has changed.  NAME
   is the name of the variable.  This is called with NAME set to one of
   MAIL, MAILCHECK, or MAILPATH.  */
void
sv_mail (name)
     char *name;
{
  /* If the time interval for checking the files has changed, then
     reset the mail timer.  Otherwise, one of the pathname vars
     to the users mailbox has changed, so rebuild the array of
     filenames. */
  if (name[4] == 'C')  /* if (strcmp (name, "MAILCHECK") == 0) */
    reset_mail_timer ();
  else
    {
      free_mail_files ();
      remember_mail_dates ();
    }
}

void
sv_funcnest (name)
     char *name;
{
  SHELL_VAR *v;
  intmax_t num;

  v = find_variable (name);
  if (v == 0)
    funcnest_max = 0;
  else if (legal_number (value_cell (v), &num) == 0)
    funcnest_max = 0;
  else
    funcnest_max = num;
}

/* What to do when EXECIGNORE changes. */
void
sv_execignore (name)
     char *name;
{
  setup_exec_ignore (name);
}

/* What to do when GLOBIGNORE changes. */
void
sv_globignore (name)
     char *name;
{
  if (privileged_mode == 0)
    setup_glob_ignore (name);
}

#if defined (READLINE)
void
sv_comp_wordbreaks (name)
     char *name;
{
  SHELL_VAR *sv;

  sv = find_variable (name);
  if (sv == 0)
    reset_completer_word_break_chars ();
}

/* What to do just after one of the TERMxxx variables has changed.
   If we are an interactive shell, then try to reset the terminal
   information in readline. */
void
sv_terminal (name)
     char *name;
{
  if (interactive_shell && no_line_editing == 0)
    rl_reset_terminal (get_string_value ("TERM"));
}

void
sv_hostfile (name)
     char *name;
{
  SHELL_VAR *v;

  v = find_variable (name);
  if (v == 0)
    clear_hostname_list ();
  else
    hostname_list_initialized = 0;
}

#if defined (STRICT_POSIX)
/* In strict posix mode, we allow assignments to LINES and COLUMNS (and values
   found in the initial environment) to override the terminal size reported by
   the kernel. */
void
sv_winsize (name)
     char *name;
{
  SHELL_VAR *v;
  intmax_t xd;
  int d;

  if (posixly_correct == 0 || interactive_shell == 0 || no_line_editing)
    return;

  v = find_variable (name);
  if (v == 0 || var_isset (v) == 0)
    rl_reset_screen_size ();
  else
    {
      if (legal_number (value_cell (v), &xd) == 0)
	return;
      winsize_assignment = 1;
      d = xd;			/* truncate */
      if (name[0] == 'L')	/* LINES */
	rl_set_screen_size (d, -1);
      else			/* COLUMNS */
	rl_set_screen_size (-1, d);
      winsize_assignment = 0;
    }
}
#endif /* STRICT_POSIX */
#endif /* READLINE */

/* Update the value of HOME in the export environment so tilde expansion will
   work on cygwin. */
#if defined (__CYGWIN__)
sv_home (name)
     char *name;
{
  array_needs_making = 1;
  maybe_make_export_env ();
}
#endif

#if defined (HISTORY)
/* What to do after the HISTSIZE or HISTFILESIZE variables change.
   If there is a value for this HISTSIZE (and it is numeric), then stifle
   the history.  Otherwise, if there is NO value for this variable,
   unstifle the history.  If name is HISTFILESIZE, and its value is
   numeric, truncate the history file to hold no more than that many
   lines. */
void
sv_histsize (name)
     char *name;
{
  char *temp;
  intmax_t num;
  int hmax;

  temp = get_string_value (name);

  if (temp && *temp)
    {
      if (legal_number (temp, &num))
	{
	  hmax = num;
	  if (hmax < 0 && name[4] == 'S')
	    unstifle_history ();	/* unstifle history if HISTSIZE < 0 */
	  else if (name[4] == 'S')
	    {
	      stifle_history (hmax);
	      hmax = where_history ();
	      if (history_lines_this_session > hmax)
		history_lines_this_session = hmax;
	    }
	  else if (hmax >= 0)	/* truncate HISTFILE if HISTFILESIZE >= 0 */
	    {
	      history_truncate_file (get_string_value ("HISTFILE"), hmax);
	      /* If we just shrank the history file to fewer lines than we've
		 already read, make sure we adjust our idea of how many lines
		 we have read from the file. */
	      if (hmax < history_lines_in_file)
		history_lines_in_file = hmax;
	    }
	}
    }
  else if (name[4] == 'S')
    unstifle_history ();
}

/* What to do after the HISTIGNORE variable changes. */
void
sv_histignore (name)
     char *name;
{
  setup_history_ignore (name);
}

/* What to do after the HISTCONTROL variable changes. */
void
sv_history_control (name)
     char *name;
{
  char *temp;
  char *val;
  int tptr;

  history_control = 0;
  temp = get_string_value (name);

  if (temp == 0 || *temp == 0)
    return;

  tptr = 0;
  while (val = extract_colon_unit (temp, &tptr))
    {
      if (STREQ (val, "ignorespace"))
	history_control |= HC_IGNSPACE;
      else if (STREQ (val, "ignoredups"))
	history_control |= HC_IGNDUPS;
      else if (STREQ (val, "ignoreboth"))
	history_control |= HC_IGNBOTH;
      else if (STREQ (val, "erasedups"))
	history_control |= HC_ERASEDUPS;

      free (val);
    }
}

#if defined (BANG_HISTORY)
/* Setting/unsetting of the history expansion character. */
void
sv_histchars (name)
     char *name;
{
  char *temp;

  temp = get_string_value (name);
  if (temp)
    {
      history_expansion_char = *temp;
      if (temp[0] && temp[1])
	{
	  history_subst_char = temp[1];
	  if (temp[2])
	      history_comment_char = temp[2];
	}
    }
  else
    {
      history_expansion_char = '!';
      history_subst_char = '^';
      history_comment_char = '#';
    }
}
#endif /* BANG_HISTORY */

void
sv_histtimefmt (name)
     char *name;
{
  SHELL_VAR *v;

  if (v = find_variable (name))
    {
      if (history_comment_char == 0)
	history_comment_char = '#';
    }
  history_write_timestamps = (v != 0);
}
#endif /* HISTORY */

#if defined (HAVE_TZSET)
void
sv_tz (name)
     char *name;
{
  SHELL_VAR *v;

  v = find_variable (name);
  if (v && exported_p (v))
    array_needs_making = 1;
  else if (v == 0)
    array_needs_making = 1;

  if (array_needs_making)
    {
      maybe_make_export_env ();  
      tzset ();
    }
}
#endif

/* If the variable exists, then the value of it can be the number
   of times we actually ignore the EOF.  The default is small,
   (smaller than csh, anyway). */
void
sv_ignoreeof (name)
     char *name;
{
  SHELL_VAR *tmp_var;
  char *temp;

  eof_encountered = 0;

  tmp_var = find_variable (name);
  ignoreeof = tmp_var && var_isset (tmp_var);
  temp = tmp_var ? value_cell (tmp_var) : (char *)NULL;
  if (temp)
    eof_encountered_limit = (*temp && all_digits (temp)) ? atoi (temp) : 10;
  set_shellopts ();	/* make sure `ignoreeof' is/is not in $SHELLOPTS */
}

void
sv_optind (name)
     char *name;
{
  SHELL_VAR *var;
  char *tt;
  int s;

  var = find_variable ("OPTIND");
  tt = var ? get_variable_value (var) : (char *)NULL;

  /* Assume that if var->context < variable_context and variable_context > 0
     then we are restoring the variables's previous state while returning
     from a function. */
  if (tt && *tt)
    {
      s = atoi (tt);

      /* According to POSIX, setting OPTIND=1 resets the internal state
	 of getopt (). */
      if (s < 0 || s == 1)
	s = 0;
    }
  else
    s = 0;
  getopts_reset (s);
}

void
sv_opterr (name)
     char *name;
{
  char *tt;

  tt = get_string_value ("OPTERR");
  sh_opterr = (tt && *tt) ? atoi (tt) : 1;
}

void
sv_strict_posix (name)
     char *name;
{
  SHELL_VAR *var;

  var = find_variable (name);
  posixly_correct = var && var_isset (var);
  posix_initialize (posixly_correct);
#if defined (READLINE)
  if (interactive_shell)
    posix_readline_initialize (posixly_correct);
#endif /* READLINE */
  set_shellopts ();	/* make sure `posix' is/is not in $SHELLOPTS */
}

void
sv_locale (name)
     char *name;
{
  char *v;
  int r;

  v = get_string_value (name);
  if (name[0] == 'L' && name[1] == 'A')	/* LANG */
    r = set_lang (name, v);
  else
    r = set_locale_var (name, v);		/* LC_*, TEXTDOMAIN* */

#if 1
  if (r == 0 && posixly_correct)
    set_exit_status (EXECUTION_FAILURE);
#endif
}

#if defined (ARRAY_VARS)
void
set_pipestatus_array (ps, nproc)
     int *ps;
     int nproc;
{
  SHELL_VAR *v;
  ARRAY *a;
  ARRAY_ELEMENT *ae;
  register int i;
  char *t, tbuf[INT_STRLEN_BOUND(int) + 1];

  v = find_variable ("PIPESTATUS");
  if (v == 0)
    v = make_new_array_variable ("PIPESTATUS");
  if (array_p (v) == 0)
    return;		/* Do nothing if not an array variable. */
  a = array_cell (v);

  if (a == 0 || array_num_elements (a) == 0)
    {
      for (i = 0; i < nproc; i++)	/* was ps[i] != -1, not i < nproc */
	{
	  t = inttostr (ps[i], tbuf, sizeof (tbuf));
	  array_insert (a, i, t);
	}
      return;
    }

  /* Fast case */
  if (array_num_elements (a) == nproc && nproc == 1)
    {
#ifndef ALT_ARRAY_IMPLEMENTATION
      ae = element_forw (a->head);
#else
      ae = a->elements[0];
#endif
      ARRAY_ELEMENT_REPLACE (ae, itos (ps[0]));
    }
  else if (array_num_elements (a) <= nproc)
    {
      /* modify in array_num_elements members in place, then add */
#ifndef ALT_ARRAY_IMPLEMENTATION
      ae = a->head;
#endif
      for (i = 0; i < array_num_elements (a); i++)
	{
#ifndef ALT_ARRAY_IMPLEMENTATION
	  ae = element_forw (ae);
#else
	  ae = a->elements[i];
#endif
	  ARRAY_ELEMENT_REPLACE (ae, itos (ps[i]));
	}
      /* add any more */
      for ( ; i < nproc; i++)
	{
	  t = inttostr (ps[i], tbuf, sizeof (tbuf));
	  array_insert (a, i, t);
	}
    }
  else
    {
#ifndef ALT_ARRAY_IMPLEMENTATION
      /* deleting elements.  it's faster to rebuild the array. */	  
      array_flush (a);
      for (i = 0; i < nproc; i++)
	{
	  t = inttostr (ps[i], tbuf, sizeof (tbuf));
	  array_insert (a, i, t);
	}
#else
      /* deleting elements. replace the first NPROC, free the rest */
      for (i = 0; i < nproc; i++)
	{
	  ae = a->elements[i];
	  ARRAY_ELEMENT_REPLACE (ae, itos (ps[i]));
	}
      for ( ; i <= array_max_index (a); i++)
	{
	  array_dispose_element (a->elements[i]);
	  a->elements[i] = (ARRAY_ELEMENT *)NULL;
	}

      /* bookkeeping usually taken care of by array_insert */
      set_max_index (a, nproc - 1);
      set_first_index (a, 0);
      set_num_elements (a, nproc);
#endif /* ALT_ARRAY_IMPLEMENTATION */
    }
}

ARRAY *
save_pipestatus_array ()
{
  SHELL_VAR *v;
  ARRAY *a;

  v = find_variable ("PIPESTATUS");
  if (v == 0 || array_p (v) == 0 || array_cell (v) == 0)
    return ((ARRAY *)NULL);
    
  a = array_copy (array_cell (v));

  return a;
}

void
restore_pipestatus_array (a)
     ARRAY *a;
{
  SHELL_VAR *v;
  ARRAY *a2;

  v = find_variable ("PIPESTATUS");
  /* XXX - should we still assign even if existing value is NULL? */
  if (v == 0 || array_p (v) == 0 || array_cell (v) == 0)
    return;

  a2 = array_cell (v);
  var_setarray (v, a); 

  array_dispose (a2);
}
#endif

void
set_pipestatus_from_exit (s)
     int s;
{
#if defined (ARRAY_VARS)
  static int v[2] = { 0, -1 };

  v[0] = s;
  set_pipestatus_array (v, 1);
#endif
}

void
sv_xtracefd (name)
     char *name;
{
  SHELL_VAR *v;
  char *t, *e;
  int fd;
  FILE *fp;

  v = find_variable (name);
  if (v == 0)
    {
      xtrace_reset ();
      return;
    }

  t = value_cell (v);
  if (t == 0 || *t == 0)
    xtrace_reset ();
  else
    {
      fd = (int)strtol (t, &e, 10);
      if (e != t && *e == '\0' && sh_validfd (fd))
	{
	  fp = fdopen (fd, "w");
	  if (fp == 0)
	    internal_error (_("%s: %s: cannot open as FILE"), name, value_cell (v));
	  else
	    xtrace_set (fd, fp);
	}
      else
	internal_error (_("%s: %s: invalid value for trace file descriptor"), name, value_cell (v));
    }
}

#define MIN_COMPAT_LEVEL 31

void
sv_shcompat (name)
     char *name;
{
  SHELL_VAR *v;
  char *val;
  int tens, ones, compatval;

  v = find_variable (name);
  if (v == 0)
    {
      shell_compatibility_level = DEFAULT_COMPAT_LEVEL;
      set_compatibility_opts ();
      return;
    }
  val = value_cell (v);
  if (val == 0 || *val == '\0')
    {
      shell_compatibility_level = DEFAULT_COMPAT_LEVEL;
      set_compatibility_opts ();
      return;
    }
  /* Handle decimal-like compatibility version specifications: 4.2 */
  if (ISDIGIT (val[0]) && val[1] == '.' && ISDIGIT (val[2]) && val[3] == 0)
    {
      tens = val[0] - '0';
      ones = val[2] - '0';
      compatval = tens*10 + ones;
    }
  /* Handle integer-like compatibility version specifications: 42 */
  else if (ISDIGIT (val[0]) && ISDIGIT (val[1]) && val[2] == 0)
    {
      tens = val[0] - '0';
      ones = val[1] - '0';
      compatval = tens*10 + ones;
    }
  else
    {
compat_error:
      internal_error (_("%s: %s: compatibility value out of range"), name, val);
      shell_compatibility_level = DEFAULT_COMPAT_LEVEL;
      set_compatibility_opts ();
      return;
    }

  if (compatval < MIN_COMPAT_LEVEL || compatval > DEFAULT_COMPAT_LEVEL)
    goto compat_error;

  shell_compatibility_level = compatval;
  set_compatibility_opts ();
}

#if defined (JOB_CONTROL)
void
sv_childmax (name)
     char *name;
{
  char *tt;
  int s;

  tt = get_string_value (name);
  s = (tt && *tt) ? atoi (tt) : 0;
  set_maxchild (s);
}
#endif
