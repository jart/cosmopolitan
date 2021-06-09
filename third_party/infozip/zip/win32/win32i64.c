/*
  win32/win32i64.c - Zip 3

  Copyright (c) 1990-2007 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2005-Feb-10 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/

#include "../zip.h"

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>
#include <windows.h>
/* for LARGE_FILE_SUPPORT but may not be needed */
#include <io.h>


/* --------------------------------------------------- */
/* Large File Support
 *
 * Initial functions by E. Gordon and R. Nausedat
 * 9/10/2003
 *
 * These implement 64-bit file support for Windows.  The
 * defines and headers are in win32/osdep.h.
 *
 * These moved from win32.c by Mike White to avoid conflicts
 * in WiZ of same name functions in UnZip and Zip libraries.
 * 9/25/04 EG
 */

#if defined(LARGE_FILE_SUPPORT) && !defined(__CYGWIN__)

/* 64-bit buffered ftello
 *
 * Win32 does not provide a 64-bit buffered
 * ftell (in the published api anyway) so below provides
 * hopefully close version.
 * We have not gotten _telli64 to work with buffered
 * streams.  Below cheats by using fgetpos improperly and
 * may not work on other ports.
 */

zoff_t zftello(stream)
  FILE *stream;
{
  fpos_t fpos = 0;
  
  if (fgetpos(stream, &fpos) != 0) {
    return -1L;
  } else {
    return fpos;
  }
}


/* 64-bit buffered fseeko
 *
 * Win32 does not provide a 64-bit buffered
 * fseeko so use _lseeki64 and fflush.  Note
 * that SEEK_CUR can lose track of location
 * if fflush is done between the last buffered
 * io and this call.
 */

int zfseeko(stream, offset, origin)
  FILE *stream;
  zoff_t offset;
  int origin;
{
  zoff_t location;

  location = zftello(stream);
  fflush(stream);
  if (origin == SEEK_CUR) {
    /* instead of synching up lseek easier just to figure and
       use an absolute offset */
    offset = location + offset;
    location = _lseeki64(fileno(stream), offset, SEEK_SET);
  } else {
    location = _lseeki64(fileno(stream), offset, origin);
  }
  if (location == -1L) {
    return -1L;
  } else {
    return 0;
  }
}
#endif  /* Win32 LARGE_FILE_SUPPORT */

#if 0
FILE* zfopen(filename,mode)
char *filename;
char *mode;
{
FILE* fTemp;
  
  fTemp = fopen(filename,mode);
  if( fTemp == NULL )
    return NULL;
  
  /* sorry, could not make VC60 and its rtl work properly without setting the file buffer to NULL. the  */
  /* problem seems to be _telli64 which seems to return the max stream position, comments are welcome   */
  setbuf(fTemp,NULL);

  return fTemp;
}
#endif
/* --------------------------------------------------- */
