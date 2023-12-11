/* complete.c, created from complete.def. */
#line 22 "./complete.def"

#line 51 "./complete.def"

#include "config.h"

#include <stdio.h>

#include "bashtypes.h"

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#include "bashansi.h"
#include "bashintl.h"

#include "shell.h"
#include "builtins.h"
#include "pcomplete.h"
#include "bashline.h"

#include "common.h"
#include "bashgetopt.h"

#include "third_party/readline/readline.h"

#define STRDUP(x)       ((x) ? savestring (x) : (char *)NULL)

/* Structure containing all the non-action (binary) options; filled in by
   build_actions(). */
struct _optflags {
  int pflag;
  int rflag;
  int Dflag;
  int Eflag;
  int Iflag;
};

static int find_compact PARAMS((char *));
static int find_compopt PARAMS((char *));

static int build_actions PARAMS((WORD_LIST *, struct _optflags *, unsigned long *, unsigned long *));

static int remove_cmd_completions PARAMS((WORD_LIST *));

static int print_one_completion PARAMS((char *, COMPSPEC *));
static int print_compitem PARAMS((BUCKET_CONTENTS *));
static void print_compopts PARAMS((const char *, COMPSPEC *, int));
static void print_all_completions PARAMS((void));
static int print_cmd_completions PARAMS((WORD_LIST *));

static void print_compoptions PARAMS((unsigned long, int));
static void print_compactions PARAMS((unsigned long));
static void print_arg PARAMS((const char *, const char *, int));
static void print_cmd_name PARAMS((const char *));

static char *Garg, *Warg, *Parg, *Sarg, *Xarg, *Farg, *Carg;

static const struct _compacts {
  const char * const actname;
  unsigned long actflag;
  int actopt;
} compacts[] = {
  { "alias",     CA_ALIAS,     'a' },
  { "arrayvar",  CA_ARRAYVAR,   0 },
  { "binding",   CA_BINDING,    0 },
  { "builtin",   CA_BUILTIN,   'b' },
  { "command",   CA_COMMAND,   'c' },
  { "directory", CA_DIRECTORY, 'd' },
  { "disabled",  CA_DISABLED,   0 },
  { "enabled",   CA_ENABLED,    0 },
  { "export",    CA_EXPORT,    'e' },
  { "file",      CA_FILE,      'f' },
  { "function",  CA_FUNCTION,   0 },
  { "helptopic", CA_HELPTOPIC,  0 },
  { "hostname",  CA_HOSTNAME,   0 },
  { "group",     CA_GROUP,     'g' },
  { "job",       CA_JOB,       'j' },
  { "keyword",   CA_KEYWORD,   'k' },
  { "running",   CA_RUNNING,    0 },
  { "service",   CA_SERVICE,   's' },
  { "setopt",    CA_SETOPT,     0 },
  { "shopt",     CA_SHOPT,      0 },
  { "signal",    CA_SIGNAL,     0 },
  { "stopped",   CA_STOPPED,    0 },
  { "user",      CA_USER,      'u' },
  { "variable",  CA_VARIABLE,  'v' },
  { (char *)NULL, 0, 0 },
};

/* This should be a STRING_INT_ALIST */
static const struct _compopt {
  const char * const optname;
  unsigned long optflag;
} compopts[] = {
  { "bashdefault", COPT_BASHDEFAULT },
  { "default",	COPT_DEFAULT },
  { "dirnames", COPT_DIRNAMES },
  { "filenames",COPT_FILENAMES},
  { "noquote", COPT_NOQUOTE },
  { "nosort", COPT_NOSORT },
  { "nospace",	COPT_NOSPACE },
  { "plusdirs", COPT_PLUSDIRS },
  { (char *)NULL, 0 },
};

static int
find_compact (name)
     char *name;
{
  register int i;

  for (i = 0; compacts[i].actname; i++)
    if (STREQ (name, compacts[i].actname))
      return i;
  return -1;
}

static int
find_compopt (name)
     char *name;
{
  register int i;

  for (i = 0; compopts[i].optname; i++)
    if (STREQ (name, compopts[i].optname))
      return i;
  return -1;
}

/* Build the actions and compspec options from the options specified in LIST.
   ACTP is a pointer to an unsigned long in which to place the bitmap of
   actions.  OPTP is a pointer to an unsigned long in which to place the
   bitmap of compspec options (arguments to `-o').  PP, if non-null, gets 1
   if -p is supplied; RP, if non-null, gets 1 if -r is supplied.
   If either is null, the corresponding option generates an error.
   This also sets variables corresponding to options that take arguments as
   a side effect; the caller should ensure that those variables are set to
   NULL before calling build_actions.  Return value:
   	EX_USAGE = bad option
   	EXECUTION_SUCCESS = some options supplied
   	EXECUTION_FAILURE = no options supplied
*/

static int
build_actions (list, flagp, actp, optp)
     WORD_LIST *list;
     struct _optflags *flagp;
     unsigned long *actp, *optp;
{
  int opt, ind, opt_given;
  unsigned long acts, copts;
  WORD_DESC w;

  acts = copts = (unsigned long)0L;
  opt_given = 0;

  reset_internal_getopt ();
  while ((opt = internal_getopt (list, "abcdefgjko:prsuvA:G:W:P:S:X:F:C:DEI")) != -1)
    {
      opt_given = 1;
      switch (opt)
	{
	case 'r':
	  if (flagp)
	    {
	      flagp->rflag = 1;
	      break;
	    }
	  else
	    {
	      sh_invalidopt ("-r");
	      builtin_usage ();
	      return (EX_USAGE);
	    }

	case 'p':
	  if (flagp)
	    {
	      flagp->pflag = 1;
	      break;
	    }
	  else
	    {
	      sh_invalidopt ("-p");
	      builtin_usage ();
	      return (EX_USAGE);
	    }

	case 'a':
	  acts |= CA_ALIAS;
	  break;
	case 'b':
	  acts |= CA_BUILTIN;
	  break;
	case 'c':
	  acts |= CA_COMMAND;
	  break;
	case 'd':
	  acts |= CA_DIRECTORY;
	  break;
	case 'e':
	  acts |= CA_EXPORT;
	  break;
	case 'f':
	  acts |= CA_FILE;
	  break;
	case 'g':
	  acts |= CA_GROUP;
	  break;
	case 'j':
	  acts |= CA_JOB;
	  break;
	case 'k':
	  acts |= CA_KEYWORD;
	  break;
	case 's':
	  acts |= CA_SERVICE;
	  break;
	case 'u':
	  acts |= CA_USER;
	  break;
	case 'v':
	  acts |= CA_VARIABLE;
	  break;
	case 'o':
	  ind = find_compopt (list_optarg);
	  if (ind < 0)
	    {
	      sh_invalidoptname (list_optarg);
	      return (EX_USAGE);
	    }
	  copts |= compopts[ind].optflag;
	  break;
	case 'A':
	  ind = find_compact (list_optarg);
	  if (ind < 0)
	    {
	      builtin_error (_("%s: invalid action name"), list_optarg);
	      return (EX_USAGE);
	    }
	  acts |= compacts[ind].actflag;
	  break;
	case 'C':
	  Carg = list_optarg;
	  break;
	case 'D':
	  if (flagp)
	    {
	      flagp->Dflag = 1;
	      break;
	    }
	  else
	    {
	      sh_invalidopt ("-D");
	      builtin_usage ();
	      return (EX_USAGE);
	    }
	case 'E':
	  if (flagp)
	    {
	      flagp->Eflag = 1;
	      break;
	    }
	  else
	    {
	      sh_invalidopt ("-E");
	      builtin_usage ();
	      return (EX_USAGE);
	    }
	case 'I':
	  if (flagp)
	    {
	      flagp->Iflag = 1;
	      break;
	    }
	  else
	    {
	      sh_invalidopt ("-I");
	      builtin_usage ();
	      return (EX_USAGE);
	    }
	case 'F':
	  w.word = Farg = list_optarg;
	  w.flags = 0;
	  if (check_identifier (&w, posixly_correct) == 0 || strpbrk (Farg, shell_break_chars) != 0)
	    {
	      sh_invalidid (Farg);
	      return (EX_USAGE);
	    }
	  break;
	case 'G':
	  Garg = list_optarg;
	  break;
	case 'P':
	  Parg = list_optarg;
	  break;
	case 'S':
	  Sarg = list_optarg;
	  break;
	case 'W':
	  Warg = list_optarg;
	  break;
	case 'X':
	  Xarg = list_optarg;
	  break;
	CASE_HELPOPT;
	default:
	  builtin_usage ();
	  return (EX_USAGE);
	}
    }

  *actp = acts;
  *optp = copts;

  return (opt_given ? EXECUTION_SUCCESS : EXECUTION_FAILURE);
}

/* Add, remove, and display completion specifiers. */
int
complete_builtin (list)
     WORD_LIST *list;
{
  int opt_given, rval;
  unsigned long acts, copts;
  COMPSPEC *cs;
  struct _optflags oflags;
  WORD_LIST *l, *wl;

  if (list == 0)
    {
      print_all_completions ();
      return (EXECUTION_SUCCESS);
    }

  opt_given = oflags.pflag = oflags.rflag = 0;
  oflags.Dflag = oflags.Eflag = oflags.Iflag = 0;

  acts = copts = (unsigned long)0L;
  Garg = Warg = Parg = Sarg = Xarg = Farg = Carg = (char *)NULL;
  cs = (COMPSPEC *)NULL;

  /* Build the actions from the arguments.  Also sets the [A-Z]arg variables
     as a side effect if they are supplied as options. */
  rval = build_actions (list, &oflags, &acts, &copts);
  if (rval == EX_USAGE)
    return (rval);
  opt_given = rval != EXECUTION_FAILURE;

  list = loptend;

  if (oflags.Dflag)
    wl = make_word_list (make_bare_word (DEFAULTCMD), (WORD_LIST *)NULL);
  else if (oflags.Eflag)
    wl = make_word_list (make_bare_word (EMPTYCMD), (WORD_LIST *)NULL);
  else if (oflags.Iflag)
    wl = make_word_list (make_bare_word (INITIALWORD), (WORD_LIST *)NULL);
  else
    wl = (WORD_LIST *)NULL;

  /* -p overrides everything else */
  if (oflags.pflag || (list == 0 && opt_given == 0))
    {
      if (wl)
	{
	  rval = print_cmd_completions (wl);
	  dispose_words (wl);
	  return rval;
	}
      else if (list == 0)
	{
	  print_all_completions ();
	  return (EXECUTION_SUCCESS);
	}
      return (print_cmd_completions (list));
    }

  /* next, -r overrides everything else. */
  if (oflags.rflag)
    {
      if (wl)
	{
	  rval = remove_cmd_completions (wl);
	  dispose_words (wl);
	  return rval;
	}
      else if (list == 0)
	{
	  progcomp_flush ();
	  return (EXECUTION_SUCCESS);
	}
      return (remove_cmd_completions (list));
    }

  if (wl == 0 && list == 0 && opt_given)
    {
      builtin_usage ();
      return (EX_USAGE);
    }

  /* If we get here, we need to build a compspec and add it for each
     remaining argument. */
  cs = compspec_create ();
  cs->actions = acts;
  cs->options = copts;

  cs->globpat = STRDUP (Garg);
  cs->words = STRDUP (Warg);
  cs->prefix = STRDUP (Parg);
  cs->suffix = STRDUP (Sarg);
  cs->funcname = STRDUP (Farg);
  cs->command = STRDUP (Carg);
  cs->filterpat = STRDUP (Xarg);

  for (rval = EXECUTION_SUCCESS, l = wl ? wl : list ; l; l = l->next)
    {
      /* Add CS as the compspec for the specified commands. */
      if (progcomp_insert (l->word->word, cs) == 0)
	rval = EXECUTION_FAILURE;
    }

  dispose_words (wl);
  return (rval);
}

static int
remove_cmd_completions (list)
     WORD_LIST *list;
{
  WORD_LIST *l;
  int ret;

  for (ret = EXECUTION_SUCCESS, l = list; l; l = l->next)
    {
      if (progcomp_remove (l->word->word) == 0)
	{
	  builtin_error (_("%s: no completion specification"), l->word->word);
	  ret = EXECUTION_FAILURE;
	}
    }
  return ret;
}

static void
print_compoptions (copts, full)
     unsigned long copts;
     int full;
{
  const struct _compopt *co;

  for (co = compopts; co->optname; co++)
    if (copts & co->optflag)
      printf ("-o %s ", co->optname);
    else if (full)
      printf ("+o %s ", co->optname);
}

static void
print_compactions (acts)
     unsigned long acts;
{
  const struct _compacts *ca;

  /* simple flags first */
  for (ca = compacts; ca->actname; ca++)
    if (ca->actopt && (acts & ca->actflag))
      printf ("-%c ", ca->actopt);

  /* then the rest of the actions */
  for (ca = compacts; ca->actname; ca++)
    if (ca->actopt == 0 && (acts & ca->actflag))
      printf ("-A %s ", ca->actname);
}

static void
print_arg (arg, flag, quote)
     const char *arg, *flag;
     int quote;
{
  char *x;

  if (arg)
    {
      x = quote ? sh_single_quote (arg) : (char *)arg;
      printf ("%s %s ", flag, x);
      if (x != arg)
	free (x);
    }
}

static void
print_cmd_name (cmd)
     const char *cmd;
{
  char *x;

  if (STREQ (cmd, DEFAULTCMD))
    printf ("-D");
  else if (STREQ (cmd, EMPTYCMD))
    printf ("-E");
  else if (STREQ (cmd, INITIALWORD))
    printf ("-I");
  else if (*cmd == 0)		/* XXX - can this happen? */
    printf ("''");
  else if (sh_contains_shell_metas (cmd))
    {
      x = sh_single_quote (cmd);
      printf ("%s", x);
      free (x);
    }
  else
    printf ("%s", cmd);
}

static int
print_one_completion (cmd, cs)
     char *cmd;
     COMPSPEC *cs;
{
  printf ("complete ");

  print_compoptions (cs->options, 0);
  print_compactions (cs->actions);

  /* now the rest of the arguments */

  /* arguments that require quoting */
  print_arg (cs->globpat, "-G", 1);
  print_arg (cs->words, "-W", 1);
  print_arg (cs->prefix, "-P", 1);
  print_arg (cs->suffix, "-S", 1);
  print_arg (cs->filterpat, "-X", 1);

  print_arg (cs->command, "-C", 1);

  /* simple arguments that don't require quoting */
  print_arg (cs->funcname, "-F", sh_contains_shell_metas (cs->funcname) != 0);

  print_cmd_name (cmd);
  printf ("\n");

  return (0);
}

static void
print_compopts (cmd, cs, full)
     const char *cmd;
     COMPSPEC *cs;
     int full;
{
  printf ("compopt ");

  print_compoptions (cs->options, full);
  print_cmd_name (cmd);

  printf ("\n");
}

static int
print_compitem (item)
     BUCKET_CONTENTS *item;
{
  COMPSPEC *cs;
  char *cmd;

  cmd = item->key;
  cs = (COMPSPEC *)item->data;

  return (print_one_completion (cmd, cs));
}

static void
print_all_completions ()
{
  progcomp_walk (print_compitem);
}

static int
print_cmd_completions (list)
     WORD_LIST *list;
{
  WORD_LIST *l;
  COMPSPEC *cs;
  int ret;

  for (ret = EXECUTION_SUCCESS, l = list; l; l = l->next)
    {
      cs = progcomp_search (l->word->word);
      if (cs)
	print_one_completion (l->word->word, cs);
      else
	{
	  builtin_error (_("%s: no completion specification"), l->word->word);
	  ret = EXECUTION_FAILURE;
	}
    }

  return (sh_chkwrite (ret));
}

#line 663 "./complete.def"

int
compgen_builtin (list)
     WORD_LIST *list;
{
  int rval;
  unsigned long acts, copts;
  COMPSPEC *cs;
  STRINGLIST *sl;
  char *word, **matches;
  char *old_line;
  int old_ind;

  if (list == 0)
    return (EXECUTION_SUCCESS);

  acts = copts = (unsigned long)0L;
  Garg = Warg = Parg = Sarg = Xarg = Farg = Carg = (char *)NULL;
  cs = (COMPSPEC *)NULL;

  /* Build the actions from the arguments.  Also sets the [A-Z]arg variables
     as a side effect if they are supplied as options. */
  rval = build_actions (list, (struct _optflags *)NULL, &acts, &copts);
  if (rval == EX_USAGE)
    return (rval);
  if (rval == EXECUTION_FAILURE)
    return (EXECUTION_SUCCESS);

  list = loptend;

  word = (list && list->word) ? list->word->word : "";

  if (Farg)
    builtin_error (_("warning: -F option may not work as you expect"));
  if (Carg)
    builtin_error (_("warning: -C option may not work as you expect"));

  /* If we get here, we need to build a compspec and evaluate it. */
  cs = compspec_create ();
  cs->actions = acts;
  cs->options = copts;
  cs->refcount = 1;

  cs->globpat = STRDUP (Garg);
  cs->words = STRDUP (Warg);
  cs->prefix = STRDUP (Parg);
  cs->suffix = STRDUP (Sarg);
  cs->funcname = STRDUP (Farg);
  cs->command = STRDUP (Carg);
  cs->filterpat = STRDUP (Xarg);

  rval = EXECUTION_FAILURE;

  /* probably don't have to save these, just being safe */
  old_line = pcomp_line;
  old_ind = pcomp_ind;
  pcomp_line = (char *)NULL;
  pcomp_ind = 0;
  sl = gen_compspec_completions (cs, "compgen", word, 0, 0, 0);
  pcomp_line = old_line;
  pcomp_ind = old_ind;

  /* If the compspec wants the bash default completions, temporarily
     turn off programmable completion and call the bash completion code. */
  if ((sl == 0 || sl->list_len == 0) && (copts & COPT_BASHDEFAULT))
    {
      matches = bash_default_completion (word, 0, 0, 0, 0);
      sl = completions_to_stringlist (matches);
      strvec_dispose (matches);
    }

  /* This isn't perfect, but it's the best we can do, given what readline
     exports from its set of completion utility functions. */
  if ((sl == 0 || sl->list_len == 0) && (copts & COPT_DEFAULT))
    {
      matches = rl_completion_matches (word, rl_filename_completion_function);
      strlist_dispose (sl);
      sl = completions_to_stringlist (matches);
      strvec_dispose (matches);
    }

  if (sl)
    {
      if (sl->list && sl->list_len)
	{
	  rval = EXECUTION_SUCCESS;
	  strlist_print (sl, (char *)NULL);
	}
      strlist_dispose (sl);
    }

  compspec_dispose (cs);
  return (rval);
}

#line 788 "./complete.def"

int
compopt_builtin (list)
     WORD_LIST *list;
{
  int opts_on, opts_off, *opts, opt, oind, ret, Dflag, Eflag, Iflag;
  WORD_LIST *l, *wl;
  COMPSPEC *cs;

  opts_on = opts_off = Eflag = Dflag = Iflag = 0;
  ret = EXECUTION_SUCCESS;

  reset_internal_getopt ();
  while ((opt = internal_getopt (list, "+o:DEI")) != -1)
    {
      opts = (list_opttype == '-') ? &opts_on : &opts_off;

      switch (opt)
	{
	case 'o':
	  oind = find_compopt (list_optarg);
	  if (oind < 0)
	    {
	      sh_invalidoptname (list_optarg);
	      return (EX_USAGE);
	    }
	  *opts |= compopts[oind].optflag;
	  break;
	case 'D':
	  Dflag = 1;
	  break;
	case 'E':
	  Eflag = 1;
	  break;
	case 'I':
	  Iflag = 1;
	  break;
	CASE_HELPOPT;
	default:
	  builtin_usage ();
	  return (EX_USAGE);
	}
    }
  list = loptend;

  if (Dflag)
    wl = make_word_list (make_bare_word (DEFAULTCMD), (WORD_LIST *)NULL);
  else if (Eflag)
    wl = make_word_list (make_bare_word (EMPTYCMD), (WORD_LIST *)NULL);
  else if (Iflag)
    wl = make_word_list (make_bare_word (INITIALWORD), (WORD_LIST *)NULL);
  else
    wl = (WORD_LIST *)NULL;

  if (list == 0 && wl == 0)
    {
      if (RL_ISSTATE (RL_STATE_COMPLETING) == 0 || pcomp_curcs == 0)
	{
	  builtin_error (_("not currently executing completion function"));
	  return (EXECUTION_FAILURE);
	}
      cs = pcomp_curcs;

      if (opts_on == 0 && opts_off == 0)
	{
	  print_compopts (pcomp_curcmd, cs, 1);
          return (sh_chkwrite (ret));
	}

      /* Set the compspec options */
      pcomp_set_compspec_options (cs, opts_on, 1);
      pcomp_set_compspec_options (cs, opts_off, 0);

      /* And change the readline variables the options control */
      pcomp_set_readline_variables (opts_on, 1);
      pcomp_set_readline_variables (opts_off, 0);

      return (ret);
    }

  for (l = wl ? wl : list; l; l = l->next)
    {
      cs = progcomp_search (l->word->word);
      if (cs == 0)
	{
	  builtin_error (_("%s: no completion specification"), l->word->word);
	  ret = EXECUTION_FAILURE;
	  continue;
	}
      if (opts_on == 0 && opts_off == 0)
	{
	  print_compopts (l->word->word, cs, 1);
	  continue;			/* XXX -- fill in later */
	}

      /* Set the compspec options */
      pcomp_set_compspec_options (cs, opts_on, 1);
      pcomp_set_compspec_options (cs, opts_off, 0);
    }

  if (wl)
    dispose_words (wl);

  return (ret);
}
