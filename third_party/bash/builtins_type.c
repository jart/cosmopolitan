/* type.c, created from type.def. */
#line 22 "./type.def"

#line 52 "./type.def"

#include "config.h"

#include "bashtypes.h"
#include "posixstat.h"

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#include <stdio.h>
#include "bashansi.h"
#include "bashintl.h"

#include "shell.h"
#include "parser.h"
#include "execute_cmd.h"
#include "findcmd.h"
#include "hashcmd.h"

#if defined (ALIAS)
#include "alias.h"
#endif /* ALIAS */

#include "common.h"
#include "bashgetopt.h"

extern int find_reserved_word PARAMS((char *));

/* For each word in LIST, find out what the shell is going to do with
   it as a simple command. i.e., which file would this shell use to
   execve, or if it is a builtin command, or an alias.  Possible flag
   arguments:
	-t		Returns the "type" of the object, one of
			`alias', `keyword', `function', `builtin',
			or `file'.

	-p		Returns the pathname of the file if -type is
			a file.

	-a		Returns all occurrences of words, whether they
			be a filename in the path, alias, function,
			or builtin.

	-f		Suppress shell function lookup, like `command'.

	-P		Force a path search even in the presence of other
			definitions.

   Order of evaluation:
	alias
	keyword
	function
	builtin
	file
 */

int
type_builtin (list)
     WORD_LIST *list;
{
  int dflags, any_failed, opt;
  WORD_LIST *this;

  if (list == 0)
    return (EXECUTION_SUCCESS);

  dflags = CDESC_SHORTDESC;	/* default */
  any_failed = 0;

  /* Handle the obsolescent `-type', `-path', and `-all' by prescanning
     the arguments and converting those options to the form that
     internal_getopt recognizes. Converts `--type', `--path', and `--all'
     also. THIS SHOULD REALLY GO AWAY. */
  for (this = list; this && this->word->word[0] == '-'; this = this->next)
    {
      char *flag = &(this->word->word[1]);

      if (STREQ (flag, "type") || STREQ (flag, "-type"))
	{
	  this->word->word[1] = 't';
	  this->word->word[2] = '\0';
	}
      else if (STREQ (flag, "path") || STREQ (flag, "-path"))
	{
	  this->word->word[1] = 'p';
	  this->word->word[2] = '\0';
	}
      else if (STREQ (flag, "all") || STREQ (flag, "-all"))
	{
	  this->word->word[1] = 'a';
	  this->word->word[2] = '\0';
	}
    }

  reset_internal_getopt ();
  while ((opt = internal_getopt (list, "afptP")) != -1)
    {
      switch (opt)
	{
	case 'a':
	  dflags |= CDESC_ALL;
	  break;
	case 'f':
	  dflags |= CDESC_NOFUNCS;
	  break;
	case 'p':
	  dflags |= CDESC_PATH_ONLY;
	  dflags &= ~(CDESC_TYPE|CDESC_SHORTDESC);
	  break;
	case 't':
	  dflags |= CDESC_TYPE;
	  dflags &= ~(CDESC_PATH_ONLY|CDESC_SHORTDESC);
	  break;
	case 'P':	/* shorthand for type -ap */
	  dflags |= (CDESC_PATH_ONLY|CDESC_FORCE_PATH);
	  dflags &= ~(CDESC_TYPE|CDESC_SHORTDESC);
	  break;
	CASE_HELPOPT;
	default:
	  builtin_usage ();
	  return (EX_USAGE);
	}
    }
  list = loptend;

  while (list)
    {
      int found;

      found = describe_command (list->word->word, dflags);

      if (!found && (dflags & (CDESC_PATH_ONLY|CDESC_TYPE)) == 0)
	sh_notfound (list->word->word);

      any_failed += found == 0;
      list = list->next;
    }

  opt = (any_failed == 0) ? EXECUTION_SUCCESS : EXECUTION_FAILURE;
  return (sh_chkwrite (opt));
}

/*
 * Describe COMMAND as required by the type and command builtins.
 *
 * Behavior is controlled by DFLAGS.  Flag values are
 *	CDESC_ALL	print all descriptions of a command
 *	CDESC_SHORTDESC	print the description for type and command -V
 *	CDESC_REUSABLE	print in a format that may be reused as input
 *	CDESC_TYPE	print the type for type -t
 *	CDESC_PATH_ONLY	print the path for type -p
 *	CDESC_FORCE_PATH	force a path search for type -P
 *	CDESC_NOFUNCS	skip function lookup for type -f
 *	CDESC_ABSPATH	convert to absolute path, no ./ prefix
 *	CDESC_STDPATH	command -p standard path list
 *
 * CDESC_ALL says whether or not to look for all occurrences of COMMAND, or
 * return after finding it once.
 */
int
describe_command (command, dflags)
     char *command;
     int dflags;
{
  int found, i, found_file, f, all;
  char *full_path, *x, *pathlist;
  SHELL_VAR *func;
#if defined (ALIAS)
  alias_t *alias;
#endif

  all = (dflags & CDESC_ALL) != 0;
  found = found_file = 0;
  full_path = (char *)NULL;

#if defined (ALIAS)
  /* Command is an alias? */
  if (((dflags & CDESC_FORCE_PATH) == 0) && expand_aliases && (alias = find_alias (command)))
    {
      if (dflags & CDESC_TYPE)
	puts ("alias");
      else if (dflags & CDESC_SHORTDESC)
	printf (_("%s is aliased to `%s'\n"), command, alias->value);
      else if (dflags & CDESC_REUSABLE)
	{
	  x = sh_single_quote (alias->value);
	  printf ("alias %s=%s\n", command, x);
	  free (x);
	}

      found = 1;

      if (all == 0)
	return (1);
    }
#endif /* ALIAS */

  /* Command is a shell reserved word? */
  if (((dflags & CDESC_FORCE_PATH) == 0) && (i = find_reserved_word (command)) >= 0)
    {
      if (dflags & CDESC_TYPE)
	puts ("keyword");
      else if (dflags & CDESC_SHORTDESC)
	printf (_("%s is a shell keyword\n"), command);
      else if (dflags & CDESC_REUSABLE)
	printf ("%s\n", command);

      found = 1;

      if (all == 0)
	return (1);
    }

  /* Command is a function? */
  if (((dflags & (CDESC_FORCE_PATH|CDESC_NOFUNCS)) == 0) && (func = find_function (command)))
    {
      if (dflags & CDESC_TYPE)
	puts ("function");
      else if (dflags & CDESC_SHORTDESC)
	{
	  char *result;

	  printf (_("%s is a function\n"), command);

	  /* We're blowing away THE_PRINTED_COMMAND here... */

	  result = named_function_string (command, function_cell (func), FUNC_MULTILINE|FUNC_EXTERNAL);
	  printf ("%s\n", result);
	}
      else if (dflags & CDESC_REUSABLE)
	printf ("%s\n", command);

      found = 1;

      if (all == 0)
	return (1);
    }

  /* Command is a builtin? */
  if (((dflags & CDESC_FORCE_PATH) == 0) && find_shell_builtin (command))
    {
      if (dflags & CDESC_TYPE)
	puts ("builtin");
      else if (dflags & CDESC_SHORTDESC)
	{
	  if (posixly_correct && find_special_builtin (command) != 0)
	    printf (_("%s is a special shell builtin\n"), command);
	  else
	    printf (_("%s is a shell builtin\n"), command);
	}
      else if (dflags & CDESC_REUSABLE)
	printf ("%s\n", command);

      found = 1;

      if (all == 0)
	return (1);
    }

  /* Command is a disk file? */
  /* If the command name given is already an absolute command, just
     check to see if it is executable. */
  if (absolute_program (command))
    {
      f = file_status (command);
      if (f & FS_EXECABLE)
	{
	  if (dflags & CDESC_TYPE)
	    puts ("file");
	  else if (dflags & CDESC_SHORTDESC)
	    printf (_("%s is %s\n"), command, command);
	  else if (dflags & (CDESC_REUSABLE|CDESC_PATH_ONLY))
	    printf ("%s\n", command);

	  /* There's no use looking in the hash table or in $PATH,
	     because they're not consulted when an absolute program
	     name is supplied. */
	  return (1);
	}
    }

  /* If the user isn't doing "-a", then we might care about
     whether the file is present in our hash table. */
  if (all == 0 || (dflags & CDESC_FORCE_PATH))
    {
      if (full_path = phash_search (command))
	{
	  if (dflags & CDESC_TYPE)
	    puts ("file");
	  else if (dflags & CDESC_SHORTDESC)
	    printf (_("%s is hashed (%s)\n"), command, full_path);
	  else if (dflags & (CDESC_REUSABLE|CDESC_PATH_ONLY))
	    printf ("%s\n", full_path);

	  free (full_path);
	  return (1);
	}
    }

  /* Now search through $PATH. */
  while (1)
    {
      if (dflags & CDESC_STDPATH)	/* command -p, all cannot be non-zero */
	{
	  pathlist = conf_standard_path ();
	  full_path = find_in_path (command, pathlist, FS_EXEC_PREFERRED|FS_NODIRS);
	  free (pathlist);
	  /* Will only go through this once, since all == 0 if STDPATH set */
	}
      else if (all == 0)
	full_path = find_user_command (command);
      else
	full_path = user_command_matches (command, FS_EXEC_ONLY, found_file);	/* XXX - should that be FS_EXEC_PREFERRED? */

      if (full_path == 0)
	break;

      /* If we found the command as itself by looking through $PATH, it
	 probably doesn't exist.  Check whether or not the command is an
	 executable file.  If it's not, don't report a match.  This is
	 the default posix mode behavior */
      if (STREQ (full_path, command) || posixly_correct)
	{
	  f = file_status (full_path);
	  if ((f & FS_EXECABLE) == 0)
	    {
	      free (full_path);
	      full_path = (char *)NULL;
	      if (all == 0)
		break;
	    }
	  else if (ABSPATH (full_path))
	    ;	/* placeholder; don't need to do anything yet */
	  else if (dflags & (CDESC_REUSABLE|CDESC_PATH_ONLY|CDESC_SHORTDESC))
	    {
	      f = MP_DOCWD | ((dflags & CDESC_ABSPATH) ? MP_RMDOT : 0);
	      x = sh_makepath ((char *)NULL, full_path, f);
	      free (full_path);
	      full_path = x;
	    }
	}
      /* If we require a full path and don't have one, make one */
      else if ((dflags & CDESC_ABSPATH) && ABSPATH (full_path) == 0)
	{
	  x = sh_makepath ((char *)NULL, full_path, MP_DOCWD|MP_RMDOT);
	  free (full_path);
	  full_path = x;
	}

      found_file++;
      found = 1;

      if (dflags & CDESC_TYPE)
	puts ("file");
      else if (dflags & CDESC_SHORTDESC)
	printf (_("%s is %s\n"), command, full_path);
      else if (dflags & (CDESC_REUSABLE|CDESC_PATH_ONLY))
	printf ("%s\n", full_path);

      free (full_path);
      full_path = (char *)NULL;

      if (all == 0)
	break;
    }

  return (found);
}
