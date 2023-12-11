/* eval.c, created from eval.def. */
#line 22 "./eval.def"

#line 34 "./eval.def"

#include "config.h"
#if defined (HAVE_UNISTD_H)
#  ifdef _MINIX
#    include <sys/types.h>
#  endif
#  include <unistd.h>
#endif

#include "shell.h"
#include "bashgetopt.h"
#include "common.h"

/* Parse the string that these words make, and execute the command found. */
int
eval_builtin (list)
     WORD_LIST *list;
{
  if (no_options (list))
    return (EX_USAGE);
  list = loptend;	/* skip over possible `--' */

  return (list ? evalstring (string_list (list), "eval", SEVAL_NOHIST) : EXECUTION_SUCCESS);
}
