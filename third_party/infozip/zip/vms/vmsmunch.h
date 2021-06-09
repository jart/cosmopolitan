/*
  Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 1999-Oct-05 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, both of these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
*/
/*---------------------------------------------------------------------------

  vmsmunch.h

  A few handy #defines, plus the contents of three header files from Joe
  Meadows' FILE program.  Used by VMSmunch and by various routines which
  call VMSmunch (e.g., in Zip and UnZip).

        02-Apr-1994     Jamie Hanrahan  jeh@cmkrnl.com
                        Moved definition of VMStimbuf struct from vmsmunch.c
                        to here.

        06-Apr-1994     Jamie Hanrahan  jeh@cmkrnl.com
                        Moved "contents of three header files" (not needed by
                        callers of vmsmunch) to VMSdefs.h .

        07-Apr-1994     Richard Levitte levitte@e.kth.se
                        Inserted a forward declaration of VMSmunch.

        17-Sep-1995     Chr. Spieler    spieler@linac.ikp.physik.th-darmstadt.de
                        Added wrapper to prevent multiple loading of this file.

        10-Oct-1995     Chr. Spieler    spieler@linac.ikp.physik.th-darmstadt.de
                        Use lowercase names for all VMS specific source files

        15-Dec-1995     Chr. Spieler    spieler@linac.ikp.physik.th-darmstadt.de
                        Removed ALL "tabs" from source file.

  ---------------------------------------------------------------------------*/

#ifndef __vmsmunch_h
#define __vmsmunch_h 1

#define GET_TIMES       4
#define SET_TIMES       0
#define GET_RTYPE       1
#define CHANGE_RTYPE    2
#define RESTORE_RTYPE   3

struct VMStimbuf {      /* VMSmunch */
    char *actime;       /* VMS revision date, ASCII format */
    char *modtime;      /* VMS creation date, ASCII format */
};

extern int VMSmunch(char *filename, int action, char *ptr);

#endif /* !__vmsmunch_h */
