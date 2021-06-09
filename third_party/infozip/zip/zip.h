/*
  zip.h - Zip 3

/---------------------------------------------------------------------/

Info-ZIP Licence

This is version 2007-Mar-4 of the Info-ZIP license.
The definitive version of this document should be available at
ftp://ftp.info-zip.org/pub/infozip/license.html indefinitely and
a copy at http://www.info-zip.org/pub/infozip/license.html.


Copyright (c) 1990-2008 Info-ZIP.  All rights reserved.

For the purposes of this copyright and license, "Info-ZIP" is defined as
the following set of individuals:

   Mark Adler, John Bush, Karl Davis, Harald Denker, Jean-Michel Dubois,
   Jean-loup Gailly, Hunter Goatley, Ed Gordon, Ian Gorman, Chris Herborth,
   Dirk Haase, Greg Hartwig, Robert Heath, Jonathan Hudson, Paul Kienitz,
   David Kirschbaum, Johnny Lee, Onno van der Linden, Igor Mandrichenko,
   Steve P. Miller, Sergio Monesi, Keith Owens, George Petrov, Greg Roelofs,
   Kai Uwe Rommel, Steve Salisbury, Dave Smith, Steven M. Schweda,
   Christian Spieler, Cosmin Truta, Antoine Verheijen, Paul von Behren,
   Rich Wales, Mike White.

This software is provided "as is," without warranty of any kind, express
or implied.  In no event shall Info-ZIP or its contributors be held liable
for any direct, indirect, incidental, special or consequential damages
arising out of the use of or inability to use this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the above disclaimer and the following restrictions:

    1. Redistributions of source code (in whole or in part) must retain
       the above copyright notice, definition, disclaimer, and this list
       of conditions.

    2. Redistributions in binary form (compiled executables and libraries)
       must reproduce the above copyright notice, definition, disclaimer,
       and this list of conditions in documentation and/or other materials
       provided with the distribution.  The sole exception to this condition
       is redistribution of a standard UnZipSFX binary (including SFXWiz) as
       part of a self-extracting archive; that is permitted without inclusion
       of this license, as long as the normal SFX banner has not been removed
       from the binary or disabled.

    3. Altered versions--including, but not limited to, ports to new operating
       systems, existing ports with new graphical interfaces, versions with
       modified or added functionality, and dynamic, shared, or static library
       versions not from Info-ZIP--must be plainly marked as such and must not
       be misrepresented as being the original source or, if binaries,
       compiled from the original source.  Such altered versions also must not
       be misrepresented as being Info-ZIP releases--including, but not
       limited to, labeling of the altered versions with the names "Info-ZIP"
       (or any variation thereof, including, but not limited to, different
       capitalizations), "Pocket UnZip," "WiZ" or "MacZip" without the
       explicit permission of Info-ZIP.  Such altered versions are further
       prohibited from misrepresentative use of the Zip-Bugs or Info-ZIP
       e-mail addresses or the Info-ZIP URL(s), such as to imply Info-ZIP
       will provide support for the altered versions.

    4. Info-ZIP retains the right to use the names "Info-ZIP," "Zip," "UnZip,"
       "UnZipSFX," "WiZ," "Pocket UnZip," "Pocket Zip," and "MacZip" for its
       own source and binary releases.

/---------------------------------------------------------------------/

*/

/*
 *  zip.h by Mark Adler
 */
#ifndef __zip_h
#define __zip_h 1

#define ZIP   /* for crypt.c:  include zip password functions, not unzip */

/* Types centralized here for easy modification */
#define local static            /* More meaningful outside functions */
typedef unsigned char uch;      /* unsigned 8-bit value */
typedef unsigned short ush;     /* unsigned 16-bit value */
typedef unsigned long ulg;      /* unsigned 32-bit value */

/* Set up portability */
#include "tailor.h"

#ifdef USE_ZLIB
#  include "zlib.h"
#endif

/* In the utilities, the crc32() function is only used for UNICODE_SUPPORT. */
#if defined(UTIL) && !defined(UNICODE_SUPPORT)
#  define CRC_TABLE_ONLY
#endif

#define MIN_MATCH  3
#define MAX_MATCH  258
/* The minimum and maximum match lengths */

#ifndef WSIZE
#  define WSIZE  (0x8000)
#endif
/* Maximum window size = 32K. If you are really short of memory, compile
 * with a smaller WSIZE but this reduces the compression ratio for files
 * of size > WSIZE. WSIZE must be a power of two in the current implementation.
 */

#define MIN_LOOKAHEAD (MAX_MATCH+MIN_MATCH+1)
/* Minimum amount of lookahead, except at the end of the input file.
 * See deflate.c for comments about the MIN_MATCH+1.
 */

#define MAX_DIST  (WSIZE-MIN_LOOKAHEAD)
/* In order to simplify the code, particularly on 16 bit machines, match
 * distances are limited to MAX_DIST instead of WSIZE.
 */

/* Forget FILENAME_MAX (incorrectly = 14 on some System V) */
#ifdef DOS
#  define FNMAX 256
#else
#  define FNMAX 1024
#endif

#ifndef MATCH
#  define MATCH shmatch         /* Default for pattern matching: UNIX style */
#endif

/* Structure carrying extended timestamp information */
typedef struct iztimes {
   time_t atime;                /* new access time */
   time_t mtime;                /* new modification time */
   time_t ctime;                /* new creation time (!= Unix st.ctime) */
} iztimes;

/* Lengths of headers after signatures in bytes */
#define LOCHEAD 26
#define CENHEAD 42
#define ENDHEAD 18
#define EC64LOC 16
#define EC64REC 52

/* Structures for in-memory file information */
struct zlist {
  /* See central header in zipfile.c for what vem..off are */
  /* Do not rearrange these as less than smart coding in zipfile.c
     in scanzipf_reg() depends on u being set to ver and then stepping
     through as a byte array.  Ack.  Should be fixed.  5/25/2005 EG */
  /* All the new read code does not rely on this order.  */
  ush vem, ver, flg, how;
  ulg tim, crc;
  uzoff_t siz, len;             /* zip64 support 08/29/2003 R.Nausedat */
  /* changed from extent to ush 3/10/2005 EG */
  ush nam, ext, cext, com;      /* offset of ext must be >= LOCHEAD */
  ulg dsk;                      /* disk number was ush but now ulg */
  ush att, lflg;                /* offset of lflg must be >= LOCHEAD */
  uzoff_t off;
  ulg atx;
  char *name;                   /* File name in zip file */
  char *extra;                  /* Extra field (set only if ext != 0) */
  char *cextra;                 /* Extra in central (set only if cext != 0) */
  char *comment;                /* Comment (set only if com != 0) */
  char *iname;                  /* Internal file name after cleanup (stored in archive) */
  char *zname;                  /* External version of internal name */
  char *oname;                  /* Display version of name used in messages */
#ifdef UNICODE_SUPPORT
  /* Unicode support */
  char *uname;                  /* UTF-8 version of iname */
  /* if uname has chars not in local char set, zuname can be different than zname */
  char *zuname;                 /* Escaped Unicode zname from uname */
  char *ouname;                 /* Display version of zuname */
# ifdef WIN32
  char *wuname;                 /* Converted back ouname for Win32 */
  wchar_t *namew;               /* Windows wide character version of name */
  wchar_t *inamew;              /* Windows wide character version of iname */
  wchar_t *znamew;              /* Windows wide character version of zname */
# endif
#endif
  int mark;                     /* Marker for files to operate on */
  int trash;                    /* Marker for files to delete */
  int current;                  /* Marker for files that are current to what is on OS (filesync) */
  int dosflag;                  /* Set to force MSDOS file attributes */
  struct zlist far *nxt;        /* Pointer to next header in list */
};
struct flist {
  char *name;                   /* Raw internal file name */
  char *iname;                  /* Internal file name after cleanup */
  char *zname;                  /* External version of internal name */
  char *oname;                  /* Display version of internal name */
#ifdef UNICODE_SUPPORT
  char *uname;                  /* UTF-8 name */
# ifdef WIN32
  wchar_t *namew;               /* Windows wide character version of name */
  wchar_t *inamew;              /* Windows wide character version of iname */
  wchar_t *znamew;              /* Windows wide character version of zname */
# endif
#endif
  int dosflag;                  /* Set to force MSDOS file attributes */
  uzoff_t usize;                /* usize from initial scan */
  struct flist far *far *lst;   /* Pointer to link pointing here */
  struct flist far *nxt;        /* Link to next name */
};
struct plist {
  char *zname;                  /* External version of internal name */
  int select;                   /* Selection flag ('i' or 'x') */
};

/* internal file attribute */
#define UNKNOWN (-1)
#define BINARY  0
#define ASCII   1
#define __EBCDIC 2

/* extra field definitions */
#define EF_VMCMS     0x4704   /* VM/CMS Extra Field ID ("G")*/
#define EF_MVS       0x470f   /* MVS Extra Field ID ("G")   */
#define EF_IZUNIX    0x5855   /* UNIX Extra Field ID ("UX") */
#define EF_IZUNIX2   0x7855   /* Info-ZIP's new Unix ("Ux") */
#define EF_TIME      0x5455   /* universal timestamp ("UT") */
#define EF_OS2EA     0x0009   /* OS/2 Extra Field ID (extended attributes) */
#define EF_ACL       0x4C41   /* ACL Extra Field ID (access control list, "AL") */
#define EF_NTSD      0x4453   /* NT Security Descriptor Extra Field ID, ("SD") */
#define EF_BEOS      0x6542   /* BeOS Extra Field ID ("Be") */
#define EF_ATHEOS    0x7441   /* AtheOS Extra Field ID ("At") */
#define EF_QDOS      0xfb4a   /* SMS/QDOS ("J\373") */
#define EF_AOSVS     0x5356   /* AOS/VS ("VS") */
#define EF_SPARK     0x4341   /* David Pilling's Acorn/SparkFS ("AC") */
#define EF_THEOS     0x6854   /* THEOS ("Th") */
#define EF_TANDEM    0x4154   /* Tandem NSK ("TA") */

/* Definitions for extra field handling: */
#define EF_SIZE_MAX  ((unsigned)0xFFFF) /* hard limit of total e.f. length */
#define EB_HEADSIZE       4     /* length of a extra field block header */
#define EB_ID             0     /* offset of block ID in header */
#define EB_LEN            2     /* offset of data length field in header */
#define EB_MEMCMPR_HSIZ   6     /* header length for memcompressed data */
#define EB_DEFLAT_EXTRA  10     /* overhead for 64kByte "undeflatable" data */

#define EB_UX_MINLEN      8     /* minimal "UX" field contains atime, mtime */
#define EB_UX_ATIME       0     /* offset of atime in "UX" extra field data */
#define EB_UX_MTIME       4     /* offset of mtime in "UX" extra field data */

#define EB_UX_FULLSIZE    12    /* full "UX" field (atime, mtime, uid, gid) */
#define EB_UX_UID         8     /* byte offset of UID in "UX" field data */
#define EB_UX_GID         10    /* byte offset of GID in "UX" field data */

#define EB_UT_MINLEN      1     /* minimal UT field contains Flags byte */
#define EB_UT_FLAGS       0     /* byte offset of Flags field */
#define EB_UT_TIME1       1     /* byte offset of 1st time value */
#define EB_UT_FL_MTIME    (1 << 0)      /* mtime present */
#define EB_UT_FL_ATIME    (1 << 1)      /* atime present */
#define EB_UT_FL_CTIME    (1 << 2)      /* ctime present */
#define EB_UT_LEN(n)      (EB_UT_MINLEN + 4 * (n))

#define EB_UX2_MINLEN     4     /* minimal Ux field contains UID/GID */
#define EB_UX2_UID        0     /* byte offset of UID in "Ux" field data */
#define EB_UX2_GID        2     /* byte offset of GID in "Ux" field data */
#define EB_UX2_VALID      (1 << 8)      /* UID/GID present */

/* ASCII definitions for line terminators in text files: */
#define LF     10        /* '\n' on ASCII machines; must be 10 due to EBCDIC */
#define CR     13        /* '\r' on ASCII machines; must be 13 due to EBCDIC */
#define CTRLZ  26        /* DOS & OS/2 EOF marker (used in fileio.c, vms.c) */

/* return codes of password fetches (negative: user abort; positive: error) */
#define IZ_PW_ENTERED   0       /* got some PWD string, use/try it */
#define IZ_PW_CANCEL    -1      /* no password available (for this entry) */
#define IZ_PW_CANCELALL -2      /* no password, skip any further PWD request */
#define IZ_PW_ERROR     5       /* = PK_MEM2 : failure (no mem, no tty, ...) */
#define IZ_PW_SKIPVERIFY IZ_PW_CANCEL   /* skip encrypt. passwd verification */

/* mode flag values of password prompting function */
#define ZP_PW_ENTER     0       /* request for encryption password */
#define ZP_PW_VERIFY    1       /* request for reentering password */

/* Error return codes and PERR macro */
#include "ziperr.h"

#if 0            /* Optimization: use the (const) result of crc32(0L,NULL,0) */
#  define CRCVAL_INITIAL  crc32(0L, (uch *)NULL, 0)
# if 00 /* not used, should be removed !! */
#  define ADLERVAL_INITIAL adler16(0U, (uch *)NULL, 0)
# endif /* 00 */
#else
#  define CRCVAL_INITIAL  0L
# if 00 /* not used, should be removed !! */
#  define ADLERVAL_INITIAL 1
# endif /* 00 */
#endif

#define DOSTIME_MINIMUM         ((ulg)0x00210000L)
#define DOSTIME_2038_01_18      ((ulg)0x74320000L)


/* Public globals */
extern uch upper[256];          /* Country dependent case map table */
extern uch lower[256];
#ifdef EBCDIC
extern ZCONST uch ascii[256];   /* EBCDIC <--> ASCII translation tables */
extern ZCONST uch ebcdic[256];
#endif /* EBCDIC */
#if (!defined(USE_ZLIB) || defined(USE_OWN_CRCTAB))
  extern ZCONST ulg near *crc_32_tab;
#else
  extern ZCONST ulg Far *crc_32_tab;
#endif

/* Are these ever used?  6/12/05 EG */
#ifdef IZ_ISO2OEM_ARRAY         /* ISO 8859-1 (Win CP 1252) --> OEM CP 850 */
extern ZCONST uch Far iso2oem[128];
#endif
#ifdef IZ_OEM2ISO_ARRAY         /* OEM CP 850 --> ISO 8859-1 (Win CP 1252) */
extern ZCONST uch Far oem2iso[128];
#endif

extern char errbuf[FNMAX+4081]; /* Handy place to build error messages */
extern int recurse;             /* Recurse into directories encountered */
extern int dispose;             /* Remove files after put in zip file */
extern int pathput;             /* Store path with name */

#ifdef RISCOS
extern int scanimage;           /* Scan through image files */
#endif

#define BEST -1                 /* Use best method (deflation or store) */
#define STORE 0                 /* Store method */
#define DEFLATE 8               /* Deflation method*/
#define BZIP2 12                /* BZIP2 method */
#ifdef BZIP2_SUPPORT
#define LAST_KNOWN_COMPMETHOD   BZIP2
#else
#define LAST_KNOWN_COMPMETHOD   DEFLATE
#endif

extern int method;              /* Restriction on compression method */

extern ulg skip_this_disk;
extern int des_good;            /* Good data descriptor found */
extern ulg des_crc;             /* Data descriptor CRC */
extern uzoff_t des_csize;       /* Data descriptor csize */
extern uzoff_t des_usize;       /* Data descriptor usize */
extern int dosify;              /* Make new entries look like MSDOS */
extern char *special;           /* Don't compress special suffixes */
extern int verbose;             /* Report oddities in zip file structure */
extern int fix;                 /* Fix the zip file */
extern int filesync;            /* 1=file sync, delete entries not on file system */
extern int adjust;              /* Adjust the unzipsfx'd zip file */
extern int level;               /* Compression level */
extern int translate_eol;       /* Translate end-of-line LF -> CR LF */
#ifdef VMS
   extern int vmsver;           /* Append VMS version number to file names */
   extern int vms_native;       /* Store in VMS format */
   extern int vms_case_2;       /* ODS2 file name case in VMS. -1: down. */
   extern int vms_case_5;       /* ODS5 file name case in VMS. +1: preserve. */

/* Accomodation for /NAMES = AS_IS with old header files. */
# define cma$tis_errno_get_addr CMA$TIS_ERRNO_GET_ADDR
# define lib$establish LIB$ESTABLISH
# define lib$get_foreign LIB$GET_FOREIGN
# define lib$get_input LIB$GET_INPUT
# define lib$sig_to_ret LIB$SIG_TO_RET
# define ots$cvt_tu_l OTS$CVT_TU_L
# define str$concat STR$CONCAT
# define str$find_first_substring STR$FIND_FIRST_SUBSTRING
# define str$free1_dx STR$FREE1_DX
# define sys$asctim SYS$ASCTIM
# define sys$assign SYS$ASSIGN
# define sys$bintim SYS$BINTIM
# define sys$close SYS$CLOSE
# define sys$connect SYS$CONNECT
# define sys$dassgn SYS$DASSGN
# define sys$display SYS$DISPLAY
# define sys$getjpiw SYS$GETJPIW
# define sys$open SYS$OPEN
# define sys$parse SYS$PARSE
# define sys$qiow SYS$QIOW
# define sys$read SYS$READ
# define sys$search SYS$SEARCH
#endif /* VMS */
#if defined(OS2) || defined(WIN32)
   extern int use_longname_ea;   /* use the .LONGNAME EA as the file's name */
#endif
#if defined (QDOS) || defined(QLZIP)
extern short qlflag;
#endif
/* 9/26/04 EG */
extern int no_wild;             /* wildcards are disabled */
extern int allow_regex;         /* 1 = allow [list] matching (regex) */
extern int wild_stop_at_dir;    /* wildcards do not include / in matches */
#ifdef UNICODE_SUPPORT
  extern int using_utf8;        /* 1 if current character set is UTF-8 */
# ifdef WIN32
   extern int no_win32_wide;    /* 1 = no wide functions, like GetFileAttributesW() */
# endif
#endif
/* 10/20/04 */
extern zoff_t dot_size;         /* if not 0 then display dots every size buffers */
extern zoff_t dot_count;        /* if dot_size not 0 counts buffers */
/* status 10/30/04 */
extern int display_counts;      /* display running file count */
extern int display_bytes;       /* display running bytes remaining */
extern int display_globaldots;  /* display dots for archive instead of for each file */
extern int display_volume;      /* display current input and output volume (disk) numbers */
extern int display_usize;       /* display uncompressed bytes */
extern ulg files_so_far;        /* files processed so far */
extern ulg bad_files_so_far;    /* files skipped so far */
extern ulg files_total;         /* files total to process */
extern uzoff_t bytes_so_far;    /* bytes processed so far (from initial scan) */
extern uzoff_t good_bytes_so_far;/* good bytes read so far */
extern uzoff_t bad_bytes_so_far;/* bad bytes skipped so far */
extern uzoff_t bytes_total;     /* total bytes to process (from initial scan) */
/* logfile 6/5/05 */
extern int logall;          /* 0 = warnings/errors, 1 = all */
extern FILE *logfile;           /* pointer to open logfile or NULL */
extern int logfile_append;      /* append to existing logfile */
extern char *logfile_path;      /* pointer to path of logfile */
#ifdef WIN32
extern int nonlocal_name;       /* Name has non-local characters */
extern int nonlocal_path;       /* Path has non-local characters */
#endif
#ifdef UNICODE_SUPPORT
/* Unicode 10/12/05 */
extern int use_wide_to_mb_default;/* use the default MB char instead of escape */
#endif

extern int hidden_files;        /* process hidden and system files */
extern int volume_label;        /* add volume label */
extern int dirnames;            /* include directory names */
extern int filter_match_case;   /* 1=match case when filter() */
extern int diff_mode;           /* 1=require --out and only store changed and add */
#if defined(WIN32)
extern int only_archive_set;    /* only include if DOS archive bit set */
extern int clear_archive_bits;   /* clear DOS archive bit of included files */
#endif
extern int linkput;             /* Store symbolic links as such */
extern int noisy;               /* False for quiet operation */
extern int extra_fields;        /* 0=create minimum, 1=don't copy old, 2=keep old */
#ifdef NTSD_EAS
 extern int use_privileges;     /* use security privilege overrides */
#endif
extern int use_descriptors;     /* use data descriptors (extended headings) */
extern int allow_empty_archive; /* if no files, create empty archive anyway */
extern int copy_only;           /* 1 = copy archive with no changes */
extern int zip_to_stdout;       /* output to stdout */
extern int output_seekable;     /* 1 = output seekable 3/13/05 EG */
#ifdef ZIP64_SUPPORT            /* zip64 globals 10/4/03 E. Gordon */
 extern int force_zip64;        /* force use of zip64 when streaming from stdin */
 extern int zip64_entry;        /* current entry needs Zip64 */
 extern int zip64_archive;      /* at least 1 entry needs zip64 */
#endif
extern int allow_fifo;          /* Allow reading Unix FIFOs, waiting if pipe open */
extern int show_files;          /* show files to operate on and exit (=2 log only) */

extern char *tempzip;           /* temp file name */
extern FILE *y;                 /* output file now global for splits */

#ifdef UNICODE_SUPPORT
  extern int utf8_force;         /* 1=store UTF-8 as standard per AppNote bit 11 */
#endif
extern int unicode_escape_all;  /* 1=escape all non-ASCII characters in paths */
extern int unicode_mismatch;    /* unicode mismatch is 0=error, 1=warn, 2=ignore, 3=no */

extern time_t scan_delay;       /* seconds before display Scanning files message */
extern time_t scan_dot_time;    /* time in seconds between Scanning files dots */
extern time_t scan_start;       /* start of file scan */
extern time_t scan_last;        /* time of last message */
extern int scan_started;        /* scan has started */
extern uzoff_t scan_count;      /* Used for "Scanning files..." message */

extern ulg before;              /* 0=ignore, else exclude files before this time */
extern ulg after;               /* 0=ignore, else exclude files newer than this time */

/* in split globals */

extern ulg total_disks;

extern ulg current_in_disk;
extern uzoff_t current_in_offset;
extern ulg skip_current_disk;


/* out split globals */

extern ulg    current_local_disk; /* disk with current local header */

extern ulg     current_disk;     /* current disk number */
extern ulg     cd_start_disk;    /* central directory start disk */
extern uzoff_t cd_start_offset;  /* offset of start of cd on cd start disk */
extern uzoff_t cd_entries_this_disk; /* cd entries this disk */
extern uzoff_t total_cd_entries; /* total cd entries in new/updated archive */
extern ulg     zip64_eocd_disk;  /* disk with Zip64 EOCD Record */
extern uzoff_t zip64_eocd_offset; /* offset of Zip64 EOCD Record */
/* for split method 1 (keep split with local header open and update) */
extern char *current_local_tempname; /* name of temp file */
extern FILE  *current_local_file; /* file pointer for current local header */
extern uzoff_t current_local_offset; /* offset to start of current local header */
/* global */
extern uzoff_t bytes_this_split; /* bytes written to current split */
extern int read_split_archive;   /* 1=scanzipf_reg detected spanning signature */
extern int split_method;         /* 0=no splits, 1=seekable, 2=data descs, -1=no */
extern uzoff_t split_size;       /* how big each split should be */
extern int split_bell;           /* when pause for next split ring bell */
extern uzoff_t bytes_prev_splits; /* total bytes written to all splits before this */
extern uzoff_t bytes_this_entry; /* bytes written for this entry across all splits */
extern int noisy_splits;         /* note when splits are being created */
extern int mesg_line_started;    /* 1=started writing a line to mesg */
extern int logfile_line_started; /* 1=started writing a line to logfile */
extern char *key;               /* Scramble password or NULL */
extern char *tempath;           /* Path for temporary files */
extern FILE *mesg;              /* Where informational output goes */
extern char *zipfile;           /* New or existing zip archive (zip file) */
extern FILE *in_file;           /* Current input file for spits */
extern char *in_path;           /* Name of input archive, used to track reading splits */
extern char *in_split_path;     /* in split path */
extern char *out_path;          /* Name of output file, usually same as zipfile */
extern int zip_attributes;

/* zip64 support 08/31/2003 R.Nausedat */
extern uzoff_t zipbeg;          /* Starting offset of zip structures */
extern uzoff_t cenbeg;          /* Starting offset of central directory */
extern uzoff_t tempzn;          /* Count of bytes written to output zip file */

/* NOTE: zcount and fcount cannot exceed "size_t" (resp. "extent") range.
   This is an internal limitation built into Zip's action handling:
   Zip keeps "{z|f}count * struct {z|f}list" arrays in (flat) memory,
   for sorting, file matching, and building the central-dir structures.
 */

extern struct zlist far *zfiles;/* Pointer to list of files in zip file */
extern extent zcount;           /* Number of files in zip file */
extern int zipfile_exists;      /* 1 if zipfile exists */
extern ush zcomlen;             /* Length of zip file comment */
extern char *zcomment;          /* Zip file comment (not zero-terminated) */
extern struct flist far **fsort;/* List of files sorted by name */
extern struct zlist far **zsort;/* List of files sorted by name */
#ifdef UNICODE_SUPPORT
extern struct zlist far **zusort;/* List of files sorted by zuname */
#endif
extern struct flist far *found; /* List of names found */
extern struct flist far *far *fnxt;     /* Where to put next in found list */
extern extent fcount;           /* Count of names in found list */

extern struct plist *patterns;  /* List of patterns to be matched */
extern unsigned pcount;         /* number of patterns */
extern unsigned icount;         /* number of include only patterns */
extern unsigned Rcount;         /* number of -R include patterns */

#ifdef IZ_CHECK_TZ
extern int zp_tz_is_valid;      /* signals "timezone info is available" */
#endif
#if (defined(MACOS) || defined(WINDLL))
extern int zipstate;            /* flag "zipfile has been stat()'ed */
#endif

/* Diagnostic functions */
#ifdef DEBUG
# ifdef MSDOS
#  undef  stderr
#  define stderr stdout
# endif
#  define diag(where) fprintf(stderr, "zip diagnostic: %s\n", where)
#  define Assert(cond,msg) {if(!(cond)) error(msg);}
# ifdef THEOS
#  define Trace(x) _fprintf x
#  define Tracev(x) {if (verbose) _fprintf x ;}
#  define Tracevv(x) {if (verbose>1) _fprintf x ;}
#  define Tracec(c,x) {if (verbose && (c)) _fprintf x ;}
#  define Tracecv(c,x) {if (verbose>1 && (c)) _fprintf x ;}
# else
#  define Trace(x) fprintf x
#  define Tracev(x) {if (verbose) fprintf x ;}
#  define Tracevv(x) {if (verbose>1) fprintf x ;}
#  define Tracec(c,x) {if (verbose && (c)) fprintf x ;}
#  define Tracecv(c,x) {if (verbose>1 && (c)) fprintf x ;}
# endif
#else
#  define diag(where)
#  define Assert(cond,msg)
#  define Trace(x)
#  define Tracev(x)
#  define Tracevv(x)
#  define Tracec(c,x)
#  define Tracecv(c,x)
#endif

#ifdef DEBUGNAMES
#  define free(x) { int *v;Free(x); v=x;*v=0xdeadbeef;x=(void *)0xdeadbeef; }
#endif

/* Public function prototypes */

#ifndef UTIL
#ifdef USE_ZIPMAIN
int zipmain OF((int, char **));
#else
int main OF((int, char **));
#endif /* USE_ZIPMAIN */
#endif

#ifdef EBCDIC
extern int aflag;
#endif /* EBCDIC */
#ifdef CMS_MVS
extern int bflag;
#endif /* CMS_MVS */
void zipmessage_nl OF((ZCONST char *, int));
void zipmessage OF((ZCONST char *, ZCONST char *));
void zipwarn OF((ZCONST char *, ZCONST char *));
void ziperr OF((int, ZCONST char *));
#ifdef UTIL
#  define error(msg)    ziperr(ZE_LOGIC, msg)
#else
   void error OF((ZCONST char *));
#  ifdef VMSCLI
     void help OF((void));
#  endif
   int encr_passwd OF((int, char *, int, ZCONST char *));
#endif

        /* in zipup.c */
#ifndef UTIL
  /* zip64 support 08/31/2003 R.Nausedat */
   int percent OF((uzoff_t, uzoff_t));

   int zipup OF((struct zlist far *));
#  ifdef USE_ZLIB
     void zl_deflate_free OF((void));
#  else
     void flush_outbuf OF((char *, unsigned *));
     int seekable OF((void));
     extern unsigned (*read_buf) OF((char *, unsigned int));
#  endif /* !USE_ZLIB */
#  ifdef ZP_NEED_MEMCOMPR
     ulg memcompress OF((char *, ulg, char *, ulg));
#  endif
#  ifdef BZIP2_SUPPORT
   void bz_compress_free OF((void));
#  endif
#endif /* !UTIL */

        /* in zipfile.c */
#ifndef UTIL
   struct zlist far *zsearch OF((ZCONST char *));
#  ifdef USE_EF_UT_TIME
     int get_ef_ut_ztime OF((struct zlist far *, iztimes *));
#  endif /* USE_EF_UT_TIME */
   int trash OF((void));
#endif /* !UTIL */
char *ziptyp OF((char *));
int readzipfile OF((void));
int putlocal OF((struct zlist far *, int));
int putextended OF((struct zlist far *));
int putcentral OF((struct zlist far *));
/* zip64 support 09/05/2003 R.Nausedat */
int putend OF((uzoff_t, uzoff_t, uzoff_t, extent, char *));
/* moved seekable to separate function 3/14/05 EG */
int is_seekable OF((FILE *));
int zipcopy OF((struct zlist far *));
int readlocal OF((struct zlist far **, struct zlist far *));
/* made global for handling extra fields */
char *get_extra_field OF((ush, char *, unsigned));
char *copy_nondup_extra_fields OF((char *, unsigned, char *, unsigned, unsigned *));

        /* in fileio.c */
#ifndef UTIL
   char *getnam OF((FILE *));
   struct flist far *fexpel OF((struct flist far *));
   char *last OF((char *, int));
# ifdef UNICODE_SUPPORT
   wchar_t *lastw OF((wchar_t *, wchar_t));
# endif
   char *msname OF((char *));
# ifdef UNICODE_SUPPORT
   wchar_t *msnamew OF((wchar_t *));
# endif
   int check_dup OF((void));
   int filter OF((char *, int));
   int newname OF((char *, int, int));
# ifdef UNICODE_SUPPORT
#  ifdef WIN32
   int newnamew OF((wchar_t *, int, int));
#  endif
# endif
   /* used by copy mode */
   int proc_archive_name OF((char *, int));
#endif /* !UTIL */
#if (!defined(UTIL) || defined(W32_STATROOT_FIX))
   time_t dos2unixtime OF((ulg));
#endif
#ifndef UTIL
   ulg dostime OF((int, int, int, int, int, int));
   ulg unix2dostime OF((time_t *));
   int issymlnk OF((ulg a));
#  ifdef S_IFLNK
#    define rdsymlnk(p,b,n) readlink(p,b,n)
/*   extern int readlink OF((char *, char *, int)); */
#  else /* !S_IFLNK */
#    define rdsymlnk(p,b,n) (0)
#  endif /* !S_IFLNK */
#endif /* !UTIL */

int destroy OF((char *));
int replace OF((char *, char *));
int getfileattr OF((char *));
int setfileattr OF((char *, int));
char *tempname OF((char *));

/* for splits */
int close_split OF((ulg, FILE *, char *));
int ask_for_split_read_path OF((ulg));
int ask_for_split_write_path OF((ulg));
char *get_in_split_path OF((char *, ulg));
char *find_in_split_path OF((char *, ulg));
char *get_out_split_path OF((char *, ulg));
int rename_split OF((char *, char *));
int set_filetype OF((char *));

int bfcopy OF((uzoff_t));

int fcopy OF((FILE *, FILE *, uzoff_t));

#ifdef ZMEM
   char *memset OF((char *, int, unsigned int));
   char *memcpy OF((char *, char *, unsigned int));
   int memcmp OF((char *, char *, unsigned int));
#endif /* ZMEM */

        /* in system dependent fileio code (<system>.c) */
#ifndef UTIL
# ifdef PROCNAME
   int wild OF((char *));
# endif
   char *in2ex OF((char *));
   char *ex2in OF((char *, int, int *));
#if defined(UNICODE_SUPPORT) && defined(WIN32)
   int has_win32_wide OF((void));
   wchar_t *in2exw OF((wchar_t *));
   wchar_t *ex2inw OF((wchar_t *, int, int *));
   int procnamew OF((wchar_t *, int));
#endif
   int procname OF((char *, int));
   void stamp OF((char *, ulg));

   ulg filetime OF((char *, ulg *, zoff_t *, iztimes *));
   /* Windows Unicode */
# ifdef UNICODE_SUPPORT
# ifdef WIN32
   ulg filetimew OF((wchar_t *, ulg *, zoff_t *, iztimes *));
   char *get_win32_utf8path OF((char *));
   wchar_t *local_to_wchar_string OF ((char *));
# endif
# endif

# if !(defined(VMS) && defined(VMS_PK_EXTRA))
   int set_extra_field OF((struct zlist far *, iztimes *));
# endif /* ?(VMS && VMS_PK_EXTRA) */
   int deletedir OF((char *));
# ifdef MY_ZCALLOC
     zvoid far *zcalloc OF((unsigned int, unsigned int));
     zvoid zcfree       OF((zvoid far *));
# endif /* MY_ZCALLOC */
#endif /* !UTIL */
void version_local OF((void));

        /* in util.c */
#ifndef UTIL
int   fseekable    OF((FILE *));
char *isshexp      OF((char *));
#ifdef UNICODE_SUPPORT
# ifdef WIN32
   wchar_t *isshexpw     OF((wchar_t *));
   int dosmatchw   OF((ZCONST wchar_t *, ZCONST wchar_t *, int));
# endif
#endif
int   shmatch      OF((ZCONST char *, ZCONST char *, int));
# if defined(DOS) || defined(WIN32)
   int dosmatch    OF((ZCONST char *, ZCONST char *, int));
# endif /* DOS || WIN32 */
#endif /* !UTIL */

/* functions to convert zoff_t to a string */
char *zip_fuzofft      OF((uzoff_t, char *, char*));
char *zip_fzofft       OF((zoff_t, char *, char*));

/* read and write number strings like 10M */
int DisplayNumString OF ((FILE *file, uzoff_t i));
int WriteNumString OF((uzoff_t num, char *outstring));
uzoff_t ReadNumString OF((char *numstring));

/* returns true if abbrev is abbreviation for string */
int abbrevmatch OF((char *, char *, int, int));

void init_upper    OF((void));
int  namecmp       OF((ZCONST char *string1, ZCONST char *string2));

#ifdef EBCDIC
  char *strtoasc     OF((char *str1, ZCONST char *str2));
  char *strtoebc     OF((char *str1, ZCONST char *str2));
  char *memtoasc     OF((char *mem1, ZCONST char *mem2, unsigned len));
  char *memtoebc     OF((char *mem1, ZCONST char *mem2, unsigned len));
#endif /* EBCDIC */
#ifdef IZ_ISO2OEM_ARRAY
  char *str_iso_to_oem    OF((char *dst, ZCONST char *src));
#endif
#ifdef IZ_OEM2ISO_ARRAY
  char *str_oem_to_iso    OF((char *dst, ZCONST char *src));
#endif

zvoid far **search OF((ZCONST zvoid *, ZCONST zvoid far **, extent,
                       int (*)(ZCONST zvoid *, ZCONST zvoid far *)));
void envargs       OF((int *, char ***, char *, char *));
void expand_args   OF((int *, char ***));

int  is_text_buf   OF((ZCONST char *buf_ptr, unsigned buf_size));
/* this is no longer used ...
unsigned int adler16 OF((unsigned int, ZCONST uch *, extent));
*/
        /*  crc functions are now declared in crc32.h */

#ifndef UTIL
#ifndef USE_ZLIB
        /* in deflate.c */
void lm_init OF((int, ush *));
void lm_free OF((void));

uzoff_t deflate OF((void));

        /* in trees.c */
void     ct_init      OF((ush *, int *));
int      ct_tally     OF((int, int));
uzoff_t  flush_block  OF((char far *, ulg, int));
void     bi_init      OF((char *, unsigned int, int));
#endif /* !USE_ZLIB */
#endif /* !UTIL */

        /* in system specific assembler code, replacing C code in trees.c */
#if defined(ASMV) && defined(RISCOS)
  void     send_bits    OF((int, int));
  unsigned bi_reverse   OF((unsigned int, int));
#endif /* ASMV && RISCOS */

/*---------------------------------------------------------------------------
    VMS-only functions:
  ---------------------------------------------------------------------------*/
#ifdef VMS
   int    vms_stat        OF((char *, stat_t *));              /* vms.c */
   void   vms_exit        OF((int));                           /* vms.c */
#ifndef UTIL
#ifdef VMSCLI
   ulg    vms_zip_cmdline OF((int *, char ***));                /* cmdline.c */
   void   VMSCLI_help     OF((void));                           /* cmdline.c */
#endif /* VMSCLI */
#endif /* !UTIL */
#endif /* VMS */

/*
#ifdef ZIP64_SUPPORT
   update_local_Zip64_extra_field OF((struct zlist far *, FILE *));
#endif
*/

/*---------------------------------------------------------------------------
    WIN32-only functions:
  ---------------------------------------------------------------------------*/
#ifdef WIN32
   int ZipIsWinNT         OF((void));                         /* win32.c */
   int ClearArchiveBit    OF((char *));                       /* win32.c */
# ifdef UNICODE_SUPPORT
   int ClearArchiveBitW   OF((wchar_t *));                    /* win32.c */
# endif
#endif /* WIN32 */

#if (defined(WINDLL) || defined(DLL_ZIPAPI))
/*---------------------------------------------------------------------------
    Prototypes for public Zip API (DLL) functions.
  ---------------------------------------------------------------------------*/
#include "api.h"
#endif /* WINDLL || DLL_ZIPAPI */


   /* WIN32_OEM */
#ifdef WIN32
/*
# if defined(UNICODE_SUPPORT) || defined(WIN32_OEM)
*/
  /* convert oem to ansi string */
  char *oem_to_local_string OF((char *, char *));
/*
# endif
*/
#endif

#ifdef WIN32
/*
# if defined(UNICODE_SUPPORT) || defined(WIN32_OEM)
*/
  /* convert local string to oem string */
  char *local_to_oem_string OF((char *, char *));
/*
# endif
*/
#endif



/*---------------------------------------------------------------------
    Unicode Support
    28 August 2005
  ---------------------------------------------------------------------*/
#ifdef UNICODE_SUPPORT

  /* Default character when a zwchar too big for wchar_t */
# define zwchar_to_wchar_t_default_char '_'

  /* Default character string when wchar_t does not convert to mb */
# define wide_to_mb_default_string "_"

  /* wide character type */
  typedef unsigned long zwchar;

  /* check if string is all ASCII */
  int is_ascii_string OF((char *));
#ifdef WIN32
  int is_ascii_stringw OF((wchar_t *));
  zwchar *wchar_to_wide_string OF((wchar_t *));
#endif

  /* convert UTF-8 string to multi-byte string */
  char *utf8_to_local_string OF((char *));
  char *utf8_to_escape_string OF((char *));

  /* convert UTF-8 string to wide string */
  zwchar *utf8_to_wide_string OF((char *));

  /* convert wide string to multi-byte string */
  char *wide_to_local_string OF((zwchar *));
  char *wide_to_escape_string OF((zwchar *));
  char *local_to_escape_string OF((char *));
#ifdef WIN32
  /* convert UTF-8 to wchar */
  wchar_t *utf8_to_wchar_string OF ((char *));

  char *wchar_to_local_string OF((wchar_t *));
#endif

  /* convert local string to multi-byte display string */
  char *local_to_display_string OF((char *));

  /* convert wide character to escape string */
  char *wide_char_to_escape_string OF((unsigned long));

#if 0
  /* convert escape string to wide character */
  unsigned long escape_string_to_wide OF((char *));
#endif

  /* convert local to UTF-8 */
  char *local_to_utf8_string OF ((char *));

  /* convert local to wide string */
  zwchar *local_to_wide_string OF ((char *));

  /* convert wide string to UTF-8 */
  char *wide_to_utf8_string OF((zwchar *));
#ifdef WIN32
  char *wchar_to_utf8_string OF((wchar_t *));
#endif

#endif /* UNICODE_SUPPORT */


/*---------------------------------------------------
 * Split archives
 *
 * 10/20/05 EG
 */

#define BFWRITE_DATA 0
#define BFWRITE_LOCALHEADER 1
#define BFWRITE_CENTRALHEADER 2
#define BFWRITE_HEADER 3 /* data descriptor or end records */

size_t bfwrite OF((ZCONST void *buffer, size_t size, size_t count,
                   int));

/* for putlocal() */
#define PUTLOCAL_WRITE 0
#define PUTLOCAL_REWRITE 1


/*--------------------------------------------------------------------
    Long option support
    23 August 2003
    See fileio.c
  --------------------------------------------------------------------*/

/* The below is for use in the caller-provided options table */

/* value_type - value is always returned as a string. */
#define o_NO_VALUE        0   /* this option does not take a value */
#define o_REQUIRED_VALUE  1   /* this option requires a value */
#define o_OPTIONAL_VALUE  2   /* value is optional (see get_option() for details) */
#define o_VALUE_LIST      3   /* this option takes a list of values */
#define o_ONE_CHAR_VALUE  4   /* next char is value (does not end short opt string) */
#define o_NUMBER_VALUE    5   /* value is integer (does not end short opt string) */


/* negatable - a dash following the option (but before any value) sets negated. */
#define o_NOT_NEGATABLE   0   /* trailing '-' to negate either starts value or generates error */
#define o_NEGATABLE       1   /* trailing '-' sets negated to TRUE */


/* option_num can be this when option not in options table */
#define o_NO_OPTION_MATCH     -1

/* special values returned by get_option - do not use these as option IDs */
#define o_NON_OPTION_ARG      ((unsigned long) 0xFFFF)    /* returned for non-option
                                                             args */
#define o_ARG_FILE_ERR        ((unsigned long) 0xFFFE)    /* internal recursion
                                                             return (user never sees) */

/* options array is set in zip.c */
struct option_struct {
  char *shortopt;           /* char * to sequence of char that is short option */
  char Far *longopt;        /* char * to long option string */
  int  value_type;          /* from above */
  int  negatable;           /* from above */
  unsigned long option_ID;  /* value returned by get_option when this option is found */
  char Far *name;           /* optional string for option returned on some errors */
};
extern struct option_struct far options[];


/* moved here from fileio.c to make global - 10/6/05 EG */

/* If will support wide for Unicode then need to add */
  /* multi-byte */
#ifdef _MBCS
# ifndef MULTIBYTE_GETOPTNS
#   define MULTIBYTE_GETOPTNS
# endif
#endif
#ifdef MULTIBYTE_GETOPTNS
  int mb_clen OF((ZCONST char *));
# define MB_CLEN(ptr) mb_clen(ptr)
# define MB_NEXTCHAR(ptr) ((ptr) += MB_CLEN(ptr))
#else
  /* no multi-byte */
# define MB_CLEN(ptr) (1)
# define MB_NEXTCHAR(ptr) ((ptr)++)
#endif


/* function prototypes */

/* get the next option from args */
unsigned long get_option OF((char ***pargs, int *argc, int *argnum, int *optchar,
                             char **value, int *negated, int *first_nonopt_arg,
                             int *option_num, int recursion_depth));

/* copy args - copy an args array, allocating space as needed */
char **copy_args OF((char **args, int max_args));

/* free args - free args created with one of these functions */
int free_args OF ((char **args));

/* insert arg - copy an arg into args */
int insert_arg OF ((char ***args, ZCONST char *arg, int insert_at,
                    int free_args));


/*--------------------------------------------------------------------
    End of Long option support
  --------------------------------------------------------------------*/


#endif /* !__zip_h */
/* end of zip.h */
