/* set.c, created from set.def. */
#line 22 "./set.def"

#include "config.h"

#if defined (HAVE_UNISTD_H)
#  ifdef _MINIX
#    include <sys/types.h>
#  endif
#  include <unistd.h>
#endif

#include <stdio.h>

#include "bashansi.h"
#include "bashintl.h"

#include "shell.h"
#include "parser.h"
#include "flags.h"
#include "common.h"
#include "bashgetopt.h"

#if defined (READLINE)
#  include "input.h"
#  include "bashline.h"
#  include "third_party/readline/readline.h"
#endif

#if defined (HISTORY)
#  include "bashhist.h"
#endif

#line 153 "./set.def"

typedef int setopt_set_func_t PARAMS((int, char *));
typedef int setopt_get_func_t PARAMS((char *));

static int find_minus_o_option PARAMS((char *));

static void print_minus_o_option PARAMS((char *, int, int));
static void print_all_shell_variables PARAMS((void));

static int set_ignoreeof PARAMS((int, char *));
static int set_posix_mode PARAMS((int, char *));

#if defined (READLINE)
static int set_edit_mode PARAMS((int, char *));
static int get_edit_mode PARAMS((char *));
#endif

#if defined (HISTORY)
static int bash_set_history PARAMS((int, char *));
#endif

static const char * const on = "on";
static const char * const off = "off";

static int previous_option_value;

/* A struct used to match long options for set -o to the corresponding
   option letter or internal variable.  The functions can be called to
   dynamically generate values.  If you add a new variable name here
   that doesn't have a corresponding single-character option letter, make
   sure to set the value appropriately in reset_shell_options. */
const struct {
  char *name;
  int letter;
  int *variable;
  setopt_set_func_t *set_func;
  setopt_get_func_t *get_func;
} o_options[] = {
  { "allexport",  'a', (int *)NULL, (setopt_set_func_t *)NULL, (setopt_get_func_t *)NULL },
#if defined (BRACE_EXPANSION)
  { "braceexpand",'B', (int *)NULL, (setopt_set_func_t *)NULL, (setopt_get_func_t *)NULL  },
#endif
#if defined (READLINE)
  { "emacs",     '\0', (int *)NULL, set_edit_mode, get_edit_mode },
#endif
  { "errexit",	  'e', (int *)NULL, (setopt_set_func_t *)NULL, (setopt_get_func_t *)NULL  },
  { "errtrace",	  'E', (int *)NULL, (setopt_set_func_t *)NULL, (setopt_get_func_t *)NULL  },
  { "functrace",  'T', (int *)NULL, (setopt_set_func_t *)NULL, (setopt_get_func_t *)NULL  },
  { "hashall",    'h', (int *)NULL, (setopt_set_func_t *)NULL, (setopt_get_func_t *)NULL  },
#if defined (BANG_HISTORY)
  { "histexpand", 'H', (int *)NULL, (setopt_set_func_t *)NULL, (setopt_get_func_t *)NULL  },
#endif /* BANG_HISTORY */
#if defined (HISTORY)
  { "history",   '\0', &enable_history_list, bash_set_history, (setopt_get_func_t *)NULL },
#endif
  { "ignoreeof", '\0', &ignoreeof, set_ignoreeof, (setopt_get_func_t *)NULL },
  { "interactive-comments", '\0', &interactive_comments, (setopt_set_func_t *)NULL, (setopt_get_func_t *)NULL },
  { "keyword",    'k', (int *)NULL, (setopt_set_func_t *)NULL, (setopt_get_func_t *)NULL  },
#if defined (JOB_CONTROL)
  { "monitor",	  'm', (int *)NULL, (setopt_set_func_t *)NULL, (setopt_get_func_t *)NULL  },
#endif
  { "noclobber",  'C', (int *)NULL, (setopt_set_func_t *)NULL, (setopt_get_func_t *)NULL  },
  { "noexec",	  'n', (int *)NULL, (setopt_set_func_t *)NULL, (setopt_get_func_t *)NULL  },
  { "noglob",	  'f', (int *)NULL, (setopt_set_func_t *)NULL, (setopt_get_func_t *)NULL  },
#if defined (HISTORY)
  { "nolog",     '\0', &dont_save_function_defs, (setopt_set_func_t *)NULL, (setopt_get_func_t *)NULL },
#endif
#if defined (JOB_CONTROL)
  { "notify",	  'b', (int *)NULL, (setopt_set_func_t *)NULL, (setopt_get_func_t *)NULL  },
#endif /* JOB_CONTROL */
  { "nounset",	  'u', (int *)NULL, (setopt_set_func_t *)NULL, (setopt_get_func_t *)NULL  },
  { "onecmd",	  't', (int *)NULL, (setopt_set_func_t *)NULL, (setopt_get_func_t *)NULL },
  { "physical",   'P', (int *)NULL, (setopt_set_func_t *)NULL, (setopt_get_func_t *)NULL  },
  { "pipefail",  '\0', &pipefail_opt, (setopt_set_func_t *)NULL, (setopt_get_func_t *)NULL  },
  { "posix",     '\0', &posixly_correct, set_posix_mode, (setopt_get_func_t *)NULL },
  { "privileged", 'p', (int *)NULL, (setopt_set_func_t *)NULL, (setopt_get_func_t *)NULL  },
  { "verbose",	  'v', (int *)NULL, (setopt_set_func_t *)NULL, (setopt_get_func_t *)NULL  },
#if defined (READLINE)
  { "vi",        '\0', (int *)NULL, set_edit_mode, get_edit_mode },
#endif
  { "xtrace",	  'x', (int *)NULL, (setopt_set_func_t *)NULL, (setopt_get_func_t *)NULL  },
  {(char *)NULL, 0 , (int *)NULL, (setopt_set_func_t *)NULL, (setopt_get_func_t *)NULL },
};

#define N_O_OPTIONS	(sizeof (o_options) / sizeof (o_options[0]))

#define GET_BINARY_O_OPTION_VALUE(i, name) \
  ((o_options[i].get_func) ? (*o_options[i].get_func) (name) \
			   : (*o_options[i].variable))

#define SET_BINARY_O_OPTION_VALUE(i, onoff, name) \
  ((o_options[i].set_func) ? (*o_options[i].set_func) (onoff, name) \
			   : (*o_options[i].variable = (onoff == FLAG_ON)))

static int
find_minus_o_option (name)
     char *name;
{
  register int i;

  for (i = 0; o_options[i].name; i++)
    if (STREQ (name, o_options[i].name))
      return i;
  return -1;
}

int
minus_o_option_value (name)
     char *name;
{
  register int	i;
  int *on_or_off;

  i = find_minus_o_option (name);
  if (i < 0)
    return (-1);

  if (o_options[i].letter)
    {
      on_or_off = find_flag (o_options[i].letter);
      return ((on_or_off == FLAG_UNKNOWN) ? -1 : *on_or_off);
    }
  else
    return (GET_BINARY_O_OPTION_VALUE (i, name));
}

#define MINUS_O_FORMAT "%-15s\t%s\n"

static void
print_minus_o_option (name, value, pflag)
     char *name;
     int value, pflag;
{
  if (pflag == 0)
    printf (MINUS_O_FORMAT, name, value ? on : off);
  else
    printf ("set %co %s\n", value ? '-' : '+', name);
}

void
list_minus_o_opts (mode, reusable)
     int mode, reusable;
{
  register int	i;
  int *on_or_off, value;

  for (i = 0; o_options[i].name; i++)
    {
      if (o_options[i].letter)
	{
	  value = 0;
	  on_or_off = find_flag (o_options[i].letter);
	  if (on_or_off == FLAG_UNKNOWN)
	    on_or_off = &value;
	  if (mode == -1 || mode == *on_or_off)
	    print_minus_o_option (o_options[i].name, *on_or_off, reusable);
	}
      else
	{
	  value = GET_BINARY_O_OPTION_VALUE (i, o_options[i].name);
	  if (mode == -1 || mode == value)
	    print_minus_o_option (o_options[i].name, value, reusable);
	}
    }
}

char **
get_minus_o_opts ()
{
  char **ret;
  int i;

  ret = strvec_create (N_O_OPTIONS + 1);
  for (i = 0; o_options[i].name; i++)
    ret[i] = o_options[i].name;
  ret[i] = (char *)NULL;
  return ret;
}

char *
get_current_options ()
{
  char *temp;
  int i, posixopts;

  posixopts = num_posix_options ();	/* shopts modified by posix mode */
  /* Make the buffer big enough to hold the set -o options and the shopt
     options modified by posix mode. */
  temp = (char *)xmalloc (1 + N_O_OPTIONS + posixopts);
  for (i = 0; o_options[i].name; i++)
    {
      if (o_options[i].letter)
	temp[i] = *(find_flag (o_options[i].letter));
      else
	temp[i] = GET_BINARY_O_OPTION_VALUE (i, o_options[i].name);
    }

  /* Add the shell options that are modified by posix mode to the end of the
     bitmap. They will be handled in set_current_options() */
  get_posix_options (temp+i);
  temp[i+posixopts] = '\0';
  return (temp);
}

void
set_current_options (bitmap)
     const char *bitmap;
{
  int i, v, cv, *on_or_off;

  if (bitmap == 0)
    return;

  for (i = 0; o_options[i].name; i++)
    {
      v = bitmap[i] ? FLAG_ON : FLAG_OFF;
      if (o_options[i].letter)
	{
	  /* We should not get FLAG_UNKNOWN here */
	  on_or_off = find_flag (o_options[i].letter);
	  cv = *on_or_off ? FLAG_ON : FLAG_OFF;
	  if (v != cv)
	    change_flag (o_options[i].letter, v);
	}
      else
	{
	  cv = GET_BINARY_O_OPTION_VALUE (i, o_options[i].name);
	  cv = cv ? FLAG_ON : FLAG_OFF;
	  if (v != cv)
	    SET_BINARY_O_OPTION_VALUE (i, v, o_options[i].name);
	}
    }

  /* Now reset the variables changed by posix mode */
  set_posix_options (bitmap+i);
}

static int
set_ignoreeof (on_or_off, option_name)
     int on_or_off;
     char *option_name;
{
  ignoreeof = on_or_off == FLAG_ON;
  unbind_variable_noref ("ignoreeof");
  if (ignoreeof)
    bind_variable ("IGNOREEOF", "10", 0); 
  else
    unbind_variable_noref ("IGNOREEOF");
  sv_ignoreeof ("IGNOREEOF");
  return 0;
}

static int
set_posix_mode (on_or_off, option_name)
     int on_or_off;
     char *option_name;
{
  /* short-circuit on no-op */
  if ((on_or_off == FLAG_ON && posixly_correct) ||
      (on_or_off == FLAG_OFF && posixly_correct == 0))
    return 0;

  posixly_correct = on_or_off == FLAG_ON;
  if (posixly_correct == 0)
    unbind_variable_noref ("POSIXLY_CORRECT");
  else
    bind_variable ("POSIXLY_CORRECT", "y", 0);
  sv_strict_posix ("POSIXLY_CORRECT");
  return (0);
}

#if defined (READLINE)
/* Magic.  This code `knows' how readline handles rl_editing_mode. */
static int
set_edit_mode (on_or_off, option_name)
     int on_or_off;
     char *option_name;
{
  int isemacs;

  if (on_or_off == FLAG_ON)
    {
      rl_variable_bind ("editing-mode", option_name);

      if (interactive)
	with_input_from_stdin ();
      no_line_editing = 0;
    }
  else
    {
      isemacs = rl_editing_mode == 1;
      if ((isemacs && *option_name == 'e') || (!isemacs && *option_name == 'v'))
	{
	  if (interactive)
	    with_input_from_stream (stdin, "stdin");
	  no_line_editing = 1;
	}
    }
  return 1-no_line_editing;
}

static int
get_edit_mode (name)
     char *name;
{
  return (*name == 'e' ? no_line_editing == 0 && rl_editing_mode == 1
		       : no_line_editing == 0 && rl_editing_mode == 0);
}
#endif /* READLINE */

#if defined (HISTORY)
static int
bash_set_history (on_or_off, option_name)
     int on_or_off;
     char *option_name;
{
  if (on_or_off == FLAG_ON)
    {
      enable_history_list = 1;
      bash_history_enable ();
      if (history_lines_this_session == 0)
	load_history ();
    }
  else
    {
      enable_history_list = 0;
      bash_history_disable ();
    }
  return (1 - enable_history_list);
}
#endif

int
set_minus_o_option (on_or_off, option_name)
     int on_or_off;
     char *option_name;
{
  register int i;

  i = find_minus_o_option (option_name);
  if (i < 0)
    {
      sh_invalidoptname (option_name);
      return (EX_USAGE);
    }

  if (o_options[i].letter == 0)
    {
      previous_option_value = GET_BINARY_O_OPTION_VALUE (i, o_options[i].name);
      SET_BINARY_O_OPTION_VALUE (i, on_or_off, option_name);
      return (EXECUTION_SUCCESS);
    }
  else
    {
      if ((previous_option_value = change_flag (o_options[i].letter, on_or_off)) == FLAG_ERROR)
	{
	  sh_invalidoptname (option_name);
	  return (EXECUTION_FAILURE);
	}
      else
	return (EXECUTION_SUCCESS);
    }
}

static void
print_all_shell_variables ()
{
  SHELL_VAR **vars;

  vars = all_shell_variables ();
  if (vars)
    {
      print_var_list (vars);
      free (vars);
    }

  /* POSIX.2 does not allow function names and definitions to be output when
     `set' is invoked without options (PASC Interp #202). */
  if (posixly_correct == 0)
    {
      vars = all_shell_functions ();
      if (vars)
	{
	  print_func_list (vars);
	  free (vars);
	}
    }
}

void
set_shellopts ()
{
  char *value;
  char tflag[N_O_OPTIONS];
  int vsize, i, vptr, *ip, exported;
  SHELL_VAR *v;

  for (vsize = i = 0; o_options[i].name; i++)
    {
      tflag[i] = 0;
      if (o_options[i].letter)
	{
	  ip = find_flag (o_options[i].letter);
	  if (ip && *ip)
	    {
	      vsize += strlen (o_options[i].name) + 1;
	      tflag[i] = 1;
	    }
	}
      else if (GET_BINARY_O_OPTION_VALUE (i, o_options[i].name))
	{
	  vsize += strlen (o_options[i].name) + 1;
	  tflag[i] = 1;
	}
    }

  value = (char *)xmalloc (vsize + 1);

  for (i = vptr = 0; o_options[i].name; i++)
    {
      if (tflag[i])
	{
	  strcpy (value + vptr, o_options[i].name);
	  vptr += strlen (o_options[i].name);
	  value[vptr++] = ':';
	}
    }

  if (vptr)
    vptr--;			/* cut off trailing colon */
  value[vptr] = '\0';

  v = find_variable ("SHELLOPTS");

  /* Turn off the read-only attribute so we can bind the new value, and
     note whether or not the variable was exported. */
  if (v)
    {
      VUNSETATTR (v, att_readonly);
      exported = exported_p (v);
    }
  else
    exported = 0;

  v = bind_variable ("SHELLOPTS", value, 0);

  /* Turn the read-only attribute back on, and turn off the export attribute
     if it was set implicitly by mark_modified_vars and SHELLOPTS was not
     exported before we bound the new value. */
  VSETATTR (v, att_readonly);
  if (mark_modified_vars && exported == 0 && exported_p (v))
    VUNSETATTR (v, att_exported);

  free (value);
}

void
parse_shellopts (value)
     char *value;
{
  char *vname;
  int vptr;

  vptr = 0;
  while (vname = extract_colon_unit (value, &vptr))
    {
      set_minus_o_option (FLAG_ON, vname);
      free (vname);
    }
}

void
initialize_shell_options (no_shellopts)
     int no_shellopts;
{
  char *temp;
  SHELL_VAR *var;

  if (no_shellopts == 0)
    {
      var = find_variable ("SHELLOPTS");
      /* set up any shell options we may have inherited. */
      if (var && imported_p (var))
	{
	  temp = (array_p (var) || assoc_p (var)) ? (char *)NULL : savestring (value_cell (var));
	  if (temp)
	    {
	      parse_shellopts (temp);
	      free (temp);
	    }
	}
    }

  /* Set up the $SHELLOPTS variable. */
  set_shellopts ();
}

/* Reset the values of the -o options that are not also shell flags.  This is
   called from execute_cmd.c:initialize_subshell() when setting up a subshell
   to run an executable shell script without a leading `#!'. */
void
reset_shell_options ()
{
  pipefail_opt = 0;
  ignoreeof = 0;

#if defined (STRICT_POSIX)
  posixly_correct = 1;
#else
  posixly_correct = 0;
#endif
#if defined (HISTORY)
  dont_save_function_defs = 0;
  remember_on_history = enable_history_list = 1;	/* XXX */
#endif
}

/* Set some flags from the word values in the input list.  If LIST is empty,
   then print out the values of the variables instead.  If LIST contains
   non-flags, then set $1 - $9 to the successive words of LIST. */
int
set_builtin (list)
     WORD_LIST *list;
{
  int on_or_off, flag_name, force_assignment, opts_changed, rv, r;
  register char *arg;
  char s[3];

  if (list == 0)
    {
      print_all_shell_variables ();
      return (sh_chkwrite (EXECUTION_SUCCESS));
    }

  /* Check validity of flag arguments. */
  rv = EXECUTION_SUCCESS;
  reset_internal_getopt ();
  while ((flag_name = internal_getopt (list, optflags)) != -1)
    {
      switch (flag_name)
	{
	  case 'i':	/* don't allow set -i */
	    s[0] = list_opttype;
	    s[1] = 'i';
	    s[2] = '\0';
	    sh_invalidopt (s);
	    builtin_usage ();
	    return (EX_USAGE);
	  CASE_HELPOPT;
	  case '?':
	    builtin_usage ();
	    return (list_optopt == '?' ? EXECUTION_SUCCESS : EX_USAGE);
	  default:
	    break;
	}
    }
    
  /* Do the set command.  While the list consists of words starting with
     '-' or '+' treat them as flags, otherwise, start assigning them to
     $1 ... $n. */
  for (force_assignment = opts_changed = 0; list; )
    {
      arg = list->word->word;

      /* If the argument is `--' or `-' then signal the end of the list
	 and remember the remaining arguments. */
      if (arg[0] == '-' && (!arg[1] || (arg[1] == '-' && !arg[2])))
	{
	  list = list->next;

	  /* `set --' unsets the positional parameters. */
	  if (arg[1] == '-')
	    force_assignment = 1;

	  /* Until told differently, the old shell behaviour of
	     `set - [arg ...]' being equivalent to `set +xv [arg ...]'
	     stands.  Posix.2 says the behaviour is marked as obsolescent. */
	  else
	    {
	      change_flag ('x', '+');
	      change_flag ('v', '+');
	      opts_changed = 1;
	    }

	  break;
	}

      if ((on_or_off = *arg) && (on_or_off == '-' || on_or_off == '+'))
	{
	  while (flag_name = *++arg)
	    {
	      if (flag_name == '?')
		{
		  builtin_usage ();
		  return (EXECUTION_SUCCESS);
		}
	      else if (flag_name == 'o') /* -+o option-name */
		{
		  char *option_name;
		  WORD_LIST *opt;

		  opt = list->next;

		  if (opt == 0)
		    {
		      list_minus_o_opts (-1, (on_or_off == '+'));
		      rv = sh_chkwrite (rv);
		      continue;
		    }

		  option_name = opt->word->word;

		  if (option_name == 0 || *option_name == '\0' ||
		      *option_name == '-' || *option_name == '+')
		    {
		      list_minus_o_opts (-1, (on_or_off == '+'));
		      continue;
		    }
		  list = list->next; /* Skip over option name. */

		  opts_changed = 1;
		  if ((r = set_minus_o_option (on_or_off, option_name)) != EXECUTION_SUCCESS)
		    {
		      set_shellopts ();
		      return (r);
		    }
		}
	      else if (change_flag (flag_name, on_or_off) == FLAG_ERROR)
		{
		  s[0] = on_or_off;
		  s[1] = flag_name;
		  s[2] = '\0';
		  sh_invalidopt (s);
		  builtin_usage ();
		  set_shellopts ();
		  return (EXECUTION_FAILURE);
		}
	      opts_changed = 1;
	    }
	}
      else
	{
	  break;
	}
      list = list->next;
    }

  /* Assigning $1 ... $n */
  if (list || force_assignment)
    remember_args (list, 1);
  /* Set up new value of $SHELLOPTS */
  if (opts_changed)
    set_shellopts ();
  return (rv);
}

#line 830 "./set.def"

#define NEXT_VARIABLE()	any_failed++; list = list->next; continue;

int
unset_builtin (list)
  WORD_LIST *list;
{
  int unset_function, unset_variable, unset_array, opt, nameref, any_failed;
  int global_unset_func, global_unset_var, vflags, base_vflags, valid_id;
  char *name, *tname;

  unset_function = unset_variable = unset_array = nameref = any_failed = 0;
  global_unset_func = global_unset_var = 0;

  reset_internal_getopt ();
  while ((opt = internal_getopt (list, "fnv")) != -1)
    {
      switch (opt)
	{
	case 'f':
	  global_unset_func = 1;
	  break;
	case 'v':
	  global_unset_var = 1;
	  break;
	case 'n':
	  nameref = 1;
	  break;
	CASE_HELPOPT;
	default:
	  builtin_usage ();
	  return (EX_USAGE);
	}
    }

  list = loptend;

  if (global_unset_func && global_unset_var)
    {
      builtin_error (_("cannot simultaneously unset a function and a variable"));
      return (EXECUTION_FAILURE);
    }
  else if (unset_function && nameref)
    nameref = 0;

#if defined (ARRAY_VARS)
  base_vflags = assoc_expand_once ? VA_NOEXPAND : 0;
#endif

  while (list)
    {
      SHELL_VAR *var;
      int tem;
#if defined (ARRAY_VARS)
      char *t;
#endif

      name = list->word->word;

      unset_function = global_unset_func;
      unset_variable = global_unset_var;

#if defined (ARRAY_VARS)
      vflags = builtin_arrayref_flags (list->word, base_vflags);
#endif

#if defined (ARRAY_VARS)
      unset_array = 0;
      /* XXX valid array reference second arg was 0 */
      if (!unset_function && nameref == 0 && tokenize_array_reference (name, vflags, &t))
	unset_array = 1;
#endif
      /* Get error checking out of the way first.  The low-level functions
	 just perform the unset, relying on the caller to verify. */
      valid_id = legal_identifier (name);

      /* Whether or not we are in posix mode, if neither -f nor -v appears,
	 skip over trying to unset variables with invalid names and just
	 treat them as potential shell function names. */
      if (global_unset_func == 0 && global_unset_var == 0 && valid_id == 0)
	{
	  unset_variable = unset_array = 0;
	  unset_function = 1;
	}

      /* Bash allows functions with names which are not valid identifiers
	 to be created when not in posix mode, so check only when in posix
	 mode when unsetting a function. */
      if (unset_function == 0 && valid_id == 0)
	{
	  sh_invalidid (name);
	  NEXT_VARIABLE ();
	}

      /* Search for functions here if -f supplied or if NAME cannot be a
	 variable name. */
      var = unset_function ? find_function (name)
			   : (nameref ? find_variable_last_nameref (name, 0) : find_variable (name));

      /* Some variables (but not functions yet) cannot be unset, period. */
      if (var && unset_function == 0 && non_unsettable_p (var))
	{
	  builtin_error (_("%s: cannot unset"), name);
	  NEXT_VARIABLE ();
	}

      /* if we have a nameref we want to use it */
      if (var && unset_function == 0 && nameref == 0 && STREQ (name, name_cell(var)) == 0)
	name = name_cell (var);

      /* Posix.2 says try variables first, then functions.  If we would
	 find a function after unsuccessfully searching for a variable,
	 note that we're acting on a function now as if -f were
	 supplied.  The readonly check below takes care of it. */
      if (var == 0 && nameref == 0 &&  unset_variable == 0 && unset_function == 0)
	{
	  if (var = find_function (name))
	    unset_function = 1;
	}

      /* Posix.2 says that unsetting readonly variables is an error. */
      if (var && readonly_p (var))
	{
	  builtin_error (_("%s: cannot unset: readonly %s"),
			 var->name, unset_function ? "function" : "variable");
	  NEXT_VARIABLE ();
	}

      /* Unless the -f option is supplied, the name refers to a variable. */
#if defined (ARRAY_VARS)
      if (var && unset_array)
	{
	  if (shell_compatibility_level <= 51)
	    vflags |= VA_ALLOWALL;

	  /* Let unbind_array_element decide what to do with non-array vars */
	  tem = unbind_array_element (var, t, vflags);	/* XXX new third arg */
	  if (tem == -2 && array_p (var) == 0 && assoc_p (var) == 0)
	    {
	      builtin_error (_("%s: not an array variable"), var->name);
	      NEXT_VARIABLE ();
	    }
	  else if (tem < 0)
	    any_failed++;
	}
      else
#endif /* ARRAY_VARS */
      /* If we're trying to unset a nameref variable whose value isn't a set
	 variable, make sure we still try to unset the nameref's value */
      if (var == 0 && nameref == 0 && unset_function == 0)
	{
	  var = find_variable_last_nameref (name, 0);
	  if (var && nameref_p (var))
	    {
#if defined (ARRAY_VARS)
	      if (valid_array_reference (nameref_cell (var), 0))
		{
		  int len;

		  tname = savestring (nameref_cell (var));
		  if (var = array_variable_part (tname, 0, &t, &len))
		    {
		      /* change to what unbind_array_element now expects */
		      if (t[len - 1] == ']')
			t[len - 1] = 0;
		      tem = unbind_array_element (var, t, vflags);	/* XXX new third arg */
		    }
		  free (tname);
		}
	      else
#endif
		tem = unbind_variable (nameref_cell (var));
	    }
	  else
	    tem = unbind_variable (name);
	}
      else
	tem = unset_function ? unbind_func (name) : (nameref ? unbind_nameref (name) : unbind_variable (name));

      /* This is what Posix.2 says:  ``If neither -f nor -v
	 is specified, the name refers to a variable; if a variable by
	 that name does not exist, a function by that name, if any,
	 shall be unset.'' */
      if (tem == -1 && nameref == 0 && unset_function == 0 && unset_variable == 0)
	tem = unbind_func (name);

      name = list->word->word;		/* reset above for namerefs */

      /* SUSv3, POSIX.1-2001 say:  ``Unsetting a variable or function that
	 was not previously set shall not be considered an error.'' */

      if (unset_function == 0)
	stupidly_hack_special_variables (name);

      list = list->next;
    }

  return (any_failed ? EXECUTION_FAILURE : EXECUTION_SUCCESS);
}
