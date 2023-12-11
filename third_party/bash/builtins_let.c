/* let.c, created from let.def. */
#line 66 "./let.def"

#include "config.h"

#if defined (HAVE_UNISTD_H)
#  ifdef _MINIX
#    include <sys/types.h>
#  endif
#  include <unistd.h>
#endif

#include "bashintl.h"

#include "shell.h"
#include "common.h"

/* Arithmetic LET function. */
int
let_builtin (list)
     WORD_LIST *list;
{
  intmax_t ret;
  int expok;

  CHECK_HELPOPT (list);

  /* Skip over leading `--' argument. */
  if (list && list->word && ISOPTION (list->word->word, '-'))
    list = list->next;

  if (list == 0)
    {
      builtin_error (_("expression expected"));
      return (EXECUTION_FAILURE);
    }

  for (; list; list = list->next)
    {
      ret = evalexp (list->word->word, EXP_EXPANDED, &expok);
      if (expok == 0)
	return (EXECUTION_FAILURE);
    }

  return ((ret == 0) ? EXECUTION_FAILURE : EXECUTION_SUCCESS);
}

#ifdef INCLUDE_UNUSED
int
exp_builtin (list)
     WORD_LIST *list;
{
  char *exp;
  intmax_t ret;
  int expok;

  if (list == 0)
    {
      builtin_error (_("expression expected"));
      return (EXECUTION_FAILURE);
    }

  exp = string_list (list);
  ret = evalexp (exp, EXP_EXPANDED, &expok);
  (void)free (exp);
  return (((ret == 0) || (expok == 0)) ? EXECUTION_FAILURE : EXECUTION_SUCCESS);
}
#endif
