/* command.c, created from command.def. */
#line 22 "./command.def"

#line 41 "./command.def"

#include "config.h"

#if defined (HAVE_UNISTD_H)
#  ifdef _MINIX
#    include <sys/types.h>
#  endif
#  include <unistd.h>
#endif

#include "bashansi.h"

#include "shell.h"
#include "execute_cmd.h"
#include "flags.h"
#include "bashgetopt.h"
#include "common.h"

#if defined (_CS_PATH) && defined (HAVE_CONFSTR) && !HAVE_DECL_CONFSTR
extern size_t confstr PARAMS((int, char *, size_t));
#endif

/* Run the commands mentioned in LIST without paying attention to shell
   functions. */
int
command_builtin (list)
     WORD_LIST *list;
{
  int result, verbose, use_standard_path, opt;
  COMMAND *command;

  verbose = use_standard_path = 0;
  reset_internal_getopt ();
  while ((opt = internal_getopt (list, "pvV")) != -1)
    {
      switch (opt)
	{
	case 'p':
	  use_standard_path = CDESC_STDPATH;
	  break;
	case 'V':
	  verbose = CDESC_SHORTDESC|CDESC_ABSPATH;	/* look in common.h for constants */
	  break;
	case 'v':
	  verbose = CDESC_REUSABLE;	/* ditto */
	  break;
	CASE_HELPOPT;
	default:
	  builtin_usage ();
	  return (EX_USAGE);
	}
    }
  list = loptend;

  if (list == 0)
    return (EXECUTION_SUCCESS);

#if defined (RESTRICTED_SHELL)
  if (use_standard_path && restricted)
    {
      sh_restricted ("-p");
      return (EXECUTION_FAILURE);
    }
#endif

  if (verbose)
    {
      int found, any_found;

      for (any_found = 0; list; list = list->next)
	{
	  found = describe_command (list->word->word, verbose|use_standard_path);

	  if (found == 0 && verbose != CDESC_REUSABLE)
	    sh_notfound (list->word->word);

	  any_found += found;
	}

      return (any_found ? EXECUTION_SUCCESS : EXECUTION_FAILURE);
    }

  begin_unwind_frame ("command_builtin");

#define COMMAND_BUILTIN_FLAGS (CMD_NO_FUNCTIONS | CMD_INHIBIT_EXPANSION | CMD_COMMAND_BUILTIN | (use_standard_path ? CMD_STDPATH : 0))

  INTERNAL_DEBUG (("command_builtin: running execute_command for `%s'", list->word->word));

  /* We don't want this to be reparsed (consider command echo 'foo &'), so
     just make a simple_command structure and call execute_command with it. */
  command = make_bare_simple_command ();
  command->value.Simple->words = (WORD_LIST *)copy_word_list (list);
  command->value.Simple->redirects = (REDIRECT *)NULL;
  command->flags |= COMMAND_BUILTIN_FLAGS;
  command->value.Simple->flags |= COMMAND_BUILTIN_FLAGS;

  add_unwind_protect ((char *)dispose_command, command);
  result = execute_command (command);

  run_unwind_frame ("command_builtin");

  return (result);
}
