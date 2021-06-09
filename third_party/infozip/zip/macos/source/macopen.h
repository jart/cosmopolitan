/*
  Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 1999-Oct-05 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, both of these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
*/
#ifndef __MACOPEN_H__
#define __MACOPEN_H__

#include <stdio.h>
#include <Files.h>


FILE *MacFopen(const char *path, const char *mode);
int MacOpen(const char *path, int oflag, ...);

int my_open(char *path, int oflag);

#endif /* __MACOPEN_H__  */
