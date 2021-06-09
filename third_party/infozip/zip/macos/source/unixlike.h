/*
  Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 1999-Oct-05 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, both of these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
*/
/*
 *      Directory Operations for Mac based on BSD 4.3   <macdir.h>
 *      By Jason Linhart, January 1997
 */

#ifndef _UNIXLIKE_H
#define _UNIXLIKE_H       1

#include <stat.h>

#ifndef NAME_MAX
#define NAME_MAX    2048
#endif

#define UNX_IFMT        0170000     /* Unix file type mask */
#define UNX_IFSOCK      0140000     /* Unix socket (BSD, not SysV or Amiga) */
#define UNX_IFLNK       0120000     /* Unix symbolic link (not SysV, Amiga) */
#define UNX_IFREG       0100000     /* Unix regular file */
#define UNX_IFBLK       0060000     /* Unix block special       (not Amiga) */
#define UNX_IFDIR       0040000     /* Unix directory */
#define UNX_IFCHR       0020000     /* Unix character special   (not Amiga) */
#define UNX_IFIFO       0010000     /* Unix fifo    (BCC, not MSC or Amiga) */

#define UNX_ISUID       04000       /* Unix set user id on execution */
#define UNX_ISGID       02000       /* Unix set group id on execution */
#define UNX_ISVTX       01000       /* Unix directory permissions control */
#define UNX_ENFMT       UNX_ISGID   /* Unix record locking enforcement flag */

#define UNX_IRWXU       00700       /* Unix read, write, execute: owner */
#define UNX_IRUSR       00400       /* Unix read permission: owner */
#define UNX_IWUSR       00200       /* Unix write permission: owner */
#define UNX_IXUSR       00100       /* Unix execute permission: owner */

#define UNX_IRWXG       00070       /* Unix read, write, execute: group */
#define UNX_IRGRP       00040       /* Unix read permission: group */
#define UNX_IWGRP       00020       /* Unix write permission: group */
#define UNX_IXGRP       00010       /* Unix execute permission: group */

#define UNX_IRWXO       00007       /* Unix read, write, execute: other */
#define UNX_IROTH       00004       /* Unix read permission: other */
#define UNX_IWOTH       00002       /* Unix write permission: other */
#define UNX_IXOTH       00001       /* Unix execute permission: other */

/* historical file modes */
#define S_IREAD    0x100
#define S_IWRITE   0x80
#define S_IEXEC    0x40


#define isatty(arg) 1


#define EINVAL          22      /* Invalid argument */
#define ENAMETOOLONG    63      /* File name too long */


struct dirent {
        char            d_name[NAME_MAX];
};

/*
 * The following definitions are usually found in fcntl.h.
 * However, MetroWerks has screwed that file up a couple of times
 * and all we need are the defines.
 */
#define O_APPEND  0x0100    /* open the file in append mode */
#define O_CREAT   0x0200    /* create the file if it doesn't exist */
#define O_EXCL    0x0400    /* if the file exists don't create it again */
#define O_TRUNC   0x0800    /* truncate the file after opening it */


int Zmacstat (const char *path, struct stat *buf);
int chmod(char *path, int mode);


#include "macstuff.h"

#endif /* _UNIXLIKE_H  */
