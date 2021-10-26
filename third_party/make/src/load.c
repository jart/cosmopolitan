/* Loading dynamic objects for GNU Make.
Copyright (C) 2012-2020 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the License, or (at your option) any later
version.

GNU Make is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "makeint.h"

#if MAKE_LOAD

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <errno.h>

#define SYMBOL_EXTENSION        "_gmk_setup"

#include "debug.h"
#include "filedef.h"
#include "variable.h"

/* Tru64 V4.0 does not have this flag */
#ifndef RTLD_GLOBAL
# define RTLD_GLOBAL 0
#endif

struct load_list
  {
    struct load_list *next;
    const char *name;
    void *dlp;
  };

static struct load_list *loaded_syms = NULL;

static load_func_t
load_object (const floc *flocp, int noerror, const char *ldname,
             const char *symname)
{
  static void *global_dl = NULL;
  load_func_t symp;

  if (! global_dl)
    {
      global_dl = dlopen (NULL, RTLD_NOW|RTLD_GLOBAL);
      if (! global_dl)
        {
          const char *err = dlerror ();
          OS (fatal, flocp, _("Failed to open global symbol table: %s"), err);
        }
    }

  symp = (load_func_t) dlsym (global_dl, symname);
  if (! symp)
    {
      struct load_list *new;
      void *dlp = NULL;

    /* If the path has no "/", try the current directory first.  */
      if (! strchr (ldname, '/')
#ifdef HAVE_DOS_PATHS
          && ! strchr (ldname, '\\')
#endif
         )
        dlp = dlopen (concat (2, "./", ldname), RTLD_LAZY|RTLD_GLOBAL);

      /* If we haven't opened it yet, try the default search path.  */
      if (! dlp)
        dlp = dlopen (ldname, RTLD_LAZY|RTLD_GLOBAL);

      /* Still no?  Then fail.  */
      if (! dlp)
        {
          const char *err = dlerror ();
          if (noerror)
            DB (DB_BASIC, ("%s", err));
          else
            OS (error, flocp, "%s", err);
          return NULL;
        }

      /* Assert that the GPL license symbol is defined.  */
      symp = (load_func_t) dlsym (dlp, "plugin_is_GPL_compatible");
      if (! symp)
        OS (fatal, flocp,
             _("Loaded object %s is not declared to be GPL compatible"),
             ldname);

      symp = (load_func_t) dlsym (dlp, symname);
      if (! symp)
        {
          const char *err = dlerror ();
          OSSS (fatal, flocp, _("Failed to load symbol %s from %s: %s"),
                symname, ldname, err);
        }

      /* Add this symbol to a trivial lookup table.  This is not efficient but
         it's highly unlikely we'll be loading lots of objects, and we only
         need it to look them up on unload, if we rebuild them.  */
      new = xmalloc (sizeof (struct load_list));
      new->name = xstrdup (ldname);
      new->dlp = dlp;
      new->next = loaded_syms;
      loaded_syms = new;
    }

  return symp;
}

int
load_file (const floc *flocp, const char **ldname, int noerror)
{
  size_t nmlen = strlen (*ldname);
  char *new = alloca (nmlen + CSTRLEN (SYMBOL_EXTENSION) + 1);
  char *symname = NULL;
  char *loaded;
  const char *fp;
  int r;
  load_func_t symp;

  /* Break the input into an object file name and a symbol name.  If no symbol
     name was provided, compute one from the object file name.  */
  fp = strchr (*ldname, '(');
  if (fp)
    {
      const char *ep;

      /* There's an open paren, so see if there's a close paren: if so use
         that as the symbol name.  We can't have whitespace: it would have
         been chopped up before this function is called.  */
      ep = strchr (fp+1, ')');
      if (ep && ep[1] == '\0')
        {
          size_t l = fp - *ldname;;

          ++fp;
          if (fp == ep)
            OS (fatal, flocp, _("Empty symbol name for load: %s"), *ldname);

          /* Make a copy of the ldname part.  */
          memcpy (new, *ldname, l);
          new[l] = '\0';
          *ldname = new;
          nmlen = l;

          /* Make a copy of the symbol name part.  */
          symname = new + l + 1;
          memcpy (symname, fp, ep - fp);
          symname[ep - fp] = '\0';
        }
    }

  /* Add this name to the string cache so it can be reused later.  */
  *ldname = strcache_add (*ldname);

  /* If this object has been loaded, we're done.  */
  loaded = allocated_variable_expand ("$(.LOADED)");
  fp = strstr (loaded, *ldname);
  r = fp && (fp==loaded || fp[-1]==' ') && (fp[nmlen]=='\0' || fp[nmlen]==' ');
  if (r)
    goto exit;

  /* If we didn't find a symbol name yet, construct it from the ldname.  */
  if (! symname)
    {
      char *p = new;

      fp = strrchr (*ldname, '/');
#ifdef HAVE_DOS_PATHS
      if (fp)
        {
          const char *fp2 = strchr (fp, '\\');

          if (fp2 > fp)
            fp = fp2;
        }
      else
        fp = strrchr (*ldname, '\\');
      /* The (improbable) case of d:foo.  */
      if (fp && *fp && fp[1] == ':')
        fp++;
#endif
      if (!fp)
        fp = *ldname;
      else
        ++fp;
      while (isalnum (*fp) || *fp == '_')
        *(p++) = *(fp++);
      strcpy (p, SYMBOL_EXTENSION);
      symname = new;
    }

  DB (DB_VERBOSE, (_("Loading symbol %s from %s\n"), symname, *ldname));

  /* Load it!  */
  symp = load_object (flocp, noerror, *ldname, symname);
  if (! symp)
    return 0;

  /* Invoke the symbol.  */
  r = (*symp) (flocp);

  /* If it succeeded, add the load file to the loaded variable.  */
  if (r > 0)
    {
      size_t loadlen = strlen (loaded);
      char *newval = alloca (loadlen + strlen (*ldname) + 2);
      /* Don't add a space if it's empty.  */
      if (loadlen)
        {
          memcpy (newval, loaded, loadlen);
          newval[loadlen++] = ' ';
        }
      strcpy (&newval[loadlen], *ldname);
      do_variable_definition (flocp, ".LOADED", newval, o_default, f_simple, 0);
    }

 exit:
  free (loaded);
  return r;
}

void
unload_file (const char *name)
{
  struct load_list *d;

  for (d = loaded_syms; d != NULL; d = d->next)
    if (streq (d->name, name) && d->dlp)
      {
        if (dlclose (d->dlp))
          perror_with_name ("dlclose: ", d->name);
        d->dlp = NULL;
        break;
      }
}

#else

int
load_file (const floc *flocp, const char **ldname UNUSED, int noerror)
{
  if (! noerror)
    O (fatal, flocp,
       _("The 'load' operation is not supported on this platform."));

  return 0;
}

void
unload_file (const char *name UNUSED)
{
  O (fatal, NILF, "INTERNAL: Cannot unload when load is not supported!");
}

#endif  /* MAKE_LOAD */
