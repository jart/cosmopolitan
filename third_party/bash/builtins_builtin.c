/* builtin.c, created from builtin.def. */
#line 22 "./builtin.def"

#line 36 "./builtin.def"
#include "config.h"

#if defined (HAVE_UNISTD_H)
#  ifdef _MINIX
#    include <sys/types.h>
#  endif
#  include <unistd.h>
#endif

#include "shell.h"
#include "execute_cmd.h"
#include "common.h"
#include "bashgetopt.h"

/* Run the command mentioned in list directly, without going through the
   normal alias/function/builtin/filename lookup process. */
int
builtin_builtin (list)
     WORD_LIST *list;
{
  sh_builtin_func_t *function;
  register char *command;

  if (no_options (list))
    return (EX_USAGE);
  list = loptend;	/* skip over possible `--' */

  if (list == 0)
    return (EXECUTION_SUCCESS);

  command = list->word->word;
#if defined (DISABLED_BUILTINS)
  function = builtin_address (command);
#else /* !DISABLED_BUILTINS */
  function = find_shell_builtin (command);
#endif /* !DISABLED_BUILTINS */

  if (function == 0)
    {
      sh_notbuiltin (command);
      return (EXECUTION_FAILURE);
    }
  else
    {
      this_command_name = command;
      this_shell_builtin = function;	/* overwrite "builtin" as this builtin */
      list = list->next;
      return ((*function) (list));
    }
}
