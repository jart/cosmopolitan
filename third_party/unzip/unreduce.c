/*
  Copyright (c) 1990-2002 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in unzip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*---------------------------------------------------------------------------

  unreduce.c

  Copyright-clean dummy module, without any real code.

  If you really need unreduce support, replace this module by the full
  source code, available as add-on package from our distribution site.

  ---------------------------------------------------------------------------*/


#define __UNREDUCE_C    /* identifies this source module */
#define UNZIP_INTERNAL
#include "third_party/unzip/unzip.h"      /* defines COPYRIGHT_CLEAN by default */


#ifndef COPYRIGHT_CLEAN

 /* This line is indented to hide the #error directive from pure traditional
  * K&R C preprocessors. These do not recognize the #error directive, but
  * they also recognize only lines that start with a '#' in column 1 as
  * preprocessor directives.
  */
 #error This dummy-module does not support the unreduce method!

#endif /* !COPYRIGHT_CLEAN */
