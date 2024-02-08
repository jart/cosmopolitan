/* caller.c, created from caller.def. */
#line 23 "./caller.def"

#line 41 "./caller.def"

#include "config.h"
#include <stdio.h>
#include "chartypes.h"
#include "bashtypes.h"

#if defined (HAVE_UNISTD_H)
#  ifdef _MINIX
#    include <sys/types.h>
#  endif
#  include <unistd.h>
#endif

#include <errno.h>

#include "bashintl.h"

#include "shell.h"
#include "common.h"
#include "builtext.h"
#include "bashgetopt.h"

#ifdef LOADABLE_BUILTIN
#  include "builtins.h"
#endif

#if !defined (errno)
extern int errno;
#endif /* !errno */

int
caller_builtin (list)
     WORD_LIST *list;
{
#if !defined (ARRAY_VARS)
  printf ("1 NULL\n");
  return (EXECUTION_FAILURE);
#else
  SHELL_VAR *funcname_v, *bash_source_v, *bash_lineno_v;
  ARRAY *funcname_a, *bash_source_a, *bash_lineno_a;
  char *funcname_s, *source_s, *lineno_s;
  intmax_t num;

  CHECK_HELPOPT (list);

  GET_ARRAY_FROM_VAR ("FUNCNAME", funcname_v, funcname_a);
  GET_ARRAY_FROM_VAR ("BASH_SOURCE", bash_source_v, bash_source_a);
  GET_ARRAY_FROM_VAR ("BASH_LINENO", bash_lineno_v, bash_lineno_a);

  if (bash_lineno_a == 0 || array_empty (bash_lineno_a))
    return (EXECUTION_FAILURE);

  if (bash_source_a == 0 || array_empty (bash_source_a))
    return (EXECUTION_FAILURE);

 if (no_options (list))
    return (EX_USAGE);
  list = loptend;       /* skip over possible `--' */

  /* If there is no argument list, then give short form: line filename. */
  if (list == 0)
    {
      lineno_s = array_reference (bash_lineno_a, 0);
      source_s = array_reference (bash_source_a, 1);
      printf("%s %s\n", lineno_s ? lineno_s : "NULL", source_s ? source_s : "NULL");
      return (EXECUTION_SUCCESS);
    }
  
  if (funcname_a == 0 || array_empty (funcname_a))
    return (EXECUTION_FAILURE);

  if (legal_number (list->word->word, &num))
    {
      lineno_s = array_reference (bash_lineno_a, num);
      source_s = array_reference (bash_source_a, num+1);
      funcname_s = array_reference (funcname_a, num+1);

      if (lineno_s == NULL|| source_s == NULL || funcname_s == NULL)
	return (EXECUTION_FAILURE);

      printf("%s %s %s\n", lineno_s, funcname_s, source_s);
    }
  else
    {
      sh_invalidnum (list->word->word);
      builtin_usage ();
      return (EX_USAGE);
    }

  return (EXECUTION_SUCCESS);
#endif
}

#ifdef LOADABLE_BUILTIN
static char *caller_doc[] = {
N_("Returns the context of the current subroutine call.\n\
    \n\
    Without EXPR, returns \"$line $filename\".  With EXPR, returns\n\
    \"$line $subroutine $filename\"; this extra information can be used to\n\
    provide a stack trace.\n\
    \n\
    The value of EXPR indicates how many call frames to go back before the\n\
    current one; the top frame is frame 0."),
  (char *)NULL
};

struct builtin caller_struct = {
	"caller",
	caller_builtin,
	BUILTIN_ENABLED,
	caller_doc,
	"caller [EXPR]",
	0
};

#endif /* LOADABLE_BUILTIN */
