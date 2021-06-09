/*
  Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 1999-Oct-05 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, both of these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html

  This AtheOS - specific file is based on unix.c and beos.c;
  changes by Ruslan Nickolaev (nruslan@hotbox.ru)
*/

#include "zip.h"

#ifndef UTIL    /* the companion #endif is a bit of ways down ... */

#include <time.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdlib.h>
#include <string.h>

#include <atheos/fs_attribs.h>


#define PAD 0
#define PATH_END '/'

/* Library functions not in (most) header files */

#ifdef _POSIX_VERSION
#  include <utime.h>
#else
   int utime OF((char *, time_t *));
#endif

extern char *label;
local ulg label_time = 0;
local ulg label_mode = 0;
local time_t label_utim = 0;

/* Local functions */
local char *readd OF((DIR *));
local int get_attr_dir( const char *, char **, off_t * );
local int add_UT_ef( struct zlist far * );
local int add_Ux_ef( struct zlist far * );
local int add_At_ef( struct zlist far * );

local char *readd(d)
DIR *d;                 /* directory stream to read from */
/* Return a pointer to the next name in the directory stream d, or NULL if
   no more entries or an error occurs. */
{
  struct dirent *e;

  e = readdir(d);
  return e == NULL ? (char *) NULL : e->d_name;
}

int procname(n, caseflag)
char *n;                /* name to process */
int caseflag;           /* true to force case-sensitive match */
/* Process a name or sh expression to operate on (or exclude).  Return
   an error code in the ZE_ class. */
{
  char *a;              /* path and name for recursion */
  DIR *d;               /* directory stream from opendir() */
  char *e;              /* pointer to name from readd() */
  int m;                /* matched flag */
  char *p;              /* path for recursion */
  struct stat s;        /* result of stat() */
  struct zlist far *z;  /* steps through zfiles list */

  if (strcmp(n, "-") == 0)   /* if compressing stdin */
    return newname(n, 0, caseflag);
  else if (LSSTAT(n, &s))
  {
    /* Not a file or directory--search for shell expression in zip file */
    p = ex2in(n, 0, (int *)NULL);       /* shouldn't affect matching chars */
    m = 1;
    for (z = zfiles; z != NULL; z = z->nxt) {
      if (MATCH(p, z->iname, caseflag))
      {
        z->mark = pcount ? filter(z->zname, caseflag) : 1;
        if (verbose)
            fprintf(mesg, "zip diagnostic: %scluding %s\n",
               z->mark ? "in" : "ex", z->name);
        m = 0;
      }
    }
    free((zvoid *)p);
    return m ? ZE_MISS : ZE_OK;
  }

  /* Live name--use if file, recurse if directory */
  if ((s.st_mode & S_IFREG) == S_IFREG ||
      (s.st_mode & S_IFLNK) == S_IFLNK)
  {
    /* add or remove name of file */
    if ((m = newname(n, 0, caseflag)) != ZE_OK)
      return m;
  }
  else if ((s.st_mode & S_IFDIR) == S_IFDIR)
  {
    /* Add trailing / to the directory name */
    if ((p = malloc(strlen(n)+2)) == NULL)
      return ZE_MEM;
    if (strcmp(n, ".") == 0) {
      *p = '\0';  /* avoid "./" prefix and do not create zip entry */
    } else {
      strcpy(p, n);
      a = p + strlen(p);
      if (a[-1] != '/')
        strcpy(a, "/");
      if (dirnames && (m = newname(p, 1, caseflag)) != ZE_OK) {
        free((zvoid *)p);
        return m;
      }
    }
    /* recurse into directory */
    if (recurse && (d = opendir(n)) != NULL)
    {
      while ((e = readd(d)) != NULL) {
        if (strcmp(e, ".") && strcmp(e, ".."))
        {
          if ((a = malloc(strlen(p) + strlen(e) + 1)) == NULL)
          {
            closedir(d);
            free((zvoid *)p);
            return ZE_MEM;
          }
          strcat(strcpy(a, p), e);
          if ((m = procname(a, caseflag)) != ZE_OK)   /* recurse on name */
          {
            if (m == ZE_MISS)
              zipwarn("name not matched: ", a);
            else
              ziperr(m, a);
          }
          free((zvoid *)a);
        }
      }
      closedir(d);
    }
    free((zvoid *)p);
  } /* (s.st_mode & S_IFDIR) */
  else
    zipwarn("ignoring special file: ", n);
  return ZE_OK;
}

char *ex2in(x, isdir, pdosflag)
char *x;                /* external file name */
int isdir;              /* input: x is a directory */
int *pdosflag;          /* output: force MSDOS file attributes? */
/* Convert the external file name to a zip file name, returning the malloc'ed
   string or NULL if not enough memory. */
{
  char *n;              /* internal file name (malloc'ed) */
  char *t = NULL;       /* shortened name */
  int dosflag;

  dosflag = dosify;  /* default for non-DOS and non-OS/2 */

  /* Find starting point in name before doing malloc */
  /* Strip "//host/share/" part of a UNC name */
  if (!strncmp(x,"//",2) && (x[2] != '\0' && x[2] != '/')) {
    n = x + 2;
    while (*n != '\0' && *n != '/')
      n++;              /* strip host name */
    if (*n != '\0') {
      n++;
      while (*n != '\0' && *n != '/')
        n++;            /* strip `share' name */
    }
    if (*n != '\0')
      t = n + 1;
  } else
      t = x;
  while (*t == '/')
    t++;                /* strip leading '/' chars to get a relative path */
  while (*t == '.' && t[1] == '/')
    t += 2;             /* strip redundant leading "./" sections */

  /* Make changes, if any, to the copied name (leave original intact) */
  if (!pathput)
    t = last(t, PATH_END);

  /* Malloc space for internal name and copy it */
  if ((n = malloc(strlen(t) + 1)) == NULL)
    return NULL;
  strcpy(n, t);

  if (isdir == 42) return n;    /* avoid warning on unused variable */

  if (dosify)
    msname(n);

  /* Returned malloc'ed name */
  if (pdosflag)
    *pdosflag = dosflag;
  return n;
}

char *in2ex(n)
char *n;                /* internal file name */
/* Convert the zip file name to an external file name, returning the malloc'ed
   string or NULL if not enough memory. */
{
  char *x;              /* external file name */

  if ((x = malloc(strlen(n) + 1 + PAD)) == NULL)
    return NULL;
  strcpy(x, n);
  return x;
}

/*
 * XXX use ztimbuf in both POSIX and non POSIX cases ?
 */
void stamp(f, d)
char *f;                /* name of file to change */
ulg d;                  /* dos-style time to change it to */
/* Set last updated and accessed time of file f to the DOS time d. */
{
#ifdef _POSIX_VERSION
  struct utimbuf u;     /* argument for utime()  const ?? */
#else
  time_t u[2];          /* argument for utime() */
#endif

  /* Convert DOS time to time_t format in u */
#ifdef _POSIX_VERSION
  u.actime = u.modtime = dos2unixtime(d);
  utime(f, &u);
#else
  u[0] = u[1] = dos2unixtime(d);
  utime(f, u);
#endif

}

ulg filetime(f, a, n, t)
char *f;                /* name of file to get info on */
ulg *a;                 /* return value: file attributes */
long *n;                /* return value: file size */
iztimes *t;             /* return value: access, modific. and creation times */
/* If file *f does not exist, return 0.  Else, return the file's last
   modified date and time as an MSDOS date and time.  The date and
   time is returned in a long with the date most significant to allow
   unsigned integer comparison of absolute times.  Also, if a is not
   a NULL pointer, store the file attributes there, with the high two
   bytes being the Unix attributes, and the low byte being a mapping
   of that to DOS attributes.  If n is not NULL, store the file size
   there.  If t is not NULL, the file's access, modification and creation
   times are stored there as UNIX time_t values.
   If f is "-", use standard input as the file. If f is a device, return
   a file size of -1 */
{
  struct stat s;        /* results of stat() */
  char *name;
  int len = strlen(f);

  if (f == label) {
    if (a != NULL)
      *a = label_mode;
    if (n != NULL)
      *n = -2L; /* convention for a label name */
    if (t != NULL)
      t->atime = t->mtime = t->ctime = label_utim;
    return label_time;
  }
  if ((name = malloc(len + 1)) == NULL {
    ZIPERR(ZE_MEM, "filetime");
  }
  strcpy(name, f);
  if (name[len - 1] == '/')
    name[len - 1] = '\0';
  /* not all systems allow stat'ing a file with / appended */
  if (strcmp(f, "-") == 0) {
    if (fstat(fileno(stdin), &s) != 0) {
      free(name);
      error("fstat(stdin)");
    }
  }
  else if (LSSTAT(name, &s) != 0) {
    /* Accept about any file kind including directories
     * (stored with trailing / with -r option)
     */
    free(name);
    return 0;
  }
  free(name);

  if (a != NULL) {
#ifndef OS390
    *a = ((ulg)s.st_mode << 16) | !(s.st_mode & S_IWRITE);
#else
/*
**  The following defines are copied from the unizip source and represent the
**  legacy Unix mode flags.  These fixed bit masks are no longer required
**  by XOPEN standards - the S_IS### macros being the new recommended method.
**  The approach here of setting the legacy flags by testing the macros should
**  work under any _XOPEN_SOURCE environment (and will just rebuild the same bit
**  mask), but is required if the legacy bit flags differ from legacy Unix.
*/
#define UNX_IFDIR      0040000     /* Unix directory */
#define UNX_IFREG      0100000     /* Unix regular file */
#define UNX_IFSOCK     0140000     /* Unix socket (BSD, not SysV or Amiga) */
#define UNX_IFLNK      0120000     /* Unix symbolic link (not SysV, Amiga) */
#define UNX_IFBLK      0060000     /* Unix block special       (not Amiga) */
#define UNX_IFCHR      0020000     /* Unix character special   (not Amiga) */
#define UNX_IFIFO      0010000     /* Unix fifo    (BCC, not MSC or Amiga) */
    {
    mode_t legacy_modes;

    /* Initialize with permission bits - which are not implementation optional */
    legacy_modes = s.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO | S_ISUID | S_ISGID | S_ISVTX);
    if (S_ISDIR(s.st_mode))
      legacy_modes |= UNX_IFDIR;
    if (S_ISREG(s.st_mode))
      legacy_modes |= UNX_IFREG;
    if (S_ISLNK(s.st_mode))
      legacy_modes |= UNX_IFLNK;
    if (S_ISBLK(s.st_mode))
      legacy_modes |= UNX_IFBLK;
    if (S_ISCHR(s.st_mode))
      legacy_modes |= UNX_IFCHR;
    if (S_ISFIFO(s.st_mode))
      legacy_modes |= UNX_IFIFO;
    if (S_ISSOCK(s.st_mode))
      legacy_modes |= UNX_IFSOCK;
    *a = ((ulg)legacy_modes << 16) | !(s.st_mode & S_IWRITE);
    }
#endif
    if ((s.st_mode & S_IFMT) == S_IFDIR) {
      *a |= MSDOS_DIR_ATTR;
    }
  }
  if (n != NULL)
    *n = (s.st_mode & S_IFMT) == S_IFREG ? s.st_size : -1L;
  if (t != NULL) {
    t->atime = s.st_atime;
    t->mtime = s.st_mtime;
    t->ctime = t->mtime;   /* best guess, (s.st_ctime: last status change!!) */
  }
  return unix2dostime(&s.st_mtime);
}

/* ----------------------------------------------------------------------

Return a malloc()'d buffer containing all of the attributes and their names
for the file specified in name.  You have to free() this yourself.  The length
of the buffer is also returned.

If get_attr_dir() fails, the buffer will be NULL, total_size will be 0,
and an error will be returned:

    EOK    - no errors occurred
    EINVAL - attr_buff was pointing at a buffer
    ENOMEM - insufficient memory for attribute buffer

Other errors are possible (whatever is returned by the fs_attrib.h functions).

PROBLEMS:

- pointers are 32-bits; attributes are limited to ssize_t in size so it's
  possible to overflow... in practice, this isn't too likely... your
  machine will thrash like hell before that happens

*/

#define INITIAL_BUFF_SIZE 65536

int get_attr_dir( const char *name, char **attr_buff, off_t *total_size )
{
    int               retval = EOK;
    int               fd;
    DIR              *fa_dir;
    struct dirent    *fa_ent;
    off_t             attrs_size = 0;
    size_t           entname_size;
    char             *ptr;
    struct attr_info  fa_info;

    *total_size = 0;

    /* ----------------------------------------------------------------- */
    /* Sanity-check.                                                     */
    if( *attr_buff != NULL ) {
        return EINVAL;
    }

    /* ----------------------------------------------------------------- */
    /* Can we open the file/directory?                                   */
    /*                                                                   */
    /* linkput is a zip global; it's set to 1 if we're storing symbolic  */
    /* links as symbolic links (instead of storing the thing the link    */
    /* points to)... if we're storing the symbolic link as a link, we'll */
    /* want the link's file attributes, otherwise we want the target's.  */

    fd = open( name, linkput ? O_RDONLY | O_NOTRAVERSE : O_RDONLY );
    if( fd < 0 ) {
        return errno;
    }

    /* ----------------------------------------------------------------- */
    /* Allocate an initial buffer; 64k should usually be enough.         */
    *attr_buff = (char *)malloc( INITIAL_BUFF_SIZE );
    ptr        = *attr_buff;
    if( ptr == NULL ) {
        close( fd );

        return ENOMEM;
    }

    /* ----------------------------------------------------------------- */
    /* Open the attributes directory for this file.                      */
    fa_dir = open_attrdir( fd );
    if( fa_dir == NULL ) {
        close( fd );

        free( ptr );
        *attr_buff = NULL;

        return retval;
    }

    /* ----------------------------------------------------------------- */
    /* Read all the attributes; the buffer could grow > 64K if there are */
    /* many and/or they are large.                                       */
    while( ( fa_ent = read_attrdir( fa_dir ) ) != NULL ) {
        retval = stat_attr( fd, fa_ent->d_name, &fa_info );
        /* TODO: check retval != EOK */

        entname_size = strlen( fa_ent->d_name ) + 1;
        attrs_size += entname_size + sizeof( struct attr_info ) + fa_info.ai_size;

        if( attrs_size > INITIAL_BUFF_SIZE ) {
            unsigned long offset = ptr - *attr_buff;

            *attr_buff = (char *)realloc( *attr_buff, attrs_size );
            if( *attr_buff == NULL ) {
                retval = close_attrdir( fa_dir );
                /* TODO: check retval != EOK */
                close( fd );
                return ENOMEM;
            }

            ptr = *attr_buff + offset;
        }

        /* Now copy the data for this attribute into the buffer. */
        strcpy( ptr, fa_ent->d_name );
        ptr += entname_size;

  memcpy( ptr, &fa_info, sizeof( struct attr_info ) );
        ptr += sizeof( struct attr_info );

        if( fa_info.ai_size > 0 ) {
            ssize_t read_bytes = read_attr( fd, fa_ent->d_name, fa_info.ai_type, ptr, 0, fa_info.ai_size );
            if( read_bytes != fa_info.ai_size ) {
                /* print a warning about mismatched sizes */
                char buff[80];
                sprintf( buff, "read %d, expected %d", read_bytes, (ssize_t)fa_info.ai_size );
                zipwarn( "attribute size mismatch: ", buff );
            }

            ptr += fa_info.ai_size;
        }
    }

    /* ----------------------------------------------------------------- */
    /* Close the attribute directory.                                    */
    retval = close_attrdir( fa_dir );
    /* TODO: check retval != EOK */

    /* ----------------------------------------------------------------- */
    /* If the buffer is too big, shrink it.                              */
    if( attrs_size < INITIAL_BUFF_SIZE ) {
        *attr_buff = (char *)realloc( *attr_buff, attrs_size );
        if( *attr_buff == NULL ) {
            close( fd );
            return ENOMEM;
        }
    }

    *total_size = attrs_size;

    close( fd );

    return EOK;
}

/* ---------------------------------------------------------------------- */
/* Add a 'UT' extra field to the zlist data pointed to by z.              */

#define EB_L_UT_SIZE    (EB_HEADSIZE + EB_UT_LEN(2))
#define EB_C_UT_SIZE    (EB_HEADSIZE + EB_UT_LEN(1))

local int add_UT_ef( struct zlist far *z )
{
    char        *l_ef = NULL;
    char        *c_ef = NULL;
    struct stat  s;

#ifdef IZ_CHECK_TZ
    if (!zp_tz_is_valid)
        return ZE_OK;           /* skip silently if no valid TZ info */
#endif

    /* We can't work if there's no entry to work on. */
    if( z == NULL ) {
        return ZE_LOGIC;
    }

    /* Check to make sure we've got enough room in the extra fields. */
    if( z->ext + EB_L_UT_SIZE > USHRT_MAX ||
        z->cext + EB_C_UT_SIZE > USHRT_MAX ) {
        return ZE_MEM;
    }

    /* stat() the file (or the symlink) to get the data; if we can't get */
    /* the data, there's no point in trying to fill out the fields.      */
    if(LSSTAT( z->name, &s ) ) {
        return ZE_OPEN;
    }

    /* Allocate memory for the local and central extra fields. */
    if( z->extra && z->ext != 0 ) {
        l_ef = (char *)realloc( z->extra, z->ext + EB_L_UT_SIZE );
    } else {
        l_ef = (char *)malloc( EB_L_UT_SIZE );
        z->ext = 0;
    }
    if( l_ef == NULL ) {
        return ZE_MEM;
    }
    z->extra = l_ef;
    l_ef += z->ext;

    if( z->cextra && z->cext != 0 ) {
        c_ef = (char *)realloc( z->cextra, z->cext + EB_C_UT_SIZE );
    } else {
        c_ef = (char *)malloc( EB_C_UT_SIZE );
        z->cext = 0;
    }
    if( c_ef == NULL ) {
        return ZE_MEM;
    }
    z->cextra = c_ef;
    c_ef += z->cext;

    /* Now add the local version of the field. */
    *l_ef++ = 'U';
    *l_ef++ = 'T';
    *l_ef++ = (char)(EB_UT_LEN(2)); /* length of data in local EF */
    *l_ef++ = (char)0;
    *l_ef++ = (char)(EB_UT_FL_MTIME | EB_UT_FL_ATIME);
    *l_ef++ = (char)(s.st_mtime);
    *l_ef++ = (char)(s.st_mtime >> 8);
    *l_ef++ = (char)(s.st_mtime >> 16);
    *l_ef++ = (char)(s.st_mtime >> 24);
    *l_ef++ = (char)(s.st_atime);
    *l_ef++ = (char)(s.st_atime >> 8);
    *l_ef++ = (char)(s.st_atime >> 16);
    *l_ef++ = (char)(s.st_atime >> 24);

    z->ext += EB_L_UT_SIZE;

    /* Now add the central version. */
    memcpy(c_ef, l_ef-EB_L_UT_SIZE, EB_C_UT_SIZE);
    c_ef[EB_LEN] = (char)(EB_UT_LEN(1)); /* length of data in central EF */

    z->cext += EB_C_UT_SIZE;

    return ZE_OK;
}

/* ---------------------------------------------------------------------- */
/* Add a 'Ux' extra field to the zlist data pointed to by z.              */

#define EB_L_UX2_SIZE   (EB_HEADSIZE + EB_UX2_MINLEN)
#define EB_C_UX2_SIZE   (EB_HEADSIZE)

local int add_Ux_ef( struct zlist far *z )
{
    char        *l_ef = NULL;
    char        *c_ef = NULL;
    struct stat  s;

    /* Check to make sure we've got enough room in the extra fields. */
    if( z->ext + EB_L_UX2_SIZE > USHRT_MAX ||
        z->cext + EB_C_UX2_SIZE > USHRT_MAX ) {
        return ZE_MEM;
    }

    /* stat() the file (or the symlink) to get the data; if we can't get */
    /* the data, there's no point in trying to fill out the fields.      */
    if(LSSTAT( z->name, &s ) ) {
        return ZE_OPEN;
    }

    /* Allocate memory for the local and central extra fields. */
    if( z->extra && z->ext != 0 ) {
        l_ef = (char *)realloc( z->extra, z->ext + EB_L_UX2_SIZE );
    } else {
        l_ef = (char *)malloc( EB_L_UX2_SIZE );
        z->ext = 0;
    }
    if( l_ef == NULL ) {
        return ZE_MEM;
    }
    z->extra = l_ef;
    l_ef += z->ext;

    if( z->cextra && z->cext != 0 ) {
        c_ef = (char *)realloc( z->cextra, z->cext + EB_C_UX2_SIZE );
    } else {
        c_ef = (char *)malloc( EB_C_UX2_SIZE );
        z->cext = 0;
    }
    if( c_ef == NULL ) {
        return ZE_MEM;
    }
    z->cextra = c_ef;
    c_ef += z->cext;

    /* Now add the local version of the field. */
    *l_ef++ = 'U';
    *l_ef++ = 'x';
    *l_ef++ = (char)(EB_UX2_MINLEN);
    *l_ef++ = (char)(EB_UX2_MINLEN >> 8);
    *l_ef++ = (char)(s.st_uid);
    *l_ef++ = (char)(s.st_uid >> 8);
    *l_ef++ = (char)(s.st_gid);
    *l_ef++ = (char)(s.st_gid >> 8);

    z->ext += EB_L_UX2_SIZE;

    /* Now add the central version of the field. */
    *c_ef++ = 'U';
    *c_ef++ = 'x';
    *c_ef++ = 0;
    *c_ef++ = 0;

    z->cext += EB_C_UX2_SIZE;

    return ZE_OK;
}

/* ---------------------------------------------------------------------- */
/* Add a 'At' extra field to the zlist data pointed to by z.              */

#define EB_L_AT_SIZE    (EB_HEADSIZE + EB_L_AT_LEN) /* + attr size */
#define EB_C_AT_SIZE    (EB_HEADSIZE + EB_C_AT_LEN)

#define MEMCOMPRESS_HEADER      6   /* ush compression type, ulg CRC */
#define DEFLAT_WORSTCASE_ADD    5   /* byte blocktype, 2 * ush blocklength */
#define MEMCOMPRESS_OVERHEAD    (MEMCOMPRESS_HEADER + DEFLAT_WORSTCASE_ADD)

local int add_At_ef( struct zlist far *z )
{
    char *l_ef       = NULL;
    char *c_ef       = NULL;
    char *attrbuff   = NULL;
    off_t attrsize   = 0;
    char *compbuff   = NULL;
    ush   compsize   = 0;
    uch   flags      = 0;

    /* Check to make sure we've got enough room in the extra fields. */
    if( z->ext + EB_L_AT_SIZE > USHRT_MAX ||
        z->cext + EB_C_AT_SIZE > USHRT_MAX ) {
        return ZE_MEM;
    }

    /* Attempt to load up a buffer full of the file's attributes. */
    {
        if (get_attr_dir( z->name, &attrbuff, &attrsize) != EOK ) {
            return ZE_OPEN;
        }
        if (attrsize == 0) {
            return ZE_OK;
        }
        if (attrbuff == NULL) {
            return ZE_LOGIC;
        }

        /* Check for way too much data. */
        if (attrsize > (off_t)ULONG_MAX) {
            zipwarn( "uncompressed attributes truncated", "" );
            attrsize = (off_t)(ULONG_MAX - MEMCOMPRESS_OVERHEAD);
        }
    }

    if (verbose) {
        printf( "\t[in=%lu]", (unsigned long)attrsize );
    }

    /* Try compressing the data */
    compbuff = (char *)malloc( (size_t)attrsize + MEMCOMPRESS_OVERHEAD );
    if( compbuff == NULL ) {
        return ZE_MEM;
    }
    compsize = memcompress( compbuff,
                            (size_t)attrsize + MEMCOMPRESS_OVERHEAD,
                            attrbuff,
                            (size_t)attrsize );
    if (verbose) {
        printf( " [out=%u]", compsize );
    }

    /* Attempt to optimise very small attributes. */
    if (compsize > attrsize) {
        free( compbuff );
        compsize = (ush)attrsize;
        compbuff = attrbuff;

        flags = EB_AT_FL_NATURAL;
    }

    /* Check to see if we really have enough room in the EF for the data. */
    if( ( z->ext + compsize + EB_L_AT_LEN ) > USHRT_MAX ) {
        compsize = USHRT_MAX - EB_L_AT_LEN - z->ext;
    }

    /* Allocate memory for the local and central extra fields. */
    if( z->extra && z->ext != 0 ) {
        l_ef = (char *)realloc( z->extra, z->ext + EB_L_AT_SIZE + compsize );
    } else {
        l_ef = (char *)malloc( EB_L_AT_SIZE + compsize );
        z->ext = 0;
    }
    if( l_ef == NULL ) {
        return ZE_MEM;
    }
    z->extra = l_ef;
    l_ef += z->ext;

    if( z->cextra && z->cext != 0 ) {
        c_ef = (char *)realloc( z->cextra, z->cext + EB_C_AT_SIZE );
    } else {
        c_ef = (char *)malloc( EB_C_AT_SIZE );
        z->cext = 0;
    }
    if( c_ef == NULL ) {
        return ZE_MEM;
    }
    z->cextra = c_ef;
    c_ef += z->cext;

    /* Now add the local version of the field. */
    *l_ef++ = 'A';
    *l_ef++ = 't';
    *l_ef++ = (char)(compsize + EB_L_AT_LEN);
    *l_ef++ = (char)((compsize + EB_L_AT_LEN) >> 8);
    *l_ef++ = (char)((unsigned long)attrsize);
    *l_ef++ = (char)((unsigned long)attrsize >> 8);
    *l_ef++ = (char)((unsigned long)attrsize >> 16);
    *l_ef++ = (char)((unsigned long)attrsize >> 24);
    *l_ef++ = flags;
    memcpy( l_ef, compbuff, (size_t)compsize );

    z->ext += EB_L_AT_SIZE + compsize;

    /* And the central version. */
    *c_ef++ = 'A';
    *c_ef++ = 't';
    *c_ef++ = (char)(EB_C_AT_LEN);
    *c_ef++ = (char)(EB_C_AT_LEN >> 8);
    *c_ef++ = (char)compsize;
    *c_ef++ = (char)(compsize >> 8);
    *c_ef++ = (char)(compsize >> 16);
    *c_ef++ = (char)(compsize >> 24);
    *c_ef++ = flags;

    z->cext += EB_C_AT_SIZE;

    return ZE_OK;
}

/* Extra field info:
   - 'UT' - UNIX time extra field
   - 'Ux' - UNIX uid/gid extra field
   - 'At' - AtheOS file attributes extra field

   This is done the same way ../unix/unix.c stores the 'UT'/'Ux' fields
   (full data in local header, only modification time in central header),
   with the 'At' field added to the end and the size of the 'At' field
   in the central header.

   See the end of atheos/osdep.h for a simple explanation of the 'At' EF
   layout.
 */
int set_extra_field(z, z_utim)
  struct zlist far *z;
  iztimes *z_utim;
  /* store full data in local header but just modification time stamp info
     in central header */
{
    int retval;

    /* Check to make sure z is valid. */
    if( z == NULL ) {
        return ZE_LOGIC;
    }

    retval = add_UT_ef(z);
    if( retval != ZE_OK ) {
        return retval;
    }

    retval = add_Ux_ef(z);
    if( retval != ZE_OK ) {
        return retval;
    }

    return add_At_ef(z); /* last function; we can use return value directly */
}

/* ---------------------------------------------------------------------- */
/* Set a file's MIME type.                                                */
void setfiletype(const char *file, const char *type)
{
    int fd;
    off_t nLen;
    ssize_t nError;

    fd = open( file, O_RDWR );

    if (fd < 0) {
        zipwarn( "can't open zipfile to write file type", "" );
    }

    else
    {
        nLen = strlen( type );
        /* FIXME: write_attr() should return count of writed bytes */
        nError = write_attr( fd, "os::MimeType", O_TRUNC, ATTR_TYPE_STRING, type, 0, nLen );
        if (nError < 0) {
            zipwarn( "couldn't write complete file type", "" );
        }
        close( fd );
    }
}

#endif /* !UTIL */

/******************************/
/*  Function version_local()  */
/******************************/

void version_local()
{
    static ZCONST char CompiledWith[] = "Compiled with %s%s for %s%s%s%s.\n\n";

    printf(CompiledWith,

#ifdef __GNUC__
      "gcc ", __VERSION__,
#else
      "(unknown compiler)", "",
#endif

      "Syllable",

#if defined(i486) || defined(__i486) || defined(__i486__) || defined(i386) || defined(__i386) || defined(__i386__)
      " (x86)",
#else
      " (unknown platform)",
#endif

#ifdef __DATE__
      " on ", __DATE__
#else
      "", ""
#endif
    );

} /* end function version_local() */
