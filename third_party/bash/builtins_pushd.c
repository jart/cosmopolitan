/* pushd.c, created from pushd.def. */
#line 22 "./pushd.def"

#line 55 "./pushd.def"

#line 84 "./pushd.def"

#line 115 "./pushd.def"

#include "config.h"

#if defined (PUSHD_AND_POPD)
#include <stdio.h>
#if defined (HAVE_SYS_PARAM_H)
#  include <sys/param.h>
#endif

#if defined (HAVE_UNISTD_H)
#  ifdef _MINIX
#    include <sys/types.h>
#  endif
#  include <unistd.h>
#endif

#include "bashansi.h"
#include "bashintl.h"

#include <errno.h>

#include "tilde.h"

#include "shell.h"
#include "maxpath.h"
#include "common.h"
#include "builtext.h"

#ifdef LOADABLE_BUILTIN
#  include "builtins.h"
#endif

#if !defined (errno)
extern int errno;
#endif /* !errno */

/* The list of remembered directories. */
static char **pushd_directory_list = (char **)NULL;

/* Number of existing slots in this list. */
static int directory_list_size;

/* Offset to the end of the list. */
static int directory_list_offset;

static void pushd_error PARAMS((int, char *));
static void clear_directory_stack PARAMS((void));
static int cd_to_string PARAMS((char *));
static int change_to_temp PARAMS((char *));
static void add_dirstack_element PARAMS((char *));
static int get_dirstack_index PARAMS((intmax_t, int, int *));

#define NOCD		0x01
#define ROTATE		0x02
#define LONGFORM	0x04
#define CLEARSTAK	0x08

int
pushd_builtin (list)
     WORD_LIST *list;
{
  WORD_LIST *orig_list;
  char *temp, *current_directory, *top;
  int j, flags, skipopt;
  intmax_t num;
  char direction;

  orig_list = list;

  CHECK_HELPOPT (list);
  if (list && list->word && ISOPTION (list->word->word, '-'))
    {
      list = list->next;
      skipopt = 1;
    }
  else
    skipopt = 0;

  /* If there is no argument list then switch current and
     top of list. */
  if (list == 0)
    {
      if (directory_list_offset == 0)
	{
	  builtin_error (_("no other directory"));
	  return (EXECUTION_FAILURE);
	}

      current_directory = get_working_directory ("pushd");
      if (current_directory == 0)
	return (EXECUTION_FAILURE);

      j = directory_list_offset - 1;
      temp = pushd_directory_list[j];
      pushd_directory_list[j] = current_directory;
      j = change_to_temp (temp);
      free (temp);
      return j;
    }

  for (flags = 0; skipopt == 0 && list; list = list->next)
    {
      if (ISOPTION (list->word->word, 'n'))
	{
	  flags |= NOCD;
	}
      else if (ISOPTION (list->word->word, '-'))
	{
	  list = list->next;
	  break;
	}
      else if (list->word->word[0] == '-' && list->word->word[1] == '\0')
	/* Let `pushd -' work like it used to. */
	break;
      else if (((direction = list->word->word[0]) == '+') || direction == '-')
	{
	  if (legal_number (list->word->word + 1, &num) == 0)
	    {
	      sh_invalidnum (list->word->word);
	      builtin_usage ();
	      return (EX_USAGE);
	    }

	  if (direction == '-')
	    num = directory_list_offset - num;

	  if (num > directory_list_offset || num < 0)
	    {
	      pushd_error (directory_list_offset, list->word->word);
	      return (EXECUTION_FAILURE);
	    }
	  flags |= ROTATE;
	}
      else if (*list->word->word == '-')
	{
	  sh_invalidopt (list->word->word);
	  builtin_usage ();
	  return (EX_USAGE);
	}
      else
	break;
    }

  if (flags & ROTATE)
    {
      /* Rotate the stack num times.  Remember, the current
	 directory acts like it is part of the stack. */
      temp = get_working_directory ("pushd");

      if (num == 0)
	{
	  j = ((flags & NOCD) == 0) ? change_to_temp (temp) : EXECUTION_SUCCESS;
	  free (temp);
	  return j;
	}

      do
	{
	  top = pushd_directory_list[directory_list_offset - 1];

	  for (j = directory_list_offset - 2; j > -1; j--)
	    pushd_directory_list[j + 1] = pushd_directory_list[j];

	  pushd_directory_list[j + 1] = temp;

	  temp = top;
	  num--;
	}
      while (num);

      j = ((flags & NOCD) == 0) ? change_to_temp (temp) : EXECUTION_SUCCESS;
      free (temp);
      return j;
    }

  if (list == 0)
    return (EXECUTION_SUCCESS);

  /* Change to the directory in list->word->word.  Save the current
     directory on the top of the stack. */
  current_directory = get_working_directory ("pushd");
  if (current_directory == 0)
    return (EXECUTION_FAILURE);

  j = ((flags & NOCD) == 0) ? cd_builtin (skipopt ? orig_list : list) : EXECUTION_SUCCESS;
  if (j == EXECUTION_SUCCESS)
    {
      add_dirstack_element ((flags & NOCD) ? savestring (list->word->word) : current_directory);
      dirs_builtin ((WORD_LIST *)NULL);
      if (flags & NOCD)
	free (current_directory);
      return (EXECUTION_SUCCESS);
    }
  else
    {
      free (current_directory);
      return (EXECUTION_FAILURE);
    }
}

/* Pop the directory stack, and then change to the new top of the stack.
   If LIST is non-null it should consist of a word +N or -N, which says
   what element to delete from the stack.  The default is the top one. */
int
popd_builtin (list)
     WORD_LIST *list;
{
  register int i;
  intmax_t which;
  int flags;
  char direction;
  char *which_word;

  CHECK_HELPOPT (list);

  which_word = (char *)NULL;
  for (flags = 0, which = 0, direction = '+'; list; list = list->next)
    {
      if (ISOPTION (list->word->word, 'n'))
	{
	  flags |= NOCD;
	}
      else if (ISOPTION (list->word->word, '-'))
	{
	  list = list->next;
	  break;
	}
      else if (((direction = list->word->word[0]) == '+') || direction == '-')
	{
	  if (legal_number (list->word->word + 1, &which) == 0)
	    {
	      sh_invalidnum (list->word->word);
	      builtin_usage ();
	      return (EX_USAGE);
	    }
	  which_word = list->word->word;
	}
      else if (*list->word->word == '-')
	{
	  sh_invalidopt (list->word->word);
	  builtin_usage ();
	  return (EX_USAGE);
	}
      else if (*list->word->word)
	{
	  builtin_error (_("%s: invalid argument"), list->word->word);
	  builtin_usage ();
	  return (EX_USAGE);
	}
      else
	break;
    }

  if (which > directory_list_offset || (which < -directory_list_offset) || (directory_list_offset == 0 && which == 0))
    {
      pushd_error (directory_list_offset, which_word ? which_word : "");
      return (EXECUTION_FAILURE);
    }

  /* Handle case of no specification, or top of stack specification. */
  if ((direction == '+' && which == 0) ||
      (direction == '-' && which == directory_list_offset))
    {
      i = ((flags & NOCD) == 0) ? cd_to_string (pushd_directory_list[directory_list_offset - 1])
      				: EXECUTION_SUCCESS;
      if (i != EXECUTION_SUCCESS)
	return (i);
      free (pushd_directory_list[--directory_list_offset]);
    }
  else
    {
      /* Since an offset other than the top directory was specified,
	 remove that directory from the list and shift the remainder
	 of the list into place. */
      i = (direction == '+') ? directory_list_offset - which : which;
      if (i < 0 || i > directory_list_offset)
	{
	  pushd_error (directory_list_offset, which_word ? which_word : "");
	  return (EXECUTION_FAILURE);
	}
      free (pushd_directory_list[i]);
      directory_list_offset--;

      /* Shift the remainder of the list into place. */
      for (; i < directory_list_offset; i++)
	pushd_directory_list[i] = pushd_directory_list[i + 1];
    }

  dirs_builtin ((WORD_LIST *)NULL);
  return (EXECUTION_SUCCESS);
}

/* Print the current list of directories on the directory stack. */
int
dirs_builtin (list)
     WORD_LIST *list;
{
  int flags, desired_index, index_flag, vflag;
  intmax_t i;
  char *temp, *w;

  CHECK_HELPOPT (list);
  for (flags = vflag = index_flag = 0, desired_index = -1, w = ""; list; list = list->next)
    {
      if (ISOPTION (list->word->word, 'l'))
	{
	  flags |= LONGFORM;
	}
      else if (ISOPTION (list->word->word, 'c'))
	{
	  flags |= CLEARSTAK;
	}
      else if (ISOPTION (list->word->word, 'v'))
	{
	  vflag |= 2;
	}
      else if (ISOPTION (list->word->word, 'p'))
	{
	  vflag |= 1;
	}
      else if (ISOPTION (list->word->word, '-'))
	{
	  list = list->next;
	  break;
	}
      else if (*list->word->word == '+' || *list->word->word == '-')
	{
	  int sign;
	  if (legal_number (w = list->word->word + 1, &i) == 0)
	    {
	      sh_invalidnum (list->word->word);
	      builtin_usage ();
	      return (EX_USAGE);
	    }
	  sign = (*list->word->word == '+') ? 1 : -1;
	  desired_index = get_dirstack_index (i, sign, &index_flag);
	}
      else
	{
	  sh_invalidopt (list->word->word);
	  builtin_usage ();
	  return (EX_USAGE);
	}
    }

  if (flags & CLEARSTAK)
    {
      clear_directory_stack ();
      return (EXECUTION_SUCCESS);
    }

  if (index_flag && (desired_index < 0 || desired_index > directory_list_offset))
    {
      pushd_error (directory_list_offset, w);
      return (EXECUTION_FAILURE);
    }

#define DIRSTACK_FORMAT(temp) \
  (flags & LONGFORM) ? temp : polite_directory_format (temp)

  /* The first directory printed is always the current working directory. */
  if (index_flag == 0 || (index_flag == 1 && desired_index == 0))
    {
      temp = get_working_directory ("dirs");
      if (temp == 0)
	temp = savestring (_("<no current directory>"));
      if (vflag & 2)
	printf ("%2d  %s", 0, DIRSTACK_FORMAT (temp));
      else
	printf ("%s", DIRSTACK_FORMAT (temp));
      free (temp);
      if (index_flag)
	{
	  putchar ('\n');
	  return (sh_chkwrite (EXECUTION_SUCCESS));
	}
    }

#define DIRSTACK_ENTRY(i) \
  (flags & LONGFORM) ? pushd_directory_list[i] \
		     : polite_directory_format (pushd_directory_list[i])

  /* Now print the requested directory stack entries. */
  if (index_flag)
    {
      if (vflag & 2)
	printf ("%2d  %s", directory_list_offset - desired_index,
			   DIRSTACK_ENTRY (desired_index));
      else
	printf ("%s", DIRSTACK_ENTRY (desired_index));
    }
  else
    for (i = directory_list_offset - 1; i >= 0; i--)
      if (vflag >= 2)
	printf ("\n%2d  %s", directory_list_offset - (int)i, DIRSTACK_ENTRY (i));
      else
	printf ("%s%s", (vflag & 1) ? "\n" : " ", DIRSTACK_ENTRY (i));

  putchar ('\n');

  return (sh_chkwrite (EXECUTION_SUCCESS));
}

static void
pushd_error (offset, arg)
     int offset;
     char *arg;
{
  if (offset == 0)
    builtin_error (_("directory stack empty"));
  else
    sh_erange (arg, _("directory stack index"));
}

static void
clear_directory_stack ()
{
  register int i;

  for (i = 0; i < directory_list_offset; i++)
    free (pushd_directory_list[i]);
  directory_list_offset = 0;
}

/* Switch to the directory in NAME.  This uses the cd_builtin to do the work,
   so if the result is EXECUTION_FAILURE then an error message has already
   been printed. */
static int
cd_to_string (name)
     char *name;
{
  WORD_LIST *tlist;
  WORD_LIST *dir;
  int result;

  dir = make_word_list (make_word (name), NULL);
  tlist = make_word_list (make_word ("--"), dir);
  result = cd_builtin (tlist);
  dispose_words (tlist);
  return (result);
}

static int
change_to_temp (temp)
     char *temp;
{
  int tt;

  tt = temp ? cd_to_string (temp) : EXECUTION_FAILURE;

  if (tt == EXECUTION_SUCCESS)
    dirs_builtin ((WORD_LIST *)NULL);

  return (tt);
}

static void
add_dirstack_element (dir)
     char *dir;
{
  if (directory_list_offset == directory_list_size)
    pushd_directory_list = strvec_resize (pushd_directory_list, directory_list_size += 10);
  pushd_directory_list[directory_list_offset++] = dir;
}

static int
get_dirstack_index (ind, sign, indexp)
     intmax_t ind;
     int sign, *indexp;
{
  if (indexp)
    *indexp = sign > 0 ? 1 : 2;

  /* dirs +0 prints the current working directory. */
  /* dirs -0 prints last element in directory stack */
  if (ind == 0 && sign > 0)
    return 0;
  else if (ind == directory_list_offset)
    {
      if (indexp)
	*indexp = sign > 0 ? 2 : 1;
      return 0;
    }
  else if (ind >= 0 && ind <= directory_list_offset)
    return (sign > 0 ? directory_list_offset - ind : ind);
  else
    return -1;
}

/* Used by the tilde expansion code. */
char *
get_dirstack_from_string (string)
     char *string;
{
  int ind, sign, index_flag;
  intmax_t i;

  sign = 1;
  if (*string == '-' || *string == '+')
    {
      sign = (*string == '-') ? -1 : 1;
      string++;
    }
  if (legal_number (string, &i) == 0)
    return ((char *)NULL);

  index_flag = 0;
  ind = get_dirstack_index (i, sign, &index_flag);
  if (index_flag && (ind < 0 || ind > directory_list_offset))
    return ((char *)NULL);
  if (index_flag == 0 || (index_flag == 1 && ind == 0))
    return (get_string_value ("PWD"));
  else
    return (pushd_directory_list[ind]);
}

#ifdef INCLUDE_UNUSED
char *
get_dirstack_element (ind, sign)
     intmax_t ind;
     int sign;
{
  int i;

  i = get_dirstack_index (ind, sign, (int *)NULL);
  return (i < 0 || i > directory_list_offset) ? (char *)NULL
					      : pushd_directory_list[i];
}
#endif

void
set_dirstack_element (ind, sign, value)
     intmax_t ind;
     int  sign;
     char *value;
{
  int i;

  i = get_dirstack_index (ind, sign, (int *)NULL);
  if (ind == 0 || i < 0 || i > directory_list_offset)
    return;
  free (pushd_directory_list[i]);
  pushd_directory_list[i] = savestring (value);
}

WORD_LIST *
get_directory_stack (flags)
     int flags;
{
  register int i;
  WORD_LIST *ret;
  char *d, *t;

  for (ret = (WORD_LIST *)NULL, i = 0; i < directory_list_offset; i++)
    {
      d = (flags&1) ? polite_directory_format (pushd_directory_list[i])
		    : pushd_directory_list[i];
      ret = make_word_list (make_word (d), ret);
    }
  /* Now the current directory. */
  d = get_working_directory ("dirstack");
  i = 0;	/* sentinel to decide whether or not to free d */
  if (d == 0)
    d = ".";
  else
    {
      t = (flags&1) ? polite_directory_format (d) : d;
      /* polite_directory_format sometimes returns its argument unchanged.
	 If it does not, we can free d right away.  If it does, we need to
	 mark d to be deleted later. */
      if (t != d)
	{
	  free (d);
	  d = t;
	}
      else /* t == d, so d is what we want */
	i = 1;
    }
  ret = make_word_list (make_word (d), ret);
  if (i)
    free (d);
  return ret;	/* was (REVERSE_LIST (ret, (WORD_LIST *)); */
}

#ifdef LOADABLE_BUILTIN
char * const dirs_doc[] = {
N_("Display the list of currently remembered directories.  Directories\n\
    find their way onto the list with the `pushd' command; you can get\n\
    back up through the list with the `popd' command.\n\
    \n\
    Options:\n\
      -c	clear the directory stack by deleting all of the elements\n\
      -l	do not print tilde-prefixed versions of directories relative\n\
    	to your home directory\n\
      -p	print the directory stack with one entry per line\n\
      -v	print the directory stack with one entry per line prefixed\n\
    	with its position in the stack\n\
    \n\
    Arguments:\n\
      +N	Displays the Nth entry counting from the left of the list shown by\n\
    	dirs when invoked without options, starting with zero.\n\
    \n\
      -N	Displays the Nth entry counting from the right of the list shown by\n\
	dirs when invoked without options, starting with zero."),
  (char *)NULL
};

char * const pushd_doc[] = {
N_("Adds a directory to the top of the directory stack, or rotates\n\
    the stack, making the new top of the stack the current working\n\
    directory.  With no arguments, exchanges the top two directories.\n\
    \n\
    Options:\n\
      -n	Suppresses the normal change of directory when adding\n\
    	directories to the stack, so only the stack is manipulated.\n\
    \n\
    Arguments:\n\
      +N	Rotates the stack so that the Nth directory (counting\n\
    	from the left of the list shown by `dirs', starting with\n\
    	zero) is at the top.\n\
    \n\
      -N	Rotates the stack so that the Nth directory (counting\n\
    	from the right of the list shown by `dirs', starting with\n\
    	zero) is at the top.\n\
    \n\
      dir	Adds DIR to the directory stack at the top, making it the\n\
    	new current working directory.\n\
    \n\
    The `dirs' builtin displays the directory stack."),
  (char *)NULL
};

char * const popd_doc[] = {
N_("Removes entries from the directory stack.  With no arguments, removes\n\
    the top directory from the stack, and changes to the new top directory.\n\
    \n\
    Options:\n\
      -n	Suppresses the normal change of directory when removing\n\
    	directories from the stack, so only the stack is manipulated.\n\
    \n\
    Arguments:\n\
      +N	Removes the Nth entry counting from the left of the list\n\
    	shown by `dirs', starting with zero.  For example: `popd +0'\n\
    	removes the first directory, `popd +1' the second.\n\
    \n\
      -N	Removes the Nth entry counting from the right of the list\n\
    	shown by `dirs', starting with zero.  For example: `popd -0'\n\
    	removes the last directory, `popd -1' the next to last.\n\
    \n\
    The `dirs' builtin displays the directory stack."),
  (char *)NULL
};

struct builtin pushd_struct = {
	"pushd",
	pushd_builtin,
	BUILTIN_ENABLED,
	pushd_doc,
	"pushd [+N | -N] [-n] [dir]",
	0
};

struct builtin popd_struct = {
	"popd",
	popd_builtin,
	BUILTIN_ENABLED,
	popd_doc,
	"popd [+N | -N] [-n]",
	0
};

struct builtin dirs_struct = {
	"dirs",
	dirs_builtin,
	BUILTIN_ENABLED,
	dirs_doc,
	"dirs [-clpv] [+N] [-N]",
	0
};
#endif /* LOADABLE_BUILTIN */

#endif /* PUSHD_AND_POPD */
