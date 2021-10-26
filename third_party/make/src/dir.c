/* Directory hashing for GNU Make.
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
#include "hash.h"
#include "filedef.h"
#include "dep.h"

#ifdef  HAVE_DIRENT_H
# include <dirent.h>
# define NAMLEN(dirent) strlen((dirent)->d_name)
# ifdef VMS
/* its prototype is in vmsdir.h, which is not needed for HAVE_DIRENT_H */
const char *vmsify (const char *name, int type);
# endif
#else
# define dirent direct
# define NAMLEN(dirent) (dirent)->d_namlen
# ifdef HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# ifdef HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# ifdef HAVE_NDIR_H
#  include <ndir.h>
# endif
# ifdef HAVE_VMSDIR_H
#  include "vmsdir.h"
# endif /* HAVE_VMSDIR_H */
#endif

/* In GNU systems, <dirent.h> defines this macro for us.  */
#ifdef _D_NAMLEN
# undef NAMLEN
# define NAMLEN(d) _D_NAMLEN(d)
#endif

#if (defined (POSIX) || defined (VMS) || defined (WINDOWS32)) && !defined (__GNU_LIBRARY__)
/* Posix does not require that the d_ino field be present, and some
   systems do not provide it. */
# define REAL_DIR_ENTRY(dp) 1
# define FAKE_DIR_ENTRY(dp)
#else
# define REAL_DIR_ENTRY(dp) (dp->d_ino != 0)
# define FAKE_DIR_ENTRY(dp) (dp->d_ino = 1)
#endif /* POSIX */

#ifdef __MSDOS__
#include <ctype.h>
#include <fcntl.h>

/* If it's MSDOS that doesn't have _USE_LFN, disable LFN support.  */
#ifndef _USE_LFN
#define _USE_LFN 0
#endif

static const char *
dosify (const char *filename)
{
  static char dos_filename[14];
  char *df;
  int i;

  if (filename == 0 || _USE_LFN)
    return filename;

  /* FIXME: what about filenames which violate
     8+3 constraints, like "config.h.in", or ".emacs"?  */
  if (strpbrk (filename, "\"*+,;<=>?[\\]|") != 0)
    return filename;

  df = dos_filename;

  /* First, transform the name part.  */
  for (i = 0; i < 8 && ! STOP_SET (*filename, MAP_DOT|MAP_NUL); ++i)
    *df++ = tolower ((unsigned char)*filename++);

  /* Now skip to the next dot.  */
  while (! STOP_SET (*filename, MAP_DOT|MAP_NUL))
    ++filename;
  if (*filename != '\0')
    {
      *df++ = *filename++;
      for (i = 0; i < 3 && ! STOP_SET (*filename, MAP_DOT|MAP_NUL); ++i)
        *df++ = tolower ((unsigned char)*filename++);
    }

  /* Look for more dots.  */
  while (! STOP_SET (*filename, MAP_DOT|MAP_NUL))
    ++filename;
  if (*filename == '.')
    return filename;
  *df = 0;
  return dos_filename;
}
#endif /* __MSDOS__ */

#ifdef WINDOWS32
#include "pathstuff.h"
#endif

#ifdef _AMIGA
#include <ctype.h>
#endif

#ifdef HAVE_CASE_INSENSITIVE_FS
static const char *
downcase (const char *filename)
{
  static PATH_VAR (new_filename);
  char *df;

  if (filename == 0)
    return 0;

  df = new_filename;
  while (*filename != '\0')
    {
      *df++ = tolower ((unsigned char)*filename);
      ++filename;
    }

  *df = 0;

  return new_filename;
}
#endif /* HAVE_CASE_INSENSITIVE_FS */

#ifdef VMS

static char *
downcase_inplace(char *filename)
{
  char *name;
  name = filename;
  while (*name != '\0')
    {
      *name = tolower ((unsigned char)*name);
      ++name;
    }
  return filename;
}

#ifndef _USE_STD_STAT
/* VMS 8.2 fixed the VMS stat output to have unique st_dev and st_ino
   when _USE_STD_STAT is used on the compile line.

   Prior to _USE_STD_STAT support, the st_dev is a pointer to thread
   static memory containing the device of the last filename looked up.

   Todo: find out if the ino_t still needs to be faked on a directory.
 */

/* Define this if the older VMS_INO_T is needed */
#define VMS_INO_T 1

static int
vms_hash (const char *name)
{
  int h = 0;

  while (*name)
    {
      unsigned char uc = *name;
      int g;
#ifdef HAVE_CASE_INSENSITIVE_FS
      h = (h << 4) + (isupper (uc) ? tolower (uc) : uc);
#else
      h = (h << 4) + uc;
#endif
      name++;
      g = h & 0xf0000000;
      if (g)
        {
          h = h ^ (g >> 24);
          h = h ^ g;
        }
    }
  return h;
}

/* fake stat entry for a directory */
static int
vmsstat_dir (const char *name, struct stat *st)
{
  char *s;
  int h;
  DIR *dir;

  dir = opendir (name);
  if (dir == 0)
    return -1;
  closedir (dir);
  s = strchr (name, ':');       /* find device */
  if (s)
    {
      /* to keep the compiler happy we said "const char *name", now we cheat */
      *s++ = 0;
      st->st_dev = (char *)vms_hash (name);
      h = vms_hash (s);
      *(s-1) = ':';
    }
  else
    {
      st->st_dev = 0;
      h = vms_hash (name);
    }

  st->st_ino[0] = h & 0xff;
  st->st_ino[1] = h & 0xff00;
  st->st_ino[2] = h >> 16;

  return 0;
}

# define stat(__path, __sbuf) vmsstat_dir (__path, __sbuf)

#endif /* _USE_STD_STAT */
#endif /* VMS */

/* Hash table of directories.  */

#ifndef DIRECTORY_BUCKETS
#define DIRECTORY_BUCKETS 199
#endif

struct directory_contents
  {
    dev_t dev;                  /* Device and inode numbers of this dir.  */
#ifdef WINDOWS32
    /* Inode means nothing on WINDOWS32. Even file key information is
     * unreliable because it is random per file open and undefined for remote
     * filesystems. The most unique attribute I can come up with is the fully
     * qualified name of the directory. Beware though, this is also
     * unreliable. I'm open to suggestion on a better way to emulate inode.  */
    char *path_key;
    time_t ctime;
    time_t mtime;        /* controls check for stale directory cache */
    int fs_flags;     /* FS_FAT, FS_NTFS, ... */
# define FS_FAT      0x1
# define FS_NTFS     0x2
# define FS_UNKNOWN  0x4
#else
# ifdef VMS_INO_T
    ino_t ino[3];
# else
    ino_t ino;
# endif
#endif /* WINDOWS32 */
    struct hash_table dirfiles; /* Files in this directory.  */
    DIR *dirstream;             /* Stream reading this directory.  */
  };

static unsigned long
directory_contents_hash_1 (const void *key_0)
{
  const struct directory_contents *key = key_0;
  unsigned long hash;

#ifdef WINDOWS32
  hash = 0;
  ISTRING_HASH_1 (key->path_key, hash);
  hash ^= ((unsigned int) key->dev << 4) ^ (unsigned int) key->ctime;
#else
# ifdef VMS_INO_T
  hash = (((unsigned int) key->dev << 4)
          ^ ((unsigned int) key->ino[0]
             + (unsigned int) key->ino[1]
             + (unsigned int) key->ino[2]));
# else
  hash = ((unsigned int) key->dev << 4) ^ (unsigned int) key->ino;
# endif
#endif /* WINDOWS32 */
  return hash;
}

static unsigned long
directory_contents_hash_2 (const void *key_0)
{
  const struct directory_contents *key = key_0;
  unsigned long hash;

#ifdef WINDOWS32
  hash = 0;
  ISTRING_HASH_2 (key->path_key, hash);
  hash ^= ((unsigned int) key->dev << 4) ^ (unsigned int) ~key->ctime;
#else
# ifdef VMS_INO_T
  hash = (((unsigned int) key->dev << 4)
          ^ ~((unsigned int) key->ino[0]
              + (unsigned int) key->ino[1]
              + (unsigned int) key->ino[2]));
# else
  hash = ((unsigned int) key->dev << 4) ^ (unsigned int) ~key->ino;
# endif
#endif /* WINDOWS32 */

  return hash;
}

/* Sometimes it's OK to use subtraction to get this value:
     result = X - Y;
   But, if we're not sure of the type of X and Y they may be too large for an
   int (on a 64-bit system for example).  So, use ?: instead.
   See Savannah bug #15534.

   NOTE!  This macro has side-effects!
*/

#define MAKECMP(_x,_y)  ((_x)<(_y)?-1:((_x)==(_y)?0:1))

static int
directory_contents_hash_cmp (const void *xv, const void *yv)
{
  const struct directory_contents *x = xv;
  const struct directory_contents *y = yv;
  int result;

#ifdef WINDOWS32
  ISTRING_COMPARE (x->path_key, y->path_key, result);
  if (result)
    return result;
  result = MAKECMP(x->ctime, y->ctime);
  if (result)
    return result;
#else
# ifdef VMS_INO_T
  result = MAKECMP(x->ino[0], y->ino[0]);
  if (result)
    return result;
  result = MAKECMP(x->ino[1], y->ino[1]);
  if (result)
    return result;
  result = MAKECMP(x->ino[2], y->ino[2]);
  if (result)
    return result;
# else
  result = MAKECMP(x->ino, y->ino);
  if (result)
    return result;
# endif
#endif /* WINDOWS32 */

  return MAKECMP(x->dev, y->dev);
}

/* Table of directory contents hashed by device and inode number.  */
static struct hash_table directory_contents;

struct directory
  {
    const char *name;                   /* Name of the directory.  */

    /* The directory's contents.  This data may be shared by several
       entries in the hash table, which refer to the same directory
       (identified uniquely by 'dev' and 'ino') under different names.  */
    struct directory_contents *contents;
  };

static unsigned long
directory_hash_1 (const void *key)
{
  return_ISTRING_HASH_1 (((const struct directory *) key)->name);
}

static unsigned long
directory_hash_2 (const void *key)
{
  return_ISTRING_HASH_2 (((const struct directory *) key)->name);
}

static int
directory_hash_cmp (const void *x, const void *y)
{
  return_ISTRING_COMPARE (((const struct directory *) x)->name,
                          ((const struct directory *) y)->name);
}

/* Table of directories hashed by name.  */
static struct hash_table directories;

/* Never have more than this many directories open at once.  */

#define MAX_OPEN_DIRECTORIES 10

static unsigned int open_directories = 0;


/* Hash table of files in each directory.  */

struct dirfile
  {
    const char *name;           /* Name of the file.  */
    size_t length;
    short impossible;           /* This file is impossible.  */
    unsigned char type;
  };

static unsigned long
dirfile_hash_1 (const void *key)
{
  return_ISTRING_HASH_1 (((struct dirfile const *) key)->name);
}

static unsigned long
dirfile_hash_2 (const void *key)
{
  return_ISTRING_HASH_2 (((struct dirfile const *) key)->name);
}

static int
dirfile_hash_cmp (const void *xv, const void *yv)
{
  const struct dirfile *x = xv;
  const struct dirfile *y = yv;
  int result = (int) (x->length - y->length);
  if (result)
    return result;
  return_ISTRING_COMPARE (x->name, y->name);
}

#ifndef DIRFILE_BUCKETS
#define DIRFILE_BUCKETS 107
#endif

static int dir_contents_file_exists_p (struct directory_contents *dir,
                                       const char *filename);
static struct directory *find_directory (const char *name);

/* Find the directory named NAME and return its 'struct directory'.  */

static struct directory *
find_directory (const char *name)
{
  struct directory *dir;
  struct directory **dir_slot;
  struct directory dir_key;

  dir_key.name = name;
  dir_slot = (struct directory **) hash_find_slot (&directories, &dir_key);
  dir = *dir_slot;

  if (HASH_VACANT (dir))
    {
      /* The directory was not found.  Create a new entry for it.  */
      const char *p = name + strlen (name);
      struct stat st;
      int r;

      dir = xmalloc (sizeof (struct directory));
#if defined(HAVE_CASE_INSENSITIVE_FS) && defined(VMS)
      /* Todo: Why is this only needed on VMS? */
      {
        char *lname = downcase_inplace (xstrdup (name));
        dir->name = strcache_add_len (lname, p - name);
        free (lname);
      }
#else
      dir->name = strcache_add_len (name, p - name);
#endif
      hash_insert_at (&directories, dir, dir_slot);
      /* The directory is not in the name hash table.
         Find its device and inode numbers, and look it up by them.  */

#if defined(WINDOWS32)
      {
        char tem[MAXPATHLEN], *tstart, *tend;

        /* Remove any trailing slashes.  Windows32 stat fails even on
           valid directories if they end in a slash. */
        memcpy (tem, name, p - name + 1);
        tstart = tem;
        if (tstart[1] == ':')
          tstart += 2;
        for (tend = tem + (p - name - 1);
             tend > tstart && (*tend == '/' || *tend == '\\');
             tend--)
          *tend = '\0';

        r = stat (tem, &st);
      }
#else
      EINTRLOOP (r, stat (name, &st));
#endif

      if (r < 0)
        {
        /* Couldn't stat the directory.  Mark this by
           setting the 'contents' member to a nil pointer.  */
          dir->contents = 0;
        }
      else
        {
          /* Search the contents hash table; device and inode are the key.  */

#ifdef WINDOWS32
          char *w32_path;
#endif
          struct directory_contents *dc;
          struct directory_contents **dc_slot;
          struct directory_contents dc_key;

          dc_key.dev = st.st_dev;
#ifdef WINDOWS32
          dc_key.path_key = w32_path = w32ify (name, 1);
          dc_key.ctime = st.st_ctime;
#else
# ifdef VMS_INO_T
          dc_key.ino[0] = st.st_ino[0];
          dc_key.ino[1] = st.st_ino[1];
          dc_key.ino[2] = st.st_ino[2];
# else
          dc_key.ino = st.st_ino;
# endif
#endif
          dc_slot = (struct directory_contents **) hash_find_slot (&directory_contents, &dc_key);
          dc = *dc_slot;

          if (HASH_VACANT (dc))
            {
              /* Nope; this really is a directory we haven't seen before.  */
#ifdef WINDOWS32
              char  fs_label[BUFSIZ];
              char  fs_type[BUFSIZ];
              unsigned long  fs_serno;
              unsigned long  fs_flags;
              unsigned long  fs_len;
#endif
              dc = (struct directory_contents *)
                xmalloc (sizeof (struct directory_contents));

              /* Enter it in the contents hash table.  */
              dc->dev = st.st_dev;
#ifdef WINDOWS32
              dc->path_key = xstrdup (w32_path);
              dc->ctime = st.st_ctime;
              dc->mtime = st.st_mtime;

              /* NTFS is the only WINDOWS32 filesystem that bumps mtime on a
                 directory when files are added/deleted from a directory.  */
              w32_path[3] = '\0';
              if (GetVolumeInformation (w32_path, fs_label, sizeof (fs_label),
                                        &fs_serno, &fs_len, &fs_flags, fs_type,
                                        sizeof (fs_type)) == FALSE)
                dc->fs_flags = FS_UNKNOWN;
              else if (!strcmp (fs_type, "FAT"))
                dc->fs_flags = FS_FAT;
              else if (!strcmp (fs_type, "NTFS"))
                dc->fs_flags = FS_NTFS;
              else
                dc->fs_flags = FS_UNKNOWN;
#else
# ifdef VMS_INO_T
              dc->ino[0] = st.st_ino[0];
              dc->ino[1] = st.st_ino[1];
              dc->ino[2] = st.st_ino[2];
# else
              dc->ino = st.st_ino;
# endif
#endif /* WINDOWS32 */
              hash_insert_at (&directory_contents, dc, dc_slot);
              ENULLLOOP (dc->dirstream, opendir (name));
              if (dc->dirstream == 0)
                /* Couldn't open the directory.  Mark this by setting the
                   'files' member to a nil pointer.  */
                dc->dirfiles.ht_vec = 0;
              else
                {
                  hash_init (&dc->dirfiles, DIRFILE_BUCKETS,
                             dirfile_hash_1, dirfile_hash_2, dirfile_hash_cmp);
                  /* Keep track of how many directories are open.  */
                  ++open_directories;
                  if (open_directories == MAX_OPEN_DIRECTORIES)
                    /* We have too many directories open already.
                       Read the entire directory and then close it.  */
                    dir_contents_file_exists_p (dc, 0);
                }
            }

          /* Point the name-hashed entry for DIR at its contents data.  */
          dir->contents = dc;
        }
    }

  return dir;
}

/* Return 1 if the name FILENAME is entered in DIR's hash table.
   FILENAME must contain no slashes.  */

static int
dir_contents_file_exists_p (struct directory_contents *dir,
                            const char *filename)
{
  struct dirfile *df;
  struct dirent *d;
#ifdef WINDOWS32
  struct stat st;
  int rehash = 0;
#endif

  if (dir == 0 || dir->dirfiles.ht_vec == 0)
    /* The directory could not be stat'd or opened.  */
    return 0;

#ifdef __MSDOS__
  filename = dosify (filename);
#endif

#ifdef HAVE_CASE_INSENSITIVE_FS
  filename = downcase (filename);
#endif

#ifdef __EMX__
  if (filename != 0)
    _fnlwr (filename); /* lower case for FAT drives */
#endif
  if (filename != 0)
    {
      struct dirfile dirfile_key;

      if (*filename == '\0')
        {
          /* Checking if the directory exists.  */
          return 1;
        }
      dirfile_key.name = filename;
      dirfile_key.length = strlen (filename);
      df = hash_find_item (&dir->dirfiles, &dirfile_key);
      if (df)
        return !df->impossible;
    }

  /* The file was not found in the hashed list.
     Try to read the directory further.  */

  if (dir->dirstream == 0)
    {
#ifdef WINDOWS32
      /*
       * Check to see if directory has changed since last read. FAT
       * filesystems force a rehash always as mtime does not change
       * on directories (ugh!).
       */
      if (dir->path_key)
        {
          if ((dir->fs_flags & FS_FAT) != 0)
            {
              dir->mtime = time ((time_t *) 0);
              rehash = 1;
            }
          else if (stat (dir->path_key, &st) == 0 && st.st_mtime > dir->mtime)
            {
              /* reset date stamp to show most recent re-process.  */
              dir->mtime = st.st_mtime;
              rehash = 1;
            }

          /* If it has been already read in, all done.  */
          if (!rehash)
            return 0;

          /* make sure directory can still be opened; if not return.  */
          dir->dirstream = opendir (dir->path_key);
          if (!dir->dirstream)
            return 0;
        }
      else
#endif
        /* The directory has been all read in.  */
        return 0;
    }

  while (1)
    {
      /* Enter the file in the hash table.  */
      size_t len;
      struct dirfile dirfile_key;
      struct dirfile **dirfile_slot;

      ENULLLOOP (d, readdir (dir->dirstream));
      if (d == 0)
        {
          if (errno)
            pfatal_with_name ("INTERNAL: readdir");
          break;
        }

#if defined(VMS) && defined(HAVE_DIRENT_H)
      /* In VMS we get file versions too, which have to be stripped off.
         Some versions of VMS return versions on Unix files even when
         the feature option to strip them is set.  */
      {
        char *p = strrchr (d->d_name, ';');
        if (p)
          *p = '\0';
      }
#endif
      if (!REAL_DIR_ENTRY (d))
        continue;

      len = NAMLEN (d);
      dirfile_key.name = d->d_name;
      dirfile_key.length = len;
      dirfile_slot = (struct dirfile **) hash_find_slot (&dir->dirfiles, &dirfile_key);
#ifdef WINDOWS32
      /*
       * If re-reading a directory, don't cache files that have
       * already been discovered.
       */
      if (! rehash || HASH_VACANT (*dirfile_slot))
#endif
        {
          df = xmalloc (sizeof (struct dirfile));
#if defined(HAVE_CASE_INSENSITIVE_FS) && defined(VMS)
          /* TODO: Why is this only needed on VMS? */
          df->name = strcache_add_len (downcase_inplace (d->d_name), len);
#else
          df->name = strcache_add_len (d->d_name, len);
#endif
#ifdef HAVE_STRUCT_DIRENT_D_TYPE
          df->type = d->d_type;
#endif
          df->length = len;
          df->impossible = 0;
          hash_insert_at (&dir->dirfiles, df, dirfile_slot);
        }
      /* Check if the name matches the one we're searching for.  */
      if (filename != 0 && patheq (d->d_name, filename))
        return 1;
    }

  /* If the directory has been completely read in,
     close the stream and reset the pointer to nil.  */
  if (d == 0)
    {
      --open_directories;
      closedir (dir->dirstream);
      dir->dirstream = 0;
    }
  return 0;
}

/* Return 1 if the name FILENAME in directory DIRNAME
   is entered in the dir hash table.
   FILENAME must contain no slashes.  */

int
dir_file_exists_p (const char *dirname, const char *filename)
{
#ifdef VMS
  if ((filename != NULL) && (dirname != NULL))
    {
      int want_vmsify;
      want_vmsify = (strpbrk (dirname, ":<[") != NULL);
      if (want_vmsify)
        filename = vmsify (filename, 0);
    }
#endif
  return dir_contents_file_exists_p (find_directory (dirname)->contents,
                                     filename);
}

/* Return 1 if the file named NAME exists.  */

int
file_exists_p (const char *name)
{
  const char *dirend;
  const char *dirname;
  const char *slash;

#ifndef NO_ARCHIVES
  if (ar_name (name))
    return ar_member_date (name) != (time_t) -1;
#endif

  dirend = strrchr (name, '/');
#ifdef VMS
  if (dirend == 0)
    {
      dirend = strrchr (name, ']');
      dirend == NULL ? dirend : dirend++;
    }
  if (dirend == 0)
    {
      dirend = strrchr (name, '>');
      dirend == NULL ? dirend : dirend++;
    }
  if (dirend == 0)
    {
      dirend = strrchr (name, ':');
      dirend == NULL ? dirend : dirend++;
    }
#endif /* VMS */
#ifdef HAVE_DOS_PATHS
  /* Forward and backslashes might be mixed.  We need the rightmost one.  */
  {
    const char *bslash = strrchr (name, '\\');
    if (!dirend || bslash > dirend)
      dirend = bslash;
    /* The case of "d:file".  */
    if (!dirend && name[0] && name[1] == ':')
      dirend = name + 1;
  }
#endif /* HAVE_DOS_PATHS */
  if (dirend == 0)
#ifndef _AMIGA
    return dir_file_exists_p (".", name);
#else /* !AMIGA */
    return dir_file_exists_p ("", name);
#endif /* AMIGA */

  slash = dirend;
  if (dirend == name)
    dirname = "/";
  else
    {
      char *p;
#ifdef HAVE_DOS_PATHS
  /* d:/ and d: are *very* different...  */
      if (dirend < name + 3 && name[1] == ':' &&
          (*dirend == '/' || *dirend == '\\' || *dirend == ':'))
        dirend++;
#endif
      p = alloca (dirend - name + 1);
      memcpy (p, name, dirend - name);
      p[dirend - name] = '\0';
      dirname = p;
    }
#ifdef VMS
  if (*slash == '/')
    slash++;
#else
  slash++;
#endif
  return dir_file_exists_p (dirname, slash);
}

/* Mark FILENAME as 'impossible' for 'file_impossible_p'.
   This means an attempt has been made to search for FILENAME
   as an intermediate file, and it has failed.  */

void
file_impossible (const char *filename)
{
  const char *dirend;
  const char *p = filename;
  struct directory *dir;
  struct dirfile *new;

  dirend = strrchr (p, '/');
#ifdef VMS
  if (dirend == NULL)
    {
      dirend = strrchr (p, ']');
      dirend == NULL ? dirend : dirend++;
    }
  if (dirend == NULL)
    {
      dirend = strrchr (p, '>');
      dirend == NULL ? dirend : dirend++;
    }
  if (dirend == NULL)
    {
      dirend = strrchr (p, ':');
      dirend == NULL ? dirend : dirend++;
    }
#endif
#ifdef HAVE_DOS_PATHS
  /* Forward and backslashes might be mixed.  We need the rightmost one.  */
  {
    const char *bslash = strrchr (p, '\\');
    if (!dirend || bslash > dirend)
      dirend = bslash;
    /* The case of "d:file".  */
    if (!dirend && p[0] && p[1] == ':')
      dirend = p + 1;
  }
#endif /* HAVE_DOS_PATHS */
  if (dirend == 0)
#ifdef _AMIGA
    dir = find_directory ("");
#else /* !AMIGA */
    dir = find_directory (".");
#endif /* AMIGA */
  else
    {
      const char *dirname;
      const char *slash = dirend;
      if (dirend == p)
        dirname = "/";
      else
        {
          char *cp;
#ifdef HAVE_DOS_PATHS
          /* d:/ and d: are *very* different...  */
          if (dirend < p + 3 && p[1] == ':' &&
              (*dirend == '/' || *dirend == '\\' || *dirend == ':'))
            dirend++;
#endif
          cp = alloca (dirend - p + 1);
          memcpy (cp, p, dirend - p);
          cp[dirend - p] = '\0';
          dirname = cp;
        }
      dir = find_directory (dirname);
#ifdef VMS
      if (*slash == '/')
        filename = p = slash + 1;
      else
        filename = p = slash;
#else
      filename = p = slash + 1;
#endif
    }

  if (dir->contents == 0)
    /* The directory could not be stat'd.  We allocate a contents
       structure for it, but leave it out of the contents hash table.  */
    dir->contents = xcalloc (sizeof (struct directory_contents));

  if (dir->contents->dirfiles.ht_vec == 0)
    {
      hash_init (&dir->contents->dirfiles, DIRFILE_BUCKETS,
                 dirfile_hash_1, dirfile_hash_2, dirfile_hash_cmp);
    }

  /* Make a new entry and put it in the table.  */

  new = xmalloc (sizeof (struct dirfile));
  new->length = strlen (filename);
#if defined(HAVE_CASE_INSENSITIVE_FS) && defined(VMS)
  /* todo: Why is this only needed on VMS? */
  new->name = strcache_add_len (downcase (filename), new->length);
#else
  new->name = strcache_add_len (filename, new->length);
#endif
  new->impossible = 1;
  hash_insert (&dir->contents->dirfiles, new);
}

/* Return nonzero if FILENAME has been marked impossible.  */

int
file_impossible_p (const char *filename)
{
  const char *dirend;
  struct directory_contents *dir;
  struct dirfile *dirfile;
  struct dirfile dirfile_key;
#ifdef VMS
  int want_vmsify = 0;
#endif

  dirend = strrchr (filename, '/');
#ifdef VMS
  if (dirend == NULL)
    {
      want_vmsify = (strpbrk (filename, "]>:^") != NULL);
      dirend = strrchr (filename, ']');
    }
  if (dirend == NULL && want_vmsify)
    dirend = strrchr (filename, '>');
  if (dirend == NULL && want_vmsify)
    dirend = strrchr (filename, ':');
#endif
#ifdef HAVE_DOS_PATHS
  /* Forward and backslashes might be mixed.  We need the rightmost one.  */
  {
    const char *bslash = strrchr (filename, '\\');
    if (!dirend || bslash > dirend)
      dirend = bslash;
    /* The case of "d:file".  */
    if (!dirend && filename[0] && filename[1] == ':')
      dirend = filename + 1;
  }
#endif /* HAVE_DOS_PATHS */
  if (dirend == 0)
#ifdef _AMIGA
    dir = find_directory ("")->contents;
#else /* !AMIGA */
    dir = find_directory (".")->contents;
#endif /* AMIGA */
  else
    {
      const char *dirname;
      const char *slash = dirend;
      if (dirend == filename)
        dirname = "/";
      else
        {
          char *cp;
#ifdef HAVE_DOS_PATHS
          /* d:/ and d: are *very* different...  */
          if (dirend < filename + 3 && filename[1] == ':' &&
              (*dirend == '/' || *dirend == '\\' || *dirend == ':'))
            dirend++;
#endif
          cp = alloca (dirend - filename + 1);
          memcpy (cp, filename, dirend - filename);
          cp[dirend - filename] = '\0';
          dirname = cp;
        }
      dir = find_directory (dirname)->contents;
#ifdef VMS
      if (*slash == '/')
        filename = slash + 1;
      else
        filename = slash;
#else
      filename = slash + 1;
#endif
    }

  if (dir == 0 || dir->dirfiles.ht_vec == 0)
    /* There are no files entered for this directory.  */
    return 0;

#ifdef __MSDOS__
  filename = dosify (filename);
#endif
#ifdef HAVE_CASE_INSENSITIVE_FS
  filename = downcase (filename);
#endif
#ifdef VMS
  if (want_vmsify)
    filename = vmsify (filename, 1);
#endif

  dirfile_key.name = filename;
  dirfile_key.length = strlen (filename);
  dirfile = hash_find_item (&dir->dirfiles, &dirfile_key);
  if (dirfile)
    return dirfile->impossible;

  return 0;
}

/* Return the already allocated name in the
   directory hash table that matches DIR.  */

const char *
dir_name (const char *dir)
{
  return find_directory (dir)->name;
}

/* Print the data base of directories.  */

void
print_dir_data_base (void)
{
  unsigned int files;
  unsigned int impossible;
  struct directory **dir_slot;
  struct directory **dir_end;

  puts (_("\n# Directories\n"));

  files = impossible = 0;

  dir_slot = (struct directory **) directories.ht_vec;
  dir_end = dir_slot + directories.ht_size;
  for ( ; dir_slot < dir_end; dir_slot++)
    {
      struct directory *dir = *dir_slot;
      if (! HASH_VACANT (dir))
        {
          if (dir->contents == 0)
            printf (_("# %s: could not be stat'd.\n"), dir->name);
          else if (dir->contents->dirfiles.ht_vec == 0)
            {
#ifdef WINDOWS32
              printf (_("# %s (key %s, mtime %I64u): could not be opened.\n"),
                      dir->name, dir->contents->path_key,
                      (unsigned long long)dir->contents->mtime);
#else  /* WINDOWS32 */
#ifdef VMS_INO_T
              printf (_("# %s (device %d, inode [%d,%d,%d]): could not be opened.\n"),
                      dir->name, dir->contents->dev,
                      dir->contents->ino[0], dir->contents->ino[1],
                      dir->contents->ino[2]);
#else
              printf (_("# %s (device %ld, inode %ld): could not be opened.\n"),
                      dir->name, (long int) dir->contents->dev,
                      (long int) dir->contents->ino);
#endif
#endif /* WINDOWS32 */
            }
          else
            {
              unsigned int f = 0;
              unsigned int im = 0;
              struct dirfile **files_slot;
              struct dirfile **files_end;

              files_slot = (struct dirfile **) dir->contents->dirfiles.ht_vec;
              files_end = files_slot + dir->contents->dirfiles.ht_size;
              for ( ; files_slot < files_end; files_slot++)
                {
                  struct dirfile *df = *files_slot;
                  if (! HASH_VACANT (df))
                    {
                      if (df->impossible)
                        ++im;
                      else
                        ++f;
                    }
                }
#ifdef WINDOWS32
              printf (_("# %s (key %s, mtime %I64u): "),
                      dir->name, dir->contents->path_key,
                      (unsigned long long)dir->contents->mtime);
#else  /* WINDOWS32 */
#ifdef VMS_INO_T
              printf (_("# %s (device %d, inode [%d,%d,%d]): "),
                      dir->name, dir->contents->dev,
                      dir->contents->ino[0], dir->contents->ino[1],
                      dir->contents->ino[2]);
#else
              printf (_("# %s (device %ld, inode %ld): "),
                      dir->name,
                      (long)dir->contents->dev, (long)dir->contents->ino);
#endif
#endif /* WINDOWS32 */
              if (f == 0)
                fputs (_("No"), stdout);
              else
                printf ("%u", f);
              fputs (_(" files, "), stdout);
              if (im == 0)
                fputs (_("no"), stdout);
              else
                printf ("%u", im);
              fputs (_(" impossibilities"), stdout);
              if (dir->contents->dirstream == 0)
                puts (".");
              else
                puts (_(" so far."));
              files += f;
              impossible += im;
            }
        }
    }

  fputs ("\n# ", stdout);
  if (files == 0)
    fputs (_("No"), stdout);
  else
    printf ("%u", files);
  fputs (_(" files, "), stdout);
  if (impossible == 0)
    fputs (_("no"), stdout);
  else
    printf ("%u", impossible);
  printf (_(" impossibilities in %lu directories.\n"), directories.ht_fill);
}

/* Hooks for globbing.  */

/* Structure describing state of iterating through a directory hash table.  */

struct dirstream
  {
    struct directory_contents *contents; /* The directory being read.  */
    struct dirfile **dirfile_slot; /* Current slot in table.  */
  };

/* Forward declarations.  */
static __ptr_t open_dirstream (const char *);
static struct dirent *read_dirstream (__ptr_t);

static __ptr_t
open_dirstream (const char *directory)
{
  struct dirstream *new;
  struct directory *dir = find_directory (directory);

  if (dir->contents == 0 || dir->contents->dirfiles.ht_vec == 0)
    /* DIR->contents is nil if the directory could not be stat'd.
       DIR->contents->dirfiles is nil if it could not be opened.  */
    return 0;

  /* Read all the contents of the directory now.  There is no benefit
     in being lazy, since glob will want to see every file anyway.  */

  dir_contents_file_exists_p (dir->contents, 0);

  new = xmalloc (sizeof (struct dirstream));
  new->contents = dir->contents;
  new->dirfile_slot = (struct dirfile **) new->contents->dirfiles.ht_vec;

  return (__ptr_t) new;
}

static struct dirent *
read_dirstream (__ptr_t stream)
{
  static char *buf;
  static size_t bufsz;

  struct dirstream *const ds = (struct dirstream *) stream;
  struct directory_contents *dc = ds->contents;
  struct dirfile **dirfile_end = (struct dirfile **) dc->dirfiles.ht_vec + dc->dirfiles.ht_size;

  while (ds->dirfile_slot < dirfile_end)
    {
      struct dirfile *df = *ds->dirfile_slot++;
      if (! HASH_VACANT (df) && !df->impossible)
        {
          /* The glob interface wants a 'struct dirent', so mock one up.  */
          struct dirent *d;
          size_t len = df->length + 1;
          size_t sz = sizeof (*d) - sizeof (d->d_name) + len;
          if (sz > bufsz)
            {
              bufsz *= 2;
              if (sz > bufsz)
                bufsz = sz;
              buf = xrealloc (buf, bufsz);
            }
          d = (struct dirent *) buf;
#ifdef __MINGW32__
# if __MINGW32_MAJOR_VERSION < 3 || (__MINGW32_MAJOR_VERSION == 3 && \
                                     __MINGW32_MINOR_VERSION == 0)
          d->d_name = xmalloc (len);
# endif
#endif
          FAKE_DIR_ENTRY (d);
#ifdef _DIRENT_HAVE_D_NAMLEN
          d->d_namlen = len - 1;
#endif
#ifdef HAVE_STRUCT_DIRENT_D_TYPE
          d->d_type = df->type;
#endif
          memcpy (d->d_name, df->name, len);
          return d;
        }
    }

  return 0;
}

/* On 64 bit ReliantUNIX (5.44 and above) in LFS mode, stat() is actually a
 * macro for stat64().  If stat is a macro, make a local wrapper function to
 * invoke it.
 *
 * On MS-Windows, stat() "succeeds" for foo/bar/. where foo/bar is a
 * regular file; fix that here.
 */
#if !defined(stat) && !defined(WINDOWS32) || defined(VMS)
# ifndef VMS
#  ifndef HAVE_SYS_STAT_H
int stat (const char *path, struct stat *sbuf);
#  endif
# else
    /* We are done with the fake stat.  Go back to the real stat */
#   ifdef stat
#     undef stat
#   endif
# endif
# define local_stat stat
#else
static int
local_stat (const char *path, struct stat *buf)
{
  int e;
#ifdef WINDOWS32
  size_t plen = strlen (path);

  /* Make sure the parent of "." exists and is a directory, not a
     file.  This is because 'stat' on Windows normalizes the argument
     foo/. => foo without checking first that foo is a directory.  */
  if (plen > 1 && path[plen - 1] == '.'
      && (path[plen - 2] == '/' || path[plen - 2] == '\\'))
    {
      char parent[MAXPATHLEN];

      strncpy (parent, path, plen - 2);
      parent[plen - 2] = '\0';
      if (stat (parent, buf) < 0 || !_S_ISDIR (buf->st_mode))
        return -1;
    }
#endif

  EINTRLOOP (e, stat (path, buf));
  return e;
}
#endif

/* Similarly for lstat.  */
#if !defined(lstat) && !defined(WINDOWS32) || defined(VMS)
# ifndef VMS
#  ifndef HAVE_SYS_STAT_H
int lstat (const char *path, struct stat *sbuf);
#  endif
# else
    /* We are done with the fake lstat.  Go back to the real lstat */
#   ifdef lstat
#     undef lstat
#   endif
# endif
# define local_lstat lstat
#elif defined(WINDOWS32)
/* Windows doesn't support lstat().  */
# define local_lstat local_stat
#else
static int
local_lstat (const char *path, struct stat *buf)
{
  int e;
  EINTRLOOP (e, lstat (path, buf));
  return e;
}
#endif

void
dir_setup_glob (glob_t *gl)
{
  gl->gl_offs = 0;
  gl->gl_opendir = open_dirstream;
  gl->gl_readdir = read_dirstream;
  gl->gl_closedir = free;
  gl->gl_lstat = local_lstat;
  gl->gl_stat = local_stat;
}

void
hash_init_directories (void)
{
  hash_init (&directories, DIRECTORY_BUCKETS,
             directory_hash_1, directory_hash_2, directory_hash_cmp);
  hash_init (&directory_contents, DIRECTORY_BUCKETS,
             directory_contents_hash_1, directory_contents_hash_2,
             directory_contents_hash_cmp);
}
