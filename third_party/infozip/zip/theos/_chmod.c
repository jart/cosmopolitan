/*
  Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 1999-Oct-05 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, both of these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
*/
/* Change UNIX modes */

#pragma library

#include <sc.h>

int _chmod(const char *fname, short mask)
{
    extern char _um2tm_(short);

    return _filechange(fname,'p',(size_t) _um2tm_(mask)|0x80);
}

