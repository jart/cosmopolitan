/*
  globals.c - Zip 3

  Copyright (c) 1990-2008 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2007-Mar-4 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*
 *  globals.c by Mark Adler
 */
#define __GLOBALS_C

#define GLOBALS         /* include definition of errors[] in zip.h */
#ifndef UTIL
#define UTIL            /* do not declare the read_buf variable */
#endif

#include "zip.h"


/* Handy place to build error messages */
char errbuf[FNMAX+4081];

/* Argument processing globals */
int recurse = 0;        /* 1=recurse into directories encountered */
int dispose = 0;        /* 1=remove files after put in zip file */
int pathput = 1;        /* 1=store path with name */
#ifdef RISCOS
int scanimage = 1;      /* 1=scan through image files */
#endif
int method = BEST;      /* one of BEST, DEFLATE (only), or STORE (only) */
int dosify = 0;         /* 1=make new entries look like MSDOS */
int verbose = 0;        /* 1=report oddities in zip file structure */
int fix = 0;            /* 1=fix the zip file, 2=FF, 3=ZipNote */
int filesync = 0;       /* 1=file sync, delete entries not on file system */
int adjust = 0;         /* 1=adjust offsets for sfx'd file (keep preamble) */
int level = 6;          /* 0=fastest compression, 9=best compression */
int translate_eol = 0;  /* Translate end-of-line LF -> CR LF */
#ifdef VMS
   int vmsver = 0;      /* 1=append VMS version number to file names */
   int vms_native = 0;  /* 1=store in VMS format */
   int vms_case_2 = 0;  /* ODS2 file name case in VMS.  -1: down. */
   int vms_case_5 = 0;  /* ODS5 file name case in VMS.  +1: preserve. */
#endif /* VMS */
#if defined(OS2) || defined(WIN32)
   int use_longname_ea = 0;  /* 1=use the .LONGNAME EA as the file's name */
#endif
/* 9/26/04 */
int no_wild = 0;             /* 1 = wildcards are disabled */
int allow_regex = 0;         /* 1 = allow [list] matching */
#ifdef WILD_STOP_AT_DIR
   int wild_stop_at_dir = 1; /* default wildcards do not include / in matches */
#else
   int wild_stop_at_dir = 0; /* default wildcards do include / in matches */
#endif

#ifdef UNICODE_SUPPORT
   int using_utf8 = 0;       /* 1 if current character set UTF-8 */
# ifdef WIN32
   int no_win32_wide = -1; /* 1 = no wide functions, like GetFileAttributesW() */
# endif
#endif

ulg skip_this_disk = 0;
int des_good = 0;       /* Good data descriptor found */
ulg des_crc = 0;        /* Data descriptor CRC */
uzoff_t des_csize = 0;  /* Data descriptor csize */
uzoff_t des_usize = 0;  /* Data descriptor usize */

/* dots 10/20/04 */
zoff_t dot_size = 0;          /* bytes processed in deflate per dot, 0 = no dots */
zoff_t dot_count = 0;         /* buffers seen, recyles at dot_size */
/* status 10/30/04 */
int display_counts = 0;       /* display running file count */
int display_bytes = 0;        /* display running bytes remaining */
int display_globaldots = 0;   /* display dots for archive instead of each file */
int display_volume = 0;       /* display current input and output volume (disk) numbers */
int display_usize = 0;        /* display uncompressed bytes */
ulg files_so_far = 0;         /* files processed so far */
ulg bad_files_so_far = 0;     /* bad files skipped so far */
ulg files_total = 0;          /* files total to process */
uzoff_t bytes_so_far = 0;     /* bytes processed so far (from initial scan) */
uzoff_t good_bytes_so_far = 0;/* good bytes read so far */
uzoff_t bad_bytes_so_far = 0; /* bad bytes skipped so far */
uzoff_t bytes_total = 0;      /* total bytes to process (from initial scan) */

/* logfile 6/5/05 */
int logall = 0;               /* 0 = warnings/errors, 1 = all */
FILE *logfile = NULL;         /* pointer to open logfile or NULL */
int logfile_append = 0;       /* append to existing logfile */
char *logfile_path = NULL;    /* pointer to path of logfile */

int hidden_files = 0;         /* process hidden and system files */
int volume_label = 0;         /* add volume label */
int dirnames = 1;             /* include directory entries by default */
int filter_match_case = 1;    /* 1=match case when filter() */
int diff_mode = 0;            /* 1=require --out and only store changed and add */
#if defined(WIN32)
int only_archive_set = 0;     /* include only files with DOS archive bit set */
int clear_archive_bits = 0;   /* clear DOS archive bit of included files */
#endif
int linkput = 0;              /* 1=store symbolic links as such */
int noisy = 1;                /* 0=quiet operation */
int extra_fields = 1;         /* 0=create minimum, 1=don't copy old, 2=keep old */
int use_descriptors = 0;      /* 1=use data descriptors 12/29/04 */
int zip_to_stdout = 0;        /* output zipfile to stdout 12/30/04 */
int allow_empty_archive = 0;  /* if no files, create empty archive anyway 12/28/05 */
int copy_only = 0;            /* 1=copying archive entries only */
int allow_fifo = 0;           /* 1=allow reading Unix FIFOs, waiting if pipe open */
int show_files = 0;           /* show files to operate on and exit (=2 log only) */

int output_seekable = 1;      /* 1 = output seekable 3/13/05 EG */

#ifdef ZIP64_SUPPORT          /* zip64 support 10/4/03 */
  int force_zip64 = -1;       /* if 1 force entries to be zip64, 0 force not zip64 */
                              /* mainly for streaming from stdin */
  int zip64_entry = 0;        /* current entry needs Zip64 */
  int zip64_archive = 0;      /* if 1 then at least 1 entry needs zip64 */
#endif

#ifdef NTSD_EAS
  int use_privileges = 0;     /* 1=use security privilege overrides */
#endif
#ifndef RISCOS
#ifndef QDOS
#ifndef TANDEM
char *special = ".Z:.zip:.zoo:.arc:.lzh:.arj"; /* List of special suffixes */
#else /* TANDEM */
char *special = " Z: zip: zoo: arc: lzh: arj"; /* List of special suffixes */
#endif
#else /* QDOS */
char *special = "_Z:_zip:_zoo:_arc:_lzh:_arj"; /* List of special suffixes */
#endif
#else /* RISCOS */
char *special = "DDC:D96:68E";
#endif /* ?RISCOS */
char *key = NULL;       /* Scramble password if scrambling */
char *tempath = NULL;   /* Path for temporary files */
FILE *mesg;             /* stdout by default, stderr for piping */

#ifdef UNICODE_SUPPORT
 int utf8_force = 0;    /* 1=force storing UTF-8 as standard per AppNote bit 11 */
#endif
int unicode_escape_all = 0; /* 1=escape all non-ASCII characters in paths */
int unicode_mismatch = 1; /* unicode mismatch is 0=error, 1=warn, 2=ignore, 3=no */

time_t scan_delay = 5;  /* seconds before display Scanning files message */
time_t scan_dot_time = 2; /* time in seconds between Scanning files dots */
time_t scan_start = 0;  /* start of scan */
time_t scan_last = 0;   /* time of last message */
int scan_started = 0;   /* scan has started */
uzoff_t scan_count = 0; /* Used for Scanning files ... message */

ulg before = 0;         /* 0=ignore, else exclude files before this time */
ulg after = 0;          /* 0=ignore, else exclude files newer than this time */

/* Zip file globals */
char *zipfile;          /* New or existing zip archive (zip file) */

/* zip64 support 08/31/2003 R.Nausedat */
/* all are across splits - subtract bytes_prev_splits to get offsets for current disk */
uzoff_t zipbeg;               /* Starting offset of zip structures */
uzoff_t cenbeg;               /* Starting offset of central dir */
uzoff_t tempzn;               /* Count of bytes written to output zip files */

/* 10/28/05 */
char *tempzip = NULL;         /* name of temp file */
FILE *y = NULL;               /* output file now global so can change in splits */
FILE *in_file = NULL;         /* current input file for splits */
char *in_path = NULL;         /* base name of input archive file */
char *in_split_path = NULL;   /* in split path */
char *out_path = NULL;        /* base name of output file, usually same as zipfile */
int zip_attributes = 0;

/* in split globals */

ulg     total_disks = 0;        /* total disks in archive */
ulg     current_in_disk = 0;    /* current read split disk */
uzoff_t current_in_offset = 0;  /* current offset in current read disk */
ulg     skip_current_disk = 0;  /* if != 0 and fix then skip entries on this disk */


/* out split globals */

ulg     current_local_disk = 0;   /* disk with current local header */

ulg     current_disk = 0;         /* current disk number */
ulg     cd_start_disk = (ulg)-1;  /* central directory start disk */
uzoff_t cd_start_offset = 0;      /* offset of start of cd on cd start disk */
uzoff_t cd_entries_this_disk = 0; /* cd entries this disk */
uzoff_t total_cd_entries = 0;     /* total cd entries in new/updated archive */
ulg     zip64_eocd_disk = 0;      /* disk with Zip64 End Of Central Directory Record */
uzoff_t zip64_eocd_offset = 0;    /* offset for Zip64 EOCD Record */

/* for split method 1 (keep split with local header open and update) */
char *current_local_tempname = NULL; /* name of temp file */
FILE  *current_local_file = NULL; /* file pointer for current local header */
uzoff_t current_local_offset = 0; /* offset to start of current local header */

/* global */
uzoff_t bytes_this_split = 0;     /* bytes written to the current split */
int read_split_archive = 0;       /* 1=scanzipf_reg detected spanning signature */
int split_method = 0;             /* 0=no splits, 1=seekable, 2=data desc, -1=no */
uzoff_t split_size = 0;           /* how big each split should be */
int split_bell = 0;               /* when pause for next split ring bell */
uzoff_t bytes_prev_splits = 0;    /* total bytes written to all splits before this */
uzoff_t bytes_this_entry = 0;     /* bytes written for this entry across all splits */
int noisy_splits = 0;             /* note when splits are being created */
int mesg_line_started = 0;        /* 1=started writing a line to mesg */
int logfile_line_started = 0;     /* 1=started writing a line to logfile */

#ifdef WIN32
  int nonlocal_name = 0;          /* Name has non-local characters */
  int nonlocal_path = 0;          /* Path has non-local characters */
#endif
#ifdef UNICODE_SUPPORT
  int use_wide_to_mb_default = 0;
#endif

struct zlist far *zfiles = NULL;  /* Pointer to list of files in zip file */
/* The limit for number of files using the Zip64 format is 2^64 - 1 (8 bytes)
   but extent is used for many internal sorts and other tasks and is generally
   long on 32-bit systems.  Because of that, but more because of various memory
   utilization issues limiting the practical number of central directory entries
   that can be sorted, the number of actual entries that can be stored probably
   can't exceed roughly 2^30 on 32-bit systems so extent is probably sufficient. */
extent zcount;                    /* Number of files in zip file */
int zipfile_exists = 0;           /* 1 if zipfile exists */
ush zcomlen;                      /* Length of zip file comment */
char *zcomment = NULL;            /* Zip file comment (not zero-terminated) */
struct zlist far **zsort;         /* List of files sorted by name */
#ifdef UNICODE_SUPPORT
  struct zlist far **zusort;      /* List of files sorted by zuname */
#endif

/* Files to operate on that are not in zip file */
struct flist far *found = NULL;   /* List of names found */
struct flist far * far *fnxt = &found;
                                  /* Where to put next name in found list */
extent fcount;                    /* Count of files in list */

/* Patterns to be matched */
struct plist *patterns = NULL;  /* List of patterns to be matched */
unsigned pcount = 0;            /* number of patterns */
unsigned icount = 0;            /* number of include only patterns */
unsigned Rcount = 0;            /* number of -R include patterns */

#ifdef IZ_CHECK_TZ
int zp_tz_is_valid;     /* signals "timezone info is available" */
#endif
