/* bind.c, created from bind.def. */
#line 22 "./bind.def"

#include "config.h"

#line 63 "./bind.def"

#if defined (READLINE)

#if defined (HAVE_UNISTD_H)
#  ifdef _MINIX
#    include <sys/types.h>
#  endif
#  include <unistd.h>
#endif

#include <stdio.h>
#include <errno.h>
#if !defined (errno)
extern int errno;
#endif /* !errno */

#include "third_party/readline/readline.h"
#include "third_party/readline/history.h"

#include "bashintl.h"

#include "shell.h"
#include "bashline.h"
#include "bashgetopt.h"
#include "common.h"

static int query_bindings PARAMS((char *));
static int unbind_command PARAMS((char *));
static int unbind_keyseq PARAMS((char *));

#define BIND_RETURN(x)  do { return_code = x; goto bind_exit; } while (0)

#define LFLAG	0x0001
#define PFLAG	0x0002
#define FFLAG	0x0004
#define VFLAG	0x0008
#define QFLAG	0x0010
#define MFLAG	0x0020
#define RFLAG	0x0040
#define PPFLAG	0x0080
#define VVFLAG	0x0100
#define SFLAG   0x0200
#define SSFLAG  0x0400
#define UFLAG	0x0800
#define XFLAG	0x1000
#define XXFLAG	0x2000

int
bind_builtin (list)
     WORD_LIST *list;
{
  int return_code;
  Keymap kmap, saved_keymap;
  int flags, opt;
  char *initfile, *map_name, *fun_name, *unbind_name, *remove_seq, *cmd_seq, *t;

  if (no_line_editing)
    {
#if 0
      builtin_error (_("line editing not enabled"));
      return (EXECUTION_FAILURE);
#else
      builtin_warning (_("line editing not enabled"));
#endif
    }

  kmap = saved_keymap = (Keymap) NULL;
  flags = 0;
  initfile = map_name = fun_name = unbind_name = remove_seq = cmd_seq = (char *)NULL;
  return_code = EXECUTION_SUCCESS;

  if (bash_readline_initialized == 0)
    initialize_readline ();

  begin_unwind_frame ("bind_builtin");
  unwind_protect_var (rl_outstream);

  rl_outstream = stdout;

  reset_internal_getopt ();  
  while ((opt = internal_getopt (list, "lvpVPsSXf:q:u:m:r:x:")) != -1)
    {
      switch (opt)
	{
	case 'l':
	  flags |= LFLAG;
	  break;
	case 'v':
	  flags |= VFLAG;
	  break;
	case 'p':
	  flags |= PFLAG;
	  break;
	case 'f':
	  flags |= FFLAG;
	  initfile = list_optarg;
	  break;
	case 'm':
	  flags |= MFLAG;
	  map_name = list_optarg;
	  break;
	case 'q':
	  flags |= QFLAG;
	  fun_name = list_optarg;
	  break;
	case 'u':
	  flags |= UFLAG;
	  unbind_name = list_optarg;
	  break;
	case 'r':
	  flags |= RFLAG;
	  remove_seq = list_optarg;
	  break;
	case 'V':
	  flags |= VVFLAG;
	  break;
	case 'P':
	  flags |= PPFLAG;
	  break;
	case 's':
	  flags |= SFLAG;
	  break;
	case 'S':
	  flags |= SSFLAG;
	  break;
	case 'x':
	  flags |= XFLAG;
	  cmd_seq = list_optarg;
	  break;
	case 'X':
	  flags |= XXFLAG;
	  break;
	case GETOPT_HELP:
	default:
	  builtin_usage ();
	  BIND_RETURN (EX_USAGE);
	}
    }

  list = loptend;

  /* First, see if we need to install a special keymap for this
     command.  Then start on the arguments. */

  if ((flags & MFLAG) && map_name)
    {
      kmap = rl_get_keymap_by_name (map_name);
      if (kmap == 0)
	{
	  builtin_error (_("`%s': invalid keymap name"), map_name);
	  BIND_RETURN (EXECUTION_FAILURE);
	}
    }

  if (kmap)
    {
      saved_keymap = rl_get_keymap ();
      rl_set_keymap (kmap);
    }

  /* XXX - we need to add exclusive use tests here.  It doesn't make sense
     to use some of these options together. */
  /* Now hack the option arguments */
  if (flags & LFLAG)
    rl_list_funmap_names ();

  if (flags & PFLAG)
    rl_function_dumper (1);

  if (flags & PPFLAG)
    rl_function_dumper (0);

  if (flags & SFLAG)
    rl_macro_dumper (1);

  if (flags & SSFLAG)
    rl_macro_dumper (0);

  if (flags & VFLAG)
    rl_variable_dumper (1);

  if (flags & VVFLAG)
    rl_variable_dumper (0);

  if ((flags & FFLAG) && initfile)
    {
      if (rl_read_init_file (initfile) != 0)
	{
	  t = printable_filename (initfile, 0);
	  builtin_error (_("%s: cannot read: %s"), t, strerror (errno));
	  if (t != initfile)
	    free (t);
	  BIND_RETURN (EXECUTION_FAILURE);
	}
    }

  if ((flags & QFLAG) && fun_name)
    return_code = query_bindings (fun_name);

  if ((flags & UFLAG) && unbind_name)
    return_code = unbind_command (unbind_name);

  if ((flags & RFLAG) && remove_seq)
    {
      opt = unbind_keyseq (remove_seq);
      BIND_RETURN (opt);
    }

  if (flags & XFLAG)
    return_code = bind_keyseq_to_unix_command (cmd_seq);

  if (flags & XXFLAG)
    return_code = print_unix_command_map ();

  /* Process the rest of the arguments as binding specifications. */
  while (list)
    {
      int olen, nlen, d, i;
      char **obindings, **nbindings;

      obindings = rl_invoking_keyseqs (bash_execute_unix_command);
      olen = obindings ? strvec_len (obindings) : 0;

      rl_parse_and_bind (list->word->word);

      nbindings = rl_invoking_keyseqs (bash_execute_unix_command);
      nlen = nbindings ? strvec_len (nbindings) : 0;

      if (nlen < olen)	/* fewer bind -x bindings */
	for (d = olen - nlen, i = 0; i < olen && d > 0; i++)
	  if (nlen == 0 || strvec_search (nbindings, obindings[i]) < 0)
	    {
	      unbind_unix_command (obindings[i]);
	      d--;
	    }

      strvec_dispose (obindings);
      strvec_dispose (nbindings);

      list = list->next;
    }

 bind_exit:
  if (saved_keymap)
    rl_set_keymap (saved_keymap);

  run_unwind_frame ("bind_builtin");

  if (return_code < 0)
    return_code = EXECUTION_FAILURE;

  return (sh_chkwrite (return_code));
}

static int
query_bindings (name)
     char *name;
{
  rl_command_func_t *function;
  char **keyseqs;
  int j;

  function = rl_named_function (name);
  if (function == 0)
    {
      builtin_error (_("`%s': unknown function name"), name);
      return EXECUTION_FAILURE;
    }

  keyseqs = rl_invoking_keyseqs (function);

  if (!keyseqs)
    {
      printf (_("%s is not bound to any keys.\n"), name);
      return EXECUTION_FAILURE;
    }

  printf (_("%s can be invoked via "), name);
  for (j = 0; j < 5 && keyseqs[j]; j++)
    printf ("\"%s\"%s", keyseqs[j], keyseqs[j + 1] ? ", " : ".\n");
  if (keyseqs[j])
    printf ("...\n");
  strvec_dispose (keyseqs);
  return EXECUTION_SUCCESS;
}

static int
unbind_command (name)
     char *name;
{
  rl_command_func_t *function;

  function = rl_named_function (name);
  if (function == 0)
    {
      builtin_error (_("`%s': unknown function name"), name);
      return EXECUTION_FAILURE;
    }

  rl_unbind_function_in_map (function, rl_get_keymap ());
  return EXECUTION_SUCCESS;
}

static int
unbind_keyseq (seq)
     char *seq;
{
  char *kseq;
  int kslen, type;
  rl_command_func_t *f;

  kseq = (char *)xmalloc ((2 * strlen (seq)) + 1);
  if (rl_translate_keyseq (seq, kseq, &kslen))
    {
      free (kseq);
      builtin_error (_("`%s': cannot unbind"), seq);
      return EXECUTION_FAILURE;
    }
  if ((f = rl_function_of_keyseq_len (kseq, kslen, (Keymap)0, &type)) == 0)
    {
      free (kseq);
      return (EXECUTION_SUCCESS);
    }
  if (type == ISKMAP)
    f = ((Keymap) f)[ANYOTHERKEY].function;

  /* I wish this didn't have to translate the key sequence again, but readline
     doesn't have a binding function that takes a translated key sequence as
     an argument. */
  if (rl_bind_keyseq (seq, (rl_command_func_t *)NULL) != 0)
    {
      free (kseq);
      builtin_error (_("`%s': cannot unbind"), seq);
      return (EXECUTION_FAILURE);
    }

  if (f == bash_execute_unix_command)
    unbind_unix_command (seq);

  free (kseq);
  return (EXECUTION_SUCCESS);
}
#endif /* READLINE */
