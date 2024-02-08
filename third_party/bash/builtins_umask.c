/* umask.c, created from umask.def. */
#line 22 "./umask.def"

#line 41 "./umask.def"

#include "config.h"

#include "bashtypes.h"
#include "filecntl.h"
#if ! defined(_MINIX) && defined (HAVE_SYS_FILE_H)
#  include <sys/file.h>
#endif

#if defined (HAVE_UNISTD_H)
#include <unistd.h>
#endif

#include <stdio.h>
#include "chartypes.h"

#include "bashintl.h"

#include "shell.h"
#include "posixstat.h"
#include "common.h"
#include "bashgetopt.h"

/* **************************************************************** */
/*                                                                  */
/*                     UMASK Builtin and Helpers                    */
/*                                                                  */
/* **************************************************************** */

static void print_symbolic_umask PARAMS((mode_t));
static int symbolic_umask PARAMS((WORD_LIST *));

/* Set or display the mask used by the system when creating files.  Flag
   of -S means display the umask in a symbolic mode. */
int
umask_builtin (list)
     WORD_LIST *list;
{
  int print_symbolically, opt, umask_value, pflag;
  mode_t umask_arg;

  print_symbolically = pflag = 0;
  reset_internal_getopt ();
  while ((opt = internal_getopt (list, "Sp")) != -1)
    {
      switch (opt)
	{
	case 'S':
	  print_symbolically++;
	  break;
	case 'p':
	  pflag++;
	  break;
	CASE_HELPOPT;
	default:
	  builtin_usage ();
	  return (EX_USAGE);
	}
    }

  list = loptend;

  if (list)
    {
      if (DIGIT (*list->word->word))
	{
	  umask_value = read_octal (list->word->word);

	  /* Note that other shells just let you set the umask to zero
	     by specifying a number out of range.  This is a problem
	     with those shells.  We don't change the umask if the input
	     is lousy. */
	  if (umask_value == -1)
	    {
	      sh_erange (list->word->word, _("octal number"));
	      return (EXECUTION_FAILURE);
	    }
	}
      else
	{
	  umask_value = symbolic_umask (list);
	  if (umask_value == -1)
	    return (EXECUTION_FAILURE);
	}
      umask_arg = (mode_t)umask_value;
      umask (umask_arg);
      if (print_symbolically)
	print_symbolic_umask (umask_arg);
    }
  else				/* Display the UMASK for this user. */
    {
      umask_arg = umask (022);
      umask (umask_arg);

      if (pflag)
	printf ("umask%s ", (print_symbolically ? " -S" : ""));
      if (print_symbolically)
	print_symbolic_umask (umask_arg);
      else
	printf ("%04lo\n", (unsigned long)umask_arg);
    }

  return (sh_chkwrite (EXECUTION_SUCCESS));
}

/* Print the umask in a symbolic form.  In the output, a letter is
   printed if the corresponding bit is clear in the umask. */
static void
#if defined (__STDC__)
print_symbolic_umask (mode_t um)
#else
print_symbolic_umask (um)
     mode_t um;
#endif
{
  char ubits[4], gbits[4], obits[4];		/* u=rwx,g=rwx,o=rwx */
  int i;

  i = 0;
  if ((um & S_IRUSR) == 0)
    ubits[i++] = 'r';
  if ((um & S_IWUSR) == 0)
    ubits[i++] = 'w';
  if ((um & S_IXUSR) == 0)
    ubits[i++] = 'x';
  ubits[i] = '\0';

  i = 0;
  if ((um & S_IRGRP) == 0)
    gbits[i++] = 'r';
  if ((um & S_IWGRP) == 0)
    gbits[i++] = 'w';
  if ((um & S_IXGRP) == 0)
    gbits[i++] = 'x';
  gbits[i] = '\0';

  i = 0;
  if ((um & S_IROTH) == 0)
    obits[i++] = 'r';
  if ((um & S_IWOTH) == 0)
    obits[i++] = 'w';
  if ((um & S_IXOTH) == 0)
    obits[i++] = 'x';
  obits[i] = '\0';

  printf ("u=%s,g=%s,o=%s\n", ubits, gbits, obits);
}

int
parse_symbolic_mode (mode, initial_bits)
     char *mode;
     int initial_bits;
{
  int who, op, perm, bits, c;
  char *s;

  for (s = mode, bits = initial_bits;;)
    {
      who = op = perm = 0;

      /* Parse the `who' portion of the symbolic mode clause. */
      while (member (*s, "agou"))
	{
	  switch (c = *s++)
	    {
	    case 'u':
	      who |= S_IRWXU;
	      continue;
	    case 'g':
	      who |= S_IRWXG;
	      continue;
	    case 'o':
	      who |= S_IRWXO;
	      continue;
	    case 'a':
	      who |= S_IRWXU | S_IRWXG | S_IRWXO;
	      continue;
	    default:
	      break;
	    }
	}

      /* The operation is now sitting in *s. */
      op = *s++;
      switch (op)
	{
	case '+':
	case '-':
	case '=':
	  break;
	default:
	  builtin_error (_("`%c': invalid symbolic mode operator"), op);
	  return (-1);
	}

      /* Parse out the `perm' section of the symbolic mode clause. */
      while (member (*s, "rwx"))
	{
	  c = *s++;

	  switch (c)
	    {
	    case 'r':
	      perm |= S_IRUGO;
	      break;
	    case 'w':
	      perm |= S_IWUGO;
	      break;
	    case 'x':
	      perm |= S_IXUGO;
	      break;
	    }
	}

      /* Now perform the operation or return an error for a
	 bad permission string. */
      if (!*s || *s == ',')
	{
	  if (who)
	    perm &= who;

	  switch (op)
	    {
	    case '+':
	      bits |= perm;
	      break;
	    case '-':
	      bits &= ~perm;
	      break;
	    case '=':
	      if (who == 0)
		who = S_IRWXU | S_IRWXG | S_IRWXO;
	      bits &= ~who;
	      bits |= perm;
	      break;

	    /* No other values are possible. */
	    }

	  if (*s == '\0')
	    break;
	  else
	    s++;	/* skip past ',' */
	}
      else
	{
	  builtin_error (_("`%c': invalid symbolic mode character"), *s);
	  return (-1);
	}
    }

  return (bits);
}

/* Set the umask from a symbolic mode string similar to that accepted
   by chmod.  If the -S argument is given, then print the umask in a
   symbolic form. */
static int
symbolic_umask (list)
     WORD_LIST *list;
{
  int um, bits;

  /* Get the initial umask.  Don't change it yet. */
  um = umask (022);
  umask (um);

  /* All work is done with the complement of the umask -- it's
     more intuitive and easier to deal with.  It is complemented
     again before being returned. */
  bits = parse_symbolic_mode (list->word->word, ~um & 0777);
  if (bits == -1)
    return (-1);

  um = ~bits & 0777;
  return (um);
}
