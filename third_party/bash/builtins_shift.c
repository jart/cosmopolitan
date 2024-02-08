/* shift.c, created from shift.def. */
#line 22 "./shift.def"

#include "config.h"

#if defined (HAVE_UNISTD_H)
#  ifdef _MINIX
#    include <sys/types.h>
#  endif
#  include <unistd.h>
#endif

#include "bashansi.h"
#include "bashintl.h"

#include "shell.h"
#include "common.h"

#line 49 "./shift.def"

int print_shift_error;

/* Shift the arguments ``left''.  Shift DOLLAR_VARS down then take one
   off of REST_OF_ARGS and place it into DOLLAR_VARS[9].  If LIST has
   anything in it, it is a number which says where to start the
   shifting.  Return > 0 if `times' > $#, otherwise 0. */
int
shift_builtin (list)
     WORD_LIST *list;
{
  intmax_t times;
  int itimes, nargs;

  CHECK_HELPOPT (list);

  if (get_numeric_arg (list, 0, &times) == 0)
    return (EXECUTION_FAILURE);

  if (times == 0)
    return (EXECUTION_SUCCESS);
  else if (times < 0)
    {
      sh_erange (list ? list->word->word : NULL, _("shift count"));
      return (EXECUTION_FAILURE);
    }
  nargs = number_of_args ();
  if (times > nargs)
    {
      if (print_shift_error)
	sh_erange (list ? list->word->word : NULL, _("shift count"));
      return (EXECUTION_FAILURE);
    }
  else if (times == nargs)
    clear_dollar_vars ();
  else
    shift_args (itimes = times);

  invalidate_cached_quoted_dollar_at ();

  return (EXECUTION_SUCCESS);
}
