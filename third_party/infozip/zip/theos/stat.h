#ifndef __theos_stat_h
#define __theos_stat_h
/*
  Copyright (c) 1990-2000 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/

/* extended stat structure for stat, fstat, chmod */
/* Written by Jean-Michel Dubois */

#pragma field word
struct stat {
    unsigned short  st_mode;        /* file attributes */
        #define S_IFMT      0xf000      /* file type mask */
        #define S_IFIFO     0x1000      /* pipe */
        #define S_IFCHR     0x2000      /* char device */
        #define S_IFSOCK    0x3000      /* socket */
        #define S_IFDIR     0x4000      /* directory */
        #define S_IFLIB     0x5000      /* library */
        #define S_IFBLK     0x6000      /* block device */
        #define S_IFREG     0x8000      /* regular file */
        #define S_IFREL     0x9000      /* relative (direct) */
        #define S_IFKEY     0xA000      /* keyed */
        #define S_IFIND     0xB000      /* indexed */
        #define S_IFRND     0xC000      /* ???? */
        #define S_IFR16     0xD000      /* 16 bit real mode program */
        #define S_IFP16     0xE000      /* 16 bit protected mode prog */
        #define S_IFP32     0xF000      /* 32 bit protected mode prog */

        #define S_ISUID     0x0800      /* meaningless */
        #define S_ISGID     0x0400      /* meaningless */
        #define S_ISVTX     0x0200      /* meaningless */

        #define S_IMODF     0x0800      /* modified */
        #define S_INHID     0x0400      /* not hidden */

        #define S_IRWXU     0x03c0      /* read,write,execute: owner */
        #define S_IEUSR     0x0200      /* erase permission: owner */
        #define S_IRUSR     0x0100      /* read permission: owner */
        #define S_IWUSR     0x0080      /* write permission: owner */
        #define S_IXUSR     0x0040      /* execute permission: owner */
    /* group permissions */
        #define S_IRWXG     0x0238
        #define S_IEGRP     0x0200
        #define S_IRGRP     0x0020
        #define S_IWGRP     0x0010
        #define S_IXGRP     0x0008
    /* other never has erase permission */
        #define S_IRWXO     0x0207      /* read,write,execute: other */
        #define S_IROTH     0x0004      /* read permission: other */
        #define S_IWOTH     0x0002      /* write permission: other */
        #define S_IXOTH     0x0001      /* execute permission: other */

        #define S_IREAD     0x0100      /* read permission, owner */
        #define S_IEXEC     0x0040      /* execute permission, owner */
        #define S_IWRITE    0x0080      /* write permission, owner */
    short           st_ino;         /* not used */
    short           st_dev;         /* not used */
    short           st_rdev;        /* not used */
    short           st_nlink;       /* not used */
    short           st_uid;         /* owner id */
    short           st_gid;         /* not used */
    unsigned long   st_size;        /* size of file */
    unsigned long   st_atime;       /* not used */
    unsigned long   st_mtime;       /* date & time last modified */
    unsigned long   st_ctime;       /* not used */
    unsigned long   st_blksize;     /* buffer size */
    unsigned short  st_org;         /* organization */
    unsigned short  st_rlen;        /* record size */
    unsigned short  st_klen;        /* key size */
    char            st_grow;        /* growing factor */
    char            st_protect;     /* native protections */
};
#pragma field

#define S_ISREG(m)      (((m) & S_IFMT) >= S_IFREG)
#define S_ISLIB(m)      (((m) & S_IFMT) == S_IFLIB)
#define S_ISDIR(m)      (((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)      (((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m)      (((m) & S_IFMT) == S_IFBLK)

#define S_ISSEQ(m)      (((m) & S_IFMT) == S_IFREG)
#define S_ISREL(m)      (((m) & S_IFMT) == S_IFREL)
#define S_ISKEY(m)      (((m) & S_IFMT) == S_IFKEY)
#define S_ISIND(m)      (((m) & S_IFMT) == S_IFIND)
#define S_ISPRG(m)      (((m) & S_IFMT) >= S_IFP16)
#define S_ISLNK(m)      0

/* avoid conflict with original THEOS functions */

#define stat(a,b)       _stat(a,b)
#define fstat(a,b)      _fstat(a,b)
#define chmod(a,b)      _chmod(a,b)

extern int _stat(const char *file, struct stat *statptr);
extern int _fstat(int fd, struct stat *statptr);
extern int _chmod(const char *file, short mask);

#define _chstat(a,b)    ((int) _sc_168(a,'p',(size_t)(b)))
extern unsigned short   _sc_168(const char _far *fn, int cmd, size_t value);

#endif /* !__theos_stat_h */
