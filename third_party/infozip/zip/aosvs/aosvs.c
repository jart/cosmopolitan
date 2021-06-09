/*
  Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 1999-Oct-05 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, both of these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
*/
#include <dirent.h>
#include <time.h>

#include "zip.h"
#include <paru.h>                 /* parameter definitions */
#include <sys_calls.h>            /* AOS/VS system call interface */
#include <packets/filestatus.h>   /* AOS/VS ?FSTAT packet defs */

#ifndef UTIL            /* AOS/VS specific fileio code not needed for UTILs */

#define PAD 0
#define PATH_END ':'

/*
 * could probably avoid the union -
 * all are same size & we're going to assume this
 */
typedef union zvsfstat_stru
{
     P_FSTAT        norm_fstat_packet;      /* normal fstat packet */
     P_FSTAT_DIR    dir_fstat_packet;       /* DIR/CPD fstat packet */
     P_FSTAT_UNIT   unit_fstat_packet;      /* unit (device) fstat packet */
     P_FSTAT_IPC    ipc_fstat_packet;       /* IPC file fstat packet */
} ZVSFSTAT_STRU;

typedef struct zextrafld
{
     char           extra_header_id[2]; /* set to VS - in theory, an int */
     char           extra_data_size[2]; /* size of rest, in Intel little-endian order */
     char           extra_sentinel[4];  /* set to FCI w/ trailing null */
     unsigned char  extra_rev;          /* set to 10 for rev 1.0 */
     ZVSFSTAT_STRU  fstat_packet;       /* the fstat packet */
     char           aclbuf[$MXACL];     /* raw ACL, or link-resolution name */
} ZEXTRAFLD;

#define ZEXTRA_HEADID   "VS"
#define ZEXTRA_SENTINEL "FCI"
#define ZEXTRA_REV      (unsigned char) 10

local ZEXTRAFLD   zzextrafld;         /* buffer for extra field containing
                                         ?FSTAT packet & ACL buffer */
local char        zlinkres[$MXPL];    /* buf for link resolution contents */
local char        znamebuf[$MXPL];    /* buf for AOS/VS filename */
static char     vsnamebuf[$MXPL];
static char     uxnamebuf[FNMAX];
static P_FSTAT  vsfstatbuf;

local ulg label_time = 0;
local ulg label_mode = 0;
local time_t label_utim = 0;

/* Local functions */
local char *readd OF((DIR *));

char *readd(d)
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
  if ((s.st_mode & S_IFDIR) == 0)
  {
    /* add or remove name of file */
    if ((m = newname(n, 0, caseflag)) != ZE_OK)
      return m;
  } else {
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
  } /* (s.st_mode & S_IFDIR) == 0) */
  return ZE_OK;
}

char *strlower(s)
char *s;                /* string to convert */
/* Convert all uppercase letters to lowercase in string s */
{
  char *p;              /* scans string */

  for (p = s; *p; p++)
    if (*p >= 'A' && *p <= 'Z')
      *p += 'a' - 'A';
  return s;
}

char *strupper(s)
char *s;                /* string to convert */
/* Convert all lowercase letters to uppercase in string s */
{
  char *p;              /* scans string */

  for (p = s; *p; p++)
    if (*p >= 'a' && *p <= 'z')
      *p -= 'a' - 'A';
  return s;
}

char *ex2in(x, isdir, pdosflag)
char *x;                /* external file name */
int isdir;              /* input: x is a directory */
int *pdosflag;          /* output: force MSDOS file attributes? */
/* Convert the external file name to a zip file name, returning the malloc'ed
   string or NULL if not enough memory. */
{
  char *n;              /* internal file name (malloc'ed) */
  char *t;              /* shortened name */
  int dosflag;

  dosflag = dosify; /* default for non-DOS and non-OS/2 */

  /* Find starting point in name before doing malloc */
  for (t = x; *t == '/'; t++)
    ;

  if (*t == '=')                /* AOS/VS for ./ */
    t++;
  else if (*t == ':')           /* AOS/VS for / */
    t++;

  if (!pathput)
    t = last(t, PATH_END);

  if (*t == '^')        /* AOS/VS for ../ */
  {
    if ((n = malloc(strlen(t) + 3)) == NULL)
      return NULL;
    strcpy(n, "../");
    strcpy(n + 3, t + 1);
  }
  else if (*t == '@')   /* AOS/VS for :PER:, kind of like /dev/ */
  {
    if ((n = malloc(strlen(t) + 5)) == NULL)
      return NULL;
    strcpy(n, "/PER/");
    strcpy(n + 5, t + 1);
  }
  else
  {
    if ((n = malloc(strlen(t) + 1)) == NULL)
      return NULL;
    strcpy(n, t);
  }
  /* now turn AOS/VS dir separators (colons) into slashes */
  for (t = n;  *t != '\0';  t++)
    if (*t == ':')
      *t = '/';
  /*
   * Convert filename to uppercase (for correct matching).
   * (It may make more sense to patch the matching code, since
   * we may want those filenames in uppercase on the target system,
   * but this seems better at present.  If we're converting, uppercase
   * also seems to make sense.)
   */
  strupper(n);


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

void stamp(f, d)
char *f;                /* name of file to change */
ulg d;                  /* dos-style time to change it to */
/* Set last updated and accessed time of file f to the DOS time d. */
{
  time_t u[2];          /* argument for utime() */

  /* Convert DOS time to time_t format in u */
  u[0] = u[1] = dos2unixtime(d);
  utime(f, u);
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
  /* convert FNMAX to malloc - 11/8/04 EG */
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
  if ((name = malloc(len + 1)) == NULL) {
    ZIPERR(ZE_MEM, "filetime");
  }
  strcpy(name, f);
  if (name[len - 1] == '/')
    name[len - 1] = '\0';
  /* not all systems allow stat'ing a file with / appended */

  if (strcmp(f, "-") == 0) {
    if (fstat(fileno(stdin), &s) != 0)
      error("fstat(stdin)");
  } else if (LSSTAT(name, &s) != 0) {
             /* Accept about any file kind including directories
              * (stored with trailing / with -r option)
              */
    free(name);
    return 0;
  }
  free(name);

  if (a != NULL) {
    *a = ((ulg)s.st_mode << 16) | !(s.st_mode & S_IWRITE);
    if ((s.st_mode & S_IFDIR) != 0) {
      *a |= MSDOS_DIR_ATTR;
    }
  }
  if (n != NULL)
    *n = (s.st_mode & S_IFMT) == S_IFREG ? s.st_size : -1L;
  if (t != NULL) {
    t->atime = s.st_atime;
    t->mtime = s.st_mtime;
    t->ctime = s.st_ctime;
  }

  return unix2dostime(&s.st_ctime);
}

int deletedir(d)
char *d;
{
   return rmdir(d);
}

int set_extra_field(z, z_utim)
  struct zlist far *z;
  iztimes *z_utim;
  /* create extra field and change z->att if desired */
  /* NOTE: this AOS/VS version assumes the pathname in z->name is an
   * AOS/VS pathname, not a unix-style one.  Since you can zip up using
   * unix-style pathnames, this may create problems occasionally.
   * We COULD add code to parse back to AOS/VS format ...
   * (This might also fail for other reasons such as access denied, but
   * that should already have occurred.)
   * We set the central-dir extra fld pointer & length here to the same data.
   */
{
  int             aclend = 0;
/*
 * use this to simplify because different calls depending on
 * whether links are resolved
 */
  unsigned short  errc;

  z->ext = 0;               /* init to no extra field */
/* get the ?FSTAT info & the acl - if no errors, get memory & store.
 * (first, we have to cut off the trailing slash that was added if
 * it's a dir, since AOS/VS doesn't accept that kind of thing)
 */
  strncpy(znamebuf, z->name, $MXPL);
  znamebuf[$MXPL-1] = '\0';
  if (znamebuf[strlen(znamebuf)-1] == '/')
    znamebuf[strlen(znamebuf)-1] = '\0';
  if (linkput)
    errc = sys_fstat(znamebuf, BIT1, &(zzextrafld.fstat_packet));
  else
    errc = sys_fstat(znamebuf, 0, &(zzextrafld.fstat_packet));
  if (errc)
  {
    fprintf(stderr,
            "\n    Warning: can't get ?FSTAT info & acl of %s - error %d\n    ",
            znamebuf, errc);
    perror("sys_fstat()");
  }
  else
  {
    /* store the ACL - or, if a link (no ACL!), store the resolution name */
    if (zzextrafld.fstat_packet.norm_fstat_packet.styp_type != $FLNK)
    {
      if ((errc = sys_gacl(znamebuf, zzextrafld.aclbuf)) != 0)
      {
        fprintf(stderr, "\n    Warning: can't get acl of %s - error %d\n    ",
                z->name, errc);
        perror("sys_gacl()");
      }
      else
      {
        /* find length of ACL - ends with double-null */
        while (aclend++ < $MXACL  &&
               (zzextrafld.aclbuf[aclend - 1] != '\0'  ||
                zzextrafld.aclbuf[aclend] != '\0'))
          /* EMPTY LOOP */ ;
        if ((z->cextra = z->extra =
             malloc(sizeof(ZEXTRAFLD) - $MXACL + aclend + 4)) != NULL)
        {
          strncpy(zzextrafld.extra_header_id, ZEXTRA_HEADID,
                  sizeof(zzextrafld.extra_header_id));
          strncpy(zzextrafld.extra_sentinel, ZEXTRA_SENTINEL,
                  sizeof(zzextrafld.extra_sentinel));
          zzextrafld.extra_rev = ZEXTRA_REV;    /* this is a char, no need
                                                   to worry about byte order */
          /* set size (Intel (little-endian)) 2-byte int, which we've set
             as array to make it easier */
          errc = (unsigned short) (sizeof(ZEXTRAFLD) - $MXACL + aclend + 4 -
                                   sizeof(zzextrafld.extra_header_id) -
                                   sizeof(zzextrafld.extra_data_size));
          zzextrafld.extra_data_size[0] = errc & 0xFF;  /* low-order byte */
          zzextrafld.extra_data_size[1] = errc >> 8;    /* high-order byte */
          memcpy((char *) z->extra, (char *) &zzextrafld,
                 sizeof(ZEXTRAFLD) - $MXACL + aclend + 4);
          z->cext = z->ext = sizeof(ZEXTRAFLD) - $MXACL + aclend + 4;
        }
      }
    }
    else /* a link */
    {
      if ((errc = sys_glink(z->name, zzextrafld.aclbuf)) != 0)
      {
        fprintf(stderr,
              "\n    Warning: can't get link-resolution of %s - error %d\n    ",
                z->name, errc);
        perror("sys_glink()");
      }
      else
      {
        aclend = strlen(zzextrafld.aclbuf) + 1;
        if ((z->extra = malloc(sizeof(ZEXTRAFLD) - $MXACL + aclend + 4))
            != NULL)
        {
          strncpy(zzextrafld.extra_header_id, ZEXTRA_HEADID,
                  sizeof(zzextrafld.extra_header_id));
          strncpy(zzextrafld.extra_sentinel, ZEXTRA_SENTINEL,
                  sizeof(zzextrafld.extra_sentinel));
          zzextrafld.extra_rev = ZEXTRA_REV;    /* this is a char, no need
                                                   to worry about byte order */
          /* set size (Intel (little-endian)) 2-byte int, which we've set
             as array to make it easier */
          errc = (unsigned short) (sizeof(ZEXTRAFLD) - $MXACL + aclend + 4 -
                                   sizeof(zzextrafld.extra_header_id) -
                                   sizeof(zzextrafld.extra_data_size));
          zzextrafld.extra_data_size[0] = errc & 0xFF;  /* low-order byte */
          zzextrafld.extra_data_size[1] = errc >> 8;    /* high-order byte */
          memcpy((char *) z->extra, (char *) &zzextrafld,
                 sizeof(ZEXTRAFLD) - $MXACL + aclend + 4);
          z->ext = sizeof(ZEXTRAFLD) - $MXACL + aclend + 4;
        }
      }
    }
  }
  return ZE_OK;
}

#endif /* !UTIL */

void version_local()
{
    printf("Compiled with %s under %s.\n",
      "a C compiler",
      "AOS/VS"
    );
}


/*
 * This file defines for AOS/VS two Unix functions relating to links;
 * the calling code should have the following defines:
 *
 *    #define       lstat(path,buf)             zvs_lstat(path,buf)
 *    #define       readlink(path,buf,nbytes)   zvs_readlink(path,buf,nbytes)
 *
 * For these functions, I'm going to define yet 2 MORE filename buffers
 * and also insert code to change pathnames to Unix & back.  This is
 * easier than changing all the other places this kind of thing happens to
 * be efficient.  This is a kludge.  I'm also going to put the functions
 * here for my immediate convenience rather than somewhere else for
 * someone else's.
 *
 * WARNING: the use of static buffers means that you'd better get your
 * data out of these buffers before the next call to any of these functions!
 *
 */

/* =========================================================================
 * ZVS_LSTAT() - get (or simulate) stat information WITHOUT following symlinks
 *      This is intended to look to the outside like the unix lstat()
 *      function.  We do a quick-&-dirty filename conversion.
 *
 *      If the file is NOT a symbolic link, we can just do a stat() on it and
 *      that should be fine.  But if it IS a link, we have to set the elements
 *      of the stat struct ourselves, since AOS/VS doesn't have a built-in
 *      lstat() function.
 *
 *      RETURNS: 0 on success, or -1 otherwise
 *
 */

int zvs_lstat(char *path, struct stat *buf)
{
    char        *cp_vs = vsnamebuf;
    char        *cp_ux = path;
    int         mm, dd, yy;

    /*
     * Convert the Unix pathname to an AOS/VS pathname.
     * This is quick & dirty; it won't handle (for instance) pathnames with
     * ../ in the middle of them, and may choke on other Unixisms.  We hope
     * they're unlikely.
     */
    if (!strncmp(cp_ux, "../", 3))
    {
        *cp_vs++ = '^';        /* AOS/VS for ../ */
        cp_ux += 3;
    }
    else if (!strncmp(cp_ux, "./", 2))
    {
        *cp_vs++ = '=';        /* AOS/VS for ./ */
        cp_ux += 2;
    }

    do
    {
        if (*cp_ux == '/')
        {
            *cp_vs++ = ':';
        }
        else
        {
            *cp_vs++ = (char) toupper(*cp_ux);
        }

    } while (*cp_ux++ != '\0'  &&  cp_vs - vsnamebuf < sizeof(vsnamebuf));

    /* If Unix name was too long for our buffer, return an error return */
    if (cp_vs - vsnamebuf >= sizeof(vsnamebuf)  &&  *(cp_vs - 1) != '\0')
        return (-1);     /* error */

    /* Make AOS/VS ?FSTAT call that won't follow links & see if we find
     * anything.  If not, we return error.
     */
    if (sys_fstat(vsnamebuf,
                  BIT1,                 /* BIT1 says to not resolve links */
                  &vsfstatbuf))
        return (-1);     /* error */

    /* If we DID find the file but it's not a link,
     * call stat() and return its value.
     */
    if (vsfstatbuf.styp_type != $FLNK)
        return (stat(path, buf));        /* call with Unix pathname ... */

    /* Otherwise, we have to kludge up values for the stat structure */
    memset((char *) buf, 0, sizeof(*buf));   /* init to nulls (0 values) */
    buf->st_mode = S_IFLNK | 0777;           /* link and rwxrwxrwx */
    buf->st_uid = -1;                        /* this is what we get on AOS/VS
                                                anyway (maybe unless we set up
                                                a dummy password file?) */
    buf->st_nlink = 1;
    /* The DG date we've got is days since 12/31/67 and seconds/2.  So we
     * need to subtract 732 days (if that's not negative), convert to seconds,
     * and add adjusted seconds.
     */
    if (vsfstatbuf.stch.short_time[0] < 732)
        buf->st_ctime = buf->st_mtime = buf->st_atime = 0L;
    else
    {
        buf->st_ctime = buf->st_mtime = buf->st_atime =
                ((long) vsfstatbuf.stch.short_time[0] - 732L) * 24L * 3600L +
                2L * (long) vsfstatbuf.stch.short_time[1];
    }

    /* And we need to get the filename linked to and use its length as
     * the file size.  We'll use the Unix pathname buffer for this - hope
     * it's big enough.  (We won't overwrite anything, but we could get a
     * truncated path.)  If there's an error, here's our last chance to
     * say anything.
     */
    if ((buf->st_size = zvs_readlink(vsnamebuf, uxnamebuf, FNMAX)) < 0)
        return (-1);
    else
        return (0);

} /* end zvs_lstat() */

/* =========================================================================
 * ZVS_READLINK() - get pathname pointed to by an AOS/VS link file
 *      This is intended to look to the outside like the unix readlink()
 *      function.  We do a quick-&-dirty filename conversion.
 *
 *      RETURNS: the length of the output path (in bytes), or -1 if an error
 *
 */

int zvs_readlink(char *path, char *buf, int nbytes)
{
    char    *cp_vs = vsnamebuf;
    char    *cp_ux = buf;

    /* This is called with z->name, the filename the user gave, so we'll get
     * the link-resolution name on the assumption that it's a valid AOS/VS
     * name. We're also assuming a reasonable value (> 5) for nbytes.
     */
    if (sys_glink(path, vsnamebuf))
        return (-1);     /* readlink() is supposed to return -1 on error */

    /* Now, convert the AOS/VS pathname to a Unix pathname.
     * Note that sys_glink(), unlike readlink(), does add a null.
     */
    if (*cp_vs == '^')        /* AOS/VS for ../ */
    {
        strncpy(cp_ux, "../", 3);
        cp_ux += 3;
        cp_vs++;
    }
    else if (*cp_vs == '@')   /* AOS/VS for :PER:, kind of like /dev/ */
    {
        strncpy(cp_ux, "/PER/", 5);
        cp_ux += 5;
        cp_vs++;
    }
    else if (*cp_vs == '=')   /* AOS/VS for ./ */
    {
        strncpy(cp_ux, "./", 2);
        cp_ux += 2;
        cp_vs++;
    }
    while (*cp_vs != '\0'  &&  cp_ux - buf < nbytes)
    {
        if (*cp_vs == ':')
        {
            *cp_ux++ = '/';
        }
        else
        {
            *cp_ux++ = (char) toupper(*cp_vs);
        }
        cp_vs++;
    }

    return (cp_ux - buf);   /* # characters in Unix path (no trailing null) */

} /* end zvs_readlink() */
