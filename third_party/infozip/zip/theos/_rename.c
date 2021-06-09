/*
  Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 1999-Oct-05 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, both of these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define EXDEV 590

#define _sys_rename()   _sc_140()
extern unsigned short   _sys_rename(const char _far *oldfn, char *newfn);

/* rename a file. Report an error on cross disk renames */

static void _n_(const char* fn, char* bfn)
{
    if (*fn != '.' && *fn != '/')
        strcpy(bfn, "./");
    else
        *bfn = '\0';
    strcat(bfn, fn);
}

int _rename(const char* old, const char* new)
{
    char* p;
    char* q;
    char* r;
    char olddrv, newdrv;
    char dir[FILENAME_MAX];
    short status;
    char bold[FILENAME_MAX], bnew[FILENAME_MAX];

    p = strrchr(old, ':');
    q = strrchr(new, ':');

    /* if at least one path includes a disk name, check for equality */
    if (p != NULL || q != NULL) {
        /* getcwd return a NULL pointer for /:S */
        getcwd(dir, FILENAME_MAX);
        r = strrchr(dir, ':');

        if (p == NULL)
            p = r;
        olddrv = p ? p[1] : 'S';

        if (q == NULL)
            q = r;
        newdrv = q ? q[1] : 'S';

        /* return an error if uppercased disk names are not the same */
        if ((old & ~0x20) != (new & ~0x20)) {
            _errarg = NULL;
            return errno = _errnum = EXDEV;
        }
    }
    /* prepend ./ if there is no path to force rename to work on files
     * in the current directory instead of default library
     */
    _n_(old, bold);
    _n_(new, bnew);

    status = _sys_rename(bold, bnew);
    /* can be :
     *  0 no error
     * 19 "old" file not found
     * 44 "new" file already exist
     * 45 "new" filename missing
     * 46 "old" file name missing
     */
    if (status) {
        errno = _errnum = status;
        _errarg = (status == 44 || status == 45) ? new : old;
    }

    return status;
}
