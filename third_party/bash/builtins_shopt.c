/* shopt.c, created from shopt.def. */
#line 22 "./shopt.def"

#line 43 "./shopt.def"

#include "config.h"

#if defined (HAVE_UNISTD_H)
#  ifdef _MINIX
#    include <sys/types.h>
#  endif
#  include <unistd.h>
#endif

#include <stdio.h>

#include "version.h"

#include "bashintl.h"

#include "shell.h"
#include "flags.h"
#include "common.h"
#include "bashgetopt.h"

#if defined (READLINE)
#  include "bashline.h"
#endif

#if defined (HISTORY)
#  include "bashhist.h"
#endif

#define UNSETOPT	0
#define SETOPT		1

#define OPTFMT		"%-15s\t%s\n"

extern int allow_null_glob_expansion, fail_glob_expansion, glob_dot_filenames;
extern int cdable_vars, mail_warning, source_uses_path;
extern int no_exit_on_failed_exec, print_shift_error;
extern int check_hashed_filenames, promptvars;
extern int cdspelling, expand_aliases;
extern int extended_quote;
extern int check_window_size;
extern int glob_ignore_case, match_ignore_case;
extern int hup_on_exit;
extern int xpg_echo;
extern int gnu_error_format;
extern int check_jobs_at_exit;
extern int autocd;
extern int glob_star;
extern int glob_asciirange;
extern int glob_always_skip_dot_and_dotdot;
extern int lastpipe_opt;
extern int inherit_errexit;
extern int localvar_inherit;
extern int localvar_unset;
extern int varassign_redir_autoclose;
extern int singlequote_translations;
extern int patsub_replacement;

#if defined (EXTENDED_GLOB)
extern int extended_glob;
#endif

#if defined (READLINE)
extern int hist_verify, history_reediting, perform_hostname_completion;
extern int no_empty_command_completion;
extern int force_fignore;
extern int dircomplete_spelling, dircomplete_expand;
extern int complete_fullquote;

extern int enable_hostname_completion PARAMS((int));
#endif

#if defined (PROGRAMMABLE_COMPLETION)
extern int prog_completion_enabled;
extern int progcomp_alias;
#endif

#if defined (DEBUGGER)
extern int debugging_mode;
#endif

#if defined (ARRAY_VARS)
extern int assoc_expand_once;
extern int array_expand_once;
int expand_once_flag;
#endif

#if defined (SYSLOG_HISTORY)
extern int syslog_history;
#endif

static void shopt_error PARAMS((char *));

static int set_shellopts_after_change PARAMS((char *, int));
static int set_compatibility_level PARAMS((char *, int));

#if defined (RESTRICTED_SHELL)
static int set_restricted_shell PARAMS((char *, int));
#endif

#if defined (READLINE)
static int shopt_enable_hostname_completion PARAMS((char *, int));
static int shopt_set_complete_direxpand PARAMS((char *, int));
#endif

#if defined (ARRAY_VARS)
static int set_assoc_expand PARAMS((char *, int));
#endif

static int shopt_set_debug_mode PARAMS((char *, int));

static int shopt_login_shell;
static int shopt_compat31;
static int shopt_compat32;
static int shopt_compat40;
static int shopt_compat41;
static int shopt_compat42;
static int shopt_compat43;
static int shopt_compat44;

typedef int shopt_set_func_t PARAMS((char *, int));

/* If you add a new variable name here, make sure to set the default value
   appropriately in reset_shopt_options. */

static struct {
  char *name;
  int  *value;
  shopt_set_func_t *set_func;
} shopt_vars[] = {
  { "autocd", &autocd, (shopt_set_func_t *)NULL },
#if defined (ARRAY_VARS)
  { "assoc_expand_once", &expand_once_flag, set_assoc_expand },
#endif
  { "cdable_vars", &cdable_vars, (shopt_set_func_t *)NULL },
  { "cdspell", &cdspelling, (shopt_set_func_t *)NULL },
  { "checkhash", &check_hashed_filenames, (shopt_set_func_t *)NULL },
#if defined (JOB_CONTROL)
  { "checkjobs", &check_jobs_at_exit, (shopt_set_func_t *)NULL },
#endif
  { "checkwinsize", &check_window_size, (shopt_set_func_t *)NULL },
#if defined (HISTORY)
  { "cmdhist", &command_oriented_history, (shopt_set_func_t *)NULL },
#endif
  { "compat31", &shopt_compat31, set_compatibility_level },
  { "compat32", &shopt_compat32, set_compatibility_level },
  { "compat40", &shopt_compat40, set_compatibility_level },
  { "compat41", &shopt_compat41, set_compatibility_level },
  { "compat42", &shopt_compat42, set_compatibility_level },
  { "compat43", &shopt_compat43, set_compatibility_level },
  { "compat44", &shopt_compat44, set_compatibility_level },
#if defined (READLINE)
  { "complete_fullquote", &complete_fullquote, (shopt_set_func_t *)NULL},
  { "direxpand", &dircomplete_expand, shopt_set_complete_direxpand },
  { "dirspell", &dircomplete_spelling, (shopt_set_func_t *)NULL },
#endif
  { "dotglob", &glob_dot_filenames, (shopt_set_func_t *)NULL },
  { "execfail", &no_exit_on_failed_exec, (shopt_set_func_t *)NULL },
  { "expand_aliases", &expand_aliases, (shopt_set_func_t *)NULL },
#if defined (DEBUGGER)
  { "extdebug", &debugging_mode, shopt_set_debug_mode },
#endif
#if defined (EXTENDED_GLOB)
  { "extglob", &extended_glob, (shopt_set_func_t *)NULL },
#endif
  { "extquote", &extended_quote, (shopt_set_func_t *)NULL },
  { "failglob", &fail_glob_expansion, (shopt_set_func_t *)NULL },
#if defined (READLINE)
  { "force_fignore", &force_fignore, (shopt_set_func_t *)NULL },
#endif
  { "globasciiranges", &glob_asciirange, (shopt_set_func_t *)NULL },
  { "globskipdots", &glob_always_skip_dot_and_dotdot, (shopt_set_func_t *)NULL },
  { "globstar", &glob_star, (shopt_set_func_t *)NULL },
  { "gnu_errfmt", &gnu_error_format, (shopt_set_func_t *)NULL },
#if defined (HISTORY)
  { "histappend", &force_append_history, (shopt_set_func_t *)NULL },
#endif
#if defined (READLINE)
  { "histreedit", &history_reediting, (shopt_set_func_t *)NULL },
  { "histverify", &hist_verify, (shopt_set_func_t *)NULL },
  { "hostcomplete", &perform_hostname_completion, shopt_enable_hostname_completion },
#endif
  { "huponexit", &hup_on_exit, (shopt_set_func_t *)NULL },
  { "inherit_errexit", &inherit_errexit, (shopt_set_func_t *)NULL },
  { "interactive_comments", &interactive_comments, set_shellopts_after_change },
  { "lastpipe", &lastpipe_opt, (shopt_set_func_t *)NULL },
#if defined (HISTORY)
  { "lithist", &literal_history, (shopt_set_func_t *)NULL },
#endif
  { "localvar_inherit", &localvar_inherit, (shopt_set_func_t *)NULL },
  { "localvar_unset", &localvar_unset, (shopt_set_func_t *)NULL },
  { "login_shell", &shopt_login_shell, set_login_shell },
  { "mailwarn", &mail_warning, (shopt_set_func_t *)NULL },
#if defined (READLINE)
  { "no_empty_cmd_completion", &no_empty_command_completion, (shopt_set_func_t *)NULL },
#endif
  { "nocaseglob", &glob_ignore_case, (shopt_set_func_t *)NULL },
  { "nocasematch", &match_ignore_case, (shopt_set_func_t *)NULL },
  { "noexpand_translation", &singlequote_translations, (shopt_set_func_t *)NULL },
  { "nullglob",	&allow_null_glob_expansion, (shopt_set_func_t *)NULL },
  { "patsub_replacement", &patsub_replacement, (shopt_set_func_t *)NULL },
#if defined (PROGRAMMABLE_COMPLETION)
  { "progcomp", &prog_completion_enabled, (shopt_set_func_t *)NULL },
#  if defined (ALIAS)
  { "progcomp_alias", &progcomp_alias, (shopt_set_func_t *)NULL },
#  endif
#endif
  { "promptvars", &promptvars, (shopt_set_func_t *)NULL },
#if defined (RESTRICTED_SHELL)
  { "restricted_shell", &restricted_shell, set_restricted_shell },
#endif
  { "shift_verbose", &print_shift_error, (shopt_set_func_t *)NULL },
  { "sourcepath", &source_uses_path, (shopt_set_func_t *)NULL },
#if defined (SYSLOG_HISTORY) && defined (SYSLOG_SHOPT)
  { "syslog_history", &syslog_history, (shopt_set_func_t *)NULL },
#endif
  { "varredir_close", &varassign_redir_autoclose, (shopt_set_func_t *)NULL },
  { "xpg_echo", &xpg_echo, (shopt_set_func_t *)NULL },
  { (char *)0, (int *)0, (shopt_set_func_t *)NULL }
};

#define N_SHOPT_OPTIONS		(sizeof (shopt_vars) / sizeof (shopt_vars[0]))

#define GET_SHOPT_OPTION_VALUE(i)	(*shopt_vars[i].value)

static const char * const on = "on";
static const char * const off = "off";

static int find_shopt PARAMS((char *));
static int toggle_shopts PARAMS((int, WORD_LIST *, int));
static void print_shopt PARAMS((char *, int, int));
static int list_shopts PARAMS((WORD_LIST *, int));
static int list_some_shopts PARAMS((int, int));
static int list_shopt_o_options PARAMS((WORD_LIST *, int));
static int list_some_o_options PARAMS((int, int));
static int set_shopt_o_options PARAMS((int, WORD_LIST *, int));

#define SFLAG	0x01
#define UFLAG	0x02
#define QFLAG	0x04
#define OFLAG	0x08
#define PFLAG	0x10

int
shopt_builtin (list)
     WORD_LIST *list;
{
  int opt, flags, rval;

  flags = 0;
  reset_internal_getopt ();
  while ((opt = internal_getopt (list, "psuoq")) != -1)
    {
      switch (opt)
	{
	case 's':
	  flags |= SFLAG;
	  break;
	case 'u':
	  flags |= UFLAG;
	  break;
	case 'q':
	  flags |= QFLAG;
	  break;
	case 'o':
	  flags |= OFLAG;
	  break;
	case 'p':
	  flags |= PFLAG;
	  break;
	CASE_HELPOPT;
	default:
	  builtin_usage ();
	  return (EX_USAGE);
	}
    }
  list = loptend;

  if ((flags & (SFLAG|UFLAG)) == (SFLAG|UFLAG))
    {
      builtin_error (_("cannot set and unset shell options simultaneously"));
      return (EXECUTION_FAILURE);
    }

  rval = EXECUTION_SUCCESS;
  if ((flags & OFLAG) && ((flags & (SFLAG|UFLAG)) == 0))	/* shopt -o */
    rval = list_shopt_o_options (list, flags);
  else if (list && (flags & OFLAG))		/* shopt -so args */
    rval = set_shopt_o_options ((flags & SFLAG) ? FLAG_ON : FLAG_OFF, list, flags & QFLAG);
  else if (flags & OFLAG)	/* shopt -so */
    rval = list_some_o_options ((flags & SFLAG) ? 1 : 0, flags);
  else if (list && (flags & (SFLAG|UFLAG)))	/* shopt -su args */
    rval = toggle_shopts ((flags & SFLAG) ? SETOPT : UNSETOPT, list, flags & QFLAG);
  else if ((flags & (SFLAG|UFLAG)) == 0)	/* shopt [args] */
    rval = list_shopts (list, flags);
  else						/* shopt -su */
    rval = list_some_shopts ((flags & SFLAG) ? SETOPT : UNSETOPT, flags);
  return (rval);
}

/* Reset the options managed by `shopt' to the values they would have at
   shell startup.  Variables from shopt_vars. */
void
reset_shopt_options ()
{
  autocd = cdable_vars = cdspelling = 0;
  check_hashed_filenames = CHECKHASH_DEFAULT;
  check_window_size = CHECKWINSIZE_DEFAULT;
  allow_null_glob_expansion = glob_dot_filenames = 0;
  no_exit_on_failed_exec = 0;
  expand_aliases = 0;
  extended_quote = 1;
  fail_glob_expansion = 0;
  glob_asciirange = GLOBASCII_DEFAULT;
  glob_star = 0;
  gnu_error_format = 0;
  hup_on_exit = 0;
  inherit_errexit = 0;
  interactive_comments = 1;
  lastpipe_opt = 0;
  localvar_inherit = localvar_unset = 0;
  mail_warning = 0;
  glob_ignore_case = match_ignore_case = 0;
  print_shift_error = 0;
  source_uses_path = promptvars = 1;
  varassign_redir_autoclose = 0;
  singlequote_translations = 0;
  patsub_replacement = 1;

#if defined (JOB_CONTROL)
  check_jobs_at_exit = 0;
#endif

#if defined (EXTENDED_GLOB)
  extended_glob = EXTGLOB_DEFAULT;
#endif

#if defined (ARRAY_VARS)
  expand_once_flag = assoc_expand_once = 0;
#endif

#if defined (HISTORY)
  literal_history = 0;
  force_append_history = 0;
  command_oriented_history = 1;
#endif

#if defined (SYSLOG_HISTORY)
#  if defined (SYSLOG_SHOPT)
  syslog_history = SYSLOG_SHOPT;
#  else
  syslog_history = 1;
#  endif /* SYSLOG_SHOPT */
#endif

#if defined (READLINE)
  complete_fullquote = 1;
  force_fignore = 1;
  hist_verify = history_reediting = 0;
  perform_hostname_completion = 1;
#  if DIRCOMPLETE_EXPAND_DEFAULT
  dircomplete_expand = 1;
#  else
  dircomplete_expand = 0;
#endif
  dircomplete_spelling = 0;
  no_empty_command_completion = 0;
#endif

#if defined (PROGRAMMABLE_COMPLETION)
  prog_completion_enabled = 1;
#  if defined (ALIAS)
  progcomp_alias = 0;
#  endif
#endif

#if defined (DEFAULT_ECHO_TO_XPG) || defined (STRICT_POSIX)
  xpg_echo = 1;
#else
  xpg_echo = 0;
#endif /* DEFAULT_ECHO_TO_XPG */

  shopt_login_shell = login_shell;
}

static int
find_shopt (name)
     char *name;
{
  int i;

  for (i = 0; shopt_vars[i].name; i++)
    if (STREQ (name, shopt_vars[i].name))
      return i;
  return -1;
}

static void
shopt_error (s)
     char *s;
{
  builtin_error (_("%s: invalid shell option name"), s);
}

static int
toggle_shopts (mode, list, quiet)
     int mode;
     WORD_LIST *list;
     int quiet;
{
  WORD_LIST *l;
  int ind, rval;
  SHELL_VAR *v;

  for (l = list, rval = EXECUTION_SUCCESS; l; l = l->next)
    {
      ind = find_shopt (l->word->word);
      if (ind < 0)
	{
	  shopt_error (l->word->word);
	  rval = EXECUTION_FAILURE;
	}
      else
	{
	  *shopt_vars[ind].value = mode;	/* 1 for set, 0 for unset */
	  if (shopt_vars[ind].set_func)
	    (*shopt_vars[ind].set_func) (shopt_vars[ind].name, mode);
	}
    }

  /* Don't set $BASHOPTS here if it hasn't already been initialized */
  if (v = find_variable ("BASHOPTS"))
    set_bashopts ();
  return (rval);
}

static void
print_shopt (name, val, flags)
     char *name;
     int val, flags;
{
  if (flags & PFLAG)
    printf ("shopt %s %s\n", val ? "-s" : "-u", name);
  else
    printf (OPTFMT, name, val ? on : off);
}

/* List the values of all or any of the `shopt' options.  Returns 0 if
   all were listed or all variables queried were on; 1 otherwise. */
static int
list_shopts (list, flags)
     WORD_LIST *list;
     int flags;
{
  WORD_LIST *l;
  int i, val, rval;

  if (list == 0)
    {
      for (i = 0; shopt_vars[i].name; i++)
	{
	  val = *shopt_vars[i].value;
	  if ((flags & QFLAG) == 0)
	    print_shopt (shopt_vars[i].name, val, flags);
	}
      return (sh_chkwrite (EXECUTION_SUCCESS));
    }

  for (l = list, rval = EXECUTION_SUCCESS; l; l = l->next)
    {
      i = find_shopt (l->word->word);
      if (i < 0)
	{
	  shopt_error (l->word->word);
	  rval = EXECUTION_FAILURE;
	  continue;
	}
      val = *shopt_vars[i].value;
      if (val == 0)
	rval = EXECUTION_FAILURE;
      if ((flags & QFLAG) == 0)
	print_shopt (l->word->word, val, flags);
    }

  return (sh_chkwrite (rval));
}

static int
list_some_shopts (mode, flags)
     int mode, flags;
{
  int val, i;

  for (i = 0; shopt_vars[i].name; i++)
    {
      val = *shopt_vars[i].value;
      if (((flags & QFLAG) == 0) && mode == val)
	print_shopt (shopt_vars[i].name, val, flags);
    }
  return (sh_chkwrite (EXECUTION_SUCCESS));
}

static int
list_shopt_o_options (list, flags)
     WORD_LIST *list;
     int flags;
{
  WORD_LIST *l;
  int val, rval;

  if (list == 0)
    {
      if ((flags & QFLAG) == 0)
	list_minus_o_opts (-1, (flags & PFLAG));
      return (sh_chkwrite (EXECUTION_SUCCESS));
    }

  for (l = list, rval = EXECUTION_SUCCESS; l; l = l->next)
    {
      val = minus_o_option_value (l->word->word);
      if (val == -1)
	{
	  sh_invalidoptname (l->word->word);
	  rval = EXECUTION_FAILURE;
	  continue;
	}
      if (val == 0)
	rval = EXECUTION_FAILURE;
      if ((flags & QFLAG) == 0)
	{
	  if (flags & PFLAG)
	    printf ("set %co %s\n", val ? '-' : '+', l->word->word);
	  else
	    printf (OPTFMT, l->word->word, val ? on : off);
	}
    }
  return (sh_chkwrite (rval));
}

static int
list_some_o_options (mode, flags)
     int mode, flags;
{
  if ((flags & QFLAG) == 0)
    list_minus_o_opts (mode, (flags & PFLAG));
  return (sh_chkwrite (EXECUTION_SUCCESS));
}

static int
set_shopt_o_options (mode, list, quiet)
     int mode;
     WORD_LIST *list;
     int quiet;
{
  WORD_LIST *l;
  int rval;

  for (l = list, rval = EXECUTION_SUCCESS; l; l = l->next)
    {
      if (set_minus_o_option (mode, l->word->word) == EXECUTION_FAILURE)
	rval = EXECUTION_FAILURE;
    }
  set_shellopts ();
  return rval;
}

/* If we set or unset interactive_comments with shopt, make sure the
   change is reflected in $SHELLOPTS. */
static int
set_shellopts_after_change (option_name, mode)
     char *option_name;
     int mode;
{
  set_shellopts ();
  return (0);
}

static int
shopt_set_debug_mode (option_name, mode)
     char *option_name;
     int mode;
{
#if defined (DEBUGGER)
  error_trace_mode = function_trace_mode = debugging_mode;
  set_shellopts ();
  if (debugging_mode)
    init_bash_argv ();
#endif
  return (0);
}

#if defined (READLINE)
static int
shopt_enable_hostname_completion (option_name, mode)
     char *option_name;
     int mode;
{
  return (enable_hostname_completion (mode));
}
#endif

static int
set_compatibility_level (option_name, mode)
     char *option_name;
     int mode;
{
  int ind, oldval;
  char *rhs;

  /* If we're unsetting one of the compatibility options, make sure the
     current value is in the range of the compatNN space. */
  if (mode == 0)
    oldval = shell_compatibility_level;

  /* If we're setting something, redo some of the work we did above in
     toggle_shopt().  Unset everything and reset the appropriate option
     based on OPTION_NAME. */
  if (mode)
    {
      shopt_compat31 = shopt_compat32 = 0;
      shopt_compat40 = shopt_compat41 = shopt_compat42 = shopt_compat43 = 0;
      shopt_compat44 = 0;
      ind = find_shopt (option_name);
      *shopt_vars[ind].value = mode;
    }

  /* Then set shell_compatibility_level based on what remains */
  if (shopt_compat31)
    shell_compatibility_level = 31;
  else if (shopt_compat32)
    shell_compatibility_level = 32;
  else if (shopt_compat40)
    shell_compatibility_level = 40;
  else if (shopt_compat41)
    shell_compatibility_level = 41;
  else if (shopt_compat42)
    shell_compatibility_level = 42;
  else if (shopt_compat43)
    shell_compatibility_level = 43;
  else if (shopt_compat44)
    shell_compatibility_level = 44;
  else if (oldval > 44 && shell_compatibility_level < DEFAULT_COMPAT_LEVEL)
    ;
  else
    shell_compatibility_level = DEFAULT_COMPAT_LEVEL;

  /* Make sure the current compatibility level is reflected in BASH_COMPAT */
  rhs = itos (shell_compatibility_level);
  bind_variable ("BASH_COMPAT", rhs, 0);
  free (rhs);

  return 0;
}

/* Set and unset the various compatibility options from the value of
   shell_compatibility_level; used by sv_shcompat */
void
set_compatibility_opts ()
{
  shopt_compat31 = shopt_compat32 = 0;
  shopt_compat40 = shopt_compat41 = shopt_compat42 = shopt_compat43 = 0;
  shopt_compat44 = 0;
  switch (shell_compatibility_level)
    {
      case DEFAULT_COMPAT_LEVEL:
      case 51:			/* completeness */
      case 50:
	break;
      case 44:
	shopt_compat44 = 1; break;
      case 43:
	shopt_compat43 = 1; break;
      case 42:
	shopt_compat42 = 1; break;
      case 41:
	shopt_compat41 = 1; break;
      case 40:
	shopt_compat40 = 1; break;
      case 32:
	shopt_compat32 = 1; break;
      case 31:
	shopt_compat31 = 1; break;
    }
}

#if defined (READLINE)
static int
shopt_set_complete_direxpand (option_name, mode)
     char *option_name;
     int mode;
{
  set_directory_hook ();
  return 0;
}
#endif

#if defined (RESTRICTED_SHELL)
/* Don't allow the value of restricted_shell to be modified. */

static int
set_restricted_shell (option_name, mode)
     char *option_name;
     int mode;
{
  static int save_restricted = -1;

  if (save_restricted == -1)
    save_restricted = shell_is_restricted (shell_name);

  restricted_shell = save_restricted;
  return (0);
}
#endif /* RESTRICTED_SHELL */

/* Not static so shell.c can call it to initialize shopt_login_shell */
int
set_login_shell (option_name, mode)
     char *option_name;
     int mode;
{
  shopt_login_shell = login_shell != 0;
  return (0);
}

char **
get_shopt_options ()
{
  char **ret;
  int n, i;

  n = sizeof (shopt_vars) / sizeof (shopt_vars[0]);
  ret = strvec_create (n + 1);
  for (i = 0; shopt_vars[i].name; i++)
    ret[i] = savestring (shopt_vars[i].name);
  ret[i] = (char *)NULL;
  return ret;
}

/*
 * External interface for other parts of the shell.  NAME is a string option;
 * MODE is 0 if we want to unset an option; 1 if we want to set an option.
 * REUSABLE is 1 if we want to print output in a form that may be reused.
 */
int
shopt_setopt (name, mode)
     char *name;
     int mode;
{
  WORD_LIST *wl;
  int r;

  wl = add_string_to_list (name, (WORD_LIST *)NULL);
  r = toggle_shopts (mode, wl, 0);
  dispose_words (wl);
  return r;
}

int
shopt_listopt (name, reusable)
     char *name;
     int reusable;
{
  int i;

  if (name == 0)
    return (list_shopts ((WORD_LIST *)NULL, reusable ? PFLAG : 0));

  i = find_shopt (name);
  if (i < 0)
    {
      shopt_error (name);
      return (EXECUTION_FAILURE);
    }

  print_shopt (name, *shopt_vars[i].value, reusable ? PFLAG : 0);
  return (sh_chkwrite (EXECUTION_SUCCESS));
}

void
set_bashopts ()
{
  char *value;
  char tflag[N_SHOPT_OPTIONS];
  int vsize, i, vptr, *ip, exported;
  SHELL_VAR *v;

  for (vsize = i = 0; shopt_vars[i].name; i++)
    {
      tflag[i] = 0;
      if (GET_SHOPT_OPTION_VALUE (i))
	{
	  vsize += strlen (shopt_vars[i].name) + 1;
	  tflag[i] = 1;
	}
    }

  value = (char *)xmalloc (vsize + 1);

  for (i = vptr = 0; shopt_vars[i].name; i++)
    {
      if (tflag[i])
	{
	  strcpy (value + vptr, shopt_vars[i].name);
	  vptr += strlen (shopt_vars[i].name);
	  value[vptr++] = ':';
	}
    }

  if (vptr)
    vptr--;			/* cut off trailing colon */
  value[vptr] = '\0';

  v = find_variable ("BASHOPTS");

  /* Turn off the read-only attribute so we can bind the new value, and
     note whether or not the variable was exported. */
  if (v)
    {
      VUNSETATTR (v, att_readonly);
      exported = exported_p (v);
    }
  else
    exported = 0;

  v = bind_variable ("BASHOPTS", value, 0);

  /* Turn the read-only attribute back on, and turn off the export attribute
     if it was set implicitly by mark_modified_vars and SHELLOPTS was not
     exported before we bound the new value. */
  VSETATTR (v, att_readonly);
  if (mark_modified_vars && exported == 0 && exported_p (v))
    VUNSETATTR (v, att_exported);

  free (value);
}

void
parse_bashopts (value)
     char *value;
{
  char *vname;
  int vptr, ind;

  vptr = 0;
  while (vname = extract_colon_unit (value, &vptr))
    {
      ind = find_shopt (vname);
      if (ind >= 0)
	{
          *shopt_vars[ind].value = 1;
	  if (shopt_vars[ind].set_func)
	    (*shopt_vars[ind].set_func) (shopt_vars[ind].name, 1);
	}
      free (vname);
    }
}

void
initialize_bashopts (no_bashopts)
     int no_bashopts;
{
  char *temp;
  SHELL_VAR *var;

  if (no_bashopts == 0)
    {
      var = find_variable ("BASHOPTS");
      /* set up any shell options we may have inherited. */
      if (var && imported_p (var))
	{
	  temp = (array_p (var) || assoc_p (var)) ? (char *)NULL : savestring (value_cell (var));
	  if (temp)
	    {
	      parse_bashopts (temp);
	      free (temp);
	    }
	}
    }

  /* Set up the $BASHOPTS variable. */
  set_bashopts ();
}

#if defined (ARRAY_VARS)
static int
set_assoc_expand (option_name, mode)
     char *option_name;
     int mode;
{
#if 0 /* leave this disabled */
  if (shell_compatibility_level <= 51)
#endif
    assoc_expand_once = expand_once_flag;
  return 0;
}
#endif
