/*
  windll/structs.h - Zip 3

  Copyright (c) 1990-2003 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2003-May-08 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
#ifndef _ZIP_STRUCTS_H
#define _ZIP_STRUCTS_H

#ifndef Far
#  define Far far
#endif

/* Porting definations between Win 3.1x and Win32 */
#ifdef WIN32
#  define far
#  define _far
#  define __far
#  define near
#  define _near
#  define __near
#endif

#include "../api.h"

#endif /* _ZIP_STRUCTS_H */
