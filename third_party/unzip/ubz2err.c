/*
  Copyright (c) 1990-2008 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2007-Mar-04 or later
  (the contents of which are also included in unzip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*---------------------------------------------------------------------------

  ubz2err.c

  This file contains the "fatal error" callback routine required by the
  "minimal" (silent, non-stdio) setup of the bzip2 compression library.

  The fatal bzip2 error bail-out routine is provided in a separate code
  module, so that it can be easily overridden when the UnZip package is
  used as a static link library. One example is the WinDLL static library
  usage for building a monolythic binary of the Windows application "WiZ"
  that supports bzip2 both in compression and decompression operations.

  Contains:  bz_internal_error()      (USE_BZIP2 only)

  ---------------------------------------------------------------------------*/


#define __UBZ2ERR_C     /* identifies this source module */
#define UNZIP_INTERNAL
#include "third_party/unzip/unzip.h"
#include "third_party/unzip/globals.h"

#ifdef USE_BZIP2

/**********************************/
/*  Function bz_internal_error()  */
/**********************************/

/* Call-back function for the bzip2 decompression code (compiled with
 * BZ_NO_STDIO), required to handle fatal internal bug-type errors of
 * the bzip2 library.
 */
void bz_internal_error(bzerrcode)
    int bzerrcode;
{
    GETGLOBALS();

    Info(slide, 0x421, ((char *)slide,
      "error: internal fatal libbzip2 error number %d\n", bzerrcode));
#ifdef WINDLL
    longjmp(dll_error_return, 1);
#else
    DESTROYGLOBALS();
    EXIT(PK_BADERR);
#endif
} /* end function bz_internal_error() */

#endif /* USE_BZIP2 */
