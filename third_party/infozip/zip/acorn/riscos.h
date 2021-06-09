/*
  Copyright (c) 1990-2002 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/* riscos.h */

#ifndef __riscos_h
#define __riscos_h

#include <time.h>
#include <stdio.h>

typedef struct {
  int errnum;
  char errmess[252];
} os_error;

#ifndef __swiven_h
#  include "swiven.h"
#endif

#define MAXPATHLEN 256
#define MAXFILENAMELEN 64  /* should be 11 for ADFS, 13 for DOS, 64 seems a sensible value... */
#define DIR_BUFSIZE 1024   /* this should be enough to read a whole E-Format directory */

struct stat {
  unsigned int st_dev;
  int st_ino;
  unsigned int st_mode;
  int st_nlink;
  unsigned short st_uid;
  unsigned short st_gid;
  unsigned int st_rdev;
  unsigned int st_size;
  unsigned int st_blksize;
  time_t st_atime;
  time_t st_mtime;
  time_t st_ctime;
};

typedef struct {
  char *dirname;
  void *buf;
  int size;
  char *act;
  int offset;
  int read;
} DIR;

#define dstrm DIR

struct dirent {
  unsigned int d_off;          /* offset of next disk directory entry */
  int d_fileno;                /* file number of entry */
  size_t d_reclen;             /* length of this record */
  size_t d_namlen;             /* length of d_name */
  char d_name[MAXFILENAMELEN]; /* name */
};

typedef struct {
  unsigned int load_addr;
  unsigned int exec_addr;
  int lenght;
  int attrib;
  int objtype;
  char name[13];
} riscos_direntry;

#define SPARKID   0x4341        /* = "AC" */
#define SPARKID_2 0x30435241    /* = "ARC0" */

typedef struct {
  short         ID;
  short         size;
  int           ID_2;
  unsigned int  loadaddr;
  unsigned int  execaddr;
  int           attr;
  int           zero;
} extra_block;


#define S_IFMT  0770000

#define S_IFDIR 0040000
#define S_IFREG 0100000  /* 0200000 in UnixLib !?!?!?!? */

#ifndef S_IEXEC
#  define S_IEXEC  0000100
#  define S_IWRITE 0000200
#  define S_IREAD  0000400
#endif

extern char *exts2swap; /* Extensions to swap */

int stat(char *filename,struct stat *res);
DIR *opendir(char *dirname);
struct dirent *readdir(DIR *d);
char *readd(DIR *d);
void closedir(DIR *d);
int unlink(char *f);
int chmod(char *file, int mode);
void setfiletype(char *fname,int ftype);
void getRISCOSexts(char *envstr);
int checkext(char *suff);
int swapext(char *name, char *exptr);
void remove_prefix(void);
void set_prefix(void);
struct tm *riscos_localtime(const time_t *timer);
struct tm *riscos_gmtime(const time_t *timer);

int riscos_fseek(FILE *fd, long offset, int whence);
/* work around broken assumption that fseek() is OK with -ve file offsets */

#endif /* !__riscos_h */
