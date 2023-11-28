/*
  Copyright (c) 1990-2000 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in unzip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/* inflate.h for UnZip -- by Mark Adler
   version c14f, 23 November 1995 */


/* Copyright history:
   - Starting with UnZip 5.41 of 16-April-2000, this source file
     is covered by the Info-Zip LICENSE cited above.
   - Prior versions of this source file, found in UnZip source packages
     up to UnZip 5.40, were put in the public domain.
     The original copyright note by Mark Adler was:
         "You can do whatever you like with this source file,
         though I would prefer that if you modify it and
         redistribute it that you include comments to that effect
         with your name and the date.  Thank you."

   History:
   vers    date          who           what
   ----  ---------  --------------  ------------------------------------
    c14  12 Mar 93  M. Adler        made inflate.c standalone with the
                                    introduction of inflate.h.
    c14d 28 Aug 93  G. Roelofs      replaced flush/FlushOutput with new version
    c14e 29 Sep 93  G. Roelofs      moved everything into unzip.h; added crypt.h
    c14f 23 Nov 95  G. Roelofs      added UNZIP_INTERNAL to accommodate newly
                                    split unzip.h
 */

#define UNZIP_INTERNAL
#include "third_party/unzip/unzip.h"     /* provides slide[], typedefs and macros */
#ifdef FUNZIP
#include "third_party/unzip/crypt.h"   /* provides NEXTBYTE macro for crypt version of funzip */
#endif
