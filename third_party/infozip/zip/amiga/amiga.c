/*
  Copyright (c) 1990-2005 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2005-Feb-10 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/* OS specific routines for AMIGA platform.
 *
 * John Bush    <John.Bush@east.sun.com>  BIX: jbush
 * Paul Kienitz <kie@pacbell.net>
 *
 * History:
 *
 * Date     DoBee    Comments
 * -------  -------- -----------------------------------------------
 * 21Jan93  JBush    Original coding.
 *                   Incorporated filedate.c (existing routine).
 *
 * 31Jan93  JBush    Made filedate.c include unconditional.
 *
 * 18Jul93  PaulK    Moved Aztec _abort() here from stat.c because we
 *                   can't share the same one between Zip and UnZip.
 *                   Added close_leftover_open_dirs() call to it.
 *
 * 17Apr95  PaulK    Added Amiga internal version string so that
 *                   installer programs can compare the version being
 *                   installed to see if the copy the user already has
 *                   is older or newer.  Added Prestart_Hook to support
 *                   debug tracing in deflate.a.
 *
 *  6May95  PaulK    Added GetComment() for filenote support.
 *
 * 12Nov95  PaulK    Added #define ZIP in front of filedate.c, for
 *                   new options in there; removed declare of set_con()
 *                   since echon() no longer expands to it (or anything).
 *
 * 12Feb96  PaulK    Removed call of echon() entirely.
 *
 * 12Jul97  PaulK    Made both Aztec and SAS define USE_TIME_LIB for filedate.c
 *
 * 26Aug97  PaulK    Added ClearIOErr_exit()
 *
 *  2Jan98  HWalt    Adapted for SAS/C using stat.c replacement functions
 *
 *  6Jun00  PaulK    Removed references to time_lib, since new filedate.c
 *                   supercedes it
 */

#include <exec/memory.h>
#ifdef AZTEC_C
#  include <libraries/dos.h>
#  include <libraries/dosextens.h>
#  include <clib/exec_protos.h>
#  include <clib/dos_protos.h>
#  include <pragmas/exec_lib.h>
#  include <pragmas/dos_lib.h>
#else
#  include <proto/exec.h>
#  include <proto/dos.h>
#endif
#include <stdlib.h>
#include "ziperr.h"
void ziperr(int c, const char *h);

#define ZIP
#if !defined(UTIL)
#  define NO_MKTIME
#endif

#ifdef AZTEC_C

/* ============================================================= */
/* filedate.c is an external file, since it's shared with UnZip. */
/* Aztec includes it here, but SAS/C now compiles it separately. */
#  include "amiga/filedate.c"

/* the same applies to stat.c                                    */
#  include "amiga/stat.c"

#  define setenv BOGUS_INCOMPATIBLE_setenv
#  include <fcntl.h>
#  undef setenv
#  ifdef DEBUG
#    define PRESTART_HOOK
#  endif
#endif

extern void close_leftover_open_dirs(void);


/* the following handles cleanup when a ^C interrupt happens: */

void _abort(void)               /* called when ^C is pressed */
{
    close_leftover_open_dirs();
    ziperr(ZE_ABORT, "^C");
}

void ClearIOErr_exit(int e)     /* EXIT is defined as this */
{
    if (!e)
        ((struct Process *) FindTask(NULL))->pr_Result2 = 0;
    /* we clear IoErr() since we are successful, in a 1.x-compatible way */
    exit(e);
}


/* Make sure the version number here matches the number in revision.h */
/* as closely as possible in strict decimal "#.#" form:               */
const char version_id[] = "\0$VER: Zip 2.3 ("
#  include "env:VersionDate"
")\r\n";

/* call this with an arg of NULL to free storage: */

char *GetComment(char *filename)
{
    BPTR lk;
    static struct FileInfoBlock *fib = NULL;

    if (!filename) {
        if (fib) FreeMem(fib, sizeof(*fib));
        fib = NULL;
        return NULL;
    }
    if (!fib) {
        if (!(fib = AllocMem(sizeof(*fib), MEMF_PUBLIC)))
            ziperr(ZE_MEM, "was checking filenotes");
    }
    if (!(lk = Lock(filename, ACCESS_READ)))
        return NULL;
    if (!Examine(lk, fib))
        fib->fib_Comment[0] = '\0';
    UnLock(lk);
    return fib->fib_Comment[0] ? &fib->fib_Comment[0] : NULL;
}
