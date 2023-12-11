/* colon.c, created from colon.def. */
#line 22 "./colon.def"

#line 34 "./colon.def"

#line 43 "./colon.def"

#line 52 "./colon.def"

#include "config.h"

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#include "bashansi.h"
#include "shell.h"

/* Return a successful result. */
int
colon_builtin (ignore)
     WORD_LIST *ignore;
{
  return (0);
}

/* Return an unsuccessful result. */
int
false_builtin (ignore)
     WORD_LIST *ignore;
{
  return (1);
}
