/* Implementation of pattern-matching file search paths for GNU Make.
Copyright (C) 1988-2020 Free Software Foundation, Inc.
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
#include "filedef.h"
#include "variable.h"
#ifdef WINDOWS32
#include "pathstuff.h"
#endif


/* Structure used to represent a selective VPATH searchpath.  */

struct vpath
  {
    struct vpath *next;      /* Pointer to next struct in the linked list.  */
    const char *pattern;     /* The pattern to match.  */
    const char *percent;     /* Pointer into 'pattern' where the '%' is.  */
    size_t patlen;           /* Length of the pattern.  */
    const char **searchpath; /* Null-terminated list of directories.  */
    size_t maxlen;           /* Maximum length of any entry in the list.  */
  };

/* Linked-list of all selective VPATHs.  */

static struct vpath *vpaths;

/* Structure for the general VPATH given in the variable.  */

static struct vpath *general_vpath;

/* Structure for GPATH given in the variable.  */

static struct vpath *gpaths;


/* Reverse the chain of selective VPATH lists so they will be searched in the
   order given in the makefiles and construct the list from the VPATH
   variable.  */

void
build_vpath_lists (void)
{
  struct vpath *new = 0;
  struct vpath *old, *nexto;
  char *p;

  /* Reverse the chain.  */
  for (old = vpaths; old != 0; old = nexto)
    {
      nexto = old->next;
      old->next = new;
      new = old;
    }

  vpaths = new;

  /* If there is a VPATH variable with a nonnull value, construct the
     general VPATH list from it.  We use variable_expand rather than just
     calling lookup_variable so that it will be recursively expanded.  */

  {
    /* Turn off --warn-undefined-variables while we expand SHELL and IFS.  */
    int save = warn_undefined_variables_flag;
    warn_undefined_variables_flag = 0;

    p = variable_expand ("$(strip $(VPATH))");

    warn_undefined_variables_flag = save;
  }

  if (*p != '\0')
    {
      /* Save the list of vpaths.  */
      struct vpath *save_vpaths = vpaths;
      char gp[] = "%";

      /* Empty 'vpaths' so the new one will have no next, and 'vpaths'
         will still be nil if P contains no existing directories.  */
      vpaths = 0;

      /* Parse P.  */
      construct_vpath_list (gp, p);

      /* Store the created path as the general path,
         and restore the old list of vpaths.  */
      general_vpath = vpaths;
      vpaths = save_vpaths;
    }

  /* If there is a GPATH variable with a nonnull value, construct the
     GPATH list from it.  We use variable_expand rather than just
     calling lookup_variable so that it will be recursively expanded.  */

  {
    /* Turn off --warn-undefined-variables while we expand SHELL and IFS.  */
    int save = warn_undefined_variables_flag;
    warn_undefined_variables_flag = 0;

    p = variable_expand ("$(strip $(GPATH))");

    warn_undefined_variables_flag = save;
  }

  if (*p != '\0')
    {
      /* Save the list of vpaths.  */
      struct vpath *save_vpaths = vpaths;
      char gp[] = "%";

      /* Empty 'vpaths' so the new one will have no next, and 'vpaths'
         will still be nil if P contains no existing directories.  */
      vpaths = 0;

      /* Parse P.  */
      construct_vpath_list (gp, p);

      /* Store the created path as the GPATH,
         and restore the old list of vpaths.  */
      gpaths = vpaths;
      vpaths = save_vpaths;
    }
}

/* Construct the VPATH listing for the PATTERN and DIRPATH given.

   This function is called to generate selective VPATH lists and also for
   the general VPATH list (which is in fact just a selective VPATH that
   is applied to everything).  The returned pointer is either put in the
   linked list of all selective VPATH lists or in the GENERAL_VPATH
   variable.

   If DIRPATH is nil, remove all previous listings with the same
   pattern.  If PATTERN is nil, remove all VPATH listings.  Existing
   and readable directories that are not "." given in the DIRPATH
   separated by the path element separator (defined in makeint.h) are
   loaded into the directory hash table if they are not there already
   and put in the VPATH searchpath for the given pattern with trailing
   slashes stripped off if present (and if the directory is not the
   root, "/").  The length of the longest entry in the list is put in
   the structure as well.  The new entry will be at the head of the
   VPATHS chain.  */

void
construct_vpath_list (char *pattern, char *dirpath)
{
  unsigned int elem;
  char *p;
  const char **vpath;
  size_t maxvpath;
  unsigned int maxelem;
  const char *percent = NULL;

  if (pattern != 0)
    percent = find_percent (pattern);

  if (dirpath == 0)
    {
      /* Remove matching listings.  */
      struct vpath *path, *lastpath;

      lastpath = 0;
      path = vpaths;
      while (path != 0)
        {
          struct vpath *next = path->next;

          if (pattern == 0
              || (((percent == 0 && path->percent == 0)
                   || (percent - pattern == path->percent - path->pattern))
                  && streq (pattern, path->pattern)))
            {
              /* Remove it from the linked list.  */
              if (lastpath == 0)
                vpaths = path->next;
              else
                lastpath->next = next;

              /* Free its unused storage.  */
              /* MSVC erroneously warns without a cast here.  */
              free ((void *)path->searchpath);
              free (path);
            }
          else
            lastpath = path;

          path = next;
        }

      return;
    }

#ifdef WINDOWS32
    convert_vpath_to_windows32 (dirpath, ';');
#endif

  /* Skip over any initial separators and blanks.  */
  while (STOP_SET (*dirpath, MAP_BLANK|MAP_PATHSEP))
    ++dirpath;

  /* Figure out the maximum number of VPATH entries and put it in
     MAXELEM.  We start with 2, one before the first separator and one
     nil (the list terminator) and increment our estimated number for
     each separator or blank we find.  */
  maxelem = 2;
  p = dirpath;
  while (*p != '\0')
    if (STOP_SET (*p++, MAP_BLANK|MAP_PATHSEP))
      ++maxelem;

  vpath = xmalloc (maxelem * sizeof (const char *));
  maxvpath = 0;

  elem = 0;
  p = dirpath;
  while (*p != '\0')
    {
      char *v;
      size_t len;

      /* Find the end of this entry.  */
      v = p;
      while (*p != '\0'
#if defined(HAVE_DOS_PATHS) && (PATH_SEPARATOR_CHAR == ':')
             /* Platforms whose PATH_SEPARATOR_CHAR is ':' and which
                also define HAVE_DOS_PATHS would like us to recognize
                colons after the drive letter in the likes of
                "D:/foo/bar:C:/xyzzy".  */
             && (*p != PATH_SEPARATOR_CHAR
                 || (p == v + 1 && (p[1] == '/' || p[1] == '\\')))
#else
             && *p != PATH_SEPARATOR_CHAR
#endif
             && !ISBLANK (*p))
        ++p;

      len = p - v;
      /* Make sure there's no trailing slash,
         but still allow "/" as a directory.  */
#if defined(__MSDOS__) || defined(__EMX__) || defined(HAVE_DOS_PATHS)
      /* We need also to leave alone a trailing slash in "d:/".  */
      if (len > 3 || (len > 1 && v[1] != ':'))
#endif
      if (len > 1 && p[-1] == '/')
        --len;

      /* Put the directory on the vpath list.  */
      if (len > 1 || *v != '.')
        {
          vpath[elem++] = dir_name (strcache_add_len (v, len));
          if (len > maxvpath)
            maxvpath = len;
        }

      /* Skip over separators and blanks between entries.  */
      while (STOP_SET (*p, MAP_BLANK|MAP_PATHSEP))
        ++p;
    }

  if (elem > 0)
    {
      struct vpath *path;
      /* ELEM is now incremented one element past the last
         entry, to where the nil-pointer terminator goes.
         Usually this is maxelem - 1.  If not, shrink down.  */
      if (elem < (maxelem - 1))
        vpath = xrealloc (vpath, (elem+1) * sizeof (const char *));

      /* Put the nil-pointer terminator on the end of the VPATH list.  */
      vpath[elem] = NULL;

      /* Construct the vpath structure and put it into the linked list.  */
      path = xmalloc (sizeof (struct vpath));
      path->searchpath = vpath;
      path->maxlen = maxvpath;
      path->next = vpaths;
      vpaths = path;

      /* Set up the members.  */
      path->pattern = strcache_add (pattern);
      path->patlen = strlen (pattern);
      path->percent = percent ? path->pattern + (percent - pattern) : 0;
    }
  else
    /* There were no entries, so free whatever space we allocated.  */
    /* MSVC erroneously warns without a cast here.  */
    free ((void *)vpath);
}

/* Search the GPATH list for a pathname string that matches the one passed
   in.  If it is found, return 1.  Otherwise we return 0.  */

int
gpath_search (const char *file, size_t len)
{
  if (gpaths && (len <= gpaths->maxlen))
    {
      const char **gp;
      for (gp = gpaths->searchpath; *gp != NULL; ++gp)
        if (strneq (*gp, file, len) && (*gp)[len] == '\0')
          return 1;
    }

  return 0;
}


/* Search the given VPATH list for a directory where the name pointed to by
   FILE exists.  If it is found, we return a cached name of the existing file
   and set *MTIME_PTR (if MTIME_PTR is not NULL) to its modtime (or zero if no
   stat call was done). Also set the matching directory index in PATH_INDEX
   if it is not NULL. Otherwise we return NULL.  */

static const char *
selective_vpath_search (struct vpath *path, const char *file,
                        FILE_TIMESTAMP *mtime_ptr, unsigned int* path_index)
{
  int not_target;
  char *name;
  const char *n;
  const char *filename;
  const char **vpath = path->searchpath;
  size_t maxvpath = path->maxlen;
  unsigned int i;
  size_t flen, name_dplen;
  int exists = 0;

  /* Find out if *FILE is a target.
     If and only if it is NOT a target, we will accept prospective
     files that don't exist but are mentioned in a makefile.  */
  {
    struct file *f = lookup_file (file);
    not_target = f == 0 || !f->is_target;
  }

  flen = strlen (file);

  /* Split *FILE into a directory prefix and a name-within-directory.
     NAME_DPLEN gets the length of the prefix; FILENAME gets the pointer to
     the name-within-directory and FLEN is its length.  */

  n = strrchr (file, '/');
#ifdef HAVE_DOS_PATHS
  /* We need the rightmost slash or backslash.  */
  {
    const char *bslash = strrchr (file, '\\');
    if (!n || bslash > n)
      n = bslash;
  }
#endif
  name_dplen = n != 0 ? n - file : 0;
  filename = name_dplen > 0 ? n + 1 : file;
  if (name_dplen > 0)
    flen -= name_dplen + 1;

  /* Get enough space for the biggest VPATH entry, a slash, the directory
     prefix that came with FILE, another slash (although this one may not
     always be necessary), the filename, and a null terminator.  */
  name = alloca (maxvpath + 1 + name_dplen + 1 + flen + 1);

  /* Try each VPATH entry.  */
  for (i = 0; vpath[i] != 0; ++i)
    {
      int exists_in_cache = 0;
      char *p = name;
      size_t vlen = strlen (vpath[i]);

      /* Put the next VPATH entry into NAME at P and increment P past it.  */
      memcpy (p, vpath[i], vlen);
      p += vlen;

      /* Add the directory prefix already in *FILE.  */
      if (name_dplen > 0)
        {
#ifndef VMS
          *p++ = '/';
#else
          /* VMS: if this is not in VMS format, treat as Unix format */
          if ((*p != ':') && (*p != ']') && (*p != '>'))
            *p++ = '/';
#endif
          memcpy (p, file, name_dplen);
          p += name_dplen;
        }

#ifdef HAVE_DOS_PATHS
      /* Cause the next if to treat backslash and slash alike.  */
      if (p != name && p[-1] == '\\' )
        p[-1] = '/';
#endif
      /* Now add the name-within-directory at the end of NAME.  */
#ifndef VMS
      if (p != name && p[-1] != '/')
        {
          *p = '/';
          memcpy (p + 1, filename, flen + 1);
        }
      else
#else
      /* VMS use a slash if no directory terminator present */
      if (p != name && p[-1] != '/' && p[-1] != ':' &&
          p[-1] != '>' && p[-1] != ']')
        {
          *p = '/';
          memcpy (p + 1, filename, flen + 1);
        }
      else
#endif
        memcpy (p, filename, flen + 1);

      /* Check if the file is mentioned in a makefile.  If *FILE is not
         a target, that is enough for us to decide this file exists.
         If *FILE is a target, then the file must be mentioned in the
         makefile also as a target to be chosen.

         The restriction that *FILE must not be a target for a
         makefile-mentioned file to be chosen was added by an
         inadequately commented change in July 1990; I am not sure off
         hand what problem it fixes.

         In December 1993 I loosened this restriction to allow a file
         to be chosen if it is mentioned as a target in a makefile.  This
         seem logical.

         Special handling for -W / -o: make sure we preserve the special
         values here.  Actually this whole thing is a little bogus: I think
         we should ditch the name/hname thing and look into the renamed
         capability that already exists for files: that is, have a new struct
         file* entry for the VPATH-found file, and set the renamed field if
         we use it.
      */
      {
        struct file *f = lookup_file (name);
        if (f != 0)
          {
            exists = not_target || f->is_target;
            if (exists && mtime_ptr
                && (f->last_mtime == OLD_MTIME || f->last_mtime == NEW_MTIME))
              {
                *mtime_ptr = f->last_mtime;
                mtime_ptr = 0;
              }
          }
      }

      if (!exists)
        {
          /* That file wasn't mentioned in the makefile.
             See if it actually exists.  */

#ifdef VMS
          /* For VMS syntax just use the original vpath */
          if (*p != '/')
            exists_in_cache = exists = dir_file_exists_p (vpath[i], filename);
          else
#endif
            {
              /* Clobber a null into the name at the last slash.
                 Now NAME is the name of the directory to look in.  */
              *p = '\0';
              /* We know the directory is in the hash table now because either
                 construct_vpath_list or the code just above put it there.
                 Does the file we seek exist in it?  */
              exists_in_cache = exists = dir_file_exists_p (name, filename);
            }
        }

      if (exists)
        {
          /* The file is in the directory cache.
             Now check that it actually exists in the filesystem.
             The cache may be out of date.  When vpath thinks a file
             exists, but stat fails for it, confusion results in the
             higher levels.  */

          struct stat st;

#ifndef VMS
          /* Put the slash back in NAME.  */
          *p = '/';
#else
          /* If the slash was removed, put it back */
          if (*p == 0)
            *p = '/';
#endif

          if (exists_in_cache)  /* Makefile-mentioned file need not exist.  */
            {
              int e;

              EINTRLOOP (e, stat (name, &st)); /* Does it really exist?  */
              if (e != 0)
                {
                  exists = 0;
                  continue;
                }

              /* Store the modtime into *MTIME_PTR for the caller.  */
              if (mtime_ptr != 0)
                {
                  *mtime_ptr = FILE_TIMESTAMP_STAT_MODTIME (name, st);
                  mtime_ptr = 0;
                }
            }

          /* We have found a file.
             If we get here and mtime_ptr hasn't been set, record
             UNKNOWN_MTIME to indicate this.  */
          if (mtime_ptr != 0)
            *mtime_ptr = UNKNOWN_MTIME;

          /* Store the name we found and return it.  */

          if (path_index)
            *path_index = i;

          return strcache_add_len (name, (p + 1 - name) + flen);
        }
    }

  return 0;
}


/* Search the VPATH list whose pattern matches FILE for a directory where FILE
   exists.  If it is found, return the cached name of an existing file, and
   set *MTIME_PTR (if MTIME_PTR is not NULL) to its modtime (or zero if no
   stat call was done). Also set the matching directory index in VPATH_INDEX
   and PATH_INDEX if they are not NULL.  Otherwise we return 0.  */

const char *
vpath_search (const char *file, FILE_TIMESTAMP *mtime_ptr,
              unsigned int* vpath_index, unsigned int* path_index)
{
  struct vpath *v;

  /* If there are no VPATH entries or FILENAME starts at the root,
     there is nothing we can do.  */

  if (file[0] == '/'
#ifdef HAVE_DOS_PATHS
      || file[0] == '\\' || file[1] == ':'
#endif
      || (vpaths == 0 && general_vpath == 0))
    return 0;

  if (vpath_index)
    {
      *vpath_index = 0;
      *path_index = 0;
    }

  for (v = vpaths; v != 0; v = v->next)
    {
      if (pattern_matches (v->pattern, v->percent, file))
        {
          const char *p = selective_vpath_search (
            v, file, mtime_ptr, path_index);
          if (p)
            return p;
        }

      if (vpath_index)
        ++*vpath_index;
    }


  if (general_vpath != 0)
    {
      const char *p = selective_vpath_search (
        general_vpath, file, mtime_ptr, path_index);
      if (p)
        return p;
    }

  return 0;
}




/* Print the data base of VPATH search paths.  */

void
print_vpath_data_base (void)
{
  unsigned int nvpaths;
  struct vpath *v;

  puts (_("\n# VPATH Search Paths\n"));

  nvpaths = 0;
  for (v = vpaths; v != 0; v = v->next)
    {
      unsigned int i;

      ++nvpaths;

      printf ("vpath %s ", v->pattern);

      for (i = 0; v->searchpath[i] != 0; ++i)
        printf ("%s%c", v->searchpath[i],
                v->searchpath[i + 1] == 0 ? '\n' : PATH_SEPARATOR_CHAR);
    }

  if (vpaths == 0)
    puts (_("# No 'vpath' search paths."));
  else
    printf (_("\n# %u 'vpath' search paths.\n"), nvpaths);

  if (general_vpath == 0)
    puts (_("\n# No general ('VPATH' variable) search path."));
  else
    {
      const char **path = general_vpath->searchpath;
      unsigned int i;

      fputs (_("\n# General ('VPATH' variable) search path:\n# "), stdout);

      for (i = 0; path[i] != 0; ++i)
        printf ("%s%c", path[i],
                path[i + 1] == 0 ? '\n' : PATH_SEPARATOR_CHAR);
    }
}
