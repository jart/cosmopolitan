/*
  Copyright (c) 1990-2002 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*
 *  Header declaration(s) which are forced to go after earlier includes
 */

#ifndef __tannsk_h   /* prevent multiple inclusions */
#define __tannsk_h

/* ztimbuf is declared in zip\tailor.h after include of tandem.h */
int utime (const char *, const ztimbuf *);

#endif /* !__tannsk_h */
