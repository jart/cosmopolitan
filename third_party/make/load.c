/* Loading dynamic objects for GNU Make.
Copyright (C) 2012-2023 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the License, or (at your option) any later
version.

GNU Make is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <https://www.gnu.org/licenses/>.  */

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
            DB (DB_BASIC, ("%s\n", err));
          else
            OS (error, flocp, "%s", err);
          return NULL;
        }

      DB (DB_VERBOSE, (_("Loaded shared object %s\n"), ldname));

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
load_file (const floc *flocp, struct file *file, int noerror)
{
  const char *ldname = file->name;
  size_t nmlen = strlen (ldname);
  char *new = alloca (nmlen + CSTRLEN (SYMBOL_EXTENSION) + 1);
  char *symname = NULL;
  const char *fp;
  int r;
  load_func_t symp;

  /* Break the input into an object file name and a symbol name.  If no symbol
     name was provided, compute one from the object file name.  */
  fp = strchr (ldname, '(');
  if (fp)
    {
      const char *ep;

      /* There's an open paren, so see if there's a close paren: if so use
         that as the symbol name.  We can't have whitespace: it would have
         been chopped up before this function is called.  */
      ep = strchr (fp+1, ')');
      if (ep && ep[1] == '\0')
        {
          size_t l = fp - ldname;

          ++fp;
          if (fp == ep)
            OS (fatal, flocp, _("Empty symbol name for load: %s"), ldname);

          /* Make a copy of the ldname part.  */
          memcpy (new, ldname, l);
          new[l] = '\0';
          ldname = new;
          nmlen = l;

          /* Make a copy of the symbol name part.  */
          symname = new + l + 1;
          memcpy (symname, fp, ep - fp);
          symname[ep - fp] = '\0';
        }
    }

  /* Make sure this name is in the string cache.  */
  ldname = file->name = strcache_add (ldname);

  /* If this object has been loaded, we're done: return -1 to ensure make does
     not rebuild again.  If a rebuild is allowed it was set up when this
     object was initially loaded.  */
  file = lookup_file (ldname);
  if (file && file->loaded)
    return -1;

  /* If we didn't find a symbol name yet, construct it from the ldname.  */
  if (! symname)
    {
      char *p = new;

      fp = strrchr (ldname, '/');
#ifdef HAVE_DOS_PATHS
      if (fp)
        {
          const char *fp2 = strchr (fp, '\\');

          if (fp2 > fp)
            fp = fp2;
        }
      else
        fp = strrchr (ldname, '\\');
      /* The (improbable) case of d:foo.  */
      if (fp && *fp && fp[1] == ':')
        fp++;
#endif
      if (!fp)
        fp = ldname;
      else
        ++fp;
      while (isalnum ((unsigned char) *fp) || *fp == '_')
        *(p++) = *(fp++);
      strcpy (p, SYMBOL_EXTENSION);
      symname = new;
    }

  DB (DB_VERBOSE, (_("Loading symbol %s from %s\n"), symname, ldname));

  /* Load it!  */
  symp = load_object (flocp, noerror, ldname, symname);
  if (! symp)
    return 0;

  /* Invoke the symbol.  */
  r = (*symp) (flocp);

  /* If the load didn't fail, add the file to the .LOADED variable.  */
  if (r)
    do_variable_definition(flocp, ".LOADED", ldname, o_file, f_append_value, 0);

  return r;
}

int
unload_file (const char *name)
{
  int rc = 0;
  struct load_list *d;

  for (d = loaded_syms; d != NULL; d = d->next)
    if (streq (d->name, name) && d->dlp)
      {
        DB (DB_VERBOSE, (_("Unloading shared object %s\n"), name));
        rc = dlclose (d->dlp);
        if (rc)
          perror_with_name ("dlclose: ", d->name);
        else
          d->dlp = NULL;
        break;
      }

  return rc;
}

#else

int
load_file (const floc *flocp, struct file *file UNUSED, int noerror)
{
  if (! noerror)
    O (fatal, flocp,
       _("The 'load' operation is not supported on this platform"));

  return 0;
}

int
unload_file (const char *name UNUSED)
{
  O (fatal, NILF, "INTERNAL: Cannot unload when load is not supported");
}

#endif  /* MAKE_LOAD */
