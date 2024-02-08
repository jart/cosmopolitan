/* break.c, created from break.def. */
#line 22 "./break.def"

#line 34 "./break.def"
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

static int check_loop_level PARAMS((void));

/* The depth of while's and until's. */
int loop_level = 0;

/* Non-zero when a "break" instruction is encountered. */
int breaking = 0;

/* Non-zero when we have encountered a continue instruction. */
int continuing = 0;

/* Set up to break x levels, where x defaults to 1, but can be specified
   as the first argument. */
int
break_builtin (list)
     WORD_LIST *list;
{
  intmax_t newbreak;

  CHECK_HELPOPT (list);

  if (check_loop_level () == 0)
    return (EXECUTION_SUCCESS);

  (void)get_numeric_arg (list, 1, &newbreak);

  if (newbreak <= 0)
    {
      sh_erange (list->word->word, _("loop count"));
      breaking = loop_level;
      return (EXECUTION_FAILURE);
    }

  if (newbreak > loop_level)
    newbreak = loop_level;

  breaking = newbreak;

  return (EXECUTION_SUCCESS);
}

#line 101 "./break.def"

/* Set up to continue x levels, where x defaults to 1, but can be specified
   as the first argument. */
int
continue_builtin (list)
     WORD_LIST *list;
{
  intmax_t newcont;

  CHECK_HELPOPT (list);

  if (check_loop_level () == 0)
    return (EXECUTION_SUCCESS);

  (void)get_numeric_arg (list, 1, &newcont);

  if (newcont <= 0)
    {
      sh_erange (list->word->word, _("loop count"));
      breaking = loop_level;
      return (EXECUTION_FAILURE);
    }

  if (newcont > loop_level)
    newcont = loop_level;

  continuing = newcont;

  return (EXECUTION_SUCCESS);
}

/* Return non-zero if a break or continue command would be okay.
   Print an error message if break or continue is meaningless here. */
static int
check_loop_level ()
{
#if defined (BREAK_COMPLAINS)
  if (loop_level == 0 && posixly_correct == 0)
    builtin_error (_("only meaningful in a `for', `while', or `until' loop"));
#endif /* BREAK_COMPLAINS */

  return (loop_level);
}
