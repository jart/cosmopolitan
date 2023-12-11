/* return.c, created from return.def. */
#line 22 "./return.def"

#line 36 "./return.def"

#include "config.h"

#if defined (HAVE_UNISTD_H)
#  ifdef _MINIX
#    include <sys/types.h>
#  endif
#  include <unistd.h>
#endif

#include "bashintl.h"

#include "shell.h"
#include "execute_cmd.h"
#include "common.h"
#include "bashgetopt.h"

/* If we are executing a user-defined function then exit with the value
   specified as an argument.  if no argument is given, then the last
   exit status is used. */
int
return_builtin (list)
     WORD_LIST *list;
{
  CHECK_HELPOPT (list);

  return_catch_value = get_exitstat (list);

  if (return_catch_flag)
    sh_longjmp (return_catch, 1);
  else
    {
      builtin_error (_("can only `return' from a function or sourced script"));
      return (EX_USAGE);
    }
}
