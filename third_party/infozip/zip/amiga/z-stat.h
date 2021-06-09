/*
  Copyright (c) 1990-2000 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
#ifndef __amiga_z_stat_h
#define __amiga_z_stat_h

/* Since older versions of the Lattice C compiler for Amiga, and all current */
/* versions of the Manx Aztec C compiler for Amiga, either provide no stat() */
/* function or provide one inadequate for unzip (Aztec's has no st_mode      */
/* field), we provide our own stat() function in stat.c by Paul Wells, and   */
/* this fake stat.h file by Paul Kienitz.  Paul Wells originally used the    */
/* Lattice stat.h but that does not work for Aztec and is not distributable  */
/* with this package, so I made a separate one.  This has to be pulled into  */
/* unzip.h when compiling an Amiga version, as "amiga/z-stat.h".             */

/* We also provide here a "struct dirent" for use with opendir() & readdir() */
/* functions included in amiga/stat.c.  If you use amiga/stat.c, this must   */
/* be included wherever you use either readdir() or stat().                  */

#ifdef AZTEC_C
#  define __STAT_H
#else  /* __SASC */
/* do not include the following header, replacement definitions are here */
#  define _STAT_H      /* do not include SAS/C <stat.h> */
#  define _DIRENT_H    /* do not include SAS/C <dirent.h> */
#  define _SYS_DIR_H   /* do not include SAS/C <sys/dir.h> */
#  define _COMMIFMT_H  /* do not include SAS/C <sys/commifmt.h> */
#  include <dos.h>
#endif
#include <libraries/dos.h>
#include <time.h>


struct stat {
    unsigned short st_mode;
    time_t st_ctime, st_atime, st_mtime;
    long st_size;
    long st_ino;
    long st_blocks;
    short st_attr, st_dev, st_nlink, st_uid, st_gid, st_rdev;
};

#define S_IFDIR  (1<<11)
#define S_IFREG  (1<<10)

#if 0
   /* these values here are totally random: */
#  define S_IFLNK  (1<<14)
#  define S_IFSOCK (1<<13)
#  define S_IFCHR  (1<<8)
#  define S_IFIFO  (1<<7)
#  define S_IFMT   (S_IFDIR|S_IFREG|S_IFCHR|S_IFLNK)
#else
#  define S_IFMT   (S_IFDIR|S_IFREG)
#endif

#define S_IHIDDEN    (1<<7)
#define S_ISCRIPT    (1<<6)
#define S_IPURE      (1<<5)
#define S_IARCHIVE   (1<<4)
#define S_IREAD      (1<<3)
#define S_IWRITE     (1<<2)
#define S_IEXECUTE   (1<<1)
#define S_IDELETE    (1<<0)

int stat(const char *name, struct stat *buf);
int fstat(int handle, struct stat *buf);      /* returns dummy values */

typedef struct dirent {
    struct dirent       *d_cleanuplink,
                       **d_cleanupparent;
    BPTR                 d_parentlock;
    struct FileInfoBlock d_fib;
} DIR;
#define                  d_name  d_fib.fib_FileName

extern unsigned short disk_not_mounted;         /* flag set by opendir() */

DIR *opendir(const char *);
void closedir(DIR *);
void close_leftover_open_dirs(void);    /* call this if aborted in mid-run */
struct dirent *readdir(DIR *);
int umask(void);

#ifdef AZTEC_C
int rmdir(const char *);
int chmod(const char *filename, int bits);
#endif

#endif /* __amiga_z_stat_h */
