/*
  zipfile.c - Zip 3

  Copyright (c) 1990-2008 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2007-Mar-4 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*
 *  zipfile.c by Mark Adler.
 */
#define __ZIPFILE_C

#include "zip.h"
#include "revision.h"
#ifdef UNICODE_SUPPORT
# include "crc32.h"
#endif

/* for realloc 2/6/2005 EG */
#include <stdlib.h>

#include <errno.h>

/* for toupper() */
#include <ctype.h>

#ifdef VMS
#  include "vms/vms.h"
#  include "vms/vmsmunch.h"
#  include "vms/vmsdefs.h"
#endif

#ifdef WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#endif

/*
 * XXX start of zipfile.h
 */
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

/* Macros for writing machine integers to little-endian format */
#define PUTSH(a,f) {putc((char)((a) & 0xff),(f)); putc((char)((a) >> 8),(f));}
#define PUTLG(a,f) {PUTSH((a) & 0xffff,(f)) PUTSH((a) >> 16,(f))}

#ifdef ZIP64_SUPPORT           /* zip64 support 08/31/2003 R.Nausedat */
# define PUTLLG(a,f) {PUTLG((a) & 0xffffffff,(f)) PUTLG((a) >> 32,(f))}
#endif


/* -- Structure of a ZIP file -- */

/* Signatures for zip file information headers */
#define LOCSIG     0x04034b50L
#define CENSIG     0x02014b50L
#define ENDSIG     0x06054b50L
#define EXTLOCSIG  0x08074b50L

/* Offsets of values in headers */
/* local header */
#define LOCVER  0               /* version needed to extract */
#define LOCFLG  2               /* encrypt, deflate flags */
#define LOCHOW  4               /* compression method */
#define LOCTIM  6               /* last modified file time, DOS format */
#define LOCDAT  8               /* last modified file date, DOS format */
#define LOCCRC  10              /* uncompressed crc-32 for file */
#define LOCSIZ  14              /* compressed size in zip file */
#define LOCLEN  18              /* uncompressed size */
#define LOCNAM  22              /* length of filename */
#define LOCEXT  24              /* length of extra field */

/* extended local header (data descriptor) following file data (if bit 3 set) */
/* if Zip64 then all are 8 byte and not below - 11/1/03 EG */
#define EXTCRC  0               /* uncompressed crc-32 for file */
#define EXTSIZ  4               /* compressed size in zip file */
#define EXTLEN  8               /* uncompressed size */

/* central directory header */
#define CENVEM  0               /* version made by */
#define CENVER  2               /* version needed to extract */
#define CENFLG  4               /* encrypt, deflate flags */
#define CENHOW  6               /* compression method */
#define CENTIM  8               /* last modified file time, DOS format */
#define CENDAT  10              /* last modified file date, DOS format */
#define CENCRC  12              /* uncompressed crc-32 for file */
#define CENSIZ  16              /* compressed size in zip file */
#define CENLEN  20              /* uncompressed size */
#define CENNAM  24              /* length of filename */
#define CENEXT  26              /* length of extra field */
#define CENCOM  28              /* file comment length */
#define CENDSK  30              /* disk number start */
#define CENATT  32              /* internal file attributes */
#define CENATX  34              /* external file attributes */
#define CENOFF  38              /* relative offset of local header */

/* end of central directory record */
#define ENDDSK  0               /* number of this disk */
#define ENDBEG  2               /* number of the starting disk */
#define ENDSUB  4               /* entries on this disk */
#define ENDTOT  6               /* total number of entries */
#define ENDSIZ  8               /* size of entire central directory */
#define ENDOFF  12              /* offset of central on starting disk */
#define ENDCOM  16              /* length of zip file comment */

/* zip64 support 08/31/2003 R.Nausedat */

/* EOCDL_SIG used to detect Zip64 archive */
#define ZIP64_EOCDL_SIG                  0x07064b50
/* EOCDL size is used in the empty archive check */
#define ZIP64_EOCDL_OFS_SIZE                20

#define ZIP_UWORD16_MAX                  0xFFFF                        /* border value */
#define ZIP_UWORD32_MAX                  0xFFFFFFFF                    /* border value */
#define ZIP_EF_HEADER_SIZE               4                             /* size of pre-header of extra fields */

#ifdef ZIP64_SUPPORT
# define ZIP64_EXTCRC                    0                             /* uncompressed crc-32 for file */
# define ZIP64_EXTSIZ                    4                             /* compressed size in zip file */
# define ZIP64_EXTLEN                    12                            /* uncompressed size */
# define ZIP64_EOCD_SIG                  0x06064b50
# define ZIP64_EOCD_OFS_SIZE             40
# define ZIP64_EOCD_OFS_CD_START         48
# define ZIP64_EOCDL_OFS_SIZE                20
# define ZIP64_EOCDL_OFS_EOCD_START      8
# define ZIP64_EOCDL_OFS_TOTALDISKS      16
# define ZIP64_MIN_VER                   45                            /* min version to set in the CD extra records */
# define ZIP64_CENTRAL_DIR_TAIL_SIZE     (56 - 8 - 4)                  /* size of zip64 central dir tail, minus sig and size field bytes */
# define ZIP64_CENTRAL_DIR_TAIL_SIG      0x06064B50L                   /* zip64 central dir tail signature */
# define ZIP64_CENTRAL_DIR_TAIL_END_SIG  0x07064B50L                   /* zip64 end of cen dir locator signature */
# define ZIP64_LARGE_FILE_HEAD_SIZE      32                            /* total size of zip64 extra field */
# define ZIP64_EF_TAG                    0x0001                        /* ID for zip64 extra field */
# define ZIP64_EFIELD_OFS_OSIZE          ZIP_EF_HEADER_SIZE            /* zip64 extra field: offset to original file size */
# define ZIP64_EFIELD_OFS_CSIZE          (ZIP64_EFIELD_OFS_OSIZE + 8)  /* zip64 extra field: offset to compressed file size */
# define ZIP64_EFIELD_OFS_OFS            (ZIP64_EFIELD_OFS_CSIZE + 8)  /* zip64 extra field: offset to offset in archive */
# define ZIP64_EFIELD_OFS_DISK           (ZIP64_EFIELD_OFS_OFS + 8)    /* zip64 extra field: offset to start disk # */
/* -------------------------------------------------------------------------------------------------------------------------- */
 local int adjust_zip_local_entry OF((struct zlist far *));
 local void adjust_zip_central_entry OF((struct zlist far *));
#if 0
 local int remove_local_extra_field OF((struct zlist far *, ulg));
 local int remove_central_extra_field OF((struct zlist far *, ulg));
#endif
 local int add_central_zip64_extra_field OF((struct zlist far *));
 local int add_local_zip64_extra_field OF((struct zlist far *));
#endif /* ZIP64_SUPPORT */
#ifdef UNICODE_SUPPORT
# define UTF8_PATH_EF_TAG                0x7075                        /* ID for Unicode path (up) extra field */
 local int add_Unicode_Path_local_extra_field OF((struct zlist far *));
 local int add_Unicode_Path_cen_extra_field OF((struct zlist far *));
#endif

/* New General Purpose Bit Flag bit 11 flags when entry path and
   comment are in UTF-8 */
#define UTF8_BIT (1 << 11)

/* moved out of ZIP64_SUPPORT - 2/6/2005 EG */
local void write_ushort_to_mem OF((ush, char *));                      /* little endian conversions */
local void write_ulong_to_mem OF((ulg, char *));
#ifdef ZIP64_SUPPORT
 local void write_int64_to_mem OF((uzoff_t, char *));
#endif /* def ZIP64_SUPPORT */
#ifdef UNICODE_SUPPORT
 local void write_string_to_mem OF((char *, char *));
#endif
#if 0
local char *get_extra_field OF((ush, char *, unsigned));           /* zip64 */
#endif
#ifdef UNICODE_SUPPORT
local void read_Unicode_Path_entry OF((struct zlist far *));
local void read_Unicode_Path_local_entry OF((struct zlist far *));
#endif

/* added these self allocators - 2/6/2005 EG */
local void append_ushort_to_mem OF((ush, char **, extent *, extent *));
local void append_ulong_to_mem OF((ulg, char **, extent *, extent *));
#ifdef ZIP64_SUPPORT
 local void append_int64_to_mem OF((uzoff_t, char **, extent *, extent *));
#endif /* def ZIP64_SUPPORT */
local void append_string_to_mem OF((char *, int, char**, extent *, extent *));


/* Local functions */

local int find_next_signature OF((FILE *f));
local int find_signature OF((FILE *, ZCONST char *));
local int is_signature OF((ZCONST char *, ZCONST char *));
local int at_signature OF((FILE *, ZCONST char *));

local int zqcmp OF((ZCONST zvoid *, ZCONST zvoid *));
#ifdef UNICODE_SUPPORT
local int zuqcmp OF((ZCONST zvoid *, ZCONST zvoid *));
#endif
#if 0
 local int scanzipf_reg OF((FILE *f));
#endif
local int scanzipf_regnew OF((void));
#ifndef UTIL
 local int rqcmp OF((ZCONST zvoid *, ZCONST zvoid *));
 local int zbcmp OF((ZCONST zvoid *, ZCONST zvoid far *));
# ifdef UNICODE_SUPPORT
 local int zubcmp OF((ZCONST zvoid *, ZCONST zvoid far *));
#  if 0
 local int zuebcmp OF((ZCONST zvoid *, ZCONST zvoid far *));
#  endif
# endif /* UNICODE_SUPPORT */
 local void zipoddities OF((struct zlist far *));
# if 0
  local int scanzipf_fix OF((FILE *f));
# endif
 local int scanzipf_fixnew OF((void));
# ifdef USE_EF_UT_TIME
   local int ef_scan_ut_time OF((char *ef_buf, extent ef_len, int ef_is_cent,
                                   iztimes *z_utim));
# endif /* USE_EF_UT_TIME */
 local void cutpath OF((char *p, int delim));
#endif /* !UTIL */

/*
 * XXX end of zipfile.h
 */

/* Local data */

#ifdef HANDLE_AMIGA_SFX
   ulg amiga_sfx_offset;        /* place where size field needs updating */
#endif

local int zqcmp(a, b)
ZCONST zvoid *a, *b;          /* pointers to pointers to zip entries */
/* Used by qsort() to compare entries in the zfile list.
 * Compares the internal names z->iname */
{
  char *aname = (*(struct zlist far **)a)->iname;
  char *bname = (*(struct zlist far **)b)->iname;

  return namecmp(aname, bname);
}

#ifdef UNICODE_SUPPORT
local int zuqcmp(a, b)
ZCONST zvoid *a, *b;          /* pointers to pointers to zip entries */
/* Used by qsort() to compare entries in the zfile list.
 * Compares the internal names z->zuname */
{
  char *aname = (*(struct zlist far **)a)->iname;
  char *bname = (*(struct zlist far **)b)->iname;

  /* zuname could be NULL */
  if ((*(struct zlist far **)a)->zuname)
    aname = (*(struct zlist far **)a)->zuname;
  if ((*(struct zlist far **)b)->zuname)
    bname = (*(struct zlist far **)b)->zuname;
  return namecmp(aname, bname);
}
#endif


#ifndef UTIL

local int rqcmp(a, b)
ZCONST zvoid *a, *b;          /* pointers to pointers to zip entries */
/* Used by qsort() to compare entries in the zfile list.
 * Compare the internal names z->iname, but in reverse order. */
{
  return namecmp((*(struct zlist far **)b)->iname,
                 (*(struct zlist far **)a)->iname);
}


local int zbcmp(n, z)
ZCONST zvoid *n;        /* string to search for */
ZCONST zvoid far *z;    /* pointer to a pointer to a zip entry */
/* Used by search() to compare a target to an entry in the zfile list. */
{
  return namecmp((char *)n, ((struct zlist far *)z)->zname);
}

#ifdef UNICODE_SUPPORT
/* search unicode paths */
local int zubcmp(n, z)
ZCONST zvoid *n;        /* string to search for */
ZCONST zvoid far *z;    /* pointer to a pointer to a zip entry */
/* Used by search() to compare a target to an entry in the zfile list. */
{
  char *zuname = ((struct zlist far *)z)->zuname;

  /* zuname is NULL if no UTF-8 name */
  if (zuname == NULL)
    zuname = ((struct zlist far *)z)->zname;

  return namecmp((char *)n, zuname);
}

#if 0
/* search escaped unicode paths */
local int zuebcmp(n, z)
ZCONST zvoid *n;        /* string to search for */
ZCONST zvoid far *z;    /* pointer to a pointer to a zip entry */
/* Used by search() to compare a target to an entry in the zfile list. */
{
  char *zuname = ((struct zlist far *)z)->zuname;
  char *zuename;
  int k;

  /* zuname is NULL if no UTF-8 name */
  if (zuname == NULL)
    zuname = ((struct zlist far *)z)->zname;
  zuename = local_to_escape_string(zuname);
  k = namecmp((char *)n, zuename);
  free(zuename);

  return k;
}
#endif
#endif


struct zlist far *zsearch(n)
  ZCONST char *n;      /* name to find */
/* Return a pointer to the entry in zfile with the name n, or NULL if
   not found. */
{
  zvoid far **p;        /* result of search() */

  if (zcount) {
    if ((p = search(n, (ZCONST zvoid far **)zsort, zcount, zbcmp)) != NULL)
      return *(struct zlist far **)p;
#ifdef UNICODE_SUPPORT
    else if (unicode_mismatch != 3 && fix != 2 &&
        (p = search(n, (ZCONST zvoid far **)zusort, zcount, zubcmp)) != NULL)
      return *(struct zlist far **)p;
#endif
    else
      return NULL;
  }
  return NULL;
}

#endif /* !UTIL */

#ifndef VMS     /* See [.VMS]VMS.C for VMS-specific ziptyp(). */
#  ifndef PATHCUT
#    define PATHCUT '/'
#  endif

char *ziptyp(s)
  char *s;             /* file name to force to zip */
/* If the file name *s has a dot (other than the first char), or if
   the -A option is used (adjust self-extracting file) then return
   the name, otherwise append .zip to the name.  Allocate the space for
   the name in either case.  Return a pointer to the new name, or NULL
   if malloc() fails. */
{
  char *q;              /* temporary pointer */
  char *t;              /* pointer to malloc'ed string */
#  ifdef THEOS
  char *r;              /* temporary pointer */
  char *disk;
#  endif

  if ((t = malloc(strlen(s) + 5)) == NULL)
    return NULL;
  strcpy(t, s);
#  ifdef __human68k__
  _toslash(t);
#  endif
#  ifdef MSDOS
  for (q = t; *q; INCSTR(q))
    if (*q == '\\')
      *q = '/';
#  endif /* MSDOS */
#  if defined(__RSXNT__) || defined(WIN32_CRT_OEM)
   /* RSXNT/EMX C rtl uses OEM charset */
  AnsiToOem(t, t);
#  endif
  if (adjust) return t;
#  ifndef RISCOS
#    ifndef QDOS
#      ifdef AMIGA
  if ((q = MBSRCHR(t, '/')) == NULL)
    q = MBSRCHR(t, ':');
  if (MBSRCHR((q ? q + 1 : t), '.') == NULL)
#      else /* !AMIGA */
#        ifdef THEOS
  /* the argument expansion add a dot to the end of file names when
   * there is no extension and at least one of a argument has wild cards.
   * So check for at least one character in the extension if there is a dot
   * in file name */
  if ((q = MBSRCHR((q = MBSRCHR(t, PATHCUT)) == NULL ? t : q + 1, '.')) == NULL
    || q[1] == '\0') {
#        else /* !THEOS */
#          ifdef TANDEM
  if (MBSRCHR((q = MBSRCHR(t, '.')) == NULL ? t : q + 1, ' ') == NULL)
#          else /* !TANDEM */
  if (MBSRCHR((q = MBSRCHR(t, PATHCUT)) == NULL ? t : q + 1, '.') == NULL)
#          endif /* ?TANDEM */
#        endif /* ?THEOS */
#      endif /* ?AMIGA */
#      ifdef CMS_MVS
    if (strncmp(t,"dd:",3) != 0 && strncmp(t,"DD:",3) != 0)
#      endif /* CMS_MVS */
#      ifdef THEOS
    /* insert .zip extension before disk name */
    if ((r = MBSRCHR(t, ':')) != NULL) {
        /* save disk name */
        if ((disk = strdup(r)) == NULL)
            return NULL;
        strcpy(r[-1] == '.' ? r - 1 : r, ".zip");
        strcat(t, disk);
        free(disk);
    } else {
        if (q != NULL && *q == '.')
          strcpy(q, ".zip");
        else
          strcat(t, ".zip");
    }
  }
#      else /* !THEOS */
#        ifdef TANDEM     /*  Tandem can't cope with extensions */
    strcat(t, " ZIP");
#        else /* !TANDEM */
    strcat(t, ".zip");
#        endif /* ?TANDEM */
#      endif /* ?THEOS */
#    else /* QDOS */
  q = LastDir(t);
  if(MBSRCHR(q, '_') == NULL && MBSRCHR(q, '.') == NULL)
  {
      strcat(t, "_zip");
  }
#    endif /* QDOS */
#  endif /* !RISCOS */
  return t;
}
#endif  /* ndef VMS */

/* ---------------------------------------------------- */

/* moved out of ZIP64_SUPPORT - 2/6/2005 EG */

/* 08/31/2003 R.Nausedat */

local void write_ushort_to_mem( OFT( ush) usValue,
                                OFT( char *)pPtr)
#ifdef NO_PROTO
  ush usValue;
  char *pPtr;
#endif /* def NO_PROTO */
{
  *pPtr++ = ((char)(usValue) & 0xff);
  *pPtr = ((char)(usValue >> 8) & 0xff);
}

local void write_ulong_to_mem(uValue, pPtr)
ulg uValue;
char *pPtr;
{
  write_ushort_to_mem((ush)(uValue & 0xffff), pPtr);
  write_ushort_to_mem((ush)((uValue >> 16) & 0xffff), pPtr + 2);
}

#ifdef ZIP64_SUPPORT

local void write_int64_to_mem(l64Value,pPtr)
  uzoff_t l64Value;
  char *pPtr;
{
  write_ulong_to_mem((ulg)(l64Value & 0xffffffff),pPtr);
  write_ulong_to_mem((ulg)((l64Value >> 32) & 0xffffffff),pPtr + 4);
}

#endif /* def ZIP64_SUPPORT */

#ifdef UNICODE_SUPPORT

/* Write a string to memory */
local void write_string_to_mem(strValue, pPtr)
  char *strValue;
  char *pPtr;
{
  if (strValue != NULL) {
    int ssize = strlen(strValue);
    int i;

    for (i = 0; i < ssize; i++) {
      *(pPtr + i) = *(strValue + i);
    }
  }
}

#endif /* def UNICODE_SUPPORT */



/* same as above but allocate memory as needed and keep track of current end
   using offset - 2/6/05 EG */

#if 0 /* ubyte version not used */
local void append_ubyte_to_mem( OFT( unsigned char) ubValue,
                                OFT( char **) pPtr,
                                OFT( extent *) offset,
                                OFT( extent *) blocksize)
#ifdef NO_PROTO
  unsigned char ubValue;  /* byte to append */
  char **pPtr;            /* start of block */
  extent *offset;         /* next byte to write */
  extent *blocksize;      /* current size of block */
#endif /* def NO_PROTO */
{
  if (*pPtr == NULL) {
    /* malloc a 1K block */
    (*blocksize) = 1024;
    *pPtr = (char *) malloc(*blocksize);
    if (*pPtr == NULL) {
      ziperr(ZE_MEM, "append_ubyte_to_mem");
    }
  }
  /* if (*offset) + 1 > (*blocksize) - 1 */
  else if ((*offset) > (*blocksize) - (1 + 1)) {
    /* realloc a bigger block in 1 K increments */
    (*blocksize) += 1024;
    *pPtr = realloc(*pPtr, *blocksize);
    if (*pPtr == NULL) {
      ziperr(ZE_MEM, "append_ubyte_to_mem");
    }
  }
  *(*pPtr + *offset) = ubValue;
  (*offset)++;
}
#endif

local void append_ushort_to_mem( OFT( ush) usValue,
                                 OFT( char **) pPtr,
                                 OFT( extent *) offset,
                                 OFT( extent *) blocksize)
#ifdef NO_PROTO
  ush usValue;
  char **pPtr;
  extent *offset;
  extent *blocksize;
#endif /* def NO_PROTO */
{
  if (*pPtr == NULL) {
    /* malloc a 1K block */
    (*blocksize) = 1024;
    *pPtr = (char *) malloc(*blocksize);
    if (*pPtr == NULL) {
      ziperr(ZE_MEM, "append_ushort_to_mem");
    }
  }
  /* if (*offset) + 2 > (*blocksize) - 1 */
  else if ((*offset) > (*blocksize) - (1 + 2)) {
    /* realloc a bigger block in 1 K increments */
    (*blocksize) += 1024;
    *pPtr = realloc(*pPtr, (extent)*blocksize);
    if (*pPtr == NULL) {
      ziperr(ZE_MEM, "append_ushort_to_mem");
    }
  }
  write_ushort_to_mem(usValue, (*pPtr) + (*offset));
  (*offset) += 2;
}

local void append_ulong_to_mem(uValue, pPtr, offset, blocksize)
  ulg uValue;
  char **pPtr;
  extent *offset;
  extent *blocksize;
{
  if (*pPtr == NULL) {
    /* malloc a 1K block */
    (*blocksize) = 1024;
    *pPtr = (char *) malloc(*blocksize);
    if (*pPtr == NULL) {
      ziperr(ZE_MEM, "append_ulong_to_mem");
    }
  }
  else if ((*offset) > (*blocksize) - (1 + 4)) {
    /* realloc a bigger block in 1 K increments */
    (*blocksize) += 1024;
    *pPtr = realloc(*pPtr, *blocksize);
    if (*pPtr == NULL) {
      ziperr(ZE_MEM, "append_ulong_to_mem");
    }
  }
  write_ulong_to_mem(uValue, (*pPtr) + (*offset));
  (*offset) += 4;
}

#ifdef ZIP64_SUPPORT

local void append_int64_to_mem(l64Value, pPtr, offset, blocksize)
  uzoff_t l64Value;
  char **pPtr;
  extent *offset;
  extent *blocksize;
{
  if (*pPtr == NULL) {
    /* malloc a 1K block */
    (*blocksize) = 1024;
    *pPtr = (char *) malloc(*blocksize);
    if (*pPtr == NULL) {
      ziperr(ZE_MEM, "append_int64_to_mem");
    }
  }
  else if ((*offset) > (*blocksize) - (1 + 8)) {
    /* realloc a bigger block in 1 K increments */
    (*blocksize) += 1024;
    *pPtr = realloc(*pPtr, *blocksize);
    if (*pPtr == NULL) {
      ziperr(ZE_MEM, "append_int64_to_mem");
    }
  }
  write_int64_to_mem(l64Value, (*pPtr) + (*offset));
  (*offset) += 8;
}

#endif /* def ZIP64_SUPPORT */

/* Append a string to the memory block. */
local void append_string_to_mem(strValue, strLength, pPtr, offset, blocksize)
  char *strValue;
  int  strLength;
  char **pPtr;
  extent *offset;
  extent *blocksize;
{
  if (strValue != NULL) {
    unsigned bsize = 1024;
    unsigned ssize = strLength;
    unsigned i;

    if (ssize > bsize) {
      bsize = ssize;
    }
    if (*pPtr == NULL) {
      /* malloc a 1K block */
      (*blocksize) = bsize;
      *pPtr = (char *) malloc(*blocksize);
      if (*pPtr == NULL) {
        ziperr(ZE_MEM, "append_string_to_mem");
      }
    }
    else if ((*offset) + ssize > (*blocksize) - 1) {
      /* realloc a bigger block in 1 K increments */
      (*blocksize) += bsize;
      *pPtr = realloc(*pPtr, *blocksize);
      if (*pPtr == NULL) {
        ziperr(ZE_MEM, "append_string_to_mem");
      }
    }
    for (i = 0; i < ssize; i++) {
      *(*pPtr + *offset + i) = *(strValue + i);
    }
    (*offset) += ssize;
  }
}

/* ---------------------------------------------------- */

/* zip64 support 08/31/2003 R.Nausedat */
/* moved out of zip64 support 10/22/05 */

/* Searches pExtra for extra field with specified tag.
 * If it finds one it returns a pointer to it, else NULL.
 * Renamed and made generic.  10/3/03
 */
char *get_extra_field( OFT( ush) tag,
                       OFT( char *) pExtra,
                       OFT( unsigned) iExtraLen)
#ifdef NO_PROTO
  ush tag;              /* tag to look for */
  char *pExtra;         /* pointer to extra field in memory */
  unsigned iExtraLen;   /* length of extra field */
#endif /* def NO_PROTO */
{
  char  *pTemp;
  ush   usBlockTag;
  ush   usBlockSize;

  if( pExtra == NULL )
    return NULL;

  for (pTemp = pExtra; pTemp < pExtra  + iExtraLen - ZIP_EF_HEADER_SIZE;)
  {
    usBlockTag = SH(pTemp);       /* get tag */
    usBlockSize = SH(pTemp + 2);  /* get field data size */
    if (usBlockTag == tag)
      return pTemp;
    pTemp += (usBlockSize + ZIP_EF_HEADER_SIZE);
  }
  return NULL;
}

/* copy_nondup_extra_fields
 *
 * Copy any extra fields in old that are not in new to new.
 * Returns the new extra fields block and newLen is new length.
 */
char *copy_nondup_extra_fields(oldExtra, oldExtraLen, newExtra, newExtraLen, newLen)
  char *oldExtra;       /* pointer to old extra fields */
  unsigned oldExtraLen; /* length of old extra fields */
  char *newExtra;       /* pointer to new extra fields */
  unsigned newExtraLen; /* length of new extra fields */
  unsigned *newLen;     /* length of new extra fields after copy */
{
  char *returnExtra = NULL;
  ush   returnExtraLen = 0;
  char *tempExtra;
  char *pTemp;
  ush   tag;
  ush   blocksize;

  if( oldExtra == NULL ) {
    /* no old extra fields so return copy of newExtra */
    if (newExtra == NULL || newExtraLen == 0) {
      *newLen = 0;
      return NULL;
    } else {
      if ((returnExtra = malloc(newExtraLen)) == NULL)
        ZIPERR(ZE_MEM, "extra field copy");
      memcpy(returnExtra, newExtra, newExtraLen);
      returnExtraLen = newExtraLen;
      *newLen = returnExtraLen;
      return returnExtra;
    }
  }

  /* allocate block large enough for all extra fields */
  if ((tempExtra = malloc(0xFFFF)) == NULL)
    ZIPERR(ZE_MEM, "extra field copy");

  /* look for each old extra field in new block */
  for (pTemp = oldExtra; pTemp < oldExtra  + oldExtraLen;)
  {
    tag = SH(pTemp);            /* get tag */
    blocksize = SH(pTemp + 2);  /* get field data size */
    if (get_extra_field(tag, newExtra, newExtraLen) == NULL) {
      /* tag not in new block so add it */
      memcpy(tempExtra + returnExtraLen, pTemp, blocksize + 4);
      returnExtraLen += blocksize + 4;
    }
    pTemp += blocksize + 4;
  }

  /* copy all extra fields from new block */
  memcpy(tempExtra + returnExtraLen, newExtra, newExtraLen);
  returnExtraLen += newExtraLen;

  /* copy tempExtra to returnExtra */
  if ((returnExtra = malloc(returnExtraLen)) == NULL)
    ZIPERR(ZE_MEM, "extra field copy");
  memcpy(returnExtra, tempExtra, returnExtraLen);
  free(tempExtra);

  *newLen = returnExtraLen;
  return returnExtra;
}

#ifdef UNICODE_SUPPORT

/* The latest format is
     1 byte     Version of Unicode Path Extra Field
     4 bytes    Name Field CRC32 Checksum
     variable   UTF-8 Version Of Name
 */

local void read_Unicode_Path_entry(pZipListEntry)
  struct zlist far *pZipListEntry;
{
  char *pTemp;
  char *UPath;
  char *iname;
  ush ELen;
  uch Version;
  ush ULen;
  ulg chksum = CRCVAL_INITIAL;
  ulg iname_chksum;

  /* check if we have a Unicode Path extra field ... */
  pTemp = get_extra_field( UTF8_PATH_EF_TAG, pZipListEntry->cextra, pZipListEntry->cext );
  pZipListEntry->uname = NULL;
  if( pTemp == NULL ) {
    return;
  }

  /* ... if so, update corresponding entries in struct zlist */

  pTemp += 2;

  /* length of this extra field */
  ELen = SH(pTemp);
  pTemp += 2;

  /* version */
  Version = (uch) *pTemp;
  pTemp += 1;
  if (Version > 1) {
    zipwarn("Unicode Path Extra Field version > 1 - skipping", pZipListEntry->oname);
    return;
  }

  /* iname CRC */
  iname_chksum = LG(pTemp);
  pTemp += 4;

  /*
   * Compute the CRC-32 checksum of iname
   */
/*
  crc_16 = crc16f((uch *)(pZipListEntry->iname), strlen(pZipListEntry->iname));
 */

  if ((iname = malloc(strlen(pZipListEntry->iname) + 1)) == NULL) {
    ZIPERR(ZE_MEM, "write Unicode");
  }
  strcpy(iname, pZipListEntry->iname);

  chksum = crc32(chksum, (uch *)(iname), strlen(iname));

  free(iname);

/*  chksum = adler16(ADLERVAL_INITIAL,
    (uch *)(pZipListEntry->iname), strlen(pZipListEntry->iname));
*/

  /* If the checksums's don't match then likely iname has been modified and
   * the Unicode Path is no longer valid
   */
  if (chksum != iname_chksum) {
    printf("unicode_mismatch = %d\n", unicode_mismatch);
    if (unicode_mismatch == 1) {
      /* warn and continue */
      zipwarn("Unicode does not match path - ignoring Unicode: ", pZipListEntry->oname);
    } else if (unicode_mismatch == 2) {
      /* ignore and continue */
    } else if (unicode_mismatch == 0) {
      /* error */
      sprintf(errbuf, "Unicode does not match path:  %s\n", pZipListEntry->oname);
      strcat(errbuf,
        "                     Likely entry name changed but Unicode not updated\n");
      strcat(errbuf,
        "                     Use -UN=i to ignore errors or n for no Unicode paths");
      zipwarn(errbuf, "");
      ZIPERR(ZE_FORM, "Unicode path error");
    }
    return;
  }

  ULen = ELen - 5;

  /* UTF-8 Path */
  if (ULen == 0) {
    /* standard path is UTF-8 so use that */
    ULen = pZipListEntry->nam;
    if ((UPath = malloc(ULen + 1)) == NULL) {
      return;
    }
    strcpy(UPath, pZipListEntry->name);
  } else {
    /* use Unicode path */
    if ((UPath = malloc(ULen + 1)) == NULL) {
      return;
    }
    strncpy(UPath, pTemp, ULen);
    UPath[ULen] = '\0';
  }
  pZipListEntry->uname = UPath;
  return;
}

local void read_Unicode_Path_local_entry(pZipListEntry)
  struct zlist far *pZipListEntry;
{
  char *pTemp;
  char *UPath;
  char *iname;
  ush ELen;
  uch Version;
  ush ULen;
  ulg chksum = CRCVAL_INITIAL;
  ulg iname_chksum;

  /* check if we have a Unicode Path extra field ... */
  pTemp = get_extra_field( UTF8_PATH_EF_TAG, pZipListEntry->extra, pZipListEntry->ext );
  pZipListEntry->uname = NULL;
  if( pTemp == NULL ) {
    return;
  }

  /* ... if so, update corresponding entries in struct zlist */

  pTemp += 2;

  /* length of this extra field */
  ELen = SH(pTemp);
  pTemp += 2;

  /* version */
  Version = (uch) *pTemp;
  pTemp += 1;
  if (Version > 1) {
    zipwarn("Unicode Path Extra Field version > 1 - skipping", pZipListEntry->oname);
    return;
  }

  /* iname CRC */
  iname_chksum = LG(pTemp);
  pTemp += 4;

  /*
   * Compute 32-bit crc of iname and AND halves to make 16-bit version
   */
  /*
  chksum = adler16(ADLERVAL_INITIAL,
    (uch *)(pZipListEntry->iname), strlen(pZipListEntry->iname));
  */

  if ((iname = malloc(strlen(pZipListEntry->iname) + 1)) == NULL) {
    ZIPERR(ZE_MEM, "write Unicode");
  }
  strcpy(iname, pZipListEntry->iname);

  chksum = crc32(chksum, (uch *)(iname), strlen(iname));

  free(iname);

  /* If the checksums's don't match then likely iname has been modified and
   * the Unicode Path is no longer valid
   */
  if (chksum != iname_chksum) {
    if (unicode_mismatch == 1) {
      /* warn and continue */
      zipwarn("Unicode does not match path - ignoring Unicode: ", pZipListEntry->oname);
    } else if (unicode_mismatch == 2) {
      /* ignore and continue */
    } else if (unicode_mismatch == 0) {
      /* error */
      sprintf(errbuf, "Unicode does not match path:  %s\n", pZipListEntry->oname);
      strcat(errbuf,
        "                     Likely entry name changed but Unicode not updated\n");
      strcat(errbuf,
        "                     Use -UN=i to ignore errors or n for no Unicode paths");
      zipwarn(errbuf, "");
      ZIPERR(ZE_FORM, "Unicode path error");
    }
    return;
  }

  ULen = ELen - 5;

  /* UTF-8 Path */
  if (ULen == 0) {
    /* standard path is UTF-8 so use that */
    ULen = pZipListEntry->nam;
    if ((UPath = malloc(ULen + 1)) == NULL) {
      return;
    }
    strcpy(UPath, pZipListEntry->name);
  } else {
    /* use Unicode path */
    if ((UPath = malloc(ULen + 1)) == NULL) {
      return;
    }
    strncpy(UPath, pTemp, ULen);
    UPath[ULen] = '\0';
  }
  pZipListEntry->uname = UPath;
  return;
}

#endif /* def UNICODE_SUPPORT */

#ifdef ZIP64_SUPPORT           /* zip64 support 08/31/2003 R.Nausedat */

/* searches the cextra member of zlist for a zip64 extra field. if it finds one it  */
/* updates the len, siz and off members of zlist with the corresponding values of   */
/* the zip64 extra field, that is if either the len, siz or off member of zlist is  */
/* set to its max value we have to use the corresponding value from the zip64 extra */
/* field. as of now the dsk member of zlist is not much of interest since we should */
/* not modify multi volume archives at all.                                         */
local void adjust_zip_central_entry(pZipListEntry)
  struct zlist far *pZipListEntry;
{
  char  *pTemp;

  /* assume not using zip64 fields */
  zip64_entry = 0;

  /* check if we have a "large file" Zip64 extra field ... */
  pTemp = get_extra_field( ZIP64_EF_TAG, pZipListEntry->cextra, pZipListEntry->cext );
  if( pTemp == NULL )
    return;

  /* using zip64 field */
  zip64_entry = 1;
  pTemp += ZIP_EF_HEADER_SIZE;

  /* ... if so, update corresponding entries in struct zlist */
  if (pZipListEntry->len == ZIP_UWORD32_MAX)
  {
    pZipListEntry->len = LLG(pTemp);
    pTemp += 8;
  }

  if (pZipListEntry->siz == ZIP_UWORD32_MAX)
  {
    pZipListEntry->siz = LLG(pTemp);
    pTemp += 8;
  }

  if (pZipListEntry->off == ZIP_UWORD32_MAX)
  {
    pZipListEntry->off = LLG(pTemp);
    pTemp += 8;
  }

  if (pZipListEntry->dsk == ZIP_UWORD16_MAX)
  {
    pZipListEntry->dsk = LG(pTemp);
  }

}


/* adjust_zip_local_entry
 *
 * Return 1 if there is a Zip64 extra field and 0 if not
 */
local int adjust_zip_local_entry(pZipListEntry)
  struct zlist far *pZipListEntry;
{
  char  *pTemp;

  /* assume not using zip64 fields */
  zip64_entry = 0;

  /* check if we have a "large file" Zip64 extra field ... */
  pTemp = get_extra_field(ZIP64_EF_TAG, pZipListEntry->extra, pZipListEntry->ext );
  if( pTemp == NULL )
    return zip64_entry;

  /* using zip64 field */
  zip64_entry = 1;
  pTemp += ZIP_EF_HEADER_SIZE;

  /* ... if so, update corresponding entries in struct zlist */
  if (pZipListEntry->len == ZIP_UWORD32_MAX)
  {
    pZipListEntry->len = LLG(pTemp);
    pTemp += 8;
  }

  if (pZipListEntry->siz == ZIP_UWORD32_MAX)
  {
    pZipListEntry->siz = LLG(pTemp);
    pTemp += 8;
  }
  return zip64_entry;
}

/* adds a zip64 extra field to the data the cextra member of zlist points to. If
 * there is already a zip64 extra field present delete it first.
 */
local int add_central_zip64_extra_field(pZipListEntry)
  struct zlist far *pZipListEntry;
{
  char   *pExtraFieldPtr;
  char   *pTemp;
  ush    usTemp;
  ush    efsize = 0;
  ush    esize;
  ush    oldefsize;
  extent len;
  int    used_zip64 = 0;

  /* get length of ef based on which fields exceed limits */
  /* AppNote says:
   *      The order of the fields in the ZIP64 extended
   *      information record is fixed, but the fields will
   *      only appear if the corresponding Local or Central
   *      directory record field is set to 0xFFFF or 0xFFFFFFFF.
   */
  efsize = ZIP_EF_HEADER_SIZE;             /* type + size */
  if (pZipListEntry->len > ZIP_UWORD32_MAX || force_zip64 == 1) {
    /* compressed size */
    efsize += 8;
    used_zip64 = 1;
  }
  if (pZipListEntry->siz > ZIP_UWORD32_MAX) {
    /* uncompressed size */
    efsize += 8;
    used_zip64 = 1;
  }
  if (pZipListEntry->off > ZIP_UWORD32_MAX) {
    /* offset */
    efsize += 8;
    used_zip64 = 1;
  }
  if (pZipListEntry->dsk > ZIP_UWORD16_MAX) {
    /* disk number */
    efsize += 4;
    used_zip64 = 1;
  }

  if (used_zip64 && force_zip64 == 0) {
    zipwarn("Large entry support disabled using -fz- but needed", "");
    return ZE_BIG;
  }

  /* malloc zip64 extra field? */
  if( pZipListEntry->cextra == NULL )
  {
    if (efsize == ZIP_EF_HEADER_SIZE) {
      return ZE_OK;
    }
    if ((pExtraFieldPtr = pZipListEntry->cextra = (char *) malloc(efsize)) == NULL) {
      return ZE_MEM;
    }
    pZipListEntry->cext = efsize;
  }
  else
  {
    /* check if we have a "large file" extra field ... */
    pExtraFieldPtr = get_extra_field(ZIP64_EF_TAG, pZipListEntry->cextra, pZipListEntry->cext);
    if( pExtraFieldPtr == NULL )
    {
      /* ... we don't, so re-malloc enough memory for the old extra data plus
       * the size of the zip64 extra field
       */
      if ((pExtraFieldPtr = (char *) malloc(efsize + pZipListEntry->cext)) == NULL) {
        return ZE_MEM;
      }
      /* move the old extra field */
      memmove(pExtraFieldPtr, pZipListEntry->cextra, pZipListEntry->cext);
      free(pZipListEntry->cextra);
      pZipListEntry->cextra = pExtraFieldPtr;
      pExtraFieldPtr += pZipListEntry->cext;
      pZipListEntry->cext += efsize;
    }
    else
    {
      /* ... we have. sort out the existing zip64 extra field and remove it from
       * pZipListEntry->cextra, re-malloc enough memory for the old extra data
       * left plus the size of the zip64 extra field
       */
      usTemp = SH(pExtraFieldPtr + 2);
      /* if pZipListEntry->cextra == pExtraFieldPtr and pZipListEntry->cext == usTemp + efsize
       * we should have only one extra field, and this is a zip64 extra field. as some
       * zip tools seem to require fixed zip64 extra fields we have to check if
       * usTemp + ZIP_EF_HEADER_SIZE is equal to ZIP64_LARGE_FILE_HEAD_SIZE. if it
       * isn't, we free the old extra field and allocate memory for a new one
       */
      if( pZipListEntry->cext == (extent)(usTemp + ZIP_EF_HEADER_SIZE) )
      {
        /* just Zip64 extra field in extra field */
        if( pZipListEntry->cext != efsize )
        {
          /* wrong size */
          if ((pExtraFieldPtr = (char *) malloc(efsize)) == NULL) {
            return ZE_MEM;
          }
          free(pZipListEntry->cextra);
          pZipListEntry->cextra = pExtraFieldPtr;
          pZipListEntry->cext = efsize;
        }
      }
      else
      {
        /* get the old Zip64 extra field out and add new */
        oldefsize = usTemp + ZIP_EF_HEADER_SIZE;
        if ((pTemp = (char *) malloc(pZipListEntry->cext - oldefsize + efsize)) == NULL) {
          return ZE_MEM;
        }
        len = (extent)(pExtraFieldPtr - pZipListEntry->cextra);
        memcpy(pTemp, pZipListEntry->cextra, len);
        memcpy(pTemp + len, pExtraFieldPtr + oldefsize,
          pZipListEntry->cext - oldefsize - len);
        pZipListEntry->cext -= oldefsize;
        pExtraFieldPtr = pTemp + pZipListEntry->cext;
        pZipListEntry->cext += efsize;
        free(pZipListEntry->cextra);
        pZipListEntry->cextra = pTemp;
      }
    }
  }

  /* set zip64 extra field members */
  write_ushort_to_mem(ZIP64_EF_TAG, pExtraFieldPtr);
  write_ushort_to_mem((ush) (efsize - ZIP_EF_HEADER_SIZE), pExtraFieldPtr + 2);
  esize = ZIP_EF_HEADER_SIZE;
  if (pZipListEntry->len > ZIP_UWORD32_MAX || force_zip64 == 1) {
    write_int64_to_mem(pZipListEntry->len, pExtraFieldPtr + esize);
    esize += 8;
  }
  if (pZipListEntry->siz > ZIP_UWORD32_MAX) {
    write_int64_to_mem(pZipListEntry->siz, pExtraFieldPtr + esize);
    esize += 8;
  }
  if (pZipListEntry->off > ZIP_UWORD32_MAX) {
    write_int64_to_mem(pZipListEntry->off, pExtraFieldPtr + esize);
    esize += 8;
  }
  if (pZipListEntry->dsk > ZIP_UWORD16_MAX) {
    write_ulong_to_mem(pZipListEntry->dsk, pExtraFieldPtr + esize);
  }

  /* un' wech */
  return ZE_OK;
}

#if 0
/* Remove extra field in local extra field
 * Return 1 if found, else 0
 * 12/28/05
 */
local int remove_local_extra_field(pZEntry, tag)
  struct zlist far *pZEntry;
  ulg tag;
{
  char  *pExtra;
  char  *pOldExtra;
  char  *pOldTemp;
  char  *pTemp;
  ush   newEFSize;
  ush   usTemp;
  ush   blocksize;

  /* check if we have the extra field ... */
  pOldExtra = get_extra_field( (ush)tag, pZEntry->extra, pZEntry->ext );
  if (pOldExtra)
  {
    /* We have. Get rid of it. */
    blocksize = SH( pOldExtra + 2 );
    newEFSize = pZEntry->ext - blocksize;
    pExtra = (char *) malloc( newEFSize );
    if( pExtra == NULL )
      ziperr(ZE_MEM, "Remove Local Extra Field");
    /* move all before EF */
    usTemp = (extent) (pOldExtra - pZEntry->extra);
    pTemp = pExtra;
    memcpy( pTemp, pZEntry->extra, usTemp );
    /* move all after old Zip64 EF */
    pTemp = pExtra + usTemp;
    pOldTemp = pOldExtra + blocksize;
    usTemp = pZEntry->ext - usTemp - blocksize;
    memcpy( pTemp, pOldTemp, usTemp);
    /* replace extra fields */
    pZEntry->ext = newEFSize;
    free(pZEntry->extra);
    pZEntry->extra = pExtra;
    return 1;
  } else {
    return 0;
  }
}

/* Remove extra field in central extra field
 * Return 1 if found, else 0
 * 12/28/05
 */
local int remove_central_extra_field(pZEntry, tag)
  struct zlist far *pZEntry;
  ulg tag;
{
  char  *pExtra;
  char  *pOldExtra;
  char  *pOldTemp;
  char  *pTemp;
  ush   newEFSize;
  ush   usTemp;
  ush   blocksize;

  /* check if we have the extra field ... */
  pOldExtra = get_extra_field( (ush)tag, pZEntry->cextra, pZEntry->cext );
  if (pOldExtra)
  {
    /* We have. Get rid of it. */
    blocksize = SH( pOldExtra + 2 );
    newEFSize = pZEntry->cext - blocksize;
    pExtra = (char *) malloc( newEFSize );
    if( pExtra == NULL )
      ziperr(ZE_MEM, "Remove Local Extra Field");
    /* move all before EF */
    usTemp = (extent) (pOldExtra - pZEntry->cextra);
    pTemp = pExtra;
    memcpy( pTemp, pZEntry->cextra, usTemp );
    /* move all after old Zip64 EF */
    pTemp = pExtra + usTemp;
    pOldTemp = pOldExtra + blocksize;
    usTemp = pZEntry->cext - usTemp - blocksize;
    memcpy( pTemp, pOldTemp, usTemp);
    /* replace extra fields */
    pZEntry->cext = newEFSize;
    free(pZEntry->cextra);
    pZEntry->cextra = pExtra;
    return 1;
  } else {
    return 0;
  }
}
#endif

/* Add Zip64 extra field to local header
 * 10/5/03 EG
 */
local int add_local_zip64_extra_field(pZEntry)
  struct zlist far *pZEntry;
{
  char  *pZ64Extra;
  char  *pOldZ64Extra;
  char  *pOldTemp;
  char  *pTemp;
  ush   newEFSize;
  ush   usTemp;
  ush   blocksize;
  ush   Z64LocalLen = ZIP_EF_HEADER_SIZE +  /* tag + EF Data Len */
                      8 +                   /* original uncompressed length of file */
                      8;                    /* compressed size of file */

  /* malloc zip64 extra field? */
  /* after the below pZ64Extra should point to start of Zip64 extra field */
  if (pZEntry->ext == 0 || pZEntry->extra == NULL)
  {
    /* get new extra field */
    pZ64Extra = pZEntry->extra = (char *) malloc(Z64LocalLen);
    if (pZEntry->extra == NULL) {
      ziperr( ZE_MEM, "Zip64 local extra field" );
    }
    pZEntry->ext = Z64LocalLen;
  }
  else
  {
    /* check if we have a Zip64 extra field ... */
    pOldZ64Extra = get_extra_field( ZIP64_EF_TAG, pZEntry->extra, pZEntry->ext );
    if (pOldZ64Extra == NULL)
    {
      /* ... we don't, so re-malloc enough memory for the old extra data plus */
      /* the size of the zip64 extra field */
      pZ64Extra = (char *) malloc( Z64LocalLen + pZEntry->ext );
      if (pZ64Extra == NULL)
        ziperr( ZE_MEM, "Zip64 Extra Field" );
      /* move old extra field and update pointer and length */
      memmove( pZ64Extra, pZEntry->extra, pZEntry->ext);
      free( pZEntry->extra );
      pZEntry->extra = pZ64Extra;
      pZ64Extra += pZEntry->ext;
      pZEntry->ext += Z64LocalLen;
    }
    else
    {
      /* ... we have. Sort out the existing zip64 extra field and remove it
       * from pZEntry->extra, re-malloc enough memory for the old extra data
       * left plus the size of the zip64 extra field */
      blocksize = SH( pOldZ64Extra + 2 );
      /* If the right length then go with it, else get rid of it and add a new extra field
       * to existing block. */
      if (blocksize == Z64LocalLen - ZIP_EF_HEADER_SIZE)
      {
        /* looks good */
        pZ64Extra = pOldZ64Extra;
      }
      else
      {
        newEFSize = pZEntry->ext - (blocksize + ZIP_EF_HEADER_SIZE) + Z64LocalLen;
        pZ64Extra = (char *) malloc( newEFSize );
        if( pZ64Extra == NULL )
          ziperr(ZE_MEM, "Zip64 Extra Field");
        /* move all before Zip64 EF */
        usTemp = (extent) (pOldZ64Extra - pZEntry->extra);
        pTemp = pZ64Extra;
        memcpy( pTemp, pZEntry->extra, usTemp );
        /* move all after old Zip64 EF */
        pTemp = pZ64Extra + usTemp;
        pOldTemp = pOldZ64Extra + ZIP_EF_HEADER_SIZE + blocksize;
        usTemp = pZEntry->ext - usTemp - blocksize;
        memcpy( pTemp, pOldTemp, usTemp);
        /* replace extra fields */
        pZEntry->ext = newEFSize;
        free(pZEntry->extra);
        pZEntry->extra = pZ64Extra;
        pZ64Extra = pTemp + usTemp;
      }
    }
  }
  /* set/update zip64 extra field members */
  write_ushort_to_mem(ZIP64_EF_TAG, pZ64Extra);
  write_ushort_to_mem((ush) (Z64LocalLen - ZIP_EF_HEADER_SIZE), pZ64Extra + 2);
  write_int64_to_mem(pZEntry->len, pZ64Extra + 2 + 2);
  write_int64_to_mem(pZEntry->siz, pZ64Extra + 2 + 2 + 8);

  return ZE_OK;
}

# endif /* ZIP64_SUPPORT */

#ifdef UNICODE_SUPPORT
/* Add UTF-8 path extra field
 * 10/11/05
 */
local int add_Unicode_Path_local_extra_field(pZEntry)
  struct zlist far *pZEntry;
{
  char  *pUExtra;
  char  *pOldUExtra;
  char  *pOldTemp;
  char  *pTemp;
#ifdef WIN32_OEM
  char  *inameLocal;
#endif
  ush   newEFSize;
  ush   usTemp;
  ush   ULen = strlen(pZEntry->uname);
  ush   blocksize;
  ulg   chksum = CRCVAL_INITIAL;
  ush   ULocalLen = ZIP_EF_HEADER_SIZE +  /* tag + EF Data Len */
                    1 +                   /* version */
                    4 +                   /* iname chksum */
                    ULen;                 /* UTF-8 path */

  /* malloc Unicode Path extra field? */
  /* after the below pUExtra should point to start of Unicode Path extra field */
  if (pZEntry->ext == 0 || pZEntry->extra == NULL)
  {
    /* get new extra field */
    pUExtra = pZEntry->extra = (char *) malloc(ULocalLen);
    if (pZEntry->extra == NULL) {
      ziperr( ZE_MEM, "UTF-8 Path local extra field" );
    }
    pZEntry->ext = ULocalLen;
  }
  else
  {
    /* check if we have a Unicode Path extra field ... */
    pOldUExtra = get_extra_field( UTF8_PATH_EF_TAG, pZEntry->extra, pZEntry->ext );
    if (pOldUExtra == NULL)
    {
      /* ... we don't, so re-malloc enough memory for the old extra data plus */
      /* the size of the UTF-8 Path extra field */
      pUExtra = (char *) malloc( ULocalLen + pZEntry->ext );
      if (pUExtra == NULL)
        ziperr( ZE_MEM, "UTF-8 Path Extra Field" );
      /* move old extra field and update pointer and length */
      memmove( pUExtra, pZEntry->extra, pZEntry->ext);
      free( pZEntry->extra );
      pZEntry->extra = pUExtra;
      pUExtra += pZEntry->ext;
      pZEntry->ext += ULocalLen;
    }
    else
    {
      /* ... we have. Sort out the existing UTF-8 Path extra field and remove it
       * from pZEntry->extra, re-malloc enough memory for the old extra data
       * left plus the size of the UTF-8 Path extra field */
      blocksize = SH( pOldUExtra + 2 );
      /* If the right length then go with it, else get rid of it and add a new extra field
       * to existing block. */
      if (blocksize == ULocalLen - ZIP_EF_HEADER_SIZE)
      {
        /* looks good */
        pUExtra = pOldUExtra;
      }
      else
      {
        newEFSize = pZEntry->ext - (blocksize + ZIP_EF_HEADER_SIZE) + ULocalLen;
        pUExtra = (char *) malloc( newEFSize );
        if( pUExtra == NULL )
          ziperr(ZE_MEM, "UTF-8 Path Extra Field");
        /* move all before UTF-8 Path EF */
        usTemp = (extent) (pOldUExtra - pZEntry->extra);
        pTemp = pUExtra;
        memcpy( pTemp, pZEntry->extra, usTemp );
        /* move all after old UTF-8 Path EF */
        pTemp = pUExtra + usTemp;
        pOldTemp = pOldUExtra + ZIP_EF_HEADER_SIZE + blocksize;
        usTemp = pZEntry->ext - usTemp - blocksize;
        memcpy( pTemp, pOldTemp, usTemp);
        /* replace extra fields */
        pZEntry->ext = newEFSize;
        free(pZEntry->extra);
        pZEntry->extra = pUExtra;
        pUExtra = pTemp + usTemp;
      }
    }
  }

  /*
   * Compute the Adler-16 checksum of iname
   */
/*
  chksum = adler16(ADLERVAL_INITIAL,
                   (uch *)(pZEntry->iname), strlen(pZEntry->iname));
*/

#ifdef WIN32_OEM
  if ((inameLocal = malloc(strlen(pZEntry->iname) + 1)) == NULL) {
    ZIPERR(ZE_MEM, "write Unicode");
  }
  /* if oem translation done convert back for checksum */
  if ((pZEntry->vem & 0xff00) == 0) {
    /* get original */
    INTERN_TO_OEM(pZEntry->iname, inameLocal);
  } else {
    strcpy(inameLocal, pZEntry->iname);
  }
#else
# define inameLocal (pZEntry->iname)
#endif

  chksum = crc32(chksum, (uch *)(inameLocal), strlen(inameLocal));

#ifdef WIN32_OEM
  free(inameLocal);
#else
# undef inameLocal
#endif

  /* set/update UTF-8 Path extra field members */
  /* tag header */
  write_ushort_to_mem(UTF8_PATH_EF_TAG, pUExtra);
  /* data size */
  write_ushort_to_mem((ush) (ULocalLen - ZIP_EF_HEADER_SIZE), pUExtra + 2);
  /* version */
  *(pUExtra + 2 + 2) = 1;
  /* iname chksum */
  write_ulong_to_mem(chksum, pUExtra + 2 + 2 + 1);
  /* UTF-8 path */
  write_string_to_mem(pZEntry->uname, pUExtra + 2 + 2 + 1 + 4);

  return ZE_OK;
}

local int add_Unicode_Path_cen_extra_field(pZEntry)
  struct zlist far *pZEntry;
{
  char  *pUExtra;
  char  *pOldUExtra;
  char  *pOldTemp;
  char  *pTemp;
#ifdef WIN32_OEM
  char  *inameLocal;
#endif
  ush   newEFSize;
  ush   usTemp;
  ush   ULen = strlen(pZEntry->uname);
  ush   blocksize;
  ulg   chksum = CRCVAL_INITIAL;
  ush   UCenLen = ZIP_EF_HEADER_SIZE +  /* tag + EF Data Len */
                  1 +                   /* version */
                  4 +                   /* checksum */
                  ULen;                 /* UTF-8 path */

  /* malloc Unicode Path extra field? */
  /* after the below pUExtra should point to start of Unicode Path extra field */
  if (pZEntry->cext == 0 || pZEntry->cextra == NULL)
  {
    /* get new extra field */
    pUExtra = pZEntry->cextra = (char *) malloc(UCenLen);
    if (pZEntry->cextra == NULL) {
      ziperr( ZE_MEM, "UTF-8 Path cen extra field" );
    }
    pZEntry->cext = UCenLen;
  }
  else
  {
    /* check if we have a Unicode Path extra field ... */
    pOldUExtra = get_extra_field( UTF8_PATH_EF_TAG, pZEntry->cextra, pZEntry->cext );
    if (pOldUExtra == NULL)
    {
      /* ... we don't, so re-malloc enough memory for the old extra data plus */
      /* the size of the UTF-8 Path extra field */
      pUExtra = (char *) malloc( UCenLen + pZEntry->cext );
      if (pUExtra == NULL)
        ziperr( ZE_MEM, "UTF-8 Path Extra Field" );
      /* move old extra field and update pointer and length */
      memmove( pUExtra, pZEntry->cextra, pZEntry->cext);
      free( pZEntry->cextra );
      pZEntry->cextra = pUExtra;
      pUExtra += pZEntry->cext;
      pZEntry->cext += UCenLen;
    }
    else
    {
      /* ... we have. Sort out the existing UTF-8 Path extra field and remove it
       * from pZEntry->extra, re-malloc enough memory for the old extra data
       * left plus the size of the UTF-8 Path extra field */
      blocksize = SH( pOldUExtra + 2 );
      /* If the right length then go with it, else get rid of it and add a new extra field
       * to existing block. */
      if (blocksize == UCenLen - ZIP_EF_HEADER_SIZE)
      {
        /* looks good */
        pUExtra = pOldUExtra;
      }
      else
      {
        newEFSize = pZEntry->cext - (blocksize + ZIP_EF_HEADER_SIZE) + UCenLen;
        pUExtra = (char *) malloc( newEFSize );
        if( pUExtra == NULL )
          ziperr(ZE_MEM, "UTF-8 Path Extra Field");
        /* move all before UTF-8 Path EF */
        usTemp = (extent) (pOldUExtra - pZEntry->cextra);
        pTemp = pUExtra;
        memcpy( pTemp, pZEntry->cextra, usTemp );
        /* move all after old UTF-8 Path EF */
        pTemp = pUExtra + usTemp;
        pOldTemp = pOldUExtra + ZIP_EF_HEADER_SIZE + blocksize;
        usTemp = pZEntry->cext - usTemp - blocksize;
        memcpy( pTemp, pOldTemp, usTemp);
        /* replace extra fields */
        pZEntry->cext = newEFSize;
        free(pZEntry->cextra);
        pZEntry->cextra = pUExtra;
        pUExtra = pTemp + usTemp;
      }
    }
  }

  /*
   * Compute the CRC-32 checksum of iname
   */
#ifdef WIN32_OEM
  if ((inameLocal = malloc(strlen(pZEntry->iname) + 1)) == NULL) {
    ZIPERR(ZE_MEM, "write Unicode");
  }
  /* if oem translation done convert back for checksum */
  if ((pZEntry->vem & 0xff00) == 0) {
    /* get original */
    INTERN_TO_OEM(pZEntry->iname, inameLocal);
  } else {
    strcpy(inameLocal, pZEntry->iname);
  }
#else
# define inameLocal (pZEntry->iname)
#endif

  chksum = crc32(chksum, (uch *)(inameLocal), strlen(inameLocal));

#ifdef WIN32_OEM
  free(inameLocal);
#else
# undef inameLocal
#endif

  /*
   * Compute the Adler-16 checksum of iname
   */
/*
  chksum = adler16(ADLERVAL_INITIAL,
                   (uch *)(pZEntry->iname), strlen(pZEntry->iname));
*/

  /* set/update UTF-8 Path extra field members */
  /* tag header */
  write_ushort_to_mem(UTF8_PATH_EF_TAG, pUExtra);
  /* data size */
  write_ushort_to_mem((ush) (UCenLen - ZIP_EF_HEADER_SIZE), pUExtra + 2);
  /* version */
  *(pUExtra + 2 + 2) = 1;
  /* iname checksum */
  write_ulong_to_mem(chksum, pUExtra + 2 + 2 + 1);
  /* UTF-8 path */
  write_string_to_mem(pZEntry->uname, pUExtra + 2 + 2 + 1 + 4);

  return ZE_OK;
}
#endif /* def UNICODE_SUPPORT */


zoff_t ffile_size OF((FILE *));


/* 2004-12-06 SMS.
 * ffile_size() returns reliable file size or EOF.
 * May be used to detect large files in a small-file program.
 */
zoff_t ffile_size( file)
FILE *file;
{
  int sts;
  size_t siz;
  zoff_t ofs;
  char waste[ 4];

  /* Seek to actual EOF. */
  sts = zfseeko( file, 0, SEEK_END);
  if (sts != 0)
  {
    /* fseeko() failed.  (Unlikely.) */
    ofs = EOF;
  }
  else
  {
    /* Get apparent offset at EOF. */
    ofs = zftello( file);
    if (ofs < 0)
    {
      /* Offset negative (overflow).  File too big. */
      ofs = EOF;
    }
    else
    {
      /* Seek to apparent EOF offset.
         Won't be at actual EOF if offset was truncated.
      */
      sts = zfseeko( file, ofs, SEEK_SET);
      if (sts != 0)
      {
        /* fseeko() failed.  (Unlikely.) */
        ofs = EOF;
      }
      else
      {
        /* Read a byte at apparent EOF.  Should set EOF flag. */
        siz = fread( waste, 1, 1, file);
        if (feof( file) == 0)
        {
          /* Not at EOF, but should be.  File too big. */
          ofs = EOF;
        }
      }
    }
  }
  /* Seek to BOF.
   *
   * 2007-05-23 SMS.
   * Note that a problem in a prehistoric VAX C run-time library
   * requires that rewind() be used instead of fseek(), or else
   * the EOF flag is not cleared properly.
   */
  /* As WIN32 has this same problem (EOF not being cleared) when
   * NO_ZIP64_SUPPORT is set but LARGE_FILE_SUPPORT is set on a
   * small file, seems no reason not to always use rewind().
   * 8/5/07 EG
   */
#if 0
#ifdef VAXC
  sts = rewind( file);
#else /* def VAXC */
  sts = zfseeko( file, 0, SEEK_SET);
#endif /* def VAXC [else] */
#endif
  rewind(file);

  return ofs;
}


#ifndef UTIL

local void zipoddities(z)
struct zlist far *z;
{
    if ((z->vem >> 8) >= NUM_HOSTS)
    {
        sprintf(errbuf, "made by version %d.%d on system type %d: ",
                (ush)(z->vem & 0xff) / (ush)10, (ush)(z->vem & 0xff) % (ush)10,
                z->vem >> 8);
        zipwarn(errbuf, z->oname);
    }
    if (z->ver != 10 && z->ver != 11 && z->ver != 20)
    {
        sprintf(errbuf, "needs unzip %d.%d on system type %d: ",
                (ush)(z->ver & 0xff) / (ush)10,
                (ush)(z->ver & 0xff) % (ush)10, z->ver >> 8);
        zipwarn(errbuf, z->oname);
    }

    if ((fix == 2) && (z->flg != z->lflg))
    /* The comparision between central and local version of the
       "general purpose bit flag" cannot be used from scanzipf_regnew(),
       because in the "regular" zipfile processing, the local header reads
       have been postponed until the actual entry processing takes place.
       They have not yet been read when "zipoddities()" is called.
       This change was neccessary to support multivolume archives.
     */
    {
        sprintf(errbuf, "local flags = 0x%04x, central = 0x%04x: ",
                z->lflg, z->flg);
        zipwarn(errbuf, z->oname);
    }
    else if (z->flg & ~0xf && (z->flg & ~0xf0) != UTF8_BIT)
    /* Only bit in high byte we support is the new UTF-8 bit */
    {
        sprintf(errbuf, "undefined bits used in flags = 0x%04x: ", z->flg);
        zipwarn(errbuf, z->oname);
    }
    if (z->how > LAST_KNOWN_COMPMETHOD)    {
        sprintf(errbuf, "unknown compression method %u: ", z->how);
        zipwarn(errbuf, z->oname);
    }
    if (z->dsk)
    {
        sprintf(errbuf, "starts on disk %lu: ", z->dsk);
        zipwarn(errbuf, z->oname);
    }
    if (z->att!=ASCII && z->att!=BINARY && z->att!=__EBCDIC)
    {
        sprintf(errbuf, "unknown internal attributes = 0x%04x: ", z->att);
        zipwarn(errbuf, z->oname);
    }
# if 0
/* This test is ridiculous, it produces an error message for almost every */
/* platform of origin other than MS-DOS, Unix, VMS, and Acorn!  Perhaps   */
/* we could test "if (z->dosflag && z->atx & ~0xffL)", but what for?      */
    if (((n = z->vem >> 8) != 3) && n != 2 && n != 13 && z->atx & ~0xffL)
    {
        sprintf(errbuf, "unknown external attributes = 0x%08lx: ", z->atx);
        zipwarn(errbuf, z->oname);
    }
# endif

    /* This test is just annoying, as Zip itself does not write the same
       extra fields to both the local and central headers.  It's much more
       complicated than this test implies.  3/17/05 */
#if 0
    if (z->ext || z->cext)
    {
# if 0
        if (z->ext && z->cext && z->extra != z->cextra)
        {
          sprintf(errbuf,
                  "local extra (%ld bytes) != central extra (%ld bytes): ",
                  (ulg)z->ext, (ulg)z->cext);
          if (noisy) fprintf(mesg, "\tzip info: %s%s\n", errbuf, z->oname);
        }
#   if (!defined(RISCOS) && !defined(CMS_MVS))
        /* in noisy mode, extra field sizes are always reported */
        else if (noisy)
#   else /* RISCOS || CMS_MVS */
/* avoid warnings for zipfiles created on the same type of OS system! */
/* or, was this warning really intended (eg. OS/2)? */
        /* Only give info if extra bytes were added by another system */
        else if (noisy && ((z->vem >> 8) != (OS_CODE >> 8)))
#   endif /* ?(RISCOS || CMS_MVS) */
# endif /* 0 */
        {
            fprintf(mesg, "zip info: %s has %ld bytes of %sextra data\n",
                    z->oname, z->ext ? (ulg)z->ext : (ulg)z->cext,
                    z->ext ? (z->cext ? "" : "local ") : "central ");
        }
    }
#endif
}


#if 0 /* scanzipf_fix() no longer used */
/*
 * scanzipf_fix is called with zip -F or zip -FF
 * read the file from front to back and pick up the pieces
 * NOTE: there are still checks missing to see if the header
 *       that was found is *VALID*
 *
 * Still much work to do so can handle more cases.  1/18/04 EG
 */
local int scanzipf_fix(f)
  FILE *f;                      /* zip file */
/*
   The name of the zip file is pointed to by the global "zipfile".  The globals
   zipbeg, cenbeg, zfiles, zcount, zcomlen, zcomment, and zsort are filled in.
   Return an error code in the ZE_ class.
*/
{
    ulg a = 0L;                 /* attributes returned by filetime() */
    char b[CENHEAD];            /* buffer for central headers */
    ush flg;                    /* general purpose bit flag */
    int m;                      /* mismatch flag */
    extent n;                   /* length of name */
    uzoff_t p;                  /* current file offset */
    uzoff_t s;                  /* size of data, start of central */
    struct zlist far * far *x;  /* pointer last entry's link */
    struct zlist far *z;        /* current zip entry structure */

#ifndef ZIP64_SUPPORT

/* 2004-12-06 SMS.
 * Check for too-big file before doing any serious work.
 */
    if (ffile_size( f) == EOF)
      return ZE_ZIP64;

#endif /* ndef ZIP64_SUPPORT */


    /* Get any file attribute valid for this OS, to set in the central
     * directory when fixing the archive:
     */
# ifndef UTIL
    filetime(zipfile, &a, (zoff_t*)&s, NULL);
# endif
    x = &zfiles;                        /* first link */
    p = 0;                              /* starting file offset */
# ifdef HANDLE_AMIGA_SFX
    amiga_sfx_offset = 0L;
# endif

    /* Find start of zip structures */
    for (;;) {
      /* look for signature */
      while ((m = getc(f)) != EOF && m != 0x50)    /* 0x50 == 'P' */
      {
# ifdef HANDLE_AMIGA_SFX
        if (p == 0 && m == 0)
          amiga_sfx_offset = 1L;
        else if (amiga_sfx_offset) {
          if ((p == 1 && m != 0) || (p == 2 && m != 3)
                                 || (p == 3 && (uch) m != 0xF3))
            amiga_sfx_offset = 0L;
        }
# endif /* HANDLE_AMIGA_SFX */
        p++;
      }
      /* found a P */
      b[0] = (char) m;
      /* local - 11/2/03 EG */
      if (fread(b+1, 3, 1, f) != 1 || (s = LG(b)) == LOCSIG)
        break;
      /* why search for ENDSIG if doing only local - 11/2/03 EG
      if (fread(b+1, 3, 1, f) != 1 || (s = LG(b)) == LOCSIG || s == ENDSIG)
        break;
      */
      /* back up */
      if (zfseeko(f, -3L, SEEK_CUR))
        return ferror(f) ? ZE_READ : ZE_EOF;
      /* move 1 byte forward */
      p++;
    }
    zipbeg = p;
# ifdef HANDLE_AMIGA_SFX
    if (amiga_sfx_offset && zipbeg >= 12 && (zipbeg & 3) == 0
        && fseek(f, -12L, SEEK_CUR) == 0 && fread(b, 12, 1, f) == 1
        && LG(b + 4) == 0xF1030000 /* 1009 in Motorola byte order */)
      amiga_sfx_offset = zipbeg - 4;
    else
      amiga_sfx_offset = 0L;
# endif /* HANDLE_AMIGA_SFX */

    /* Read local headers */
    while (LG(b) == LOCSIG)
    {
      if ((z = (struct zlist far *)farmalloc(sizeof(struct zlist))) == NULL ||
          zcount + 1 < zcount)
        return ZE_MEM;
      if (fread(b, LOCHEAD, 1, f) != 1) {
          farfree((zvoid far *)z);
          break;
      }

      z->ver = SH(LOCVER + b);
      z->vem = (ush)(dosify ? 20 : OS_CODE + Z_MAJORVER * 10 + Z_MINORVER);
      z->dosflag = dosify;
      flg = z->flg = z->lflg = SH(LOCFLG + b);
      z->how = SH(LOCHOW + b);
      z->tim = LG(LOCTIM + b);          /* time and date into one long */
      z->crc = LG(LOCCRC + b);
      z->siz = LG(LOCSIZ + b);
      z->len = LG(LOCLEN + b);
      n = z->nam = SH(LOCNAM + b);
      z->cext = z->ext = SH(LOCEXT + b);

      z->com = 0;
      z->dsk = 0;
      z->att = 0;
      z->atx = dosify ? a & 0xff : a;     /* Attributes from filetime() */
      z->mark = 0;
      z->trash = 0;

      /* attention: this one breaks the VC optimizer (Release Build) */
      /* may be fixed - 11/1/03 EG */
      s = fix > 1 ? 0L : z->siz; /* discard compressed size with -FF */

      /* Initialize all fields pointing to malloced data to NULL */
      z->zname = z->name = z->iname = z->extra = z->cextra = z->comment = NULL;
      z->oname = NULL;
#ifdef UNICODE_SUPPORT
      z->uname = z->zuname = z->ouname = NULL;
#endif

      /* Link into list */
      *x = z;
      z->nxt = NULL;
      x = &z->nxt;

      /* Read file name and extra field and skip data */
      if (n == 0)
      {
        sprintf(errbuf, "%lu", (ulg)zcount + 1);
        zipwarn("zero-length name for entry #", errbuf);
# ifndef DEBUG
        return ZE_FORM;
# endif
      }
      if ((z->iname = malloc(n+1)) ==  NULL ||
          (z->ext && (z->extra = malloc(z->ext)) == NULL))
        return ZE_MEM;
      if (fread(z->iname, n, 1, f) != 1 ||
          (z->ext && fread(z->extra, z->ext, 1, f) != 1))
        return ferror(f) ? ZE_READ : ZE_EOF;

#  ifdef ZIP64_SUPPORT
      /* adjust/update siz,len and off (to come: dsk) entries */
      /* PKZIP does not care of the version set in a CDH: if  */
      /* there is a zip64 extra field assigned to a CDH PKZIP */
      /* uses it, we should do so, too.                       */
      zip64_entry = adjust_zip_local_entry(z);
      /* z->siz may be updated */
      s = fix > 1 ? 0L : z->siz; /* discard compressed size with -FF */
#  endif

      if (s && zfseeko(f, (zoff_t)s, SEEK_CUR))
        return ferror(f) ? ZE_READ : ZE_EOF;
      /* If there is an extended local header, s is either 0 or
       * the correct compressed size.
       */
      z->iname[n] = '\0';               /* terminate name */
      z->zname = in2ex(z->iname);       /* convert to external name */
      if (z->zname == NULL)
        return ZE_MEM;
      z->name = z->zname;
      z->cextra = z->extra;
      if (noisy) fprintf(mesg, "zip: reading %s\n", z->zname);

      /* Save offset, update for next header */
      z->off = p;
      p += 4 + LOCHEAD + n + z->ext + s;
      zcount++;

      /* Skip extended local header if there is one */
      if ((flg & 8) != 0) {
        /* Skip the compressed data if compressed size is unknown.
         * For safety, we should use the central directory.
         */
        if (s == 0) {
          for (;;) {
            while ((m = getc(f)) != EOF && m != 0x50) ;  /* 0x50 == 'P' */
            b[0] = (char) m;
            if (fread(b+1, 15, 1, f) != 1 || LG(b) == EXTLOCSIG)
              break;
            if (zfseeko(f, -15L, SEEK_CUR))
              return ferror(f) ? ZE_READ : ZE_EOF;
          }
# ifdef ZIP64_SUPPORT
          if (zip64_entry) {        /* from extra field */
            /* all are 8 bytes */
            s = LG(4 + ZIP64_EXTSIZ + b);
          } else {
            s = LG(4 + EXTSIZ + b);
          }
# else
          s = LG(4 + EXTSIZ + b);
# endif
          p += s;
          if ((uzoff_t) zftello(f) != p+16L) {
            zipwarn("bad extended local header for ", z->zname);
            return ZE_FORM;
          }
        } else {
          /* compressed size non-zero, assume that it is valid: */
          Assert(p == zftello(f), "bad compressed size with extended header");

          if (zfseeko(f, p, SEEK_SET) || fread(b, 16, 1, f) != 1)
            return ferror(f) ? ZE_READ : ZE_EOF;
          if (LG(b) != EXTLOCSIG) {
            zipwarn("extended local header not found for ", z->zname);
            return ZE_FORM;
          }
        }
        /* overwrite the unknown values of the local header: */

        /* already in host format */
# ifdef ZIP64_SUPPORT
        z->crc = LG(4 + ZIP64_EXTCRC + b);
        z->siz = s;
        z->len = LG(4 + ZIP64_EXTLEN + b);
# else
        z->crc = LG(4 + EXTCRC + b);
        z->siz = s;
        z->len = LG(4 + EXTLEN + b);
# endif

        p += 16L;
      }
      else if (fix > 1) {
        /* Don't trust the compressed size */
        for (;;) {
          while ((m = getc(f)) != EOF && m != 0x50) p++; /* 0x50 == 'P' */
          b[0] = (char) m;
          if (fread(b+1, 3, 1, f) != 1 || (s = LG(b)) == LOCSIG || s == CENSIG)
            break;
          if (zfseeko(f, -3L, SEEK_CUR))
            return ferror(f) ? ZE_READ : ZE_EOF;
          p++;
        }
        s = p - (z->off + 4 + LOCHEAD + n + z->ext);
        if (s != z->siz) {
          fprintf(mesg, " compressed size %s, actual size %s for %s\n",
                  zip_fzofft(z->siz, NULL, "u"), zip_fzofft(s, NULL, "u"),
                  z->zname);
          z->siz = s;
        }
        /* next LOCSIG already read at this point, don't read it again: */
        continue;
      }

      /* Read next signature */
      if (fread(b, 4, 1, f) != 1)
          break;
    }

    s = p;                              /* save start of central */

    if (LG(b) != CENSIG && noisy) {
      fprintf(mesg, "zip warning: %s %s truncated.\n", zipfile,
              fix > 1 ? "has been" : "would be");

      if (fix == 1) {
        fprintf(mesg,
   "Retry with option -qF to truncate, with -FF to attempt full recovery\n");
        ZIPERR(ZE_FORM, NULL);
      }
    }

    cenbeg = s;

    if (zipbeg && noisy)
      fprintf(mesg, "%s: adjusting offsets for a preamble of %s bytes\n",
              zipfile, zip_fzofft(zipbeg, NULL, "u"));
    return ZE_OK;
} /* end of function scanzipf_fix() */
#endif /* never, scanzipf_fix() no longer used */

#endif /* !UTIL */

/*
 * read_local
 *
 * Read the local header assumed at in_file file pointer.
 * localz is the returned local header, z is the central directory entry.
 *
 * This is used by crypt.c.
 *
 * Return ZE code
 */
int readlocal(localz, z)
  struct zlist far **localz;
  struct zlist far *z;
{
  char buf[LOCHEAD + 1];
  struct zlist far *locz;

#ifndef UTIL
  ulg start_disk = 0;
  uzoff_t start_offset = 0;
  char *split_path;

  start_disk = z->dsk;
  start_offset = z->off;

  /* don't assume reading the right disk */

  if (start_disk != current_in_disk) {
    if (in_file) {
      fclose(in_file);
      in_file = NULL;
    }
  }

  current_in_disk = start_disk;

  /* disks are archive.z01, archive.z02, ..., archive.zip */
  split_path = get_in_split_path(in_path, current_in_disk);

  if (in_file == NULL) {
    while ((in_file = zfopen(split_path, FOPR)) == NULL) {
      /* could not open split */

      /* Ask for directory with split.  Updates in_path */
      if (ask_for_split_read_path(start_disk) != ZE_OK) {
        return ZE_ABORT;
      }
      free(split_path);
      split_path = get_in_split_path(in_path, start_disk);
    }
  }
#endif

  /* For utilities assume archive is on one disk for now */

  if (zfseeko(in_file, z->off, SEEK_SET) != 0) {
    fclose(in_file);
    in_file = NULL;
    zipwarn("reading archive fseek: ", strerror(errno));
    return ZE_READ;
  }
  if (!at_signature(in_file, "PK\03\04")) {
    fclose(in_file);
    in_file = NULL;
    zipwarn("Did not find entry for ", z->iname);
    return ZE_FORM;
  }

  /* read local header */
  if (fread(buf, LOCHEAD, 1, in_file) != 1) {
    int f = ferror(in_file);
    zipwarn("reading local entry: ", strerror(errno));
    fclose(in_file);
    return f ? ZE_READ : ZE_EOF;
  }

  /* Local Header
       local file header signature     4 bytes  (0x04034b50)
       version needed to extract       2 bytes
       general purpose bit flag        2 bytes
       compression method              2 bytes
       last mod file time              2 bytes
       last mod file date              2 bytes
       crc-32                          4 bytes
       compressed size                 4 bytes
       uncompressed size               4 bytes
       file name length                2 bytes
       extra field length              2 bytes

       file name (variable size)
       extra field (variable size)
   */

  if ((locz = (struct zlist far *)farmalloc(sizeof(struct zlist))) == NULL) {
    zipwarn("reading entry", "");
    fclose(in_file);
    return ZE_MEM;
  }

  locz->ver = SH(LOCVER + buf);
  locz->lflg = SH(LOCFLG + buf);
  locz->how = SH(LOCHOW + buf);
  locz->tim = LG(LOCTIM + buf);          /* time and date into one long */
  locz->crc = LG(LOCCRC + buf);
  locz->nam = SH(LOCNAM + buf);
  locz->ext = SH(LOCEXT + buf);

  /* Initialize all fields pointing to malloced data to NULL */
  locz->zname = locz->name = locz->iname = locz->extra = NULL;
  locz->oname = NULL;
#ifdef UNICODE_SUPPORT
  locz->uname = NULL;
  locz->zuname = NULL;
  locz->ouname = NULL;
#endif

  /* Read file name, extra field and comment field */
  if ((locz->iname = malloc(locz->nam+1)) ==  NULL ||
      (locz->ext && (locz->extra = malloc(locz->ext)) == NULL))
    return ZE_MEM;
  if (fread(locz->iname, locz->nam, 1, in_file) != 1 ||
      (locz->ext && fread(locz->extra, locz->ext, 1, in_file) != 1))
    return ferror(in_file) ? ZE_READ : ZE_EOF;
  locz->iname[z->nam] = '\0';                  /* terminate name */
#ifdef UNICODE_SUPPORT
  if (unicode_mismatch != 3)
    read_Unicode_Path_local_entry(locz);
#endif
#ifdef WIN32
  {
    /* translate archive name from OEM if came from OEM-charset environment */
    unsigned hostver = (z->vem & 0xff);
    Ext_ASCII_TO_Native(locz->iname, (z->vem >> 8), hostver,
                        ((z->atx & 0xffff0000L) != 0), TRUE);
  }
#endif
  if ((locz->name = malloc(locz->nam+1)) ==  NULL)
    return ZE_MEM;
  strcpy(locz->name, locz->iname);

#ifdef ZIP64_SUPPORT
  zip64_entry = adjust_zip_local_entry(locz);
#endif

  /* Compare localz to z */
  if (locz->ver != z->ver) {
    sprintf(errbuf, "Local Version Needed (%d) does not match CD (%d): ", locz->ver, z->ver);
    zipwarn(errbuf, z->iname);
  }
  if (locz->lflg != z->flg) {
    zipwarn("Local Entry Flag does not match CD: ", z->iname);
  }
  if (locz->crc != z->crc) {
    zipwarn("Local Entry CRC does not match CD: ", z->iname);
  }

  /* as copying get uncompressed and compressed sizes from central directory */
  locz->len = z->len;
  locz->siz = z->siz;

  *localz = locz;

  return ZE_OK;
} /* end function readlocal() */

#if 0 /* following functions are not (no longer) used. */
/*
 * scanzipf_reg starts searching for the End Signature at the end of the file
 * The End Signature points to the Central Directory Signature which points
 * to the Local Directory Signature
 * XXX probably some more consistency checks are needed
 */
local int scanzipf_reg(f)
  FILE *f;                      /* zip file */
/*
   The name of the zip file is pointed to by the global "zipfile".  The globals
   zipbeg, cenbeg, zfiles, zcount, zcomlen, zcomment, and zsort are filled in.
   Return an error code in the ZE_ class.
*/
{
    char b[CENHEAD];            /* buffer for central headers */
    extent n;                   /* length of name */
    struct zlist far * far *x;  /* pointer last entry's link */
    struct zlist far *z;        /* current zip entry structure */
    char *t;                    /* temporary pointer */
    char far *u;                /* temporary variable */
    int found;
    char *buf;                  /* temp buffer for reading zipfile */
# ifdef ZIP64_SUPPORT
    ulg u4;                     /* unsigned 4 byte variable */
    char bf[8];
    uzoff_t u8;                 /* unsigned 8 byte variable */
    uzoff_t censiz;             /* size of central directory */
    uzoff_t z64eocd;            /* Zip64 End Of Central Directory record byte offset */
# else
    ush flg;                    /* general purpose bit flag */
    int m;                      /* mismatch flag */
# endif
    zoff_t deltaoff = 0;


#ifndef ZIP64_SUPPORT

    /* 2004-12-06 SMS.
     * Check for too-big file before doing any serious work.
     */
    if (ffile_size( f) == EOF)
      return ZE_ZIP64;

#endif /* ndef ZIP64_SUPPORT */


    buf = malloc(4096 + 4);
    if (buf == NULL)
      return ZE_MEM;

#ifdef HANDLE_AMIGA_SFX
    amiga_sfx_offset = (fread(buf, 1, 4, f) == 4 && LG(buf) == 0xF3030000);
    /* == 1 if this file is an Amiga executable (presumably UnZipSFX) */
#endif
    /* detect spanning signature */
    zfseeko(f, 0, SEEK_SET);
    read_split_archive = (fread(buf, 1, 4, f) == 4 && LG(buf) == 0x08074b50L);
    found = 0;
    t = &buf[4096];
    t[1] = '\0';
    t[2] = '\0';
    t[3] = '\0';
    /* back up as much as 4k from end */
    /* zip64 support 08/31/2003 R.Nausedat */
    if (zfseeko(f, -4096L, SEEK_END) == 0) {
      zipbeg = (uzoff_t) (zftello(f) + 4096L);
      /* back up 4k blocks and look for End Of CD signature */
      while (!found && zipbeg >= 4096) {
        zipbeg -= 4096L;
        buf[4096] = t[1];
        buf[4097] = t[2];
        buf[4098] = t[3];
/*
 * XXX error check ??
 */
        fread(buf, 1, 4096, f);
        zfseeko(f, -8192L, SEEK_CUR);
        t = &buf[4095];
/*
 * XXX far pointer arithmetic in DOS
 */
        while (t >= buf) {
          /* Check for ENDSIG ("PK\5\6" in ASCII) */
          if (LG(t) == ENDSIG) {
            found = 1;
/*
 * XXX error check ??
 * XXX far pointer arithmetic in DOS
 */
            zipbeg += (uzoff_t) (t - buf);
            zfseeko(f, (zoff_t) zipbeg + 4L, SEEK_SET);
            break;
          }
          --t;
        }
      }
    }
    else
      /* file less than 4k bytes */
      zipbeg = 4096L;
/*
 * XXX warn: garbage at the end of the file ignored
 */
    if (!found && zipbeg > 0) {
      size_t s;

      zfseeko(f, 0L, SEEK_SET);
      clearerr(f);
      s = fread(buf, 1, (size_t) zipbeg, f);
      /* add 0 bytes at end */
      buf[s] = t[1];
      buf[s + 1] = t[2];
      buf[s + 2] = t[3];
      t = &buf[s - 1];
/*
 * XXX far pointer comparison in DOS
 */
      while (t >= buf) {
        /* Check for ENDSIG ("PK\5\6" in ASCII) */
        if (LG(t) == ENDSIG) {
          found = 1;
/*
 * XXX far pointer arithmetic in DOS
 */
          zipbeg = (ulg) (t - buf);
          zfseeko(f, (zoff_t) zipbeg + 4L, SEEK_SET);
          break;
        }
        --t;
      }
    }
    free(buf);
    if (!found) {
      zipwarn("missing end signature--probably not a zip file (did you", "");
      zipwarn("remember to use binary mode when you transferred it?)", "");
      return ZE_FORM;
    }

/*
 * Check for a Zip64 EOCD Locator signature - 12/10/04 EG
 */
#ifndef ZIP64_SUPPORT
    /* If Zip64 not enabled check if archive being read is Zip64 */
    /* back up 24 bytes (size of Z64 EOCDL and ENDSIG) */
    if (zfseeko(f, -24, SEEK_CUR) != 0) {
        perror("fseek");
        return ZE_FORM; /* XXX */
    }
    /* read Z64 EOCDL if there */
    if (fread(b, 20, 1, f) != 1) {
      return ZE_READ;
    }
    /* first 4 bytes are the signature if there */
    if (LG(b) == ZIP64_EOCDL_SIG) {
      zipwarn("found Zip64 signature - this may be a Zip64 archive", "");
      zipwarn("PKZIP 4.5 or later needed - set ZIP64_SUPPORT in Zip 3", "");
      return ZE_ZIP64;
    }

    /* now should be back at the EOCD signature */
    if (fread(b, 4, 1, f) != 1) {
      zipwarn("unable to read after relative seek", "");
      return ZE_READ;
    }
    if (LG(b) != ENDSIG) {
      zipwarn("unable to relative seek in archive", "");
      return ZE_FORM;
    }
#if 0
    if (fseek(f, -4, SEEK_CUR) != 0) {
        perror("fseek");
        return ZE_FORM; /* XXX */
    }
#endif
#endif

    /* Read end header */
    if (fread(b, ENDHEAD, 1, f) != 1)
      return ferror(f) ? ZE_READ : ZE_EOF;
    if (SH(ENDDSK + b) || SH(ENDBEG + b) ||
        SH(ENDSUB + b) != SH(ENDTOT + b))
      zipwarn("multiple disk information ignored", "");
    zcomlen = SH(ENDCOM + b);
    if (zcomlen)
    {
      if ((zcomment = malloc(zcomlen)) == NULL)
        return ZE_MEM;
      if (fread(zcomment, zcomlen, 1, f) != 1)
      {
        free((zvoid *)zcomment);
        zcomment = NULL;
        return ferror(f) ? ZE_READ : ZE_EOF;
      }
#ifdef EBCDIC
      if (zcomment)
         memtoebc(zcomment, zcomment, zcomlen);
#endif /* EBCDIC */
    }
#ifdef ZIP64_SUPPORT
    /* account for Zip64 EOCD Record and Zip64 EOCD Locator */

    /* Z64 EOCDL should be just before EOCD (unless this is an empty archive) */
    cenbeg = zipbeg - ZIP64_EOCDL_OFS_SIZE;
    /* check for empty archive */
    /* changed cenbeg to uzoff_t so instead of cenbeg >= 0 use new check - 5/23/05 EG */
    if (zipbeg >= ZIP64_EOCDL_OFS_SIZE) {
      /* look for signature */
      if (zfseeko(f, cenbeg, SEEK_SET)) {
        zipwarn("end of file seeking Z64EOCDL", "");
        return ZE_FORM;
      }
      if (fread(bf, 4, 1, f) != 1) {
        ziperr(ZE_FORM, "read error");
      }
      u4 = LG(bf);
      if (u4 == ZIP64_EOCDL_SIG) {
        /* found Zip64 EOCD Locator */
        /* check for disk information */
        zfseeko(f, cenbeg + ZIP64_EOCDL_OFS_TOTALDISKS, SEEK_SET);
        if (fread(bf, 4, 1, f) != 1) {
          ziperr(ZE_FORM, "read error");
        }
        u4 = LG(bf);
        if (u4 != 1) {
          ziperr(ZE_FORM, "multiple disk archives not yet supported");
        }

        /* look for Zip64 EOCD Record */
        zfseeko(f, cenbeg + ZIP64_EOCDL_OFS_EOCD_START, SEEK_SET);
        if (fread(bf, 8, 1, f) != 1) {
         ziperr(ZE_FORM, "read error");
        }
        z64eocd = LLG(bf);
        if (zfseeko(f, z64eocd, SEEK_SET)) {
          ziperr(ZE_FORM, "error searching for Z64 EOCD Record");
        }
        if (fread(bf, 4, 1, f) != 1) {
         ziperr(ZE_FORM, "read error");
        }
        u4 = LG(bf);
        if (u4 != ZIP64_EOCD_SIG) {
          ziperr(ZE_FORM, "Z64 EOCD not found but Z64 EOCD Locator exists");
        }
        /* get size of CD */
        zfseeko(f, z64eocd + ZIP64_EOCD_OFS_SIZE, SEEK_SET);
        if (fread(bf, 8, 1, f) != 1) {
         ziperr(ZE_FORM, "read error");
        }
        censiz = LLG(bf);
        /* get start of CD */
        zfseeko(f, z64eocd + ZIP64_EOCD_OFS_CD_START, SEEK_SET);
        if (fread(bf, 8, 1, f) == (size_t) -1) {
         ziperr(ZE_FORM, "read error");
        }
        cenbeg = LLG(bf);
        u8 = z64eocd - cenbeg;
        deltaoff = adjust ? u8 - censiz : 0L;
      } else {
        /* assume no Locator and no Zip64 EOCD Record */
        censiz = LG(ENDSIZ + b);
        cenbeg = LG(b + ENDOFF);
        u8 = zipbeg - censiz;
        deltaoff = adjust ? u8 - censiz : 0L;
      }
    }
#else /* !ZIP64_SUPPORT */
/*
 * XXX assumes central header immediately precedes end header
 */
    /* start of central directory */
    cenbeg = zipbeg - LG(ENDSIZ + b);
/*
printf("start of central directory cenbeg %ld\n", cenbeg);
*/

    /* offset to first entry of archive */
    deltaoff = adjust ? cenbeg - LG(b + ENDOFF) : 0L;
#endif /* ?ZIP64_SUPPORT */

    if (zipbeg < ZIP64_EOCDL_OFS_SIZE) {
      /* zip file seems empty */
      return ZE_OK;
    }

    if (zfseeko(f, cenbeg, SEEK_SET) != 0) {
        perror("fseek");
        return ZE_FORM; /* XXX */
    }

    x = &zfiles;                        /* first link */

    if (fread(b, 4, 1, f) != 1)
      return ferror(f) ? ZE_READ : ZE_EOF;

    while (LG(b) == CENSIG) {
      /* Read central header. The portion of the central header that should
         be in common with local header is read raw, for later comparison.
         (this requires that the offset of ext in the zlist structure
         be greater than or equal to LOCHEAD) */
      if (fread(b, CENHEAD, 1, f) != 1)
        return ferror(f) ? ZE_READ : ZE_EOF;
      if ((z = (struct zlist far *)farmalloc(sizeof(struct zlist))) == NULL)
        return ZE_MEM;
      z->vem = SH(CENVEM + b);
      for (u = (char far *)(&(z->ver)), n = 0; n < (CENNAM-CENVER); n++)
        u[n] = b[CENVER + n];
      z->nam = SH(CENNAM + b);          /* used before comparing cen vs. loc */
      z->cext = SH(CENEXT + b);         /* may be different from z->ext */
      z->com = SH(CENCOM + b);
      z->dsk = SH(CENDSK + b);
      z->att = SH(CENATT + b);
      z->atx = LG(CENATX + b);
      z->off = LG(CENOFF + b) + deltaoff;
      z->dosflag = (z->vem & 0xff00) == 0;

      /* Initialize all fields pointing to malloced data to NULL */
      z->zname = z->name = z->iname = z->extra = z->cextra = z->comment = NULL;
      z->oname = NULL;
#ifdef UNICODE_SUPPORT
      z->uname = NULL;      /* UTF-8 path */
      z->zuname = NULL;     /* Escaped local version of uname */
      z->ouname = NULL;     /* Display version of zuname */
#endif

      /* Link into list */
      *x = z;
      z->nxt = NULL;
      x = &z->nxt;

      /* Read file name, extra field and comment field */
      if (z->nam == 0)
      {
        sprintf(errbuf, "%lu", (ulg)zcount + 1);
        zipwarn("zero-length name for entry #", errbuf);
#ifndef DEBUG
        farfree((zvoid far *)z);
        return ZE_FORM;
#endif
      }
      if ((z->iname = malloc(z->nam+1)) ==  NULL ||
          (z->cext && (z->cextra = malloc(z->cext)) == NULL) ||
          (z->com && (z->comment = malloc(z->com)) == NULL))
        return ZE_MEM;
      if (fread(z->iname, z->nam, 1, f) != 1 ||
          (z->cext && fread(z->cextra, z->cext, 1, f) != 1) ||
          (z->com && fread(z->comment, z->com, 1, f) != 1))
        return ferror(f) ? ZE_READ : ZE_EOF;
      z->iname[z->nam] = '\0';                  /* terminate name */

#ifdef EBCDIC
      if (z->com)
         memtoebc(z->comment, z->comment, z->com);
#endif /* EBCDIC */

#ifdef ZIP64_SUPPORT
      /* zip64 support 08/31/2003 R.Nausedat                          */
      /* here, we have to read the len, siz etc values from the CD    */
      /* entry as we might have to adjust them regarding their        */
      /* correspronding zip64 extra fields.                           */
      /* also, we cannot compare the values from the CD entries with  */
      /* the values from the LH as they might be different.           */
      z->len = LG(CENLEN + b);
      z->siz = LG(CENSIZ + b);
      z->crc = LG(CENCRC + b);
      z->tim = LG(CENTIM + b);   /* time and date into one long */
      z->how = SH(CENHOW + b);
      z->flg = SH(CENFLG + b);
      z->ver = SH(CENVER + b);
      /* adjust/update siz,len and off (to come: dsk) entries */
      /* PKZIP does not care of the version set in a CDH: if  */
      /* there is a zip64 extra field assigned to a CDH PKZIP */
      /* uses it, we should do so, too.                       */
      adjust_zip_central_entry(z);
#endif /* ZIP64_SUPPORT */

      /* Update zipbeg offset, prepare for next header */
      if (z->off < zipbeg)
         zipbeg = z->off;
      zcount++;
      /* Read next signature */
      if (fread(b, 4, 1, f) != 1)
          return ferror(f) ? ZE_READ : ZE_EOF;
    }

    /* Point to start of header list and read local headers */
    z = zfiles;
    while (z != NULL) {
      /* Read next signature */
      if (zfseeko(f, z->off, SEEK_SET) != 0 || fread(b, 4, 1, f) != 1)
        return ferror(f) ? ZE_READ : ZE_EOF;
      if (LG(b) == LOCSIG) {
        if (fread(b, LOCHEAD, 1, f) != 1)
            return ferror(f) ? ZE_READ : ZE_EOF;

        z->lflg = SH(LOCFLG + b);
        n = SH(LOCNAM + b);
        z->ext = SH(LOCEXT + b);

        /* Compare name and extra fields */
        if (n != z->nam)
        {
#ifdef EBCDIC
          strtoebc(z->iname, z->iname);
#endif
          zipwarn("name lengths in local and central differ for ", z->iname);
          return ZE_FORM;
        }
        if ((t = malloc(z->nam)) == NULL)
          return ZE_MEM;
        if (fread(t, z->nam, 1, f) != 1)
        {
          free((zvoid *)t);
          return ferror(f) ? ZE_READ : ZE_EOF;
        }
        if (memcmp(t, z->iname, z->nam))
        {
          free((zvoid *)t);
#ifdef EBCDIC
          strtoebc(z->iname, z->iname);
#endif
          zipwarn("names in local and central differ for ", z->iname);
          return ZE_FORM;
        }
        free((zvoid *)t);
        if (z->ext)
        {
          if ((z->extra = malloc(z->ext)) == NULL)
            return ZE_MEM;
          if (fread(z->extra, z->ext, 1, f) != 1)
          {
            free((zvoid *)(z->extra));
            return ferror(f) ? ZE_READ : ZE_EOF;
          }
          if (z->ext == z->cext && memcmp(z->extra, z->cextra, z->ext) == 0)
          {
            free((zvoid *)(z->extra));
            z->extra = z->cextra;
          }
        }

#ifdef ZIP64_SUPPORT       /* zip64 support 09/02/2003 R.Nausedat */
        /*
        for now the below is left out if ZIP64_SUPPORT is defined as the fields
        len, siz and off in struct zlist are type of int64 if ZIP64_SUPPORT
        is defined. In either way, the values read from the central directory
        should be valid. comments are welcome
        */
#else /* !ZIP64_SUPPORT */
        /* Check extended local header if there is one */
        /* bit 3 */
        if ((z->lflg & 8) != 0)
        {
          char buf2[16];
          ulg s;                        /* size of compressed data */

          s = LG(LOCSIZ + b);
          if (s == 0)
            s = LG((CENSIZ-CENVER) + (char far *)(&(z->ver)));
          if (zfseeko(f, (z->off + (4+LOCHEAD) + z->nam + z->ext + s), SEEK_SET)
              || (fread(buf2, 16, 1, f) != 1))
            return ferror(f) ? ZE_READ : ZE_EOF;
          if (LG(buf2) != EXTLOCSIG)
          {
# ifdef EBCDIC
            strtoebc(z->iname, z->iname);
# endif
            zipwarn("extended local header not found for ", z->iname);
            return ZE_FORM;
          }
          /* overwrite the unknown values of the local header: */
          for (n = 0; n < 12; n++)
            b[LOCCRC+n] = buf2[4+n];
        }

        /* Compare local header with that part of central header (except
           for the reserved bits in the general purpose flags and except
           for the already checked entry name length */
        /* If I have read this right we are stepping through the z struct
           here as a byte array.  Need to fix this.  5/25/2005 EG */
        u = (char far *)(&(z->ver));
        flg = SH((CENFLG-CENVER) + u);          /* Save central flags word */
        u[CENFLG-CENVER+1] &= 0x1f;             /* Mask reserved flag bits */
        b[LOCFLG+1] &= 0x1f;
        for (m = 0, n = 0; n < LOCNAM; n++) {
          if (b[n] != u[n])
          {
            if (!m)
            {
              zipwarn("local and central headers differ for ", z->iname);
              m = 1;
            }
            if (noisy)
            {
              sprintf(errbuf, " offset %u--local = %02x, central = %02x",
                      (unsigned)n, (uch)b[n], (uch)u[n]);
              zipwarn(errbuf, "");
            }
          }
        }
        if (m && !adjust)
          return ZE_FORM;

        /* Complete the setup of the zlist entry by translating the remaining
         * central header fields in memory, starting with the fields with
         * highest offset. This order of the conversion commands takes into
         * account potential buffer overlaps caused by structure padding.
         */
        z->len = LG((CENLEN-CENVER) + u);
        z->siz = LG((CENSIZ-CENVER) + u);
        z->crc = LG((CENCRC-CENVER) + u);
        z->tim = LG((CENTIM-CENVER) + u);   /* time and date into one long */
        z->how = SH((CENHOW-CENVER) + u);
        z->flg = flg;                       /* may be different from z->lflg */
        z->ver = SH((CENVER-CENVER) + u);
#endif /* ?ZIP64_SUPPORT */

        /* Clear actions */
        z->mark = 0;
        z->trash = 0;
#ifdef UNICODE_SUPPORT
        if (unicode_mismatch != 3) {
          read_Unicode_Path_entry(z);
          if (z->uname) {
            /* match based on converted Unicode name */
            z->name = utf8_to_local_string(z->uname);
# ifdef EBCDIC
            /* z->zname is used for printing and must be coded in native charset */
            strtoebc(z->zname, z->name);
# else
            if ((z->zname = malloc(strlen(z->name) + 1)) == NULL) {
              ZIPERR(ZE_MEM, "scanzipf_reg");
            }
            strcpy(z->zname, z->name);
# endif
            z->oname = local_to_display_string(z->zname);
          } else {
            /* no UTF-8 path */
            if ((z->name = malloc(strlen(z->iname) + 1)) == NULL) {
              ZIPERR(ZE_MEM, "scanzipf_reg");
            }
            strcpy(z->name, z->iname);
            if ((z->zname = malloc(strlen(z->iname) + 1)) == NULL) {
              ZIPERR(ZE_MEM, "scanzipf_reg");
            }
            strcpy(z->zname, z->iname);
            z->oname = local_to_display_string(z->iname);
          }
        }
#else /* !UNICODE_SUPPORT */
# ifdef UTIL
/* We only need z->iname in the utils */
        z->name = z->iname;
#  ifdef EBCDIC
/* z->zname is used for printing and must be coded in native charset */
        if ((z->zname = malloc(z->nam+1)) ==  NULL)
          return ZE_MEM;
        strtoebc(z->zname, z->iname);
#  else
        z->zname = z->iname;
#  endif
# else /* !UTIL */
        z->zname = in2ex(z->iname);       /* convert to external name */
        if (z->zname == NULL)
          return ZE_MEM;
        z->name = z->zname;
# endif /* ?UTIL */
        if ((z->oname = malloc(strlen(z->zname) + 1)) == NULL) {
          ZIPERR(ZE_MEM, "scanzipf_reg");
        }
        strcpy(z->oname, z->zname);
#endif /* ?UNICODE_SUPPORT */
      }
      else {
#ifdef EBCDIC
        strtoebc(z->iname, z->iname);
#endif
        zipwarn("local header not found for ", z->iname);
        return ZE_FORM;
      }
#ifndef UTIL
      if (verbose && fix == 0)
        zipoddities(z);
#endif
      z = z->nxt;
    }

    if (zipbeg && noisy)
      fprintf(mesg, "%s: %s a preamble of %s bytes\n",
              zipfile, adjust ? "adjusting offsets for" : "found",
              zip_fzofft(zipbeg, NULL, "u"));
#ifdef HANDLE_AMIGA_SFX
    if (zipbeg < 12 || (zipbeg & 3) != 0 /* must be longword aligned */)
      amiga_sfx_offset = 0;
    else if (amiga_sfx_offset) {
      char buf2[16];
      if (!fseek(f, zipbeg - 12, SEEK_SET) && fread(buf2, 12, 1, f) == 1) {
        if (LG(buf2 + 4) == 0xF1030000 /* 1009 in Motorola byte order */)
          /* could also check if LG(buf2) == 0xF2030000... no for now */
          amiga_sfx_offset = zipbeg - 4;
        else
          amiga_sfx_offset = 0L;
      }
    }
#endif /* HANDLE_AMIGA_SFX */
    return ZE_OK;
} /* end of function scanzipf_reg() */
#endif /* never */




/* find_next_signature
 *
 * Scan the file forward and look for the next PK signature.
 *
 * Return 1 if find one and leave file pointer pointing to next char
 * after signature and set sigbuf to signature.
 *
 * Return 0 if not.  Will be at EOF on return unless error.
 *
 */

local char sigbuf[4];   /* signature found */

#if 0 /* currently unused */
/* copy signature */
char *copy_sig(copyto, copyfrom)
  char *copyto;
  char *copyfrom;
{
  int i;

  for (i = 0; i < 4; i++) {
    copyto[i] = copyfrom[i];
  }
  return copyto;
}
#endif /* currently unused */


local int find_next_signature(f)
  FILE *f;
{
  int m;
  /*
  zoff_t here;
  */

  /* look for P K ? ? signature */

  m = getc(f);

  /*
  here = zftello(f);
  */

  while (m != EOF)
  {
    if (m == 0x50 /*'P' except EBCDIC*/) {
      /* found a P */
      sigbuf[0] = (char) m;

      if ((m = getc(f)) == EOF)
        break;
      if (m != 0x4b /*'K' except EBCDIC*/) {
        /* not a signature */
        ungetc(m, f);
      } else {
        /* found P K */
        sigbuf[1] = (char) m;

        if ((m = getc(f)) == EOF)
          break;
        if (m == 0x50 /*'P' except EBCDIC*/) {
          /* not a signature but maybe start of new one */
          ungetc(m, f);
          continue;
        } else if (m >= 16) {
          /* last 2 chars expect < 16 for signature */
          continue;
        }
        sigbuf[2] = (char) m;

        if ((m = getc(f)) == EOF)
          break;
        if (m == 0x50 /*'P' except EBCDIC*/) {
          /* not a signature but maybe start of new one */
          ungetc(m, f);
          continue;
        } else if (m >= 16) {
          /* last 2 chars expect < 16 */
          continue;
        }
        sigbuf[3] = (char) m;

        /* found possible signature */
        return 1;
      }
    }
    m = getc(f);
  }
  if (ferror(f)) {
    return 0;
  }

  /* found nothing */
  return 0;
}

/* find_signature
 *
 * Find signature.
 *
 * Return 1 if found and leave file pointing to next character
 * after signature.  Set sigbuf with signature.
 *
 * Return 0 if not found.
 */

local int find_signature(f, signature)
  FILE *f;
  ZCONST char *signature;
{
  int i;
  char sig[4];
  /*
  zoff_t here = zftello(f);
  */

  for (i = 0; i < 4; i++)
    sig[i] = signature[i];

  /* for EBCDIC */
  if (sig[0] == 'P')
    sig[0] = 0x50;
  if (sig[1] == 'K')
    sig[1] = 0x4b;

  while (!feof(f)) {
    if (!find_next_signature(f)) {
      return 0;
    } else {
      for (i = 0; i < 4; i++) {
        if (sig[i] != sigbuf[i]) {
          /* not a match */
          break;
        }
      }
      if (i == 4) {
        /* found it */
        return 1;
      }
    }
  }
  return 0;
}


/* is_signature
 *
 * Compare signatures
 *
 * Return 1 if the signatures match.
 */

local int is_signature(sig1, sig2)
  ZCONST char *sig1;
  ZCONST char *sig2;
{
  int i;
  char tsig1[4];
  char tsig2[4];

  for (i = 0; i < 4; i++) {
    tsig1[i] = sig1[i];
    tsig2[i] = sig2[i];
  }

  /* for EBCDIC */
  if (tsig1[0] == 'P')
    tsig1[0] = 0x50;
  if (tsig1[1] == 'K')
    tsig1[1] = 0x4b;

  if (tsig2[0] == 'P')
    tsig2[0] = 0x50;
  if (tsig2[1] == 'K')
    tsig2[1] = 0x4b;

  for (i = 0; i < 4; i++) {
    if (tsig1[i] != tsig2[i]) {
      /* not a match */
      break;
    }
  }
  if (i == 4) {
    /* found it */
    return 1;
  }
  return 0;
}


/* at_signature
 *
 * Is at signature in file
 *
 * Return 1 if at the signature and leave file pointing to next character
 * after signature.
 *
 * Return 0 if not.
 */

local int at_signature(f, signature)
  FILE *f;
  ZCONST char *signature;
{
  int i;
  extent m;
  char sig[4];
  char b[4];

  for (i = 0; i < 4; i++)
    sig[i] = signature[i];

  /* for EBCDIC */
  if (sig[0] == 'P')
    sig[0] = 0x50;
  if (sig[1] == 'K')
    sig[1] = 0x4b;

  m = fread(b, 1, 4, f);
  if (m != 4) {
    return 0;
  } else {
    for (i = 0; i < 4; i++) {
      if (sig[i] != b[i]) {
        /* not a match */
        break;
      }
    }
    if (i == 4) {
      /* found it */
      return 1;
    }
  }
  return 0;
}


#ifndef UTIL

local int scanzipf_fixnew()
/*
   Scan an assumed broke archive from the beginning, salvaging what can.

   Generally scanzipf_regnew() is used for reading archives normally and
   for fixing archives with a readable central directory using -F.  This
   scan is used by -FF and is for an archive that is unreadable by
   scanzipf_regnew().

   Start with the first file of the archive, either .z01 or .zip, and
   look for local entries.  Read local entries found and create zlist
   entries for them.  If we find central directory entries, read them
   and update the zlist created while reading local entries.

   The input path for the .zip file is in in_path.  If this is a multiple disk
   archive get the paths for splits from in_path as we go.  If a split is not in
   the same directory as the last split we ask the user where it is and update
   in_path.
 */
/*
   This is old:

   The name of the zip file is pointed to by the global "zipfile".  The globals
   zipbeg, cenbeg, zfiles, zcount, zcomlen, zcomment, and zsort are filled in.
   Return an error code in the ZE_ class.
*/
{
  /* This function only reads the standard End-of-CentralDir record and the
     standard CentralDir-Entry records directly.  To conserve stack space,
     only a buffer of minimal size is declared.
   */
# if CENHEAD > ENDHEAD
#   define FIXSCAN_BUFSIZE  CENHEAD
# else
#   define FIXSCAN_BUFSIZE  ENDHEAD
# endif

  char    scbuf[FIXSCAN_BUFSIZE];  /* buffer big enough for headers */
  char   *split_path;
  ulg     eocdr_disk;
  uzoff_t eocdr_offset;

  uzoff_t current_offset = 0; /* offset before */
  uzoff_t offset = 0;         /* location after return from seek */

  int skip_disk = 0;          /* 1 if user asks to skip current disk */
  int skipped_disk = 0;       /* 1 if skipped start disk and start offset is useless */

  int r = 0;                  /* zipcopy return */
  uzoff_t s;                  /* size of data, start of central */
  struct zlist far * far *x;  /* pointer last entry's link */
  struct zlist far *z;        /* current zip entry structure */
  int plen;
  char *in_path_ext;
  int in_central_directory = 0; /* found a central directory record */
  struct zlist far *cz;
  uzoff_t cd_total_entries = 0; /* number of entries according to EOCDR */
  ulg     in_cd_start_disk;     /* central directory start disk */
  uzoff_t in_cd_start_offset;   /* offset of start of cd on cd start disk */


  total_disks = 1000000;

  /* open the zipfile */
  /* This must be .zip file, even if it doesn't exist */

  /* see if zipfile name ends in .zip */
  plen = strlen(in_path);

#ifdef VMS
  /* On VMS, adjust plen (and in_path_ext) to avoid the file version. */
  plen -= strlen(vms_file_version(in_path));
#endif /* def VMS */
  in_path_ext = zipfile + plen - 4;

  if (plen >= 4 &&
      in_path_ext[0] == '.' &&
      toupper(in_path_ext[1]) == 'Z' &&
      in_path_ext[2] >= '0' && in_path_ext[2] <= '9' &&
      in_path_ext[3] >= '0' && in_path_ext[3] <= '9' &&
      (plen == 4 || (in_path_ext[4] >= '0' && in_path_ext[4] <= '9'))) {
    /* This may be a split but not the end split */
    strcpy(errbuf, "if archive to fix is split archive, need to provide\n");
    strcat(errbuf, "      path of the last split with .zip extension,\n");
    strcat(errbuf, "      even if it doesn't exist (zip will ask for splits)");
    zipwarn(errbuf, "");
    return ZE_FORM;
  }

  if ((in_file = zfopen(in_path, FOPR)) == NULL) {
    zipwarn("could not open input archive: ", in_path);
  }
  else
  {

#ifndef ZIP64_SUPPORT
    /* 2004-12-06 SMS.
     * Check for too-big file before doing any serious work.
     */
    if (ffile_size( in_file) == EOF) {
      fclose(in_file);
      in_file = NULL;
      zipwarn("input file requires Zip64 support: ", in_path);
      return ZE_ZIP64;
    }
#endif /* ndef ZIP64_SUPPORT */

    /* look for End Of Central Directory Record */

    /* back up 64k (the max size of the EOCDR) from end */
    if (zfseeko(in_file, -0x40000L, SEEK_END) != 0) {
      /* assume file is less than 64 KB so backup to beginning */
      if (zfseeko(in_file, 0L, SEEK_SET) != 0) {
        fclose(in_file);
        in_file = NULL;
        zipwarn("unable to seek in input file ", in_path);
        return ZE_READ;
      }
    }


    /* find EOCD Record signature */
    if (!find_signature(in_file, "PK\05\06")) {
      /* No End Of Central Directory Record */
      strcpy(errbuf, "Missing end (EOCDR) signature - either this archive\n");
      strcat(errbuf, "                     is not readable or the end is damaged");
      zipwarn(errbuf, "");
    }
    else
    {
      /* at start of data after EOCDR signature */
      eocdr_offset = (uzoff_t) zftello(in_file);

      /* OK, it is possible this is not the last EOCDR signature (might be
         EOCDR signature from a stored archive in the last 64 KB) and so not
         the one we want.

         The below assumes the signature does not appear in the assumed
         ASCII text .ZIP file comment.  Even if something like UTF-8
         is stored in the comment, it's unlikely the binary \05 and \06
         will be in the comment text.
      */
      while (find_signature(in_file, "PK\05\06")) {
        eocdr_offset = (uzoff_t) zftello(in_file);
      }

      /* found EOCDR */
      /* format is
           end of central dir signature     4 bytes  (0x06054b50)
           number of this disk              2 bytes
           number of the disk with the
            start of the central directory  2 bytes
           total number of entries in the
            central directory on this disk  2 bytes
           total number of entries in
            the central directory           2 bytes
           size of the central directory    4 bytes
           offset of start of central
            directory with respect to
            the starting disk number        4 bytes
           .ZIP file comment length         2 bytes
           .ZIP file comment        (variable size)
       */

      if (zfseeko(in_file, eocdr_offset, SEEK_SET) != 0) {
        fclose(in_file);
        in_file = NULL;
        zipwarn("unable to seek in input file ", in_path);
        return ZE_READ;
      }

      /* read the EOCDR */
      s = fread(scbuf, 1, ENDHEAD, in_file);

      /* make sure we read enough bytes */
      if (s < ENDHEAD) {
        sprintf(errbuf, "End record (EOCDR) only %s bytes - assume truncated",
                  zip_fzofft(s, NULL, "u"));
        zipwarn(errbuf, "");
      }
      else
      {
        /* the first field should be number of this (the last) disk */
        eocdr_disk = (ulg)SH(scbuf);
        total_disks = eocdr_disk + 1;

        /* assume this is this disk - if Zip64 it may not be as the
           disk number may be bigger than this field can hold
        */
        current_in_disk = total_disks - 1;

        /* Central Directory disk, offset, and total entries */
        in_cd_start_disk = (ulg)SH(scbuf + 2);
        in_cd_start_offset = (uzoff_t)LG(scbuf + 12);
        cd_total_entries = (uzoff_t)SH(scbuf + 6);

        /* the in_cd_start_disk should always be less than the total_disks,
           unless the -1 flags are being used */
        if (total_disks < 0x10000 && in_cd_start_disk > total_disks) {
          zipwarn("End record (EOCDR) has bad disk numbers - ignoring EOCDR", "");
          total_disks = 0;
        }
        else
        {
          /* length of zipfile comment */
          zcomlen = SH(scbuf + ENDCOM);
          if (zcomlen)
          {
            if ((zcomment = malloc(zcomlen + 1)) == NULL)
              return ZE_MEM;
            if (fread(zcomment, zcomlen, 1, in_file) != 1)
            {
              free((zvoid *)zcomment);
              zcomment = NULL;
              zipwarn("zipfile comment truncated - ignoring", "");
            } else {
              zcomment[zcomlen] = '\0';
            }
#ifdef EBCDIC
            if (zcomment)
               memtoebc(zcomment, zcomment, zcomlen);
#endif /* EBCDIC */
          }
        }
        if (total_disks != 1)
          sprintf(errbuf, " Found end record (EOCDR) - says expect %lu splits", total_disks);
        else
          sprintf(errbuf, " Found end record (EOCDR) - says expect single disk archive");
        zipmessage(errbuf, "");
        if (zcomment)
          zipmessage("  Found archive comment", "");
      } /* good EOCDR */

    } /* found EOCDR */

    /* if total disks is other than 1 then this is not start disk */
    /* if the EOCDR is bad, total_disks is 0 */

    /* if total_disks = 0, then guess if this is a single-disk archive
       by seeing if starts with local header */

    if (total_disks == 0) {
      int issig;
      /* seek to top */
      if (zfseeko(in_file, 0, SEEK_SET) != 0) {
        fclose(in_file);
        in_file = NULL;
        zipwarn("unable to seek in input file ", in_path);
        return ZE_READ;
      }
      /* get next signature */
      issig = find_next_signature(in_file);
      if (issig) {
        current_in_offset = zftello(in_file);
        if (current_in_offset == 4 && is_signature(sigbuf, "PK\03\03")) {
          /* could be multi-disk aborted signature at top */
          /* skip */
          issig = find_next_signature(in_file);
        } else if (current_in_offset <= 4 && is_signature(sigbuf, "PK\03\03")) {
          /* multi-disk spanning signature */
          total_disks = 99999;
        }
      }
      if (issig && total_disks == 0) {
        current_in_offset = zftello(in_file);

        if (current_in_offset == 8 && is_signature(sigbuf, "PK\03\04")) {

          /* Local Header Record at top */

          printf("Is this a single-disk archive?  (y/n): ");
          fflush(stdout);

          if (fgets(errbuf, 100, stdin) != NULL) {
            if (errbuf[0] == 'y' || errbuf[0] == 'Y') {
              total_disks = 1;
              zipmessage("  Assuming single-disk archive", "");
            }
          }
        }
      }
    }
    if (!noisy)
      /* if quiet assume single-disk archive */
      total_disks = 1;

    if (total_disks == 1000000) {
      /* still don't know, so ask */
      printf("Is this a single-disk archive?  (y/n): ");
      fflush(stdout);

      if (fgets(errbuf, 100, stdin) != NULL) {
        if (errbuf[0] == 'y' || errbuf[0] == 'Y') {
          total_disks = 1;
          zipmessage("  Assuming single-disk archive", "");
        }
      }
    }
    if (total_disks == 1000000) {
      /* assume max */
      total_disks = 100000;
    }

  } /* .zip file exists */

  /* Skip reading the Zip64 EOCDL, Zip64 EOCDR, or central directory */

  /* Now read the archive starting with first disk.  Find local headers,
     create entry in zlist, then copy entry to new archive */

  /* Multi-volume file names end in .z01, .z02, ..., .z10, .zip for 11 disk archive */

  /* Unless quiet, always close the in_path disk and ask user for first disk,
     unless there is an End Of Central Directory record and that says there is
     only one disk.
     If quiet, assume the file pointed to is a single file archive to fix. */
  if (noisy && in_file) {
    fclose(in_file);
    in_file = NULL;
  }

  /* Read the archive disks - no idea how many disks there are
     since we can't trust the EOCDR and other end records
   */
  zipmessage("Scanning for entries...", "");

  for (current_in_disk = 0; current_in_disk < total_disks; current_in_disk++) {
    /* get the path for this disk */
    split_path = get_in_split_path(in_path, current_in_disk);

    /* if in_file is not NULL then in_file is already open */
    if (in_file == NULL) {
      /* open the split */
      while ((in_file = zfopen(split_path, FOPR)) == NULL) {
        int result;
        /* could not open split */

        /* Ask for directory with split.  Updates global variable in_path */
        result = ask_for_split_read_path(current_in_disk);
        if (result == ZE_ABORT) {
          zipwarn("could not find split: ", split_path);
          return ZE_ABORT;
        } else if (result == ZE_EOF) {
          zipmessage_nl("", 1);
          zipwarn("user ended reading - closing archive", "");
          return ZE_EOF;
        } else if (result == ZE_FORM) {
          /* user asked to skip this disk */
          zipmessage_nl("", 1);
          sprintf(errbuf, "skipping disk %lu ...\n", current_in_disk);
          zipwarn(errbuf, "");
          skip_disk = 1;
          break;
        }

        split_path = get_in_split_path(in_path, current_in_disk);
      }
      if (skip_disk) {
        /* skip this current disk - this works because central directory entries
           can't be split across splits */
        skip_disk = 0;
        skipped_disk = 1;
        continue;
      }
    }

    if (skipped_disk) {
      /* Not much to do here as between entries.  Entries are copied
         in zipcopy() and that has to handle missing disks while
         reading data for an entry.
       */
    }

    /* Main loop */
    /* Look for next signature and process it */
    while (find_next_signature(in_file)) {
      current_in_offset = zftello(in_file);

      if (is_signature(sigbuf, "PK\05\06")) {

        /* End Of Central Directory Record */

        sprintf(errbuf, "EOCDR found (%2lu %6s)...",
                current_in_disk + 1, zip_fzofft(current_in_offset - 4, NULL, "u"));
        zipmessage_nl(errbuf, 1);


      } else if (is_signature(sigbuf, "PK\06\06")) {

        /* Zip64 End Of Central Directory Record */

        sprintf(errbuf, "Zip64 EOCDR found (%2lu %6s)...",
                current_in_disk + 1, zip_fzofft(current_in_offset - 4, NULL, "u"));
        zipmessage_nl(errbuf, 1);


      } else if (is_signature(sigbuf, "PK\06\07")) {

        /* Zip64 End Of Central Directory Locator */

        sprintf(errbuf, "Zip64 EOCDL found (%2lu %6s)...",
                current_in_disk + 1, zip_fzofft(current_in_offset - 4, NULL, "u"));
        zipmessage_nl(errbuf, 1);


      } else if (is_signature(sigbuf, "PK\03\04")) {

        /* Local Header Record */


        if (verbose) {
          sprintf(errbuf, " Local (%2lu %6s):",
                  current_in_disk + 1, zip_fzofft(current_in_offset - 4, NULL, "u"));
          zipmessage_nl(errbuf, 0);
        }

        /* Create zlist entry.  Most will be filled in by zipcopy(). */

        if ((z = (struct zlist far *)farmalloc(sizeof(struct zlist))) == NULL) {
          zipwarn("reading central directory", "");
          return ZE_MEM;
        }

        z->vem = 0;
        z->ver = 0;
        z->flg = 0;
        z->how = 0;
        z->tim = 0;          /* time and date into one long */
        z->crc = 0;
        z->siz = 0;
        z->len = 0;
        z->nam = 0;          /* used before comparing cen vs. loc */
        z->cext = 0;         /* may be different from z->ext */
        z->com = 0;
        z->dsk = 0;
        z->att = 0;
        z->atx = 0;
        z->off = 0;
        z->dosflag = 0;

        /* Initialize all fields pointing to malloced data to NULL */
        z->zname = z->name = z->iname = z->extra = z->cextra = z->comment = NULL;
        z->oname = NULL;
#ifdef UNICODE_SUPPORT
        z->uname = z->zuname = z->ouname = NULL;
#endif

        /* Attempt to copy entry */

        r = zipcopy(z);

        if (in_central_directory) {
          sprintf(errbuf, "Entry after central directory found (%2lu %6s)...",
                  current_in_disk + 1, zip_fzofft(current_in_offset - 4, NULL, "u"));
          zipmessage_nl(errbuf, 1);
          in_central_directory = 0;
        }

        if (r == ZE_EOF)
          /* user said no more splits */
          break;
        else if (r == ZE_OK) {
          zcount++;
          files_total++;
          bytes_total += z->siz;

          /* Link into list */
          if (zfiles == NULL)
            /* first link */
            x = &zfiles;
          /* Link into list */
          *x = z;
          z->nxt = NULL;
          x = &z->nxt;
        }

      } else if (is_signature(sigbuf, "PK\01\02")) {

        /* Central directory header */


        /* sort the zlist */
        if (in_central_directory == 0) {
          zipmessage("Central Directory found...", "");
          /* If one or more files, sort by name */
          if (zcount)
          {
            struct zlist far * far *x;    /* pointer into zsort array */
            struct zlist far *z;          /* pointer into zfiles linked list */
            int i = 0;
            extent zl_size = zcount * sizeof(struct zlist far *);

            if (zl_size / sizeof(struct zlist far *) != zcount ||
                (x = zsort = (struct zlist far **)malloc(zl_size)) == NULL)
              return ZE_MEM;
            for (z = zfiles; z != NULL; z = z->nxt)
              x[i++] = z;
            qsort((char *)zsort, zcount, sizeof(struct zlist far *), zqcmp);

            /* Skip Unicode searching */
          }
        }

        if (verbose) {
          sprintf(errbuf, " Cen   (%2lu %6s): ",
                  current_in_disk + 1, zip_fzofft(current_in_offset - 4, NULL, "u"));
          zipmessage_nl(errbuf, 0);
        }

        in_central_directory = 1;

        /* Read central directory entry */

        /* central directory signature */

        /* The format of a central directory record
          central file header signature   4 bytes  (0x02014b50)
          version made by                 2 bytes
          version needed to extract       2 bytes
          general purpose bit flag        2 bytes
          compression method              2 bytes
          last mod file time              2 bytes
          last mod file date              2 bytes
          crc-32                          4 bytes
          compressed size                 4 bytes
          uncompressed size               4 bytes
          file name length                2 bytes
          extra field length              2 bytes
          file comment length             2 bytes
          disk number start               2 bytes
          internal file attributes        2 bytes
          external file attributes        4 bytes
          relative offset of local header 4 bytes

          file name (variable size)
          extra field (variable size)
          file comment (variable size)
         */

        if (fread(scbuf, CENHEAD, 1, in_file) != 1) {
          zipwarn("reading central directory: ", strerror(errno));
          zipwarn("bad archive - error reading central directory", "");
          zipwarn("skipping this entry...", "");
          continue;
        }

        if ((cz = (struct zlist far *)farmalloc(sizeof(struct zlist))) == NULL) {
          zipwarn("reading central directory", "");
          return ZE_MEM;
        }

        cz->vem = SH(CENVEM + scbuf);
        cz->ver = SH(CENVER + scbuf);
        cz->flg = SH(CENFLG + scbuf);
        cz->how = SH(CENHOW + scbuf);
        cz->tim = LG(CENTIM + scbuf);   /* time and date into one long */
        cz->crc = LG(CENCRC + scbuf);
        cz->siz = LG(CENSIZ + scbuf);
        cz->len = LG(CENLEN + scbuf);
        cz->nam = SH(CENNAM + scbuf);   /* used before comparing cen vs. loc */
        cz->cext = SH(CENEXT + scbuf);  /* may be different from z->ext */
        cz->com = SH(CENCOM + scbuf);
        cz->dsk = SH(CENDSK + scbuf);
        cz->att = SH(CENATT + scbuf);
        cz->atx = LG(CENATX + scbuf);
        cz->off = LG(CENOFF + scbuf);
        cz->dosflag = (cz->vem & 0xff00) == 0;

        /* Initialize all fields pointing to malloced data to NULL */
        cz->zname = cz->name = cz->iname = cz->extra = cz->cextra = NULL;
        cz->comment = cz->oname = NULL;
#ifdef UNICODE_SUPPORT
        cz->uname = cz->zuname = cz->ouname = NULL;
#endif

        /* Read file name, extra field and comment field */
        if (cz->nam == 0)
        {
          sprintf(errbuf, "%lu", (ulg)zcount + 1);
          zipwarn("zero-length name for entry #", errbuf);
          zipwarn("skipping this entry...", "");
          continue;
        }
        if ((cz->iname = malloc(cz->nam+1)) ==  NULL ||
            (cz->cext && (cz->cextra = malloc(cz->cext + 1)) == NULL) ||
            (cz->com && (cz->comment = malloc(cz->com + 1)) == NULL))
          return ZE_MEM;
        if (fread(cz->iname, cz->nam, 1, in_file) != 1 ||
            (cz->cext && fread(cz->cextra, cz->cext, 1, in_file) != 1) ||
            (cz->com && fread(cz->comment, cz->com, 1, in_file) != 1)) {
          zipwarn("error reading entry:  ", strerror(errno));
          zipwarn("skipping this entry...", "");
          continue;
        }
        cz->iname[cz->nam] = '\0';                  /* terminate name */

        /* Look up this name in zlist from local entries */
        z = zsearch(cz->iname);


        if (z && z->tim == cz->tim) {

          /* Apparently as iname and date and time match this central
             directory entry goes with this zlist entry */

          if (verbose) {
            /* cen dir name matches a local name */
            sprintf(errbuf, "updating: %s", cz->iname);
            zipmessage_nl(errbuf, 0);
          }

          if (z->crc != cz->crc) {
            sprintf(errbuf, "local (%lu) and cen (%lu) crc mismatch", z->crc, cz->crc);
            zipwarn(errbuf, "");
          }

          z->vem = cz->vem;
         /* z->ver = cz->ver; */
         /* z->flg = cz->flg; */
         /* z->how = cz->how; */
         /* z->tim = cz->tim; */          /* time and date into one long */
         /* z->crc = cz->crc; */
         /* z->siz = cz->siz; */
         /* z->len = cz->len; */
         /* z->nam = cz->nam; */          /* used before comparing cen vs. loc */
          z->cext = cz->cext;             /* may be different from z->ext */
          z->com = cz->com;
          z->cextra = cz->cextra;
          z->comment = cz->comment;
         /* z->dsk = cz->dsk; */
          z->att = cz->att;
          z->atx = cz->atx;
         /* z->off = cz->off; */
          z->dosflag = cz->dosflag;

#ifdef UNICODE_SUPPORT
          if (unicode_mismatch != 3 && z->uname == NULL) {
            if (z->flg & UTF8_BIT) {
              /* path is UTF-8 */
              if ((z->uname = malloc(strlen(z->iname) + 1)) == NULL) {
                ZIPERR(ZE_MEM, "reading archive");
              }
              strcpy(z->uname, z->iname);
            } else {
              /* check for UTF-8 path extra field */
              read_Unicode_Path_entry(z);
            }
          }
#endif

#ifdef WIN32
          /* Input path may be OEM */
          {
            unsigned hostver = (z->vem & 0xff);
            Ext_ASCII_TO_Native(z->iname, (z->vem >> 8), hostver,
                                ((z->atx & 0xffff0000L) != 0), FALSE);
          }
#endif

#ifdef EBCDIC
          if (z->com)
             memtoebc(z->comment, z->comment, z->com);
#endif /* EBCDIC */
#ifdef WIN32
          /* Comment may be OEM */
          {
            unsigned hostver = (z->vem & 0xff);
            Ext_ASCII_TO_Native(z->comment, (z->vem >> 8), hostver,
                                ((z->atx & 0xffff0000L) != 0), FALSE);
          }
#endif

#ifdef ZIP64_SUPPORT
          /* zip64 support 08/31/2003 R.Nausedat                          */
          /* here, we have to read the len, siz etc values from the CD    */
          /* entry as we might have to adjust them regarding their        */
          /* correspronding zip64 extra fields.                           */
          /* also, we cannot compare the values from the CD entries with  */
          /* the values from the LH as they might be different.           */

          /* adjust/update siz,len and off (to come: dsk) entries */
          /* PKZIP does not care of the version set in a CDH: if  */
          /* there is a zip64 extra field assigned to a CDH PKZIP */
          /* uses it, we should do so, too.                       */
  /*
          adjust_zip_central_entry(z);
   */
#endif

        /* Update zipbeg beginning of archive offset, prepare for next header */
/*
          if (z->dsk == 0 && (!zipbegset || z->off < zipbeg)) {
            zipbeg = z->off;
            zipbegset = 1;
          }
          zcount++;
 */

#ifndef UTIL
          if (verbose)
            zipoddities(z);
#endif

          current_offset = zftello(y);

          if (zfseeko(y, z->off, SEEK_SET) != 0) {
            fclose(in_file);
            in_file = NULL;
            zipwarn("writing archive seek: ", strerror(errno));
            return ZE_WRITE;
          }

          if (putlocal(z, PUTLOCAL_REWRITE) != ZE_OK)
            zipwarn("Error rewriting local header", "");

          if (zfseeko(y, current_offset, SEEK_SET) != 0) {
            fclose(in_file);
            in_file = NULL;
            zipwarn("write archive seek: ", strerror(errno));
            return ZE_WRITE;
          }
          offset = zftello(y);
          if (current_offset != offset) {
            fclose(in_file);
            in_file = NULL;
            zipwarn("seek after local: ", strerror(errno));
            return ZE_WRITE;
          }

          if (verbose)
            zipmessage_nl("", 1);

        } else {
          /* cen dir name does not match local name */
          sprintf(errbuf, "no local entry: %s", cz->iname);
          zipmessage_nl(errbuf, 1);
        }

      } else if (zfiles == NULL && is_signature(sigbuf, "PK\07\010")) {

        /* assume spanning signature at top of archive */
        if (total_disks == 1) {
          zipmessage("  Found spanning marker, but did not expect split (multi-disk) archive...", "");

        } else if (total_disks > 1) {
          zipmessage("  Found spanning marker - expected as this is split (multi-disk) archive...", "");

        } else {
          zipmessage("  Found spanning marker - could be split archive...", "");

        }

      } else {

        /* this signature shouldn't be here */
        int c;
        char errbuftemp[40];

        strcpy(errbuf, "unexpected signature ");
        for (c = 0; c < 4; c++) {
          sprintf(errbuftemp, "%02x ", sigbuf[c]);
          strcat(errbuf, errbuftemp);
        }
        sprintf(errbuftemp, "on disk %lu at %s\n", current_in_disk,
                                 zip_fzofft(current_in_offset - 4, NULL, "u"));
        strcat(errbuf, errbuftemp);
        zipwarn(errbuf, "");
        zipwarn("skipping this signature...", "");
      }


    } /* while reading file */

    /* close disk and do next disk */
    if (in_file)
      fclose(in_file);
    in_file = NULL;
    free(split_path);

    if (r == ZE_EOF)
      /* user says no more splits */
      break;

  } /* for each disk */

  return ZE_OK;

} /* end of function scanzipf_fixnew() */

#endif /* !UTIL */






/* ---------------------- */
/* New regular scan       */

/*
 * scanzipf_regnew is similar to the orignal scanzipf_reg in that it
 * reads the end of the archive and goes from there.  Unlike that
 * scan this one stops after reading the central directory and does
 * not read the local headers.  After the directory scan for new
 * files is done in zip.c the zlist created here is used to read
 * the old archive entries there.  The local headers are read using
 * readlocal() in zipcopy().
 *
 * This scan assumes the zip file is well structured.  If not it may
 * fail and the new scanzipf_fixnew should be used.
 *
 * 2006-2-4, 2007-12-10 EG
 */

local int scanzipf_regnew()
/*
   The input path for the .zip file is in in_path.  If a split archive,
   the path for each split is created from the current disk number
   and in_path.  If a split is not in the same directory as the last
   split we ask the user where it is and update in_path.
 */
/*
   This is old but more or less still applies:

   The name of the zip file is pointed to by the global "zipfile".  The globals
   zipbeg, cenbeg, zfiles, zcount, zcomlen, zcomment, and zsort are filled in.
   Return an error code in the ZE_ class.
*/
{
  /* In this function, a local buffer is used to read in the following Zip
     structures:
      End-of-CentralDir record (EOCDR) (ENDHEAD)
      Zip64-End-of-CentralDir-Record locator (Zip64 EOCDL) (EC64LOC)
      Zip64-End-of-CentralDir record (Zip64 EOCDR) (EC64REC)
      CentralDir-Entry record (CENHEAD)
     To conserve valuable stack space, this buffer is sized to the largest
     of these structures.
   */
# if CENHEAD > ENDHEAD
#   define SCAN_BUFSIZE CENHEAD   /* CENHEAD should be the larger struct */
# else
#   define SCAN_BUFSIZE ENDHEAD
# endif

#ifdef ZIP64_SUPPORT
# if EC64REC > SCAN_BUFSIZE
#   undef SCAN_BUFSIZE
#   define SCAN_BUFSIZE EC64REC   /* EC64 record should be largest struct */
# endif
# if EC64LOC > SCAN_BUFSIZE
#   undef SCAN_BUFSIZE
#   define SCAN_BUFSIZE EC64LOC
# endif
#endif

  char    scbuf[SCAN_BUFSIZE];  /* buffer just enough for all header types */
  char   *split_path;
  ulg     eocdr_disk;
  uzoff_t eocdr_offset;
# ifdef ZIP64_SUPPORT
  ulg     z64eocdr_disk;
  uzoff_t z64eocdr_offset;
  uzoff_t z64eocdr_size;
  ush     version_made;
  ush     version_needed = 0;
  zoff_t zip64_eocdr_start;
  zoff_t z64eocdl_offset;
# endif /* def ZIP64_SUPPORT */
  uzoff_t cd_total_entries;        /* num of entries as read from (Zip64) EOCDR */
  ulg     in_cd_start_disk;        /* central directory start disk */
  uzoff_t in_cd_start_offset;      /* offset of start of cd on cd start disk */
  uzoff_t adjust_offset = 0;       /* bytes before first entry (size of sfx prefix) */
  uzoff_t cd_total_size = 0;       /* total size of cd */


  int first_CD = 1;           /* looking for first CD entry */
  int zipbegset = 0;

  int skip_disk = 0;          /* 1 if user asks to skip current disk */
  int skipped_disk = 0;       /* 1 if skipped start disk and start offset is useless */

  uzoff_t s;                  /* size of data, start of central */
  struct zlist far * far *x;  /* pointer last entry's link */
  struct zlist far *z;        /* current zip entry structure */


  /* open the zipfile */
  if ((in_file = zfopen(in_path, FOPR)) == NULL) {
    zipwarn("could not open input archive", in_path);
    return ZE_OPEN;
  }

#ifndef ZIP64_SUPPORT
  /* 2004-12-06 SMS.
   * Check for too-big file before doing any serious work.
   */
  if (ffile_size( in_file) == EOF) {
    fclose(in_file);
    in_file = NULL;
    zipwarn("input file requires Zip64 support: ", in_path);
    return ZE_ZIP64;
  }
#endif /* ndef ZIP64_SUPPORT */

  /* look for End Of Central Directory Record */

  /* In a valid Zip archive, the EOCDR can be at most (64k-1 + ENDHEAD + 4)
     bytes (=65557 bytes) from the end of the file.
     We back up 128k, to allow some junk being appended to a Zip file.
   */
  if ((zfseeko(in_file, -0x20000L, SEEK_END) != 0) ||
      /* Some fseek() implementations (e.g. MSC 8.0 16-bit) fail to signal
         an error when seeking before the beginning of the file.
         As work-around, we check the position returned by zftello()
         for the error value -1.
       */
      (zftello(in_file) == (zoff_t)-1L)) {
    /* file is less than 128 KB so back up to beginning */
    if (zfseeko(in_file, 0L, SEEK_SET) != 0) {
      fclose(in_file);
      in_file = NULL;
      zipwarn("unable to seek in input file ", in_path);
      return ZE_READ;
    }
  }

  /* find EOCD Record signature */
  if (!find_signature(in_file, "PK\05\06")) {
    /* No End Of Central Directory Record */
    fclose(in_file);
    in_file = NULL;
    if (fix == 1) {
      zipwarn("bad archive - missing end signature", "");
      zipwarn("(If downloaded, was binary mode used?  If not, the", "");
      zipwarn(" archive may be scrambled and not recoverable)", "");
      zipwarn("Can't use -F to fix (try -FF)", "");
    } else{
      zipwarn("missing end signature--probably not a zip file (did you", "");
      zipwarn("remember to use binary mode when you transferred it?)", "");
      zipwarn("(if you are trying to read a damaged archive try -F)", "");
    }
    return ZE_FORM;
  }

  /* at start of data after EOCDR signature */
  eocdr_offset = (uzoff_t) zftello(in_file);

  /* OK, it is possible this is not the last EOCDR signature (might be
     EOCDR signature from a stored archive in the last 128 KB) and so not
     the one we want.

     The below assumes the signature does not appear in the assumed ASCII text
     .ZIP file comment.
  */
  while (find_signature(in_file, "PK\05\06")) {
    /* previous one was not the one */
    eocdr_offset = (uzoff_t) zftello(in_file);
  }

  /* found EOCDR */
  /* format is
       end of central dir signature     4 bytes  (0x06054b50)
       number of this disk              2 bytes
       number of the disk with the
        start of the central directory  2 bytes
       total number of entries in the
        central directory on this disk  2 bytes
       total number of entries in
        the central directory           2 bytes
       size of the central directory    4 bytes
       offset of start of central
        directory with respect to
        the starting disk number        4 bytes
       .ZIP file comment length         2 bytes
       .ZIP file comment        (variable size)
   */

  if (zfseeko(in_file, eocdr_offset, SEEK_SET) != 0) {
    fclose(in_file);
    in_file = NULL;
    zipwarn("unable to seek in input file ", in_path);
    return ZE_READ;
  }

  /* read the EOCDR */
  s = fread(scbuf, 1, ENDHEAD, in_file);

  /* the first field should be number of this (the last) disk */
  eocdr_disk = (ulg)SH(scbuf);
  total_disks = eocdr_disk + 1;

  /* Assume EOCDR disk is this disk.  If a lot of disks, the Zip64 field
     may be needed and this EOCDR field could be set to the Zip64 flag
     value as the disk number may be bigger than this field can hold.
  */
  current_in_disk = total_disks - 1;

  /* Central Directory disk, offset, and total entries */
  in_cd_start_disk = (ulg)SH(scbuf + ENDBEG);
  in_cd_start_offset = (uzoff_t)LG(scbuf + ENDOFF);
  cd_total_entries = (uzoff_t)SH(scbuf + ENDTOT);
  cd_total_size = (uzoff_t)LG(scbuf + ENDSIZ);

  /* length of zipfile comment */
  zcomlen = SH(scbuf + ENDCOM);
  if (zcomlen)
  {
    if ((zcomment = malloc(zcomlen + 1)) == NULL)
      return ZE_MEM;
    if (fread(zcomment, zcomlen, 1, in_file) != 1)
    {
      free((zvoid *)zcomment);
      zcomment = NULL;
      return ferror(in_file) ? ZE_READ : ZE_EOF;
    }
    zcomment[zcomlen] = '\0';
#ifdef EBCDIC
    if (zcomment)
       memtoebc(zcomment, zcomment, zcomlen);
#endif /* EBCDIC */
  }

  if (cd_total_entries == 0) {
    /* empty archive */

    fclose(in_file);
    in_file = NULL;
    return ZE_OK;
  }

  /* if total disks is other than 1 then multi-disk archive */
  if (total_disks != 1) {
    /* zipfile name must end in .zip for split archives */
    int plen = strlen(in_path);
    char *in_path_ext;

    if (adjust) {
      zipwarn("Adjusting split archives not yet supported", "");
      return ZE_FORM;
    }

#ifdef VMS
    /* On VMS, adjust plen (and in_path_ext) to avoid the file version. */
    plen -= strlen(vms_file_version(in_path));
#endif /* def VMS */
    in_path_ext = zipfile + plen - 4;

    if (plen < 4 ||
        in_path_ext[0] != '.' ||
        toupper(in_path_ext[1]) != 'Z' ||
        toupper(in_path_ext[2]) != 'I' ||
        toupper(in_path_ext[3]) != 'P') {
      zipwarn("archive name must end in .zip for splits", "");
      fclose(in_file);
      in_file = NULL;
      return ZE_PARMS;
    }
  }

  /* if input or output are split archives, must be different archives */
  if ((total_disks != 1 || split_method) && !show_files &&
      strcmp(in_path, out_path) == 0) {
    fclose(in_file);
    in_file = NULL;
    zipwarn("cannot update a split archive (use --out option)", "");
    return ZE_PARMS;
  }

  /* if fixing archive, input and output must be different archives */
  if (fix == 1 && strcmp(in_path, out_path) == 0) {
    fclose(in_file);
    in_file = NULL;
    zipwarn("must use --out when fixing an archive", "");
    return ZE_PARMS;
  }


  /* Get sfx offset if adjusting. Above we made sure not split archive. */
  /* Also check for an offset if fix and single disk archive. */
  if ((fix == 1 && total_disks == 1) || adjust) {
    zoff_t cd_start;
# ifdef ZIP64_SUPPORT
    zoff_t zip64_eocdr_start;
# endif

    /* First attempt.  If the CD start offset and size are valid in the EOCDR
       (meaning they are not the Zip64 flag values that say the actual values
       are in the Zip64 EOCDR), we can use them to get the offset */
    if (in_cd_start_offset != 0xFFFFFFFF && cd_total_size != 0xFFFFFFFF) {
      /* Search for start of central directory */
      /* There still might be a Zip64 EOCDR.  This assumes if there is
         a Zip64 EOCDR, it's version 1 and 52 bytes */
      cd_start = eocdr_offset - cd_total_size - 24 - 56;
      if (zfseeko(in_file, cd_start, SEEK_SET) != 0) {
        fclose(in_file);
        in_file = NULL;
        if (fix == 1) {
          zipwarn("could not seek back to start of central directory: ", strerror(errno));
          zipwarn("(try -FF)", "");
        } else {
          zipwarn("reading archive fseek: ", strerror(errno));
        }
        return ZE_FORM;
      }
      if (find_signature(in_file, "PK\01\02")) {
        /* Should now be after first central directory header signature in archive */
        adjust_offset = zftello(in_file) - 4 - in_cd_start_offset;
      } else {
        zipwarn("central dir not where expected - could not adjust offsets", "");
        zipwarn("(try -FF)", "");
        return ZE_FORM;
      }
    } else {

      /* Second attempt.  We need the Zip64 EOCDL to get the offset */

      /*
       * Check for a Zip64 EOCD Locator signature
       */

      /* Format of Z64EOCD Locator is
           zip64 end of central dir locator
            signature                       4 bytes  (0x07064b50)
           number of the disk with the
            start of the zip64 end of
            central directory               4 bytes
           relative offset of the zip64
            end of central directory record 8 bytes
           total number of disks            4 bytes
       */

      /* back up 20 bytes from EOCDR to Z64 EOCDL */
      if (zfseeko(in_file, eocdr_offset - 24, SEEK_SET) != 0) {
        fclose(in_file);
        in_file = NULL;
        if (fix == 1) {
          zipwarn("could not seek back to Zip64 EOCDL: ", strerror(errno));
          zipwarn("(try -FF)", "");
        } else {
          zipwarn("reading archive fseek: ", strerror(errno));
        }
        return ZE_FORM;
      }
      if (at_signature(in_file, "PK\06\07"))
#ifndef ZIP64_SUPPORT
      {
        fclose(in_file);
        in_file = NULL;
        zipwarn("found Zip64 signature - this may be a Zip64 archive", "");
        zipwarn("Need PKZIP 4.5 or later compatible zip", "");
        zipwarn("Set ZIP64_SUPPORT in Zip 3", "");
        return ZE_ZIP64;
      }
#else /* ZIP64_SUPPORT */
      {
        z64eocdl_offset = zftello(in_file) - 4;

        /* read Z64 EOCDL */
        if (fread(scbuf, EC64LOC, 1, in_file) != 1) {
          fclose(in_file);
          in_file = NULL;
          zipwarn("reading archive: ", strerror(errno));
          return ZE_READ;
        }
        /* now should be back at the EOCD signature */
        if (!at_signature(in_file, "PK\05\06")) {
          fclose(in_file);
          in_file = NULL;
          zipwarn("unable to read EOCD after seek: ", in_path);
          return ZE_READ;
        }

        /* read disk and offset to Zip64 EOCDR and total disks */
        z64eocdr_disk = LG(scbuf);
        z64eocdr_offset = LLG(scbuf + 4);
        total_disks = LG(scbuf + 12);

        /* For now no split archives */
        if (total_disks != 1) {
          zipwarn("Adjusting split archives not supported:  ", in_path);
          zipwarn("(try -FF)", "");
          return ZE_FORM;
        }

        /* go to the Zip64 EOCDR */
        if (zfseeko(in_file, z64eocdr_offset, SEEK_SET) != 0) {
          fclose(in_file);
          in_file = NULL;
          zipwarn("reading archive fseek: ", strerror(errno));
          return ZE_FORM;
        }
        /* Should be at Zip64 EOCDR signature */
        if (at_signature(in_file, "PK\06\06")) {
          /* apparently no offset */

        } else {
          /* Wasn't there, so calculate based on Zip64 EOCDL offset */

          zip64_eocdr_start = z64eocdl_offset - 24 - 56;
          if (zfseeko(in_file, zip64_eocdr_start, SEEK_SET) != 0) {
            fclose(in_file);
            in_file = NULL;
            if (fix == 1) {
              zipwarn("could not seek back to Zip64 EOCDR: ", strerror(errno));
              zipwarn("(try -FF)", "");
            } else {
              zipwarn("reading archive fseek: ", strerror(errno));
            }
            return ZE_FORM;
          }
          if (find_next_signature(in_file) && is_signature(sigbuf, "PK\06\06")) {
            /* Should now be after Zip64 EOCDR signature in archive */
            adjust_offset = zftello(in_file) - 4 - z64eocdr_offset;
          } else {
            zipwarn("Could not determine offset of entries", "");
            zipwarn("(try -FF)", "");
            return ZE_FORM;
          }
        }
      }
#endif
    }
    if (noisy) {
      if (adjust_offset) {
        sprintf(errbuf, "Zip entry offsets appear off by %s bytes - correcting...",
                        zip_fzofft(adjust_offset, NULL, NULL));
      } else {
        sprintf(errbuf, "Zip entry offsets do not need adjusting");
      }
      zipmessage(errbuf, "");
    }
  }


  /*
   * Check for a Zip64 EOCD Locator signature
   */

  /* Format of Z64EOCD Locator is
       zip64 end of central dir locator
        signature                       4 bytes  (0x07064b50)
       number of the disk with the
        start of the zip64 end of
        central directory               4 bytes
       relative offset of the zip64
        end of central directory record 8 bytes
       total number of disks            4 bytes
   */

  /* back up 20 bytes from EOCDR to Z64 EOCDL */
  if (zfseeko(in_file, eocdr_offset - 24, SEEK_SET) != 0) {
    fclose(in_file);
    in_file = NULL;
    if (fix == 1) {
      zipwarn("bad archive - could not seek back to Zip64 EOCDL: ", strerror(errno));
      zipwarn("(try -FF)", "");
    } else {
      zipwarn("reading archive fseek: ", strerror(errno));
    }
    return ZE_FORM;
  }
  if (at_signature(in_file, "PK\06\07"))
#ifndef ZIP64_SUPPORT
  {
    fclose(in_file);
    in_file = NULL;
    zipwarn("found Zip64 signature - this may be a Zip64 archive", "");
    zipwarn("Need PKZIP 4.5 or later compatible zip", "");
    zipwarn("Set ZIP64_SUPPORT in Zip 3", "");
    return ZE_ZIP64;
  }
#else /* ZIP64_SUPPORT */
  {
    z64eocdl_offset = zftello(in_file) - 4;
    /* read Z64 EOCDL */
    if (fread(scbuf, EC64LOC, 1, in_file) != 1) {
      fclose(in_file);
      in_file = NULL;
      zipwarn("reading archive: ", strerror(errno));
      return ZE_READ;
    }
    /* now should be back at the EOCD signature */
    if (!at_signature(in_file, "PK\05\06")) {
      fclose(in_file);
      in_file = NULL;
      zipwarn("unable to read EOCD after seek: ", in_path);
      return ZE_READ;
    }

    /* read disk and offset to Zip64 EOCDR and total disks */
    z64eocdr_disk = LG(scbuf);
    z64eocdr_offset = LLG(scbuf + 4) + adjust_offset;
    total_disks = LG(scbuf + 12);

    /* set the current disk */
    current_in_disk = total_disks - 1;

    /* Now need to read the Zip64 EOCD Record to get version needed
       to extract */

    if (z64eocdr_disk != total_disks - 1) {
      /* Zip64 EOCDR not on this disk */

      /* done with this disk (since apparently there are no CD entries
         on it) */
      fclose(in_file);
      in_file = NULL;

      /* get the path for the disk with the Zip64 EOCDR */
      split_path = get_in_split_path(in_path, z64eocdr_disk);

      while ((in_file = zfopen(split_path, FOPR)) == NULL) {
        /* could not open split */

        /* Ask where this split is.  This call also updates global in_path. */
        if (ask_for_split_read_path(z64eocdr_disk) != ZE_OK) {
          return ZE_ABORT;
        }
        free(split_path);
        split_path = get_in_split_path(in_path, z64eocdr_disk);
      }
      free(split_path);
    }

    current_in_disk = z64eocdr_disk;

    /* go to the Zip64 EOCDR */
    if (zfseeko(in_file, z64eocdr_offset, SEEK_SET) != 0) {
      fclose(in_file);
      in_file = NULL;
      zipwarn("reading archive fseek: ", strerror(errno));
      return ZE_FORM;
    }
    /* Should be at Zip64 EOCDR signature */
    if (!at_signature(in_file, "PK\06\06")) {
      /* Wasn't there, so calculate based on Zip64 EOCDL offset */
      zip64_eocdr_start = z64eocdl_offset - 24 - 56;
      if (zfseeko(in_file, zip64_eocdr_start, SEEK_SET) != 0) {
        fclose(in_file);
        in_file = NULL;
        if (fix == 1) {
          zipwarn("bad archive - could not seek back to Zip64 EOCDR: ", strerror(errno));
          zipwarn("(try -FF)", "");
        } else {
          zipwarn("reading archive fseek: ", strerror(errno));
        }
        return ZE_FORM;
      }
      if (find_next_signature(in_file) && is_signature(sigbuf, "PK\06\06")) {
        /* Should now be after Zip64 EOCDR signature in archive */
        adjust_offset = zftello(in_file) - 4 - z64eocdr_offset;
        zipwarn("Zip64 EOCDR not found where expected - compensating", "");
        zipwarn("(try -A to adjust offsets)", "");
      } else {
        fclose(in_file);
        in_file = NULL;
        if (fix == 1) {
          zipwarn("bad archive - Zip64 EOCDR not found in split:  ", in_path);
          zipwarn("(try -FF)", "");
        } else {
          zipwarn("Zip64 End Of Central Directory Record not found:  ", in_path);
        }
        return ZE_FORM;
      }
    }

    /*
     * Read the Z64 End Of Central Directory Record
     */

    /* The format of the Z64 EOCDR is
        zip64 end of central dir
         signature                       4 bytes  (0x06064b50)
        size of zip64 end of central
         directory record                8 bytes
        version made by                  2 bytes
        version needed to extract        2 bytes
        number of this disk              4 bytes
        number of the disk with the
         start of the central directory  4 bytes
        total number of entries in the
         central directory on this disk  8 bytes
        total number of entries in the
         central directory               8 bytes
        size of the central directory    8 bytes
        offset of start of central
         directory with respect to
         the starting disk number        8 bytes
        (version 2 of the Zip64 EOCDR has more after this)
        zip64 extensible data sector    (variable size)
     */

    /* read the first 52 bytes of the Zip64 EOCDR (we don't support
       version 2, which supports PKZip licensed features)
    */
    s = fread(scbuf, 1, EC64REC, in_file);
    if (s < EC64REC) {
      if (fix == 1) {
        zipwarn("bad archive - Zip64 EOCDR bad or truncated", "");
        zipwarn("(try -FF)", "");
      } else {
        zipwarn("Zip64 EOCD Record bad or truncated", "");
      }
      fclose(in_file);
      in_file = NULL;
      return ZE_FORM;
    }
    z64eocdr_size = LLG(scbuf);
    version_made = SH(scbuf + 8);
    version_needed = SH(scbuf + 10);
    in_cd_start_disk = LG(scbuf + 16);
    cd_total_entries = LLG(scbuf + 28);
    in_cd_start_offset = LLG(scbuf + 44) + adjust_offset;

    if (version_needed > 46) {
      int major = version_needed / 10;
      int minor = version_needed - (major * 10);
      sprintf(errbuf, "This archive requires version %d.%d", major, minor);
      zipwarn(errbuf, "");
      zipwarn("Zip currently only supports up to version 4.6 archives", "");
      zipwarn("(up to 4.5 if bzip2 is not compiled in)", "");
      if (fix == 1)
        zipwarn("If -F fails try -FF to try to salvage something", "");
      else if (fix == 2)
        zipwarn("Attempting to salvage what can", "");
      else {
        zipwarn("Try -F to attempt to read anyway", "");
        fclose(in_file);
        in_file = NULL;
        return ZE_FORM;
      }
    }
  }
#endif /* ?ZIP64_SUPPORT */

  /* Now read the central directory and create the zlist */

  /* Multi-volume file names end in .z01, .z02, ..., .z10, .zip for 11 disk archive */

  in_cd_start_offset += adjust_offset;
  cenbeg = in_cd_start_offset;
  zipbegset = 0;
  zipbeg = 0;
  first_CD = 1;

  /* if the central directory starts on other than this disk, close this disk */
  if (current_in_disk != in_cd_start_disk) {
    /* close current disk */
    fclose(in_file);
    in_file = NULL;
  }

  /* Read the disks with the central directory in order - usually the
     central directory fits on the last disk, but it doesn't have to.
   */
  for (current_in_disk = in_cd_start_disk;
       current_in_disk < total_disks;
       current_in_disk++) {
    /* get the path for this disk */
    if (current_in_disk == total_disks - 1) {
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

    /* if in_file is not NULL then in_file is already open */
    if (in_file == NULL) {
      /* open the split */
      while ((in_file = zfopen(split_path, FOPR)) == NULL) {
        int result;
        /* could not open split */

        /* Ask for directory with split.  Updates global variable in_path */
        result = ask_for_split_read_path(current_in_disk);
        if (result == ZE_ABORT) {
          zipwarn("could not find split: ", split_path);
          return ZE_ABORT;
        } else if (result == ZE_FORM) {
          /* user asked to skip this disk */
          sprintf(errbuf, "skipping disk %lu ...\n", current_in_disk);
          zipwarn(errbuf, "");
          skip_disk = 1;
          break;
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
      if (skip_disk) {
        /* skip this current disk - this works because central directory entries
           can't be split across splits */
        skip_disk = 0;
        skipped_disk = 1;
        continue;
      }
    }

    if (skipped_disk) {
      /* skipped start CD disk so start searching for CD signature at start of disk */
      first_CD = 0;
    } else {
      /* seek to the first CD entry */
      if (first_CD) {
        if (zfseeko(in_file, in_cd_start_offset, SEEK_SET) != 0) {
          fclose(in_file);
          in_file = NULL;
          zipwarn("unable to seek in input file ", split_path);
          return ZE_READ;
        }
        first_CD = 0;
        x = &zfiles;                        /* first link */
      }
    }

    /* Main loop */
    /* Look for next signature and process it */
    while (find_next_signature(in_file)) {
      current_in_offset = zftello(in_file);

      if (is_signature(sigbuf, "PK\05\06")) {
        /* End Of Central Directory Record */
        /*
          fprintf(mesg, "EOCDR signature at %d / %I64d\n",
                  current_in_disk, current_in_offset - 4);
        */
        break;

      } else if (is_signature(sigbuf, "PK\06\06")) {
        /* Zip64 End Of Central Directory Record */
        /*
          fprintf(mesg, "Zip64 EOCDR signature at %d / %I64d\n",
                  current_in_disk, current_in_offset - 4);
        */
        break;

      } else if (!is_signature(sigbuf, "PK\01\02")) {
        /* Not Central Directory Record */

        /* this signature shouldn't be here */
        if (fix == 1) {
          int c;
          char errbuftemp[40];

          strcpy(errbuf, "bad archive - unexpected signature ");
          for (c = 0; c < 4; c++) {
            sprintf(errbuftemp, "%02x ", sigbuf[c]);
            strcat(errbuf, errbuftemp);
          }
          sprintf(errbuftemp, "on disk %lu at %s\n", current_in_disk,
                                   zip_fzofft(current_in_offset - 4, NULL, "u"));
          strcat(errbuf, errbuftemp);
          zipwarn(errbuf, "");
          zipwarn("skipping this signature...", "");
          continue;
        } else {
          sprintf(errbuf, "unexpected signature on disk %lu at %s\n",
                  current_in_disk, zip_fzofft(current_in_offset - 4, NULL, "u"));
          zipwarn(errbuf, "");
          zipwarn("archive not in correct format: ", split_path);
          zipwarn("(try -F to attempt recovery)", "");
          fclose(in_file);
          in_file = NULL;
          return ZE_FORM;
        }
      }

      /* central directory signature */
      if (verbose && fix == 1) {
        fprintf(mesg, "central directory header signature on disk %lu at %s\n",
                current_in_disk, zip_fzofft(current_in_offset - 4, NULL, "u"));
      }

      /* The format of a central directory record
        central file header signature   4 bytes  (0x02014b50)
        version made by                 2 bytes
        version needed to extract       2 bytes
        general purpose bit flag        2 bytes
        compression method              2 bytes
        last mod file time              2 bytes
        last mod file date              2 bytes
        crc-32                          4 bytes
        compressed size                 4 bytes
        uncompressed size               4 bytes
        file name length                2 bytes
        extra field length              2 bytes
        file comment length             2 bytes
        disk number start               2 bytes
        internal file attributes        2 bytes
        external file attributes        4 bytes
        relative offset of local header 4 bytes

        file name (variable size)
        extra field (variable size)
        file comment (variable size)
       */

      if (fread(scbuf, CENHEAD, 1, in_file) != 1) {
        zipwarn("reading central directory: ", strerror(errno));
        if (fix == 1) {
          zipwarn("bad archive - error reading central directory", "");
          zipwarn("skipping this entry...", "");
          continue;
        } else {
          return ferror(in_file) ? ZE_READ : ZE_EOF;
        }
      }

      if ((z = (struct zlist far *)farmalloc(sizeof(struct zlist))) == NULL) {
        zipwarn("reading central directory", "");
        return ZE_MEM;
      }

      z->vem = SH(CENVEM + scbuf);
      z->ver = SH(CENVER + scbuf);
      z->flg = SH(CENFLG + scbuf);
      z->how = SH(CENHOW + scbuf);
      z->tim = LG(CENTIM + scbuf);      /* time and date into one long */
      z->crc = LG(CENCRC + scbuf);
      z->siz = LG(CENSIZ + scbuf);
      z->len = LG(CENLEN + scbuf);
      z->nam = SH(CENNAM + scbuf);      /* used before comparing cen vs. loc */
      z->cext = SH(CENEXT + scbuf);     /* may be different from z->ext */
      z->com = SH(CENCOM + scbuf);
      z->dsk = SH(CENDSK + scbuf);
      z->att = SH(CENATT + scbuf);
      z->atx = LG(CENATX + scbuf);
      z->off = LG(CENOFF + scbuf);      /* adjust_offset is added below */
      z->dosflag = (z->vem & 0xff00) == 0;

      /* Initialize all fields pointing to malloced data to NULL */
      z->zname = z->name = z->iname = z->extra = z->cextra = z->comment = NULL;
      z->oname = NULL;
#ifdef UNICODE_SUPPORT
      z->uname = z->zuname = z->ouname = NULL;
#endif

      /* Read file name, extra field and comment field */
      if (z->nam == 0)
      {
        sprintf(errbuf, "%lu", (ulg)zcount + 1);
        zipwarn("zero-length name for entry #", errbuf);
        if (fix == 1) {
          zipwarn("skipping this entry...", "");
          continue;
        }
#ifndef DEBUG
        return ZE_FORM;
#endif
      }
      if ((z->iname = malloc(z->nam+1)) ==  NULL ||
          (z->cext && (z->cextra = malloc(z->cext)) == NULL) ||
          (z->com && (z->comment = malloc(z->com)) == NULL))
        return ZE_MEM;
      if (fread(z->iname, z->nam, 1, in_file) != 1 ||
          (z->cext && fread(z->cextra, z->cext, 1, in_file) != 1) ||
          (z->com && fread(z->comment, z->com, 1, in_file) != 1)) {
        if (fix == 1) {
          zipwarn("error reading entry:  ", strerror(errno));
          zipwarn("skipping this entry...", "");
          continue;
        }
        return ferror(in_file) ? ZE_READ : ZE_EOF;
      }
      z->iname[z->nam] = '\0';                  /* terminate name */
#ifdef UNICODE_SUPPORT
      if (unicode_mismatch != 3) {
        if (z->flg & UTF8_BIT) {
          char *iname;
          /* path is UTF-8 */
          if ((z->uname = malloc(strlen(z->iname) + 1)) == NULL) {
            zipwarn("could not allocate memory: scanzipf_reg", "");
            return ZE_MEM;
          }
          strcpy(z->uname, z->iname);
          /* Create a local name.  If UTF-8 system this should also be UTF-8 */
          iname = utf8_to_local_string(z->uname);
          if (iname) {
            free(z->iname);
            z->iname = iname;
          }
          else
            zipwarn("illegal UTF-8 name: ", z->uname);
        } else {
          /* check for UTF-8 path extra field */
          read_Unicode_Path_entry(z);
        }
      }
#endif

#ifdef WIN32
      /* Input path may be OEM */
      {
        unsigned hostver = (z->vem & 0xff);
        Ext_ASCII_TO_Native(z->iname, (z->vem >> 8), hostver,
                            ((z->atx & 0xffff0000L) != 0), FALSE);
      }
#endif

#ifdef EBCDIC
      if (z->com)
         memtoebc(z->comment, z->comment, z->com);
#endif /* EBCDIC */
#ifdef WIN32
      /* Comment may be OEM */
      {
        unsigned hostver = (z->vem & 0xff);
        Ext_ASCII_TO_Native(z->comment, (z->vem >> 8), hostver,
                            ((z->atx & 0xffff0000L) != 0), FALSE);
      }
#endif

#ifdef ZIP64_SUPPORT
      /* zip64 support 08/31/2003 R.Nausedat                          */
      /* here, we have to read the len, siz etc values from the CD    */
      /* entry as we might have to adjust them regarding their        */
      /* correspronding zip64 extra fields.                           */
      /* also, we cannot compare the values from the CD entries with  */
      /* the values from the LH as they might be different.           */

      /* adjust/update siz,len and off (to come: dsk) entries */
      /* PKZIP does not care of the version set in a CDH: if  */
      /* there is a zip64 extra field assigned to a CDH PKZIP */
      /* uses it, we should do so, too.                       */
      adjust_zip_central_entry(z);
#endif
      /* if adjusting for sfx prefix, add the offset */
      if ((fix ==1 && total_disks == 1) || adjust) z->off += adjust_offset;

      /* Update zipbeg beginning of archive offset, prepare for next header */
      if (z->dsk == 0 && (!zipbegset || z->off < zipbeg)) {
        zipbeg = z->off;
        zipbegset = 1;
      }
      zcount++;

      /* Clear actions */
      z->mark = 0;
      z->trash = 0;
#if defined(UNICODE_SUPPORT) && !defined(UTIL)
      z->zname = in2ex(z->iname);       /* convert to external name */
      if (z->zname == NULL)
        return ZE_MEM;
      if ((z->name = malloc(strlen(z->zname) + 1)) == NULL) {
        zipwarn("could not allocate memory: scanzipf_reg", "");
        return ZE_MEM;
      }
      strcpy(z->name, z->zname);
      z->oname = local_to_display_string(z->iname);

# ifdef WIN32
      z->namew = NULL;
      z->inamew = NULL;
      z->znamew = NULL;
# endif

      if (unicode_mismatch != 3) {
        if (z->uname) {
          /* create zuname which is alternate zname for matching based on
             converted Unicode name */
          char *name;

          /* Convert UTF-8 to current local character set */
          name = utf8_to_local_string(z->uname);

          if (name == NULL) {
            /*
            zipwarn("illegal UTF-8 name: ", z->uname);
            */
            /* not able to convert name, so use iname */
            if ((name = malloc(strlen(z->iname) + 1)) == NULL) {
              zipwarn("could not allocate memory: scanzipf_reg", "");
              return ZE_MEM;
            }
            strcpy(name, z->iname);
          }

# ifdef EBCDIC
          /* z->zname is used for printing and must be coded in native charset */
          strtoebc(z->zuname, name);
# else /* !EBCDIC */
          if ((z->zuname = malloc(strlen(name) + 1)) == NULL) {
            zipwarn("could not allocate memory: scanzipf_reg", "");
            return ZE_MEM;
          }
          strcpy(z->zuname, name);
          /* For output to terminal */
          if (unicode_escape_all) {
            char *ouname;
            /* Escape anything not 7-bit ASCII */
            ouname = utf8_to_escape_string(z->uname);
            if (ouname)
              z->ouname = ouname;
            else {
              if ((z->ouname = malloc(strlen(name) + 1)) == NULL) {
                zipwarn("could not allocate memory: scanzipf_reg", "");
                return ZE_MEM;
              }
              strcpy(z->ouname, name);
            }
          } else {
            if ((z->ouname = malloc(strlen(name) + 1)) == NULL) {
              zipwarn("could not allocate memory: scanzipf_reg", "");
              return ZE_MEM;
            }
            strcpy(z->ouname, name);
          }
#  ifdef WIN32

          if (!no_win32_wide) {
            z->inamew = utf8_to_wchar_string(z->uname);
            z->znamew = in2exw(z->inamew); /* convert to external name */
            if (z->znamew == NULL)
              return ZE_MEM;
          }

          local_to_oem_string(z->ouname, z->ouname);
          /* For matching.  There seems to be something lost
             in the translation from displaying a name in a
             console window using zip -su on Win32 and using
             that name in a command line to match what's in
             the archive.  This is klugy though.
          */
          if ((z->wuname = malloc(strlen(z->ouname) + 1)) == NULL) {
            zipwarn("could not allocate memory: scanzipf_reg", "");
            return ZE_MEM;
          }
          strcpy(z->wuname, z->ouname);
          oem_to_local_string(z->wuname, z->wuname);
#  endif /* WIN32 */
# endif /* ?EBCDIC */
        } else {
          /* no uname */
# ifdef WIN32
          if (!no_win32_wide) {
            z->inamew = local_to_wchar_string(z->iname);
            z->znamew = in2exw(z->inamew); /* convert to external name */
            if (z->znamew == NULL)
              return ZE_MEM;
          }
# endif
        }
      }
#else /* !(UNICODE_SUPPORT && !UTIL) */
# ifdef UTIL
/* We only need z->iname in the utils */
      z->name = z->iname;
#  ifdef EBCDIC
/* z->zname is used for printing and must be coded in native charset */
      if ((z->zname = malloc(z->nam+1)) ==  NULL) {
        zipwarn("could not allocate memory: scanzipf_reg", "");
        return ZE_MEM;
      }
      strtoebc(z->zname, z->iname);
#  else
      z->zname = z->iname;
#  endif
# else /* !UTIL */
      z->zname = in2ex(z->iname);       /* convert to external name */
      if (z->zname == NULL)
        return ZE_MEM;
      z->name = z->zname;
# endif /* ?UTIL */
      if ((z->oname = malloc(strlen(z->zname) + 1)) == NULL) {
        zipwarn("could not allocate memory: scanzipf_reg", "");
        return ZE_MEM;
      }
      strcpy(z->oname, z->zname);
#endif /* ?(UNICODE_SUPPORT && !UTIL) */

#ifndef UTIL
      if (verbose && fix == 0)
        zipoddities(z);
#endif

      /* Link into list */
      *x = z;
      z->nxt = NULL;
      x = &z->nxt;

    } /* while reading file */

    /* close disk and do next disk */
    fclose(in_file);
    in_file = NULL;
    free(split_path);

    if (!is_signature(sigbuf, "PK\01\02")) {
      /* if the last signature is not a CD signature and we get here then
         hit either the  Zip64 EOCDR or the EOCDR and done */
      break;
    }

  } /* for each disk */

  if (zcount != cd_total_entries) {
    sprintf(errbuf, "expected %s entries but found %s",
      zip_fzofft(cd_total_entries, NULL, "u"),
      zip_fzofft(zcount, NULL, "u"));
    zipwarn(errbuf, "");
    return ZE_FORM;
  }

  return ZE_OK;

} /* end of function scanzipf_regnew() */








/* ---------------------- */




/*
 * readzipfile initializes the global variables that hold the zipfile
 * directory info and opens the zipfile. For the actual zipfile scan,
 * the subroutine scanzipf_reg() or scanzipf_fix() is called,
 * depending on the mode of operation (regular processing, or zipfix mode).
 */
int readzipfile()
/*
   The name of the zip file is pointed to by the global "zipfile".
   The globals zipbeg, zfiles, zcount, and zcomlen are initialized.
   Return an error code in the ZE_ class.
*/
{
  FILE *f;              /* zip file */
  int retval;           /* return code */
  int readable;         /* 1 if zipfile exists and is readable */

  /* Initialize zip file info */
  zipbeg = 0;
  zfiles = NULL;                        /* Points to first header */
  zcount = 0;                           /* number of files */
  zcomlen = 0;                          /* zip file comment length */
  retval = ZE_OK;
  f = NULL;                             /* shut up some compilers */
  zipfile_exists = 0;

  /* If zip file exists, read headers and check structure */
#ifdef VMS
  if (zipfile == NULL || !(*zipfile) || !strcmp(zipfile, "-"))
    return ZE_OK;
  {
    int rtype;

    if ((VMSmunch(zipfile, GET_RTYPE, (char *)&rtype) == RMS$_NORMAL) &&
        (rtype == FAT$C_VARIABLE)) {
      fprintf(mesg,
     "\n     Error:  zipfile is in variable-length record format.  Please\n\
     run \"bilf b %s\" to convert the zipfile to fixed-length\n\
     record format.\n\n", zipfile);
      return ZE_FORM;
    }
  }
  readable = ((f = zfopen(zipfile, FOPR)) != NULL);
#else /* !VMS */
  readable = (zipfile != NULL && *zipfile && strcmp(zipfile, "-"));
  if (readable) {
    readable = ((f = zfopen(zipfile, FOPR)) != NULL);
  }
#endif /* ?VMS */

  /* skip check if streaming */
  if (!readable) {
    if (!zip_to_stdout && fix != 2 && strcmp(in_path, out_path)) {
      /* If -O used then in_path must exist */
      if (fix == 1)
        zipwarn("No .zip file found\n        ",
                "(If all you have are splits (.z01, .z02, ...) and no .zip, try -FF)");
      ZIPERR(ZE_OPEN, zipfile);
    }
  } else {
    zipfile_exists = 1;
  }

#ifdef MVS
  /* Very nasty special case for MVS.  Just because the zipfile has been
   * opened for reading does not mean that we can actually read the data.
   * Typical JCL to create a zipfile is
   *
   * //ZIPFILE  DD  DISP=(NEW,CATLG),DSN=prefix.ZIP,
   * //             SPACE=(CYL,(10,10))
   *
   * That creates a VTOC entry with an end of file marker (DS1LSTAR) of zero.
   * Alas the VTOC end of file marker is only used when the file is opened in
   * append mode.  When a file is opened in read mode, the "other" end of file
   * marker is used, a zero length data block signals end of file when reading.
   * With a brand new file which has not been written to yet, it is undefined
   * what you read off the disk.  In fact you read whatever data was in the same
   * disk tracks before the zipfile was allocated.  You would be amazed at the
   * number of application programmers who still do not understand this.  Makes
   * for interesting and semi-random errors, GIGO.
   *
   * Newer versions of SMS will automatically write a zero length block when a
   * file is allocated.  However not all sites run SMS or they run older levels
   * so we cannot rely on that.  The only safe thing to do is close the file,
   * open in append mode (we already know that the file exists), close it again,
   * reopen in read mode and try to read a data block.  Opening and closing in
   * append mode will write a zero length block where DS1LSTAR points, making
   * sure that the VTOC and internal end of file markers are in sync.  Then it
   * is safe to read data.  If we cannot read one byte of data after all that,
   * it is a brand new zipfile and must not be read.
   */
  if (readable)
  {
    char c;
    fclose(f);
    /* append mode */
    if ((f = zfopen(zipfile, "ab")) == NULL) {
      ZIPERR(ZE_OPEN, zipfile);
    }
    fclose(f);
    /* read mode again */
    if ((f = zfopen(zipfile, FOPR)) == NULL) {
      ZIPERR(ZE_OPEN, zipfile);
    }
    if (fread(&c, 1, 1, f) != 1) {
      /* no actual data */
      readable = 0;
      fclose(f);
    }
    else{
      fseek(f, 0, SEEK_SET);  /* at least one byte in zipfile, back to the start */
    }
  }
#endif /* MVS */

  /* ------------------------ */
  /* new file read */



#ifndef UTIL
  if (fix == 2) {
    scanzipf_fixnew();
  }
  else
#endif
  if (readable)
  {
    /* close file as the new scan opens the splits as needed */
    fclose(f);
# ifndef UTIL
    retval = (fix == 2 && !adjust) ? scanzipf_fixnew() : scanzipf_regnew();
# else
    retval = scanzipf_regnew();
# endif
  }

  if (fix != 2 && readable)
  {
    /* If one or more files, sort by name */
    if (zcount)
    {
      struct zlist far * far *x;    /* pointer into zsort array */
      struct zlist far *z;          /* pointer into zfiles linked list */
      extent zl_size = zcount * sizeof(struct zlist far *);

      if (zl_size / sizeof(struct zlist far *) != zcount ||
          (x = zsort = (struct zlist far **)malloc(zl_size)) == NULL)
        return ZE_MEM;
      for (z = zfiles; z != NULL; z = z->nxt)
        *x++ = z;
      qsort((char *)zsort, zcount, sizeof(struct zlist far *), zqcmp);

#ifdef UNICODE_SUPPORT
      /* sort by zuname (local conversion of UTF-8 name) */
      if (zl_size / sizeof(struct zlist far *) != zcount ||
          (x = zusort = (struct zlist far **)malloc(zl_size)) == NULL)
        return ZE_MEM;
      for (z = zfiles; z != NULL; z = z->nxt)
        *x++ = z;
      qsort((char *)zusort, zcount, sizeof(struct zlist far *), zuqcmp);
#endif
    }
  }

  /* ------------------------ */

  return retval;
} /* end of function readzipfile() */


int putlocal(z, rewrite)
  struct zlist far *z;    /* zip entry to write local header for */
  int rewrite;            /* did seek to rewrite */
/* Write a local header described by *z to file *f.  Return an error code
   in the ZE_ class. */
{
  /* If any of compressed size (siz), uncompressed size (len), offset(off), or
     disk number (dsk) is larger than can fit in the below standard fields then a
     Zip64 flag value is stored and a Zip64 extra field is created.
     Only siz and len are in the local header while all can be in the central
     directory header.

     For the local header if the extra field is created must store both
     uncompressed and compressed sizes.

     This assumes that for large entries the compressed size won't need a
     Zip64 extra field if the uncompressed size did not.  This assumption should
     only fail for a large file of nearly totally uncompressable data.

     If streaming stdin in and use_descriptors is set then always create a Zip64
     extra field flagging the data descriptor as being in Zip64 format.  This is
     needed as don't know if need Zip64 or not when need to set Zip64 flag in
     local header.

     If rewrite is set then don't count bytes written for splits
   */
  char *block = NULL;   /* mem block to write to */
  extent offset = 0;    /* offset into block */
  extent blocksize = 0; /* size of block */
#ifdef UNICODE_SUPPORT
  ush nam = z->nam;     /* size of name to write to header */
  int use_uname = 0;    /* write uname to header */
#endif
#ifdef ZIP64_SUPPORT
  int streaming_in = 0; /* streaming stdin */
  int was_zip64 = 0;

  /* If input is stdin then streaming stdin.  No problem with that.

     The problem is updating the local header data in the output once the sizes
     and crc are known.  If the output is not seekable, then need data descriptors
     and also need to assume Zip64 will be needed as don't know yet.  Even if the
     output is seekable, if the input is streamed need to write the Zip64 extra field
     before writing the data or there won't be room for it later if we need it.
  */
  streaming_in = (strcmp(z->name, "-") == 0);

  if (!rewrite) {
    zip64_entry = 0;
    /* initial local header */
    if (z->siz > ZIP_UWORD32_MAX || z->len > ZIP_UWORD32_MAX ||
      force_zip64 == 1 || (force_zip64 != 0 && streaming_in))
    {
      /* assume Zip64 */
      if (force_zip64 == 0) {
        zipwarn("Entry too big:", z->oname);
        ZIPERR(ZE_BIG, "Large entry support disabled with -fz- but needed");
      }
      zip64_entry = 1;        /* header of this entry has a field needing Zip64 */
      if (z->ver < ZIP64_MIN_VER)
        z->ver = ZIP64_MIN_VER;
      was_zip64 = 1;
    }
  } else {
    /* rewrite */
    was_zip64 = zip64_entry;
    zip64_entry = 0;
    if (z->siz > ZIP_UWORD32_MAX || z->len > ZIP_UWORD32_MAX ||
      force_zip64 == 1 || (force_zip64 != 0 && streaming_in))
    {
      /* Zip64 entry */
      zip64_entry = 1;
    }
    if (force_zip64 == 0 && zip64_entry) {
      /* tried to force into standard entry but needed Zip64 entry */
      zipwarn("Entry too big:", z->oname);
      ZIPERR(ZE_BIG, "Large entry support disabled with -fz- but entry needs");
    }
    /* Normally for a large archive if the input file is less than 4 GB then
       the compressed or stored version should be less than 4 GB.  If this
       assumption is wrong this catches it.  This is a problem even if not
       streaming as the Zip64 extra field was not written and now there's no
       room for it. */
    if (was_zip64 == 0 && zip64_entry == 1) {
      /* guessed wrong and need Zip64 */
      zipwarn("Entry too big:", z->oname);
      if (force_zip64 == 0) {
        ZIPERR(ZE_BIG, "Compressed/stored entry unexpectedly large - do not use -fz-");
      } else {
        ZIPERR(ZE_BIG, "Poor compression resulted in unexpectedly large entry - try -fz");
      }
    }
    if (zip64_entry) {
      /* Zip64 entry still */
      /* this archive needs Zip64 (version 4.5 unzipper) */
      zip64_archive = 1;
      if (z->ver < ZIP64_MIN_VER)
        z->ver = ZIP64_MIN_VER;
    } else {
      /* it turns out we do not need Zip64 */
      zip64_entry = 0;
    }
    if (was_zip64 && zip64_entry != 1) {
      z->ver = 20;
    }
  }


#endif /* ZIP64_SUPPORT */

  /* Instead of writing to the file as we go, to do splits we have to write it
     to memory and see if it will fit before writing the entire local header.
     If the local header doesn't fit we need to save it for the next disk.
   */

#ifdef ZIP64_SUPPORT
  if (zip64_entry || was_zip64)
    /* update extra field */
    add_local_zip64_extra_field( z );
#endif /* ZIP64_SUPPORT */

#ifdef UNICODE_SUPPORT
# if 0
  /* if UTF-8 bit is set on an existing entry, assume it should be */
  /* clear the UTF-8 flag */
  z->flg &= ~UTF8_BIT;
  z->lflg &= ~UTF8_BIT;
# endif

  if (z->uname) {
    /* need UTF-8 name */
    if (utf8_force || using_utf8) {
      z->lflg |= UTF8_BIT;
      z->flg |= UTF8_BIT;
    }
    if (z->flg & UTF8_BIT) {
      /* If this flag is set, then restore UTF-8 as path name */
      use_uname = 1;
      nam = strlen(z->uname);
    } else {
      /* use extra field */
      add_Unicode_Path_local_extra_field(z);
    }
  } else {
    /* clear UTF-8 bit as not needed */
    z->flg &= ~UTF8_BIT;
    z->lflg &= ~UTF8_BIT;
  }
#endif

  append_ulong_to_mem(LOCSIG, &block, &offset, &blocksize);     /* local file header signature */
  append_ushort_to_mem(z->ver, &block, &offset, &blocksize);    /* version needed to extract */
  append_ushort_to_mem(z->lflg, &block, &offset, &blocksize);   /* general purpose bit flag */
  append_ushort_to_mem(z->how, &block, &offset, &blocksize);    /* compression method */
  append_ulong_to_mem(z->tim, &block, &offset, &blocksize);     /* last mod file date time */
  append_ulong_to_mem(z->crc, &block, &offset, &blocksize);     /* crc-32 */
#ifdef ZIP64_SUPPORT        /* zip64 support 09/02/2003 R.Nausedat */
                            /* changes 10/5/03 EG */
  if (zip64_entry) {
    append_ulong_to_mem(0xFFFFFFFF, &block, &offset, &blocksize);	/* compressed size */
    append_ulong_to_mem(0xFFFFFFFF, &block, &offset, &blocksize);	/* uncompressed size */
  } else {
    append_ulong_to_mem((ulg)z->siz, &block, &offset, &blocksize);/* compressed size */
    append_ulong_to_mem((ulg)z->len, &block, &offset, &blocksize);/* uncompressed size */
  }
#else
  append_ulong_to_mem((ulg)z->siz, &block, &offset, &blocksize);    /* compressed size */
  append_ulong_to_mem((ulg)z->len, &block, &offset, &blocksize);    /* uncompressed size */
#endif
#ifdef UNICODE_SUPPORT
  append_ushort_to_mem(nam, &block, &offset, &blocksize);   /* file name length */
#else
  append_ushort_to_mem(z->nam, &block, &offset, &blocksize);   /* file name length */
#endif

  append_ushort_to_mem(z->ext, &block, &offset, &blocksize);    /* extra field length */

#ifdef UNICODE_SUPPORT
  if (use_uname) {
    /* path is UTF-8 */
    append_string_to_mem(z->uname, nam, &block, &offset, &blocksize);
  } else
#endif
#ifdef WIN32_OEM
  /* store name in OEM character set in archive */
  if ((z->vem & 0xff00) == 0)
  {
    char *oem;

    if ((oem = malloc(strlen(z->iname) + 1)) == NULL)
      ZIPERR(ZE_MEM, "putlocal oem");
    INTERN_TO_OEM(z->iname, oem);
    append_string_to_mem(oem, z->nam, &block, &offset, &blocksize); /* file name */
    free(oem);
  } else {
    append_string_to_mem(z->iname, z->nam, &block, &offset, &blocksize); /* file name */
  }
#else
  append_string_to_mem(z->iname, z->nam, &block, &offset, &blocksize); /* file name */
#endif
  if (z->ext) {
    append_string_to_mem(z->extra, z->ext, &block, &offset, &blocksize); /* extra field */
  }

  /* write the header */
  if (rewrite == PUTLOCAL_REWRITE) {
    /* use fwrite as seeked back and not extending the archive */
    /* also if split_method 1 write to file with local header */
    if (split_method == 1) {
      if (fwrite(block, 1, offset, current_local_file) != offset) {
        free(block);
        return ZE_TEMP;
      }
      /* now can close the split if local header on previous split */
      if (current_local_disk != current_disk) {
        close_split(current_local_disk, current_local_file, current_local_tempname);
        current_local_file = NULL;
        free(current_local_tempname);
      }
    } else {
      /* not doing splits */
      if (fwrite(block, 1, offset, y) != offset) {
        free(block);
        return ZE_TEMP;
      }
    }
  } else {
    /* do same if archive not split or split_method 2 with descriptors */
    /* use bfwrite which counts bytes for splits */
    if (bfwrite(block, 1, offset, BFWRITE_LOCALHEADER) != offset) {
      free(block);
      return ZE_TEMP;
    }
  }
  free(block);
  return ZE_OK;
}

int putextended(z)
  struct zlist far *z;    /* zip entry to write local header for */
  /* This is the data descriptor.
   * Write an extended local header described by *z to file *f.
   * Return an error code in the ZE_ class. */
{
  /* write to mem block then write to file 3/10/2005 */
  char *block = NULL;   /* mem block to write to */
  extent offset = 0;    /* offset into block */
  extent blocksize = 0; /* size of block */

  append_ulong_to_mem(EXTLOCSIG, &block, &offset, &blocksize);  /* extended local signature */
  append_ulong_to_mem(z->crc, &block, &offset, &blocksize);     /* crc-32 */
#ifdef ZIP64_SUPPORT
  if (zip64_entry) {
    /* use Zip64 entries */
    append_int64_to_mem(z->siz, &block, &offset, &blocksize);   /* compressed size */
    append_int64_to_mem(z->len, &block, &offset, &blocksize);   /* uncompressed size */
    /* This is rather klugy as the AppNote handles this poorly.  Typically
       we don't know at this point if we are writing a Zip64 archive or not,
       unless a file has needed Zip64.  This is particularly annoying here
       when deciding the size of the data descriptor (extended local header)
       fields as the appnote says the uncompressed and compressed sizes
       should be 8 bytes if the archive is Zip64 and 4 bytes if not.

       One interpretation is the version of the archive is determined from
       the Version Needed To Extract field in the Zip64 End Of Central Directory
       record and so either an archive should start as Zip64 and write all data
       descriptors with 8-byte fields or store everything until all the files
       are processed and then write everything to the archive as changing the
       sizes of the data descriptors is messy and just not feasible when
       streaming to standard output.  This is not easily workable and others
       use the different interpretation below.

       This was the old thought:
       We always write a standard data descriptor.  If the file has a large
       uncompressed or compressed size we set the field to the max field
       value, which we are defining as flagging the field as having a Zip64
       value that doesn't fit.  As the CRC happens before the variable size
       fields the CRC is still valid and can be used to check the file.  We
       always use deflate if streaming so signatures should not appear in
       the data and all local header signatures should be valid, allowing a
       streaming unzip to find entries by local header signatures, if max size
       values in the data descriptor sizes ignore them, and extract the file and
       check it using the CRC.  If not streaming the central directory is available
       so just use those values which are correct.

       After discussions with other groups this is the current thinking:

       Apparent industry interpretation for data descriptors:
       Data descriptor size is determined for each entry.  If the local header
       version needed to extract is 45 or higher then the entry can use Zip64
       data descriptors but more checking is needed.  If Zip64 extra field is
       present then assume data descriptor is Zip64 and local version needed
       to extract should be 45 or higher.  If standard data descriptor then
       local size fields are set to 0 and correct sizes are in standard data descriptor.
       If Zip64 data descriptor then local sizes are set to -1, Zip64 extra field
       sizes are set to 0, and the correct sizes are in the Zip64 data descriptor.

       So do this:
       If an entry is standard and the archive is updatable then seek back and
       update the local header.  No change.

       If an entry is zip64 and the archive is updatable assume the Zip64 extra
       field was created and update it.  No change.

       If data descriptors are needed then assume the archive is Zip64.  This is
       a change and means if ZIP64_SUPPORT is enabled that any non-updatable archive
       will be in Zip64 format and use Zip64 data descriptors.  This should be
       compatible with other zippers that depend on the current (though not perfect)
       AppNote description.

       If anyone has some ideas on this I'd like to hear them.

       3/20/05 EG

       Only assume need Zip64 if the input size is unknown.  If the input size is
       known we can assume Zip64 if the input is larger than 4 GB and assume not
       otherwise.  If the output is seekable we still need to create the Zip64
       extra field if the input size is unknown so we can seek back and update it.
       12/28/05 EG
       Updated 5/21/06 EG
    */
  } else {
    /* for encryption */
    append_ulong_to_mem((ulg)z->siz, &block, &offset, &blocksize);  /* compressed size */
    append_ulong_to_mem((ulg)z->len, &block, &offset, &blocksize);  /* uncompressed size */
  }
#else
  append_ulong_to_mem((ulg)z->siz, &block, &offset, &blocksize);    /* compressed size */
  append_ulong_to_mem((ulg)z->len, &block, &offset, &blocksize);    /* uncompressed size */
#endif
  /* write the header */
  if (bfwrite(block, 1, offset, BFWRITE_HEADER) != offset) {
    free(block);
    return ZE_TEMP;
  }
  free(block);
  return ZE_OK;
}

int putcentral(z)
  struct zlist far *z;    /* zip entry to write central header for */
/* Write a central header described by *z to file *f.  Return an error code
   in the ZE_ class. */
/* output now uses bfwrite which writes global y */
{
  /* If any of compressed size (siz), uncompressed size (len), offset(off), or
     disk number (dsk) is larger than can fit in the below standard fields then a
     Zip64 flag value is stored and a Zip64 extra field is created.
     Only siz and len are in the local header while all are in the central directory
     header.

     For the central directory header just store the fields required.  All previous fields
     must be stored though.  So can store none (no extra field), just uncompressed size
     (len), len then siz, len then siz then off, or len then siz then off then dsk, in
     those orders.  10/6/03 EG
   */

  /* write to mem block then write to file 3/10/2005 EG */
  char *block = NULL;   /* mem block to write to */
  extent offset = 0;    /* offset into block */
  extent blocksize = 0; /* size of block */
  uzoff_t off = 0;      /* offset to start of local header */
  ush nam = z->nam;     /* size of name to write to header */
#ifdef UNICODE_SUPPORT
  int use_uname = 0;    /* write uname to header */
#endif

#ifdef ZIP64_SUPPORT        /* zip64 support 09/02/2003 R.Nausedat */
  int iRes;
#endif

#ifdef UNICODE_SUPPORT
  if (z->uname) {
    if (utf8_force) {
      z->flg |= UTF8_BIT;
    }
    if (z->flg & UTF8_BIT) {
      /* If this flag is set, then restore UTF-8 as path name */
      use_uname = 1;
      nam = strlen(z->uname);
    } else {
      add_Unicode_Path_cen_extra_field(z);
    }
  } else {
    /* clear UTF-8 bit as not needed */
    z->flg &= ~UTF8_BIT;
    z->lflg &= ~UTF8_BIT;
  }
#endif

  off = z->off;

#ifdef ZIP64_SUPPORT        /* zip64 support 09/02/2003 R.Nausedat */
  if (z->siz > ZIP_UWORD32_MAX || z->len > ZIP_UWORD32_MAX ||
      z->off > ZIP_UWORD32_MAX || z->dsk > ZIP_UWORD16_MAX || (force_zip64 == 1))
  {
    iRes = add_central_zip64_extra_field(z);
    if( iRes != ZE_OK )
      return iRes;
  }

  append_ulong_to_mem(CENSIG, &block, &offset, &blocksize);     /* central file header signature */
  append_ushort_to_mem(z->vem, &block, &offset, &blocksize);    /* version made by */
  append_ushort_to_mem(z->ver, &block, &offset, &blocksize);    /* version needed to extract */
  append_ushort_to_mem(z->flg, &block, &offset, &blocksize);    /* general purpose bit flag */
  append_ushort_to_mem(z->how, &block, &offset, &blocksize);    /* compression method */
  append_ulong_to_mem(z->tim, &block, &offset, &blocksize);     /* last mod file date time */
  append_ulong_to_mem(z->crc, &block, &offset, &blocksize);     /* crc-32 */
  if (z->siz > ZIP_UWORD32_MAX)
  {
    /* instead of z->siz */
    append_ulong_to_mem(ZIP_UWORD32_MAX, &block, &offset, &blocksize); /* compressed size */
  }
  else
  {
    append_ulong_to_mem((ulg)z->siz, &block, &offset, &blocksize); /* compressed size */
  }
  /* if forcing Zip64 just force first ef field */
  if (z->len > ZIP_UWORD32_MAX || (force_zip64 == 1))
  {
    /* instead of z->len */
    append_ulong_to_mem(ZIP_UWORD32_MAX, &block, &offset, &blocksize); /* uncompressed size */
  }
  else
  {
    append_ulong_to_mem((ulg)z->len, &block, &offset, &blocksize); /* uncompressed size */
  }
  append_ushort_to_mem(nam, &block, &offset, &blocksize);       /* file name length */
  append_ushort_to_mem(z->cext, &block, &offset, &blocksize);   /* extra field length */
  append_ushort_to_mem(z->com, &block, &offset, &blocksize);    /* file comment length */

  if (z->dsk > ZIP_UWORD16_MAX)
  {
    /* instead of z->dsk */
    append_ushort_to_mem((ush)ZIP_UWORD16_MAX, &block, &offset, &blocksize); /* Zip64 flag */
  }
  else
  {
    append_ushort_to_mem((ush)z->dsk, &block, &offset, &blocksize);	/* disk number start */
  }
  append_ushort_to_mem(z->att, &block, &offset, &blocksize);    /* internal file attributes */
  append_ulong_to_mem(z->atx, &block, &offset, &blocksize);     /* external file attributes */
  if (off > ZIP_UWORD32_MAX)
  {
    /* instead of z->off */
    append_ulong_to_mem(ZIP_UWORD32_MAX, &block, &offset, &blocksize); /* Zip64 flag */
  }
  else
  {
    append_ulong_to_mem((ulg)off, &block, &offset, &blocksize); /* offset of local header */
  }

#else /* !ZIP64_SUPPORT */

  append_ulong_to_mem(CENSIG, &block, &offset, &blocksize);     /* central file header signature */
  append_ushort_to_mem(z->vem, &block, &offset, &blocksize);    /* version made by */
  append_ushort_to_mem(z->ver, &block, &offset, &blocksize);    /* version needed to extract */
  append_ushort_to_mem(z->flg, &block, &offset, &blocksize);    /* general purpose bit flag */
  append_ushort_to_mem(z->how, &block, &offset, &blocksize);    /* compression method */
  append_ulong_to_mem(z->tim, &block, &offset, &blocksize);     /* last mod file date time */
  append_ulong_to_mem(z->crc, &block, &offset, &blocksize);     /* crc-32 */
  append_ulong_to_mem((ulg)z->siz, &block, &offset, &blocksize);  /* compressed size */
  append_ulong_to_mem((ulg)z->len, &block, &offset, &blocksize);  /* uncompressed size */
  append_ushort_to_mem(nam, &block, &offset, &blocksize);       /* file name length */
  append_ushort_to_mem(z->cext, &block, &offset, &blocksize);   /* extra field length */
  append_ushort_to_mem(z->com, &block, &offset, &blocksize);    /* file comment length */
  append_ushort_to_mem((ush)z->dsk, &block, &offset, &blocksize); /* disk number start */
  append_ushort_to_mem(z->att, &block, &offset, &blocksize);    /* internal file attributes */
  append_ulong_to_mem(z->atx, &block, &offset, &blocksize);     /* external file attributes */
  append_ulong_to_mem((ulg)off, &block, &offset, &blocksize);   /* relative offset of local header */

#endif /* ZIP64_SUPPORT */

#ifdef EBCDIC
  if (z->com)
    memtoasc(z->comment, z->comment, z->com);
#endif /* EBCDIC */

#ifdef UNICODE_SUPPORT
  if (use_uname) {
    /* path is UTF-8 */
    append_string_to_mem(z->uname, nam, &block, &offset, &blocksize);
  } else
#endif
#ifdef WIN32_OEM
  /* store name in OEM character set in archive */
  if ((z->vem & 0xff00) == 0)
  {
    char *oem;

    if ((oem = malloc(strlen(z->iname) + 1)) == NULL)
      ZIPERR(ZE_MEM, "putcentral oem");
    INTERN_TO_OEM(z->iname, oem);
    append_string_to_mem(oem, z->nam, &block, &offset, &blocksize);
    free(oem);
  } else {
    append_string_to_mem(z->iname, z->nam, &block, &offset, &blocksize);
  }
#else
  append_string_to_mem(z->iname, z->nam, &block, &offset, &blocksize);
#endif

  if (z->cext) {
    append_string_to_mem(z->cextra, z->cext, &block, &offset, &blocksize);
  }
  if (z->com) {
#ifdef WIN32_OEM
    /* store comment in OEM character set in archive */
    if ((z->vem & 0xff00) == 0)
    {
      char *oem;

      if ((oem = malloc(strlen(z->comment) + 1)) == NULL)
        ZIPERR(ZE_MEM, "putcentral oem comment");
      INTERN_TO_OEM(z->comment, oem);
      append_string_to_mem(oem, z->com, &block, &offset, &blocksize);
      free(oem);
    } else {
      append_string_to_mem(z->comment, z->com, &block, &offset, &blocksize);
    }
#else
    append_string_to_mem(z->comment, z->com, &block, &offset, &blocksize);
#endif
  }

  /* write the header */
  if (bfwrite(block, 1, offset, BFWRITE_CENTRALHEADER) != offset) {
    free(block);
    return ZE_TEMP;
  }
  free(block);

  return ZE_OK;
}


/* Write the end of central directory data to file y.  Return an error code
   in the ZE_ class. */

int putend( OFT( uzoff_t) n,
            OFT( uzoff_t) s,
            OFT( uzoff_t) c,
            OFT( extent) m,
            OFT( char *) z
          )
#ifdef NO_PROTO
  uzoff_t n;                /* number of entries in central directory */
  uzoff_t s;                /* size of central directory */
  uzoff_t c;                /* offset of central directory */
  extent m;                 /* length of zip file comment (0 if none) */
  char *z;                  /* zip file comment if m != 0 */
#endif /* def NO_PROTO */
{
#ifdef ZIP64_SUPPORT        /* zip64 support 09/05/2003 R.Nausedat */
  ush vem;          /* version made by */
  int iNeedZip64 = 0;

  char *block = NULL;   /* mem block to write to */
  extent offset = 0;    /* offset into block */
  extent blocksize = 0; /* size of block */

  /* we have to create a zip64 archive if we have more than 64k - 1 entries,      */
  /* if the CD is > 4 GB or if the offset to the CD > 4 GB. even if the CD start  */
  /* is < 4 GB and CD start + CD size > 4GB we do not need a zip64 archive since  */
  /* the offset entry in the CD tail is still valid.  [note that there are other  */
  /* reasons for needing a Zip64 archive though, such as an uncompressed          */
  /* size > 4 GB for an entry but the entry compresses below 4 GB, so the archive */
  /* is Zip64 but the CD does not need Zip64.]                                    */
  /* order of the zip/zip64 records in a zip64 archive:                           */
  /* central directory                                                            */
  /* zip64 end of central directory record                                        */
  /* zip64 end of central directory locator                                       */
  /* end of central directory record                                              */

  /* check zip64_archive instead of force_zip64 3/19/05 */

  zip64_eocd_disk = current_disk;
  zip64_eocd_offset = bytes_this_split;

  if( n > ZIP_UWORD16_MAX || s > ZIP_UWORD32_MAX || c > ZIP_UWORD32_MAX ||
      zip64_archive )
  {
    ++iNeedZip64;
    /* write zip64 central dir tail:  */
    /*                                    */
    /* 4 bytes   zip64 end of central dir signature (0x06064b50) */
    append_ulong_to_mem((ulg)ZIP64_CENTRAL_DIR_TAIL_SIG, &block, &offset, &blocksize);
    /* 8 bytes   size of zip64 end of central directory record */
    /* a fixed size unless the end zip64 extensible data sector is used. - 3/19/05 EG */
    /* also note that AppNote 6.2 creates version 2 of this record for
       central directory encryption - 3/19/05 EG */
    append_int64_to_mem((zoff_t)ZIP64_CENTRAL_DIR_TAIL_SIZE, &block, &offset, &blocksize);

    /* 2 bytes   version made by */
    vem = OS_CODE + Z_MAJORVER * 10 + Z_MINORVER;
    append_ushort_to_mem(vem, &block, &offset, &blocksize);

    /* APPNOTE says that zip64 archives should have at least version 4.5
       in the "version needed to extract" field */
    /* 2 bytes   version needed to extract */
    append_ushort_to_mem(ZIP64_MIN_VER, &block, &offset, &blocksize);

    /* 4 bytes   number of this disk */
    append_ulong_to_mem(current_disk, &block, &offset, &blocksize);
    /* 4 bytes   number of the disk with the start of the central directory */
    append_ulong_to_mem(cd_start_disk, &block, &offset, &blocksize);
    /* 8 bytes   total number of entries in the central directory on this disk */
    append_int64_to_mem(cd_entries_this_disk, &block, &offset, &blocksize);
    /* 8 bytes   total number of entries in the central directory */
    append_int64_to_mem(n, &block, &offset, &blocksize);
    /* 8 bytes   size of the central directory */
    append_int64_to_mem(s, &block, &offset, &blocksize);
    /* 8 bytes   offset of start of central directory with respect to the starting disk number */
    append_int64_to_mem(cd_start_offset, &block, &offset, &blocksize);
    /* zip64 extensible data sector    (variable size), we don't use it... */

    /* write zip64 end of central directory locator:  */
    /*                                                    */
    /* 4 bytes   zip64 end of central dir locator  signature (0x07064b50) */
    append_ulong_to_mem(ZIP64_CENTRAL_DIR_TAIL_END_SIG, &block, &offset, &blocksize);
    /* 4 bytes   number of the disk with the start of the zip64 end of central directory */
    append_ulong_to_mem(zip64_eocd_disk, &block, &offset, &blocksize);
    /* 8 bytes   relative offset of the zip64 end of central directory record, that is */
    /* offset of CD + CD size */
    append_int64_to_mem(zip64_eocd_offset, &block, &offset, &blocksize);
    /* PUTLLG(l64Temp, f); */
    /* 4 bytes   total number of disks */
    append_ulong_to_mem(current_disk + 1, &block, &offset, &blocksize);
  }

  /* end of central dir signature */
  append_ulong_to_mem(ENDSIG, &block, &offset, &blocksize);
    /* mv archives to come :)         */
    /* for now use n for all          */
    /* 2 bytes    number of this disk */
  if (current_disk < 0xFFFF)
    append_ushort_to_mem((ush)current_disk, &block, &offset, &blocksize);
  else
    append_ushort_to_mem((ush)0xFFFF, &block, &offset, &blocksize);
  /* 2 bytes    number of the disk with the start of the central directory */
  if (cd_start_disk == (ulg)-1)
    cd_start_disk = 0;
  if (cd_start_disk < 0xFFFF)
    append_ushort_to_mem((ush)cd_start_disk, &block, &offset, &blocksize);
  else
    append_ushort_to_mem((ush)0xFFFF, &block, &offset, &blocksize);
  /* 2 bytes    total number of entries in the central directory on this disk */
  if (cd_entries_this_disk < 0xFFFF)
    append_ushort_to_mem((ush)cd_entries_this_disk, &block, &offset, &blocksize);
  else
    append_ushort_to_mem((ush)0xFFFF, &block, &offset, &blocksize);
  /* 2 bytes    total number of entries in the central directory */
  if (total_cd_entries < 0xFFFF)
    append_ushort_to_mem((ush)total_cd_entries, &block, &offset, &blocksize);
  else
    append_ushort_to_mem((ush)0xFFFF, &block, &offset, &blocksize);
  if( s > ZIP_UWORD32_MAX )
    /* instead of s */
    append_ulong_to_mem(ZIP_UWORD32_MAX, &block, &offset, &blocksize);
  else
    /* 4 bytes    size of the central directory */
    append_ulong_to_mem((ulg)s, &block, &offset, &blocksize);
  if(force_zip64 == 1 || cd_start_offset > ZIP_UWORD32_MAX)
    /* instead of cd_start_offset */
    append_ulong_to_mem(ZIP_UWORD32_MAX, &block, &offset, &blocksize);
  else
    /* 4 bytes    offset of start of central directory with respect to the starting disk number */
    append_ulong_to_mem((ulg)cd_start_offset, &block, &offset, &blocksize);

#else /* !ZIP64_SUPPORT */
  char *block = NULL;   /* mem block to write to */
  extent offset = 0;    /* offset into block */
  extent blocksize = 0; /* size of block */

  /* end of central dir signature */
  append_ulong_to_mem(ENDSIG, &block, &offset, &blocksize);
  /* 2 bytes    number of this disk */
  append_ushort_to_mem((ush)current_disk, &block, &offset, &blocksize);
  /* 2 bytes    number of the disk with the start of the central directory */
  append_ushort_to_mem((ush)cd_start_disk, &block, &offset, &blocksize);
  /* 2 bytes    total number of entries in the central directory on this disk */
  append_ushort_to_mem((ush)cd_entries_this_disk, &block, &offset, &blocksize);
  /* 2 bytes    total number of entries in the central directory */
  append_ushort_to_mem((ush)n, &block, &offset, &blocksize);
  /* 4 bytes    size of the central directory */
  append_ulong_to_mem((ulg)s, &block, &offset, &blocksize);
  /* 4 bytes    offset of start of central directory with respect to the starting disk number */
  append_ulong_to_mem((ulg)cd_start_offset, &block, &offset, &blocksize);
#endif /* ZIP64_SUPPORT */

  /* size of comment */
  append_ushort_to_mem((ush)m, &block, &offset, &blocksize);
  /* Write the comment, if any */
#ifdef EBCDIC
  memtoasc(z, z, m);
#endif
  if (m) {
    /* PKWare defines the archive comment to be ASCII only so no OEM conversion */
    append_string_to_mem(z, m, &block, &offset, &blocksize);
  }

  /* write the block */
  if (bfwrite(block, 1, offset, BFWRITE_HEADER) != offset) {
    free(block);
    return ZE_TEMP;
  }
  free(block);

#ifdef HANDLE_AMIGA_SFX
  if (amiga_sfx_offset && zipbeg /* -J zeroes this */) {
    s = zftello(y);
    while (s & 3) s++, putc(0, f);   /* final marker must be longword aligned */
    PUTLG(0xF2030000 /* 1010 in Motorola byte order */, f);
    c = (s - amiga_sfx_offset - 4) / 4;  /* size of archive part in longwords */
    if (zfseeko(y, amiga_sfx_offset, SEEK_SET) != 0)
      return ZE_TEMP;
    c = ((c >> 24) & 0xFF) | ((c >> 8) & 0xFF00)
         | ((c & 0xFF00) << 8) | ((c & 0xFF) << 24);     /* invert byte order */
    PUTLG(c, y);
    zfseeko(y, 0, SEEK_END);                                  /* just in case */
  }
#endif

  return ZE_OK;
} /* end function putend() */



/* Note: a zip "entry" includes a local header (which includes the file
   name), an encryption header if encrypting, the compressed data
   and possibly an extended local header. */

int zipcopy(z)
  struct zlist far *z;    /* zip entry to copy */
/* Copy the zip entry described by *z from in_file to y.  Return an
   error code in the ZE_ class.  Also update tempzn by the number of bytes
   copied. */
/* Now copies to global output file y */
/* Handle entries that span disks */
/* If fix == 2, assume in_file is pointing to a local header and fill
   in z from local header */
{
  uzoff_t n;            /* holds local header offset */
  ulg e = 0;            /* extended local header size */
  ulg start_disk = 0;
  uzoff_t start_offset = 0;
  char *split_path;
  char buf[LOCHEAD + 1];
  struct zlist far *localz;
  int r;


  Trace((stderr, "zipcopy %s\n", z->zname));

  /* if fix == 2 assume in_file open and pointing at local header */
  if (fix != 2) {
    start_disk = z->dsk;
    start_offset = z->off;

    /* don't assume reading the right disk */

    /* if start not on current disk then close current disk */
    if (start_disk != current_in_disk) {
      if (in_file) {
        fclose(in_file);
        in_file = NULL;
      }
    }

    current_in_disk = start_disk;

    /* disks are archive.z01, archive.z02, ..., archive.zip */
    split_path = get_in_split_path(in_path, current_in_disk);

    if (in_file == NULL) {
      while ((in_file = zfopen(split_path, FOPR)) == NULL) {
        /* could not open split */

        if (!noisy) {
          ZIPERR(ZE_OPEN, split_path);
        }

        /* Ask for directory with split.  Updates global in_path */
        r = ask_for_split_read_path(start_disk);
        if (r == ZE_ABORT) {
          /* user abort */
          return ZE_ABORT;
        } else if ((fix == 1 || fix == 2) && r == ZE_FORM) {
          /* user asks to skip this disk */
          return ZE_FORM;
        }
        free(split_path);
        split_path = get_in_split_path(in_path, start_disk);
      }
    }

    if (zfseeko(in_file, start_offset, SEEK_SET) != 0) {
      fclose(in_file);
      in_file = NULL;
      zipwarn("reading archive fseek: ", strerror(errno));
      return ZE_READ;
    }
  } /* fix != 2 */

  if (fix != 2 && !at_signature(in_file, "PK\03\04")) {
    fclose(in_file);
    in_file = NULL;
    zipwarn("Did not find entry for ", z->iname);
    return ZE_FORM;
  }

  /* read local header */
  if (fread(buf, LOCHEAD, 1, in_file) != 1) {
    int f = ferror(in_file);
    zipwarn("reading local entry: ", strerror(errno));
    if (fix != 2)
      fclose(in_file);
    return f ? ZE_READ : ZE_EOF;
  }

  /* Local Header
       local file header signature     4 bytes  (0x04034b50)
       version needed to extract       2 bytes
       general purpose bit flag        2 bytes
       compression method              2 bytes
       last mod file time              2 bytes
       last mod file date              2 bytes
       crc-32                          4 bytes
       compressed size                 4 bytes
       uncompressed size               4 bytes
       file name length                2 bytes
       extra field length              2 bytes

       file name (variable size)
       extra field (variable size)
   */

  if ((localz = (struct zlist far *)farmalloc(sizeof(struct zlist))) == NULL) {
    zipwarn("reading entry", "");
    if (fix != 2)
      fclose(in_file);
    return ZE_MEM;
  }

  localz->ver = SH(LOCVER + buf);
  localz->lflg = SH(LOCFLG + buf);
  localz->how = SH(LOCHOW + buf);
  localz->tim = LG(LOCTIM + buf);          /* time and date into one long */
  localz->crc = LG(LOCCRC + buf);
  localz->nam = SH(LOCNAM + buf);
  localz->ext = SH(LOCEXT + buf);
  if (fix == 2) {
    localz->siz = LG(LOCSIZ + buf);
    localz->len = LG(LOCLEN + buf);
  }

  if (fix == 2) {
    /* Do some sanity checks to make reasonably sure this is a local header */
    ush os = localz->ver >> 8;
    ush pkver = localz->ver - os;

    /* OS - currently 0 - 18 (AppNote 6.3) and 30 (ATHEOS) */
    if (os > 40) {
      sprintf(errbuf, "Illegal host system mapping in local header:  %d", os);
      zipwarn(errbuf, "");
      zipwarn("Skipping:  ", z->iname);
      return ZE_FORM;
    }
    /* PK Version - currently 10 - 62 (AppNote 6.2.2) */
    /* If PKZip central directory encryption is used (62), the local header
       values could be masked values.  Specifically, as of AppNote 6.2.2
       the time, crc-32, and uncompressed file size are masked and the
       file name is also replaced with a hex entry count.  Should
       still be able to recover the entries, but they may be unreadable
       without the 62 support fields. */
    if (pkver > 100) {
      sprintf(errbuf, "Illegal PK version mapping in local header:  %d", pkver);
      zipwarn(errbuf, "");
      zipwarn("Skipping:  ", z->iname);
      return ZE_FORM;
    }
    /* Currently compression method is defined as 0 - 19 and 98 (AppNote 6.3) */
    /* We can still copy an entry we can't read, but something over 200 is
       probably illegal */
    if (localz->how > 200) {
      sprintf(errbuf, "Unrecognized compression method in local header:  %d", localz->how);
      zipwarn(errbuf, "");
      zipwarn("Skipping:  ", z->iname);
      return ZE_FORM;
    }

    /* It's hard to make guesses on the other fields.  Suggestions welcome. */
  }

  /* Initialize all fields pointing to malloced data to NULL */
  localz->zname = localz->name = localz->iname = localz->extra = NULL;
  localz->oname = NULL;
#ifdef UNICODE_SUPPORT
  localz->uname = NULL;
#endif

  /* Read file name, extra field and comment field */
  if ((localz->iname = malloc(localz->nam+1)) ==  NULL ||
      (localz->ext && (localz->extra = malloc(localz->ext)) == NULL))
    return ZE_MEM;
  if (fread(localz->iname, localz->nam, 1, in_file) != 1 ||
      (localz->ext && fread(localz->extra, localz->ext, 1, in_file) != 1))
    return ferror(in_file) ? ZE_READ : ZE_EOF;
  localz->iname[localz->nam] = '\0';                  /* terminate name */
  if ((localz->name = malloc(localz->nam+1)) ==  NULL)
    return ZE_MEM;
  strcpy(localz->name, localz->iname);

#ifdef ZIP64_SUPPORT
  zip64_entry = adjust_zip_local_entry(localz);
#endif

  localz->vem = 0;
  if (fix != 2) {
    /* Need vem to determine if iname is Win32 OEM name */
    localz->vem = z->vem;

#ifdef UNICODE_SUPPORT
    if (unicode_mismatch != 3) {
      if (z->flg & UTF8_BIT) {
        char *iname;
        /* path is UTF-8 */
        localz->uname = localz->iname;
        iname = utf8_to_local_string(localz->uname);
        if (iname == NULL) {
          /* a bad UTF-8 character in name likely - go with (probably messed up) uname */
          if ((localz->iname = malloc(strlen(localz->uname) + 1)) == NULL) {
            return ZE_MEM;
          }
          strcpy(localz->iname, localz->uname);
        } else {
          /* go with local character set iname */
          localz->iname = iname;
        }
      } else {
        /* check for UTF-8 path extra field */
        read_Unicode_Path_local_entry(localz);
      }
    }
#endif

#ifdef WIN32_OEM
      /* If fix == 2 and reading local headers first, vem is not in the local
         header so we don't know when to do OEM translation, as the ver field
         is set to MSDOS (0) by all unless something specific is needed.
         However, if local header has a Unicode path extra field, we can get
         the real file name from there. */
    if ((z->vem & 0xff00) == 0)
      /* assume archive name is OEM if from DOS */
      oem_to_local_string(localz->iname, localz->iname);
#endif
  }

  if (fix == 2) {
# ifdef WIN32
#  ifdef UNICODE_SUPPORT
    localz->namew = NULL;
    localz->inamew = NULL;
    localz->znamew = NULL;
    z->namew = NULL;
    z->inamew = NULL;
    z->znamew = NULL;
#  endif
# endif
    /* set z from localz */
    z->flg = localz->lflg;
    z->len = localz->len;
    z->siz = localz->siz;

  } else {
    /* Compare localz to z */
    if (localz->ver != z->ver) {
      zipwarn("Local Version Needed To Extract does not match CD: ", z->iname);
    }
    if (localz->lflg != z->flg) {
      zipwarn("Local Entry Flag does not match CD: ", z->iname);
    }
    if (!(z->flg & 8)) {
      if (localz->crc != z->crc) {
        zipwarn("Local Entry CRC does not match CD: ", z->iname);
      }
    }
    if (fix != 3 && strcmp(localz->iname, z->iname) != 0) {
      zipwarn("Local Entry name does not match CD: ", z->iname);
    }

    /* as copying get uncompressed and compressed sizes from central directory */
    localz->len = z->len;
    localz->siz = z->siz;
  }

#if 0
  if (fix > 1) {
    if (zfseeko(in_file, z->off + n, SEEK_SET)) /* seek to compressed data */
      return ferror(in_file) ? ZE_READ : ZE_EOF;

    if (fix > 2) {
      /* Update length of entry's name, it may have been changed.  This is
         needed to support the ZipNote ability to rename archive entries. */
      z->nam = strlen(z->iname);
      n = (uzoff_t)((LOCHEAD) + (ulg)z->nam + (ulg)z->ext);
    }

    /* do not trust the old compressed size */
    if (putlocal(z, PUTLOCAL_WRITE) != ZE_OK)
      return ZE_TEMP;

    z->off = tempzn;
    tempzn += n;
    n = z->siz;
  } else {
    if (zfseeko(in_file, z->off, SEEK_SET))     /* seek to local header */
      return ferror(in_file) ? ZE_READ : ZE_EOF;

    z->off = tempzn;
    n += z->siz;
  }
#endif

  /* from zipnote */
  if (fix == 3) {
    /* Update length of entry's name, as it may have been changed.  This is
       needed to support the ZipNote ability to rename archive entries. */
    localz->nam = z->nam = strlen(z->iname);
    /* update local name */
    free(localz->iname);
    if ((localz->iname = malloc(strlen(z->iname) + 1)) == NULL) {
      zipwarn("out of memory in zipcopy", "");
      return ZE_MEM;
    }
    strcpy(localz->iname, z->iname);
  }

  /* update disk and offset */
  z->dsk = current_disk;
  z->off = bytes_this_split;

  /* copy the compressed data and the extended local header if there is one */

  /* copy the compressed data.  We recreate the local header as the local
     header can't be split and putlocal ensures it won't.  Also, since we
     use siz and len from the central directory, we don't need the extended
     local header if there is one, unless the file is encrypted as then the
     extended header is used to indicate crypt head uses file time instead
     of crc as the password check.

     If fix = 2 then we don't have the central directory yet so keep
     any data descriptors. */

  if (fix != 2 && !(z->flg & 1)) {
    /* Not encrypted */
    localz->flg = z->flg &= ~8;
    z->lflg = localz->lflg &= ~8;
  }

  e = 0;
  if (z->lflg & 8) {
#ifdef ZIP64_SUPPORT
    if (zip64_entry)
      e = 24;
    else
#endif
      e = 16;
  }
  /* 4 is signature */
  n = 4 + (uzoff_t)((LOCHEAD) + (ulg)(localz->nam) + (ulg)(localz->ext));

  n += e + z->siz;
  tempzn += n;

  /* Output name */
  if (fix == 2) {
    if ((z->oname = malloc(strlen(localz->iname) + 1)) == NULL) {
      return ZE_MEM;
    }
    strcpy(z->oname, localz->iname);
#ifndef UTIL
# ifdef WIN32
    /* Win9x console always uses OEM character coding, and
       WinNT console is set to OEM charset by default, too */
    _INTERN_OEM(z->oname);
# endif
#endif
    sprintf(errbuf, " copying: %s ", z->oname);
    zipmessage_nl(errbuf, 0);
  }

  if (fix == 2)
    z->crc = localz->crc;
  else
    localz->crc = z->crc;

  if (putlocal(localz, PUTLOCAL_WRITE) != ZE_OK)
      return ZE_TEMP;

  /*
  if (zfseeko(in_file, start_offset, SEEK_SET) != 0) {
    fclose(in_file);
    in_file = NULL;
    zipwarn("reading archive fseek: ", strerror(errno));
    return ZE_READ;
  }
  */

  /* copy the data */
  if (fix == 2 && localz->lflg & 8)
    /* read to data descriptor */
    r = bfcopy((uzoff_t) -2);
  else
    r = bfcopy(localz->siz);

  if (r == ZE_ABORT) {
      if (localz->ext) free(localz->extra);
      if (localz->nam) free(localz->iname);
      if (localz->nam) free(localz->name);
#ifdef UNICODE_SUPPORT
      if (localz->uname) free(localz->uname);
#endif
      free(localz);
      ZIPERR(ZE_ABORT, "Could not find split");
  }

  if (r == ZE_EOF || skip_this_disk) {
      /* missing disk */
      zipwarn("aborting: ", z->oname);

      if (r == ZE_OK)
        r = ZE_FORM;

      if (fix == 2) {
#ifdef DEBUG
        zoff_t here = zftello(y);
#endif

        /* fix == 2 skips right to next disk */
        skip_this_disk = 0;

        /* seek back in output to start of this entry so can overwrite */
        if (zfseeko(y, current_local_offset, SEEK_SET) != 0) {
          ZIPERR(ZE_WRITE, "seek failed on output file");
        }
        bytes_this_split = current_local_offset;
        tempzn = current_local_offset;
      }

      /* tell scan to skip this entry */
      if (localz->ext) free(localz->extra);
      if (localz->nam) free(localz->iname);
      if (localz->nam) free(localz->name);
#ifdef UNICODE_SUPPORT
      if (localz->uname) free(localz->uname);
#endif
      free(localz);
      return r;
  }

  if (fix == 2 && z->flg & 8) {
    /* this entry should have a data descriptor */
    /* only -FF needs to read the descriptor as other modes
       rely on the central directory */
    if (des_good) {
      /* found an apparently good data descriptor */
      localz->crc = des_crc;
      localz->siz = des_csize;
      localz->len = des_usize;
    } else {
      /* no end to this entry found */
      zipwarn("no end of stream entry found: ", z->oname);
      zipwarn("rewinding and scanning for later entries", "");

      /* seek back in output to start of this entry so can overwrite */
      if (zfseeko(y, current_local_offset, SEEK_SET) != 0){

      }

      /* tell scan to skip this entry */
      if (localz->ext) free(localz->extra);
      if (localz->nam) free(localz->iname);
      if (localz->nam) free(localz->name);
#ifdef UNICODE_SUPPORT
      if (localz->uname) free(localz->uname);
#endif
      free(localz);
      return ZE_FORM;
    }
  }

  if (z->flg & 8) {
    putextended(localz);
  }

  /* now can close the split if local header on previous split */
  if (split_method == 1 && current_local_disk != current_disk) {
    close_split(current_local_disk, current_local_file, current_local_tempname);
    current_local_file = NULL;
    free(current_local_tempname);
  }

  /* update local header and close start split */
  /* to use this need to seek back, do this, then come back
  if (putlocal(localz, PUTLOCAL_REWRITE) != ZE_OK)
    r = ZE_TEMP;
  */

  if (fix == 2) {
    z->ver = localz->ver;
    z->how = localz->how;
    z->tim = localz->tim;
    z->crc = localz->crc;
    z->lflg = localz->lflg;
    z->flg = localz->lflg;
    z->len = localz->len;
    z->siz = localz->siz;
    z->nam = localz->nam;
    z->ext = localz->ext;
    z->extra = localz->extra;
    /* copy local extra fields to central directory for now */
    z->cext = localz->ext;
    z->cextra = NULL;
    if (localz->ext) {
      if ((z->cextra = malloc(localz->ext + 1)) == NULL) {
      return ZE_MEM;
      }
      strcpy(z->cextra, localz->extra);
    }
    z->com = 0;
    z->att = 0;
    z->atx = 0;
    z->name = localz->name;
    z->iname = localz->iname;
#ifdef UNICODE_SUPPORT
    z->uname = localz->uname;
#endif
    if ((z->zname = malloc(localz->nam + 1)) == NULL) {
      return ZE_MEM;
    }
    strcpy(z->zname, z->iname);
  } else {
    if (localz->ext) free(localz->extra);
    if (localz->nam) free(localz->iname);
    if (localz->nam) free(localz->name);
#ifdef UNICODE_SUPPORT
    if (localz->uname) free(localz->uname);
#endif
    free(localz);
  }

  if (fix == 2) {
    sprintf(errbuf, " (%s bytes)", zip_fzofft(z->siz, NULL, "u"));
    zipmessage_nl(errbuf, 1);

    if (r == ZE_READ) {
      zipwarn("entry truncated: ", z->oname);
      sprintf(errbuf, "expected compressed/stored size %s, actual %s",
              zip_fzofft(localz->siz, NULL, "u"), zip_fzofft(bytes_this_entry, NULL, "u"));
      zipwarn(errbuf, "");
    }
  }

  return r;
}



#ifndef UTIL

#ifdef USE_EF_UT_TIME

local int ef_scan_ut_time(ef_buf, ef_len, ef_is_cent, z_utim)
char *ef_buf;                   /* buffer containing extra field */
extent ef_len;                  /* total length of extra field */
int ef_is_cent;                 /* flag indicating "is central extra field" */
iztimes *z_utim;                /* return storage: atime, mtime, ctime */
/* This function scans the extra field for EF_TIME or EF_IZUNIX blocks
 * containing Unix style time_t (GMT) values for the entry's access, creation
 * and modification time.
 * If a valid block is found, all time stamps are copied to the iztimes
 * structure.
 * The presence of an EF_TIME or EF_IZUNIX2 block results in ignoring
 * all data from probably present obsolete EF_IZUNIX blocks.
 * If multiple blocks of the same type are found, only the information from
 * the last block is used.
 * The return value is the EF_TIME Flags field (simulated in case of an
 * EF_IZUNIX block) or 0 in case of failure.
 */
{
  int flags = 0;
  unsigned eb_id;
  extent eb_len;
  int have_new_type_eb = FALSE;

  if (ef_len == 0 || ef_buf == NULL)
    return 0;

  Trace((stderr,"\nef_scan_ut_time: scanning extra field of length %u\n",
         (unsigned)ef_len));
  while (ef_len >= EB_HEADSIZE) {
    eb_id = SH(EB_ID + ef_buf);
    eb_len = SH(EB_LEN + ef_buf);

    if (eb_len > (ef_len - EB_HEADSIZE)) {
      /* Discovered some extra field inconsistency! */
      Trace((stderr,"ef_scan_ut_time: block length %u > rest ef_size %u\n",
             (unsigned)eb_len, (unsigned)(ef_len - EB_HEADSIZE)));
      break;
    }

    switch (eb_id) {
      case EF_TIME:
        flags &= ~0x00ff;       /* ignore previous IZUNIX or EF_TIME fields */
        have_new_type_eb = TRUE;
        if ( eb_len >= EB_UT_MINLEN && z_utim != NULL) {
           unsigned eb_idx = EB_UT_TIME1;
           Trace((stderr,"ef_scan_ut_time: Found TIME extra field\n"));
           flags |= (ef_buf[EB_HEADSIZE+EB_UT_FLAGS] & 0x00ff);
           if ((flags & EB_UT_FL_MTIME)) {
              if ((eb_idx+4) <= eb_len) {
                 z_utim->mtime = LG((EB_HEADSIZE+eb_idx) + ef_buf);
                 eb_idx += 4;
                 Trace((stderr,"  Unix EF modtime = %ld\n", z_utim->mtime));
              } else {
                 flags &= ~EB_UT_FL_MTIME;
                 Trace((stderr,"  Unix EF truncated, no modtime\n"));
              }
           }
           if (ef_is_cent) {
              break;            /* central version of TIME field ends here */
           }
           if (flags & EB_UT_FL_ATIME) {
              if ((eb_idx+4) <= eb_len) {
                 z_utim->atime = LG((EB_HEADSIZE+eb_idx) + ef_buf);
                 eb_idx += 4;
                 Trace((stderr,"  Unix EF acctime = %ld\n", z_utim->atime));
              } else {
                 flags &= ~EB_UT_FL_ATIME;
              }
           }
           if (flags & EB_UT_FL_CTIME) {
              if ((eb_idx+4) <= eb_len) {
                 z_utim->ctime = LG((EB_HEADSIZE+eb_idx) + ef_buf);
                 /* eb_idx += 4; */  /* superfluous for now ... */
                 Trace((stderr,"  Unix EF cretime = %ld\n", z_utim->ctime));
              } else {
                 flags &= ~EB_UT_FL_CTIME;
              }
           }
        }
        break;

      case EF_IZUNIX2:
        if (!have_new_type_eb) {
           flags &= ~0x00ff;    /* ignore any previous IZUNIX field */
           have_new_type_eb = TRUE;
        }
        break;

      case EF_IZUNIX:
        if (eb_len >= EB_UX_MINLEN) {
           Trace((stderr,"ef_scan_ut_time: Found IZUNIX extra field\n"));
           if (have_new_type_eb) {
              break;            /* Ignore IZUNIX extra field block ! */
           }
           z_utim->atime = LG((EB_HEADSIZE+EB_UX_ATIME) + ef_buf);
           z_utim->mtime = LG((EB_HEADSIZE+EB_UX_MTIME) + ef_buf);
           Trace((stderr,"  Unix EF access time = %ld\n",z_utim->atime));
           Trace((stderr,"  Unix EF modif. time = %ld\n",z_utim->mtime));
           flags |= (EB_UT_FL_MTIME | EB_UT_FL_ATIME);  /* signal success */
        }
        break;

      case EF_THEOS:
/*      printf("Not implemented yet\n"); */
        break;

      default:
        break;
    }
    /* Skip this extra field block */
    ef_buf += (eb_len + EB_HEADSIZE);
    ef_len -= (eb_len + EB_HEADSIZE);
  }

  return flags;
}

int get_ef_ut_ztime(z, z_utim)
struct zlist far *z;
iztimes *z_utim;
{
  int r;

#ifdef IZ_CHECK_TZ
  if (!zp_tz_is_valid) return 0;
#endif

  /* First, scan local extra field. */
  r = ef_scan_ut_time(z->extra, z->ext, FALSE, z_utim);

  /* If this was not successful, try central extra field, but only if
     it is really different. */
  if (!r && z->cext > 0 && z->cextra != z->extra)
    r = ef_scan_ut_time(z->cextra, z->cext, TRUE, z_utim);

  return r;
}

#endif /* USE_EF_UT_TIME */


local void cutpath(p, delim)
char *p;                /* path string */
int delim;              /* path component separator char */
/* Cut the last path component off the name *p in place.
 * This should work on both internal and external names.
 */
{
  char *r;              /* pointer to last path delimiter */

#ifdef VMS                      /* change [w.x.y]z to [w.x]y.DIR */
  if ((r = MBSRCHR(p, ']')) != NULL)
  {
    *r = 0;
    if ((r = MBSRCHR(p, '.')) != NULL)
    {
      *r = ']';
      strcat(r, ".DIR;1");     /* this assumes a little padding--see PAD */
    } else {
      *p = 0;
    }
  } else {
    if ((r = MBSRCHR(p, delim)) != NULL)
      *r = 0;
    else
      *p = 0;
  }
#else /* !VMS */
  if ((r = MBSRCHR(p, delim)) != NULL)
    *r = 0;
  else
    *p = 0;
#endif /* ?VMS */
}

int trash()
/* Delete the compressed files and the directories that contained the deleted
   files, if empty.  Return an error code in the ZE_ class.  Failure of
   destroy() or deletedir() is ignored. */
{
  extent i;             /* counter on deleted names */
  extent n;             /* number of directories to delete */
  struct zlist far **s; /* table of zip entries to handle, sorted */
  struct zlist far *z;  /* current zip entry */

  /* Delete marked names and count directories */
  n = 0;
  for (z = zfiles; z != NULL; z = z->nxt)
    if (z->mark == 1 || z->trash)
    {
      z->mark = 1;
      if (z->iname[z->nam - 1] != (char)0x2f) { /* don't unlink directory */
        if (verbose)
          fprintf(mesg, "zip diagnostic: deleting file %s\n", z->name);
        if (destroy(z->name)) {
          zipwarn("error deleting ", z->name);
        }
        /* Try to delete all paths that lead up to marked names. This is
         * necessary only with the -D option.
         */
        if (!dirnames) {
          cutpath(z->name, '/');  /* XXX wrong ??? */
          /* Below apparently does not work for Russian OEM but
             '/' should be same as 0x2f for ascii and most ports so
             changed it.  Did not trace through the mappings but
             maybe 0x2F is mapped differently on OEM_RUSS - EG 2/28/2003 */
          /* CS, 5/14/2005: iname is the byte array read from and written
             to the zip archive; it MUST be ASCII (compatible)!!!
             If something goes wrong with OEM_RUSS, there is a charcode
             mapping error between external name (z->name) and iname somewhere
             in the in2ex & ex2in code. The charcode translation should be
             checked.
             This code line is changed back to the original code. */
          /* CS, 6/12/2005: What is handled here is the difference between
             ASCII charsets and non-ASCII charsets like the family of EBCDIC
             charsets.  On these systems, the slash character '/' is not coded
             as 0x2f but as 0x61 (the ASCII 'a'). The iname struct member holds
             the name as stored in the Zip file, which are ASCII or translated
             into ASCII for new entries, whereas the "name" struct member hold
             the external name, coded in the native charset of the system
             (EBCDIC on EBCDIC systems) */
          /* cutpath(z->iname, '/'); */ /* QQQ ??? */
          cutpath(z->iname, 0x2f); /* 0x2f = ascii['/'] */
          z->nam = strlen(z->iname);
          if (z->nam > 0) {
            z->iname[z->nam - 1] = (char)0x2f;
            z->iname[z->nam++] = '\0';
          }
          if (z->nam > 0) n++;
        }
      } else {
        n++;
      }
    }

  /* Construct the list of all marked directories. Some may be duplicated
   * if -D was used.
   */
  if (n)
  {
    if ((s = (struct zlist far **)malloc(n*sizeof(struct zlist far *))) ==
        NULL)
      return ZE_MEM;
    n = 0;
    for (z = zfiles; z != NULL; z = z->nxt) {
      if (z->mark && z->nam > 0 && z->iname[z->nam - 1] == (char)0x2f /* '/' */
          && (n == 0 || strcmp(z->name, s[n-1]->name) != 0)) {
        s[n++] = z;
      }
    }
    /* Sort the files in reverse order to get subdirectories first.
     * To avoid problems with strange naming conventions as in VMS,
     * we sort on the internal names, so x/y/z will always be removed
     * before x/y. On VMS, x/y/z > x/y but [x.y.z] < [x.y]
     */
    qsort((char *)s, n, sizeof(struct zlist far *), rqcmp);

    for (i = 0; i < n; i++) {
      char *p = s[i]->name;
      if (*p == '\0') continue;
      if (p[strlen(p) - 1] == '/') { /* keep VMS [x.y]z.dir;1 intact */
        p[strlen(p) - 1] = '\0';
      }
      if (i == 0 || strcmp(s[i]->name, s[i-1]->name) != 0) {
        if (verbose) {
          fprintf(mesg, "deleting directory %s (if empty)                \n",
                  s[i]->name);
        }
        deletedir(s[i]->name);
      }
    }
    free((zvoid *)s);
  }
  return ZE_OK;
}

#endif /* !UTIL */
