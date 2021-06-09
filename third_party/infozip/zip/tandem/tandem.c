/*
  Copyright (c) 1990-2002 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*
 * routines common to TANDEM (ZIP and UNZIP)
 */

#include "zip.h"   /* This sets up ZIP / UNZIP define */

#include <tal.h>
#include "$system.zsysdefs.zsysc" nolist
#include <cextdecs> nolist
#include "tannsk.h"

static time_t gmt_to_time_t (long long *);

int isatty (fnum)
int fnum;
{
  return 1;
}

/********************/
/* Function in2ex() */
/********************/

#ifdef UNZIP
char *in2ex(__G__ n)
  __GDEF
#else
char *in2ex(n)
#endif
  char *n;              /* internal file name */
/* Convert the zip file name to an external file name, returning the malloc'ed
   string or NULL if not enough memory. */
{
  char *x;              /* external file name buffer */
  char *y;              /* pointer to external buffer */
  char *max;            /* pointer to max end of next file part */
  char *t;              /* pointer to internal - start of substring */
  char *p;              /* pointer to internal - TANDEM delimiter */
  char *e;              /* pointer to internal - DOS extension delimiter */
  char *z;              /* pointer to internal - end of substring */
  int len;              /* length of substring to copy to external name */
  int allow_dollar;     /* $ symbol allowed as next character */

  if ((x = malloc(strlen(n) + 4)) == NULL)  /* + 4 for safety */
    return NULL;

  *x = '\0';

  /* Junk pathname as requested */
#ifdef UNZIP
  if (uO.jflag && (t = strrchr(n, INTERNAL_DELIMITER)) != NULL)
    ++t;
  else
    t = n;
#endif /* UNZIP */
#ifdef ZIP
  if (!pathput)
    t = last(n, INTERNAL_DELIMITER);
  else
    t = n;
#endif /* ZIP */

  allow_dollar = TRUE;

  while (*t != '\0') {  /* File part could be sys, vol, subvol or file */
    if (*t == INTERNAL_DELIMITER) {    /* System, Volume or Subvol Name */
      t++;
      if (*t == INTERNAL_DELIMITER) {  /* System */
        strcat(x, TANDEM_NODE_STR);
        t++;
      }
      else {
        strcat(x, TANDEM_DELIMITER_STR);
        allow_dollar = FALSE;
      }
    }
    /* Work out where end of current external string is */
    y = x + strlen(x);

    /* Work out substring to copy and externalise */
    p = strchr(t, INTERNAL_DELIMITER);
    e = strchr(t, DOS_EXTENSION);
    if (p != NULL) {
      if (e > p)
        e = NULL;
    }

    z = e;
    if (z == NULL)
      z = p;
    if (z == NULL)
      z = t + strlen(t);

    /* can't have Tandem name longer than 8 characters */
    max = y + MAXFILEPARTLEN;

    /* Allow $ symbol as first character in some cases */
    if (*t == '$') {
      if (allow_dollar)
        *y++ = *t++;
      else;
        *t++;
    }

    /* Make sure first real character is alpha */
    if (! isalpha(*t) )
      *y++ = 'A';

    /* Characters left to process */
    len = z - t;

    while ( len > 0 ) {
      if ( isalnum(*t) ) {
        *y++ = toupper(*t++);
        if (y >= max)
          break;
      }
      else
        t++;
      len--;
    }
    *y = '\0';
    t = p;

    if (p == NULL) {
      /* Last part of filename, store pseudo extension if available */
      if (e != NULL) {
        strcat(x, TANDEM_EXTENSION_STR);
        y = x + strlen(x);

        /* no restriction on extension length as its virtual */
        z = e + 1;
        while ( *z != '\0' ) {
          *y++ = toupper(*z++);
        }
        *y = '\0';
      }
      break;
    }
  }

  return x;
}

void zexit(status)
  int status;
{
  /* Exit(>0) creates saveabend files */
  terminate_program (0,0,(short)status,,,);
}

/************************/
/*  Function zputc()    */
/************************/

#ifdef putc
#  undef putc
#endif

int zputc(ch, fptr)
  int ch;
  FILE *fptr;
{
  int err;
  err = putc(ch,fptr);
  fflush(fptr);
  return err;
}
#define putc zputc

#ifdef LICENSED
_tal _priv short FILE_CHANGELABEL_ (
 short,          /* IN */
 short,          /* IN */
 const short _far *    /* IN */
 );

_c _callable int changelabel OF((short, const short *, const short *));

_c _callable int changelabel(fnum, modtime, actime)
  short fnum;
  const short *modtime;
  const short *actime;
{
  int err;

  err = FILE_CHANGELABEL_(fnum, 16, modtime);
  if (!err)
    err = FILE_CHANGELABEL_(fnum, 17, actime);
  return err;
}

int islicensed(void)
{
  #define plist_items 1
  #define plist_size 10

  short myphandle[ZSYS_VAL_PHANDLE_WLEN];
  short licensetag[plist_items] = {37};
  short licensed[plist_size];
  short maxlen = plist_size;
  short items = plist_items;
  short resultlen[1], err;

  err = PROCESSHANDLE_NULLIT_(myphandle);

  if (!err)
    err = PROCESS_GETINFO_(myphandle);

  if (!err)
    err = PROCESS_GETINFOLIST_(/*cpu*/,
                               /*pin*/,
                               /*nodename*/,
                               /*nodenamelen*/,
                               myphandle,
                               licensetag,
                               items,
                               licensed,
                               maxlen,
                               resultlen
                              );

  if (err != 0)
    return 0;
  else
    return licensed[0];
}
#endif /* LICENSED */

int utime(file, time)
  const char *file;
  const ztimbuf *time;
{
#ifdef LICENSED
  int result, err;
  union timestamp_ov {
    long long fulltime;
    short wordtime[4];
  };
  union timestamp_ov lasttime, opentime;
  struct tm *modt, *opent;
  short datetime[8], errormask[1];
  short len, fnum, access, exclus, options;
  char fname[FILENAME_MAX + 1];
  short extension;
  char ext[EXTENSION_MAX + 1];

  if (islicensed() ) {
    /* Attempt to update file label */
    modt = gmtime( &time->modtime );

    datetime[0] = modt->tm_year + 1900;
    datetime[1] = modt->tm_mon + 1;
    datetime[2] = modt->tm_mday;
    datetime[3] = modt->tm_hour;
    datetime[4] = modt->tm_min;
    datetime[5] = modt->tm_sec;
    datetime[6] = datetime[7] = 0;
    errormask[0] = 0;
    lasttime.fulltime = COMPUTETIMESTAMP (datetime, errormask);

    opent = gmtime( &time->actime );

    datetime[0] = opent->tm_year + 1900;
    datetime[1] = opent->tm_mon + 1;
    datetime[2] = opent->tm_mday;
    datetime[3] = opent->tm_hour;
    datetime[4] = opent->tm_min;
    datetime[5] = opent->tm_sec;
    datetime[6] = datetime[7] = 0;
    errormask[0] = 0;
    opentime.fulltime = COMPUTETIMESTAMP (datetime, errormask);

    /* Remove any (pseudo) file extension */
    extension = parsename (file,fname,ext);
    len = strlen(fname);

    access = NSK_WRONLY;
    exclus = NSK_SHARED;
    options = NSK_NOUPDATEOPENTIME;

    extension = parsename (file,fname,ext);
    len = strlen(fname);

    err = FILE_OPEN_((char *)fname, len, &fnum, access, exclus,,,options,,,);
    result = changelabel(fnum,lasttime.wordtime,opentime.wordtime);
    err = FILE_CLOSE_(fnum);
    return result;
  }
  return -1;
#else  /* !LICENSED */
  return 0;             /* "no error", to suppress annoying failure messages */
#endif  /* ?LICENSED */
}

/* TANDEM version of chmod() function */

int chmod(file, unix_sec)
  const char *file;
  mode_t unix_sec;
{
  FILE *stream;
  struct nsk_sec_type {
    unsigned progid : 1;
    unsigned clear  : 1;
    unsigned null   : 2;
    unsigned read   : 3;
    unsigned write  : 3;
    unsigned execute: 3;
    unsigned purge  : 3;
  };
  union nsk_sec_ov {
    struct nsk_sec_type bit_ov;
    short int_ov;
  };
  union nsk_sec_ov nsk_sec;
  short fnum, err, nsk_sec_int;
  short len, access, exclus, extension, options;
  char fname[FILENAME_MAX + 1];
  char ext[EXTENSION_MAX + 1];

  nsk_sec.bit_ov.progid = 0;
  nsk_sec.bit_ov.clear  = 0;
  nsk_sec.bit_ov.null   = 0;

  /*  4="N", 5="C", 6="U", 7="-"   */

  if (unix_sec & S_IROTH) nsk_sec.bit_ov.read = 4;
  else if (unix_sec & S_IRGRP) nsk_sec.bit_ov.read = 5;
  else if (unix_sec & S_IRUSR) nsk_sec.bit_ov.read = 6;
  else nsk_sec.bit_ov.read = 7;

  if (unix_sec & S_IWOTH) nsk_sec.bit_ov.write = 4;
  else if (unix_sec & S_IWGRP) nsk_sec.bit_ov.write = 5;
  else if (unix_sec & S_IWUSR) nsk_sec.bit_ov.write = 6;
  else nsk_sec.bit_ov.write = 7;

  if (unix_sec & S_IXOTH) nsk_sec.bit_ov.execute = 4;
  else if (unix_sec & S_IXGRP) nsk_sec.bit_ov.execute = 5;
  else if (unix_sec & S_IXUSR) nsk_sec.bit_ov.execute = 6;
  else nsk_sec.bit_ov.execute = 7;

  nsk_sec.bit_ov.purge = nsk_sec.bit_ov.write;

  nsk_sec_int = nsk_sec.int_ov;

  access = NSK_RDONLY;
  exclus = NSK_SHARED;
  options = NSK_NOUPDATEOPENTIME;

  extension = parsename (file,fname,ext);
  len = strlen(fname);

  err = FILE_OPEN_((char *)fname, len, &fnum, access, exclus,,,options,,,);
  err = (SETMODE(fnum, SET_FILE_SECURITY, nsk_sec_int) != CCE);
  err = FILE_CLOSE_(fnum);

  return (err != 0 ? -1 : 0);
}

/* TANDEM version of chown() function */

int chown(file, uid, gid)
  const char *file;
  uid_t uid;
  gid_t gid;
{
  FILE *stream;
  struct nsk_own_type {
    unsigned group  : 8;
    unsigned user   : 8;
  };
  union nsk_own_ov {
    struct nsk_own_type bit_ov;
    short int_ov;
  };
  union nsk_own_ov nsk_own;
  short fnum, err, nsk_own_int;
  short len, access, exclus, extension, options;
  char fname[FILENAME_MAX + 1];
  char ext[EXTENSION_MAX + 1];

  nsk_own.bit_ov.group = gid;
  nsk_own.bit_ov.user  = uid;

  nsk_own_int = nsk_own.int_ov;

  access = NSK_RDONLY;
  exclus = NSK_SHARED;
  options = NSK_NOUPDATEOPENTIME;

  extension = parsename (file,fname,ext);
  len = strlen(fname);

  err = FILE_OPEN_((char *)fname, len, &fnum, access, exclus,,,options,,,);
  err = (SETMODE(fnum, SET_FILE_OWNER, nsk_own_int) != CCE);
  err = FILE_CLOSE_(fnum);
  return (err != 0 ? -1 : 0);
}

/* TANDEM version of getch() - non-echo character reading */
int zgetch(void)
{
  char ch;
  short f, err, count, fnum, rlen;

  rlen = 1;
  f = (short)fileno(stdin);
  fnum = fdtogfn (f);
  #define ECHO_MODE 20
  err = (SETMODE(fnum, ECHO_MODE, 0) != CCE);
  err = (READX(fnum, &ch, rlen, (short *) &count) != CCE);
  err = (SETMODE(fnum, ECHO_MODE, 1) != CCE);

  if (err)
    if (err != 1)
      return EOF;
    else
      ch = 'q';
  else
    if (count == 0)
      ch = '\r';

  return (int)ch;
}

short parsename(srce, fname, ext)
  const char *srce;
  char *fname;
  char *ext;
{
  /* As a way of supporting DOS extensions from Tandem we look for a space
     separated extension string after the Guardian filename
     e.g. ZIP ZIPFILE "$DATA4.TESTING.INVOICE TXT"
  */

  char *fstart;
  char *fptr;
  short extension = 0;

  *fname = *ext = '\0';  /* set to null string */

  fstart = (char *) srce;

  if ((fptr = strrchr(fstart, TANDEM_EXTENSION)) != NULL) {
    extension = 1;

    fptr++;
    strncat(ext, fptr, _min(EXTENSION_MAX, strlen(fptr)));

    fptr = strchr(fstart, TANDEM_EXTENSION);  /* End of filename */
    strncat(fname, fstart, _min(FILENAME_MAX, (fptr - fstart)));
  }
  else {
    /* just copy string */
    strncat(fname, srce, _min(FILENAME_MAX, strlen(srce)));
  }

  return extension;
}

static time_t gmt_to_time_t (gmt)
  long long *gmt;
{
  #define GMT_TO_LCT 0
  #define GMT_TO_LST 1

  struct tm temp_tm;
  short  date_time[8];
  long   julian_dayno;
  long long lct, lst, itime;
  short  err[1], type;

  type = GMT_TO_LCT;
  lct = CONVERTTIMESTAMP(*gmt, type,, err);

  if (!err[0]) {
    type = GMT_TO_LST;
    lst = CONVERTTIMESTAMP(*gmt, type,, err);
  }

  itime = (err[0] ? *gmt : lct);
  /* If we have no DST in force then make sure we give it a value,
     else mktime screws up if we set the isdst flag to -1 */
  temp_tm.tm_isdst = (err[0] ? 0 : ((lct == lst) ? 0 : 1));

  julian_dayno = INTERPRETTIMESTAMP(itime, date_time);

  temp_tm.tm_sec   = date_time[5];
  temp_tm.tm_min   = date_time[4];
  temp_tm.tm_hour  = date_time[3];
  temp_tm.tm_mday  = date_time[2];
  temp_tm.tm_mon   = date_time[1] - 1;     /* C's so sad */
  temp_tm.tm_year  = date_time[0] - 1900;  /* it's almost funny */

  return (mktime(&temp_tm));
}

/* TANDEM version of stat() function */
int stat(n, s)
  const char *n;
  struct stat *s;
{
  #define ilist_items 26
  #define klist_items 4
  #define slist_items 3
  #define ulist_items 1
  #define flist_size 100

  short err, i, extension;
  char fname[FILENAME_MAX + 1];
  short fnamelen;
  char ext[EXTENSION_MAX + 1];

                         /* #0  #1  #2  #3  #4  #5  #6  #7  #8  #9 */
  short ilist[ilist_items]={56,144, 54,142, 58, 62, 60, 41, 42, 44,
                            50, 51, 52, 61, 63, 66, 67, 70, 72, 73,
                            74, 75, 76, 77, 78, 79                 };
  short ilen[ilist_items] ={ 4,  4,  4,  2,  1,  2,  1,  1,  1,  1,
                             1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
                             1,  1,  1,  1,  1,  1                 };
  short ioff[ilist_items];

                         /* #0  #1  #2  #3  #4  #5  #6  #7  #8  #9 */
  short klist[klist_items]={45, 46, 68, 69                         };
  short klen[klist_items] ={ 1,  1,  1,  1                         };
  short koff[klist_items];

                         /* #0  #1  #2  #3  #4  #5  #6  #7  #8  #9 */
  short slist[slist_items]={43, 80, 90                             };
  short slen[slist_items] ={ 1,  1,  1                             };
  short soff[slist_items];

                         /* #0  #1  #2  #3  #4  #5  #6  #7  #8  #9 */
  short ulist[ulist_items]={65                                     };
  short ulen[ulist_items] ={ 1                                     };
  short uoff[ulist_items];

  short flist[flist_size];
  short extra[2];
  short *rlen=&extra[0];
  short *err_item=&extra[1];
  unsigned short *fowner;
  unsigned short *fprogid;
  char *fsec;

  nsk_stat_ov *nsk_ov;
  nsk_file_attrs *nsk_attr;

  short end, count, kind, level, options, searchid;
  short info[5];

  /* Initialise stat structure */
  s->st_dev = _S_GUARDIANOBJECT;
  s->st_ino = 0;
  s->st_nlink = 0;
  s->st_rdev = 0;
  s->st_uid = s->st_gid = 0;
  s->st_size = 0;
  s->st_atime = s->st_ctime = s->st_mtime = 0;
  s->st_reserved[0] = 0;
  s->st_reserved[1] = 0;
  s->st_reserved[2] = 0;
  nsk_ov = (nsk_stat_ov *)&s->st_reserved[0];
  nsk_attr = (nsk_file_attrs *)&nsk_ov->ov.nsk_ef_region;

  /* Check to see if name contains a (pseudo) file extension */
  extension = parsename (n,fname,ext);

  fnamelen = strlen(fname);

  options = 3; /* Allow Subvols and Templates */
  err = FILENAME_SCAN_( fname,
                        fnamelen,
                        &count,
                        &kind,
                        &level,
                        options
                      );

  /* allow kind == 2 (DEFINE names) */
  if (err != 0) return -1;

  if (kind == 1 || (kind == 0 && level < 2)) {
    /* Pattern, Subvol Name or One part Filename - lets see if it exists */
    err = FILENAME_FINDSTART_ ( &searchid,
                                fname,
                                fnamelen,
                                ,
                                DISK_DEVICE
                              );

    if (err != 0) {
      end = FILENAME_FINDFINISH_ ( searchid );
      return -1;
    }

    err = FILENAME_FINDNEXT_ ( searchid,
                               fname,
                               FILENAME_MAX,
                               &fnamelen,
                               info
                              );
    end = FILENAME_FINDFINISH_ ( searchid );

    if (err != 0)
      return -1;  /* Non existing template, subvol or file */

    if (kind == 1 || info[2] == -1) {
      s->st_mode = S_IFDIR;    /* Its an existing template or directory */
      return 0;
    }

    /* Must be a real file so drop to code below to get info on it */
  }

  err = FILE_GETINFOLISTBYNAME_( fname,
                                 fnamelen,
                                 ilist,
                                 ilist_items,
                                 flist,
                                 flist_size,
                                 rlen,
                                 err_item
                               );
  if (err != 0) return -1;

  ioff[0] = 0;

  /*  Build up table of offets into result list */
  for (i=1; i < ilist_items; i++)
    ioff[i] = ioff[i-1] + ilen[i-1];

  /* Set up main stat fields */

  /* Setup timestamps */
  s->st_atime = gmt_to_time_t ((long long *)&flist[ioff[0]]);
  s->st_mtime = s->st_ctime = gmt_to_time_t ((long long *)&flist[ioff[1]]);
  nsk_ov->ov.creation_time = gmt_to_time_t ((long long *)&flist[ioff[2]]);

  s->st_size = *(off_t *)&flist[ioff[3]];

  fowner = (unsigned short *)&flist[ioff[4]];
  s->st_uid = *fowner & 0x00ff;
  s->st_gid = *fowner >> 8;

  /* Note that Purge security (fsec[3]) in NSK has no relevance to stat() */
  fsec = (char *)&flist[ioff[5]];
  fprogid = (unsigned short *)&flist[ioff[6]];

  s->st_mode = S_IFREG |  /* Regular File */
  /*  Parse Read Flag */
               ((fsec[0] & 0x03) == 0x00 ? S_IROTH : 0) |
               ((fsec[0] & 0x02) == 0x00 ? S_IRGRP : 0) |
               ((fsec[0] & 0x03) != 0x03 ? S_IRUSR : 0) |
  /*  Parse Write Flag */
               ((fsec[1] & 0x03) == 0x00 ? S_IWOTH : 0) |
               ((fsec[1] & 0x02) == 0x00 ? S_IWGRP : 0) |
               ((fsec[1] & 0x03) != 0x03 ? S_IWUSR : 0) |
  /*  Parse Execute Flag */
               ((fsec[2] & 0x03) == 0x00 ? S_IXOTH : 0) |
               ((fsec[2] & 0x02) == 0x00 ? S_IXGRP : 0) |
               ((fsec[2] & 0x03) != 0x03 ? S_IXUSR : 0) |
  /*  Parse Progid */
               (*fprogid == 1 ? (S_ISUID | S_ISGID) : 0) ;

  /* Set up NSK additional stat fields */
  nsk_attr->progid   = (unsigned) flist[ioff[6]];
  nsk_attr->filetype = (unsigned) flist[ioff[7]];
  nsk_attr->filecode = (unsigned) flist[ioff[8]];
  nsk_attr->block    = (unsigned short) flist[ioff[9]];
  nsk_attr->priext   = (unsigned short) flist[ioff[10]];
  nsk_attr->secext   = (unsigned short) flist[ioff[11]];
  nsk_attr->maxext   = (unsigned short) flist[ioff[12]];
  nsk_attr->flags.clearonpurge = (unsigned) flist[ioff[13]];
  nsk_attr->licensed     = (unsigned) flist[ioff[14]];
  nsk_attr->flags.audited      = (unsigned) flist[ioff[15]];
  nsk_attr->flags.acompress    = (unsigned) flist[ioff[16]];
  nsk_attr->flags.refresheof   = (unsigned) flist[ioff[17]];
  nsk_attr->flags.buffered     = (unsigned) (flist[ioff[18]] == 0 ? 1 : 0);
  nsk_attr->flags.verified     = (unsigned) flist[ioff[19]];
  nsk_attr->flags.serial       = (unsigned) flist[ioff[20]];
  nsk_attr->flags.crashopen    = (unsigned) flist[ioff[22]];
  nsk_attr->flags.rollforward  = (unsigned) flist[ioff[23]];
  nsk_attr->flags.broken       = (unsigned) flist[ioff[24]];
  nsk_attr->flags.corrupt      = (unsigned) flist[ioff[25]];
  nsk_attr->fileopen     = (unsigned) flist[ioff[21]];


  if (nsk_attr->filetype == NSK_UNSTRUCTURED) {
    /* extra info for Unstructured files */
    err = FILE_GETINFOLISTBYNAME_( fname,
                                   fnamelen,
                                   ulist,
                                   ulist_items,
                                   flist,
                                   flist_size,
                                   rlen,
                                   err_item
                                 );
    if (err != 0) return -1;

    uoff[0] = 0;

    /*  Build up table of offets into result list */
    for (i=1; i < ulist_items; i++)
      uoff[i] = uoff[i-1] + ulen[i-1];
  }
  else {
    /* extra info for Structured files */
    err = FILE_GETINFOLISTBYNAME_( fname,
                                   fnamelen,
                                   slist,
                                   slist_items,
                                   flist,
                                   flist_size,
                                   rlen,
                                   err_item
                                 );
    if (err != 0) return -1;

    soff[0] = 0;

    /*  Build up table of offets into result list */
    for (i=1; i < slist_items; i++)
      soff[i] = soff[i-1] + slen[i-1];

    nsk_attr->reclen   = (unsigned) flist[soff[0]];
    nsk_attr->flags.secpart   = (unsigned) flist[soff[1]];
    nsk_attr->flags.primpart  = (unsigned)
     ( (flist[soff[2]] > 0 && nsk_attr->flags.secpart == 0) ? 1 : 0 );

    if (nsk_attr->filetype == NSK_KEYSEQUENCED) {
      /* extra info for Key Sequenced files */
      err = FILE_GETINFOLISTBYNAME_( fname,
                                     fnamelen,
                                     klist,
                                     klist_items,
                                     flist,
                                     flist_size,
                                     rlen,
                                     err_item
                                   );
      if (err != 0) return -1;

      koff[0] = 0;

      /*  Build up table of offets into result list */
      for (i=1; i < klist_items; i++)
        koff[i] = koff[i-1] + klen[i-1];

      nsk_attr->keyoff   = (unsigned) flist[koff[0]];
      nsk_attr->keylen   = (unsigned) flist[koff[1]];
      nsk_attr->flags.dcompress = (unsigned) flist[koff[2]];
      nsk_attr->flags.icompress = (unsigned) flist[koff[3]];
    }
  }

  return 0;
}

#ifndef SFX
/* TANDEM Directory processing */

DIR *opendir(const char *dirname)
{
   short i, resolve;
   char sname[FILENAME_MAX + 1];
   short snamelen;
   char fname[FILENAME_MAX + 1];
   short fnamelen;
   char *p;
   short searchid, err, end;
   struct dirent *entry;
   DIR *dirp;
   char ext[EXTENSION_MAX + 1];
   short extension;

   extension = parsename(dirname, sname, ext);
   snamelen = strlen(sname);

   /*  First we work out how detailed the template is...
    *  e.g. If the template is DAVES*.* we want the search result
    *       in the same format
    */

   p = sname;
   i = 0;
   while ((p = strchr(p, TANDEM_DELIMITER)) != NULL){
     i++;
     p++;
   };
   resolve = 2 - i;

   /*  Attempt to start a filename template */
   err = FILENAME_FINDSTART_ ( &searchid,
                               sname,
                               snamelen,
                               resolve,
                               DISK_DEVICE
                             );
   if (err != 0) {
     end = FILENAME_FINDFINISH_(searchid);
     return NULL;
   }

   /* Create DIR structure */
   if ((dirp = malloc(sizeof(DIR))) == NULL ) {
     end = FILENAME_FINDFINISH_(searchid);
     return NULL;
   }
   dirp->D_list = dirp->D_curpos = NULL;
   strcpy(dirp->D_path, dirname);

   while ((err = FILENAME_FINDNEXT_(searchid,
                                    fname,
                                    FILENAME_MAX,
                                    &fnamelen
                                   )
           ) == 0 ){
     /*  Create space for entry */
     if ((entry = malloc (sizeof(struct dirent))) == NULL) {
       end = FILENAME_FINDFINISH_(searchid);
       return NULL;
     }

     /*  Link to last entry */
     if (dirp->D_curpos == NULL)
       dirp->D_list = dirp->D_curpos = entry;  /* First name */
     else {
       dirp->D_curpos->d_next = entry;         /* Link */
       dirp->D_curpos = entry;
     };
     /* Add directory entry */
     *dirp->D_curpos->d_name = '\0';
     strncat(dirp->D_curpos->d_name,fname,fnamelen);
     if (extension) {
       strcat(dirp->D_curpos->d_name,TANDEM_EXTENSION_STR);
       strcat(dirp->D_curpos->d_name,ext);
     };
     dirp->D_curpos->d_next = NULL;
   };

   end = FILENAME_FINDFINISH_(searchid);

   if (err == 1) {  /*  Should return EOF at end of search */
     dirp->D_curpos = dirp->D_list;        /* Set current pos to start */
     return dirp;
   }
   else
     return NULL;
}

struct dirent *readdir(DIR *dirp)
{
   struct dirent *cur;

   cur = dirp->D_curpos;
   dirp->D_curpos = dirp->D_curpos->d_next;
   return cur;
}

void rewinddir(DIR *dirp)
{
   dirp->D_curpos = dirp->D_list;
}

int closedir(DIR *dirp)
{
   struct dirent *node;

   while (dirp->D_list != NULL) {
      node = dirp->D_list;
      dirp->D_list = dirp->D_list->d_next;
      free( node );
   }
   free( dirp );
   return 0;
}

#endif /* !SFX */
