/*
  Copyright (c) 1990-2000 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/* replace standard library function who needs a FILE* */

#pragma library

#include <stdio.h>
#include <fcntl.h>
#include <sc.h>
#include <lub.h>

short _isatty(int fd)
{
    register short lub;

    lub = (int) _fcntl(&stdin[fd], 5, (size_t) 0);
    return (lub >= CONIN && lub <= CONOUT)
        || (lub >= COM1 && lub <= COM4)
        || (lub >= COM5 && lub <= COM16);
}
