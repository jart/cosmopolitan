/* alias.c, created from alias.def. */
#line 42 "./alias.def"

#include "config.h"

#if defined (ALIAS)

#if defined (HAVE_UNISTD_H)
#  ifdef _MINIX
#    include <sys/types.h>
#  endif
#  include <unistd.h>
#endif

#  include "bashansi.h"
#  include "bashintl.h"

#  include <stdio.h>
#  include "shell.h"
#  include "alias.h"
#  include "common.h"
#  include "bashgetopt.h"

/* Flags for print_alias */
#define AL_REUSABLE	0x01

static void print_alias PARAMS((alias_t *, int));

/* Hack the alias command in a Korn shell way. */
int
alias_builtin (list)
     WORD_LIST *list;
{
  int any_failed, offset, pflag, dflags;
  alias_t **alias_list, *t;
  char *name, *value;

  dflags = posixly_correct ? 0 : AL_REUSABLE;
  pflag = 0;
  reset_internal_getopt ();
  while ((offset = internal_getopt (list, "p")) != -1)
    {
      switch (offset)
	{
	case 'p':
	  pflag = 1;
	  dflags |= AL_REUSABLE;
	  break;
	CASE_HELPOPT;
	default:
	  builtin_usage ();
	  return (EX_USAGE);
	}
    }

  list = loptend;

  if (list == 0 || pflag)
    {
      if (aliases == 0)
	return (EXECUTION_SUCCESS);

      alias_list = all_aliases ();

      if (alias_list == 0)
	return (EXECUTION_SUCCESS);

      for (offset = 0; alias_list[offset]; offset++)
	print_alias (alias_list[offset], dflags);

      free (alias_list);	/* XXX - Do not free the strings. */

      if (list == 0)
	return (sh_chkwrite (EXECUTION_SUCCESS));
    }

  any_failed = 0;
  while (list)
    {
      name = list->word->word;

      for (offset = 0; name[offset] && name[offset] != '='; offset++)
	;

      if (offset && name[offset] == '=')
	{
	  name[offset] = '\0';
	  value = name + offset + 1;

	  if (legal_alias_name (name, 0) == 0)
	    {
	      builtin_error (_("`%s': invalid alias name"), name);
	      any_failed++;
	    }
	  else
	    add_alias (name, value);
	}
      else
	{
	  t = find_alias (name);
	  if (t)
	    print_alias (t, dflags);
	  else
	    {
	      sh_notfound (name);
	      any_failed++;
	    }
	}
      list = list->next;
    }

  return (any_failed ? EXECUTION_FAILURE : EXECUTION_SUCCESS);
}
#endif /* ALIAS */

#line 166 "./alias.def"

#if defined (ALIAS)
/* Remove aliases named in LIST from the aliases database. */
int
unalias_builtin (list)
     register WORD_LIST *list;
{
  register alias_t *alias;
  int opt, aflag;

  aflag = 0;
  reset_internal_getopt ();
  while ((opt = internal_getopt (list, "a")) != -1)
    {
      switch (opt)
	{
	case 'a':
	  aflag = 1;
	  break;
	CASE_HELPOPT;
	default:
	  builtin_usage ();
	  return (EX_USAGE);
	}
    }

  list = loptend;

  if (aflag)
    {
      delete_all_aliases ();
      return (EXECUTION_SUCCESS);
    }

  if (list == 0)
    {
      builtin_usage ();
      return (EX_USAGE);
    }

  aflag = 0;
  while (list)
    {
      alias = find_alias (list->word->word);

      if (alias)
	remove_alias (alias->name);
      else
	{
	  sh_notfound (list->word->word);
	  aflag++;
	}

      list = list->next;
    }

  return (aflag ? EXECUTION_FAILURE : EXECUTION_SUCCESS);
}

/* Output ALIAS in such a way as to allow it to be read back in. */
static void
print_alias (alias, flags)
     alias_t *alias;
     int flags;
{
  char *value;

  value = sh_single_quote (alias->value);
  if (flags & AL_REUSABLE)
    printf ("alias %s", (alias->name && alias->name[0] == '-') ? "-- " : "");
  printf ("%s=%s\n", alias->name, value);
  free (value);

  fflush (stdout);
}
#endif /* ALIAS */
