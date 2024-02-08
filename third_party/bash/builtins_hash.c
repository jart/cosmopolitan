/* hash.c, created from hash.def. */
#line 22 "./hash.def"

#line 46 "./hash.def"

#include "config.h"

#include <stdio.h>

#include "bashtypes.h"

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#include <errno.h>

#include "bashansi.h"
#include "bashintl.h"

#include "shell.h"
#include "builtins.h"
#include "execute_cmd.h"
#include "flags.h"
#include "findcmd.h"
#include "hashcmd.h"
#include "common.h"
#include "bashgetopt.h"

extern int dot_found_in_search;

static int add_hashed_command PARAMS((char *, int));
static int print_hash_info PARAMS((BUCKET_CONTENTS *));
static int print_portable_hash_info PARAMS((BUCKET_CONTENTS *));
static int print_hashed_commands PARAMS((int));
static int list_hashed_filename_targets PARAMS((WORD_LIST *, int));

/* Print statistics on the current state of hashed commands.  If LIST is
   not empty, then rehash (or hash in the first place) the specified
   commands. */
int
hash_builtin (list)
     WORD_LIST *list;
{
  int expunge_hash_table, list_targets, list_portably, delete, opt;
  char *w, *pathname;

  if (hashing_enabled == 0)
    {
      builtin_error (_("hashing disabled"));
      return (EXECUTION_FAILURE);
    }

  expunge_hash_table = list_targets = list_portably = delete = 0;
  pathname = (char *)NULL;
  reset_internal_getopt ();
  while ((opt = internal_getopt (list, "dlp:rt")) != -1)
    {
      switch (opt)
	{
	case 'd':
	  delete = 1;
	  break;
	case 'l':
	  list_portably = 1;
	  break;
	case 'p':
	  pathname = list_optarg;
	  break;
	case 'r':
	  expunge_hash_table = 1;
	  break;
	case 't':
	  list_targets = 1;
	  break;
	CASE_HELPOPT;
	default:
	  builtin_usage ();
	  return (EX_USAGE);
	}
    }
  list = loptend;

  /* hash -t requires at least one argument. */
  if (list == 0 && (delete || list_targets))
    {
      sh_needarg (delete ? "-d" : "-t");
      return (EXECUTION_FAILURE);
    }

  /* We want hash -r to be silent, but hash -- to print hashing info, so
     we test expunge_hash_table. */
  if (list == 0 && expunge_hash_table == 0)
    {
      opt = print_hashed_commands (list_portably);
      if (opt == 0 && posixly_correct == 0 &&
	    (list_portably == 0 || shell_compatibility_level <= 50))
	printf (_("%s: hash table empty\n"), this_command_name);

      return (sh_chkwrite (EXECUTION_SUCCESS));
    }

  if (expunge_hash_table)
    phash_flush ();

  /* If someone runs `hash -r -t xyz' he will be disappointed. */
  if (list_targets)
    return (list_hashed_filename_targets (list, list_portably));
      
#if defined (RESTRICTED_SHELL)
  if (restricted && pathname)
    {
      if (strchr (pathname, '/'))
	{
          sh_restricted (pathname);
          return (EXECUTION_FAILURE);
	}
      /* If we are changing the hash table in a restricted shell, make sure the
	 target pathname can be found using a $PATH search. */
      w = find_user_command (pathname);
      if (w == 0 || *w == 0 || executable_file (w) == 0)
	{
	  sh_notfound (pathname);
	  free (w);
	  return (EXECUTION_FAILURE);
	}
      free (w);
    }
#endif

  for (opt = EXECUTION_SUCCESS; list; list = list->next)
    {
      /* Add, remove or rehash the specified commands. */
      w = list->word->word;
      if (absolute_program (w))
	continue;
      else if (pathname)
	{
	  if (file_isdir (pathname))
	    {
#ifdef EISDIR
	      builtin_error ("%s: %s", pathname, strerror (EISDIR));
#else
	      builtin_error (_("%s: is a directory"), pathname);
#endif
	      opt = EXECUTION_FAILURE;
	    }
	  else
	    phash_insert (w, pathname, 0, 0);
	}
      else if (delete)
	{
	  if (phash_remove (w))
	    {
	      sh_notfound (w);
	      opt = EXECUTION_FAILURE;
	    }
	}
      else if (add_hashed_command (w, 0))
	opt = EXECUTION_FAILURE;
    }

  fflush (stdout);
  return (opt);
}

static int
add_hashed_command (w, quiet)
     char *w;
     int quiet;
{
  int rv;
  char *full_path;

  rv = 0;
  if (find_function (w) == 0 && find_shell_builtin (w) == 0)
    {
      phash_remove (w);
      full_path = find_user_command (w);
      if (full_path && executable_file (full_path))
	phash_insert (w, full_path, dot_found_in_search, 0);
      else
	{
	  if (quiet == 0)
	    sh_notfound (w);
	  rv++;
	}
      FREE (full_path);
    }
  return (rv);
}

/* Print information about current hashed info. */
static int
print_hash_info (item)
     BUCKET_CONTENTS *item;
{
  printf ("%4d\t%s\n", item->times_found, pathdata(item)->path);
  return 0;
}

static int
print_portable_hash_info (item)
     BUCKET_CONTENTS *item;
{
  char *fp, *fn;

  fp = printable_filename (pathdata(item)->path, 1);
  fn = printable_filename (item->key, 1);
  printf ("builtin hash -p %s %s\n", fp, fn);
  if (fp != pathdata(item)->path)
    free (fp);
  if (fn != item->key)
    free (fn);
  return 0;
}

static int
print_hashed_commands (fmt)
     int fmt;
{
  if (hashed_filenames == 0 || HASH_ENTRIES (hashed_filenames) == 0)
    return (0);

  if (fmt == 0)
    printf (_("hits\tcommand\n"));
  hash_walk (hashed_filenames, fmt ? print_portable_hash_info : print_hash_info);
  return (1);
}

static int
list_hashed_filename_targets (list, fmt)
     WORD_LIST *list;
     int fmt;
{
  int all_found, multiple;
  char *target;
  WORD_LIST *l;

  all_found = 1;
  multiple = list->next != 0;

  for (l = list; l; l = l->next)
    {
      target = phash_search (l->word->word);
      if (target == 0)
	{
	  all_found = 0;
	  sh_notfound (l->word->word);
	  continue;
	}
      if (fmt)
	printf ("builtin hash -p %s %s\n", target, l->word->word);
      else
	{
	  if (multiple)
	    printf ("%s\t", l->word->word);
	  printf ("%s\n", target);
	}
      free (target);
    }

  return (all_found ? EXECUTION_SUCCESS : EXECUTION_FAILURE);
}
