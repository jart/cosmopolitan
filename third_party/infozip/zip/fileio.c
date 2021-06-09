/*
  fileio.c - Zip 3

  Copyright (c) 1990-2008 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2007-Mar-4 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*
 *  fileio.c by Mark Adler
 */
#define __FILEIO_C

#include "zip.h"
#include "crc32.h"

#ifdef MACOS
#  include "helpers.h"
#endif

#ifdef VMS
#  include "vms/vms.h"
#endif /* def VMS */

#include <time.h>

#ifdef NO_MKTIME
time_t mktime OF((struct tm *));
#endif

#ifdef OSF
#define EXDEV 18   /* avoid a bug in the DEC OSF/1 header files. */
#else
#include <errno.h>
#endif

#ifdef NO_ERRNO
extern int errno;
#endif

/* -----------------------
   For long option support
   ----------------------- */
#include <ctype.h>


#if defined(VMS) || defined(TOPS20)
#  define PAD 5
#else
#  define PAD 0
#endif

#ifdef NO_RENAME
int rename OF((ZCONST char *, ZCONST char *));
#endif


/* Local functions */
local int optionerr OF((char *, ZCONST char *, int, int));
local unsigned long get_shortopt OF((char **, int, int *, int *, char **, int *, int));
local unsigned long get_longopt OF((char **, int, int *, int *, char **, int *, int));

#ifdef UNICODE_SUPPORT
local int utf8_char_bytes OF((ZCONST char *utf8));
local long ucs4_char_from_utf8 OF((ZCONST char **utf8 ));
local int utf8_from_ucs4_char OF((char *utf8buf, ulg ch));
local int utf8_to_ucs4_string OF((ZCONST char *utf8, ulg *usc4buf,
                                  int buflen));
local int ucs4_string_to_utf8 OF((ZCONST ulg *ucs4, char *utf8buf,
                                  int buflen));
#if 0
  local int utf8_chars OF((ZCONST char *utf8));
#endif
#endif /* UNICODE_SUPPORT */

#ifndef UTIL    /* the companion #endif is a bit of ways down ... */

local int fqcmp  OF((ZCONST zvoid *, ZCONST zvoid *));
local int fqcmpz OF((ZCONST zvoid *, ZCONST zvoid *));


/* Local module level variables. */
char *label = NULL;                /* global, but only used in `system'.c */
local z_stat zipstatb;             /* now use z_stat globally - 7/24/04 EG */
#if defined(UNICODE_SUPPORT) && defined(WIN32)
 local zw_stat zipstatbw;
#endif
#if (!defined(MACOS) && !defined(WINDLL))
local int zipstate = -1;
#else
int zipstate;
#endif
/* -1 unknown, 0 old zip file exists, 1 new zip file */

#if 0
char *getnam(n, fp)
char *n;                /* where to put name (must have >=FNMAX+1 bytes) */
#endif

/* converted to return string pointer from malloc to avoid
   size limitation - 11/8/04 EG */
#define GETNAM_MAX 9000 /* hopefully big enough for now */
char *getnam(fp)
  FILE *fp;
  /* Read a \n or \r delimited name from stdin into n, and return
     n.  If EOF, then return NULL.  Also, if problem return NULL. */
{
  char name[GETNAM_MAX + 1];
  int c;                /* last character read */
  char *p;              /* pointer into name area */


  p = name;
  while ((c = getc(fp)) == '\n' || c == '\r')
    ;
  if (c == EOF)
    return NULL;
  do {
    if (p - name >= GETNAM_MAX)
      return NULL;
    *p++ = (char) c;
    c = getc(fp);
  } while (c != EOF && (c != '\n' && c != '\r'));
#ifdef WIN32
/*
 * WIN32 strips off trailing spaces and periods in filenames
 * XXX what about a filename that only consists of spaces ?
 *     Answer: on WIN32, a filename must contain at least one non-space char
 */
  while (p > name) {
    if ((c = p[-1]) != ' ' && c != '.')
      break;
    --p;
  }
#endif
  *p = 0;
  /* malloc a copy */
  if ((p = malloc(strlen(name) + 1)) == NULL) {
    return NULL;
  }
  strcpy(p, name);
  return p;
}

struct flist far *fexpel(f)
struct flist far *f;    /* entry to delete */
/* Delete the entry *f in the doubly-linked found list.  Return pointer to
   next entry to allow stepping through list. */
{
  struct flist far *t;  /* temporary variable */

  t = f->nxt;
  *(f->lst) = t;                        /* point last to next, */
  if (t != NULL)
    t->lst = f->lst;                    /* and next to last */
  if (f->name != NULL)                  /* free memory used */
    free((zvoid *)(f->name));
  if (f->zname != NULL)
    free((zvoid *)(f->zname));
  if (f->iname != NULL)
    free((zvoid *)(f->iname));
#ifdef UNICODE_SUPPORT
  if (f->uname)
    free((zvoid *)f->uname);
# ifdef WIN32
  if (f->namew)
    free((zvoid *)f->namew);
  if (f->inamew)
    free((zvoid *)f->inamew);
  if (f->znamew)
    free((zvoid *)f->znamew);
# endif
#endif
  farfree((zvoid far *)f);
  fcount--;                             /* decrement count */
  return t;                             /* return pointer to next */
}

local int fqcmp(a, b)
  ZCONST zvoid *a, *b;          /* pointers to pointers to found entries */
/* Used by qsort() to compare entries in the found list by name. */
{
  return strcmp((*(struct flist far **)a)->name,
                (*(struct flist far **)b)->name);
}

local int fqcmpz(a, b)
  ZCONST zvoid *a, *b;          /* pointers to pointers to found entries */
/* Used by qsort() to compare entries in the found list by iname. */
{
  return strcmp((*(struct flist far **)a)->iname,
                (*(struct flist far **)b)->iname);
}

char *last(p, c)
  char *p;                /* sequence of path components */
  int c;                  /* path components separator character */
/* Return a pointer to the start of the last path component. For a directory
 * name terminated by the character in c, the return value is an empty string.
 */
{
  char *t;              /* temporary variable */

  if ((t = strrchr(p, c)) != NULL)
    return t + 1;
  else
#ifndef AOS_VS
    return p;
#else
/* We want to allow finding of end of path in either AOS/VS-style pathnames
 * or Unix-style pathnames.  This presents a few little problems ...
 */
  {
    if (*p == '='  ||  *p == '^')      /* like ./ and ../ respectively */
      return p + 1;
    else
      return p;
  }
#endif
}

#if defined(UNICODE_SUPPORT) && defined(WIN32)
wchar_t *lastw(pw, c)
  wchar_t *pw;            /* sequence of path components */
  wchar_t c;              /* path components separator character */
/* Return a pointer to the start of the last path component. For a directory
 * name terminated by the character in c, the return value is an empty string.
 */
{
  wchar_t *tw;            /* temporary variable */

  if ((tw = wcsrchr(pw, c)) != NULL)
    return tw + 1;
  else
# ifndef AOS_VS
    return pw;
# else
/* We want to allow finding of end of path in either AOS/VS-style pathnames
 * or Unix-style pathnames.  This presents a few little problems ...
 */
  {
    if (*pw == (wchar_t)'='  ||  *pw == (wchar_t)'^')      /* like ./ and ../ respectively */
      return pw + 1;
    else
      return pw;
  }
# endif
}
#endif


char *msname(n)
  char *n;
/* Reduce all path components to MSDOS upper case 8.3 style names. */
{
  int c;                /* current character */
  int f;                /* characters in current component */
  char *p;              /* source pointer */
  char *q;              /* destination pointer */

  p = q = n;
  f = 0;
  while ((c = (unsigned char)*POSTINCSTR(p)) != 0)
    if (c == ' ' || c == ':' || c == '"' || c == '*' || c == '+' ||
        c == ',' || c == ';' || c == '<' || c == '=' || c == '>' ||
        c == '?' || c == '[' || c == ']' || c == '|')
      continue;                         /* char is discarded */
    else if (c == '/')
    {
      *POSTINCSTR(q) = (char)c;
      f = 0;                            /* new component */
    }
#ifdef __human68k__
    else if (ismbblead(c) && *p)
    {
      if (f == 7 || f == 11)
        f++;
      else if (*p && f < 12 && f != 8)
      {
        *q++ = c;
        *q++ = *p++;
        f += 2;
      }
    }
#endif /* __human68k__ */
    else if (c == '.')
    {
      if (f == 0)
        continue;                       /* leading dots are discarded */
      else if (f < 9)
      {
        *POSTINCSTR(q) = (char)c;
        f = 9;                          /* now in file type */
      }
      else
        f = 12;                         /* now just excess characters */
    }
    else
      if (f < 12 && f != 8)
      {
        f += CLEN(p);                   /* do until end of name or type */
        *POSTINCSTR(q) = (char)(to_up(c));
      }
  *q = 0;
  return n;
}

#ifdef UNICODE_SUPPORT
wchar_t *msnamew(nw)
  wchar_t *nw;
/* Reduce all path components to MSDOS upper case 8.3 style names. */
{
  wchar_t c;            /* current character */
  int f;                /* characters in current component */
  wchar_t *pw;          /* source pointer */
  wchar_t *qw;          /* destination pointer */

  pw = qw = nw;
  f = 0;
  while ((c = (unsigned char)*pw++) != 0)
    if (c == ' ' || c == ':' || c == '"' || c == '*' || c == '+' ||
        c == ',' || c == ';' || c == '<' || c == '=' || c == '>' ||
        c == '?' || c == '[' || c == ']' || c == '|')
      continue;                         /* char is discarded */
    else if (c == '/')
    {
      *qw++ = c;
      f = 0;                            /* new component */
    }
#ifdef __human68k__
    else if (ismbblead(c) && *pw)
    {
      if (f == 7 || f == 11)
        f++;
      else if (*pw && f < 12 && f != 8)
      {
        *qw++ = c;
        *qw++ = *pw++;
        f += 2;
      }
    }
#endif /* __human68k__ */
    else if (c == '.')
    {
      if (f == 0)
        continue;                       /* leading dots are discarded */
      else if (f < 9)
      {
        *qw++ = c;
        f = 9;                          /* now in file type */
      }
      else
        f = 12;                         /* now just excess characters */
    }
    else
      if (f < 12 && f != 8)
      {
        f++;                            /* do until end of name or type */
        *qw++ = towupper(c);
      }
  *qw = 0;
  return nw;
}
#endif


int proc_archive_name(n, caseflag)
  char *n;             /* name to process */
  int caseflag;         /* true to force case-sensitive match */
/* Process a name or sh expression in existing archive to operate
   on (or exclude).  Return an error code in the ZE_ class. */
{
  int m;                /* matched flag */
  char *p;              /* path for recursion */
  struct zlist far *z;  /* steps through zfiles list */

  if (strcmp(n, "-") == 0) {   /* if compressing stdin */
    zipwarn("Cannot select stdin when selecting archive entries", "");
    return ZE_MISS;
  }
  else
  {
    /* Search for shell expression in zip file */
    p = ex2in(n, 0, (int *)NULL);       /* shouldn't affect matching chars */
    m = 1;
    for (z = zfiles; z != NULL; z = z->nxt) {
      if (MATCH(p, z->iname, caseflag))
      {
        z->mark = pcount ? filter(z->zname, caseflag) : 1;
        if (verbose)
            fprintf(mesg, "zip diagnostic: %scluding %s\n",
               z->mark ? "in" : "ex", z->oname);
        m = 0;
      }
    }
#ifdef UNICODE_SUPPORT
    /* also check escaped Unicode names */
    for (z = zfiles; z != NULL; z = z->nxt) {
      if (z->zuname) {
#ifdef WIN32
        /* It seems something is lost in going from a listed
           name from zip -su in a console window to using that
           name in a command line.  This kluge may fix it
           and just takes zuname, converts to oem (i.e. ouname),
           then converts it back which ends up not the same as
           started with.
         */
        char *zuname = z->wuname;
#else
        char *zuname = z->zuname;
#endif
        if (MATCH(p, zuname, caseflag))
        {
          z->mark = pcount ? filter(zuname, caseflag) : 1;
          if (verbose) {
              fprintf(mesg, "zip diagnostic: %scluding %s\n",
                 z->mark ? "in" : "ex", z->oname);
              fprintf(mesg, "     Escaped Unicode:  %s\n",
                 z->ouname);
          }
          m = 0;
        }
      }
    }
#endif
    free((zvoid *)p);
    return m ? ZE_MISS : ZE_OK;
  }
}


int check_dup()
/* Sort the found list and remove duplicates.
   Return an error code in the ZE_ class. */
{
  struct flist far *f;          /* steps through found linked list */
  extent j, k;                  /* indices for s */
  struct flist far **s;         /* sorted table */
  struct flist far **nodup;     /* sorted table without duplicates */

  /* sort found list, remove duplicates */
  if (fcount)
  {
    extent fl_size = fcount * sizeof(struct flist far *);
    if ((fl_size / sizeof(struct flist far *)) != fcount ||
        (s = (struct flist far **)malloc(fl_size)) == NULL)
      return ZE_MEM;
    for (j = 0, f = found; f != NULL; f = f->nxt)
      s[j++] = f;
    /* Check names as given (f->name) */
    qsort((char *)s, fcount, sizeof(struct flist far *), fqcmp);
    for (k = j = fcount - 1; j > 0; j--)
      if (strcmp(s[j - 1]->name, s[j]->name) == 0)
        /* remove duplicate entry from list */
        fexpel(s[j]);           /* fexpel() changes fcount */
      else
        /* copy valid entry into destination position */
        s[k--] = s[j];
    s[k] = s[0];                /* First entry is always valid */
    nodup = &s[k];              /* Valid entries are at end of array s */

    /* sort only valid items and check for unique internal names (f->iname) */
    qsort((char *)nodup, fcount, sizeof(struct flist far *), fqcmpz);
    for (j = 1; j < fcount; j++)
      if (strcmp(nodup[j - 1]->iname, nodup[j]->iname) == 0)
      {
        char tempbuf[FNMAX+4081];

        sprintf(errbuf, "  first full name: %s\n", nodup[j - 1]->name);
        sprintf(tempbuf, " second full name: %s\n", nodup[j]->name);
        strcat(errbuf, "                     ");
        strcat(errbuf, tempbuf);
#ifdef EBCDIC
        strtoebc(nodup[j]->iname, nodup[j]->iname);
#endif
        sprintf(tempbuf, "name in zip file repeated: %s", nodup[j]->iname);
        strcat(errbuf, "                     ");
        strcat(errbuf, tempbuf);
        if (pathput == 0) {
          strcat(errbuf, "\n                     this may be a result of using -j");
        }
#ifdef EBCDIC
        strtoasc(nodup[j]->iname, nodup[j]->iname);
#endif
        zipwarn(errbuf, "");
        return ZE_PARMS;
      }
    free((zvoid *)s);
  }
  return ZE_OK;
}

int filter(name, casesensitive)
  char *name;
  int casesensitive;
  /* Scan the -R, -i and -x lists for matches to the given name.
     Return TRUE if the name must be included, FALSE otherwise.
     Give precedence to -x over -i and -R.
     Note that if both R and i patterns are given then must
     have a match for both.
     This routine relies on the following global variables:
       patterns                 array of match pattern structures
       pcount                   total number of patterns
       icount                   number of -i patterns
       Rcount                   number of -R patterns
     These data are set up by the command line parsing code.
   */
{
   unsigned int n;
   int slashes;
   char *p, *q;
   /* without -i patterns, every name matches the "-i select rules" */
   int imatch = (icount == 0);
   /* without -R patterns, every name matches the "-R select rules" */
   int Rmatch = (Rcount == 0);

   if (pcount == 0) return TRUE;

   for (n = 0; n < pcount; n++) {
      if (!patterns[n].zname[0])        /* it can happen... */
         continue;
      p = name;
      switch (patterns[n].select) {
       case 'R':
         if (Rmatch)
            /* one -R match is sufficient, skip this pattern */
            continue;
         /* With -R patterns, if the pattern has N path components (that is,
            N-1 slashes), then we test only the last N components of name.
          */
         slashes = 0;
         for (q = patterns[n].zname; (q = MBSCHR(q, '/')) != NULL; MB_NEXTCHAR(q))
            slashes++;
         /* The name may have M path components (M-1 slashes) */
         for (q = p; (q = MBSCHR(q, '/')) != NULL; MB_NEXTCHAR(q))
            slashes--;
         /* Now, "slashes" contains the difference "N-M" between the number
            of path components in the pattern (N) and in the name (M).
          */
         if (slashes < 0)
            /* We found "M > N"
                --> skip the first (M-N) path components of the name.
             */
            for (q = p; (q = MBSCHR(q, '/')) != NULL; MB_NEXTCHAR(q))
               if (++slashes == 0) {
                  p = q + 1;    /* q points at '/', mblen("/") is 1 */
                  break;
               }
         break;
       case 'i':
         if (imatch)
            /* one -i match is sufficient, skip this pattern */
            continue;
         break;
      }
      if (MATCH(patterns[n].zname, p, casesensitive)) {
         switch (patterns[n].select) {
            case 'x':
               /* The -x match takes precedence over everything else */
               return FALSE;
            case 'R':
               Rmatch = TRUE;
               break;
            default:
               /* this must be a type -i match */
               imatch = TRUE;
               break;
         }
      }
   }
   return imatch && Rmatch;
}


#ifdef UNICODE_SUPPORT
# ifdef WIN32

int newnamew(namew, isdir, casesensitive)
  wchar_t *namew;             /* name to add (or exclude) */
  int  isdir;                 /* true for a directory */
  int  casesensitive;         /* true for case-sensitive matching */
/* Add (or exclude) the name of an existing disk file.  Return an error
   code in the ZE_ class. */
{
  wchar_t *inamew = NULL;     /* internal name */
  wchar_t *znamew = NULL;     /* external version of iname */
  wchar_t *undosmw = NULL;    /* zname version with "-j" and "-k" options disabled */
  char *oname = NULL;         /* iname converted for display */
  char *name = NULL;
  char *iname = NULL;
  char *zname = NULL;
  char *zuname = NULL;
  char *undosm = NULL;
  struct flist far *f;        /* where in found, or new found entry */
  struct zlist far *z;        /* where in zfiles (if found) */
  int dosflag;

  /* Scanning files ...
   *
   * After 5 seconds output Scanning files...
   * then a dot every 2 seconds
   */
  if (noisy) {
    /* If find files then output message after delay */
    if (scan_count == 0) {
      time_t current = time(NULL);
      scan_start = current;
    }
    scan_count++;
    if (scan_count % 100 == 0) {
      time_t current = time(NULL);

      if (current - scan_start > scan_delay) {
        if (scan_last == 0) {
          zipmessage_nl("Scanning files ", 0);
          scan_last = current;
        }
        if (current - scan_last > scan_dot_time) {
          scan_last = current;
          fprintf(mesg, ".");
          fflush(mesg);
        }
      }
    }
  }

  /* Search for name in zip file.  If there, mark it, else add to
     list of new names to do (or remove from that list). */
  if ((inamew = ex2inw(namew, isdir, &dosflag)) == NULL)
    return ZE_MEM;

  /* Discard directory names with zip -rj */
  if (*inamew == (wchar_t)'\0') {

 /* If extensions needs to be swapped, we will have empty directory names
    instead of the original directory. For example, zipping 'c.', 'c.main'
    should zip only 'main.c' while 'c.' will be converted to '\0' by ex2in. */

    if (pathput && !recurse) error("empty name without -j or -r");
    free((zvoid *)inamew);
    return ZE_OK;
  }

  if (dosflag || !pathput) {
    int save_dosify = dosify, save_pathput = pathput;
    dosify = 0;
    pathput = 1;
    /* zname is temporarly mis-used as "undosmode" iname pointer */
    if ((znamew = ex2inw(namew, isdir, NULL)) != NULL) {
      undosmw = in2exw(znamew);
      free(znamew);
    }
    dosify = save_dosify;
    pathput = save_pathput;
  }
  if ((znamew = in2exw(inamew)) == NULL)
    return ZE_MEM;

  /* Convert names from wchar_t to char */

  name = wchar_to_local_string(namew);
  iname = wchar_to_local_string(inamew);
  zname = wchar_to_local_string(znamew);

  oname = local_to_display_string(zname);

  zuname = wchar_to_local_string(znamew);

  if (undosmw == NULL)
    undosmw = znamew;
  undosm = wchar_to_local_string(undosmw);

  if ((z = zsearch(zuname)) != NULL) {
    if (pcount && !filter(undosm, casesensitive)) {
      /* Do not clear z->mark if "exclude", because, when "dosify || !pathput"
       * is in effect, two files with different filter options may hit the
       * same z entry.
       */
      if (verbose)
        fprintf(mesg, "excluding %s\n", oname);
    } else {
      z->mark = 1;
      if ((z->name = malloc(strlen(name) + 1 + PAD)) == NULL) {
        if (undosmw != znamew)
          free(undosmw);
        if (undosm) free(undosm);
        if (inamew) free(inamew);
        if (znamew) free(znamew);
        if (name) free(name);
        if (iname) free(iname);
        if (zname) free(zname);
        if (oname) free(oname);
        if (zuname) free(zuname);
        return ZE_MEM;
      }
      strcpy(z->name, name);
      z->oname = oname;
      oname = NULL;
      z->dosflag = dosflag;

#ifdef FORCE_NEWNAME
      free((zvoid *)(z->iname));
      z->iname = iname;
      iname = NULL;
#else
      /* Better keep the old name. Useful when updating on MSDOS a zip file
       * made on Unix.
       */
#endif /* ? FORCE_NEWNAME */
    }

    if ((z->namew = (wchar_t *)malloc((wcslen(namew) + 1) * sizeof(wchar_t))) == NULL) {
      if (undosmw != znamew)
        free(undosmw);
      if (undosm) free(undosm);
      if (inamew) free(inamew);
      if (znamew) free(znamew);
      if (name) free(name);
      if (iname) free(iname);
      if (zname) free(zname);
      if (oname) free(oname);
      if (zuname) free(zuname);
      return ZE_MEM;
    }
    wcscpy(z->namew, namew);
    z->inamew = inamew;
    inamew = NULL;
    z->znamew = znamew;
    znamew = NULL;
    z->uname = wchar_to_utf8_string(z->inamew);
    if (name == label) {
       label = z->name;
    }
  } else if (pcount == 0 || filter(undosm, casesensitive)) {

    /* Check that we are not adding the zip file to itself. This
     * catches cases like "zip -m foo ../dir/foo.zip".
     */
/* Version of stat() for CMS/MVS isn't complete enough to see if       */
/* files match.  Just let ZIP.C compare the filenames.  That's good    */
/* enough for CMS anyway since there aren't paths to worry about.      */
    zw_stat statbw;     /* need for wide stat */
    wchar_t *zipfilew = local_to_wchar_string(zipfile);

    if (zipstate == -1)
       zipstate = strcmp(zipfile, "-") != 0 &&
                   zwstat(zipfilew, &zipstatbw) == 0;
    free(zipfilew);

    if (zipstate == 1 && (statbw = zipstatbw, zwstat(namew, &statbw) == 0
      && zipstatbw.st_mode  == statbw.st_mode
      && zipstatbw.st_ino   == statbw.st_ino
      && zipstatbw.st_dev   == statbw.st_dev
      && zipstatbw.st_uid   == statbw.st_uid
      && zipstatbw.st_gid   == statbw.st_gid
      && zipstatbw.st_size  == statbw.st_size
      && zipstatbw.st_mtime == statbw.st_mtime
      && zipstatbw.st_ctime == statbw.st_ctime)) {
      /* Don't compare a_time since we are reading the file */
        if (verbose)
          fprintf(mesg, "file matches zip file -- skipping\n");
        if (undosmw != znamew)
          free(undosmw);
        if (undosm) free(undosm);
        if (inamew) free(inamew);
        if (znamew) free(znamew);
        if (name) free(name);
        if (iname) free(iname);
        if (zname) free(zname);
        if (oname) free(oname);
        if (zuname) free(zuname);
        return ZE_OK;
    }

    /* allocate space and add to list */
    if ((f = (struct flist far *)farmalloc(sizeof(struct flist))) == NULL ||
        fcount + 1 < fcount ||
        (f->name = malloc(strlen(name) + 1 + PAD)) == NULL)
    {
      if (f != NULL)
        farfree((zvoid far *)f);
      if (undosmw != znamew)
        free(undosmw);
      if (undosm) free(undosm);
      if (inamew) free(inamew);
      if (znamew) free(znamew);
      if (name) free(name);
      if (iname) free(iname);
      if (zname) free(zname);
      if (oname) free(oname);
      if (zuname) free(zuname);
      return ZE_MEM;
    }
    if (undosmw != znamew)
      free((zvoid *)undosmw);
    strcpy(f->name, name);
    f->iname = iname;
    iname = NULL;
    f->zname = zname;
    zname = NULL;
    /* Unicode */
    if ((f->namew = (wchar_t *)malloc((wcslen(namew) + 1) * sizeof(wchar_t))) == NULL) {
      if (f != NULL)
        farfree((zvoid far *)f);
      if (undosmw != znamew)
        free(undosmw);
      if (undosm) free(undosm);
      if (inamew) free(inamew);
      if (znamew) free(znamew);
      if (name) free(name);
      if (iname) free(iname);
      if (zname) free(zname);
      if (oname) free(oname);
      if (zuname) free(zuname);
      return ZE_MEM;
    }
    wcscpy(f->namew, namew);
    f->znamew = znamew;
    znamew = NULL;
    f->uname = wchar_to_utf8_string(inamew);
    f->inamew = inamew;
    inamew = NULL;
    f->oname = oname;
    oname = NULL;
    f->dosflag = dosflag;
    *fnxt = f;
    f->lst = fnxt;
    f->nxt = NULL;
    fnxt = &f->nxt;
    fcount++;
    if (name == label) {
      label = f->name;
    }
  }
  if (undosm) free(undosm);
  if (inamew) free(inamew);
  if (znamew) free(znamew);
  if (name) free(name);
  if (iname) free(iname);
  if (zname) free(zname);
  if (oname) free(oname);
  if (zuname) free(zuname);
  return ZE_OK;
}

# endif
#endif

int newname(name, isdir, casesensitive)
  char *name;           /* name to add (or exclude) */
  int  isdir;           /* true for a directory */
  int  casesensitive;   /* true for case-sensitive matching */
/* Add (or exclude) the name of an existing disk file.  Return an error
   code in the ZE_ class. */
{
  char *iname, *zname;  /* internal name, external version of iname */
  char *undosm;         /* zname version with "-j" and "-k" options disabled */
  char *oname;          /* iname converted for display */
  struct flist far *f;  /* where in found, or new found entry */
  struct zlist far *z;  /* where in zfiles (if found) */
  int dosflag;

  /* Scanning files ...
   *
   * After 5 seconds output Scanning files...
   * then a dot every 2 seconds
   */
  if (noisy) {
    /* If find files then output message after delay */
    if (scan_count == 0) {
      time_t current = time(NULL);
      scan_start = current;
    }
    scan_count++;
    if (scan_count % 100 == 0) {
      time_t current = time(NULL);

      if (current - scan_start > scan_delay) {
        if (scan_last == 0) {
          zipmessage_nl("Scanning files ", 0);
          scan_last = current;
        }
        if (current - scan_last > scan_dot_time) {
          scan_last = current;
          fprintf(mesg, ".");
          fflush(mesg);
        }
      }
    }
  }

  /* Search for name in zip file.  If there, mark it, else add to
     list of new names to do (or remove from that list). */
  if ((iname = ex2in(name, isdir, &dosflag)) == NULL)
    return ZE_MEM;

  /* Discard directory names with zip -rj */
  if (*iname == '\0') {
#ifndef AMIGA
/* A null string is a legitimate external directory name in AmigaDOS; also,
 * a command like "zip -r zipfile FOO:" produces an empty internal name.
 */
# ifndef RISCOS
 /* If extensions needs to be swapped, we will have empty directory names
    instead of the original directory. For example, zipping 'c.', 'c.main'
    should zip only 'main.c' while 'c.' will be converted to '\0' by ex2in. */

    if (pathput && !recurse) error("empty name without -j or -r");

# endif /* !RISCOS */
#endif /* !AMIGA */
    free((zvoid *)iname);
    return ZE_OK;
  }
  undosm = NULL;
  if (dosflag || !pathput) {
    int save_dosify = dosify, save_pathput = pathput;
    dosify = 0;
    pathput = 1;
    /* zname is temporarly mis-used as "undosmode" iname pointer */
    if ((zname = ex2in(name, isdir, NULL)) != NULL) {
      undosm = in2ex(zname);
      free(zname);
    }
    dosify = save_dosify;
    pathput = save_pathput;
  }
  if ((zname = in2ex(iname)) == NULL)
    return ZE_MEM;
#ifdef UNICODE_SUPPORT
  /* Convert name to display or OEM name */
  oname = local_to_display_string(iname);
#else
  if ((oname = malloc(strlen(zname) + 1)) == NULL)
    return ZE_MEM;
  strcpy(oname, zname);
#endif
  if (undosm == NULL)
    undosm = zname;
  if ((z = zsearch(zname)) != NULL) {
    if (pcount && !filter(undosm, casesensitive)) {
      /* Do not clear z->mark if "exclude", because, when "dosify || !pathput"
       * is in effect, two files with different filter options may hit the
       * same z entry.
       */
      if (verbose)
        fprintf(mesg, "excluding %s\n", oname);
      free((zvoid *)iname);
      free((zvoid *)zname);
    } else {
      z->mark = 1;
      if ((z->name = malloc(strlen(name) + 1 + PAD)) == NULL) {
        if (undosm != zname)
          free((zvoid *)undosm);
        free((zvoid *)iname);
        free((zvoid *)zname);
        return ZE_MEM;
      }
      strcpy(z->name, name);
      z->oname = oname;
      z->dosflag = dosflag;

#ifdef FORCE_NEWNAME
      free((zvoid *)(z->iname));
      z->iname = iname;
#else
      /* Better keep the old name. Useful when updating on MSDOS a zip file
       * made on Unix.
       */
      free((zvoid *)iname);
      free((zvoid *)zname);
#endif /* ? FORCE_NEWNAME */
    }
#if defined(UNICODE_SUPPORT) && defined(WIN32)
    z->namew = NULL;
    z->inamew = NULL;
    z->znamew = NULL;
#endif
    if (name == label) {
       label = z->name;
    }
  } else if (pcount == 0 || filter(undosm, casesensitive)) {

    /* Check that we are not adding the zip file to itself. This
     * catches cases like "zip -m foo ../dir/foo.zip".
     */
#ifndef CMS_MVS
/* Version of stat() for CMS/MVS isn't complete enough to see if       */
/* files match.  Just let ZIP.C compare the filenames.  That's good    */
/* enough for CMS anyway since there aren't paths to worry about.      */
    z_stat statb;      /* now use structure z_stat and function zstat globally 7/24/04 EG */

    if (zipstate == -1)
       zipstate = strcmp(zipfile, "-") != 0 &&
                   zstat(zipfile, &zipstatb) == 0;

    if (zipstate == 1 && (statb = zipstatb, zstat(name, &statb) == 0
      && zipstatb.st_mode  == statb.st_mode
#ifdef VMS
      && memcmp(zipstatb.st_ino, statb.st_ino, sizeof(statb.st_ino)) == 0
      && strcmp(zipstatb.st_dev, statb.st_dev) == 0
      && zipstatb.st_uid   == statb.st_uid
#else /* !VMS */
      && zipstatb.st_ino   == statb.st_ino
      && zipstatb.st_dev   == statb.st_dev
      && zipstatb.st_uid   == statb.st_uid
      && zipstatb.st_gid   == statb.st_gid
#endif /* ?VMS */
      && zipstatb.st_size  == statb.st_size
      && zipstatb.st_mtime == statb.st_mtime
      && zipstatb.st_ctime == statb.st_ctime)) {
      /* Don't compare a_time since we are reading the file */
         if (verbose)
           fprintf(mesg, "file matches zip file -- skipping\n");
         if (undosm != zname)
           free((zvoid *)zname);
         if (undosm != iname)
           free((zvoid *)undosm);
         free((zvoid *)iname);
         free(oname);
         return ZE_OK;
    }
#endif  /* CMS_MVS */

    /* allocate space and add to list */
    if ((f = (struct flist far *)farmalloc(sizeof(struct flist))) == NULL ||
        fcount + 1 < fcount ||
        (f->name = malloc(strlen(name) + 1 + PAD)) == NULL)
    {
      if (f != NULL)
        farfree((zvoid far *)f);
      if (undosm != zname)
        free((zvoid *)undosm);
      free((zvoid *)iname);
      free((zvoid *)zname);
      free(oname);
      return ZE_MEM;
    }
    strcpy(f->name, name);
    f->iname = iname;
    f->zname = zname;
#ifdef UNICODE_SUPPORT
    /* Unicode */
    f->uname = local_to_utf8_string(iname);
#ifdef WIN32
    f->namew = NULL;
    f->inamew = NULL;
    f->znamew = NULL;
    if (strcmp(f->name, "-") == 0) {
      f->namew = local_to_wchar_string(f->name);
    }
#endif

#endif
    f->oname = oname;
    f->dosflag = dosflag;

    *fnxt = f;
    f->lst = fnxt;
    f->nxt = NULL;
    fnxt = &f->nxt;
    fcount++;
    if (name == label) {
      label = f->name;
    }
  }
  if (undosm != zname)
    free((zvoid *)undosm);
  return ZE_OK;
}

ulg dostime(y, n, d, h, m, s)
int y;                  /* year */
int n;                  /* month */
int d;                  /* day */
int h;                  /* hour */
int m;                  /* minute */
int s;                  /* second */
/* Convert the date y/n/d and time h:m:s to a four byte DOS date and
   time (date in high two bytes, time in low two bytes allowing magnitude
   comparison). */
{
  return y < 1980 ? DOSTIME_MINIMUM /* dostime(1980, 1, 1, 0, 0, 0) */ :
        (((ulg)y - 1980) << 25) | ((ulg)n << 21) | ((ulg)d << 16) |
        ((ulg)h << 11) | ((ulg)m << 5) | ((ulg)s >> 1);
}


ulg unix2dostime(t)
time_t *t;              /* unix time to convert */
/* Return the Unix time t in DOS format, rounded up to the next two
   second boundary. */
{
  time_t t_even;
  struct tm *s;         /* result of localtime() */

  t_even = (time_t)(((unsigned long)(*t) + 1) & (~1));
                                /* Round up to even seconds. */
  s = localtime(&t_even);       /* Use local time since MSDOS does. */
  if (s == (struct tm *)NULL) {
      /* time conversion error; use current time as emergency value
         (assuming that localtime() does at least accept this value!) */
      t_even = (time_t)(((unsigned long)time(NULL) + 1) & (~1));
      s = localtime(&t_even);
  }
  return dostime(s->tm_year + 1900, s->tm_mon + 1, s->tm_mday,
                 s->tm_hour, s->tm_min, s->tm_sec);
}

int issymlnk(a)
ulg a;                  /* Attributes returned by filetime() */
/* Return true if the attributes are those of a symbolic link */
{
#ifndef QDOS
#ifdef S_IFLNK
#ifdef __human68k__
  int *_dos_importlnenv(void);

  if (_dos_importlnenv() == NULL)
    return 0;
#endif
  return ((a >> 16) & S_IFMT) == S_IFLNK;
#else /* !S_IFLNK */
  return (int)a & 0;    /* avoid warning on unused parameter */
#endif /* ?S_IFLNK */
#else
  return 0;
#endif
}

#endif /* !UTIL */


#if (!defined(UTIL) && !defined(ZP_NEED_GEN_D2U_TIME))
   /* There is no need for dos2unixtime() in the ZipUtils' code. */
#  define ZP_NEED_GEN_D2U_TIME
#endif
#if ((defined(OS2) || defined(VMS)) && defined(ZP_NEED_GEN_D2U_TIME))
   /* OS/2 and VMS use a special solution to handle time-stams of files. */
#  undef ZP_NEED_GEN_D2U_TIME
#endif
#if (defined(W32_STATROOT_FIX) && !defined(ZP_NEED_GEN_D2U_TIME))
   /* The Win32 stat()-bandaid to fix stat'ing root directories needs
    * dos2unixtime() to calculate the time-stamps. */
#  define ZP_NEED_GEN_D2U_TIME
#endif

#ifdef ZP_NEED_GEN_D2U_TIME

time_t dos2unixtime(dostime)
ulg dostime;            /* DOS time to convert */
/* Return the Unix time_t value (GMT/UTC time) for the DOS format (local)
 * time dostime, where dostime is a four byte value (date in most significant
 * word, time in least significant word), see dostime() function.
 */
{
  struct tm *t;         /* argument for mktime() */
  ZCONST time_t clock = time(NULL);

  t = localtime(&clock);
  t->tm_isdst = -1;     /* let mktime() determine if DST is in effect */
  /* Convert DOS time to UNIX time_t format */
  t->tm_sec  = (((int)dostime) <<  1) & 0x3e;
  t->tm_min  = (((int)dostime) >>  5) & 0x3f;
  t->tm_hour = (((int)dostime) >> 11) & 0x1f;
  t->tm_mday = (int)(dostime >> 16) & 0x1f;
  t->tm_mon  = ((int)(dostime >> 21) & 0x0f) - 1;
  t->tm_year = ((int)(dostime >> 25) & 0x7f) + 80;

  return mktime(t);
}

#undef ZP_NEED_GEN_D2U_TIME
#endif /* ZP_NEED_GEN_D2U_TIME */


#ifndef MACOS
int destroy(f)
  char *f;             /* file to delete */
/* Delete the file *f, returning non-zero on failure. */
{
  return unlink(f);
}


int replace(d, s)
char *d, *s;            /* destination and source file names */
/* Replace file *d by file *s, removing the old *s.  Return an error code
   in the ZE_ class. This function need not preserve the file attributes,
   this will be done by setfileattr() later.
 */
{
  z_stat t;         /* results of stat() */
#if defined(CMS_MVS)
  /* cmsmvs.h defines FOPW_TEMP as memory(hiperspace).  Since memory is
   * lost at end of run, always do copy instead of rename.
   */
  int copy = 1;
#else
  int copy = 0;
#endif
  int d_exists;

#if defined(VMS) || defined(CMS_MVS)
  /* stat() is broken on VMS remote files (accessed through Decnet).
   * This patch allows creation of remote zip files, but is not sufficient
   * to update them or compress remote files */
  unlink(d);
#else /* !(VMS || CMS_MVS) */
  d_exists = (LSTAT(d, &t) == 0);
  if (d_exists)
  {
    /*
     * respect existing soft and hard links!
     */
    if (t.st_nlink > 1
# ifdef S_IFLNK
        || (t.st_mode & S_IFMT) == S_IFLNK
# endif
        )
       copy = 1;
    else if (unlink(d))
       return ZE_CREAT;                 /* Can't erase zip file--give up */
  }
#endif /* ?(VMS || CMS_MVS) */
#ifndef CMS_MVS
  if (!copy) {
      if (rename(s, d)) {               /* Just move s on top of d */
          copy = 1;                     /* failed ? */
#if !defined(VMS) && !defined(ATARI) && !defined(AZTEC_C)
#if !defined(CMS_MVS) && !defined(RISCOS) && !defined(QDOS)
    /* For VMS, ATARI, AMIGA Aztec, VM_CMS, MVS, RISCOS,
       always assume that failure is EXDEV */
          if (errno != EXDEV
#  ifdef THEOS
           && errno != EEXIST
#  else
#    ifdef ENOTSAM
           && errno != ENOTSAM /* Used at least on Turbo C */
#    endif
#  endif
              ) return ZE_CREAT;
#endif /* !CMS_MVS && !RISCOS */
#endif /* !VMS && !ATARI && !AZTEC_C */
      }
  }
#endif /* !CMS_MVS */

  if (copy) {
    FILE *f, *g;        /* source and destination files */
    int r;              /* temporary variable */

#ifdef RISCOS
    if (SWI_OS_FSControl_26(s,d,0xA1)!=NULL) {
#endif

    /* Use zfopen for almost all opens where fopen is used.  For
       most OS that support large files we use the 64-bit file
       environment and zfopen maps to fopen, but this allows
       tweeking ports that don't do that.  7/24/04 */
    if ((f = zfopen(s, FOPR)) == NULL) {
      fprintf(mesg," replace: can't open %s\n", s);
      return ZE_TEMP;
    }
    if ((g = zfopen(d, FOPW)) == NULL)
    {
      fclose(f);
      return ZE_CREAT;
    }

    r = fcopy(f, g, (ulg)-1L);
    fclose(f);
    if (fclose(g) || r != ZE_OK)
    {
      unlink(d);
      return r ? (r == ZE_TEMP ? ZE_WRITE : r) : ZE_WRITE;
    }
    unlink(s);
#ifdef RISCOS
    }
#endif
  }
  return ZE_OK;
}
#endif /* !MACOS */


int getfileattr(f)
char *f;                /* file path */
/* Return the file attributes for file f or 0 if failure */
{
#ifdef __human68k__
  struct _filbuf buf;

  return _dos_files(&buf, f, 0xff) < 0 ? 0x20 : buf.atr;
#else
  z_stat s;

  return SSTAT(f, &s) == 0 ? (int) s.st_mode : 0;
#endif
}


int setfileattr(f, a)
char *f;                /* file path */
int a;                  /* attributes returned by getfileattr() */
/* Give the file f the attributes a, return non-zero on failure */
{
#if defined(TOPS20) || defined (CMS_MVS)
  return 0;
#else
#ifdef __human68k__
  return _dos_chmod(f, a) < 0 ? -1 : 0;
#else
  return chmod(f, a);
#endif
#endif
}


/* tempname */

#ifndef VMS /* VMS-specific function is in VMS.C. */

char *tempname(zip)
  char *zip;              /* path name of zip file to generate temp name for */

/* Return a temporary file name in its own malloc'ed space, using tempath. */
{
  char *t = zip;   /* malloc'ed space for name (use zip to avoid warning) */

# ifdef CMS_MVS
  if ((t = malloc(strlen(tempath) + L_tmpnam + 2)) == NULL)
    return NULL;

#  ifdef VM_CMS
  tmpnam(t);
  /* Remove filemode and replace with tempath, if any. */
  /* Otherwise A-disk is used by default */
  *(strrchr(t, ' ')+1) = '\0';
  if (tempath!=NULL)
     strcat(t, tempath);
  return t;
#  else   /* !VM_CMS */
  /* For MVS */
  tmpnam(t);
  if (tempath != NULL)
  {
    int l1 = strlen(t);
    char *dot;
    if (*t == '\'' && *(t+l1-1) == '\'' && (dot = strchr(t, '.')))
    {
      /* MVS and not OE.  tmpnam() returns quoted string of 5 qualifiers.
       * First is HLQ, rest are timestamps.  User can only replace HLQ.
       */
      int l2 = strlen(tempath);
      if (strchr(tempath, '.') || l2 < 1 || l2 > 8)
        ziperr(ZE_PARMS, "On MVS and not OE, tempath (-b) can only be HLQ");
      memmove(t+1+l2, dot, l1+1-(dot-t));  /* shift dot ready for new hlq */
      memcpy(t+1, tempath, l2);            /* insert new hlq */
    }
    else
    {
      /* MVS and probably OE.  tmpnam() returns filename based on TMPDIR,
       * no point in even attempting to change it.  User should modify TMPDIR
       * instead.
       */
      zipwarn("MVS, assumed to be OE, change TMPDIR instead of option -b: ",
              tempath);
    }
  }
  return t;
#  endif  /* !VM_CMS */

# else /* !CMS_MVS */

#  ifdef TANDEM
  char cur_subvol [FILENAME_MAX];
  char temp_subvol [FILENAME_MAX];
  char *zptr;
  char *ptr;
  char *cptr = &cur_subvol[0];
  char *tptr = &temp_subvol[0];
  short err;
  FILE *tempf;
  int attempts;

  t = (char *)malloc(NAMELEN); /* malloc here as you cannot free */
                               /* tmpnam allocated storage later */

  zptr = strrchr(zip, TANDEM_DELIMITER);

  if (zptr != NULL) {
    /* ZIP file specifies a Subvol so make temp file there so it can just
       be renamed at end */

    *tptr = *cptr = '\0';
    strcat(cptr, getenv("DEFAULTS"));

    strncat(tptr, zip, _min(FILENAME_MAX, (zptr - zip)) ); /* temp subvol */
    strncat(t, zip, _min(NAMELEN, ((zptr - zip) + 1)) );   /* temp stem   */

    err = chvol(tptr);
    ptr = t + strlen(t);  /* point to end of stem */
  }
  else
    ptr = t;

  /* If two zips are running in same subvol then we can get contention problems
     with the temporary filename.  As a work around we attempt to create
     the file here, and if it already exists we get a new temporary name */

  attempts = 0;
  do {
    attempts++;
    tmpnam(ptr);  /* Add filename */
    tempf = zfopen(ptr, FOPW_TMP);    /* Attempt to create file */
  } while (tempf == NULL && attempts < 100);

  if (attempts >= 100) {
    ziperr(ZE_TEMP, "Could not get unique temp file name");
  }

  fclose(tempf);

  if (zptr != NULL) {
    err = chvol(cptr);  /* Put ourself back to where we came in */
  }

  return t;

#  else /* !CMS_MVS && !TANDEM */
/*
 * Do something with TMPDIR, TMP, TEMP ????
 */
  if (tempath != NULL)
  {
    if ((t = malloc(strlen(tempath) + 12)) == NULL)
      return NULL;
    strcpy(t, tempath);

#   if (!defined(VMS) && !defined(TOPS20))
#    ifdef MSDOS
    {
      char c = (char)lastchar(t);
      if (c != '/' && c != ':' && c != '\\')
        strcat(t, "/");
    }
#    else

#     ifdef AMIGA
    {
      char c = (char)lastchar(t);
      if (c != '/' && c != ':')
        strcat(t, "/");
    }
#     else /* !AMIGA */
#      ifdef RISCOS
    if (lastchar(t) != '.')
      strcat(t, ".");
#      else /* !RISCOS */

#       ifdef QDOS
    if (lastchar(t) != '_')
      strcat(t, "_");
#       else
    if (lastchar(t) != '/')
      strcat(t, "/");
#       endif /* ?QDOS */
#      endif /* ?RISCOS */
#     endif  /* ?AMIGA */
#    endif /* ?MSDOS */
#   endif /* !VMS && !TOPS20 */
  }
  else
  {
    if ((t = malloc(12)) == NULL)
      return NULL;
    *t = 0;
  }
#   ifdef NO_MKTEMP
  {
    char *p = t + strlen(t);
    sprintf(p, "%08lx", (ulg)time(NULL));
    return t;
  }
#   else
  strcat(t, "ziXXXXXX"); /* must use lowercase for Linux dos file system */
#     if defined(UNIX) && !defined(NO_MKSTEMP)
  /* tempname should not be called */
  return t;
#     else
  return mktemp(t);
#     endif
#   endif /* NO_MKTEMP */
#  endif /* TANDEM */
# endif /* CMS_MVS */
}
#endif /* !VMS */

int fcopy(f, g, n)
  FILE *f, *g;            /* source and destination files */
  /* now use uzoff_t for all file sizes 5/14/05 CS */
  uzoff_t n;               /* number of bytes to copy or -1 for all */
/* Copy n bytes from file *f to file *g, or until EOF if (zoff_t)n == -1.
   Return an error code in the ZE_ class. */
{
  char *b;              /* malloc'ed buffer for copying */
  extent k;             /* result of fread() */
  uzoff_t m;            /* bytes copied so far */

  if ((b = malloc(CBSZ)) == NULL)
    return ZE_MEM;
  m = 0;
  while (n == (uzoff_t)(-1L) || m < n)
  {
    if ((k = fread(b, 1, n == (uzoff_t)(-1) ?
                   CBSZ : (n - m < CBSZ ? (extent)(n - m) : CBSZ), f)) == 0)
    {
      if (ferror(f))
      {
        free((zvoid *)b);
        return ZE_READ;
      }
      else
        break;
    }
    if (fwrite(b, 1, k, g) != k)
    {
      free((zvoid *)b);
      fprintf(mesg," fcopy: write error\n");
      return ZE_TEMP;
    }
    m += k;
  }
  free((zvoid *)b);
  return ZE_OK;
}


/* from zipfile.c */

#ifdef THEOS
 /* Macros cause stack overflow in compiler */
 ush SH(uch* p) { return ((ush)(uch)((p)[0]) | ((ush)(uch)((p)[1]) << 8)); }
 ulg LG(uch* p) { return ((ulg)(SH(p)) | ((ulg)(SH((p)+2)) << 16)); }
#else /* !THEOS */
 /* Macros for converting integers in little-endian to machine format */
# define SH(a) ((ush)(((ush)(uch)(a)[0]) | (((ush)(uch)(a)[1]) << 8)))
# define LG(a) ((ulg)SH(a) | ((ulg)SH((a)+2) << 16))
# ifdef ZIP64_SUPPORT           /* zip64 support 08/31/2003 R.Nausedat */
#  define LLG(a) ((zoff_t)LG(a) | ((zoff_t)LG((a)+4) << 32))
# endif
#endif /* ?THEOS */


/* always copies from global in_file to global output file y */
int bfcopy(n)
  /* now use uzoff_t for all file sizes 5/14/05 CS */
  uzoff_t n;               /* number of bytes to copy or -1 for all */
/* Copy n bytes from in_file to out_file, or until EOF if (zoff_t)n == -1.

   Normally we have the compressed size from either the central directory
   entry or the local header.

   If n != -1 and EOF, close current split and open next and continue
   copying.

   If n == -2, copy until find the extended header (data descriptor).  Only
   used for -FF when no size available.

   If fix == 1 calculate CRC of input entry and verify matches.

   If fix == 2 and this entry using data descriptor keep a sliding
   window in the buffer for looking for signature.

   Return an error code in the ZE_ class. */
{
  char *b;              /* malloc'ed buffer for copying */
  extent k;             /* result of fread() */
  uzoff_t m;            /* bytes copied so far */
  extent brd;           /* bytes to read */
  zoff_t data_start = 0;
  zoff_t des_start = 0;
  char *split_path;
  extent kk;
  int i;
  char sbuf[4];         /* buffer for sliding signature window for fix = 2 */
  int des = 0;          /* this entry has data descriptor to find */

  if ((b = malloc(CBSZ)) == NULL)
    return ZE_MEM;

  if (copy_only && !display_globaldots) {
    /* initialize dot count */
    dot_count = -1;
  }

  if (fix == 2 && n == (uzoff_t) -2) {
    data_start = zftello(in_file);
    for (kk = 0; kk < 4; kk++)
      sbuf[kk] = 0;
    des = 1;
  }

  des_good = 0;

  m = 0;
  while (des || n == (uzoff_t)(-1L) || m < n)
  {
    if (des || n == (uzoff_t)(-1))
      brd = CBSZ;
    else
      brd = (n - m < CBSZ ? (extent)(n - m) : CBSZ);

    des_start = zftello(in_file);

    if ((k = fread(b, 1, brd, in_file)) == 0)
    {
      if (fix == 2 && k < brd) {
        free((zvoid *)b);
        return ZE_READ;
      }
      else if (ferror(in_file))
      {
        free((zvoid *)b);
        return ZE_READ;
      }
      else {
        break;
      }
    }


    /* end at extended local header (data descriptor) signature */
    if (des) {
      des_crc = 0;
      des_csize = 0;
      des_usize = 0;

      /* If first 4 bytes in buffer are data descriptor signature then
         try to read the data descriptor.
         If not, scan for signature and break if found, let bfwrite flush
         the data and then next read should put the data descriptor at
         the beginning of the buffer.
       */

      if (
          (b[0] != 0x50 /*'P' except EBCDIC*/ ||
           b[1] != 0x4b /*'K' except EBCDIC*/ ||
           b[2] != '\07' ||
           b[3] != '\010')) {
        /* buffer is not start of data descriptor */

        for (kk = 0; kk < k; kk++) {
          /* add byte to end of sbuf */
          for (i = 0; i < 3; i++)
            sbuf[i] = sbuf[i + 1];
          sbuf[3] = b[kk];

          /* see if this is signature */
          if (
              (sbuf[0] == 0x50 /*'P' except EBCDIC*/ &&
               sbuf[1] == 0x4b /*'K' except EBCDIC*/ &&
               sbuf[2] == '\07' &&
               sbuf[3] == '\010')) {
            kk -= 3;
            if (zfseeko(in_file, bytes_this_split + kk, SEEK_SET) != 0) {
              /* seek error */
              ZIPERR(ZE_READ, "seek failed reading descriptor");
            }
            des_start = zftello(in_file);
            k = kk;
            break;
          }
        }
      }
      else

      /* signature at start of buffer */
      {
        des_good = 0;

#ifdef ZIP64_SUPPORT
        if (zip64_entry) {

          /* read Zip64 data descriptor */
          if (k < 24) {
            /* not enough bytes, so can't be data descriptor
               as data descriptors can't be split across splits
             */
          }
          else
          {
            /* read the Zip64 descriptor */

            des_crc = LG(b + 4);
            des_csize = LLG(b + 8);
            des_usize = LLG(b + 16);

            /* if this is the right data descriptor then the sizes should match */
            if ((uzoff_t)des_start - (uzoff_t)data_start != des_csize) {
              /* apparently this signature does not go with this data so skip */

              /* write out signature as data */
              k = 4;
              if (zfseeko(in_file, des_start + k, SEEK_SET) != 0) {
                /* seek error */
                ZIPERR(ZE_READ, "seek failed reading descriptor");
              }
              if (bfwrite(b, 1, k, BFWRITE_DATA) != k)
              {
                free((zvoid *)b);
                fprintf(mesg," fcopy: write error\n");
                return ZE_TEMP;
              }
              m += k;
              continue;
            }
            else
            {
              /* apparently this is the correct data descriptor */

              /* we should check the CRC but would need to inflate
                 the data */

              /* skip descriptor as will write out later */
              des_good = 1;
              k = 24;
              data_start = zftello(in_file);
              if (zfseeko(in_file, des_start + k, SEEK_SET) != 0) {
                /* seek error */
                ZIPERR(ZE_READ, "seek failed reading descriptor");
              }
              data_start = zftello(in_file);
            }
          }

        }
        else
#endif
        {
          /* read standard data descriptor */

          if (k < 16) {
            /* not enough bytes, so can't be data descriptor
               as data descriptors can't be split across splits
             */
          }
          else
          {
            /* read the descriptor */

            des_crc = LG(b + 4);
            des_csize = LG(b + 8);
            des_usize = LG(b + 12);

            /* if this is the right data descriptor then the sizes should match */
            if ((uzoff_t)des_start - (uzoff_t)data_start != des_csize) {
              /* apparently this signature does not go with this data so skip */

              /* write out signature as data */
              k = 4;
              if (zfseeko(in_file, des_start + k, SEEK_SET) != 0) {
                /* seek error */
                ZIPERR(ZE_READ, "seek failed reading descriptor");
              }
              if (bfwrite(b, 1, k, BFWRITE_DATA) != k)
              {
                free((zvoid *)b);
                fprintf(mesg," fcopy: write error\n");
                return ZE_TEMP;
              }
              m += k;
              continue;
            }
            else
            {
              /* apparently this is the correct data descriptor */

              /* we should check the CRC but this does not work for
                 encrypted data */

              /* skip descriptor as will write out later */
              des_good = 1;
              data_start = zftello(in_file);
              k = 16;
              if (zfseeko(in_file, des_start + k, SEEK_SET) != 0) {
                /* seek error */
                ZIPERR(ZE_READ, "seek failed reading descriptor");
              }
              data_start = zftello(in_file);
            }
          }


        }
      }
    }


    if (des_good) {
      /* skip descriptor as will write out later */
    } else {
      /* write out apparently wrong descriptor as data */
      if (bfwrite(b, 1, k, BFWRITE_DATA) != k)
      {
        free((zvoid *)b);
        fprintf(mesg," fcopy: write error\n");
        return ZE_TEMP;
      }
      m += k;
    }

    if (copy_only && !display_globaldots) {
      if (dot_size > 0) {
        /* initial space */
        if (noisy && dot_count == -1) {
#ifndef WINDLL
          putc(' ', mesg);
          fflush(mesg);
#else
          fprintf(stdout,"%c",' ');
#endif
          dot_count++;
        }
        dot_count += k;
        if (dot_size <= dot_count) dot_count = 0;
      }
      if ((verbose || noisy) && dot_size && !dot_count) {
#ifndef WINDLL
        putc('.', mesg);
        fflush(mesg);
#else
        fprintf(stdout,"%c",'.');
#endif
        mesg_line_started = 1;
      }
    }

    if (des_good)
      break;

    if (des)
      continue;

    if ((des || n != (uzoff_t)(-1L)) && m < n && feof(in_file)) {
      /* open next split */
      current_in_disk++;

      if (current_in_disk >= total_disks) {
        /* done */
        break;

      } else if (current_in_disk == total_disks - 1) {
        /* last disk is archive.zip */
        if ((split_path = malloc(strlen(in_path) + 1)) == NULL) {
          zipwarn("reading archive: ", in_path);
          return ZE_MEM;
        }
        strcpy(split_path, in_path);
      } else {
        /* other disks are archive.z01, archive.z02, ... */
        split_path = get_in_split_path(in_path, current_in_disk);
      }

      fclose(in_file);

      /* open the split */
      while ((in_file = zfopen(split_path, FOPR)) == NULL) {
        int r = 0;

        /* could not open split */

        if (fix == 1 && skip_this_disk) {
          free(split_path);
          free((zvoid *)b);
          return ZE_FORM;
        }

        /* Ask for directory with split.  Updates in_path */
        r = ask_for_split_read_path(current_in_disk);
        if (r == ZE_ABORT) {
          zipwarn("could not find split: ", split_path);
          free(split_path);
          free((zvoid *)b);
          return ZE_ABORT;
        }
        if (r == ZE_EOF) {
          zipmessage_nl("", 1);
          zipwarn("user ended reading - closing archive", "");
          free(split_path);
          free((zvoid *)b);
          return ZE_EOF;
        }
        if (fix == 2 && skip_this_disk) {
          /* user asked to skip this disk */
          zipwarn("skipping split file: ", split_path);
          current_in_disk++;
        }

        if (current_in_disk == total_disks - 1) {
          /* last disk is archive.zip */
          if ((split_path = malloc(strlen(in_path) + 1)) == NULL) {
            zipwarn("reading archive: ", in_path);
            return ZE_MEM;
          }
          strcpy(split_path, in_path);
        } else {
          /* other disks are archive.z01, archive.z02, ... */
          split_path = get_in_split_path(zipfile, current_in_disk);
        }
      }
      if (fix == 2 && skip_this_disk) {
        /* user asked to skip this disk */
        free(split_path);
        free((zvoid *)b);
        return ZE_FORM;
      }
      free(split_path);
    }
  }
  free((zvoid *)b);
  return ZE_OK;
}



#ifdef NO_RENAME
int rename(from, to)
ZCONST char *from;
ZCONST char *to;
{
    unlink(to);
    if (link(from, to) == -1)
        return -1;
    if (unlink(from) == -1)
        return -1;
    return 0;
}

#endif /* NO_RENAME */


#ifdef ZMEM

/************************/
/*  Function memset()   */
/************************/

/*
 * memset - for systems without it
 *  bill davidsen - March 1990
 */

char *
memset(buf, init, len)
register char *buf;     /* buffer loc */
register int init;      /* initializer */
register unsigned int len;   /* length of the buffer */
{
    char *start;

    start = buf;
    while (len--) *(buf++) = init;
    return(start);
}


/************************/
/*  Function memcpy()   */
/************************/

char *
memcpy(dst,src,len)             /* v2.0f */
register char *dst, *src;
register unsigned int len;
{
    char *start;

    start = dst;
    while (len--)
        *dst++ = *src++;
    return(start);
}


/************************/
/*  Function memcmp()   */
/************************/

int
memcmp(b1,b2,len)                     /* jpd@usl.edu -- 11/16/90 */
register char *b1, *b2;
register unsigned int len;
{

    if (len) do {       /* examine each byte (if any) */
      if (*b1++ != *b2++)
        return (*((uch *)b1-1) - *((uch *)b2-1));  /* exit when miscompare */
    } while (--len);

    return(0);          /* no miscompares, yield 0 result */
}

#endif  /* ZMEM */


/*------------------------------------------------------------------
 * Split archives
 */


/* ask_for_split_read_path
 *
 * If the next split file is not in the current directory, ask
 * the user where it is.
 *
 * in_path is the base path for reading splits and is usually
 * the same as zipfile.  The path in in_path must be the archive
 * file ending in .zip as this is assumed by get_in_split_path().
 *
 * Updates in_path if changed.  Returns ZE_OK if OK or ZE_ABORT if
 * user cancels reading archive.
 *
 * If fix = 1 then allow skipping disk (user may not have it).
 */

#define SPLIT_MAXPATH (FNMAX + 4010)

int ask_for_split_read_path(current_disk)
  ulg current_disk;
{
  FILE *f;
  int is_readable = 0;
  int i;
  char *split_dir = NULL;
  char *archive_name = NULL;
  char *split_name = NULL;
  char *split_path = NULL;
  char buf[SPLIT_MAXPATH + 100];

  /* get split path */
  split_path = get_in_split_path(in_path, current_disk);

  /* get the directory */
  if ((split_dir = malloc(strlen(in_path) + 40)) == NULL) {
    ZIPERR(ZE_MEM, "split path");
  }
  strcpy(split_dir, in_path);

  /* remove any name at end */
  for (i = strlen(split_dir) - 1; i >= 0; i--) {
    if (split_dir[i] == '/' || split_dir[i] == '\\'
          || split_dir[i] == ':') {
      split_dir[i + 1] = '\0';
      break;
    }
  }
  if (i < 0)
    split_dir[0] = '\0';

  /* get the name of the archive */
  if ((archive_name = malloc(strlen(in_path) + 1)) == NULL) {
    ZIPERR(ZE_MEM, "split path");
  }
  if (strlen(in_path) == strlen(split_dir)) {
    archive_name[0] = '\0';
  } else {
    strcpy(archive_name, in_path + strlen(split_dir));
  }

  /* get the name of the split */
  if ((split_name = malloc(strlen(split_path) + 1)) == NULL) {
    ZIPERR(ZE_MEM, "split path");
  }
  if (strlen(in_path) == strlen(split_dir)) {
    split_name[0] = '\0';
  } else {
    strcpy(split_name, split_path + strlen(split_dir));
  }
  if (i < 0) {
    strcpy(split_dir, "(current directory)");
  }

  fprintf(mesg, "\n\nCould not find:\n");
  fprintf(mesg, "  %s\n", split_path);
  /*
  fprintf(mesg, "Please enter the path directory (. for cur dir) where\n");
  fprintf(mesg, "  %s\n", split_name);
  fprintf(mesg, "is located\n");
  */
  for (;;) {
    if (is_readable) {
      fprintf(mesg, "\nHit c      (change path to where this split file is)");
      fprintf(mesg, "\n    q      (abort archive - quit)");
      fprintf(mesg, "\n or ENTER  (continue with this split): ");
    } else {
      if (fix == 1) {
        fprintf(mesg, "\nHit c      (change path to where this split file is)");
        fprintf(mesg, "\n    s      (skip this split)");
        fprintf(mesg, "\n    q      (abort archive - quit)");
        fprintf(mesg, "\n or ENTER  (try reading this split again): ");
      } else if (fix == 2) {
        fprintf(mesg, "\nHit c      (change path to where this split file is)");
        fprintf(mesg, "\n    s      (skip this split)");
        fprintf(mesg, "\n    q      (abort archive - quit)");
        fprintf(mesg, "\n    e      (end this archive - no more splits)");
        fprintf(mesg, "\n    z      (look for .zip split - the last split)");
        fprintf(mesg, "\n or ENTER  (try reading this split again): ");
      } else {
        fprintf(mesg, "\nHit c      (change path to where this split file is)");
        fprintf(mesg, "\n    q      (abort archive - quit)");
        fprintf(mesg, "\n or ENTER  (try reading this split again): ");
      }
    }
    fflush(mesg);
    fgets(buf, SPLIT_MAXPATH, stdin);
    /* remove any newline */
    for (i = 0; buf[i]; i++) {
      if (buf[i] == '\n') {
        buf[i] = '\0';
        break;
      }
    }
    if (toupper(buf[0]) == 'Q') {
      return ZE_ABORT;
    } else if ((fix == 1 || fix == 2) && toupper(buf[0]) == 'S') {
    /*
      fprintf(mesg, "\nSkip this split/disk?  (files in this split will not be recovered) [n/y] ");
      fflush(mesg);
      fgets(buf, SPLIT_MAXPATH, stdin);
      if (buf[0] == 'y' || buf[0] == 'Y') {
    */
      skip_this_disk = current_in_disk + 1;
      return ZE_FORM;
    } else if (toupper(buf[0]) == 'C') {
      fprintf(mesg, "\nEnter path where this split is (ENTER = same dir, . = current dir)");
      fprintf(mesg, "\n: ");
      fflush(mesg);
      fgets(buf, SPLIT_MAXPATH, stdin);
      is_readable = 0;
      /* remove any newline */
      for (i = 0; buf[i]; i++) {
        if (buf[i] == '\n') {
          buf[i] = '\0';
          break;
        }
      }
      if (buf[0] == '\0') {
        /* Hit ENTER so try old path again - could be removable media was changed */
        strcpy(buf, split_path);
      }
    } else if (fix == 2 && toupper(buf[0]) == 'E') {
      /* no more splits to read */
      return ZE_EOF;
    } else if (fix == 2 && toupper(buf[0]) == 'Z') {
      total_disks = current_disk + 1;
      free(split_path);
      split_path = get_in_split_path(in_path, current_disk);
      buf[0] = '\0';
      strncat(buf, split_path, SPLIT_MAXPATH);
    }
    if (strlen(buf) > 0) {
      /* changing path */

      /* check if user wants current directory */
      if (buf[0] == '.' && buf[1] == '\0') {
        buf[0] = '\0';
      }
      /* remove any name at end */
      for (i = strlen(buf); i >= 0; i--) {
        if (buf[i] == '/' || buf[i] == '\\'
             || buf[i] == ':') {
          buf[i + 1] = '\0';
          break;
        }
      }
      /* update base_path to newdir/split_name - in_path is the .zip file path */
      free(in_path);
      if (i < 0) {
        /* just name so current directory */
        strcpy(buf, "(current directory)");
        if (archive_name == NULL) {
          i = 0;
        } else {
          i = strlen(archive_name);
        }
        if ((in_path = malloc(strlen(archive_name) + 40)) == NULL) {
          ZIPERR(ZE_MEM, "split path");
        }
        strcpy(in_path, archive_name);
      } else {
        /* not the current directory */
        /* remove any name at end */
        for (i = strlen(buf); i >= 0; i--) {
          if (buf[i] == '/') {
            buf[i + 1] = '\0';
            break;
          }
        }
        if (i < 0) {
          buf[0] = '\0';
        }
        if ((in_path = malloc(strlen(buf) + strlen(archive_name) + 40)) == NULL) {
          ZIPERR(ZE_MEM, "split path");
        }
        strcpy(in_path, buf);
        strcat(in_path, archive_name);
      }

      free(split_path);

      /* get split path */
      split_path = get_in_split_path(in_path, current_disk);

      free(split_dir);
      if ((split_dir = malloc(strlen(in_path) + 40)) == NULL) {
        ZIPERR(ZE_MEM, "split path");
      }
      strcpy(split_dir, in_path);
      /* remove any name at end */
      for (i = strlen(split_dir); i >= 0; i--) {
        if (split_dir[i] == '/') {
          split_dir[i + 1] = '\0';
          break;
        }
      }

      /* try to open it */
      if ((f = fopen(split_path, "r")) == NULL) {
        fprintf(mesg, "\nCould not find or open\n");
        fprintf(mesg, "  %s\n", split_path);
        /*
        fprintf(mesg, "Please enter the path (. for cur dir) where\n");
        fprintf(mesg, "  %s\n", split_name);
        fprintf(mesg, "is located\n");
        */
        continue;
      }
      fclose(f);
      is_readable = 1;
      fprintf(mesg, "Found:  %s\n", split_path);
    } else {
      /* try to open it */
      if ((f = fopen(split_path, "r")) == NULL) {
        fprintf(mesg, "\nCould not find or open\n");
        fprintf(mesg, "  %s\n", split_path);
        /*
        fprintf(mesg, "Please enter the path (. for cur dir) where\n");
        fprintf(mesg, "  %s\n", split_name);
        fprintf(mesg, "is located\n");
        */
        continue;
      }
      fclose(f);
      is_readable = 1;
      fprintf(mesg, "\nFound:  %s\n", split_path);
      break;
    }
  }
  free(archive_name);
  free(split_dir);
  free(split_name);

  return ZE_OK;
}


/* ask_for_split_write_path
 *
 * Verify the directory for the next split.  Called
 * when -sp is used to pause between writing splits.
 *
 * Updates out_path and return 1 if OK or 0 if cancel
 */
int ask_for_split_write_path(current_disk)
  ulg current_disk;
{
  unsigned int num = (unsigned int)current_disk + 1;
  int i;
  char *split_dir = NULL;
  char *split_name = NULL;
  char buf[FNMAX + 40];

  /* get the directory */
  if ((split_dir = malloc(strlen(out_path) + 40)) == NULL) {
    ZIPERR(ZE_MEM, "split path");
  }
  strcpy(split_dir, out_path);

  /* remove any name at end */
  for (i = strlen(split_dir); i >= 0; i--) {
    if (split_dir[i] == '/' || split_dir[i] == '\\'
          || split_dir[i] == ':') {
      split_dir[i + 1] = '\0';
      break;
    }
  }

  /* get the name of the split */
  if ((split_name = malloc(strlen(out_path) + 1)) == NULL) {
    ZIPERR(ZE_MEM, "split path");
  }
  if (strlen(out_path) == strlen(split_dir)) {
    split_name[0] = '\0';
  } else {
    strcpy(split_name, out_path + strlen(split_dir));
  }
  if (i < 0) {
    strcpy(split_dir, "(current directory)");
  }
  if (mesg_line_started)
    fprintf(mesg, "\n");
  fprintf(mesg, "\nOpening disk %d\n", num);
  fprintf(mesg, "Hit ENTER to write to default path of\n");
  fprintf(mesg, "  %s\n", split_dir);
  fprintf(mesg, "or enter a new directory path (. for cur dir) and hit ENTER\n");
  for (;;) {
    fprintf(mesg, "\nPath (or hit ENTER to continue): ");
    fflush(mesg);
    fgets(buf, FNMAX, stdin);
    /* remove any newline */
    for (i = 0; buf[i]; i++) {
      if (buf[i] == '\n') {
        buf[i] = '\0';
        break;
      }
    }
    if (strlen(buf) > 0) {
      /* changing path */

      /* current directory */
      if (buf[0] == '.' && buf[1] == '\0') {
        buf[0] = '\0';
      }
      /* remove any name at end */
      for (i = strlen(buf); i >= 0; i--) {
        if (buf[i] == '/' || buf[i] == '\\'
             || buf[i] == ':') {
          buf[i + 1] = '\0';
          break;
        }
      }
      /* update out_path to newdir/split_name */
      free(out_path);
      if (i < 0) {
        /* just name so current directory */
        strcpy(buf, "(current directory)");
        if (split_name == NULL) {
          i = 0;
        } else {
          i = strlen(split_name);
        }
        if ((out_path = malloc(strlen(split_name) + 40)) == NULL) {
          ZIPERR(ZE_MEM, "split path");
        }
        strcpy(out_path, split_name);
      } else {
        /* not the current directory */
        /* remove any name at end */
        for (i = strlen(buf); i >= 0; i--) {
          if (buf[i] == '/') {
            buf[i + 1] = '\0';
            break;
          }
        }
        if (i < 0) {
          buf[0] = '\0';
        }
        if ((out_path = malloc(strlen(buf) + strlen(split_name) + 40)) == NULL) {
          ZIPERR(ZE_MEM, "split path");
        }
        strcpy(out_path, buf);
        strcat(out_path, split_name);
      }
      fprintf(mesg, "Writing to:\n  %s\n", buf);
      free(split_name);
      free(split_dir);
      if ((split_dir = malloc(strlen(out_path) + 40)) == NULL) {
        ZIPERR(ZE_MEM, "split path");
      }
      strcpy(split_dir, out_path);
      /* remove any name at end */
      for (i = strlen(split_dir); i >= 0; i--) {
        if (split_dir[i] == '/') {
          split_dir[i + 1] = '\0';
          break;
        }
      }
      if ((split_name = malloc(strlen(out_path) + 1)) == NULL) {
        ZIPERR(ZE_MEM, "split path");
      }
      strcpy(split_name, out_path + strlen(split_dir));
    } else {
      break;
    }
  }
  free(split_dir);
  free(split_name);

  /* for now no way out except Ctrl C */
  return 1;
}


/* split_name
 *
 * get name of split being read
 */
char *get_in_split_path(base_path, disk_number)
  char *base_path;
  ulg disk_number;
{
  char *split_path = NULL;
  int base_len = 0;
  int path_len = 0;
  ulg num = disk_number + 1;
  char ext[6];
#ifdef VMS
  int vers_len;                         /* File version length. */
  char *vers_ptr;                       /* File version string. */
#endif /* def VMS */

  /*
   * A split has extension z01, z02, ..., z99, z100, z101, ... z999
   * We currently support up to .z99999
   * WinZip will also read .100, .101, ... but AppNote 6.2.2 uses above
   * so use that.  Means on DOS can only have 100 splits.
   */

  if (num == total_disks) {
    /* last disk is base path */
    if ((split_path = malloc(strlen(base_path) + 1)) == NULL) {
      ZIPERR(ZE_MEM, "base path");
    }
    strcpy(split_path, base_path);

    return split_path;
  } else {
    if (num > 99999) {
      ZIPERR(ZE_BIG, "More than 99999 splits needed");
    }
    sprintf(ext, "z%02lu", num);
  }

  /* create path for this split - zip.c checked for .zip extension */
  base_len = strlen(base_path) - 3;
  path_len = base_len + strlen(ext);

#ifdef VMS
  /* On VMS, locate the file version, and adjust base_len accordingly.
     Note that path_len is correct, as-is.
  */
  vers_ptr = vms_file_version( base_path);
  vers_len = strlen( vers_ptr);
  base_len -= vers_len;
#endif /* def VMS */

  if ((split_path = malloc(path_len + 1)) == NULL) {
    ZIPERR(ZE_MEM, "split path");
  }
  /* copy base_path except for end zip */
  strcpy(split_path, base_path);
  split_path[base_len] = '\0';
  /* add extension */
  strcat(split_path, ext);

#ifdef VMS
  /* On VMS, append (preserve) the file version. */
  strcat(split_path, vers_ptr);
#endif /* def VMS */

  return split_path;
}


/* split_name
 *
 * get name of split being written
 */
char *get_out_split_path(base_path, disk_number)
  char *base_path;
  ulg disk_number;
{
  char *split_path = NULL;
  int base_len = 0;
  int path_len = 0;
  ulg num = disk_number + 1;
  char ext[6];
#ifdef VMS
  int vers_len;                         /* File version length. */
  char *vers_ptr;                       /* File version string. */
#endif /* def VMS */

  /*
   * A split has extension z01, z02, ..., z99, z100, z101, ... z999
   * We currently support up to .z99999
   * WinZip will also read .100, .101, ... but AppNote 6.2.2 uses above
   * so use that.  Means on DOS can only have 100 splits.
   */

  if (num > 99999) {
    ZIPERR(ZE_BIG, "More than 99999 splits needed");
  }
  sprintf(ext, "z%02lu", num);

  /* create path for this split - zip.c checked for .zip extension */
  base_len = strlen(base_path) - 3;
  path_len = base_len + strlen(ext);

#ifdef VMS
  /* On VMS, locate the file version, and adjust base_len accordingly.
     Note that path_len is correct, as-is.
  */
  vers_ptr = vms_file_version( base_path);
  vers_len = strlen( vers_ptr);
  base_len -= vers_len;
#endif /* def VMS */

  if ((split_path = malloc(path_len + 1)) == NULL) {
    ZIPERR(ZE_MEM, "split path");
  }
  /* copy base_path except for end zip */
  strcpy(split_path, base_path);
  split_path[base_len] = '\0';
  /* add extension */
  strcat(split_path, ext);

#ifdef VMS
  /* On VMS, append (preserve) the file version. */
  strcat(split_path, vers_ptr);
#endif /* def VMS */

  return split_path;
}

/* close_split
 *
 * close a split - assume that the paths needed for the splits are
 * available.
 */
int close_split(disk_number, tempfile, temp_name)
  ulg disk_number;
  FILE *tempfile;
  char *temp_name;
{
  char *split_path = NULL;

  split_path = get_out_split_path(out_path, disk_number);

  if (noisy_splits) {
    zipmessage("\tClosing split ", split_path);
  }

  fclose(tempfile);

  rename_split(temp_name, split_path);
  set_filetype(split_path);

  return ZE_OK;
}

/* bfwrite
   Does the fwrite but also counts bytes and does splits */
size_t bfwrite(buffer, size, count, mode)
  ZCONST void *buffer;
  size_t size;
  size_t count;
  int mode;
{
  size_t bytes_written = 0;
  size_t r;
  size_t b = size * count;
  uzoff_t bytes_left_in_split = 0;
  size_t bytes_to_write = b;


  /* -------------------------------- */
  /* local header */
  if (mode == BFWRITE_LOCALHEADER) {
    /* writing local header - reset entry data count */
    bytes_this_entry = 0;
    /* save start of local header so we can rewrite later */
    current_local_file = y;
    current_local_disk = current_disk;
    current_local_offset = bytes_this_split;
  }

  if (split_size == 0)
    bytes_left_in_split = bytes_to_write;
  else
    bytes_left_in_split = split_size - bytes_this_split;

  if (bytes_to_write > bytes_left_in_split) {
    if (mode == BFWRITE_HEADER ||
        mode == BFWRITE_LOCALHEADER ||
        mode == BFWRITE_CENTRALHEADER) {
      /* if can't write entire header save for next split */
      bytes_to_write = 0;
    } else {
      /* normal data so fill the split */
      bytes_to_write = (size_t)bytes_left_in_split;
    }
  }

  /* -------------------------------- */
  /* central header */
  if (mode == BFWRITE_CENTRALHEADER) {
    /* set start disk for CD */
    if (cd_start_disk == (ulg)-1) {
      cd_start_disk = current_disk;
      cd_start_offset = bytes_this_split;
    }
    cd_entries_this_disk++;
    total_cd_entries++;
  }

  /* -------------------------------- */
  if (bytes_to_write > 0) {
    /* write out the bytes for this split */
    r = fwrite(buffer, size, bytes_to_write, y);
    bytes_written += r;
    bytes_to_write = b - r;
    bytes_this_split += r;
    if (mode == BFWRITE_DATA)
      /* if data descriptor do not include in count */
      bytes_this_entry += r;
  } else {
    bytes_to_write = b;
  }

  if (bytes_to_write > 0) {
    if (split_method) {
      /* still bytes to write so close split and open next split */
      bytes_prev_splits += bytes_this_split;

      if (split_method == 1 && ferror(y)) {
        /* if writing all splits to same place and have problem then bad */
        ZIPERR(ZE_WRITE, "Could not write split");
      }

      if (split_method == 2 && ferror(y)) {
        /* A split must be at least 64K except last .zip split */
        if (bytes_this_split < 64 * (uzoff_t)0x400) {
          ZIPERR(ZE_WRITE, "Not enough space to write split");
        }
      }

      /* close this split */
      if (split_method == 1 && current_local_disk == current_disk) {
        /* keep split open so can update it */
        current_local_tempname = tempzip;
      } else {
        /* close split */
        close_split(current_disk, y, tempzip);
        y = NULL;
        free(tempzip);
        tempzip = NULL;
      }
      cd_entries_this_disk = 0;
      bytes_this_split = 0;

      /* increment disk - disks are numbered 0, 1, 2, ... and
         splits are 01, 02, ... */
      current_disk++;

      if (split_method == 2 && split_bell) {
        /* bell when pause to ask for next split */
        putc('\007', mesg);
        fflush(mesg);
      }

      for (;;) {
        /* if method 2 pause and allow changing path */
        if (split_method == 2) {
          if (ask_for_split_write_path(current_disk) == 0) {
            ZIPERR(ZE_ABORT, "could not write split");
          }
        }

        /* open next split */
#if defined(UNIX) && !defined(NO_MKSTEMP)
        {
          int yd;
          int i;

          /* use mkstemp to avoid race condition and compiler warning */

          if (tempath != NULL)
          {
            /* if -b used to set temp file dir use that for split temp */
            if ((tempzip = malloc(strlen(tempath) + 12)) == NULL) {
              ZIPERR(ZE_MEM, "allocating temp filename");
            }
            strcpy(tempzip, tempath);
            if (lastchar(tempzip) != '/')
              strcat(tempzip, "/");
          }
          else
          {
            /* create path by stripping name and appending template */
            if ((tempzip = malloc(strlen(zipfile) + 12)) == NULL) {
            ZIPERR(ZE_MEM, "allocating temp filename");
            }
            strcpy(tempzip, zipfile);
            for(i = strlen(tempzip); i > 0; i--) {
              if (tempzip[i - 1] == '/')
                break;
            }
            tempzip[i] = '\0';
          }
          strcat(tempzip, "ziXXXXXX");

          if ((yd = mkstemp(tempzip)) == EOF) {
            ZIPERR(ZE_TEMP, tempzip);
          }
          if ((y = fdopen(yd, FOPW_TMP)) == NULL) {
            ZIPERR(ZE_TEMP, tempzip);
          }
        }
#else
        if ((tempzip = tempname(zipfile)) == NULL) {
          ZIPERR(ZE_MEM, "allocating temp filename");
        }
        if ((y = zfopen(tempzip, FOPW_TMP)) == NULL) {
          ZIPERR(ZE_TEMP, tempzip);
        }
#endif

        r = fwrite((char *)buffer + bytes_written, 1, bytes_to_write, y);
        bytes_written += r;
        bytes_this_split += r;
        if (!(mode == BFWRITE_HEADER ||
              mode == BFWRITE_LOCALHEADER ||
              mode == BFWRITE_CENTRALHEADER)) {
          bytes_this_entry += r;
        }
        if (bytes_to_write > r) {
          /* buffer bigger than split */
          if (split_method == 2) {
            /* let user choose another disk */
            zipwarn("Not enough room on disk", "");
            continue;
          } else {
            ZIPERR(ZE_WRITE, "Not enough room on disk");
          }
        }
        if (mode == BFWRITE_LOCALHEADER ||
            mode == BFWRITE_HEADER ||
            mode == BFWRITE_CENTRALHEADER) {
          if (split_method == 1 && current_local_file &&
              current_local_disk != current_disk) {
            /* We're opening a new split because the next header
               did not fit on the last split.  We need to now close
               the last split and update the pointers for
               the current split. */
            close_split(current_local_disk, current_local_file,
                        current_local_tempname);
            free(current_local_tempname);
          }
          current_local_tempname = tempzip;
          current_local_file = y;
          current_local_offset = 0;
          current_local_disk = current_disk;
        }
        break;
      }
    }
    else
    {
      /* likely have more than fits but no splits */

      /* probably already have error "no space left on device" */
      /* could let flush_outbuf() handle error but bfwrite() is called for
         headers also */
      if (ferror(y))
        ziperr(ZE_WRITE, "write error on zip file");
    }
  }


  /* display dots for archive instead of for each file */
  if (display_globaldots) {
    if (dot_size > 0) {
      /* initial space */
      if (dot_count == -1) {
#ifndef WINDLL
        putc(' ', mesg);
        fflush(mesg);
#else
        fprintf(stdout,"%c",' ');
#endif
        /* assume a header will be written first, so avoid 0 */
        dot_count = 1;
      }
      /* skip incrementing dot count for small buffers like for headers */
      if (size * count > 1000) {
        dot_count++;
        if (dot_size <= dot_count * (zoff_t)size * (zoff_t)count) dot_count = 0;
      }
    }
    if (dot_size && !dot_count) {
      dot_count++;
#ifndef WINDLL
      putc('.', mesg);
      fflush(mesg);
#else
      fprintf(stdout,"%c",'.');
#endif
      mesg_line_started = 1;
    }
  }


  return bytes_written;
}


#ifdef UNICODE_SUPPORT

/*---------------------------------------------
 * Unicode conversion functions
 *
 * Provided by Paul Kienitz
 *
 * Some modifications to work with Zip
 *
 *---------------------------------------------
 */

/*
   NOTES APPLICABLE TO ALL STRING FUNCTIONS:

   All of the x_to_y functions take parameters for an output buffer and
   its available length, and return an int.  The value returned is the
   length of the string that the input produces, which may be larger than
   the provided buffer length.  If the returned value is less than the
   buffer length, then the contents of the buffer will be null-terminated;
   otherwise, it will not be terminated and may be invalid, possibly
   stopping in the middle of a multibyte sequence.

   In all cases you may pass NULL as the buffer and/or 0 as the length, if
   you just want to learn how much space the string is going to require.

   The functions will return -1 if the input is invalid UTF-8 or cannot be
   encoded as UTF-8.
*/

/* utility functions for managing UTF-8 and UCS-4 strings */


/* utf8_char_bytes
 *
 * Returns the number of bytes used by the first character in a UTF-8
 * string, or -1 if the UTF-8 is invalid or null.
 */
local int utf8_char_bytes(utf8)
  ZCONST char *utf8;
{
  int      t, r;
  unsigned lead;

  if (!utf8)
    return -1;          /* no input */
  lead = (unsigned char) *utf8;
  if (lead < 0x80)
    r = 1;              /* an ascii-7 character */
  else if (lead < 0xC0)
    return -1;          /* error: trailing byte without lead byte */
  else if (lead < 0xE0)
    r = 2;              /* an 11 bit character */
  else if (lead < 0xF0)
    r = 3;              /* a 16 bit character */
  else if (lead < 0xF8)
    r = 4;              /* a 21 bit character (the most currently used) */
  else if (lead < 0xFC)
    r = 5;              /* a 26 bit character (shouldn't happen) */
  else if (lead < 0xFE)
    r = 6;              /* a 31 bit character (shouldn't happen) */
  else
    return -1;          /* error: invalid lead byte */
  for (t = 1; t < r; t++)
    if ((unsigned char) utf8[t] < 0x80 || (unsigned char) utf8[t] >= 0xC0)
      return -1;        /* error: not enough valid trailing bytes */
  return r;
}


/* ucs4_char_from_utf8
 *
 * Given a reference to a pointer into a UTF-8 string, returns the next
 * UCS-4 character and advances the pointer to the next character sequence.
 * Returns ~0 and does not advance the pointer when input is ill-formed.
 *
 * Since the Unicode standard says 32-bit values won't be used (just
 * up to the current 21-bit mappings) changed this to signed to allow -1 to
 * be returned.
 */
long ucs4_char_from_utf8(utf8)
  ZCONST char **utf8;
{
  ulg  ret;
  int  t, bytes;

  if (!utf8)
    return -1;                          /* no input */
  bytes = utf8_char_bytes(*utf8);
  if (bytes <= 0)
    return -1;                          /* invalid input */
  if (bytes == 1)
    ret = **utf8;                       /* ascii-7 */
  else
    ret = **utf8 & (0x7F >> bytes);     /* lead byte of a multibyte sequence */
  (*utf8)++;
  for (t = 1; t < bytes; t++)           /* consume trailing bytes */
    ret = (ret << 6) | (*((*utf8)++) & 0x3F);
  return (long) ret;
}


/* utf8_from_ucs4_char - Convert UCS char to UTF-8
 *
 * Returns the number of bytes put into utf8buf to represent ch, from 1 to 6,
 * or -1 if ch is too large to represent.  utf8buf must have room for 6 bytes.
 */
local int utf8_from_ucs4_char(utf8buf, ch)
  char *utf8buf;
  ulg ch;
{
  int trailing = 0;
  int leadmask = 0x80;
  int leadbits = 0x3F;
  ulg tch = ch;
  int ret;

  if (ch > 0x7FFFFFFF)
    return -1;                /* UTF-8 can represent 31 bits */
  if (ch < 0x7F)
  {
    *utf8buf++ = (char) ch;   /* ascii-7 */
    return 1;
  }
  do {
    trailing++;
    leadmask = (leadmask >> 1) | 0x80;
    leadbits >>= 1;
    tch >>= 6;
  } while (tch & ~leadbits);
  ret = trailing + 1;
  /* produce lead byte */
  *utf8buf++ = (char) (leadmask | (ch >> (6 * trailing)));
  /* produce trailing bytes */
  while (--trailing >= 0)
    *utf8buf++ = (char) (0x80 | ((ch >> (6 * trailing)) & 0x3F));
  return ret;
}


/*===================================================================*/

/* utf8_to_ucs4_string - convert UTF-8 string to UCS string
 *
 * Return UCS count.  Now returns int so can return -1.
 */
local int utf8_to_ucs4_string(utf8, ucs4buf, buflen)
  ZCONST char *utf8;
  ulg *ucs4buf;
  int buflen;
{
  int count = 0;

  for (;;)
  {
    long ch = ucs4_char_from_utf8(&utf8);
    if (ch == -1)
      return -1;
    else
    {
      if (ucs4buf && count < buflen)
        ucs4buf[count] = ch;
      if (ch == 0)
        return count;
      count++;
    }
  }
}


/* ucs4_string_to_utf8
 *
 *
 */
local int ucs4_string_to_utf8(ucs4, utf8buf, buflen)
  ZCONST ulg *ucs4;
  char *utf8buf;
  int buflen;
{
  char mb[6];
  int  count = 0;

  if (!ucs4)
    return -1;
  for (;;)
  {
    int mbl = utf8_from_ucs4_char(mb, *ucs4++);
    int c;
    if (mbl <= 0)
      return -1;
    /* We could optimize this a bit by passing utf8buf + count */
    /* directly to utf8_from_ucs4_char when buflen >= count + 6... */
    c = buflen - count;
    if (mbl < c)
      c = mbl;
    if (utf8buf && count < buflen)
      strncpy(utf8buf + count, mb, c);
    if (mbl == 1 && !mb[0])
      return count;           /* terminating nul */
    count += mbl;
  }
}


#if 0  /* currently unused */
/* utf8_chars
 *
 * Wrapper: counts the actual unicode characters in a UTF-8 string.
 */
local int utf8_chars(utf8)
  ZCONST char *utf8;
{
  return utf8_to_ucs4_string(utf8, NULL, 0);
}
#endif


/* --------------------------------------------------- */
/* Unicode Support
 *
 * These functions common for all Unicode ports.
 *
 * These functions should allocate and return strings that can be
 * freed with free().
 *
 * 8/27/05 EG
 *
 * Use zwchar for wide char which is unsigned long
 * in zip.h and 32 bits.  This avoids problems with
 * different sizes of wchar_t.
 */

#ifdef WIN32

zwchar *wchar_to_wide_string(wchar_string)
  wchar_t *wchar_string;
{
  int i;
  int wchar_len;
  zwchar *wide_string;

  wchar_len = wcslen(wchar_string);

  if ((wide_string = malloc((wchar_len + 1) * sizeof(zwchar))) == NULL) {
    ZIPERR(ZE_MEM, "wchar to wide conversion");
  }
  for (i = 0; i <= wchar_len; i++) {
    wide_string[i] = wchar_string[i];
  }

  return wide_string;
}

/* is_ascii_stringw
 * Checks if a wide string is all ascii
 */
int is_ascii_stringw(wstring)
  wchar_t *wstring;
{
  wchar_t *pw;
  wchar_t cw;

  if (wstring == NULL)
    return 0;

  for (pw = wstring; (cw = *pw) != '\0'; pw++) {
    if (cw > 0x7F) {
      return 0;
    }
  }
  return 1;
}

#endif

/* is_ascii_string
 * Checks if a string is all ascii
 */
int is_ascii_string(mbstring)
  char *mbstring;
{
  char *p;
  uch c;

  if (mbstring == NULL)
    return 0;

  for (p = mbstring; (c = (uch)*p) != '\0'; p++) {
    if (c > 0x7F) {
      return 0;
    }
  }
  return 1;
}

/* local to UTF-8 */
char *local_to_utf8_string(local_string)
  char *local_string;
{
  zwchar *wide_string = local_to_wide_string(local_string);
  char *utf8_string = wide_to_utf8_string(wide_string);

  free(wide_string);
  return utf8_string;
}

/* wide_char_to_escape_string
   provides a string that represents a wide char not in local char set

   An initial try at an algorithm.  Suggestions welcome.

   If not an ASCII char, probably need 2 bytes at least.  So if
   a 2-byte wide encode it as 4 hex digits with a leading #U.
   Since the Unicode standard has been frozen, it looks like 3 bytes
   should be enough for any large Unicode character.  In these cases
   prefix the string with #L.
   So
   #U1234
   is a 2-byte wide character with bytes 0x12 and 0x34 while
   #L123456
   is a 3-byte wide with bytes 0x12, 0x34, and 0x56.
   On Windows, wide that need two wide characters as a surrogate pair
   to represent them need to be converted to a single number.
  */

 /* set this to the max bytes an escape can be */
#define MAX_ESCAPE_BYTES 8

char *wide_char_to_escape_string(wide_char)
  zwchar wide_char;
{
  int i;
  zwchar w = wide_char;
  uch b[9];
  char e[7];
  int len;
  char *r;

  /* fill byte array with zeros */
  for (len = 0; len < sizeof(zwchar); len++) {
    b[len] = 0;
  }
  /* get bytes in right to left order */
  for (len = 0; w; len++) {
    b[len] = (char)(w % 0x100);
    w /= 0x100;
  }

  if ((r = malloc(MAX_ESCAPE_BYTES + 8)) == NULL) {
    ZIPERR(ZE_MEM, "wide_char_to_escape_string");
  }
  strcpy(r, "#");
  /* either 2 bytes or 4 bytes */
  if (len < 3) {
    len = 2;
    strcat(r, "U");
  } else {
    len = 3;
    strcat(r, "L");
  }
  for (i = len - 1; i >= 0; i--) {
    sprintf(e, "%02x", b[i]);
    strcat(r, e);
  }
  return r;
}

#if 0
/* returns the wide character represented by the escape string */
zwchar escape_string_to_wide(escape_string)
  char *escape_string;
{
  int i;
  zwchar w;
  char c;
  char u;
  int len;
  char *e = escape_string;

  if (e == NULL) {
    return 0;
  }
  if (e[0] != '#') {
    /* no leading # */
    return 0;
  }
  len = strlen(e);
  /* either #U1234 or #L123456 format */
  if (len != 6 && len != 8) {
    return 0;
  }
  w = 0;
  if (e[1] == 'L') {
    if (len != 8) {
      return 0;
    }
    /* 3 bytes */
    for (i = 2; i < 8; i++) {
      c = e[i];
      u = toupper(c);
      if (u >= 'A' && u <= 'F') {
        w = w * 0x10 + (zwchar)(u + 10 - 'A');
      } else if (c >= '0' && c <= '9') {
        w = w * 0x10 + (zwchar)(c - '0');
      } else {
        return 0;
      }
    }
  } else if (e[1] == 'U') {
    /* 2 bytes */
    for (i = 2; i < 6; i++) {
      c = e[i];
      u = toupper(c);
      if (u >= 'A' && u <= 'F') {
        w = w * 0x10 + (zwchar)(u + 10 - 'A');
      } else if (c >= '0' && c <= '9') {
        w = w * 0x10 + (zwchar)(c - '0');
      } else {
        return 0;
      }
    }
  }
  return w;
}
#endif


char *local_to_escape_string(local_string)
  char *local_string;
{
  zwchar *wide_string = local_to_wide_string(local_string);
  char *escape_string = wide_to_escape_string(wide_string);

  free(wide_string);
  return escape_string;
}

#ifdef WIN32
char *wchar_to_local_string(wstring)
  wchar_t *wstring;
{
  zwchar *wide_string = wchar_to_wide_string(wstring);
  char *local_string = wide_to_local_string(wide_string);

  free(wide_string);

  return local_string;
}
#endif


#ifndef WIN32   /* The Win32 port uses a system-specific variant. */
/* convert wide character string to multi-byte character string */
char *wide_to_local_string(wide_string)
  zwchar *wide_string;
{
  int i;
  wchar_t wc;
  int b;
  int state_dependent;
  int wsize = 0;
  int max_bytes = MB_CUR_MAX;
  char buf[9];
  char *buffer = NULL;
  char *local_string = NULL;

  for (wsize = 0; wide_string[wsize]; wsize++) ;

  if (MAX_ESCAPE_BYTES > max_bytes)
    max_bytes = MAX_ESCAPE_BYTES;

  if ((buffer = (char *)malloc(wsize * max_bytes + 1)) == NULL) {
    ZIPERR(ZE_MEM, "wide_to_local_string");
  }

  /* convert it */
  buffer[0] = '\0';
  /* set initial state if state-dependent encoding */
  wc = (wchar_t)'a';
  b = wctomb(NULL, wc);
  if (b == 0)
    state_dependent = 0;
  else
    state_dependent = 1;
  for (i = 0; i < wsize; i++) {
    if (sizeof(wchar_t) < 4 && wide_string[i] > 0xFFFF) {
      /* wchar_t probably 2 bytes */
      /* could do surrogates if state_dependent and wctomb can do */
      wc = zwchar_to_wchar_t_default_char;
    } else {
      wc = (wchar_t)wide_string[i];
    }
    b = wctomb(buf, wc);
    if (unicode_escape_all) {
      if (b == 1 && (uch)buf[0] <= 0x7f) {
        /* ASCII */
        strncat(buffer, buf, b);
      } else {
        /* use escape for wide character */
        char *e = wide_char_to_escape_string(wide_string[i]);
        strcat(buffer, e);
        free(e);
      }
    } else if (b > 0) {
      /* multi-byte char */
      strncat(buffer, buf, b);
    } else {
      /* no MB for this wide */
      if (use_wide_to_mb_default) {
        /* default character */
        strcat(buffer, wide_to_mb_default_string);
      } else {
        /* use escape for wide character */
        char *e = wide_char_to_escape_string(wide_string[i]);
        strcat(buffer, e);
        free(e);
      }
    }
  }
  if ((local_string = (char *)malloc(strlen(buffer) + 1)) == NULL) {
    free(buffer);
    ZIPERR(ZE_MEM, "wide_to_local_string");
  }
  strcpy(local_string, buffer);
  free(buffer);

  return local_string;
}
#endif /* !WIN32 */


/* convert wide character string to escaped string */
char *wide_to_escape_string(wide_string)
  zwchar *wide_string;
{
  int i;
  int wsize = 0;
  char buf[9];
  char *buffer = NULL;
  char *escape_string = NULL;

  for (wsize = 0; wide_string[wsize]; wsize++) ;

  if ((buffer = (char *)malloc(wsize * MAX_ESCAPE_BYTES + 1)) == NULL) {
    ZIPERR(ZE_MEM, "wide_to_escape_string");
  }

  /* convert it */
  buffer[0] = '\0';
  for (i = 0; i < wsize; i++) {
    if (wide_string[i] <= 0x7f && isprint((char)wide_string[i])) {
      /* ASCII */
      buf[0] = (char)wide_string[i];
      buf[1] = '\0';
      strcat(buffer, buf);
    } else {
      /* use escape for wide character */
      char *e = wide_char_to_escape_string(wide_string[i]);
      strcat(buffer, e);
      free(e);
    }
  }
  if ((escape_string = (char *)malloc(strlen(buffer) + 1)) == NULL) {
    ZIPERR(ZE_MEM, "wide_to_escape_string");
  }
  strcpy(escape_string, buffer);
  free(buffer);

  return escape_string;
}


/* convert local string to display character set string */
char *local_to_display_string(local_string)
  char *local_string;
{
  char *temp_string;
  char *display_string;

  /* For Windows, OEM string should never be bigger than ANSI string, says
     CharToOem description.
     On UNIX, non-printable characters (0x00 - 0xFF) will be replaced by
     "^x", so more space may be needed.  Note that "^" itself is a valid
     name character, so this leaves an ambiguity, but UnZip displays
     names this way, too.  (0x00 is not possible, I hope.)
     For all other ports, just make a copy of local_string.
  */

#ifdef UNIX
  char *cp_dst;                 /* Character pointers used in the */
  char *cp_src;                 /*  copying/changing procedure.   */
#endif

  if ((temp_string = (char *)malloc(2 * strlen(local_string) + 1)) == NULL) {
    ZIPERR(ZE_MEM, "local_to_display_string");
  }

#ifdef WIN32
  /* convert to OEM display character set */
  local_to_oem_string(temp_string, local_string);
#else
# ifdef UNIX
  /* Copy source string, expanding non-printable characters to "^x". */
  cp_dst = temp_string;
  cp_src = local_string;
  while (*cp_src != '\0') {
    if ((unsigned char)*cp_src < ' ') {
      *cp_dst++ = '^';
      *cp_dst++ = '@'+ *cp_src++;
    }
    else {
      *cp_dst++ = *cp_src++;
    }
  }
  *cp_dst = '\0';
# else /* not UNIX */
  strcpy(temp_string, local_string);
# endif /* UNIX */
#endif

#ifdef EBCDIC
  {
    char *ebc;

    if ((ebc = malloc(strlen(display_string) + 1)) ==  NULL) {
      ZIPERR(ZE_MEM, "local_to_display_string");
    }
    strtoebc(ebc, display_string);
    free(display_string);
    display_string = ebc;
  }
#endif

  if ((display_string = (char *)malloc(strlen(temp_string) + 1)) == NULL) {
    ZIPERR(ZE_MEM, "local_to_display_string");
  }
  strcpy(display_string, temp_string);
  free(temp_string);

  return display_string;
}

/* UTF-8 to local */
char *utf8_to_local_string(utf8_string)
  char *utf8_string;
{
  zwchar *wide_string = utf8_to_wide_string(utf8_string);
  char *loc = wide_to_local_string(wide_string);
  if (wide_string)
    free(wide_string);
  return loc;
}

/* UTF-8 to local */
char *utf8_to_escape_string(utf8_string)
  char *utf8_string;
{
  zwchar *wide_string = utf8_to_wide_string(utf8_string);
  char *escape_string = wide_to_escape_string(wide_string);
  free(wide_string);
  return escape_string;
}

#ifndef WIN32   /* The Win32 port uses a system-specific variant. */
/* convert multi-byte character string to wide character string */
zwchar *local_to_wide_string(local_string)
  char *local_string;
{
  int wsize;
  wchar_t *wc_string;
  zwchar *wide_string;

  /* for now try to convert as string - fails if a bad char in string */
  wsize = mbstowcs(NULL, local_string, MB_CUR_MAX );
  if (wsize == (size_t)-1) {
    /* could not convert */
    return NULL;
  }

  /* convert it */
  if ((wc_string = (wchar_t *)malloc((wsize + 1) * sizeof(wchar_t))) == NULL) {
    ZIPERR(ZE_MEM, "local_to_wide_string");
  }
  wsize = mbstowcs(wc_string, local_string, strlen(local_string) + 1);
  wc_string[wsize] = (wchar_t) 0;

  /* in case wchar_t is not zwchar */
  if ((wide_string = (zwchar *)malloc((wsize + 1) * sizeof(zwchar))) == NULL) {
    ZIPERR(ZE_MEM, "local_to_wide_string");
  }
  for (wsize = 0; (wide_string[wsize] = (zwchar)wc_string[wsize]); wsize++) ;
  wide_string[wsize] = (zwchar)0;
  free(wc_string);

  return wide_string;
}
#endif /* !WIN32 */


#if 0
/* All wchar functions are only used by Windows and are
   now in win32zip.c so that the Windows functions can
   be used and multiple character wide characters can
   be handled easily. */
# ifndef WIN32
char *wchar_to_utf8_string(wstring)
  wchar_t *wstring;
{
  zwchar *wide_string = wchar_to_wide_string(wstring);
  char *local_string = wide_to_utf8_string(wide_string);

  free(wide_string);

  return local_string;
}
# endif
#endif


/* convert wide string to UTF-8 */
char *wide_to_utf8_string(wide_string)
  zwchar *wide_string;
{
  int mbcount;
  char *utf8_string;

  /* get size of utf8 string */
  mbcount = ucs4_string_to_utf8(wide_string, NULL, 0);
  if (mbcount == -1)
    return NULL;
  if ((utf8_string = (char *) malloc(mbcount + 1)) == NULL) {
    ZIPERR(ZE_MEM, "wide_to_utf8_string");
  }
  mbcount = ucs4_string_to_utf8(wide_string, utf8_string, mbcount + 1);
  if (mbcount == -1)
    return NULL;

  return utf8_string;
}

/* convert UTF-8 string to wide string */
zwchar *utf8_to_wide_string(utf8_string)
  char *utf8_string;
{
  int wcount;
  zwchar *wide_string;

  wcount = utf8_to_ucs4_string(utf8_string, NULL, 0);
  if (wcount == -1)
    return NULL;
  if ((wide_string = (zwchar *) malloc((wcount + 2) * sizeof(zwchar))) == NULL) {
    ZIPERR(ZE_MEM, "utf8_to_wide_string");
  }
  wcount = utf8_to_ucs4_string(utf8_string, wide_string, wcount + 1);

  return wide_string;
}


#endif /* UNICODE_SUPPORT */


/*---------------------------------------------------------------
 *  Long option support
 *  8/23/2003
 *
 *  Defines function get_option() to get and process the command
 *  line options and arguments from argv[].  The caller calls
 *  get_option() in a loop to get either one option and possible
 *  value or a non-option argument each loop.
 *
 *  This version does not include argument file support and can
 *  work directly on argv.  The argument file code complicates things and
 *  it seemed best to leave it out for now.  If argument file support (reading
 *  in command line arguments stored in a file and inserting into
 *  command line where @filename is found) is added later the arguments
 *  can change and a freeable copy of argv will be needed and can be
 *  created using copy_args in the left out code.
 *
 *  Supports short and long options as defined in the array options[]
 *  in zip.c, multiple short options in an argument (like -jlv), long
 *  option abbreviation (like --te for --temp-file if --te unique),
 *  short and long option values (like -b filename or --temp-file filename
 *  or --temp-file=filename), optional and required values, option negation
 *  by trailing - (like -S- to not include hidden and system files in MSDOS),
 *  value lists (like -x a b c), argument permuting (returning all options
 *  and values before any non-option arguments), and argument files (where any
 *  non-option non-value argument in form @path gets substituted with the
 *  white space separated arguments in the text file at path).  In this
 *  version argument file support has been removed to simplify development but
 *  may be added later.
 *
 *  E. Gordon
 */


/* message output - char casts are needed to handle constants */
#define oWARN(message) zipwarn((char *) message, "")
#define oERR(err,message) ZIPERR(err, (char *) message)


/* Although the below provides some support for multibyte characters
   the proper thing to do may be to use wide characters and support
   Unicode.  May get to it soon.  EG
 */

/* For now stay with muti-byte characters.  May support wide characters
   in Zip 3.1.
 */

/* multibyte character set support
   Multibyte characters use typically two or more sequential bytes
   to represent additional characters than can fit in a single byte
   character set.  The code used here is based on the ANSI mblen function. */
#ifdef MULTIBYTE_GETOPTNS
  int mb_clen(ptr)
    ZCONST char *ptr;
  {
    /* return the number of bytes that the char pointed to is.  Return 1 if
       null character or error like not start of valid multibyte character. */
    int cl;

    cl = mblen(ptr, MB_CUR_MAX);
    return (cl > 0) ? cl : 1;
  }
#endif


  /* moved to zip.h */
#if 0
#ifdef UNICODE_SUPPORT
# define MB_CLEN(ptr) (1)
# define MB_NEXTCHAR(ptr) ((ptr)++)
# ifdef MULTIBYTE_GETOPTNS
#    undef MULTIBYTE_GETOPTNS
# endif
#else
# ifdef _MBCS
#  ifndef MULTIBYTE_GETOPTNS
#    define MULTIBYTE_GETOPTNS
#  endif
# endif
/* multibyte character set support
   Multibyte characters use typically two or more sequential bytes
   to represent additional characters than can fit in a single byte
   character set.  The code used here is based on the ANSI mblen function. */
#  ifdef MULTIBYTE_GETOPTNS
  local int mb_clen OF((ZCONST char *));  /* declare proto first */
  local int mb_clen(ptr)
    ZCONST char *ptr;
  {
    /* return the number of bytes that the char pointed to is.  Return 1 if
       null character or error like not start of valid multibyte character. */
    int cl;

    cl = mblen(ptr, MB_CUR_MAX);
    return (cl > 0) ? cl : 1;
  }
#  define MB_CLEN(ptr) mb_clen(ptr)
#  define MB_NEXTCHAR(ptr) ((ptr) += MB_CLEN(ptr))
# else
#  define MB_CLEN(ptr) (1)
#  define MB_NEXTCHAR(ptr) ((ptr)++)
# endif
#endif
#endif


/* constants */

/* function get_args_from_arg_file() can return this in depth parameter */
#define ARG_FILE_ERR -1

/* internal settings for optchar */
#define SKIP_VALUE_ARG   -1
#define THIS_ARG_DONE    -2
#define START_VALUE_LIST -3
#define IN_VALUE_LIST    -4
#define NON_OPTION_ARG   -5
#define STOP_VALUE_LIST  -6
/* 7/25/04 EG */
#define READ_REST_ARGS_VERBATIM -7


/* global veriables */

int enable_permute = 1;                     /* yes - return options first */
/* 7/25/04 EG */
int doubledash_ends_options = 1;            /* when -- what follows are not options */

/* buffer for error messages (this sizing is a guess but must hold 2 paths) */
#define OPTIONERR_BUF_SIZE (FNMAX * 2 + 4000)
local char Far optionerrbuf[OPTIONERR_BUF_SIZE + 1];

/* error messages */
static ZCONST char Far op_not_neg_err[] = "option %s not negatable";
static ZCONST char Far op_req_val_err[] = "option %s requires a value";
static ZCONST char Far op_no_allow_val_err[] = "option %s does not allow a value";
static ZCONST char Far sh_op_not_sup_err[] = "short option '%c' not supported";
static ZCONST char Far oco_req_val_err[] = "option %s requires one character value";
static ZCONST char Far oco_no_mbc_err[] = "option %s does not support multibyte values";
static ZCONST char Far num_req_val_err[] = "option %s requires number value";
static ZCONST char Far long_op_ambig_err[] = "long option '%s' ambiguous";
static ZCONST char Far long_op_not_sup_err[] = "long option '%s' not supported";

static ZCONST char Far no_arg_files_err[] = "argument files not enabled\n";


/* below removed as only used for processing argument files */

/* get_nextarg */
/* get_args_from_string */
/* insert_args */
/* get_args_from_arg_file */


/* copy error, option name, and option description if any to buf */
local int optionerr(buf, err, optind, islong)
  char *buf;
  ZCONST char *err;
  int optind;
  int islong;
{
  char optname[50];

  if (options[optind].name && options[optind].name[0] != '\0') {
    if (islong)
      sprintf(optname, "'%s' (%s)", options[optind].longopt, options[optind].name);
    else
      sprintf(optname, "'%s' (%s)", options[optind].shortopt, options[optind].name);
  } else {
    if (islong)
      sprintf(optname, "'%s'", options[optind].longopt);
    else
      sprintf(optname, "'%s'", options[optind].shortopt);
  }
  sprintf(buf, err, optname);
  return 0;
}


/* copy_args
 *
 * Copy arguments in args, allocating storage with malloc.
 * Copies until a NULL argument is found or until max_args args
 * including args[0] are copied.  Set max_args to 0 to copy
 * until NULL.  Always terminates returned args[] with NULL arg.
 *
 * Any argument in the returned args can be freed with free().  Any
 * freed argument should be replaced with either another string
 * allocated with malloc or by NULL if last argument so that free_args
 * will properly work.
 */
char **copy_args(args, max_args)
  char **args;
  int max_args;
{
  int j;
  char **new_args;

  if (args == NULL) {
    return NULL;
  }

  /* count args */
  for (j = 0; args[j] && (max_args == 0 || j < max_args); j++) ;

  if ((new_args = (char **) malloc((j + 1) * sizeof(char *))) == NULL) {
    oERR(ZE_MEM, "ca");
  }

  for (j = 0; args[j] && (max_args == 0 || j < max_args); j++) {
    if (args[j] == NULL) {
      /* null argument is end of args */
      new_args[j] = NULL;
      break;
    }
    if ((new_args[j] = malloc(strlen(args[j]) + 1)) == NULL) {
      free_args(new_args);
      oERR(ZE_MEM, "ca");
    }
    strcpy(new_args[j], args[j]);
  }
  new_args[j] = NULL;

  return new_args;
}


/* free args - free args created with one of these functions */
int free_args(args)
  char **args;
{
  int i;

  if (args == NULL) {
    return 0;
  }

  for (i = 0; args[i]; i++) {
    free(args[i]);
  }
  free(args);
  return i;
}


/* insert_arg
 *
 * Insert the argument arg into the array args before argument at_arg.
 * Return the new count of arguments (argc).
 *
 * If free_args is true, this function frees the old args array
 * (but not the component strings).  DO NOT set free_args on original
 * argv but only on args allocated with malloc.
 */

int insert_arg(pargs, arg, at_arg, free_args)
   char ***pargs;
   ZCONST char *arg;
   int at_arg;
   int free_args;
{
   char *newarg = NULL;
   char **args;
   char **newargs = NULL;
   int argnum;
   int newargnum;
   int argcnt;
   int newargcnt;

   if (pargs == NULL) {
     return 0;
   }
   args = *pargs;

   /* count args */
   if (args == NULL) {
     argcnt = 0;
   } else {
     for (argcnt = 0; args[argcnt]; argcnt++) ;
   }
   if (arg == NULL) {
     /* done */
     return argcnt;
   }
   newargcnt = argcnt + 1;

   /* get storage for new args */
   if ((newargs = (char **) malloc((newargcnt + 1) * sizeof(char *))) == NULL) {
     oERR(ZE_MEM, "ia");
   }

   /* copy argument pointers from args to position at_arg, copy arg, then rest args */
   argnum = 0;
   newargnum = 0;
   if (args) {
     for (; args[argnum] && argnum < at_arg; argnum++) {
       newargs[newargnum++] = args[argnum];
     }
   }
   /* copy new arg */
   if ((newarg = (char *) malloc(strlen(arg) + 1)) == NULL) {
     oERR(ZE_MEM, "ia");
   }
   strcpy(newarg, arg);

   newargs[newargnum++] = newarg;
   if (args) {
     for ( ; args[argnum]; argnum++) {
       newargs[newargnum++] = args[argnum];
     }
   }
   newargs[newargnum] = NULL;

   /* free old args array but not component strings - this assumes that
      args was allocated with malloc as copy_args does.  DO NOT DO THIS
      on the original argv.
    */
   if (free_args)
     free(args);

   *pargs = newargs;

   return newargnum;
}

/* ------------------------------------- */




/* get_shortopt
 *
 * Get next short option from arg.  The state is stored in argnum, optchar, and
 * option_num so no static storage is used.  Returns the option_ID.
 *
 * parameters:
 *    args        - argv array of arguments
 *    argnum      - index of current arg in args
 *    optchar     - pointer to index of next char to process.  Can be 0 or
 *                  const defined at top of this file like THIS_ARG_DONE
 *    negated     - on return pointer to int set to 1 if option negated or 0 otherwise
 *    value       - on return pointer to string set to value of option if any or NULL
 *                  if none.  If value is returned then the caller should free()
 *                  it when not needed anymore.
 *    option_num  - pointer to index in options[] of returned option or
 *                  o_NO_OPTION_MATCH if none.  Do not change as used by
 *                  value lists.
 *    depth       - recursion depth (0 at top level, 1 or more in arg files)
 */
local unsigned long get_shortopt(args, argnum, optchar, negated, value,
                                 option_num, depth)
  char **args;
  int argnum;
  int *optchar;
  int *negated;
  char **value;
  int *option_num;
  int depth;
{
  char *shortopt;
  int clen;
  char *nextchar;
  char *s;
  char *start;
  int op;
  char *arg;
  int match = -1;


  /* get arg */
  arg = args[argnum];
  /* current char in arg */
  nextchar = arg + (*optchar);
  clen = MB_CLEN(nextchar);
  /* next char in arg */
  (*optchar) +=  clen;
  /* get first char of short option */
  shortopt = arg + (*optchar);
  /* no value */
  *value = NULL;

  if (*shortopt == '\0') {
    /* no more options in arg */
    *optchar = 0;
    *option_num = o_NO_OPTION_MATCH;
    return 0;
  }

  /* look for match in options */
  clen = MB_CLEN(shortopt);
  for (op = 0; options[op].option_ID; op++) {
    s = options[op].shortopt;
    if (s && s[0] == shortopt[0]) {
      if (s[1] == '\0' && clen == 1) {
        /* single char match */
        match = op;
      } else {
        /* 2 wide short opt.  Could support more chars but should use long opts instead */
        if (s[1] == shortopt[1]) {
          /* match 2 char short opt or 2 byte char */
          match = op;
          if (clen == 1) (*optchar)++;
          break;
        }
      }
    }
  }

  if (match > -1) {
    /* match */
    clen = MB_CLEN(shortopt);
    nextchar = arg + (*optchar) + clen;
    /* check for trailing dash negating option */
    if (*nextchar == '-') {
      /* negated */
      if (options[match].negatable == o_NOT_NEGATABLE) {
        if (options[match].value_type == o_NO_VALUE) {
          optionerr(optionerrbuf, op_not_neg_err, match, 0);
          if (depth > 0) {
            /* unwind */
            oWARN(optionerrbuf);
            return o_ARG_FILE_ERR;
          } else {
            oERR(ZE_PARMS, optionerrbuf);
          }
        }
      } else {
        *negated = 1;
        /* set up to skip negating dash */
        (*optchar) += clen;
        clen = 1;
      }
    }

    /* value */
    clen = MB_CLEN(arg + (*optchar));
    /* optional value, one char value, and number value must follow option */
    if (options[match].value_type == o_ONE_CHAR_VALUE) {
      /* one char value */
      if (arg[(*optchar) + clen]) {
        /* has value */
        if (MB_CLEN(arg + (*optchar) + clen) > 1) {
          /* multibyte value not allowed for now */
          optionerr(optionerrbuf, oco_no_mbc_err, match, 0);
          if (depth > 0) {
            /* unwind */
            oWARN(optionerrbuf);
            return o_ARG_FILE_ERR;
          } else {
            oERR(ZE_PARMS, optionerrbuf);
          }
        }
        if ((*value = (char *) malloc(2)) == NULL) {
          oERR(ZE_MEM, "gso");
        }
        (*value)[0] = *(arg + (*optchar) + clen);
        (*value)[1] = '\0';
        *optchar += clen;
        clen = 1;
      } else {
        /* one char values require a value */
        optionerr(optionerrbuf, oco_req_val_err, match, 0);
        if (depth > 0) {
          oWARN(optionerrbuf);
          return o_ARG_FILE_ERR;
        } else {
          oERR(ZE_PARMS, optionerrbuf);
        }
      }
    } else if (options[match].value_type == o_NUMBER_VALUE) {
      /* read chars until end of number */
      start = arg + (*optchar) + clen;
      if (*start == '+' || *start == '-') {
        start++;
      }
      s = start;
      for (; isdigit(*s); MB_NEXTCHAR(s)) ;
      if (s == start) {
        /* no digits */
        optionerr(optionerrbuf, num_req_val_err, match, 0);
        if (depth > 0) {
          oWARN(optionerrbuf);
          return o_ARG_FILE_ERR;
        } else {
          oERR(ZE_PARMS, optionerrbuf);
        }
      }
      start = arg + (*optchar) + clen;
      if ((*value = (char *) malloc((int)(s - start) + 1)) == NULL) {
        oERR(ZE_MEM, "gso");
      }
      *optchar += (int)(s - start);
      strncpy(*value, start, (int)(s - start));
      (*value)[(int)(s - start)] = '\0';
      clen = MB_CLEN(s);
    } else if (options[match].value_type == o_OPTIONAL_VALUE) {
      /* optional value */
      /* This seemed inconsistent so now if no value attached to argument look
         to the next argument if that argument is not an option for option
         value - 11/12/04 EG */
      if (arg[(*optchar) + clen]) {
        /* has value */
        /* add support for optional = - 2/6/05 EG */
        if (arg[(*optchar) + clen] == '=') {
          /* skip = */
          clen++;
        }
        if (arg[(*optchar) + clen]) {
          if ((*value = (char *)malloc(strlen(arg + (*optchar) + clen) + 1))
              == NULL) {
            oERR(ZE_MEM, "gso");
          }
          strcpy(*value, arg + (*optchar) + clen);
        }
        *optchar = THIS_ARG_DONE;
      } else if (args[argnum + 1] && args[argnum + 1][0] != '-') {
        /* use next arg for value */
        if ((*value = (char *)malloc(strlen(args[argnum + 1]) + 1)) == NULL) {
          oERR(ZE_MEM, "gso");
        }
        /* using next arg as value */
        strcpy(*value, args[argnum + 1]);
        *optchar = SKIP_VALUE_ARG;
      }
    } else if (options[match].value_type == o_REQUIRED_VALUE ||
               options[match].value_type == o_VALUE_LIST) {
      /* see if follows option */
      if (arg[(*optchar) + clen]) {
        /* has value following option as -ovalue */
        /* add support for optional = - 6/5/05 EG */
        if (arg[(*optchar) + clen] == '=') {
          /* skip = */
          clen++;
        }
          if ((*value = (char *)malloc(strlen(arg + (*optchar) + clen) + 1))
              == NULL) {
          oERR(ZE_MEM, "gso");
        }
        strcpy(*value, arg + (*optchar) + clen);
        *optchar = THIS_ARG_DONE;
      } else {
        /* use next arg for value */
        if (args[argnum + 1]) {
          if ((*value = (char *)malloc(strlen(args[argnum + 1]) + 1)) == NULL) {
            oERR(ZE_MEM, "gso");
          }
          strcpy(*value, args[argnum + 1]);
          if (options[match].value_type == o_VALUE_LIST) {
            *optchar = START_VALUE_LIST;
          } else {
            *optchar = SKIP_VALUE_ARG;
          }
        } else {
          /* no value found */
          optionerr(optionerrbuf, op_req_val_err, match, 0);
          if (depth > 0) {
            oWARN(optionerrbuf);
            return o_ARG_FILE_ERR;
          } else {
            oERR(ZE_PARMS, optionerrbuf);
          }
        }
      }
    }

    *option_num = match;
    return options[match].option_ID;
  }
  sprintf(optionerrbuf, sh_op_not_sup_err, *shortopt);
  if (depth > 0) {
    /* unwind */
    oWARN(optionerrbuf);
    return o_ARG_FILE_ERR;
  } else {
    oERR(ZE_PARMS, optionerrbuf);
  }
  return 0;
}


/* get_longopt
 *
 * Get the long option in args array at argnum.
 * Parameters same as for get_shortopt.
 */

local unsigned long get_longopt(args, argnum, optchar, negated, value,
                                option_num, depth)
  char **args;
  int argnum;
  int *optchar;
  int *negated;
  char **value;
  int *option_num;
  int depth;
{
  char *longopt;
  char *lastchr;
  char *valuestart;
  int op;
  char *arg;
  int match = -1;
  *value = NULL;

  if (args == NULL) {
    *option_num = o_NO_OPTION_MATCH;
    return 0;
  }
  if (args[argnum] == NULL) {
    *option_num = o_NO_OPTION_MATCH;
    return 0;
  }
  /* copy arg so can chop end if value */
  if ((arg = (char *)malloc(strlen(args[argnum]) + 1)) == NULL) {
    oERR(ZE_MEM, "glo");
  }
  strcpy(arg, args[argnum]);

  /* get option */
  longopt = arg + 2;
  /* no value */
  *value = NULL;

  /* find = */
  for (lastchr = longopt, valuestart = longopt;
       *valuestart && *valuestart != '=';
       lastchr = valuestart, MB_NEXTCHAR(valuestart)) ;
  if (*valuestart) {
    /* found =value */
    *valuestart = '\0';
    valuestart++;
  } else {
    valuestart = NULL;
  }

  if (*lastchr == '-') {
    /* option negated */
    *negated = 1;
    *lastchr = '\0';
  } else {
    *negated = 0;
  }

  /* look for long option match */
  for (op = 0; options[op].option_ID; op++) {
    if (options[op].longopt && strcmp(options[op].longopt, longopt) == 0) {
      /* exact match */
      match = op;
      break;
    }
    if (options[op].longopt && strncmp(options[op].longopt, longopt, strlen(longopt)) == 0) {
      if (match > -1) {
        sprintf(optionerrbuf, long_op_ambig_err, longopt);
        free(arg);
        if (depth > 0) {
          /* unwind */
          oWARN(optionerrbuf);
          return o_ARG_FILE_ERR;
        } else {
          oERR(ZE_PARMS, optionerrbuf);
        }
      }
      match = op;
    }
  }

  if (match == -1) {
    sprintf(optionerrbuf, long_op_not_sup_err, longopt);
    free(arg);
    if (depth > 0) {
      oWARN(optionerrbuf);
      return o_ARG_FILE_ERR;
    } else {
      oERR(ZE_PARMS, optionerrbuf);
    }
  }

  /* one long option an arg */
  *optchar = THIS_ARG_DONE;

  /* if negated then see if allowed */
  if (*negated && options[match].negatable == o_NOT_NEGATABLE) {
    optionerr(optionerrbuf, op_not_neg_err, match, 1);
    free(arg);
    if (depth > 0) {
      /* unwind */
      oWARN(optionerrbuf);
      return o_ARG_FILE_ERR;
    } else {
      oERR(ZE_PARMS, optionerrbuf);
    }
  }
  /* get value */
  if (options[match].value_type == o_OPTIONAL_VALUE) {
    /* optional value in form option=value */
    if (valuestart) {
      /* option=value */
      if ((*value = (char *)malloc(strlen(valuestart) + 1)) == NULL) {
        free(arg);
        oERR(ZE_MEM, "glo");
      }
      strcpy(*value, valuestart);
    }
  } else if (options[match].value_type == o_REQUIRED_VALUE ||
             options[match].value_type == o_NUMBER_VALUE ||
             options[match].value_type == o_ONE_CHAR_VALUE ||
             options[match].value_type == o_VALUE_LIST) {
    /* handle long option one char and number value as required value */
    if (valuestart) {
      /* option=value */
      if ((*value = (char *)malloc(strlen(valuestart) + 1)) == NULL) {
        free(arg);
        oERR(ZE_MEM, "glo");
      }
      strcpy(*value, valuestart);
    } else {
      /* use next arg */
      if (args[argnum + 1]) {
        if ((*value = (char *)malloc(strlen(args[argnum + 1]) + 1)) == NULL) {
          free(arg);
          oERR(ZE_MEM, "glo");
        }
        /* using next arg as value */
        strcpy(*value, args[argnum + 1]);
        if (options[match].value_type == o_VALUE_LIST) {
          *optchar = START_VALUE_LIST;
        } else {
          *optchar = SKIP_VALUE_ARG;
        }
      } else {
        /* no value found */
        optionerr(optionerrbuf, op_req_val_err, match, 1);
        free(arg);
        if (depth > 0) {
          /* unwind */
          oWARN(optionerrbuf);
          return o_ARG_FILE_ERR;
        } else {
          oERR(ZE_PARMS, optionerrbuf);
        }
      }
    }
  } else if (options[match].value_type == o_NO_VALUE) {
    /* this option does not accept a value */
    if (valuestart) {
      /* --option=value */
      optionerr(optionerrbuf, op_no_allow_val_err, match, 1);
      free(arg);
      if (depth > 0) {
        oWARN(optionerrbuf);
        return o_ARG_FILE_ERR;
      } else {
        oERR(ZE_PARMS, optionerrbuf);
      }
    }
  }
  free(arg);

  *option_num = match;
  return options[match].option_ID;
}



/* get_option
 *
 * Main interface for user.  Use this function to get options, values and
 * non-option arguments from a command line provided in argv form.
 *
 * To use get_option() first define valid options by setting
 * the global variable options[] to an array of option_struct.  Also
 * either change defaults below or make variables global and set elsewhere.
 * Zip uses below defaults.
 *
 * Call get_option() to get an option (like -b or --temp-file) and any
 * value for that option (like filename for -b) or a non-option argument
 * (like archive name) each call.  If *value* is not NULL after calling
 * get_option() it is a returned value and the caller should either store
 * the char pointer or free() it before calling get_option() again to avoid
 * leaking memory.  If a non-option non-value argument is returned get_option()
 * returns o_NON_OPTION_ARG and value is set to the entire argument.
 * When there are no more arguments get_option() returns 0.
 *
 * The parameters argnum (after set to 0 on initial call),
 * optchar, first_nonopt_arg, option_num, and depth (after initial
 * call) are set and maintained by get_option() and should not be
 * changed.  The parameters argc, negated, and value are outputs and
 * can be used by the calling program.  get_option() returns either the
 * option_ID for the current option, a special value defined in
 * zip.h, or 0 when no more arguments.
 *
 * The value returned by get_option() is the ID value in the options
 * table.  This value can be duplicated in the table if different
 * options are really the same option.  The index into the options[]
 * table is given by option_num, though the ID should be used as
 * option numbers change when the table is changed.  The ID must
 * not be 0 for any option as this ends the table.  If get_option()
 * finds an option not in the table it calls oERR to post an
 * error and exit.  Errors also result if the option requires a
 * value that is missing, a value is present but the option does
 * not take one, and an option is negated but is not
 * negatable.  Non-option arguments return o_NON_OPTION_ARG
 * with the entire argument in value.
 *
 * For Zip, permuting is on and all options and their values are
 * returned before any non-option arguments like archive name.
 *
 * The arguments "-" alone and "--" alone return as non-option arguments.
 * Note that "-" should not be used as part of a short option
 * entry in the table but can be used in the middle of long
 * options such as in the long option "a-long-option".  Now "--" alone
 * stops option processing, returning any arguments following "--" as
 * non-option arguments instead of options.
 *
 * Argument file support is removed from this version. It may be added later.
 *
 * After each call:
 *   argc       is set to the current size of args[] but should not change
 *                with argument file support removed,
 *   argnum     is the index of the current arg,
 *   value      is either the value of the returned option or non-option
 *                argument or NULL if option with no value,
 *   negated    is set if the option was negated by a trailing dash (-)
 *   option_num is set to either the index in options[] for the option or
 *                o_NO_OPTION_MATCH if no match.
 * Negation is checked before the value is read if the option is negatable so
 * that the - is not included in the value.  If the option is not negatable
 * but takes a value then the - will start the value.  If permuting then
 * argnum and first_nonopt_arg are unreliable and should not be used.
 *
 * Command line is read from left to right.  As get_option() finds non-option
 * arguments (arguments not starting with - and that are not values to options)
 * it moves later options and values in front of the non-option arguments.
 * This permuting is turned off by setting enable_permute to 0.  Then
 * get_option() will return options and non-option arguments in the order
 * found.  Currently permuting is only done after an argument is completely
 * processed so that any value can be moved with options they go with.  All
 * state information is stored in the parameters argnum, optchar,
 * first_nonopt_arg and option_num.  You should not change these after the
 * first call to get_option().  If you need to back up to a previous arg then
 * set argnum to that arg (remembering that args may have been permuted) and
 * set optchar = 0 and first_nonopt_arg to the first non-option argument if
 * permuting.  After all arguments are returned the next call to get_option()
 * returns 0.  The caller can then call free_args(args) if appropriate.
 *
 * get_option() accepts arguments in the following forms:
 *  short options
 *       of 1 and 2 characters, e.g. a, b, cc, d, and ba, after a single
 *       leading -, as in -abccdba.  In this example if 'b' is followed by 'a'
 *       it matches short option 'ba' else it is interpreted as short option
 *       b followed by another option.  The character - is not legal as a
 *       short option or as part of a 2 character short option.
 *
 *       If a short option has a value it immediately follows the option or
 *       if that option is the end of the arg then the next arg is used as
 *       the value.  So if short option e has a value, it can be given as
 *             -evalue
 *       or
 *             -e value
 *       and now
 *             -e=value
 *       but now that = is optional a leading = is stripped for the first.
 *       This change allows optional short option values to be defaulted as
 *             -e=
 *       Either optional or required values can be specified.  Optional values
 *       now use both forms as ignoring the later got confusing.  Any
 *       non-value short options can preceed a valued short option as in
 *             -abevalue
 *       Some value types (one_char and number) allow options after the value
 *       so if oc is an option that takes a character and n takes a number
 *       then
 *             -abocVccn42evalue
 *       returns value V for oc and value 42 for n.  All values are strings
 *       so programs may have to convert the "42" to a number.  See long
 *       options below for how value lists are handled.
 *
 *       Any short option can be negated by following it with -.  Any - is
 *       handled and skipped over before any value is read unless the option
 *       is not negatable but takes a value and then - starts the value.
 *
 *       If the value for an optional value is just =, then treated as no
 *       value.
 *
 *  long options
 *       of arbitrary length are assumed if an arg starts with -- but is not
 *       exactly --.  Long options are given one per arg and can be abbreviated
 *       if the abbreviation uniquely matches one of the long options.
 *       Exact matches always match before partial matches.  If ambiguous an
 *       error is generated.
 *
 *       Values are specified either in the form
 *             --longoption=value
 *       or can be the following arg if the value is required as in
 *             --longoption value
 *       Optional values to long options must be in the first form.
 *
 *       Value lists are specified by o_VALUE_LIST and consist of an option
 *       that takes a value followed by one or more value arguments.
 *       The two forms are
 *             --option=value
 *       or
 *             -ovalue
 *       for a single value or
 *             --option value1 value2 value3 ... --option2
 *       or
 *             -o value1 value2 value3 ...
 *       for a list of values.  The list ends at the next option, the
 *       end of the command line, or at a single "@" argument.
 *       Each value is treated as if it was preceeded by the option, so
 *             --option1 val1 val2
 *       with option1 value_type set to o_VALUE_LIST is the same as
 *             --option1=val1 --option1=val2
 *
 *       Long options can be negated by following the option with - as in
 *             --longoption-
 *       Long options with values can also be negated if this makes sense for
 *       the caller as:
 *             --longoption-=value
 *       If = is not followed by anything it is treated as no value.
 *
 *  @path
 *       When an argument in the form @path is encountered, the file at path
 *       is opened and white space separated arguments read from the file
 *       and inserted into the command line at that point as if the contents
 *       of the file were directly typed at that location.  The file can
 *       have options, files to zip, or anything appropriate at that location
 *       in the command line.  Since Zip has permuting enabled, options and
 *       files will propagate to the appropriate locations in the command
 *       line.
 *
 *       Argument files support has been removed from this version.  It may
 *       be added back later.
 *
 *  non-option argument
 *       is any argument not given above.  If enable_permute is 1 then
 *       these are returned after all options, otherwise all options and
 *       args are returned in order.  Returns option ID o_NON_OPTION_ARG
 *       and sets value to the argument.
 *
 *
 * Arguments to get_option:
 *  char ***pargs          - pointer to arg array in the argv form
 *  int *argc              - returns the current argc for args incl. args[0]
 *  int *argnum            - the index of the current argument (caller
 *                            should set = 0 on first call and not change
 *                            after that)
 *  int *optchar           - index of next short opt in arg or special
 *  int *first_nonopt_arg  - used by get_option to permute args
 *  int *negated           - option was negated (had trailing -)
 *  char *value            - value of option if any (free when done with it) or NULL
 *  int *option_num        - the index in options of the last option returned
 *                            (can be o_NO_OPTION_MATCH)
 *  int recursion_depth    - current depth of recursion
 *                            (always set to 0 by caller)
 *                            (always 0 with argument files support removed)
 *
 *  Caller should only read the returned option ID and the value, negated,
 *  and option_num (if required) parameters after each call.
 *
 *  Ed Gordon
 *  24 August 2003 (last updated 2 April 2008 EG)
 *
 */

unsigned long get_option(pargs, argc, argnum, optchar, value,
                         negated, first_nonopt_arg, option_num, recursion_depth)
  char ***pargs;
  int *argc;
  int *argnum;
  int *optchar;
  char **value;
  int *negated;
  int *first_nonopt_arg;
  int *option_num;
  int recursion_depth;
{
  char **args;
  unsigned long option_ID;

  int argcnt;
  int first_nonoption_arg;
  char *arg = NULL;
  int h;
  int optc;
  int argn;
  int j;
  int v;
  int read_rest_args_verbatim = 0;  /* 7/25/04 - ignore options and arg files for rest args */

  /* value is outdated.  The caller should free value before
     calling get_option again. */
  *value = NULL;

  /* if args is NULL then done */
  if (pargs == NULL) {
    *argc = 0;
    return 0;
  }
  args = *pargs;
  if (args == NULL) {
    *argc = 0;
    return 0;
  }

  /* count args */
  for (argcnt = 0; args[argcnt]; argcnt++) ;

  /* if no provided args then nothing to do */
  if (argcnt < 1 || (recursion_depth == 0 && argcnt < 2)) {
    *argc = argcnt;
    /* return 0 to note that no args are left */
    return 0;
  }

  *negated = 0;
  first_nonoption_arg = *first_nonopt_arg;
  argn = *argnum;
  optc = *optchar;

  if (optc == READ_REST_ARGS_VERBATIM) {
    read_rest_args_verbatim = 1;
  }

  if (argn == -1 || (recursion_depth == 0 && argn == 0)) {
    /* first call */
    /* if depth = 0 then args[0] is argv[0] so skip */
    *option_num = o_NO_OPTION_MATCH;
    optc = THIS_ARG_DONE;
    first_nonoption_arg = -1;
  }

  /* if option_num is set then restore last option_ID in case continuing value list */
  option_ID = 0;
  if (*option_num != o_NO_OPTION_MATCH) {
    option_ID = options[*option_num].option_ID;
  }

  /* get next option if any */
  for (;;)  {
    if (read_rest_args_verbatim) {
      /* rest of args after "--" are non-option args if doubledash_ends_options set */
      argn++;
      if (argn > argcnt || args[argn] == NULL) {
        /* done */
        option_ID = 0;
        break;
      }
      arg = args[argn];
      if ((*value = (char *)malloc(strlen(arg) + 1)) == NULL) {
        oERR(ZE_MEM, "go");
      }
      strcpy(*value, arg);
      *option_num = o_NO_OPTION_MATCH;
      option_ID = o_NON_OPTION_ARG;
      break;

    /* permute non-option args after option args so options are returned first */
    } else if (enable_permute) {
      if (optc == SKIP_VALUE_ARG || optc == THIS_ARG_DONE ||
          optc == START_VALUE_LIST || optc == IN_VALUE_LIST ||
          optc == STOP_VALUE_LIST) {
        /* moved to new arg */
        if (first_nonoption_arg > -1 && args[first_nonoption_arg]) {
          /* do the permuting - move non-options after this option */
          /* if option and value separate args or starting list skip option */
          if (optc == SKIP_VALUE_ARG || optc == START_VALUE_LIST) {
            v = 1;
          } else {
            v = 0;
          }
          for (h = first_nonoption_arg; h < argn; h++) {
            arg = args[first_nonoption_arg];
            for (j = first_nonoption_arg; j < argn + v; j++) {
              args[j] = args[j + 1];
            }
            args[j] = arg;
          }
          first_nonoption_arg += 1 + v;
        }
      }
    } else if (optc == NON_OPTION_ARG) {
      /* if not permuting then already returned arg */
      optc = THIS_ARG_DONE;
    }

    /* value lists */
    if (optc == STOP_VALUE_LIST) {
      optc = THIS_ARG_DONE;
    }

    if (optc == START_VALUE_LIST || optc == IN_VALUE_LIST) {
      if (optc == START_VALUE_LIST) {
        /* already returned first value */
        argn++;
        optc = IN_VALUE_LIST;
      }
      argn++;
      arg = args[argn];
      /* if end of args and still in list and there are non-option args then
         terminate list */
      if (arg == NULL && (optc == START_VALUE_LIST || optc == IN_VALUE_LIST)
          && first_nonoption_arg > -1) {
        /* terminate value list with @ */
        /* this is only needed for argument files */
        /* but is also good for show command line so command lines with lists
           can always be read back in */
        argcnt = insert_arg(&args, "@", first_nonoption_arg, 1);
        argn++;
        if (first_nonoption_arg > -1) {
          first_nonoption_arg++;
        }
      }

      arg = args[argn];
      if (arg && arg[0] == '@' && arg[1] == '\0') {
          /* inserted arguments terminator */
          optc = STOP_VALUE_LIST;
          continue;
      } else if (arg && arg[0] != '-') {  /* not option */
        /* - and -- are not allowed in value lists unless escaped */
        /* another value in value list */
        if ((*value = (char *)malloc(strlen(args[argn]) + 1)) == NULL) {
          oERR(ZE_MEM, "go");
        }
        strcpy(*value, args[argn]);
        break;

      } else {
        argn--;
        optc = THIS_ARG_DONE;
      }
    }

    /* move to next arg */
    if (optc == SKIP_VALUE_ARG) {
      argn += 2;
      optc = 0;
    } else if (optc == THIS_ARG_DONE) {
      argn++;
      optc = 0;
    }
    if (argn > argcnt) {
      break;
    }
    if (args[argn] == NULL) {
      /* done unless permuting and non-option args */
      if (first_nonoption_arg > -1 && args[first_nonoption_arg]) {
        /* return non-option arguments at end */
        if (optc == NON_OPTION_ARG) {
          first_nonoption_arg++;
        }
        /* after first pass args are permuted but skipped over non-option args */
        /* swap so argn points to first non-option arg */
        j = argn;
        argn = first_nonoption_arg;
        first_nonoption_arg = j;
      }
      if (argn > argcnt || args[argn] == NULL) {
        /* done */
        option_ID = 0;
        break;
      }
    }

    /* after swap first_nonoption_arg points to end which is NULL */
    if (first_nonoption_arg > -1 && (args[first_nonoption_arg] == NULL)) {
      /* only non-option args left */
      if (optc == NON_OPTION_ARG) {
        argn++;
      }
      if (argn > argcnt || args[argn] == NULL) {
        /* done */
        option_ID = 0;
        break;
      }
      if ((*value = (char *)malloc(strlen(args[argn]) + 1)) == NULL) {
        oERR(ZE_MEM, "go");
      }
      strcpy(*value, args[argn]);
      optc = NON_OPTION_ARG;
      option_ID = o_NON_OPTION_ARG;
      break;
    }

    arg = args[argn];

    /* is it an option */
    if (arg[0] == '-') {
      /* option */
      if (arg[1] == '\0') {
        /* arg = - */
        /* treat like non-option arg */
        *option_num = o_NO_OPTION_MATCH;
        if (enable_permute) {
          /* permute args to move all non-option args to end */
          if (first_nonoption_arg < 0) {
            first_nonoption_arg = argn;
          }
          argn++;
        } else {
          /* not permute args so return non-option args when found */
          if ((*value = (char *)malloc(strlen(arg) + 1)) == NULL) {
            oERR(ZE_MEM, "go");
          }
          strcpy(*value, arg);
          optc = NON_OPTION_ARG;
          option_ID = o_NON_OPTION_ARG;
          break;
        }

      } else if (arg[1] == '-') {
        /* long option */
        if (arg[2] == '\0') {
          /* arg = -- */
          if (doubledash_ends_options) {
            /* Now -- stops permuting and forces the rest of
               the command line to be read verbatim - 7/25/04 EG */

            /* never permute args after -- and return as non-option args */
            if (first_nonoption_arg < 1) {
              /* -- is first non-option argument - 8/7/04 EG */
              argn--;
            } else {
              /* go back to start of non-option args - 8/7/04 EG */
              argn = first_nonoption_arg - 1;
            }

            /* disable permuting and treat remaining arguments as not
               options */
            read_rest_args_verbatim = 1;
            optc = READ_REST_ARGS_VERBATIM;

          } else {
            /* treat like non-option arg */
            *option_num = o_NO_OPTION_MATCH;
            if (enable_permute) {
              /* permute args to move all non-option args to end */
              if (first_nonoption_arg < 0) {
                first_nonoption_arg = argn;
              }
              argn++;
            } else {
              /* not permute args so return non-option args when found */
              if ((*value = (char *)malloc(strlen(arg) + 1)) == NULL) {
                oERR(ZE_MEM, "go");
              }
              strcpy(*value, arg);
              optc = NON_OPTION_ARG;
              option_ID = o_NON_OPTION_ARG;
              break;
            }
          }

        } else {
          option_ID = get_longopt(args, argn, &optc, negated, value, option_num, recursion_depth);
          if (option_ID == o_ARG_FILE_ERR) {
            /* unwind as only get this if recursion_depth > 0 */
            return option_ID;
          }
          break;
        }

      } else {
        /* short option */
        option_ID = get_shortopt(args, argn, &optc, negated, value, option_num, recursion_depth);

        if (option_ID == o_ARG_FILE_ERR) {
          /* unwind as only get this if recursion_depth > 0 */
          return option_ID;
        }

        if (optc == 0) {
          /* if optc = 0 then ran out of short opts this arg */
          optc = THIS_ARG_DONE;
        } else {
          break;
        }
      }

#if 0
    /* argument file code left out
       so for now let filenames start with @
    */

    } else if (allow_arg_files && arg[0] == '@') {
      /* arg file */
      oERR(ZE_PARMS, no_arg_files_err);
#endif

    } else {
      /* non-option */
      if (enable_permute) {
        /* permute args to move all non-option args to end */
        if (first_nonoption_arg < 0) {
          first_nonoption_arg = argn;
        }
        argn++;
      } else {
        /* no permute args so return non-option args when found */
        if ((*value = (char *)malloc(strlen(arg) + 1)) == NULL) {
          oERR(ZE_MEM, "go");
        }
        strcpy(*value, arg);
        *option_num = o_NO_OPTION_MATCH;
        optc = NON_OPTION_ARG;
        option_ID = o_NON_OPTION_ARG;
        break;
      }

    }
  }

  *pargs = args;
  *argc = argcnt;
  *first_nonopt_arg = first_nonoption_arg;
  *argnum = argn;
  *optchar = optc;

  return option_ID;
}
