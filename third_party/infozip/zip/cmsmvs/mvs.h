/*
  Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 1999-Oct-05 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, both of these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
*/
/* <dirent.h> definitions */

#define NAMELEN     8

struct dirent {
   struct dirent *d_next;
   char   d_name[NAMELEN+1];
};

typedef struct _DIR {
   struct  dirent *D_list;
   struct  dirent *D_curpos;
   char            D_path[FILENAME_MAX];
} DIR;

DIR *          opendir(const char *dirname);
struct dirent *readdir(DIR *dirp);
void           rewinddir(DIR *dirp);
int            closedir(DIR *dirp);
char *         readd(DIR *dirp);

#define ALIAS_MASK  (unsigned int) 0x80
#define SKIP_MASK   (unsigned int) 0x1F
#define TTRLEN      3
#define RECLEN      254

typedef _Packed struct {
   unsigned short int count;
   char rest[RECLEN];
} RECORD;

char    *endmark = "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";
