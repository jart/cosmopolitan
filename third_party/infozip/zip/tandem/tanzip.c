/*
  Copyright (c) 1990-2002 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*
 * routines only used by TANDEM ZIP
 */

#include "zip.h"
#include "crypt.h"
#include <tal.h>
#include "$system.zsysdefs.zsysc" nolist
#include <cextdecs> nolist
#include "tannsk.h"

/******************************/
/*  Function version_local()  */
/******************************/

void version_local()
{
    static ZCONST char CompiledWith[] = "Compiled with %s%s for %s%s%s%s.\n\n";
#if 0
    char buf[40];
#endif

    printf(CompiledWith,

#ifdef __GNUC__
      "gcc ", __VERSION__,
#else
     "C ", "T9255D44 - (16OCT98)",
#endif

     "NonStop ", "(Tandem/NSK)",

#ifdef __DATE__
      " on ", __DATE__
#else
      "", ""
#endif
      );

} /* end function version_local() */


/************************/
/*  Function nskopen()  */
/************************/

#ifdef fopen
#  undef fopen
#endif

FILE *nskopen(fname, opt)
const char *fname;
const char *opt;
{
  int fdesc;
  short fnum, err, len;
  int priext, secext;
  short maxext, filecode, blocksize;

  #define alist_items 1
  #define vlist_bytes 2
  short alist[alist_items]={42};
  unsigned short vlist[alist_items];
  short extra, *err_item=&extra;

  char nsk_work[FILENAME_MAX + 1], *nsk_fname=&nsk_work[0];

  /* See if we want to create a new file */
  if ((strcmp(opt,FOPW) == 0) || (strcmp(opt,FOPWT) == 0)) {
    blocksize = TANDEM_BLOCKSIZE;
    priext = 100;
    secext = 500;
    maxext = 978;
    filecode = NSK_ZIPFILECODE;

    if ((fdesc = creat(fname,,priext,secext)) != -1){
      fnum = fdtogfn ((short)fdesc);
      err = (SETMODE (fnum, SET_FILE_BUFFERSIZE, blocksize) != CCE);
      err = (SETMODE (fnum, SET_FILE_BUFFERED, 0, 0) != CCE);
      err = (SETMODE (fnum, SET_FILE_BUFFERED, 0, 1) != CCE);
      err = (SETMODE (fnum, SET_FILE_MAXEXTENTS, maxext) != CCE);
      err = close(fdesc);

      vlist[0] = filecode;

      /* Note that FILE_ALTERLIST_ expects uppercase names */
      /* Need to call strlen and upshift                   */
      len = strlen(fname);
      err = STRING_UPSHIFT_((char *)fname,
                            len,
                            nsk_fname,
                            len);

      err = FILE_ALTERLIST_(nsk_fname,
                            len,
                            alist,
                            alist_items,
                            vlist,
                            vlist_bytes,
                            ,
                            err_item);
    };
  };

  return fopen(fname,opt);
}
#define fopen nskopen


  int Bflag = 0;            /* Special formatting options for Tandem        */
                            /* Bit 0 = Add delimiter (0 = Yes, 1 = No)      */
                            /* Bit 1 = Delimiter Type (0 = CR/LF, 1 = LF)   */
                            /* Bit 2 = Space Fill records (0 = No, 1 = Yes) */
                            /* Bit 3 = Trim trailing space(0 = No, 1 = Yes) */
                            /* Thus, default is to add CR/LF, no padding    */
                            /* Bit 8 = Use 'safe' large read size (Expand)  */
  char nsk_delim[2] = {'\r', '\n'}; /* CR/LF */
  int nsk_delim_len = 2;
  int nsk_space_fill = 0;   /* 0 = No, 1 = Yes          */
  int nsk_trim_space = 0;   /* 0 = No, 1 = Yes          */
  unsigned short nsk_large_read = MAX_LARGE_READ;

  /* Following holds details of file currently used by zopen & zread */
  struct stat znsk_stat;
  nsk_stat_ov *znsk_ov = (nsk_stat_ov *)&znsk_stat.st_reserved[0];
  nsk_file_attrs *znsk_attr = (nsk_file_attrs *)
    ( (char *)(&znsk_stat.st_reserved[0]) +
      offsetof (struct nsk_stat_overlay, nsk_ef_region) );

  /* Following items used by zread to avoid overwriting window */
  char zreadbuf[MAX_LARGE_READ];       /* Buffer as large as biggest read */
  char *zreadptr = (char *) zreadbuf;  /* pointer to start of buffer      */
  char *zread_ovptr = NULL;            /* pointer to left overs           */
  long zread_ovlen = 0;                /* size of remaining left overs    */

  int zopen (filename, opt)
    const char *filename;
    int opt;
  {
    /* Currently ignore opt.  Choose method of I/O based on NSK file type */
    short err, len, fnum, access, exclus, bufferlen, options;
    long recnum;
    char fname[FILENAME_MAX + 1];
    short extension;
    char ext[EXTENSION_MAX + 1];

    /* Remove any (pseudo) file extension */
    extension = parsename (filename,fname,ext);
    len = strlen(fname);

    fnum = 0;
    access = NSK_RDONLY;
    exclus = NSK_SHARED;

    err = stat(fname, &znsk_stat); /* Setup global struct, used by zread */

    if (znsk_attr->filetype == NSK_UNSTRUCTURED)
      if (znsk_attr->filecode == NSK_EDITFILECODE) {
        /* Edit File */
        fnum = -1; /* Ask OPENEDIT_ to open the file for us */
        err = OPENEDIT_ ((char *)fname, len, &fnum, access, exclus);
        if (!err) {
          recnum = -1; /* Position to first line */
          err = POSITIONEDIT (fnum, recnum);
        }
      }
      else {
        /* Unstructured File */
        options = NSK_UNSTRUCTUREDACCESS;
        err = FILE_OPEN_((char *)fname, len, &fnum, access, exclus,
                         ,,options,,,);
        if (!err)
          /* Ask for large transfer mode */
          err = (SETMODE (fnum, SET_LARGE_TRANSFERS, 1) != CCE);
      }
    else {
      /* Structured File */
      bufferlen = 4096;  /* request SBB */
      err = FILE_OPEN_((char *)fname, len, &fnum, access, exclus,
                       ,,,, bufferlen ,);
      if (err == 4)
        err = 0;  /* Allow saving of files that have missing altkeys */
    }

    return (err == 0 ? (int)fnum : -1);
  }

  unsigned zread (fnum, buf, len)
    int fnum;
    char *buf;
    unsigned len;
  {
    short err, trail;
    long total, movelen;
    unsigned short countread;
    unsigned readlen;  /* typed to match incoming arg */
    char *bufptr, *readptr;

    total = err = 0;
    bufptr = buf;

    /* We use a separate buffer to read in data as it can be larger than
       WSIZE, and hence would overwrite memory */

    /* We always attempt to give the user the exact requested size
       Hence we make use of an overfow buffer for previously truncated data */

    /* see if we have some left over characters from last time */
    if (zread_ovlen) {
       movelen = _min(len,zread_ovlen);
       memcpy(bufptr, zread_ovptr, movelen);
       bufptr += movelen;
       total += movelen;
       zread_ovptr += movelen;
       zread_ovlen -= movelen;
    }

    while (!err && (total < len)) {
      readptr = zreadptr;

      if (znsk_attr->filetype == NSK_UNSTRUCTURED)
        if (znsk_attr->filecode == NSK_EDITFILECODE){
          /* Edit File */
          trail = 1;
          readlen = MAX_EDIT_READ; /* guarantee it fits in buffer */

          /* get line and preserve any trailing space characters */
          err = READEDIT (fnum,, zreadptr, (short) readlen,
                            (short *) &countread,,, trail);
          /* if countread is ever negative then we will skip a line */

          if (!err) {
            readptr = zreadptr + countread;
            /* Note it is possible for Edit files to hold trailing space */
            if (nsk_trim_space)
              while (*(readptr-1) == ' ') {
                readptr--;
                countread--;
              }

            if (nsk_delim_len) {
              memcpy(readptr, nsk_delim, nsk_delim_len);
              readptr += nsk_delim_len;
              countread += nsk_delim_len;
            }
          }
        }
        else {
          /* Unstructured File */

          /* Using large transfer mode so we have to use 2K multiples
             Use largest size possible and put remains in overflow    */

          readlen = nsk_large_read; /* use largest read, overflow into buffer*/

          err = (READX(fnum, zreadptr, readlen, (short *)&countread) != CCE);
          if (err && (errno == EINVAL)) {
            /* Read too big so scale back to smaller value */
            readlen = nsk_large_read = MAX_LARGE_READ_EXPAND;
            err = (READX(fnum, zreadptr, readlen, (short *)&countread) != CCE);
          }
          if (!err)
            readptr = zreadptr + countread;
        }
      else {
        /* Structured File */
        readlen = znsk_attr->reclen;

        err = (READX(fnum, zreadptr, readlen, (short *)&countread)!= CCE);

        if (!err) {
          readptr = zreadptr + countread;
          if (nsk_space_fill)
            while (countread < readlen) {
              *readptr++ = ' ';
              countread++;
            }
          else
            if (nsk_trim_space)
              while (*(readptr-1) == ' ') {
                readptr--;
                countread--;
              }

          if (nsk_delim_len) {
            memcpy(readptr, nsk_delim, nsk_delim_len);
            readptr += nsk_delim_len;
            countread += nsk_delim_len;
          }
        }
      }
      if (!err) {
         movelen = _min((len-total), countread);
         memcpy(bufptr, zreadptr, movelen);
         bufptr += movelen;
         total += movelen;
         if (movelen < countread) { /* still stuff in Read buffer */
           zread_ovptr = zreadptr + movelen;   /* pointer to whats left */
           zread_ovlen = countread - movelen;  /* how much is left      */
         }
      }
    }

    return ((unsigned)total);
  }

  int zclose (fnum)
    int fnum;
  {
    short err;

    if ((znsk_attr->filetype == NSK_UNSTRUCTURED)
      && (znsk_attr->filecode == NSK_EDITFILECODE))
      err = CLOSEEDIT_(fnum);
    else
      err = FILE_CLOSE_(fnum);

    return (err != 0);
  }

/* modified to work with get_option which returns
   a string with the number value without leading option */
void nskformatopt(p)
char *p;
{
  /* set up formatting options for ZIP */

  Bflag = 0; /* default option */

  Bflag = strtoul(p, NULL, 10);

  if (Bflag & NSK_SPACE_FILL)
    nsk_space_fill = 1;
  else
    nsk_space_fill = 0;

  if (Bflag & NSK_TRIM_TRAILING_SPACE)
    nsk_trim_space = 1;
  else
    nsk_trim_space = 0;

  if (Bflag & NSK_NO_DELIMITER)
    nsk_delim_len = 0;
  else {
    if (Bflag & NSK_USE_FF_DELIMITER) {
      nsk_delim[0] = '\n';
      nsk_delim_len = 1;
    }
    else {   /* CR/LF */
      nsk_delim[0] = '\r';
      nsk_delim[1] = '\n';
      nsk_delim_len = 2;
    }
  }

  if (Bflag & NSK_LARGE_READ_EXPAND)
    nsk_large_read = MAX_LARGE_READ_EXPAND;
  else
    nsk_large_read = MAX_LARGE_READ;

}


  int deletedir(d)
    char *d;                /* directory to delete */
  /* Delete the directory *d if it is empty, do nothing otherwise.
     Return the result of rmdir(), delete(), or system().
     For VMS, d must be in format [x.y]z.dir;1  (not [x.y.z]).
   */
  {
      return rmdir(d);
  }

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
    else if (stat(n, &s))
    {
      /* Not a file or directory--search for shell expression in zip file */
      p = ex2in(n, 0, (int *)NULL);       /* shouldn't affect matching chars */
      m = 1;
      for (z = zfiles; z != NULL; z = z->nxt) {
        if (MATCH(p, z->zname, caseflag))
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
      if ((p = malloc(strlen(n)+4)) == NULL)
        return ZE_MEM;

      strcpy(p, n);

      /* No concept of directories on Tandem - so do not store them ...*/
      /* code removed from which attempted to save dir name if dirnames set */

      /*  Test for recurse being set removed, since Tandem has no dir concept*/
      /*  recurse into template */
      if ((d = opendir(n)) != NULL)
      {
        while ((e = readd(d)) != NULL) {
          if ((m = procname(e, caseflag)) != ZE_OK)   /* recurse on name */
          {
            if (m == ZE_MISS)
              zipwarn("name not matched: ", e);
            else
              ziperr(m, e);
          }
        }
        closedir(d);
      }
      free((zvoid *)p);
    } /* (s.st_mode & S_IFDIR) == 0) */
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
    char *t;              /* shortened name */
    int dosflag;
    char *p;               /* pointer to temp area */
    char fname[FILENAME_MAX + 1]= ""; /* file name */
    char ext[EXTENSION_MAX + 1] = ""; /* extension name */
    short extension;    /* does the filename contain an extension */

    dosflag = dosify;  /* default for non-DOS non-OS/2 */

    /* Find starting point in name before doing malloc */
    if (*x == '=')
      t = x + 1;   /* store DEFINE names without the '=' */
    else
      t = x;

    /* Make changes, if any, to the copied name (leave original intact) */

    if (!pathput)
      t = last(t, TANDEM_DELIMITER);

    /* Malloc space for internal name and copy it */
    if ((n = malloc(strlen(t) + 4)) == NULL) /* + 4 for safety */
      return NULL;

    extension = parsename(t,fname,ext);
    t = fname;

    *n= '\0';

    while (*t != '\0') {  /* File part could be sys,vol,subvol or file */
      if (*t == TANDEM_NODE) {    /* System Name */
        strcat(n, INTERNAL_NODE_STR);
        t++;
      }
      else if (*t == TANDEM_DELIMITER) {  /* Volume or Subvol */
             strcat(n, INTERNAL_DELIMITER_STR);
             t++;
           };
      p = strchr(t,TANDEM_DELIMITER);
      if (p == NULL) break;
      strncat(n,t,(p - t));
      t = p;
    }

    strcat(n,t);  /* mop up any left over characters */

    if (extension) {
      strcat(n,DOS_EXTENSION_STR);
      strcat(n,ext);
    };

    if (isdir == 42) return n;      /* avoid warning on unused variable */

    if (dosify)
      msname(n);

    /* Returned malloc'ed name */
    if (pdosflag)
      *pdosflag = dosflag;

    return n;
  }

  void stamp(f, d)
    char *f;                /* name of file to change */
    ulg d;                  /* dos-style time to change it to */
  /* Set last updated and accessed time of file f to the DOS time d. */
  {
    ztimbuf u;            /* argument for utime() */

    /* Convert DOS time to time_t format in u.actime and u.modtime */
    u.actime = u.modtime = dos2unixtime(d);

    utime(f, &u);
  }

  ulg filetime(f, a, n, t)
    char *f;                /* name of file to get info on */
    ulg *a;                 /* return value: file attributes */
    long *n;                /* return value: file size */
    iztimes *t;             /* return value: access and modification time */
  {
    struct stat s;
    nsk_stat_ov *nsk_ov;

    if (strcmp(f, "-") == 0) {    /* if compressing stdin */
      if (n != NULL) {
        *n = -1L;
      }
    }

    if (stat(f, &s) != 0) return 0;

    if (a!= NULL) {
      *a = ((ulg)s.st_mode << 16) | !(s.st_mode & S_IWUSR);
      if ((s.st_mode & S_IFMT) == S_IFDIR) {
        *a |= MSDOS_DIR_ATTR;
      }
    }

    if (n!= NULL)
      *n = (s.st_mode & S_IFMT) == S_IFREG ? s.st_size : -1L;

    if (t != NULL) {
      t->atime = s.st_atime;
      t->mtime = s.st_mtime;
      nsk_ov = (nsk_stat_ov *)&s.st_reserved[0];
      t->ctime = nsk_ov->ov.creation_time;
    }

    return unix2dostime(&s.st_mtime);
  }

  int set_extra_field(z, z_utim)
    struct zlist far *z;
    iztimes *z_utim;
    /* create extra field and change z->att if desired */
    /* store full data in local header but just modification time stamp info
       in central header */
  {
    struct stat s;
    nsk_stat_ov *nsk_ov = (nsk_stat_ov *)&s.st_reserved[0];
    nsk_file_attrs *nsk_attr = (nsk_file_attrs *)&nsk_ov->ov.nsk_ef_region;
    char *ext, *cext;
    int lsize, csize;
#ifdef USE_EF_UT_TIME
    char *UTptr, *Uxptr;
#endif /* USE_EF_UT_TIME */

    /* For the Tandem and UT local field including the UID/GID fields, we
       have to stat the file again. */
    if (LSSTAT(z->name, &s))
      return ZE_OPEN;

    z->ext = z->cext = 0;

  #define EB_TANDEM_SIZE 20
  #define EF_TANDEM_SIZE (EB_HEADSIZE + EB_TANDEM_SIZE)

    /* allocate size of buffers to allow Tandem field */
    lsize = EF_TANDEM_SIZE;
    csize = EF_TANDEM_SIZE;

#ifdef USE_EF_UT_TIME

  #define EB_L_UT_SIZE    (EB_HEADSIZE + EB_UT_LEN(3))
  #define EB_C_UT_SIZE    (EB_HEADSIZE + EB_UT_LEN(1))
  #define EB_L_UX2_SIZE   (EB_HEADSIZE + EB_UX2_MINLEN)
  #define EB_C_UX2_SIZE   EB_HEADSIZE
  #define EF_L_UNIX_SIZE  (EB_L_UT_SIZE + EB_L_UX2_SIZE)
  #define EF_C_UNIX_SIZE  (EB_C_UT_SIZE + EB_C_UX2_SIZE)

    /* resize to allow for UT fields */
    lsize += EF_L_UNIX_SIZE;
    csize += EF_C_UNIX_SIZE;

#endif /* USE_EF_UT_TIME */

    if ((z->extra = (char *)malloc(lsize)) == NULL)
      return ZE_MEM;
    ext = z->extra;

    if ((z->cextra = (char *)malloc(csize)) == NULL)
      return ZE_MEM;
    cext = z->cextra;

    /* Place Tandem field first so its on an even boundary */
    *ext++ = *cext++ = 'T';
    *ext++ = *cext++ = 'A';
    *ext++ = *cext++ = (char)EB_TANDEM_SIZE;  /*length of data part of e.f.*/
    *ext++ = *cext++  = 0;

    /* Copy Tandem specific file information */
    memcpy(ext, (char *)nsk_attr, EB_TANDEM_SIZE);
    ext += EB_TANDEM_SIZE;
    z->ext += EF_TANDEM_SIZE;

    /* Copy same data to central field */
    memcpy(cext, (char *)nsk_attr, EB_TANDEM_SIZE);
    cext += EB_TANDEM_SIZE;
    z->cext += EF_TANDEM_SIZE;

#ifdef USE_EF_UT_TIME
    UTptr = ext;
    *ext++  = 'U';
    *ext++  = 'T';
    *ext++  = (char)EB_UT_LEN(3);    /* length of data part of local e.f. */
    *ext++  = 0;
    *ext++  = EB_UT_FL_MTIME | EB_UT_FL_ATIME | EB_UT_FL_CTIME;
    *ext++  = (char)(s.st_mtime);
    *ext++  = (char)(s.st_mtime >> 8);
    *ext++  = (char)(s.st_mtime >> 16);
    *ext++  = (char)(s.st_mtime >> 24);
    *ext++  = (char)(s.st_atime);
    *ext++ = (char)(s.st_atime >> 8);
    *ext++ = (char)(s.st_atime >> 16);
    *ext++ = (char)(s.st_atime >> 24);

    *ext++ = (char)(nsk_ov->ov.creation_time);
    *ext++ = (char)(nsk_ov->ov.creation_time >> 8);
    *ext++ = (char)(nsk_ov->ov.creation_time >> 16);
    *ext++ = (char)(nsk_ov->ov.creation_time >> 24);

    Uxptr = ext;
    *ext++ = 'U';
    *ext++ = 'x';
    *ext++ = (char)EB_UX2_MINLEN;   /* length of data part of local e.f. */
    *ext++ = 0;
    *ext++ = (char)(s.st_uid);
    *ext++ = (char)(s.st_uid >> 8);
    *ext++ = (char)(s.st_gid);
    *ext++ = (char)(s.st_gid >> 8);

    z->ext += EF_L_UNIX_SIZE;

    memcpy(cext, UTptr, EB_C_UT_SIZE);
    cext[EB_LEN] = (char)EB_UT_LEN(1);
    memcpy(cext+EB_C_UT_SIZE, Uxptr, EB_C_UX2_SIZE);
    cext[EB_LEN+EB_C_UT_SIZE] = 0;

    z->cext += EF_C_UNIX_SIZE;
    cext += EF_C_UNIX_SIZE;

#endif /* USE_EF_UT_TIME */

    return ZE_OK;
  }

#if CRYPT
  /* getpid() only available on OSS so make up dummy version using NSK PID */
  unsigned zgetpid (void)
  {
    short myphandle[ZSYS_VAL_PHANDLE_WLEN];
    short err;
    unsigned retval;

    err = PROCESSHANDLE_NULLIT_(myphandle);

    if (!err)
      err = PROCESS_GETINFO_(myphandle);

    if (!err)
      retval = (unsigned) myphandle[ZSYS_VAL_PHANDLE_WLEN - 3];
    else
#ifndef __INT32
      retval = (unsigned) 31415;
#else
      retval = (unsigned) 3141592654L;
#endif /* __INT32 */

    return retval;
  }
#endif  /* CRYPT */
