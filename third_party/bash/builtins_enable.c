/* enable.c, created from enable.def. */
#line 22 "./enable.def"

#line 50 "./enable.def"

#include "config.h"

#if defined (HAVE_UNISTD_H)
#  ifdef _MINIX
#    include <sys/types.h>
#  endif
#  include <unistd.h>
#endif

#include <stdio.h>
#include "bashansi.h"
#include "bashintl.h"

#include "shell.h"
#include "builtins.h"
#include "flags.h"
#include "common.h"
#include "bashgetopt.h"
#include "findcmd.h"

#if defined (PROGRAMMABLE_COMPLETION)
#  include "pcomplete.h"
#endif

#define ENABLED  1
#define DISABLED 2
#define SPECIAL  4
#define SILENT	 8	/* affects dyn_load_builtin behavior */

#define AFLAG	0x01
#define DFLAG	0x02
#define FFLAG	0x04
#define NFLAG	0x08
#define PFLAG	0x10
#define SFLAG	0x20

#if defined (HAVE_DLOPEN) && defined (HAVE_DLSYM)
static int dyn_load_builtin PARAMS((WORD_LIST *, int, char *));
#endif

#if defined (HAVE_DLCLOSE)
static int dyn_unload_builtin PARAMS((char *));
static void delete_builtin PARAMS((struct builtin *));
static int local_dlclose PARAMS((void *));
#endif

#define STRUCT_SUFFIX	"_struct"
/* for now */
#define LOAD_SUFFIX	"_builtin_load"
#define UNLOAD_SUFFIX	"_builtin_unload"

static void list_some_builtins PARAMS((int));
static int enable_shell_command PARAMS((char *, int));

/* Enable/disable shell commands present in LIST.  If list is not specified,
   then print out a list of shell commands showing which are enabled and
   which are disabled. */
int
enable_builtin (list)
     WORD_LIST *list;
{
  int result, flags;
  int opt, filter;
  WORD_LIST *next;
#if defined (HAVE_DLOPEN) && defined (HAVE_DLSYM)
  char *filename;
#endif

  result = EXECUTION_SUCCESS;
  flags = 0;

  reset_internal_getopt ();
  while ((opt = internal_getopt (list, "adnpsf:")) != -1)
    {
      switch (opt)
	{
	case 'a':
	  flags |= AFLAG;
	  break;
	case 'n':
	  flags |= NFLAG;
	  break;
	case 'p':
	  flags |= PFLAG;
	  break;
	case 's':
	  flags |= SFLAG;
	  break;
	case 'f':
#if defined (HAVE_DLOPEN) && defined (HAVE_DLSYM)
	  flags |= FFLAG;
	  filename = list_optarg;
	  break;
#else
	  builtin_error (_("dynamic loading not available"));
	  return (EX_USAGE);
#endif
#if defined (HAVE_DLCLOSE)
	case 'd':
	  flags |= DFLAG;
	  break;
#else
	  builtin_error (_("dynamic loading not available"));
	  return (EX_USAGE);
#endif /* HAVE_DLCLOSE */
	CASE_HELPOPT;
	default:
	  builtin_usage ();
	  return (EX_USAGE);
	}
    }

  list = loptend;

#if defined (RESTRICTED_SHELL)
  /* Restricted shells cannot load new builtins. */
  if (restricted && (flags & (FFLAG|DFLAG)))
    {
      sh_restricted ((char *)NULL);
      return (EXECUTION_FAILURE);
    }
#endif

  if (list == 0 || (flags & PFLAG))
    {
      filter = (flags & AFLAG) ? (ENABLED | DISABLED)
			       : (flags & NFLAG) ? DISABLED : ENABLED;

      if (flags & SFLAG)
	filter |= SPECIAL;

      list_some_builtins (filter);
      result = sh_chkwrite (EXECUTION_SUCCESS);
    }
#if defined (HAVE_DLOPEN) && defined (HAVE_DLSYM)
  else if (flags & FFLAG)
    {
      filter = (flags & NFLAG) ? DISABLED : ENABLED;
      if (flags & SFLAG)
	filter |= SPECIAL;

      result = dyn_load_builtin (list, filter, filename);
      if (result != EXECUTION_SUCCESS)
	result = EXECUTION_FAILURE;	/* normalize return value */
#if defined (PROGRAMMABLE_COMPLETION)
      set_itemlist_dirty (&it_builtins);
#endif
    }
#endif
#if defined (HAVE_DLCLOSE)
  else if (flags & DFLAG)
    {
      while (list)
	{
	  opt = dyn_unload_builtin (list->word->word);
	  if (opt == EXECUTION_FAILURE)
	    result = EXECUTION_FAILURE;
	  list = list->next;
	}
#if defined (PROGRAMMABLE_COMPLETION)
      set_itemlist_dirty (&it_builtins);
#endif
    }
#endif
  else
    {
      while (list)
	{
	  opt = enable_shell_command (list->word->word, flags & NFLAG);
	  next = list->next;

#if defined (HAVE_DLOPEN) && defined (HAVE_DLSYM)
	  /* If we try to enable a non-existent builtin, and we have dynamic
	     loading, try the equivalent of `enable -f name name'. */
	  if (opt == EX_NOTFOUND)
	    {
	      int dflags, r;
	      
	      dflags = ENABLED|SILENT|((flags & SFLAG) ? SPECIAL : 0);

	      list->next = 0;
	      r = dyn_load_builtin (list, dflags, list->word->word);
	      list->next = next;
	      if (r == EXECUTION_SUCCESS)
		opt = r;
#if defined (PROGRAMMABLE_COMPLETION)
	      set_itemlist_dirty (&it_builtins);
#endif
	    }
#endif

	  if (opt == EX_NOTFOUND)
	    {
	      sh_notbuiltin (list->word->word);
	      result = EXECUTION_FAILURE;
	    }
	  else if (opt != EXECUTION_SUCCESS)
	    result = EXECUTION_FAILURE;

	  list = next;
	}
    }
  return (result);
}

/* List some builtins.
   FILTER is a mask with two slots: ENABLED and DISABLED. */
static void
list_some_builtins (filter)
     int filter;
{
  register int i;

  for (i = 0; i < num_shell_builtins; i++)
    {
      if (shell_builtins[i].function == 0 || (shell_builtins[i].flags & BUILTIN_DELETED))
	continue;

      if ((filter & SPECIAL) &&
	  (shell_builtins[i].flags & SPECIAL_BUILTIN) == 0)
	continue;

      if ((filter & ENABLED) && (shell_builtins[i].flags & BUILTIN_ENABLED))
	printf ("enable %s\n", shell_builtins[i].name);
      else if ((filter & DISABLED) &&
	       ((shell_builtins[i].flags & BUILTIN_ENABLED) == 0))
	printf ("enable -n %s\n", shell_builtins[i].name);
    }
}

/* Enable the shell command NAME.  If DISABLE_P is non-zero, then
   disable NAME instead. */
static int
enable_shell_command (name, disable_p)
     char *name;
     int disable_p;
{
  struct builtin *b;

  b = builtin_address_internal (name, 1);
  if (b == 0)
    return (EX_NOTFOUND);

  if (disable_p)
    b->flags &= ~BUILTIN_ENABLED;
#if defined (RESTRICTED_SHELL)
  else if (restricted && ((b->flags & BUILTIN_ENABLED) == 0))
    {
      sh_restricted ((char *)NULL);
      return (EXECUTION_FAILURE);
    }
#endif
  else
    b->flags |= BUILTIN_ENABLED;

#if defined (PROGRAMMABLE_COMPLETION)
  set_itemlist_dirty (&it_enabled);
  set_itemlist_dirty (&it_disabled);
#endif

  return (EXECUTION_SUCCESS);
}

#if defined (HAVE_DLOPEN) && defined (HAVE_DLSYM)

#if defined (HAVE_DLFCN_H)
#  include <dlfcn.h>
#endif

static int
dyn_load_builtin (list, flags, filename)
     WORD_LIST *list;
     int flags;
     char *filename;
{
  WORD_LIST *l;
  void *handle;
  
  int total, size, new, replaced, r;
  char *struct_name, *name, *funcname;
  sh_load_func_t *loadfunc;
  struct builtin **new_builtins, *b, *new_shell_builtins, *old_builtin;
  char *loadables_path, *load_path;

  if (list == 0)
    return (EXECUTION_FAILURE);

#ifndef RTLD_LAZY
#define RTLD_LAZY 1
#endif

  handle = 0;
  if (absolute_program (filename) == 0)
    {
      loadables_path = get_string_value ("BASH_LOADABLES_PATH");
      if (loadables_path)
	{
	  load_path = find_in_path (filename, loadables_path, FS_NODIRS|FS_EXEC_PREFERRED);
	  if (load_path)
	    {
#if defined (_AIX)
	      handle = dlopen (load_path, RTLD_NOW|RTLD_GLOBAL);
#else
	      handle = dlopen (load_path, RTLD_LAZY);
#endif /* !_AIX */
	      free (load_path);
	    }
	}
    }

  /* Fall back to current directory for now */
  if (handle == 0)
#if defined (_AIX)
    handle = dlopen (filename, RTLD_NOW|RTLD_GLOBAL);
#else
    handle = dlopen (filename, RTLD_LAZY);
#endif /* !_AIX */

  if (handle == 0)
    {
      /* If we've been told to be quiet, don't complain about not finding the
	 specified shared object. */
      if ((flags & SILENT) == 0)
	{
	  name = printable_filename (filename, 0);
	  builtin_error (_("cannot open shared object %s: %s"), name, dlerror ());
	  if (name != filename)
	    free (name);
	}
      return (EX_NOTFOUND);
    }

  for (new = 0, l = list; l; l = l->next, new++)
    ;
  new_builtins = (struct builtin **)xmalloc (new * sizeof (struct builtin *));

  /* For each new builtin in the shared object, find it and its describing
     structure.  If this is overwriting an existing builtin, do so, otherwise
     save the loaded struct for creating the new list of builtins. */
  for (replaced = new = 0; list; list = list->next)
    {
      name = list->word->word;

      size = strlen (name);
      struct_name = (char *)xmalloc (size + 8);
      strcpy (struct_name, name);
      strcpy (struct_name + size, STRUCT_SUFFIX);

      old_builtin = builtin_address_internal (name, 1);

      b = (struct builtin *)dlsym (handle, struct_name);
      if (b == 0)
	{
	  name = printable_filename (filename, 0);
	  builtin_error (_("cannot find %s in shared object %s: %s"),
			    struct_name, name, dlerror ());
	  if (name != filename)
	    free (name);
	  free (struct_name);
	  continue;
	}

      funcname = xrealloc (struct_name, size + sizeof (LOAD_SUFFIX) + 1);
      strcpy (funcname, name);
      strcpy (funcname + size, LOAD_SUFFIX);

      loadfunc = (sh_load_func_t *)dlsym (handle, funcname);
      if (loadfunc)
	{
	  /* Add warning if running an init function more than once */
	  if (old_builtin && (old_builtin->flags & STATIC_BUILTIN) == 0)
	    builtin_warning (_("%s: dynamic builtin already loaded"), name);
	  r = (*loadfunc) (name);
	  if (r == 0)
	    {
	      builtin_error (_("load function for %s returns failure (%d): not loaded"), name, r);
	      free (funcname);
	      continue;
	    }
	}
      free (funcname);

      b->flags &= ~STATIC_BUILTIN;
      if (flags & SPECIAL)
	b->flags |= SPECIAL_BUILTIN;
      b->handle = handle;

      if (old_builtin)
	{
	  replaced++;
	  FASTCOPY ((char *)b, (char *)old_builtin, sizeof (struct builtin));
	}
      else
	  new_builtins[new++] = b;
    }

  if (replaced == 0 && new == 0)
    {
      free (new_builtins);
      dlclose (handle);
      return (EXECUTION_FAILURE);
    }

  if (new)
    {
      total = num_shell_builtins + new;
      size = (total + 1) * sizeof (struct builtin);

      new_shell_builtins = (struct builtin *)xmalloc (size);
      FASTCOPY ((char *)shell_builtins, (char *)new_shell_builtins,
		num_shell_builtins * sizeof (struct builtin));
      for (replaced = 0; replaced < new; replaced++)
	FASTCOPY ((char *)new_builtins[replaced],
		  (char *)&new_shell_builtins[num_shell_builtins + replaced],
		  sizeof (struct builtin));

      new_shell_builtins[total].name = (char *)0;
      new_shell_builtins[total].function = (sh_builtin_func_t *)0;
      new_shell_builtins[total].flags = 0;

      if (shell_builtins != static_shell_builtins)
	free (shell_builtins);

      shell_builtins = new_shell_builtins;
      num_shell_builtins = total;
      initialize_shell_builtins ();
    }

  free (new_builtins);
  return (EXECUTION_SUCCESS);
}
#endif

#if defined (HAVE_DLCLOSE)
static void
delete_builtin (b)
     struct builtin *b;
{
  int ind, size;
  struct builtin *new_shell_builtins;

  /* XXX - funky pointer arithmetic - XXX */
#ifdef __STDC__
  ind = b - shell_builtins;
#else
  ind = ((int)b - (int)shell_builtins) / sizeof (struct builtin);
#endif
  size = num_shell_builtins * sizeof (struct builtin);
  new_shell_builtins = (struct builtin *)xmalloc (size);

  /* Copy shell_builtins[0]...shell_builtins[ind - 1] to new_shell_builtins */
  if (ind)
    FASTCOPY ((char *)shell_builtins, (char *)new_shell_builtins,
	      ind * sizeof (struct builtin));
  /* Copy shell_builtins[ind+1]...shell_builtins[num_shell_builtins to
     new_shell_builtins, starting at ind. */
  FASTCOPY ((char *)(&shell_builtins[ind+1]),
  	    (char *)(&new_shell_builtins[ind]),
  	    (num_shell_builtins - ind) * sizeof (struct builtin));

  if (shell_builtins != static_shell_builtins)
    free (shell_builtins);

  /* The result is still sorted. */
  num_shell_builtins--;
  shell_builtins = new_shell_builtins;
}

/* Tenon's MachTen has a dlclose that doesn't return a value, so we
   finesse it with a local wrapper. */
static int
local_dlclose (handle)
     void *handle;
{
#if !defined (__MACHTEN__)
  return (dlclose (handle));
#else /* __MACHTEN__ */
  dlclose (handle);
  return ((dlerror () != NULL) ? -1 : 0);    
#endif /* __MACHTEN__ */
}

static int
dyn_unload_builtin (name)
     char *name;
{
  struct builtin *b;
  void *handle;
  char *funcname;
  sh_unload_func_t *unloadfunc;
  int ref, i, size;

  b = builtin_address_internal (name, 1);
  if (b == 0)
    {
      sh_notbuiltin (name);
      return (EXECUTION_FAILURE);
    }
  if (b->flags & STATIC_BUILTIN)
    {
      builtin_error (_("%s: not dynamically loaded"), name);
      return (EXECUTION_FAILURE);
    }

  handle = (void *)b->handle;
  for (ref = i = 0; i < num_shell_builtins; i++)
    {
      if (shell_builtins[i].handle == b->handle)
	ref++;
    }

  /* Call any unload function */
  size = strlen (name);
  funcname = xmalloc (size + sizeof (UNLOAD_SUFFIX) + 1);
  strcpy (funcname, name);
  strcpy (funcname + size, UNLOAD_SUFFIX);

  unloadfunc = (sh_unload_func_t *)dlsym (handle, funcname);
  if (unloadfunc)
    (*unloadfunc) (name);	/* void function */
  free (funcname);

  /* Don't remove the shared object unless the reference count of builtins
     using it drops to zero. */
  if (ref == 1 && local_dlclose (handle) != 0)
    {
      builtin_error (_("%s: cannot delete: %s"), name, dlerror ());
      return (EXECUTION_FAILURE);
    }

  /* Now remove this entry from the builtin table and reinitialize. */
  delete_builtin (b);

  return (EXECUTION_SUCCESS);
}
#endif
